# Backend

ASP.NET Core (.NET 8) backend service for the ArduinoGatekeeper system. Acts as the central hub between the MQTT broker, the database, and the admin application — persisting all device activity, managing user permissions, and exposing data via a REST API and real-time WebSocket.

---

## Overview

The backend runs two concurrent responsibilities:

- **MQTT client** — connects to the broker over mTLS, subscribes to device topics, persists incoming events to PostgreSQL, and publishes authorized user lists to door clients when their permissions change or when they come online
- **HTTP server** — exposes an OData v4 REST API for the admin application and a SignalR WebSocket hub for real-time log streaming

The admin application never connects to the broker directly — all MQTT interaction is handled exclusively by the backend.

---

## Authentication

All connections to the backend require mutual TLS. The admin application must present a client certificate signed by the project CA. The certificate CN is matched against the `admins` table for authorization — if the CN is not found or the admin is inactive, the request is rejected.

Admin login additionally requires a **TOTP code** (time-based one-time password) verified against a secret stored in the `admins` table (To-Do), providing a second authentication factor beyond the client certificate.

---

## MQTT

The backend connects to the broker on port **8883** using mTLS with the `backend` client certificate.

### Subscriptions

| Topic | Event |
|---|---|
| `AGK/+/dev_status` | Door came online or went offline |
| `AGK/+/scan` | Card scan event from a door |

### Publishing

| Topic | Trigger |
|---|---|
| `AGK/{id}/add_users` | Door comes online, corresponding user permissions from table `users` sent to it in batches of configurable size |
| `AGK/{id}/remove_users` | If user permissions are removed by the admin, sync it with the device |

When a door comes online or a permission changes, the backend publishes the authorized user list to the affected door in **paginated chunks**.

---

## REST API

Exposed on port **443** over HTTPS. All endpoints require a valid client certificate (mTLS).

Supports full **OData v4** query syntax: `$filter`, `$orderby`, `$top`, `$skip`, `$select`, `$expand`, `$count`.

| Endpoint | Methods | Description |
|---|---|---|
| `/api/Admins` | GET, POST, PATCH, DELETE | Manage authorized admin identities |
| `/api/Users` | GET, POST, PATCH, DELETE | Manage registered RFID cards |
| `/api/Doors` | GET, PATCH | Door registry and labels |
| `/api/Permissions` | GET, POST, DELETE | Door-to-user access mappings |
| `/api/AccessLogs` | GET | Card scan event log |
| `/api/DoorLogs` | GET | Door online/offline event log |

### OData Query Examples

```
# All scan events for a specific door, most recent first
GET /api/AccessLogs?$filter=DoorId eq 5&$orderby=CreatedAt desc&$top=50

# All denied access attempts
GET /api/AccessLogs?$filter=Granted eq false&$orderby=CreatedAt desc

# Users with their door permissions expanded
GET /api/Users?$expand=Permissions($expand=Door)

# Doors that have been registered
GET /api/Doors?$orderby=Label
```

---

## WebSocket

The backend exposes a **SignalR** hub for real-time event streaming to the admin application's log analyzer. The connection is only opened when the log analyzer view is active and closed when it is dismissed.

| Hub | URL |
|---|---|
| Log hub | `wss://agk_backend/hubs/logs` |

### Events pushed by the server

| Event | Payload | Trigger |
|---|---|---|
| `NewLogEntry` | Access log entry | Card scan received from any door |
| `NewStatusEntry` | Door log entry | Door online/offline event received |

### Client methods

| Method | Parameters | Description |
|---|---|---|
| `SubscribeToDoor` | `doorId: string` | Filter events to a specific door |
| `UnsubscribeFromDoor` | `doorId: string` | Remove door filter |

---

## Stack

| Component | Technology |
|---|---|
| Framework | ASP.NET Core (.NET 8) |
| ORM | Entity Framework Core + Npgsql |
| MQTT client | MQTTnet |
| OData | Microsoft.AspNetCore.OData |
| WebSocket | SignalR |
| Logging | Serilog |

---

## Security

- TLS 1.3 only on the Kestrel listener
- mTLS required on all connections — client cert validated against the project CA exclusively (system trust store is not used)
- Admin CN validated against the `admins` table on every request
- TOTP required for admin login
- Database connection uses mTLS with `clientcert=verify-full`
- MQTT connection uses mTLS with custom CA validation

---

## Setup

Certificates must be generated before building the image. Run from the project root:

```bash
chmod +x generate_certs.sh
./generate_certs.sh
```

The image is built and started automatically via Docker Compose:

```bash
docker compose up --build -d
```

The backend depends on both the broker and the database being reachable on startup. Docker Compose health checks ensure correct startup order.
