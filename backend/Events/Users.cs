using ArduinoGatekeeperBackend.EntityFramework.Models;

namespace ArduinoGatekeeperBackend.Events
{
    public record UserCreatedEvent(User User);
    public record UserEditedEvent(User User, string OldCardId, IEnumerable<int> OldPermissions);
    public record UserRemovedEvent(User User);
    public record SyncUsersToDoorRequestedEvent(int DoorId, IEnumerable<User> Users);
}