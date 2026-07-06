using System.ComponentModel.DataAnnotations;

namespace ArduinoGatekeeperBackend.EntityFramework.Models
{
    public class Permission
    {
        [Key]
        public int UserId { get; set; }

        [Key]
        public int DoorId { get; set; }

        public DateTime CreatedAt { get; set; }

        public User? User { get; set; }

        public Door? Door { get; set; }
    }
}