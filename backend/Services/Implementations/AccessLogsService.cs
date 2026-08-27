using ArduinoGatekeeperBackend.EntityFramework;
using ArduinoGatekeeperBackend.EntityFramework.Models;
using ArduinoGatekeeperBackend.Services.Interfaces;
using Microsoft.EntityFrameworkCore;

namespace ArduinoGatekeeperBackend.Services.Implementations
{
    public class AccessLogsService : IAccessLogsService
    {
        private readonly ArduinoGatekeeperContext _dbContext;
        private readonly ILogger<IAccessLogsService> _logger;

        public AccessLogsService(ArduinoGatekeeperContext dbContext, ILogger<AccessLogsService> logger)
        {
            _dbContext = dbContext ?? throw new ArgumentNullException(nameof(dbContext));
            _logger = logger ?? throw new ArgumentNullException(nameof(logger));
        }
        
        public IQueryable<AccessLog> GetAll() => _dbContext.AccessLogs.AsNoTracking();

        public async Task<AccessLog?> GetByIdAsync(long id) => await _dbContext.AccessLogs.AsNoTracking().SingleOrDefaultAsync(it => it.Id == id);
        
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
    }
}