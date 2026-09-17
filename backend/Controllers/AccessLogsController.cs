using ArduinoGatekeeperBackend.EntityFramework.Models;
using ArduinoGatekeeperBackend.Services.Interfaces;
using Microsoft.AspNetCore.OData.Formatter;
using Microsoft.AspNetCore.OData.Query;
using Microsoft.AspNetCore.OData.Results;
using Microsoft.AspNetCore.OData.Routing.Controllers;

namespace ArduinoGatekeeperBackend.Controllers
{
    public class AccessLogsController : ODataController
    {
        private readonly IAccessLogsService _logsService;

        public AccessLogsController(IAccessLogsService logsService)
        {
            _logsService = logsService ?? throw new ArgumentNullException(nameof(logsService));
        }

        [EnableQuery]
        public IQueryable<AccessLog> Get() => _logsService.GetAll();

        [EnableQuery]
        public SingleResult<AccessLog?> Get([FromODataUri] long key) => SingleResult.Create<AccessLog?>(_logsService.GetByIdAsync(key));
    }
}