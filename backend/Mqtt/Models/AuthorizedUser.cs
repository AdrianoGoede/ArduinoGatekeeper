namespace ArduinoGatekeeperBackend.Mqtt.Models
{
    public record AuthorizedUser
    {
        public string Uid { get; set; }

        public IEnumerable<int> Key { get; set; }
    }
}