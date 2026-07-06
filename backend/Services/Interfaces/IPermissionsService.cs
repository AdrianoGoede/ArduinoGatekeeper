using ArduinoGatekeeperBackend.EntityFramework.Models;

namespace ArduinoGatekeeperBackend.Services.Interfaces
{
    public interface IPermissionsService
    {
        public IQueryable<Permission> GetAll();
        public Task<Permission?> GetByUserAndDoorIdAsync(int userId, int doorId);
        public Task<Permission> CreateAsync(PermissionDTO door);
        public Task DeleteAsync(int userId, int doorId);
    }

    public record PermissionDTO
    {
        public int? UserId { get; set; }
        public int? DoorId { get; set; }
    }
}