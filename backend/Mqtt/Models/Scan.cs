namespace ArduinoGatekeeperBackend.Mqtt.Models
{
    public record Scan
    {
        public string CardId { get; set; }
        public string DeviceId { get; set; }
        public bool Granted { get; set; }
        public DateTime Timestamp { get; set; }
    }
}