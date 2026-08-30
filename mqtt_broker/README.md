# MQTT Broker

Mosquitto MQTT broker Docker image for the ArduinoGatekeeper system. Serves as the central message bus between door client devices and the backend service.

---

## Overview

All clients — door devices and the backend service — connect on a single listener using **mutual TLS**. Every client must present a certificate signed by the project CA to connect. There are no password-based or anonymous connections.

---

## Listener

| Port | Protocol | Auth |
|---|---|---|
| `8883` | MQTT over TLS 1.3 | mTLS — client certificate required |

The `require_certificate true` directive ensures any connection without a valid client cert is rejected at the TLS handshake level, before any MQTT traffic is exchanged.

---

## Topic Structure

| Topic | Publisher | Subscriber |
|---|---|---|
| `AGK/{id}/dev_status` | Door client | Backend |
| `AGK/{id}/scan` | Door client | Backend |
| `AGK/{id}/add_users` | Backend | Door client |
| `AGK/{id}/remove_users` | Door client | Backend |

`{id}` is the device identifier of the door client (e.g. `agk_door_1`, `agk_door_2`).

---

## Access Control

ACL rules are enforced via `acl.conf`. The `use_identity_as_username` directive maps each client's cert CN to their MQTT username for ACL lookups.

| Client | CN | Permissions |
|---|---|---|
| Backend | `backend` | `readwrite ArduinoGatekeeper/#` |
| Door clients | `Agk_N` | Write own `dev_status`, `scan`, `req_users` — Read own `add_user` |

Door clients are restricted to their own topics via the `%u` username pattern — `agk_door_1` cannot publish to `agk_door_2`'s topics.

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
