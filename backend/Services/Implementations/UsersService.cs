using ArduinoGatekeeperBackend.EntityFramework;
using ArduinoGatekeeperBackend.EntityFramework.Models;
using ArduinoGatekeeperBackend.Services.Interfaces;
using Microsoft.EntityFrameworkCore;

namespace ArduinoGatekeeperBackend.Services.Implementations
{
    public class UsersService : IUsersService
    {
        private readonly ArduinoGatekeeperContext _dbContext;
        private readonly ILogger<IUsersService> _logger;

        public UsersService(ArduinoGatekeeperContext dbContext, ILogger<IUsersService> logger)
        {
            _dbContext = dbContext ?? throw new ArgumentNullException(nameof(dbContext));
            _logger = logger ?? throw new ArgumentNullException(nameof(logger));
        }
        
        public IQueryable<User> GetAll() => _dbContext.Users.AsNoTracking();

        public async Task<User?> GetByIdAsync(int id) => await _dbContext.Users.AsNoTracking().SingleOrDefaultAsync(it => it.Id == id);
        
        public async Task<User> CreateAsync(UserDTO user)
        {
            try
            {
                var newUser = _dbContext.Users.Add(new User {
                    Label = user.Label?.Trim(),
                    CardId = user.CardId?.Trim()
                });
                await _dbContext.SaveChangesAsync();
                return newUser.Entity;
            }
            catch (Exception ex)
            {
                _logger.LogError(ex.InnerException?.Message ?? ex.Message);
                throw;
            }
        }

        public async Task<User> UpdateAsync(int id, UserDTO modified)
        {
            try
            {
                var existing = await _dbContext.Users.SingleOrDefaultAsync(it => it.Id == id) ?? throw new ArgumentException($"No record found with ID {id}");
                existing.CardId = (modified.CardId ?? existing.CardId);
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
                var user = await _dbContext.Users.SingleOrDefaultAsync(it => it.Id == id) ?? throw new ArgumentException($"No record found with ID {id}");
                _dbContext.Users.Remove(user);
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