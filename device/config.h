#ifndef __ARDUINO_GATEKEEPER_CONFIG__
#define __ARDUINO_GATEKEEPER_CONFIG__

// Output
#define SERIAL_BAUD_RATE                9600
#define STATUS_LED_INTERVAL             1000

// Internal params
#define STRING_BUFFER_SIZE              200
#define NETWORK_TASK_PRIORITY           1
#define NETWORK_TASK_STACK_DEPTH        512

// Network
#define WIFI_SSID                       ""
#define WIFI_PASSWORD                   ""

// NTP
#define NTP_SERVER_PRIMARY              "europe.pool.ntp.org"
#define NTP_SERVER_SECONDARY            "pool.ntp.org"

// MQTT
#define MQTT_BROKER_ADDRESS             ""
#define MQTT_BROKER_PORT                8883
#define MQTT_DEVICE_ID                  ""
#define MQTT_QOS_LEVEL                  2
#define MQTT_KEEP_ALIVE_INTERVAL        10000
#define SCAN_TOPIC                      "AGK/+/scan"
#define DEVICE_STATUS_TOPIC             "AGK/+/dev_status"
#define ADD_USER_TOPIC                  "AGK/users/add"
#define REMOVE_USER_TOPIC               "AGK/users/remove"

// mTLS
const char CA_CERT[] PROGMEM = R"EOF(
  ca.crt payload here!!
)EOF";

const char CLIENT_CERT[] PROGMEM = R"EOF(
  device.crt payload here!!
)EOF";

const char CLIENT_KEY[] PROGMEM = R"EOF(
  device.key payload here!!
)EOF";

#endif