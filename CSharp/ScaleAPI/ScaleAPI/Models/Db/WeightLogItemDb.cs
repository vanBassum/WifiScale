using System.ComponentModel.DataAnnotations;

namespace ScaleAPI.Models.Db
{
    public class WeightLogItemDb
    {
        [Key]
        public int Id { get; set; }
        public int RawWeight { get; set; }
        public double Weight { get; set; }
        public DateTime Timestamp { get; set; }
        public DateTime TimestampUc { get; set; }
    }
}
