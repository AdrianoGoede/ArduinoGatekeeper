#ifndef __ARDUINO_GATEKEEPER_CONFIG__
#define __ARDUINO_GATEKEEPER_CONFIG__

// Output
#define SERIAL_BAUD_RATE                9600
#define STATUS_LED_INTERVAL             1000
#define ACCESS_DENIED_LED_PIN           A0
#define ACCESS_GRANTED_LED_PIN          A5

// Internal params
#define STRING_BUFFER_SIZE              200

// Network task params
#define NETWORK_TASK_PRIORITY           1
#define NETWORK_TASK_STACK_DEPTH        8192
#define NETWORK_TASK_DELAY              10

// RFID task params
#define RFID_TASK_PRIORITY              3
#define RFID_TASK_STACK_DEPTH           4096
#define RFID_TASK_DELAY                 10
#define RFID_CS_PIN                     D10
#define RFID_RST_PIN                    D5
#define RFID_LOG_QUEUE_MAX_SIZE         50

// WiFi
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
#define ADD_USERS_TOPIC                 "AGK/+/add_users"
#define REMOVE_USERS_TOPIC              "AGK/+/remove_users"

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