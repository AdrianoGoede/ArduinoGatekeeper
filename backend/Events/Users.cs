using ArduinoGatekeeperBackend.EntityFramework.Models;

namespace ArduinoGatekeeperBackend.Events
{
    public record UserCreatedEvent(User User);
    public record UserRemovedEvent(User User);
    public record SyncUsersToDoorRequestedEvent(int doorId, IEnumerable<User> Users);
}