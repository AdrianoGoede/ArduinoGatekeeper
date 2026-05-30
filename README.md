# ArduinoGatekeeper

A self-hosted RFID access control system built around MQTT. Door clients run on Arduino UNO R4 WiFi boards and report activity to a central Mosquitto broker, while a Qt desktop application provides administration and monitoring.

---

## Architecture

```
                        ┌──────────────────────┐
                        │   Mosquitto Broker   │
                        │                      │
          TLS + creds   │   :8883  │  :8884    │       mTLS
          ┌─────────────┤          │           ├────────────┐
          │             └──────────────────────┘            │
          │                                                 │
┌─────────▼─────────────┐                      ┌────────────▼───────────┐
│   ArduinoGateKeeper   │                      │ ArduinoGatekeeperAdmin │
│ (Arduino UNO r4 WiFi) │                      │     (Qt Desktop App)   │
└───────────────────────┘                      └────────────────────────┘
```

Each Arduino door client connects on port **8883** using TLS and username/password credentials. The admin application connects on port **8884** using mutual TLS, authenticating exclusively via client certificate.

---

## Modules

### ArduinoGateKeeper
Firmware for the Arduino UNO R4 WiFi door client. Operates an RFID reader to scan cards and performs local access verification against an authorized user list received from the broker. Blinks a green or red LED to indicate access granted or denied, and reports all activity back to the broker via MQTT.

See `ArduinoGateKeeper/README.md` for setup and configuration.

### ArduinoGatekeeperAdmin
Qt desktop application for system administration. Displays a real-time dashboard showing the status and activity of all connected door clients, and allows the admin to add or remove authorized RFID users.

See `ArduinoGatekeeperAdmin/README.md` for setup and configuration.

### MosquittoServerDummy
A pre-configured Mosquitto MQTT broker Docker image intended for local development and testing. Ships with a dual-listener setup, ACL rules, and a certificate generation script. Not intended for production use.

See `MosquittoServerDummy/README.md` for setup and configuration.

---

## MQTT Topic Structure

| Topic | Publisher | Subscriber |
|-------|-----------|------------|
| `ArduinoGatekeeper/Admin/UserList` | Admin app | Door clients |
| `ArduinoGatekeeper/{id}/Status` | Door client | Admin app |
| `ArduinoGatekeeper/{id}/Log` | Door client | Admin app |

Door clients subscribe to `UserList` on startup and maintain a local copy to perform access verification on-device. This ensures doors remain operational even if the broker or admin app is temporarily unreachable.

---

## Security Model

| Layer | Mechanism |
|-------|-----------|
| Transport | TLS (all connections) |
| Door client auth | Username + password per device |
| Admin auth | Mutual TLS — client certificate required |
| Topic isolation | MQTT ACL — each door can only publish to its own topics |

The CA certificate and admin client certificate are generated locally via `MosquittoServerDummy/generate.sh` before the broker image is built. Private keys for the CA and admin certificate are never copied into the Docker image and should never be committed to version control.

---

## Getting Started

1. Generate certificates and build the broker:
   ```bash
   cd MosquittoServerDummy
   ./generate.sh
   docker build -t arduinogatekeeper-broker .
   docker run -d --name mosquitto -p 8883:8883 -p 8884:8884 arduinogatekeeper-broker
   ```

2. Flash the Arduino firmware — see `ArduinoGateKeeper/README.md`.

3. Run the admin application — see `ArduinoGatekeeperAdmin/README.md`.
