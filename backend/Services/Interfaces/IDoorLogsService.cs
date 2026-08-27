using ArduinoGatekeeperBackend.EntityFramework.Models;

namespace ArduinoGatekeeperBackend.Services.Interfaces
{
    public interface IDoorLogsService
    {
        public IQueryable<DoorLog> GetAll();
        public Task<DoorLog?> GetByIdAsync(long id);
        public Task<DoorLog> CreateAsync(DoorLogDTO log);
    }

    public record DoorLogDTO
    {
        public int? DoorId { get; set; }
        public bool? Online { get; set; }
        public DateTime? CreatedAt { get; set; }
    }
}