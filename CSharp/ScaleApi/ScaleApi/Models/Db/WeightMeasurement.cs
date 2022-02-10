using System;
using System.ComponentModel.DataAnnotations;

namespace ScaleApi.Models.Db
{
    public class WeightMeasurement
    {
        [Key]
        public int Id { get; set; }
        public int RawWeight { get; set; }
        public double Weight { get; set; }
        public DateTime MeasurementTimestamp { get; set; }
        public DateTime ReceivedTimestamp { get; set; }
    }
}
