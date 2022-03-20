using Microsoft.Extensions.DependencyInjection;
using ScaleApi.Data;
using ScaleApi.Models.Db;
using System;

namespace ScaleApi.Models.Server.Commands
{
    class PostPower : Command
    {
        public PowerData Data { get; set; }

        public override Reply Execute(IServiceScopeFactory scopeFactory)
        {
            Reply reply = new Reply();
            using (var scope = scopeFactory.CreateScope())
            {
                var dbContext = scope.ServiceProvider.GetRequiredService<AppDbContext>();
                PowerMeasurement powerMeasurement = new PowerMeasurement
                {
                    ReceivedTimestamp = DateTime.Now,
                    ActualConsumption = Data.ActualConsumption,
                    ActualProduction = Data.ActualProduction,
                    ActualTarrif = Data.ActualTarrif,
                    TimeStamp = Data.TimeStamp,
                    TotalConsumption1 = Data.TotalConsumption1,
                    TotalConsumption2 = Data.TotalConsumption2,
                    TotalGasConsumption = Data.TotalGasConsumption,
                    TotalProduction1 = Data.TotalProduction1,
                    TotalProduction2 = Data.TotalProduction2,
                };


                dbContext.PowerMeasurements.Add(powerMeasurement);
                dbContext.SaveChanges();
                reply.Success = true;
            }
            return reply;
        }
    }

    public class PowerData
    {
        public DateTime TimeStamp { get; set; }
        public float? TotalConsumption1 { get; set; }
        public float? TotalConsumption2 { get; set; }
        public float? TotalProduction1 { get; set; }
        public float? TotalProduction2 { get; set; }
        public float? ActualConsumption { get; set; }
        public float? ActualProduction { get; set; }
        public float? TotalGasConsumption { get; set; }
        public int? ActualTarrif { get; set; }
    }
}
