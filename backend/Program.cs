using System.Security.Authentication;
using System.Security.Cryptography.X509Certificates;
// using ArduinoGatekeeperBackend.Data;
// using ArduinoGatekeeperBackend.Hubs;
// using ArduinoGatekeeperBackend.Models;
// using ArduinoGatekeeperBackend.Services;
// using ArduinoGatekeeperBackend.Endpoints;
using Microsoft.AspNetCore.Authentication.Certificate;
using Microsoft.AspNetCore.OData;
using Microsoft.AspNetCore.Server.Kestrel.Https;
using Microsoft.EntityFrameworkCore;
using Microsoft.OData.ModelBuilder;
using ArduinoGatekeeperBackend.EntityFramework;
using ArduinoGatekeeperBackend.EntityFramework.Models;
using ArduinoGatekeeperBackend.Services.Interfaces;
using ArduinoGatekeeperBackend.Services;

var builder = WebApplication.CreateBuilder(args);

// Database
builder.Services.AddDbContext<ArduinoGatekeeperContext>(options => options.UseNpgsql(builder.Configuration.GetConnectionString("AgkDatabase")));

// OData
var modelBuilder = new ODataConventionModelBuilder();
modelBuilder.EntitySet<Admin>("Admins");
modelBuilder.EntitySet<User>("Users");
modelBuilder.EntitySet<Door>("Doors");
modelBuilder.EntitySet<Permission>("Permissions");

builder.Services.AddControllers()
    .AddOData(options => options
        .AddRouteComponents("api", modelBuilder.GetEdmModel())
        .Select()
        .Filter()
        .OrderBy()
        .Expand()
        .Count()
        .SetMaxTop(1000));

// Authentication — mTLS client certificate, CN validated against DB
// builder.Services.AddAuthentication(CertificateAuthenticationDefaults.AuthenticationScheme)
//     .AddCertificate(options => {
//         options.AllowedCertificateTypes = CertificateTypes.All;
//         options.RevocationMode          = X509RevocationMode.NoCheck;
//         options.ValidateCertificateUse  = true;

//         options.Events = new CertificateAuthenticationEvents {
//             OnCertificateValidated = async context => {
//                 var cn = context.ClientCertificate.GetNameInfo(X509NameType.SimpleName, false);

//                 var db = context.HttpContext.RequestServices.GetRequiredService<AppDbContext>();

//                 var isAuthorized = await db.Admins.AnyAsync(a => a.Cn == cn && a.Active);

//                 if (!isAuthorized)
//                 {
//                     context.Fail($"Certificate CN '{cn}' is not an authorized admin");
//                     return;
//                 }

//                 context.Success();
//             },

//             OnAuthenticationFailed = context =>
//             {
//                 context.Fail("Certificate authentication failed");
//                 return Task.CompletedTask;
//             }
//         };
//     });

builder.Services.AddAuthorization();

// SignalR — real-time push to log analyzer
// builder.Services.AddSignalR();

// MQTT background service
// builder.Services.AddHostedService<MqttService>();

// User list publisher — republishes UserList to broker on user changes
builder.Services.AddScoped<IAdminsService, AdminsService>();
builder.Services.AddScoped<IUsersService, UsersService>();
builder.Services.AddScoped<IDoorsService, DoorsService>();
builder.Services.AddScoped<IPermissionsService, PermissionsService>();

// Kestrel — mTLS, TLS 1.3 only
// builder.WebHost.ConfigureKestrel(options => {
//     options.ListenAnyIP(443, listen => {
//         listen.UseHttps(https => {
//             // Server certificate
//             https.ServerCertificate = new X509Certificate2(
//                 builder.Configuration["Certs:ServerCert"]!,
//                 builder.Configuration["Certs:ServerKey"]!
//             );

//             // Require client certificate (mTLS)
//             https.ClientCertificateMode = ClientCertificateMode.RequireCertificate;

//             // Only accept certs signed by our CA
//             https.ClientCertificateValidation = (cert, chain, errors) => {
//                 var ca = new X509Certificate2(builder.Configuration["Certs:CaCert"]!);
//                 chain.ChainPolicy.TrustMode = X509ChainTrustMode.CustomRootTrust;
//                 chain.ChainPolicy.CustomTrustStore.Add(ca);
//                 chain.ChainPolicy.VerificationFlags = X509VerificationFlags.AllowUnknownCertificateAuthority;
//                 return chain.Build(new X509Certificate2(cert));
//             };

//             // TLS 1.3 only — enforces AES-GCM and ChaCha20 cipher suites
//             https.SslProtocols = SslProtocols.Tls13;
//         });
//     });
// });

var app = builder.Build();

// Middleware pipeline
// app.UseHttpsRedirection();
// app.UseAuthentication();
// app.UseAuthorization();

// Endpoints
app.MapControllers();

app.Run();