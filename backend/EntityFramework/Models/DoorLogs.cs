using System.ComponentModel.DataAnnotations;

namespace ArduinoGatekeeperBackend.EntityFramework.Models
{
    public class DoorLog
    {
        [Key]
        public long Id { get; set; }
        
        public int DoorId { get; set; }

        public bool Online { get; set; }

        public DateTime CreatedAt { get; set; }

        public Door Door { get; set; }
    }
}