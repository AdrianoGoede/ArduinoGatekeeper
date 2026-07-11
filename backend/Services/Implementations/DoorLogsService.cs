using ArduinoGatekeeperBackend.EntityFramework;
using ArduinoGatekeeperBackend.EntityFramework.Models;
using ArduinoGatekeeperBackend.Services.Interfaces;
using Microsoft.EntityFrameworkCore;

namespace ArduinoGatekeeperBackend.Services.Implementations
{
    public class DoorLogsService : IDoorLogsService
    {
        private readonly ArduinoGatekeeperContext _dbContext;

        public DoorLogsService(ArduinoGatekeeperContext dbContext)
        {
            _dbContext = dbContext ?? throw new ArgumentNullException(nameof(dbContext));
        }
        
        public IQueryable<DoorLog> GetAll() => _dbContext.DoorLogs.AsNoTracking();

        public async Task<DoorLog?> GetByIdAsync(long id) => await _dbContext.DoorLogs.AsNoTracking().SingleOrDefaultAsync(it => it.Id == id);

        public async Task<DoorLog> CreateAsync(DoorLogDTO log)
        {
            var newLog = _dbContext.DoorLogs.Add(new DoorLog {
                DoorId = (log.DoorId ?? 0),
                Online = (log.Online ?? false),
                CreatedAt = (log.CreatedAt ?? DateTime.UtcNow)
            });
            await _dbContext.SaveChangesAsync();
            return newLog.Entity;
        }
    }
}