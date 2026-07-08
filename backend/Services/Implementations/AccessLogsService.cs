using ArduinoGatekeeperBackend.EntityFramework;
using ArduinoGatekeeperBackend.EntityFramework.Models;
using ArduinoGatekeeperBackend.Services.Interfaces;
using Microsoft.EntityFrameworkCore;

namespace ArduinoGatekeeperBackend.Services.Implementations
{
    public class AccessLogsService : IAccessLogsService
    {
        private readonly ArduinoGatekeeperContext _dbContext;

        public AccessLogsService(ArduinoGatekeeperContext dbContext)
        {
            _dbContext = dbContext ?? throw new ArgumentNullException(nameof(dbContext));
        }
        
        public IQueryable<AccessLog> GetAll() => _dbContext.AccessLogs.AsNoTracking();

        public async Task<AccessLog?> GetByIdAsync(long id) => await _dbContext.AccessLogs.AsNoTracking().SingleOrDefaultAsync(it => it.Id == id);
        
        public async Task<AccessLog> CreateAsync(AccessLogDTO log)
        {
            var userId = await _dbContext.Users.AsNoTracking().Where(it => it.CardId == log.CardId).Select(it => it.Id).SingleOrDefaultAsync();

            var newLog = _dbContext.AccessLogs.Add(new AccessLog {
                UserId = userId,
                DoorId = (log.DoorId ?? 0),
                Granted = (log.Granted ?? false)
            });
            await _dbContext.SaveChangesAsync();
            return newLog.Entity;
        }
    }
}