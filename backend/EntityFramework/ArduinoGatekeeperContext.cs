using ArduinoGatekeeperBackend.EntityFramework.Models;
using Microsoft.EntityFrameworkCore;

namespace ArduinoGatekeeperBackend.EntityFramework
{
    public class ArduinoGatekeeperContext : DbContext
    {
        public DbSet<Admin> Admins { get; set; }
        public DbSet<User> Users { get; set; }
        public DbSet<Door> Doors { get; set; }
        public DbSet<Permission> Permissions { get; set; }
        public DbSet<AccessLog> AccessLogs { get; set; }
        public DbSet<DoorLog> DoorLogs { get; set; }
        
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
                entity.Property(e => e.CardKey).HasColumnName("card_key").IsRequired(true);
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

            modelBuilder.Entity<Permission>(entity => {
                entity.ToTable("permissions");
                entity.HasKey(e => new { e.UserId, e.DoorId });
                entity.Property(e => e.UserId).HasColumnName("user_id").IsRequired(true);
                entity.Property(e => e.DoorId).HasColumnName("door_id").IsRequired(true);
                entity.Property(e => e.CreatedAt).HasColumnName("created_at").IsRequired(true).HasDefaultValueSql("CURRENT_TIMESTAMP");
                entity.HasOne(e => e.User).WithMany(o => o.Permissions).HasForeignKey(e => e.UserId).OnDelete(DeleteBehavior.NoAction);
                entity.HasOne(e => e.Door).WithMany(o => o.Permissions).HasForeignKey(e => e.DoorId).OnDelete(DeleteBehavior.NoAction);
            });

            modelBuilder.Entity<AccessLog>(entity => {
                entity.ToTable("access_log");
                entity.HasKey(e => e.Id);
                entity.Property(e => e.Id).HasColumnName("id").IsRequired(true).ValueGeneratedOnAdd();
                entity.Property(e => e.UserId).HasColumnName("user_id").IsRequired(true);
                entity.Property(e => e.DoorId).HasColumnName("door_id").IsRequired(true);
                entity.Property(e => e.Granted).HasColumnName("granted").IsRequired(true);
                entity.Property(e => e.CreatedAt).HasColumnName("created_at").IsRequired(true).HasDefaultValueSql("CURRENT_TIMESTAMP");
                entity.HasOne(e => e.User).WithMany(o => o.AccessLogs).HasForeignKey(e => e.UserId).OnDelete(DeleteBehavior.NoAction);
                entity.HasOne(e => e.Door).WithMany(o => o.AccessLogs).HasForeignKey(e => e.DoorId).OnDelete(DeleteBehavior.NoAction);
            });

            modelBuilder.Entity<DoorLog>(entity => {
                entity.ToTable("door_log");
                entity.HasKey(e => e.Id);
                entity.Property(e => e.Id).HasColumnName("id").IsRequired(true).ValueGeneratedOnAdd();
                entity.Property(e => e.DoorId).HasColumnName("door_id").IsRequired(true);
                entity.Property(e => e.Online).HasColumnName("online").IsRequired(true);
                entity.Property(e => e.CreatedAt).HasColumnName("created_at").IsRequired(true).HasDefaultValueSql("CURRENT_TIMESTAMP");
                entity.HasOne(e => e.Door).WithMany(o => o.DoorLogs).HasForeignKey(e => e.DoorId).OnDelete(DeleteBehavior.NoAction);
            });
            
            base.OnModelCreating(modelBuilder);
        }
    }
}