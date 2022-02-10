using Microsoft.Extensions.DependencyInjection;
using ScaleApi.Data;
using ScaleApi.Models.Db;
using System;

namespace ScaleApi.Models.Server.Commands
{
    class PostMeasurement : Command
    {
        public int? RawWeight { get; set; }
        public double? Weight { get; set; }
        public DateTime? MeasurementTimestamp { get; set; }

        public override Reply Execute(IServiceScopeFactory scopeFactory)
        {
            Reply reply = new Reply();
            using (var scope = scopeFactory.CreateScope())
            {
                var dbContext = scope.ServiceProvider.GetRequiredService<AppDbContext>();
                WeightMeasurement measurement = new WeightMeasurement
                {
                    RawWeight = RawWeight ?? 0,
                    Weight = Weight ?? 0,
                    MeasurementTimestamp = MeasurementTimestamp ?? DateTime.MinValue,
                    ReceivedTimestamp = DateTime.Now
                };
                dbContext.Measurements.Add(measurement);
                reply.Success = true;
            }
            return reply;
        }
    }
}
