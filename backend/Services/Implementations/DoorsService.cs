using ArduinoGatekeeperBackend.EntityFramework;
using ArduinoGatekeeperBackend.EntityFramework.Models;
using ArduinoGatekeeperBackend.Services.Interfaces;
using Microsoft.EntityFrameworkCore;

namespace ArduinoGatekeeperBackend.Services.Implementations
{
    public class DoorsService : IDoorsService
    {
        private readonly ArduinoGatekeeperContext _dbContext;
        private readonly ILogger<IDoorsService> _logger;

        public DoorsService(ArduinoGatekeeperContext dbContext, ILogger<IDoorsService> logger)
        {
            _dbContext = dbContext ?? throw new ArgumentNullException(nameof(dbContext));
            _logger = logger ?? throw new ArgumentNullException(nameof(logger));
        }
        
        public IQueryable<Door> GetAll() => _dbContext.Doors.AsNoTracking();

        public async Task<Door?> GetByIdAsync(int id) => await _dbContext.Doors.AsNoTracking().SingleOrDefaultAsync(it => it.Id == id);
        
        public async Task<Door> CreateAsync(DoorDTO door)
        {
            try
            {
                var newDoor = _dbContext.Doors.Add(new Door { Label = door.Label?.Trim() });
                await _dbContext.SaveChangesAsync();
                return newDoor.Entity;
            }
            catch (Exception ex)
            {
                _logger.LogError(ex.InnerException?.Message ?? ex.Message);
                throw;
            }
        }

        public async Task<Door> UpdateAsync(int id, DoorDTO modified)
        {
            try
            {
                var existing = await _dbContext.Doors.SingleOrDefaultAsync(it => it.Id == id) ?? throw new ArgumentException($"No record found with ID {id}");
                existing.Label = (modified.Label ?? existing.Label);
                await _dbContext.SaveChangesAsync();
                return existing;
            }
            catch (Exception ex)
            {
                _logger.LogError(ex.InnerException?.Message ?? ex.Message);
                throw;
            }
        }

        public async Task DeleteAsync(int id)
        {
            try
            {
                var door = await _dbContext.Doors.SingleOrDefaultAsync(it => it.Id == id) ?? throw new ArgumentException($"No record found with ID {id}");
                _dbContext.Doors.Remove(door);
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