namespace ArduinoGatekeeperBackend.Events
{
    public interface IEventBus
    {
        void Publish<T>(T @event);
        void Subscribe<T>(Action<T> handler);
    }
}