using ArduinoGatekeeperBackend.EntityFramework;
using ArduinoGatekeeperBackend.EntityFramework.Models;
using ArduinoGatekeeperBackend.Events;
using ArduinoGatekeeperBackend.Services.Interfaces;
using Microsoft.EntityFrameworkCore;

namespace ArduinoGatekeeperBackend.Services.Implementations
{
    public class AccessLogsService : IAccessLogsService
    {
        private readonly IConfiguration _config;
        private readonly ArduinoGatekeeperContext _dbContext;
        private readonly IEventBus _eventBus;
        private readonly ILogger<IAccessLogsService> _logger;

        public AccessLogsService(IConfiguration config, ArduinoGatekeeperContext dbContext, IEventBus eventBus, ILogger<AccessLogsService> logger)
        {
            _config = config ?? throw new ArgumentNullException(nameof(config));
            _dbContext = dbContext ?? throw new ArgumentNullException(nameof(dbContext));
            _eventBus = eventBus ?? throw new ArgumentNullException(nameof(eventBus));
            _logger = logger ?? throw new ArgumentNullException(nameof(logger));

            _eventBus.Subscribe<DoorScanEvent>(async e => await HandleScanEventAsync(e));
        }
        
        public IQueryable<AccessLog> GetAll() => _dbContext.AccessLogs.AsNoTracking();

        public IQueryable<AccessLog> GetByIdAsync(long id) => _dbContext.AccessLogs.AsNoTracking().Where(it => it.Id == id);
        
        public async Task<AccessLog> CreateAsync(AccessLogDTO log)
        {
            try
            {
                var userId = await _dbContext.Users.AsNoTracking().Where(it => it.CardId == log.CardId).Select(it => it.Id).SingleOrDefaultAsync();

                var newLog = _dbContext.AccessLogs.Add(new AccessLog {
                    UserId = userId,
                    DoorId = (log.DoorId ?? 0),
                    Granted = (log.Granted ?? false),
                    CreatedAt = (log.CreatedAt ?? DateTime.UtcNow)
                });
                await _dbContext.SaveChangesAsync();
                return newLog.Entity;
            }
            catch (Exception ex)
            {
                _logger.LogError(ex.InnerException?.Message ?? ex.Message);
                throw;
            }
        }

        private async Task HandleScanEventAsync(DoorScanEvent e) => await CreateAsync(new AccessLogDTO {
            CardId = (e.Scan.CardId ?? string.Empty),
            DoorId = int.Parse(e.DeviceId.Replace(_config.GetValue<string>("MqttBroker:DeviceIdPrefix")!, string.Empty)),
            Granted = e.Scan.Granted,
            CreatedAt = (e.Scan.Timestamp is not null ? DateTimeOffset.FromUnixTimeSeconds(e.Scan.Timestamp ?? 0).UtcDateTime : null)
        });
    }
}