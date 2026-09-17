using System.ComponentModel.DataAnnotations;

namespace ArduinoGatekeeperBackend.EntityFramework.Models
{
    public class Door
    {
        [Key]
        public int Id { get; set; }

        public string Label { get; set; }

        public DateTime CreatedAt { get; set; }

        public ICollection<Permission> Permissions { get; set; } = new List<Permission>();

        public ICollection<AccessLog> AccessLogs { get; set; } = new List<AccessLog>();

        public ICollection<DoorLog> DoorLogs { get; set; } = new List<DoorLog>();
    }
}