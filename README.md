# ArduinoGatekeeper

A self-hosted RFID-based access control system built around MQTT and a REST/WebSocket backend. Door clients run on Arduino Nano ESP32 or compatible boards, report activity to a central Mosquitto broker, persist data via a C# backend and PostgreSQL, and are managed through a Qt desktop admin application.

---

## Architecture

```
                        ┌──────────────────────────────┐
                        │ MQTT Broker (ag_mqtt_broker) │
                        │                              │
  mTLS (agk_door_N)     │         :8883  mTLS          │
                  ┌─────┤                              ├─────┐
                  │     └──────────────────────────────┘     │
                  │                                          │ mTLS (backend)
┌─────────────────▼──────────────┐               ┌───────────▼───────────┐                   ┌──────────────────────────┐
│ ArduinoGateKeeper (agk_door_N) │               │    C# ASP.NET Core    │ <───────────────> │  ArduinoGatekeeperAdmin  │    
│       Arduino Nano ESP32       │               │ Backend (agk_backend) │ REST +            │    Qt C++ Desktop App    │
│        FreeRTOS + RFID         │               │   OData │ SignalR     │ WebSocket (mTLS)  └──────────────────────────┘       
└────────────────────────────────┘               └───────────┬───────────┘        
                                                             │ mTLS
                                               ┌─────────────▼─────────────┐
                                               │ PostgreSQL (agk_database) │
                                               └───────────────────────────┘
```

Every connection in the system uses mutual TLS with certificates issued by a self-hosted CA. No connection accepts anonymous or password-only clients except the admin login flow, which requires both a valid client certificate and a TOTP code.

---

## Modules

### Device (agk_door_N)
Firmware for the Arduino Nano ESP32 door client. Uses FreeRTOS with dedicated tasks for RFID scanning, MQTT communication, and WiFi management. Operates an MFRC522 RFID reader, performs local access verification against a cached authorized user list received from the broker, blinks a green or red LED to indicate access granted or denied, and reports all scan activity back to the broker via MQTT over mTLS.

See `ArduinoGateKeeper/README.md` for setup and configuration.

### ArduinoGatekeeperAdmin (agk_admin)
Qt C++ desktop admin application. Authenticates via mTLS + TOTP. Communicates exclusively with the backend over HTTPS and WebSocket — it never connects to the broker directly. Displays a real-time dashboard of door status and activity, provides a log analyzer with live WebSocket updates, and allows admins to manage authorized RFID users and door permissions.

See `ArduinoGatekeeperAdmin/README.md` for setup and configuration.

### Backend (agk_backend)
ASP.NET Core (.NET 8) backend service. Connects to Mosquitto as an MQTT client over mTLS, subscribes to all door topics, persists events to PostgreSQL, and exposes an OData REST API and SignalR WebSocket hub to the admin app. Handles user list distribution to door clients via paginated MQTT publishing with acknowledgement and retry logic.

See `ArduinoGatekeeperBackend/README.md` for setup and configuration.

### MQTT Server (agk_mqtt_broker)
Pre-configured Mosquitto MQTT broker Docker image for local development and testing. Ships with a dual-listener setup (mTLS on 8884, mTLS on 8883), ACL rules, and a certificate generation script. Not intended for production use.

See `MosquittoServerDummy/README.md` for setup and configuration.

---

## Security Model

| Connection | Protocol | Auth mechanism |
|---|---|---|
| Arduino → MQTT broker | MQTT over TLS 1.3 | mTLS — unique cert per door (`agk_door_N`) |
| Backend → MQTT broker  | MQTT over TLS 1.3 | mTLS — backend client cert |
| Backend → PostgreSQL | TLS 1.3 | mTLS — `clientcert=verify-full` |
| Qt Admin → Backend | HTTPS / WSS TLS 1.3 | mTLS + TOTP |

All certificates are ECC P-256, issued by a self-hosted CA. The CA private key and admin private key are never copied into any Docker image and must not be committed to version control.

---

## MQTT Topic Structure

| Topic | Publisher | Subscriber |
|---|---|---|
| `AGK/{id}/dev_status`   | Door client -> Backend |
| `AGK/{id}/scan`         | Door client -> Backend |
| `AGK/{id}/add_users`    | Backend -> Door client |
| `AGK/{id}/remove_users` | Door client -> Backend |

Door clients publish `dev_status` on connect and disconnect, and `scan` on every card read. On receiving a `dev_status` online event, the backend pushes the authorized user list to the door in paginated chunks via `add_user`. Doors can also request a fresh user list at any time via `req_users`, with a timeout-based retry if chunks are not fully received.

---

## REST API

Exposed by the backend on port 443. All endpoints require a valid client certificate. Supports full OData query syntax (`$filter`, `$orderby`, `$top`, `$skip`, `$select`, `$expand`, `$count`).

| Endpoint | Methods | Description |
|---|---|---|
| `/api/Admins` | GET, POST, PATCH, DELETE | Manage admin CNs and roles |
| `/api/Users` | GET, POST, PATCH, DELETE | Manage authorized RFID cards |
| `/api/Doors` | GET, PATCH | Door registry and labels |
| `/api/Logs` | GET | Access event log |
| `/api/DoorLogs` | GET | Door status history |
| `/api/Permissions` | GET, POST, DELETE | Door-to-user access mappings |
| `/hubs/logs` | WebSocket | Real-time log and status push |

---

## Database Schema

```
admins        — authorized admin CNs and TOTP secrets
users         — registered RFID cards and MIFARE keys (stored as BYTEA)
doors         — door client registry and labels
permissions   — door-to-user access mappings (composite key)
access_log    — card scan events
door_log      — door online/offline events
```

---

## Docker Setup

The system runs as three containers on a shared internal Docker network:

```bash
docker network create agk_network

# Mosquitto broker
docker build -t agk-broker ./MosquittoServerDummy
docker run -d --name agk_mosquitto --network agk_network \
  -p 8883:8883 -p 8884:8884 agk-broker

# PostgreSQL
docker build -t agk-postgres ./ArduinoGatekeeperBackend/postgres
docker run -d --name agk_postgres --network agk_network agk-postgres

# Backend
docker build -t agk-backend ./ArduinoGatekeeperBackend
docker run -d --name agk_backend --network agk_network \
  -p 443:443 agk-backend
```

PostgreSQL is not exposed to the host — only the backend reaches it over the internal Docker network.

---

## Getting Started

Prerequisites
Docker and Docker Compose
OpenSSL (for certificate generation)

1. Generate Certificates

Run the certificate generation script before doing anything else:

bash
chmod +x generate_certs.sh
./generate_certs.sh

This generates the full certificate set for all components — CA, server, backend, admin, and PostgreSQL — all signed by the self-hosted CA using ECC P-256.

Keep ca.key and admin.key secure. Never commit them to version control.

2. Start the Stack
bash
docker compose up --build -d

This starts three containers on an internal Docker network:

agk_mqtt_broker — MQTT broker, port 8883 exposed to host
agk_database — PostgreSQL, internal only
agk_backend — ASP.NET Core backend, port 443 exposed to host

3. Flash Arduino Firmware

See device/README.md.

4. Run the Admin App

See admin/ArduinoGatekeeperAdmin/README.md.

Stopping and Rebuilding
bash

### Stop all containers
docker compose down

### Rebuild after code or config changes
docker compose up --build -d

### Rebuild after certificate rotation
./generate_certs.sh
docker compose up --build -d
