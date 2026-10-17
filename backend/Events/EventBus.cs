namespace ArduinoGatekeeperBackend.Events
{
    public class EventBus : IEventBus
    {
        private readonly Dictionary<Type, List<object>> _handlers = new();
        
        public void Publish<T>(T @event)
        {
            if (!_handlers.TryGetValue(typeof(T), out var handlers)) return;
            foreach (var handler in handlers)
                ((Action<T>)handler)(@event);
        }

        public void Subscribe<T>(Action<T> handler)
        {
            if (!_handlers.ContainsKey(typeof(T)))
                _handlers[typeof(T)] = new();
            _handlers[typeof(T)].Add(handler);
        }
    }
}