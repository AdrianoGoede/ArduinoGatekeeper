using ArduinoGatekeeperBackend.Events;

namespace ArduinoGatekeeperBackend.Websocket
{
    public interface IDashboardNotifier
    {
        Task NotifyNewScanEntryAsync(DoorScanRecordedEvent e);
        Task NotifyNewDoorStatusEntryAsync(DoorStatusRecordedEvent e);
        Task NotifyUserCreationAsync(UserCreatedEvent e);
        Task NotifyUserModificationAsync(UserEditedEvent e);
        Task NotifyUserDeletionAsync(UserRemovedEvent e);
    }
}