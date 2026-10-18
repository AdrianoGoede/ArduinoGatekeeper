using ArduinoGatekeeperBackend.EntityFramework;
using ArduinoGatekeeperBackend.EntityFramework.Models;
using ArduinoGatekeeperBackend.Events;
using ArduinoGatekeeperBackend.Services.Interfaces;
using Microsoft.EntityFrameworkCore;

namespace ArduinoGatekeeperBackend.Services.Implementations
{
    public class UsersService : IUsersService
    {
        private readonly ArduinoGatekeeperContext _dbContext;
        private readonly IEventBus _eventBus;
        private readonly ILogger<IUsersService> _logger;

        public UsersService(ArduinoGatekeeperContext dbContext, IEventBus eventBus, ILogger<IUsersService> logger)
        {
            _dbContext = dbContext ?? throw new ArgumentNullException(nameof(dbContext));
            _eventBus = eventBus ?? throw new ArgumentNullException(nameof(eventBus));
            _logger = logger ?? throw new ArgumentNullException(nameof(logger));
        }
        
        public IQueryable<User> GetAll() => _dbContext.Users.AsNoTracking().Where(it => it.Active);

        public IQueryable<User> GetByIdAsync(int id) => _dbContext.Users.AsNoTracking().Where(it => it.Id == id && it.Active);
        
        public async Task<User> CreateAsync(UserDTO user)
        {
            try
            {
                var newUser = _dbContext.Users.Add(new User {
                    Label = user.Label?.Trim(),
                    CardId = user.CardId?.Trim(),
                    CardKey = Convert.FromBase64String(user.CardKey ?? string.Empty),
                    Active = true,
                    Permissions = (user.AllowedDoors ?? []).Select(doorId => new Permission { DoorId = doorId }).ToArray()
                });

                await _dbContext.SaveChangesAsync();
                _eventBus.Publish(new UserCreatedEvent(newUser.Entity));
                
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
                var existing = await _dbContext.Users.Include(it => it.Permissions).SingleOrDefaultAsync(it => it.Id == id) ?? throw new ArgumentException($"No record found with ID {id}");
                var oldCardId = existing.CardId;
                var oldPermissions = existing.Permissions.Select(it => it.DoorId).ToArray();
                
                existing.Label = (modified.Label ?? existing.Label);
                existing.CardId = (modified.CardId ?? existing.CardId);
                existing.CardKey = (!string.IsNullOrWhiteSpace(modified.CardKey) ? Convert.FromBase64String(modified.CardKey) : existing.CardKey);

                var newPermissions = (modified.AllowedDoors ?? []).ToHashSet();
                var permissionsToRemove = existing.Permissions.Where(it => !newPermissions.Contains(it.DoorId)).ToArray();
                foreach (var permission in permissionsToRemove)
                    existing.Permissions.Remove(permission);

                var permissionsToAdd = newPermissions.Except(existing.Permissions.Select(it => it.DoorId)).ToArray();
                foreach (var doorId in permissionsToAdd)
                    existing.Permissions.Add(new Permission { DoorId = doorId });

                await _dbContext.SaveChangesAsync();
                _eventBus.Publish(new UserEditedEvent(existing, oldCardId, oldPermissions));

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
                var user = await _dbContext.Users.Include(it => it.Permissions).SingleOrDefaultAsync(it => it.Id == id) ?? throw new ArgumentException($"No record found with ID {id}");
                user.Active = false;
                await _dbContext.SaveChangesAsync();
                _eventBus.Publish(new UserRemovedEvent(user));
            }
            catch (Exception ex)
            {
                _logger.LogError(ex.InnerException?.Message ?? ex.Message);
                throw;
            }
        }
    }
}