using System.Security.Cryptography;
using System.Security.Cryptography.X509Certificates;
using ArduinoGatekeeperBackend.Mqtt.Models;
using ArduinoGatekeeperBackend.Services.Interfaces;
using ArduinoGatekeeperBackend.Websocket;
using Microsoft.AspNetCore.SignalR;
using Microsoft.EntityFrameworkCore;
using MQTTnet;
using Newtonsoft.Json;

namespace ArduinoGatekeeperBackend.Mqtt
{
    public class MqttService : IHostedService
    {
        private readonly IConfiguration _config;
        private readonly IServiceScopeFactory _scopeFactory;
        private readonly IHubContext<LogHub> _hubContext;
        private readonly ILogger<MqttService> _logger;
        private IMqttClient? _client;

        public MqttService(IConfiguration config, IServiceScopeFactory scopeFactory, IHubContext<LogHub> hubContext, ILogger<MqttService> logger)
        {
            _config = config ?? throw new ArgumentNullException(nameof(config));
            _scopeFactory = scopeFactory ?? throw new ArgumentNullException(nameof(scopeFactory));
            _hubContext = hubContext ?? throw new ArgumentNullException(nameof(hubContext));
            _logger = logger ?? throw new ArgumentNullException(nameof(logger));
        }
        
        public async Task StartAsync(CancellationToken cancellationToken)
        {
            try
            {
                _client = new MqttClientFactory().CreateMqttClient();

                var clientCert = X509CertificateLoader.LoadCertificateFromFile(_config.GetValue<string>("Ssl:ServerCert")!);
                using var clientKey = ECDsa.Create();
                clientKey.ImportFromPem(File.ReadAllText(_config.GetValue<string>("Ssl:ServerKey")!));

                _client.ApplicationMessageReceivedAsync += HandleIncomingMessageAsync;
                _client.ConnectedAsync += HandleConnectedAsync;
                _client.DisconnectedAsync += HandleDisconnectedAsync;
            
                var options = new MqttClientOptionsBuilder()
                    .WithTcpServer(_config.GetValue<string>("MqttBroker:Host"), _config.GetValue<int>("MqttBroker:Port", 8883))
                    .WithTlsOptions(tls => {
                        tls.UseTls(true);
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
            
                string topic = _config.GetValue<string>("MqttBroker:Topics:DeviceStatus")?.Trim() ?? throw new ArgumentNullException("DeviceStatus topic not found");
                await _client.SubscribeAsync(new MqttTopicFilterBuilder()
                    .WithTopic(topic).WithQualityOfServiceLevel(MQTTnet.Protocol.MqttQualityOfServiceLevel.ExactlyOnce)
                    .Build());
                _logger.LogInformation("Subscribed to topic '{Topic}'", topic);

                topic = _config.GetValue<string>("MqttBroker:Topics:Scan")?.Trim() ?? throw new ArgumentNullException("Scan topic not found");
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
                var deviceIdNum = int.Parse(deviceId.Replace(_config.GetValue<string>("MqttBroker:DeviceIdPrefix")!, string.Empty));

                await using var scope = _scopeFactory.CreateAsyncScope();
                var doorLogsService = scope.ServiceProvider.GetRequiredService<IDoorLogsService>();
                var result = await doorLogsService.CreateAsync(new DoorLogDTO {
                    DoorId = deviceIdNum,
                    Online = status.Online,
                    CreatedAt = (status.Timestamp is not null ? DateTimeOffset.FromUnixTimeSeconds(status.Timestamp ?? 0).UtcDateTime : null)
                });

                await _hubContext.Clients.All.SendAsync("NewStatusEntry", result);

                if (!status.Online) return;
                var addUserTopic = _config.GetValue<string>("MqttBroker:Topics:AddUser", string.Empty).Replace("+", deviceId).Trim();

                var permissionsService = scope.ServiceProvider.GetRequiredService<IPermissionsService>();
                var authUsers = permissionsService.GetAll().Include(it => it.User).Where(it => it.DoorId == deviceIdNum).Select(it => it.User).Select(usr => new { usr.CardId, usr.CardKey });
                await authUsers.ForEachAsync(async usr => {
                    await _client.PublishStringAsync(
                        addUserTopic,
                        JsonConvert.SerializeObject(new AuthorizedUser { Uid = usr.CardId, Key = usr.CardKey.Select(it => Convert.ToInt32(it)) }),
                        MQTTnet.Protocol.MqttQualityOfServiceLevel.ExactlyOnce
                    );
                });
            }
            catch (Exception ex) { _logger.LogError(ex.InnerException?.Message ?? ex.Message); }
        }

        private async Task HandleScanMessageAsync(string? deviceId, string payload)
        {
            try
            {
                if (string.IsNullOrWhiteSpace(deviceId)) return;
                var scan = JsonConvert.DeserializeObject<Scan>(payload);
                if (scan is null) return;

                await using var scope = _scopeFactory.CreateAsyncScope();
                var accessLogsService = scope.ServiceProvider.GetRequiredService<IAccessLogsService>();
                var result = await accessLogsService.CreateAsync(new AccessLogDTO {
                    CardId = (scan.CardId ?? string.Empty),
                    DoorId = int.Parse(deviceId.Replace(_config.GetValue<string>("MqttBroker:DeviceIdPrefix")!, string.Empty)),
                    Granted = scan.Granted,
                    CreatedAt = (scan.Timestamp is not null ? DateTimeOffset.FromUnixTimeSeconds(scan.Timestamp ?? 0).UtcDateTime : null)
                });

                await _hubContext.Clients.All.SendAsync("NewLogEntry", result);
            }
            catch (Exception ex) { _logger.LogError(ex.InnerException?.Message ?? ex.Message); }
        }
    }
}