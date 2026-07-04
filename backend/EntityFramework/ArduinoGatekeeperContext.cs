using ArduinoGatekeeperBackend.EntityFramework.Models;
using Microsoft.EntityFrameworkCore;

namespace ArduinoGatekeeperBackend.EntityFramework
{
    public class ArduinoGatekeeperContext : DbContext
    {
        public DbSet<Admin> Admins { get; set; }
        public DbSet<User> Users { get; set; }
        public DbSet<Door> Doors { get; set; }
        
        public ArduinoGatekeeperContext(DbContextOptions<ArduinoGatekeeperContext> options) : base(options) {}

        protected override void OnConfiguring(DbContextOptionsBuilder optionsBuilder) { base.OnConfiguring(optionsBuilder); }

        protected override void OnModelCreating(ModelBuilder modelBuilder)
        {
            modelBuilder.Entity<Admin>(entity => {
                entity.ToTable("admins");
                entity.HasKey(e => e.Id);
                entity.Property(e => e.Id).HasColumnName("id").IsRequired(true).ValueGeneratedOnAdd();
                entity.Property(e => e.Label).HasColumnName("label").IsRequired(true).HasMaxLength(200);
                entity.Property(e => e.Active).HasColumnName("active").IsRequired(true).HasDefaultValue(true);
                entity.Property(e => e.CreatedAt).HasColumnName("created_at").IsRequired(true).HasDefaultValueSql("CURRENT_TIMESTAMP");
            });

            modelBuilder.Entity<User>(entity => {
                entity.ToTable("users");
                entity.HasKey(e => e.Id);
                entity.Property(e => e.Id).HasColumnName("id").IsRequired(true).ValueGeneratedOnAdd();
                entity.Property(e => e.CardId).HasColumnName("card_id").IsRequired(true).HasMaxLength(10);
                entity.Property(e => e.Label).HasColumnName("label").IsRequired(true).HasMaxLength(200);
                entity.Property(e => e.CreatedAt).HasColumnName("created_at").IsRequired(true).HasDefaultValueSql("CURRENT_TIMESTAMP");
            });

            modelBuilder.Entity<Door>(entity => {
                entity.ToTable("doors");
                entity.HasKey(e => e.Id);
                entity.Property(e => e.Id).HasColumnName("id").IsRequired(true).ValueGeneratedOnAdd();
                entity.Property(e => e.Label).HasColumnName("label").IsRequired(true).HasMaxLength(200);
                entity.Property(e => e.CreatedAt).HasColumnName("created_at").IsRequired(true).HasDefaultValueSql("CURRENT_TIMESTAMP");
            });
            
            base.OnModelCreating(modelBuilder);
        }
    }
}