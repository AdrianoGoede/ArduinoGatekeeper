using System.ComponentModel.DataAnnotations;

namespace ArduinoGatekeeperBackend.EntityFramework.Models
{
    public class Door
    {
        [Key]
        public int Id { get; set; }

        public string Label { get; set; }

        public DateTime CreatedAt { get; set; }
    }
}