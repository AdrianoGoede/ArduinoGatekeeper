# Device

Arduino Nano ESP32 firmware for the ArduinoGatekeeper door client devices. Each device operates an MFRC522 RFID reader, performs local access verification, and reports activity to the MQTT broker.

---

## Overview

The firmware runs two concurrent FreeRTOS tasks on dedicated cores. The RFID task handles all card scanning and access verification entirely locally — it never waits on the network. The network task maintains the broker connection and handles all MQTT communication independently. The two tasks communicate through thread-safe shared data structures.

This design ensures that a slow network condition or broker reconnection never causes a missed card scan.

---

## Hardware

| Component | Details |
|---|---|
| Board | Arduino Nano ESP32 |
| RFID reader | MFRC522 (SPI) |
| Access indicator | Green LED (granted), Red LED (denied) |

---

## FreeRTOS Tasks

### RFID Task — Core 1
Polls the MFRC522 reader continuously. On a card scan:

1. Reads the card UID
2. Looks up the UID in the authorized users map
3. Authenticates the card against its stored MIFARE key
4. Blinks green or red LED based on the result
5. Pushes a log entry to the scan queue for the network task to publish

The RFID task never touches the network and is not affected by connectivity issues.

### Network Task — Core 0
Manages all network communication:

1. Maintains WiFi and MQTT broker connections, reconnecting automatically on failure
2. Polls the broker for incoming messages
3. Processes incoming `add_users` messages to update the authorized users map
4. Drains the scan queue and publishes each pending log entry to the broker
5. Publishes `dev_status` online/offline messages on connect and disconnect

---

## Authorized Users Map

The authorized users list is kept in memory as a `std::map` keyed by card UID, with the corresponding MIFARE sector key as the value. Access to the map is protected by a FreeRTOS mutex — the RFID task reads from it during authentication while the network task writes to it when user list chunks arrive.

The list is populated via paginated MQTT messages on startup and whenever the backend sends an update. If a chunk sequence is not fully received within a configurable timeout, the device re-requests the full list via the `req_users` topic.

---

## Scan Queue

Scan results are pushed to a FreeRTOS queue by the RFID task immediately after each card read. The network task drains the queue on every loop iteration, serializing each entry to JSON and publishing it to the broker. If the broker is temporarily unreachable, entries remain in the queue until connectivity is restored.

Each log entry contains:

| Field | Description |
|---|---|
| `CardId` | Card UID as a hex string |
| `DeviceId` | This device's identifier (e.g. `Agk_1`) |
| `Granted` | Whether access was granted |
| `Timestamp` | Unix timestamp (seconds, NTP-synced) |

---

## MQTT

Connects to the broker on port **8883** using mTLS. Each device has a unique client certificate with CN matching its device identifier (e.g. `Agk_1`).

### Subscriptions

| Topic | Description |
|---|---|
| `AGK/{id}/add_users` | Receive authorized user list chunks from the backend |
| `AGK/{id}/remove_users` | Receive user list chunks from the backend to remove from authorized list |

### Publications

| Topic | Description |
|---|---|
| `AGK/{id}/dev_status` | Online/offline status on connect and disconnect, upon connection publishes MQTT last will message to inform of disconnection |
| `AGK/{id}/scan` | Card scan log entry |

---

## Configuration

All device-specific settings are defined in `config.h`:

| Setting | Description |
|---|---|
| `WIFI_SSID` / `WIFI_PASSWORD` | WiFi credentials |
| `MQTT_BROKER_ADDRESS` | Broker hostname or IP |
| `MQTT_BROKER_PORT` | Broker port (default `8883`) |
| `MQTT_DEVICE_ID` | Unique device identifier (e.g. `Agk_1`) |
| `CA_CERT` | PEM-encoded CA certificate |
| `CLIENT_CERT` | PEM-encoded device client certificate |
| `CLIENT_KEY` | PEM-encoded device private key |
| `RFID_LOG_QUEUE_MAX_SIZE` | Maximum pending log entries before oldest is dropped |
| `USER_LIST_TIMEOUT_MS` | Timeout before re-requesting the user list |

> **Certificates are embedded directly in `config.h` as PEM string literals and compiled into the firmware binary.**

---

## Dependencies

All available via the Arduino Library Manager:

| Library | Purpose |
|---|---|
| `MFRC522` | RFID reader driver |
| `ArduinoMqttClient` | MQTT client |
| `ArduinoJson` | JSON serialization / deserialization |
| `FreeRTOS` | Built into the `arduino-esp32` core — no install needed |
| `NTP` | Built into the ESP-IDF — no install needed |

---

## Setup

1. Install dependencies via the Arduino Library Manager
2. Copy `config.h.example` to `config.h` and fill in WiFi credentials, broker address, device ID, and certificates
3. Flash to the board via the Arduino IDE or `arduino-cli`

Each board must be flashed with a unique `MQTT_DEVICE_ID` and a corresponding unique client certificate issued by the project CA.
