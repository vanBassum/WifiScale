using Microsoft.AspNetCore.Mvc;
using ScaleApi.Data;
using System.Threading.Tasks;

namespace ScaleApi.Controllers
{
    [Route("api/[controller]")]
    [ApiController]
    public class VersionController : Controller
    {

        AppDbContext _context;
        public VersionController(AppDbContext context)
        {
            _context = context;
        }

        // GET: api/TodoItems
        [HttpGet]
        public ActionResult<string> GetVersion()
        {
            return AppVersion.Version;
        }
    }
}
