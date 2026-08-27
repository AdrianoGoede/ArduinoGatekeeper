using ArduinoGatekeeperBackend.Services.Interfaces;
using Microsoft.AspNetCore.Mvc;
using Microsoft.AspNetCore.OData.Formatter;
using Microsoft.AspNetCore.OData.Query;
using Microsoft.AspNetCore.OData.Routing.Controllers;

namespace ArduinoGatekeeperBackend.Controllers
{
    public class DoorsController : ODataController
    {
        private readonly IDoorsService _doorsService;

        public DoorsController(IDoorsService doorsService)
        {
            _doorsService = doorsService ?? throw new ArgumentNullException(nameof(doorsService));
        }

        [EnableQuery]
        public IActionResult Get() => Ok(_doorsService.GetAll());

        [EnableQuery]
        public async Task<IActionResult> Get([FromODataUri] int key)
        {
            var result = await _doorsService.GetByIdAsync(key);
            return (result is not null ? Ok(result) : NotFound());
        }

        [HttpPost]
        public async Task<IActionResult> Post([FromBody] DoorDTO door)
        {
            try
            {
                var newDoor = await _doorsService.CreateAsync(door);
                return Created($"api/Doors({newDoor.Id})", newDoor);
            }
            catch (ArgumentException ex) { return BadRequest(ex.Message); }
        }

        [HttpPatch]
        public async Task<IActionResult> Patch([FromODataUri] int key, [FromBody] DoorDTO modified)
        {
            try
            {
                var result = await _doorsService.UpdateAsync(key, modified);
                return Accepted(result);
            }
            catch (ArgumentException ex) { return BadRequest(ex.Message); }
        }

        [HttpDelete]
        public async Task<IActionResult> Delete([FromODataUri] int key)
        {
            try
            {
                await _doorsService.DeleteAsync(key);
                return NoContent();
            }
            catch (ArgumentException ex) { return BadRequest(ex.Message); }
        }
    }
}