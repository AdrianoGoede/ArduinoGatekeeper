using ArduinoGatekeeperBackend.EntityFramework;
using ArduinoGatekeeperBackend.EntityFramework.Models;
using ArduinoGatekeeperBackend.Services.Interfaces;
using Microsoft.EntityFrameworkCore;

namespace ArduinoGatekeeperBackend.Services.Implementations
{
    public class DoorLogsService : IDoorLogsService
    {
        private readonly ArduinoGatekeeperContext _dbContext;
        private readonly ILogger<IDoorLogsService> _logger;

        public DoorLogsService(ArduinoGatekeeperContext dbContext, ILogger<IDoorLogsService> logger)
        {
            _dbContext = dbContext ?? throw new ArgumentNullException(nameof(dbContext));
            _logger = logger ?? throw new ArgumentNullException(nameof(logger));
        }
        
        public IQueryable<DoorLog> GetAll() => _dbContext.DoorLogs.AsNoTracking();

        public async Task<DoorLog?> GetByIdAsync(long id) => await _dbContext.DoorLogs.AsNoTracking().SingleOrDefaultAsync(it => it.Id == id);

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
                return newLog.Entity;
            }
            catch (Exception ex)
            {
                _logger.LogError(ex.InnerException?.Message ?? ex.Message);
                throw;
            }
        }
    }
}