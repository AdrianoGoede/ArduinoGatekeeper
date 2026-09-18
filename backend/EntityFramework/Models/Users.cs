using System.ComponentModel.DataAnnotations;

namespace ArduinoGatekeeperBackend.EntityFramework.Models
{
    public class User
    {
        [Key]
        public int Id { get; set; }

        public string CardId { get; set; }

        public byte[] CardKey { get; set; }

        public string Label { get; set; }

        public bool Active { get; set; }

        public DateTime CreatedAt { get; set; }

        public ICollection<Permission> Permissions { get; set; } = new List<Permission>();

        public ICollection<AccessLog> AccessLogs { get; set; } = new List<AccessLog>();
    }
}