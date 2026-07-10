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
#define MQTT_BROKER_ADDRESS             ""
#define MQTT_BROKER_PORT                8883
#define MQTT_DEVICE_ID                  ""
#define MQTT_QOS_LEVEL                  2
#define MQTT_KEEP_ALIVE_INTERVAL        5000

// NTP
#define NTP_SERVER_PRIMARY              "europe.pool.ntp.org"
#define NTP_SERVER_SECONDARY            "pool.ntp.org"
#define TIME_OFFSET                     0
#define TIME_RESYNC_INTERVAL            3600000

// MQTT
#define ACTIVITY_LOG_TOPIC              ""
#define DEVICE_STATUS_TOPIC             ""
#define AUTHORIZED_USERS_TOPIC          ""

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