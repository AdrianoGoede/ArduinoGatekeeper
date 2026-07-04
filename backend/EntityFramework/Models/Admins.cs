using System.ComponentModel.DataAnnotations;

#nullable disable
namespace ArduinoGatekeeperBackend.EntityFramework.Models
{
    public class Admin
    {
        [Key]
        public int Id { get; set; }

        public string CommonName { get; set; }

        public string Label { get; set; }

        public bool Active { get; set; }

        public DateTime CreatedAt { get; set; }
    }
}