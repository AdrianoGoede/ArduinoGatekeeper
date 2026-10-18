using System.Security.Cryptography;
using System.Security.Cryptography.X509Certificates;
using ArduinoGatekeeperBackend.EntityFramework.Models;
using ArduinoGatekeeperBackend.Events;
using ArduinoGatekeeperBackend.Mqtt.Models;
using MQTTnet;
using Newtonsoft.Json;

namespace ArduinoGatekeeperBackend.Mqtt
{
    public class MqttService : IHostedService
    {
        private readonly IConfiguration _config;
        private readonly IEventBus _eventBus;
        private readonly ILogger<MqttService> _logger;
        private IMqttClient? _client;

        public MqttService(IConfiguration config, IEventBus eventBus, ILogger<MqttService> logger)
        {
            _config = config ?? throw new ArgumentNullException(nameof(config));
            _eventBus = eventBus ?? throw new ArgumentNullException(nameof(eventBus));
            _logger = logger ?? throw new ArgumentNullException(nameof(logger));

            _eventBus.Subscribe<UserCreatedEvent>(async e => await HandleUserCreatedAsync(e));
            _eventBus.Subscribe<UserEditedEvent>(async e => await HandleUserEditedAsync(e));
            _eventBus.Subscribe<UserRemovedEvent>(async e => await HandleUserRemovedAsync(e));
            _eventBus.Subscribe<SyncUsersToDoorRequestedEvent>(async e => await HandleSyncUsersToDoorRequestAsync(e));
        }
        
        public async Task StartAsync(CancellationToken cancellationToken)
        {
            try
            {
                _client = new MqttClientFactory().CreateMqttClient();
                _client.ApplicationMessageReceivedAsync += HandleIncomingMessageAsync;
                _client.ConnectedAsync += HandleConnectedAsync;
                _client.DisconnectedAsync += HandleDisconnectedAsync;

                var clientCert = X509CertificateLoader.LoadCertificateFromFile(_config.GetValue<string>("Ssl:ServerCert")!);
                using var clientKey = ECDsa.Create();
                clientKey.ImportFromPem(File.ReadAllText(_config.GetValue<string>("Ssl:ServerKey")!));
            
                var options = new MqttClientOptionsBuilder()
                    .WithTcpServer(_config.GetValue<string>("MqttBroker:Host"), _config.GetValue<int>("MqttBroker:Port", 8883))
                    .WithTlsOptions(tls => {
                        tls.UseTls(true);
                        tls.WithSslProtocols(System.Security.Authentication.SslProtocols.Tls13);
                        tls.WithCertificateValidationHandler(ctx => {
                            ctx.Chain.ChainPolicy.TrustMode = X509ChainTrustMode.CustomRootTrust;
                            ctx.Chain.ChainPolicy.RevocationMode = X509RevocationMode.NoCheck;
                            ctx.Chain.ChainPolicy.CustomTrustStore.Clear();
                            ctx.Chain.ChainPolicy.CustomTrustStore.Add(new X509Certificate2(_config.GetValue<string>("Ssl:CaCert")));
                            ctx.Chain.ChainPolicy.VerificationFlags = X509VerificationFlags.AllowUnknownCertificateAuthority;
                            return ctx.Chain.Build((X509Certificate2)ctx.Certificate);
                        });
                        tls.WithClientCertificates(new X509Certificate2Collection {
                            clientCert.CopyWithPrivateKey(clientKey)
                        });
                    }).WithClientId(_config.GetValue<string>("MqttBroker:ClientId")).Build();

                await _client.ConnectAsync(options, cancellationToken);
            }
            catch (Exception ex)
            {
                _logger.LogCritical(ex.InnerException?.Message ?? ex.Message);
                throw;
            }
        }

        public async Task StopAsync(CancellationToken cancellationToken)
        {
            if (_client?.IsConnected ?? false)
                await _client.DisconnectAsync(cancellationToken: cancellationToken);
            _client?.Dispose();
        }

        private async Task HandleConnectedAsync(MqttClientConnectedEventArgs e)
        {
            try
            {
                _logger.LogInformation("Connected successfuly");
            
                string topic = _config.GetValue<string>("MqttBroker:Topics:DeviceStatus")?.Trim() ?? throw new ArgumentNullException("DeviceStatus topic not informed");
                await _client.SubscribeAsync(new MqttTopicFilterBuilder()
                    .WithTopic(topic).WithQualityOfServiceLevel(MQTTnet.Protocol.MqttQualityOfServiceLevel.ExactlyOnce)
                    .Build());
                _logger.LogInformation("Subscribed to topic '{Topic}'", topic);

                topic = _config.GetValue<string>("MqttBroker:Topics:Scan")?.Trim() ?? throw new ArgumentNullException("Scan topic not informed");
                await _client.SubscribeAsync(new MqttTopicFilterBuilder()
                    .WithTopic(topic).WithQualityOfServiceLevel(MQTTnet.Protocol.MqttQualityOfServiceLevel.ExactlyOnce)
                    .Build());
                _logger.LogInformation("Subscribed to topic '{Topic}'", topic);
            }
            catch (Exception ex)
            {
                _logger.LogCritical(ex.InnerException?.Message ?? ex.Message);
                throw;
            }
        }

        private async Task HandleDisconnectedAsync(MqttClientDisconnectedEventArgs e)
        {
            _logger.LogError("Connection lost! Attempting to reconnect...");
            var delay = _config.GetValue<int>("MqttBroker:ReconnectionDelay", 5);
            Console.WriteLine($"MQTT disconnected, reconnecting in {delay}s...");
            await Task.Delay(TimeSpan.FromSeconds(delay));
            await _client!.ReconnectAsync();
        }

