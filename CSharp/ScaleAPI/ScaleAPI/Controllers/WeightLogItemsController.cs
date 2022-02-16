#nullable disable
using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using Microsoft.AspNetCore.Http;
using Microsoft.AspNetCore.Mvc;
using Microsoft.EntityFrameworkCore;
using ScaleAPI.Data;
using ScaleAPI.Models.Db;

namespace ScaleAPI.Controllers
{
    [Route("api/[controller]")]
    [ApiController]
    public class WeightLogItemsController : ControllerBase
    {
        private readonly AppDbContext _context;

        public WeightLogItemsController(AppDbContext context)
        {
            _context = context;
        }

        // GET: api/WeightLogItems
        [HttpGet]
        public async Task<ActionResult<IEnumerable<WeightLogItemDb>>> GetWeightLogItems()
        {
            return await _context.WeightLogItems.ToListAsync();
        }

        // GET: api/WeightLogItems/5
        [HttpGet("{id}")]
        public async Task<ActionResult<WeightLogItemDb>> GetWeightLogItem(int id)
        {
            var weightLogItem = await _context.WeightLogItems.FindAsync(id);

            if (weightLogItem == null)
            {
                return NotFound();
            }

            return weightLogItem;
        }

        // PUT: api/WeightLogItems/5
        // To protect from overposting attacks, see https://go.microsoft.com/fwlink/?linkid=2123754
        [HttpPut("{id}")]
        public async Task<IActionResult> PutWeightLogItem(int id, WeightLogItemDb weightLogItem)
        {
            if (id != weightLogItem.Id)
            {
                return BadRequest();
            }

            _context.Entry(weightLogItem).State = EntityState.Modified;

            try
            {
                await _context.SaveChangesAsync();
            }
            catch (DbUpdateConcurrencyException)
            {
                if (!WeightLogItemExists(id))
                {
                    return NotFound();
                }
                else
                {
                    throw;
                }
            }

            return NoContent();
        }

        // POST: api/WeightLogItems
        // To protect from overposting attacks, see https://go.microsoft.com/fwlink/?linkid=2123754
        [HttpPost]
        public async Task<ActionResult<WeightLogItemDb>> PostWeightLogItem(WeightLogItemDb weightLogItem)
        {
            _context.WeightLogItems.Add(weightLogItem);
            await _context.SaveChangesAsync();

            return CreatedAtAction(nameof(GetWeightLogItem), new { id = weightLogItem.Id }, weightLogItem);
        }

        // DELETE: api/WeightLogItems/5
        [HttpDelete("{id}")]
        public async Task<IActionResult> DeleteWeightLogItem(int id)
        {
            var weightLogItem = await _context.WeightLogItems.FindAsync(id);
            if (weightLogItem == null)
            {
                return NotFound();
            }

            _context.WeightLogItems.Remove(weightLogItem);
            await _context.SaveChangesAsync();

            return NoContent();
        }

        private bool WeightLogItemExists(int id)
        {
            return _context.WeightLogItems.Any(e => e.Id == id);
        }
    }
}
