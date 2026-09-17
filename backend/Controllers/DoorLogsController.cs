using ArduinoGatekeeperBackend.EntityFramework.Models;
using ArduinoGatekeeperBackend.Services.Interfaces;
using Microsoft.AspNetCore.OData.Formatter;
using Microsoft.AspNetCore.OData.Query;
using Microsoft.AspNetCore.OData.Results;
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
        public IQueryable<DoorLog> Get() => _logsService.GetAll();

        [EnableQuery]
        public SingleResult<DoorLog?> Get([FromODataUri] long key) => SingleResult.Create<DoorLog?>(_logsService.GetByIdAsync(key));
    }
}