using ArduinoGatekeeperBackend.Services.Interfaces;
using Microsoft.AspNetCore.Mvc;
using Microsoft.AspNetCore.OData.Formatter;
using Microsoft.AspNetCore.OData.Query;
using Microsoft.AspNetCore.OData.Routing.Controllers;

namespace ArduinoGatekeeperBackend.Controllers
{
    public class DoorLogsController : ODataController
    {
        private readonly IDoorLogsService _logsService;

        public DoorLogsController(IDoorLogsService logsService)
        {
            _logsService = logsService ?? throw new ArgumentNullException(nameof(logsService));
        }

        [EnableQuery]
        public IActionResult Get() => Ok(_logsService.GetAll());

        [EnableQuery]
        public async Task<IActionResult> Get([FromODataUri] long key)
        {
            var result = await _logsService.GetByIdAsync(key);
            return (result is not null ? Ok(result) : NotFound());
        }
    }
}