using Microsoft.EntityFrameworkCore;
using ScaleApi.Models.Db;

namespace ScaleApi.Data
{
    public class AppDbContext : DbContext
    {
        public AppDbContext(DbContextOptions<AppDbContext> options) : base(options)
        {
        }

        public DbSet<WeightMeasurement> Measurements { get; set; }
        public DbSet<PowerMeasurement> PowerMeasurements { get; set; }
    }
}
