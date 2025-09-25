#define SERIAL_BAUD_RATE                115200
#define STATUS_LED_INTERVAL             1000
#define ACCESS_DENIED_LED_PIN           2
#define ACCESS_GRANTED_LED_PIN          4
#define RFID_SS_PIN                     10
#define RFID_RST_PIN                    9

#define STRING_BUFFER_SIZE              200
#define NETWORK_TASK_PRIORITY           1
#define NETWORK_TASK_STACK_DEPTH        512
#define ACCESS_CONTROL_TASK_PRIORITY    3
#define ACCESS_CONTROL_TASK_STACK_DEPTH 512

#define WIFI_SSID                       "ZTE_2.4G_3RDgHj"
#define WIFI_PASSWORD                   "eWiTGqhn"
#define NTP_SERVER_URL                  "europe.pool.ntp.org"
#define MQTT_BROKER_ADDRESS             "192.168.1.11"
#define MQTT_BROKER_PORT                1883

#define TIME_OFFSET                     0
#define TIME_RESYNC_INTERVAL            3600000

#define MQTT_QOS_LEVEL                  1
#define ACC_CONTROL_OPERATION_INTERVAL  20
#define NETWORK_OPERATION_INTERVAL      20
#define ACTIVITY_LOG_BUFFER_SIZE        25
#define ACTIVITY_LOG_TOPIC              "ArduinoGatekeeper/Activity"
#define AUTHORIZED_USERS_ADD_TOPIC      "ArduinoGatekeeper/AuthUsers/Add"
#define AUTHORIZED_USERS_REMOVE_TOPIC   "ArduinoGatekeeper/AuthUsers/Remove"