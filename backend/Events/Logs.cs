using ArduinoGatekeeperBackend.EntityFramework.Models;
using ArduinoGatekeeperBackend.Mqtt.Models;

namespace ArduinoGatekeeperBackend.Events
{
    public record DoorScanReceivedEvent(string DeviceId, Scan Scan);
    public record DoorStatusReceivedEvent(string DeviceId, DeviceStatus DeviceStatus);
    public record DoorScanRecordedEvent(AccessLog AccessLog);
    public record DoorStatusRecordedEvent(DoorLog DoorLog);
}