using ArduinoGatekeeperBackend.EntityFramework.Models;

namespace ArduinoGatekeeperBackend.Services.Interfaces
{
    public interface IAdminsService
    {
        public IQueryable<Admin> GetAll();
        public Task<Admin?> GetByIdAsync(int id);
        public Task<Admin> CreateAsync(AdminDTO admin);
        public Task<Admin> UpdateAsync(int id, AdminDTO modified);
        public Task DeleteAsync(int id);
    }

    public record AdminDTO
    {
        public int? Id { get; set; }
        public string? Label { get; set; }
        public bool? Active { get; set; }
    }
}