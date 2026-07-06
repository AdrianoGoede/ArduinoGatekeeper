namespace ArduinoGatekeeperBackend.EntityFramework.Models
{
    public class AccessLog
    {
        public long Id { get; set; }

        public int UserId { get; set; }

        public int DoorId { get; set; }

        public bool Granted { get; set; }

        public DateTime CreatedAt { get; set; }

        public User User { get; set; }

        public Door Door { get; set; }
    }
}