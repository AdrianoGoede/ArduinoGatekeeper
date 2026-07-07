using System.Security.Cryptography;
using System.Security.Cryptography.X509Certificates;
using MQTTnet;

namespace ArduinoGatekeeperBackend.Mqtt
{
    public class MqttService : IHostedService
    {
        private readonly IConfiguration       _config;
        private readonly IServiceScopeFactory _scopeFactory;
        private IMqttClient?                  _client;

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
        }

        public async Task StopAsync(CancellationToken cancellationToken)
        {
            throw new NotImplementedException();
        }
    }
}