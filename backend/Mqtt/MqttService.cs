using System.Security.Cryptography;
using System.Security.Cryptography.X509Certificates;
using ArduinoGatekeeperBackend.Mqtt.Models;
using ArduinoGatekeeperBackend.Services.Interfaces;
using ArduinoGatekeeperBackend.Websocket;
using Microsoft.AspNetCore.SignalR;
using MQTTnet;
using Newtonsoft.Json;

namespace ArduinoGatekeeperBackend.Mqtt
{
    public class MqttService : IHostedService
    {
        private readonly IConfiguration _config;
        private readonly IServiceScopeFactory _scopeFactory;
        private readonly IHubContext<LogHub> _hubContext;
        private IMqttClient? _client;

        public MqttService(IConfiguration config, IServiceScopeFactory scopeFactory, IHubContext<LogHub> hubContext)
        {
            _config = config ?? throw new ArgumentNullException(nameof(config));
            _scopeFactory = scopeFactory ?? throw new ArgumentNullException(nameof(scopeFactory));
            _hubContext = hubContext ?? throw new ArgumentNullException(nameof(hubContext));
        }
        
        public async Task StartAsync(CancellationToken cancellationToken)
        {
            _client = new MqttClientFactory().CreateMqttClient();

            var clientCert = X509CertificateLoader.LoadCertificateFromFile(_config.GetValue<string>("Ssl:ServerCert")!);
            using var clientKey = ECDsa.Create();
            clientKey.ImportFromPem(File.ReadAllText(_config.GetValue<string>("Ssl:ServerKey")!));

            _client.ApplicationMessageReceivedAsync += handleIncomingMessageAsync;
            _client.DisconnectedAsync += handleDesconnectAsync;
            
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
            await _client.SubscribeAsync(new MqttTopicFilterBuilder()
                .WithTopic(_config.GetValue<string>("MqttBroker:Topics:DeviceStatus")).WithQualityOfServiceLevel(MQTTnet.Protocol.MqttQualityOfServiceLevel.ExactlyOnce)
                .Build());
            await _client.SubscribeAsync(new MqttTopicFilterBuilder()
                .WithTopic(_config.GetValue<string>("MqttBroker:Topics:Scan")).WithQualityOfServiceLevel(MQTTnet.Protocol.MqttQualityOfServiceLevel.ExactlyOnce)
                .Build());
        }

        public async Task StopAsync(CancellationToken cancellationToken)
        {
            if (_client?.IsConnected ?? false)
                await _client.DisconnectAsync(cancellationToken: cancellationToken);
            _client?.Dispose();
        }

        private async Task handleIncomingMessageAsync(MqttApplicationMessageReceivedEventArgs e)
        {
            var payload = e.ApplicationMessage.ConvertPayloadToString();
            if (string.IsNullOrWhiteSpace(payload)) return;

            if (e.ApplicationMessage.Topic.EndsWith("/dev_status"))
                await handleDeviceStatusMessageAsync(payload);
            else if (e.ApplicationMessage.Topic.EndsWith("/scan"))
                await handleScanMessageAsync(payload);
        }

        private async Task handleDeviceStatusMessageAsync(string payload)
        {
            var status = JsonConvert.DeserializeObject<DeviceStatus>(payload);
            if (status is null) return;

            await using var scope = _scopeFactory.CreateAsyncScope();
            var doorLogsService = scope.ServiceProvider.GetRequiredService<IDoorLogsService>();
            var result = await doorLogsService.CreateAsync(new DoorLogDTO {
                DoorId = int.Parse(status.DeviceId.Replace(_config.GetValue<string>("MqttBroker:DeviceIdPrefix")!, string.Empty)),
                Online = status.Online,
                CreatedAt = (status.Timestamp is not null ? DateTimeOffset.FromUnixTimeSeconds(status.Timestamp ?? 0).UtcDateTime : null)
            });

            await _hubContext.Clients.All.SendAsync("NewStatusEntry", result);
        }

        private async Task handleScanMessageAsync(string payload)
        {
            var scan = JsonConvert.DeserializeObject<Scan>(payload);
            if (scan is null) return;

            await using var scope = _scopeFactory.CreateAsyncScope();
            var accessLogsService = scope.ServiceProvider.GetRequiredService<IAccessLogsService>();
            var result = await accessLogsService.CreateAsync(new AccessLogDTO {
                CardId = (scan.CardId ?? string.Empty),
                DoorId = int.Parse(scan.DeviceId.Replace(_config.GetValue<string>("MqttBroker:DeviceIdPrefix")!, string.Empty)),
                Granted = scan.Granted,
                CreatedAt = (scan.Timestamp is not null ? DateTimeOffset.FromUnixTimeSeconds(scan.Timestamp ?? 0).UtcDateTime : null)
            });

            await _hubContext.Clients.All.SendAsync("NewLogEntry", result);
        }

        private async Task handleDesconnectAsync(MqttClientDisconnectedEventArgs e)
        {
            var delay = _config.GetValue<int>("MqttBroker:ReconnectionDelay", 5);
            Console.WriteLine($"MQTT disconnected, reconnecting in {delay}s...");
            await Task.Delay(TimeSpan.FromSeconds(delay));
            await _client!.ReconnectAsync();
        }
    }
}