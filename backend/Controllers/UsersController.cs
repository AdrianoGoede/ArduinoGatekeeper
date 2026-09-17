using ArduinoGatekeeperBackend.EntityFramework.Models;
using ArduinoGatekeeperBackend.Services.Interfaces;
using Microsoft.AspNetCore.Mvc;
using Microsoft.AspNetCore.OData.Formatter;
using Microsoft.AspNetCore.OData.Query;
using Microsoft.AspNetCore.OData.Results;
using Microsoft.AspNetCore.OData.Routing.Controllers;

namespace ArduinoGatekeeperBackend.Controllers
{
    public class UsersController : ODataController
    {
        private readonly IUsersService _usersService;

        public UsersController(IUsersService usersService)
        {
            _usersService = usersService ?? throw new ArgumentNullException(nameof(usersService));
        }

        [EnableQuery]
        public IQueryable<User> Get() => _usersService.GetAll();

        [EnableQuery]
        public SingleResult<User?> Get([FromODataUri] int key) => SingleResult.Create<User?>(_usersService.GetByIdAsync(key));

        [HttpPost]
        public async Task<IActionResult> Post([FromBody] UserDTO user)
        {
            try
            {
                var newUser = await _usersService.CreateAsync(user);
                return Created($"api/Users({newUser.Id})", newUser);
            }
            catch (ArgumentException ex) { return BadRequest(ex.Message); }
        }

        [HttpPatch]
        public async Task<IActionResult> Patch([FromODataUri] int key, [FromBody] UserDTO modified)
        {
            try
            {
                var result = await _usersService.UpdateAsync(key, modified);
                return Accepted(result);
            }
            catch (ArgumentException ex) { return BadRequest(ex.Message); }
        }

        [HttpDelete]
        public async Task<IActionResult> Delete([FromODataUri] int key)
        {
            try
            {
                await _usersService.DeleteAsync(key);
                return NoContent();
            }
            catch (ArgumentException ex) { return BadRequest(ex.Message); }
        }
    }
}