using ArduinoGatekeeperBackend.EntityFramework;
using ArduinoGatekeeperBackend.EntityFramework.Models;
using ArduinoGatekeeperBackend.Services.Interfaces;
using Microsoft.EntityFrameworkCore;

namespace ArduinoGatekeeperBackend.Services
{
    public class PermissionsService : IPermissionsService
    {
        private readonly ArduinoGatekeeperContext _dbContext;

        public PermissionsService(ArduinoGatekeeperContext dbContext)
        {
            _dbContext = dbContext ?? throw new ArgumentNullException(nameof(dbContext));
        }
        
        public IQueryable<Permission> GetAll() => _dbContext.Permissions.AsNoTracking();

        public async Task<Permission?> GetByUserAndDoorIdAsync(int userId, int doorId) => await _dbContext.Permissions.AsNoTracking().SingleOrDefaultAsync(it => it.UserId == userId && it.DoorId == doorId);
        
        public async Task<Permission> CreateAsync(PermissionDTO door)
        {
            var newPermission = _dbContext.Permissions.Add(new Permission {
                UserId = (door.UserId ?? 0),
                DoorId = (door.DoorId ?? 0)
            });
            await _dbContext.SaveChangesAsync();
            return newPermission.Entity;
        }

        public async Task DeleteAsync(int userId, int doorId)
        {
            var permission = await _dbContext.Permissions.FindAsync(userId, doorId) ?? throw new ArgumentException($"No record found with User ID {userId} and Door ID {doorId}");
            _dbContext.Permissions.Remove(permission);
            await _dbContext.SaveChangesAsync();
        }
    }
}