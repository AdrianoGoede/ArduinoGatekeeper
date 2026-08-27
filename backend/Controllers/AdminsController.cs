using ArduinoGatekeeperBackend.Services.Interfaces;
using Microsoft.AspNetCore.Mvc;
using Microsoft.AspNetCore.OData.Formatter;
using Microsoft.AspNetCore.OData.Query;
using Microsoft.AspNetCore.OData.Routing.Controllers;

namespace ArduinoGatekeeperBackend.Controllers
{
    public class AdminsController : ODataController
    {
        private readonly IAdminsService _adminsService;

        public AdminsController(IAdminsService adminsService)
        {
            _adminsService = adminsService ?? throw new ArgumentNullException(nameof(adminsService));
        }

        [EnableQuery]
        public IActionResult Get() => Ok(_adminsService.GetAll());

        [EnableQuery]
        public async Task<IActionResult> Get([FromODataUri] int key)
        {
            var result = await _adminsService.GetByIdAsync(key);
            return (result is not null ? Ok(result) : NotFound());
        }

        [HttpPost]
        public async Task<IActionResult> Post([FromBody] AdminDTO admin)
        {
            try
            {
                var newAdmin = await _adminsService.CreateAsync(admin);
                return Created($"api/Admins({newAdmin.Id})", newAdmin);
            }
            catch (ArgumentException ex) { return BadRequest(ex.Message); }
        }

        [HttpPatch]
        public async Task<IActionResult> Patch([FromODataUri] int key, [FromBody] AdminDTO modified)
        {
            try
            {
                var result = await _adminsService.UpdateAsync(key, modified);
                return Accepted(result);
            }
            catch (ArgumentException ex) { return BadRequest(ex.Message); }
        }

        [HttpDelete]
        public async Task<IActionResult> Delete([FromODataUri] int key)
        {
            try
            {
                await _adminsService.DeleteAsync(key);
                return NoContent();
            }
            catch (ArgumentException ex) { return BadRequest(ex.Message); }
        }
    }
}