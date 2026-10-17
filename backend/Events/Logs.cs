using ArduinoGatekeeperBackend.Mqtt.Models;

namespace ArduinoGatekeeperBackend.Events
{
    public record DoorScanEvent(string DeviceId, Scan Scan);
    public record DoorStatusEvent(string DeviceId, DeviceStatus DeviceStatus);
}