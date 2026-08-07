using ArduinoGatekeeperBackend.EntityFramework;
using ArduinoGatekeeperBackend.EntityFramework.Models;
using ArduinoGatekeeperBackend.Services.Interfaces;
using Microsoft.EntityFrameworkCore;

namespace ArduinoGatekeeperBackend.Services.Implementations
{
    public class AdminsService : IAdminsService
    {
        private readonly ArduinoGatekeeperContext _dbContext;
        private readonly ILogger<IAdminsService> _logger;

        public AdminsService(ArduinoGatekeeperContext dbContext, ILogger<IAdminsService> logger)
        {
            _dbContext = dbContext ?? throw new ArgumentNullException(nameof(dbContext));
            _logger = logger ?? throw new ArgumentNullException(nameof(logger));
        }
        
        public IQueryable<Admin> GetAll() => _dbContext.Admins.AsNoTracking();

        public async Task<Admin?> GetByIdAsync(int id) => await _dbContext.Admins.AsNoTracking().SingleOrDefaultAsync(it => it.Id == id);

        public async Task<Admin> CreateAsync(AdminDTO admin)
        {
            try
            {
                var newAdmin = _dbContext.Admins.Add(new Admin {
                    Label = admin.Label?.Trim(),
                    Active = (admin.Active ?? true)
                });
                await _dbContext.SaveChangesAsync();
                return newAdmin.Entity;
            }
            catch (Exception ex)
            {
                _logger.LogError(ex.InnerException?.Message ?? ex.Message);
                throw;
            }
        }
        
        public async Task<Admin> UpdateAsync(int id, AdminDTO modified)
        {
            try
            {
                var existing = await _dbContext.Admins.SingleOrDefaultAsync(it => it.Id == id) ?? throw new ArgumentException($"No record found with ID {id}");
                existing.Label = (modified.Label ?? existing.Label);
                existing.Active = (modified.Active ?? existing.Active);
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
                var admin = await _dbContext.Admins.SingleOrDefaultAsync(it => it.Id == id) ?? throw new ArgumentException($"No record found with ID {id}");
                _dbContext.Admins.Remove(admin);
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