namespace ArduinoGatekeeperBackend.Mqtt.Models
{
    public record DeviceStatus
    {
        public string DeviceId { get; set; }
        public bool Online { get; set; }
        public long? Timestamp { get; set; }
    }
}