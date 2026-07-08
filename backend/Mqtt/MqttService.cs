using System.Security.Cryptography;
using System.Security.Cryptography.X509Certificates;
using ArduinoGatekeeperBackend.Mqtt.Models;
using ArduinoGatekeeperBackend.Services.Interfaces;
using MQTTnet;
using Newtonsoft.Json;

namespace ArduinoGatekeeperBackend.Mqtt
{
    public class MqttService : IHostedService
    {
        private readonly IConfiguration _config;
        private readonly IServiceScopeFactory _scopeFactory;
        private IMqttClient? _client;

        public MqttService(IConfiguration config, IServiceScopeFactory scopeFactory)
        {
            _config = config ?? throw new ArgumentNullException(nameof(config));
            _scopeFactory = scopeFactory ?? throw new ArgumentNullException(nameof(scopeFactory));
        }
        
        public async Task StartAsync(CancellationToken cancellationToken)
        {
            _client = new MqttClientFactory().CreateMqttClient();

            var clientCert = X509CertificateLoader.LoadCertificateFromFile(_config.GetValue<string>("Ssl:ServerCert")!);
            using var clientKey = ECDsa.Create();
            clientKey.ImportFromPem(File.ReadAllText(_config.GetValue<string>("Ssl:ServerKey")!));

            _client.ApplicationMessageReceivedAsync += handleIncomingMessageAsync;
            
            var options = new MqttClientOptionsBuilder()
                .WithTcpServer(_config.GetValue<string>("MqttBroker:Host"), _config.GetValue<int>("MqttBroker:Port", 8883))
                .WithTlsOptions(tls => {
                    tls.UseTls(true);
                    tls.WithCertificateValidationHandler(ctx => {
                        ctx.Chain.ChainPolicy.RevocationMode = X509RevocationMode.NoCheck;
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
            
        }

        private async Task handleScanMessageAsync(string payload)
        {
            var scan = JsonConvert.DeserializeObject<Scan>(payload);
            if (scan is null) return;

            await using var scope = _scopeFactory.CreateAsyncScope();
            var accessLogsService = scope.ServiceProvider.GetRequiredService<IAccessLogsService>();
            await accessLogsService.CreateAsync(new AccessLogDTO {
                CardId = (scan.CardId ?? string.Empty),
                DoorId = int.Parse(scan.DeviceId.Replace(_config.GetValue<string>("MqttBroker:DeviceIdPrefix")!, string.Empty)),
                Granted = scan.Granted,
                CreatedAt = scan.Timestamp
            });
        }
    }
}