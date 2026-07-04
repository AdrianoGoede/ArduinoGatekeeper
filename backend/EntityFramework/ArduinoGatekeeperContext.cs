using System.Reflection.Emit;
using ArduinoGatekeeperBackend.EntityFramework.Models;
using Microsoft.EntityFrameworkCore;

namespace ArduinoGatekeeperBackend.EntityFramework
{
    public class ArduinoGatekeeperContext : DbContext
    {
        public DbSet<Admin> Admins { get; set; }
        
        public ArduinoGatekeeperContext(DbContextOptions<ArduinoGatekeeperContext> options) : base(options) {}

        protected override void OnConfiguring(DbContextOptionsBuilder optionsBuilder) { base.OnConfiguring(optionsBuilder); }

        protected override void OnModelCreating(ModelBuilder modelBuilder)
        {
            modelBuilder.Entity<Admin>(entity => {
                entity.ToTable("admins");
                entity.HasKey(e => e.Id);
                entity.Property(e => e.Id).HasColumnName("id").IsRequired(true).ValueGeneratedOnAdd();
                entity.Property(e => e.CommonName).HasColumnName("common_name").IsRequired(true).HasMaxLength(100);
                entity.Property(e => e.Label).HasColumnName("label").IsRequired(true).HasMaxLength(200);
                entity.Property(e => e.Active).HasColumnName("active").IsRequired(true).HasDefaultValue(true);
                entity.Property(e => e.CreatedAt).HasColumnName("created_at").IsRequired(true).HasDefaultValueSql("CURRENT_TIMESTAMP");
            });
            
            base.OnModelCreating(modelBuilder);
        }
    }
}