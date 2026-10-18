using ArduinoGatekeeperBackend.Events;
using Microsoft.AspNetCore.SignalR;

namespace ArduinoGatekeeperBackend.Websocket
{
    public class DashboardNotifier : IDashboardNotifier
    {
        private readonly IConfiguration _config;
        private readonly IEventBus _eventBus;
        private readonly IHubContext<LogHub> _hubContext;
        private readonly ILogger<IDashboardNotifier> _logger;

        public DashboardNotifier(IConfiguration config, IEventBus eventBus, IHubContext<LogHub> hubContext, ILogger<IDashboardNotifier> logger)
        {
            _config = config ?? throw new ArgumentNullException(nameof(config));
            _eventBus = eventBus ?? throw new ArgumentNullException(nameof(eventBus));
            _hubContext = hubContext ?? throw new ArgumentNullException(nameof(hubContext));
            _logger = logger ?? throw new ArgumentNullException(nameof(logger));

            _eventBus.Subscribe<DoorScanRecordedEvent>(async e => await NotifyNewScanEntryAsync(e));
            _eventBus.Subscribe<DoorStatusRecordedEvent>(async e => await NotifyNewDoorStatusEntryAsync(e));
            _eventBus.Subscribe<UserCreatedEvent>(async e => await NotifyUserCreationAsync(e));
            _eventBus.Subscribe<UserEditedEvent>(async e => await NotifyUserModificationAsync(e));
            _eventBus.Subscribe<UserRemovedEvent>(async e => await NotifyUserDeletionAsync(e));
        }

        public async Task NotifyNewScanEntryAsync(DoorScanRecordedEvent e)
        {
            try
            {
                var method = _config.GetValue<string>("Websocket:DoorScanMethod") ?? throw new ArgumentNullException("DoorScanMethod not informed");
                await _hubContext.Clients.All.SendAsync(method, e.AccessLog);
            }
            catch (Exception ex) { _logger.LogError(ex.InnerException?.Message ?? ex.Message); }
        }
        
        public async Task NotifyNewDoorStatusEntryAsync(DoorStatusRecordedEvent e)
        {
            try
            {
                var method = _config.GetValue<string>("Websocket:DoorStatusMethod") ?? throw new ArgumentNullException("DoorStatusMethod not informed");
                await _hubContext.Clients.All.SendAsync(method, e.DoorLog);
            }
            catch (Exception ex) { _logger.LogError(ex.InnerException?.Message ?? ex.Message); }
        }

        public async Task NotifyUserCreationAsync(UserCreatedEvent e)
        {
            try
            {
                var method = _config.GetValue<string>("Websocket:UserCreationMethod") ?? throw new ArgumentNullException("UserCreationMethod not informed");
                await _hubContext.Clients.All.SendAsync(method, e.User);
            }
            catch (Exception ex) { _logger.LogError(ex.InnerException?.Message ?? ex.Message); }
        }

        public async Task NotifyUserModificationAsync(UserEditedEvent e)
        {
            try
            {
                var method = _config.GetValue<string>("Websocket:UserModificationMethod") ?? throw new ArgumentNullException("UserModificationMethod not informed");
                await _hubContext.Clients.All.SendAsync(method, e.User);
            }
            catch (Exception ex) { _logger.LogError(ex.InnerException?.Message ?? ex.Message); }
        }

        public async Task NotifyUserDeletionAsync(UserRemovedEvent e)
        {
            try
            {
                var method = _config.GetValue<string>("Websocket:UserDeletionMethod") ?? throw new ArgumentNullException("UserDeletionMethod not informed");
                await _hubContext.Clients.All.SendAsync(method, e.User);
            }
            catch (Exception ex) { _logger.LogError(ex.InnerException?.Message ?? ex.Message); }
        }
    }
}