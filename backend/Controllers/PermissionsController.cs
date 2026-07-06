using ArduinoGatekeeperBackend.Services.Interfaces;
using Microsoft.AspNetCore.Mvc;
using Microsoft.AspNetCore.OData.Formatter;
using Microsoft.AspNetCore.OData.Query;
using Microsoft.AspNetCore.OData.Routing.Controllers;

namespace ArduinoGatekeeperBackend.Controllers
{
    public class PermissionsController : ODataController
    {
        private readonly IPermissionsService _permissionsService;

        public PermissionsController(IPermissionsService permissionsService)
        {
            _permissionsService = permissionsService ?? throw new ArgumentNullException(nameof(permissionsService));
        }

        [EnableQuery]
        public IActionResult Get() => Ok(_permissionsService.GetAll());

        [EnableQuery]
        [HttpGet("api/Permissions(UserId={keyUserId},DoorId={keyDoorId})")]
        public async Task<IActionResult> Get([FromODataUri] int keyUserId, [FromODataUri] int keyDoorId)
        {
            var result = await _permissionsService.GetByUserAndDoorIdAsync(keyUserId, keyDoorId);
            return (result is not null ? Ok(result) : NotFound());
        }

        [HttpPost]
        public async Task<IActionResult> Post([FromBody] PermissionDTO permission)
        {
            try
            {
                var newPermission = await _permissionsService.CreateAsync(permission);
                return Created($"api/Users({newPermission.UserId},{newPermission.DoorId})", newPermission);
            }
            catch (ArgumentException ex) { return BadRequest(ex.Message); }
        }

        [HttpDelete("api/Permissions(UserId={keyUserId},DoorId={keyDoorId})")]
        public async Task<IActionResult> Delete([FromODataUri] int keyUserId, [FromODataUri] int keyDoorId)
        {
            try
            {
                await _permissionsService.DeleteAsync(keyUserId, keyDoorId);
                return NoContent();
            }
            catch (ArgumentException ex) { return BadRequest(ex.Message); }
        }
    }
}