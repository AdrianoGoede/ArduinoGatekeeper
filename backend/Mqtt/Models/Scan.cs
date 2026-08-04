namespace ArduinoGatekeeperBackend.Mqtt.Models
{
    public record Scan
    {
        public string CardId { get; set; }
        public bool Granted { get; set; }
        public long? Timestamp { get; set; }
    }
}