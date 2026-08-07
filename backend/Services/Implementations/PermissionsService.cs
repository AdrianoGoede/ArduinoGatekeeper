using ArduinoGatekeeperBackend.EntityFramework;
using ArduinoGatekeeperBackend.EntityFramework.Models;
using ArduinoGatekeeperBackend.Services.Interfaces;
using Microsoft.EntityFrameworkCore;

namespace ArduinoGatekeeperBackend.Services.Implementations
{
    public class PermissionsService : IPermissionsService
    {
        private readonly ArduinoGatekeeperContext _dbContext;
        private readonly ILogger<IPermissionsService> _logger;

        public PermissionsService(ArduinoGatekeeperContext dbContext, ILogger<IPermissionsService> logger)
        {
            _dbContext = dbContext ?? throw new ArgumentNullException(nameof(dbContext));
            _logger = logger ?? throw new ArgumentNullException(nameof(logger));
        }
        
        public IQueryable<Permission> GetAll() => _dbContext.Permissions.AsNoTracking();

        public async Task<Permission?> GetByUserAndDoorIdAsync(int userId, int doorId) => await _dbContext.Permissions.AsNoTracking().SingleOrDefaultAsync(it => it.UserId == userId && it.DoorId == doorId);
        
        public async Task<Permission> CreateAsync(PermissionDTO door)
        {
            try
            {
                var newPermission = _dbContext.Permissions.Add(new Permission {
                    UserId = (door.UserId ?? 0),
                    DoorId = (door.DoorId ?? 0)
                });
                await _dbContext.SaveChangesAsync();
                return newPermission.Entity;
            }
            catch (Exception ex)
            {
                _logger.LogError(ex.InnerException?.Message ?? ex.Message);
                throw;
            }
        }

        public async Task DeleteAsync(int userId, int doorId)
        {
            try
            {
                var permission = await _dbContext.Permissions.FindAsync(userId, doorId) ?? throw new ArgumentException($"No record found with User ID {userId} and Door ID {doorId}");
                _dbContext.Permissions.Remove(permission);
                await _dbContext.SaveChangesAsync();
            }
            catch (Exception ex)
            {
                _logger.LogError(ex.InnerException?.Message ?? ex.Message);
                throw;
            }
        }
    }
}