using Microsoft.AspNetCore.Mvc;
using ScaleApi.Data;
using ScaleApi.Models.Db;
using System.Threading.Tasks;

namespace ScaleApi.Controllers
{
    [Route("api/[controller]")]
    [ApiController]
    public class PowerController : Controller
    {

        AppDbContext _context;
        public PowerController(AppDbContext context)
        {
            _context = context;
        }

       
        // POST: api/TodoItems
        // To protect from overposting attacks, see https://go.microsoft.com/fwlink/?linkid=2123754
        [HttpPost]
        public async Task<ActionResult<PowerMeasurement>> CreateMeasurement(PowerMeasurement measurement)
        {

            //_context.Measurements.Add(measurement);
            //await _context.SaveChangesAsync();

            return measurement;
        }
    }
}
