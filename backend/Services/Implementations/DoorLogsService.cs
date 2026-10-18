using ArduinoGatekeeperBackend.EntityFramework;
using ArduinoGatekeeperBackend.EntityFramework.Models;
using ArduinoGatekeeperBackend.Events;
using ArduinoGatekeeperBackend.Services.Interfaces;
using Microsoft.EntityFrameworkCore;

namespace ArduinoGatekeeperBackend.Services.Implementations
{
    public class DoorLogsService : IDoorLogsService
    {
        private readonly IConfiguration _config;
        private readonly ArduinoGatekeeperContext _dbContext;
        private readonly IEventBus _eventBus;
        private readonly ILogger<IDoorLogsService> _logger;

        public DoorLogsService(IConfiguration config, ArduinoGatekeeperContext dbContext, IEventBus eventBus, ILogger<IDoorLogsService> logger)
        {
            _config = config ?? throw new ArgumentNullException(nameof(config));
            _dbContext = dbContext ?? throw new ArgumentNullException(nameof(dbContext));
            _eventBus = eventBus ?? throw new ArgumentNullException(nameof(eventBus));
            _logger = logger ?? throw new ArgumentNullException(nameof(logger));

            _eventBus.Subscribe<DoorStatusReceivedEvent>(async e => await HandleDoorStatusEventAsync(e));
        }
        
        public IQueryable<DoorLog> GetAll() => _dbContext.DoorLogs.AsNoTracking();

        public IQueryable<DoorLog> GetByIdAsync(long id) => _dbContext.DoorLogs.AsNoTracking().Where(it => it.Id == id);

        public async Task<DoorLog> CreateAsync(DoorLogDTO log)
        {
            try
            {
                var newLog = _dbContext.DoorLogs.Add(new DoorLog {
                    DoorId = (log.DoorId ?? 0),
                    Online = (log.Online ?? false),
                    CreatedAt = (log.CreatedAt ?? DateTime.UtcNow)
                });

                await _dbContext.SaveChangesAsync();
                _eventBus.Publish(new DoorStatusRecordedEvent(newLog.Entity));

                return newLog.Entity;
            }
            catch (Exception ex)
            {
                _logger.LogError(ex.InnerException?.Message ?? ex.Message);
                throw;
            }
        }

        private async Task HandleDoorStatusEventAsync(DoorStatusReceivedEvent e)
        {
            var deviceIdNum = int.Parse(e.DeviceId.Replace(_config.GetValue<string>("MqttBroker:DeviceIdPrefix")!, string.Empty));
            var result = await CreateAsync(new DoorLogDTO {
                DoorId = deviceIdNum,
                Online = e.DeviceStatus.Online,
                CreatedAt = (e.DeviceStatus.Timestamp is not null ? DateTimeOffset.FromUnixTimeSeconds(e.DeviceStatus.Timestamp ?? 0).UtcDateTime : null)
            });

            if (!e.DeviceStatus.Online) return;
            var users = await _dbContext.Permissions.AsNoTracking().Include(it => it.User).Where(it => it.DoorId == deviceIdNum).Select(it => it.User).ToArrayAsync();
            _eventBus.Publish(new SyncUsersToDoorRequestedEvent(deviceIdNum, users));
        }
    }
}