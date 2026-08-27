using ArduinoGatekeeperBackend.EntityFramework.Models;

namespace ArduinoGatekeeperBackend.Services.Interfaces
{
    public interface IAccessLogsService
    {
        public IQueryable<AccessLog> GetAll();
        public Task<AccessLog?> GetByIdAsync(long id);
        public Task<AccessLog> CreateAsync(AccessLogDTO log);
    }

    public record AccessLogDTO
    {
        public string? CardId { get; set; }
        public int? DoorId { get; set; }
        public bool? Granted { get; set; }
        public DateTime? CreatedAt { get; set; }
    }
}