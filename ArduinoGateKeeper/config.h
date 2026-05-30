#ifndef __ARDUINO_GATEKEEPER_CONFIG__
#define __ARDUINO_GATEKEEPER_CONFIG__

#define SERIAL_BAUD_RATE                9600
#define STATUS_LED_INTERVAL             1000
#define ACCESS_DENIED_LED_PIN           2
#define ACCESS_GRANTED_LED_PIN          4
#define RFID_CS_PIN                     10
#define RFID_RST_PIN                    9

#define STRING_BUFFER_SIZE              200
#define NETWORK_TASK_PRIORITY           1
#define NETWORK_TASK_STACK_DEPTH        512
#define ACCESS_CONTROL_TASK_PRIORITY    3
#define ACCESS_CONTROL_TASK_STACK_DEPTH 512

#define WIFI_SSID                       ""
#define WIFI_PASSWORD                   ""
#define NTP_SERVER_URL                  "europe.pool.ntp.org"
#define MQTT_BROKER_ADDRESS             ""
#define MQTT_BROKER_PORT                8883
#define MQTT_DEVICE_ID                  "Agk_1"
#define MQTT_USERNAME                   MQTT_DEVICE_ID
#define MQTT_PASSWORD                   ""
#define MQTT_QOS_LEVEL                  1
#define MQTT_KEEP_ALIVE_INTERVAL        5000

#define TIME_OFFSET                     0
#define TIME_RESYNC_INTERVAL            3600000

#define ACC_CONTROL_OPERATION_INTERVAL  10
#define NETWORK_OPERATION_INTERVAL      10
#define ACTIVITY_LOG_BUFFER_SIZE        25
#define ACTIVITY_LOG_TOPIC              "ArduinoGatekeeper/%s/Log"
#define DEVICE_STATUS_TOPIC             "ArduinoGatekeeper/%s/Status"
#define AUTHORIZED_USERS_TOPIC          "ArduinoGatekeeper/Admin/UserList"

const char BROKER_CA_CERT[] PROGMEM = R"EOF(

)EOF";

#endif