        private async Task HandleIncomingMessageAsync(MqttApplicationMessageReceivedEventArgs e)
        {
            try
            {
                var payload = e.ApplicationMessage.ConvertPayloadToString();
                if (string.IsNullOrWhiteSpace(payload))
                    throw new ArgumentException($"Received empty message on topic {e.ApplicationMessage.Topic.Trim()}");

                var deviceIdPrefix = _config.GetValue<string?>("MqttBroker:DeviceIdPrefix");
                var deviceId = e.ApplicationMessage.Topic.Split('/').FirstOrDefault(it => it.Trim().StartsWith(deviceIdPrefix))?.Trim();

                if (e.ApplicationMessage.Topic.EndsWith("/dev_status"))
                    await HandleDeviceStatusMessageAsync(deviceId, payload);
                else if (e.ApplicationMessage.Topic.EndsWith("/scan"))
                    await HandleScanMessageAsync(deviceId, payload);
            }
            catch (Exception ex) { _logger.LogError(ex.InnerException?.Message ?? ex.Message); }
        }

        private async Task HandleDeviceStatusMessageAsync(string? deviceId, string payload)
        {
            try
            {
                if (string.IsNullOrWhiteSpace(deviceId)) return;
                var status = JsonConvert.DeserializeObject<DeviceStatus>(payload);
                if (status is null) 
                    throw new JsonException($"Malformed JSON received from '{deviceId}'");
                _eventBus.Publish(new DoorStatusReceivedEvent(deviceId, status));
            }
            catch (Exception ex) { _logger.LogError(ex.InnerException?.Message ?? ex.Message); }
        }

        private async Task HandleScanMessageAsync(string? deviceId, string payload)
        {
            try
            {
                if (string.IsNullOrWhiteSpace(deviceId)) return;
                var scan = JsonConvert.DeserializeObject<Scan>(payload);
                if (scan is null)
                    throw new JsonException($"Malformed JSON received from '{deviceId}'");
                _eventBus.Publish(new DoorScanReceivedEvent(deviceId, scan));
            }
            catch (Exception ex) { _logger.LogError(ex.InnerException?.Message ?? ex.Message); }
        }

        private async Task HandleUserCreatedAsync(UserCreatedEvent e)
        {
            foreach (var permission in e.User.Permissions)
                await AddOrUpdateUsersToDoorAsync([e.User], permission.DoorId);
        }

        private async Task HandleUserEditedAsync(UserEditedEvent e)
        {
            if (_client is null || !_client.IsConnected) return;
            
            var newPermissions = e.User.Permissions.Select(it => it.DoorId);
            var doorsToRemoveFrom = ((e.User.CardId == e.OldCardId) ? e.OldPermissions.Except(newPermissions) : e.OldPermissions).Distinct();
            foreach (var doorId in doorsToRemoveFrom)
                await RemoveUsersFromDoorAsync([e.OldCardId], doorId);

            foreach (var doorId in newPermissions)
                await AddOrUpdateUsersToDoorAsync([e.User], doorId);
        }

        private async Task HandleUserRemovedAsync(UserRemovedEvent e)
        {
            if (_client is null || !_client.IsConnected) return;
            foreach (var permission in e.User.Permissions)
                await RemoveUsersFromDoorAsync([e.User], permission.DoorId);
        }

        private async Task HandleSyncUsersToDoorRequestAsync(SyncUsersToDoorRequestedEvent e) => await AddOrUpdateUsersToDoorAsync(e.Users, e.DoorId);

        private async Task AddOrUpdateUsersToDoorAsync(IEnumerable<User> users, int doorId)
        {
            if (_client is null || !_client.IsConnected) return;
            var topic = _config.GetValue<string>("MqttBroker:Topics:AddUsers")
                               .Replace("+", $"{_config.GetValue<string>("MqttBroker:DeviceIdPrefix")}{doorId}");
            var pageSize = _config.GetValue<int?>("MqttBroker:UsersSyncPageSize") ?? throw new ArgumentNullException("UsersSyncPageSize not informed");

            foreach (var page in users.Chunk(pageSize))
            {
                var payload = JsonConvert.SerializeObject(page.Select(user => new AuthorizedUser { Uid = user.CardId, Key = user.CardKey.Select(it => Convert.ToInt32(it)) }));
                await _client.PublishStringAsync(topic, payload, MQTTnet.Protocol.MqttQualityOfServiceLevel.ExactlyOnce);
            }
        }

        private async Task RemoveUsersFromDoorAsync(IEnumerable<User> users, int doorId) => await RemoveUsersFromDoorAsync(users.Select(it => it.CardId), doorId);

        private async Task RemoveUsersFromDoorAsync(IEnumerable<string> cardIds, int doorId)
        {
            if (_client is null || !_client.IsConnected) return;
            var topic = _config.GetValue<string>("MqttBroker:Topics:RemoveUsers")
                               .Replace("+", $"{_config.GetValue<string>("MqttBroker:DeviceIdPrefix")}{doorId}");
            var pageSize = _config.GetValue<int?>("MqttBroker:UsersSyncPageSize") ?? throw new ArgumentNullException("UsersSyncPageSize not informed");

            foreach (var page in cardIds.Chunk(pageSize))
            {
                var payload = JsonConvert.SerializeObject(page.Select(cardId => new { Uid = cardId }));
                await _client.PublishStringAsync(topic, payload, MQTTnet.Protocol.MqttQualityOfServiceLevel.ExactlyOnce);
            }
        }
    }
}