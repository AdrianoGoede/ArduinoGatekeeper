using ArduinoGatekeeperBackend.EntityFramework.Models;

namespace ArduinoGatekeeperBackend.Services.Interfaces
{
    public interface IUsersService
    {
        public IQueryable<User> GetAll();
        public Task<User?> GetByIdAsync(int id);
        public Task<User> CreateAsync(UserDTO user);
        public Task<User> UpdateAsync(int id, UserDTO modified);
        public Task DeleteAsync(int id);
    }

    public record UserDTO
    {
        public int? Id { get; set; }
        public string? CardId { get; set; }
        public string? Label { get; set; }
    }
}