using Microsoft.AspNetCore.SignalR;

namespace ArduinoGatekeeperBackend.Websocket
{
    public class LogHub : Hub
    {
        public async Task SubscribeToDoor(string doorId) => await Groups.AddToGroupAsync(Context.ConnectionId, doorId);

        public async Task UnsubscribeFromDoor(string doorId) => await Groups.RemoveFromGroupAsync(Context.ConnectionId, doorId);

        public override async Task OnDisconnectedAsync(Exception? ex) => await base.OnDisconnectedAsync(ex);
    }
}