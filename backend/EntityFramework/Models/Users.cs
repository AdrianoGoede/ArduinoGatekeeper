using System.ComponentModel.DataAnnotations;

namespace ArduinoGatekeeperBackend.EntityFramework.Models
{
    public class User
    {
        [Key]
        public int Id { get; set; }

        public string CardId { get; set; }

        public string Label { get; set; }

        public DateTime CreatedAt { get; set; }

        public IEnumerable<Permission> Permissions { get; set; }

        public IEnumerable<AccessLog> AccessLogs { get; set; }
    }
}