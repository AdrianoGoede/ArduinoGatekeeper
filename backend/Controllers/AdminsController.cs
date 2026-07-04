using ArduinoGatekeeperBackend.EntityFramework;
using ArduinoGatekeeperBackend.EntityFramework.Models;
using Microsoft.AspNetCore.Mvc;
using Microsoft.AspNetCore.OData.Query;
using Microsoft.AspNetCore.OData.Routing.Controllers;
using Microsoft.EntityFrameworkCore;

namespace ArduinoGatekeeperBackend.Controllers
{
    [Route("api/admins")]
    public class AdminsController : ODataController
    {
        private readonly ArduinoGatekeeperContext _dbContext;

        public AdminsController(ArduinoGatekeeperContext dbContext)
        {
            _dbContext = dbContext ?? throw new ArgumentNullException(nameof(dbContext));
        }

        [HttpGet]
        [EnableQuery]
        public IQueryable<Admin> Get() => _dbContext.Admins.AsNoTracking().AsQueryable();
    }
}