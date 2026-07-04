using ArduinoGatekeeperBackend.EntityFramework;
using ArduinoGatekeeperBackend.EntityFramework.Models;
using ArduinoGatekeeperBackend.Services.Interfaces;
using Microsoft.EntityFrameworkCore;

namespace ArduinoGatekeeperBackend.Services
{
    public class DoorsService : IDoorsService
    {
        private readonly ArduinoGatekeeperContext _dbContext;

        public DoorsService(ArduinoGatekeeperContext dbContext)
        {
            _dbContext = dbContext ?? throw new ArgumentNullException(nameof(dbContext));
        }
        
        public IQueryable<Door> GetAll() => _dbContext.Doors.AsNoTracking();

        public async Task<Door?> GetByIdAsync(int id) => await _dbContext.Doors.AsNoTracking().SingleOrDefaultAsync(it => it.Id == id);
        
        public async Task<Door> CreateAsync(DoorDTO door)
        {
            var newDoor = _dbContext.Doors.Add(new Door { Label = door.Label?.Trim() });
            await _dbContext.SaveChangesAsync();
            return newDoor.Entity;
        }

        public async Task<Door> UpdateAsync(int id, DoorDTO modified)
        {
            var existing = await _dbContext.Doors.SingleOrDefaultAsync(it => it.Id == id) ?? throw new ArgumentException($"No record found with ID {id}");
            existing.Label = (modified.Label ?? existing.Label);
            await _dbContext.SaveChangesAsync();
            return existing;
        }

        public async Task DeleteAsync(int id)
        {
            var door = await _dbContext.Doors.SingleOrDefaultAsync(it => it.Id == id) ?? throw new ArgumentException($"No record found with ID {id}");
            _dbContext.Doors.Remove(door);
            await _dbContext.SaveChangesAsync();
        }
    }
}