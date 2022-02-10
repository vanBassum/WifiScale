using Microsoft.AspNetCore.Mvc;
using Microsoft.EntityFrameworkCore;
using ScaleApi.Data;
using ScaleApi.Models.Db;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace ScaleApi.Controllers
{

    [Route("api/[controller]")]
    [ApiController]
    public class WeightController : Controller
    {

        AppDbContext _context;
        public WeightController(AppDbContext context)
        {
            _context = context;
        }

        // GET: api/TodoItems
        [HttpGet]
        public async Task<ActionResult<IEnumerable<WeightMeasurement>>> GetMeasurements()
        {
            return await _context.Measurements
                .Select(x => x)
                .ToListAsync();
        }

        // GET: api/TodoItems/5
        [HttpGet("{id}")]
        public async Task<ActionResult<WeightMeasurement>> GetMeasurement(int id)
        {
            var todoItem = await _context.Measurements.FindAsync(id);

            if (todoItem == null)
            {
                return NotFound();
            }

            return todoItem;
        }

        // POST: api/TodoItems
        // To protect from overposting attacks, see https://go.microsoft.com/fwlink/?linkid=2123754
        [HttpPost]
        public async Task<ActionResult<WeightMeasurement>> CreateMeasurement(WeightMeasurement measurement)
        {

            _context.Measurements.Add(measurement);
            await _context.SaveChangesAsync();

            return measurement;
        }

        // DELETE: api/TodoItems/5
        [HttpDelete("{id}")]
        public async Task<IActionResult> DeleteMeasurement(int id)
        {
            var todoItem = await _context.Measurements.FindAsync(id);

            if (todoItem == null)
            {
                return NotFound();
            }

            _context.Measurements.Remove(todoItem);
            await _context.SaveChangesAsync();

            return NoContent();
        }
    }
}
