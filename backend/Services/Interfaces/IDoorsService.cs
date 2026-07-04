using ArduinoGatekeeperBackend.EntityFramework.Models;

namespace ArduinoGatekeeperBackend.Services.Interfaces
{
    public interface IDoorsService
    {
        public IQueryable<Door> GetAll();
        public Task<Door?> GetByIdAsync(int id);
        public Task<Door> CreateAsync(DoorDTO door);
        public Task<Door> UpdateAsync(int id, DoorDTO modified);
        public Task DeleteAsync(int id);
    }

    public record DoorDTO
    {
        public int? Id { get; set; }
        public string? Label { get; set; }
        public bool? Active { get; set; }
    }
}