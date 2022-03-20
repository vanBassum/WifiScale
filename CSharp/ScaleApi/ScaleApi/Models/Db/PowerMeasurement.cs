using System;
using System.ComponentModel.DataAnnotations;

namespace ScaleApi.Models.Db
{
    public class PowerMeasurement
    {
        [Key]
        public int Id { get; set; }
        public DateTime ReceivedTimestamp { get; set; }

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
