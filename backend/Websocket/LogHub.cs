using Microsoft.AspNetCore.SignalR;

namespace ArduinoGatekeeperBackend.Websocket
{
    public class LogHub : Hub
    {
        public override async Task OnDisconnectedAsync(Exception? ex) => await base.OnDisconnectedAsync(ex);
    }
}