using ArduinoGatekeeperBackend.EntityFramework;
using ArduinoGatekeeperBackend.EntityFramework.Models;
using ArduinoGatekeeperBackend.Services.Interfaces;
using Microsoft.EntityFrameworkCore;

namespace ArduinoGatekeeperBackend.Services
{
    public class AdminsService : IAdminsService
    {
        private readonly ArduinoGatekeeperContext _dbContext;

        public AdminsService(ArduinoGatekeeperContext dbContext)
        {
            _dbContext = dbContext ?? throw new ArgumentNullException(nameof(dbContext));
        }
        
        public IQueryable<Admin> GetAll() => _dbContext.Admins.AsNoTracking();

        public async Task<Admin?> GetByIdAsync(int id) => await _dbContext.Admins.AsNoTracking().SingleOrDefaultAsync(it => it.Id == id);

        public async Task<Admin> CreateAsync(AdminDTO admin)
        {
            var newAdmin = _dbContext.Admins.Add(new Admin {
                Label = admin.Label?.Trim(),
                Active = (admin.Active ?? true)
            });
            await _dbContext.SaveChangesAsync();
            return newAdmin.Entity;
        }
        
        public async Task<Admin> UpdateAsync(int id, AdminDTO modified)
        {
            var existing = await _dbContext.Admins.SingleOrDefaultAsync(it => it.Id == id) ?? throw new ArgumentException($"No record found with ID {id}");
            existing.Label = (modified.Label ?? existing.Label);
            existing.Active = (modified.Active ?? existing.Active);
            await _dbContext.SaveChangesAsync();
            return existing;
        }

        public async Task DeleteAsync(int id)
        {
            var admin = await _dbContext.Admins.SingleOrDefaultAsync(it => it.Id == id) ?? throw new ArgumentException($"No record found with ID {id}");
            _dbContext.Admins.Remove(admin);
            await _dbContext.SaveChangesAsync();
        }
    }
}