# ArduinoGatekeeperAdmin

Qt C++ desktop administration application for the ArduinoGatekeeper system. Provides full management of users, doors and permissions, and displays real-time access activity via a live dashboard.

---

## Overview

The admin application communicates exclusively with the backend service — it never connects to the MQTT broker or the database directly. All data access goes through the backend's OData v4 REST API, and real-time events are received via a WebSocket connection to the backend's SignalR hub.

---

## Authentication

The application authenticates to the backend using **mutual TLS + TOTP**:

- A valid client certificate signed by the project CA must be provided on startup
- The certificate CN is validated against the `admins` table on the backend on every request

All connections use TLS 1.3. The SSL configuration is initialized once at startup and shared across all backend requests for the lifetime of the session.

---

## Features

### User Management
Full CRUD for registered RFID cards:
- View all registered users with their card ID, card key, label, status and permissions
- Register new RFID cards
- Edit user labels and permissions
- Remove users

### Door Management
Full CRUD for door client devices:
- View all registered doors with their label and last seen status
- Register new doors
- Edit door labels
- Remove doors

### Live Dashboard
Real-time activity display via WebSocket connection to the backend's SignalR hub. The WebSocket is only open while the dashboard is active:
- Access event counters (granted vs denied)
- Door online/offline status indicators
- Live activity graphs updated on each incoming event
- Recent event feed

---

## API

All data operations use the backend's **OData v4** REST API over HTTPS on port **443**. Full OData query support is used throughout — `$filter`, `$orderby`, `$top`, `$expand` and `$select` are applied per view to fetch only what is needed.

| Endpoint | Operations |
|---|---|
| `/api/Users` | GET, POST, PATCH, DELETE |
| `/api/Doors` | GET, PATCH |
| `/api/Permissions` | GET, POST, DELETE |
| `/api/Logs` | GET |
| `/api/DoorLogs` | GET |

### WebSocket

| Hub | URL |
|---|---|
| Log hub | `wss://agk_backend/hubs/logs` |

| Event | Description |
|---|---|
| `NewLogEntry` | Card scan event — updates counters and graphs |
| `NewStatusEntry` | Door online/offline event — updates status indicators |

---

## Stack

| Component | Technology |
|---|---|
| Framework | Qt 6 (Widgets, Network) |
| HTTP client | `QNetworkAccessManager` + `ODataClient` wrapper |
| WebSocket | `QWebSocket` |
| TLS | `QSslConfiguration` — mTLS, TLS 1.3, ECC P-256 |

---

## Setup

### Prerequisites

- Qt 6.5 or later
- CMake 3.19 or later
- A valid admin client certificate and key issued by the project CA (`generate_certs.sh`)
- An authenticator app (Google Authenticator, Authy, etc.) set up for your admin account

### Build

```bash
cmake -B build
cmake --build build
```

### Run

On first launch the application presents an authentication dialog prompting for:

- **CA certificate** — `ca.crt` from the project certificate set
- **Client certificate** — `admin.crt` issued to this admin
- **Client key** — `admin.key` corresponding to the client certificate

Once authenticated, the session remains active until the application is closed.
