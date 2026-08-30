# Database

PostgreSQL database Docker image for the ArduinoGatekeeper system. Stores all persistent data — admin identities, registered RFID cards, door registry, access permissions, and event logs.

---

## Overview

The database is only accessible from within the Docker network — it is never exposed to the host. All connections require mutual TLS with `clientcert=verify-full`, meaning the connecting client must present a certificate whose CN matches the database username.

Only the backend service connects to the database directly.

---

## Schema

### `admins`
Registered admin identities. Each admin authenticates to the backend via mTLS — their certificate CN is matched against this table for authorization. Optionally extended with a TOTP secret for two-factor authentication.

| Column | Type | Description |
|---|---|---|
| `id` | SERIAL | Primary key |
| `label` | VARCHAR(200) | Human-readable name |
| `active` | BOOLEAN | Whether this admin is currently authorized |
| `created_at` | TIMESTAMP | Registration timestamp |

---

### `users`
Registered RFID cards. The `card_key` is the MIFARE sector key used for card authentication, stored as raw bytes.

| Column | Type | Description |
|---|---|---|
| `id` | SERIAL | Primary key |
| `card_id` | VARCHAR(8) | Unique card UID (hex) |
| `card_key` | BYTEA | MIFARE key (6 bytes, default `000000000000`) |
| `label` | VARCHAR(200) | Human-readable name |
| `created_at` | TIMESTAMP | Registration timestamp |

---

### `doors`
Registered door client devices.

| Column | Type | Description |
|---|---|---|
| `id` | SERIAL | Primary key |
| `label` | VARCHAR(100) | Human-readable name |
| `created_at` | TIMESTAMP | Registration timestamp |

---

### `permissions`
Maps which users are authorized at which doors. Composite primary key — one row per user/door pair.

| Column | Type | Description |
|---|---|---|
| `user_id` | INT | References `users.id` |
| `door_id` | INT | References `doors.id` |
| `created_at` | TIMESTAMP | When permission was granted |

---

### `door_log`
Records door client online/offline events as received via MQTT.

| Column | Type | Description |
|---|---|---|
| `id` | BIGSERIAL | Primary key |
| `door_id` | INT | References `doors.id` |
| `online` | BOOLEAN | `true` = came online, `false` = went offline |
| `created_at` | TIMESTAMP | Event timestamp |

---

### `access_log`
Records every card scan event reported by door clients. Append-only — entries are never updated or deleted.

| Column | Type | Description |
|---|---|---|
| `id` | BIGSERIAL | Primary key |
| `user_id` | INT | References `users.id` |
| `door_id` | INT | References `doors.id` |
| `granted` | BOOLEAN | Whether access was granted |
| `created_at` | TIMESTAMP | Scan timestamp |

---

## Relationships

```
users ──────────┬── permissions ──┬────── doors
                │                 │
                └──── access_log ─┘
                                  │
                      door_log ───┘

admins (standalone — no FK relations)
```

---

## Security

- TLS 1.3 only, enforced via `postgresql.conf`
- `clientcert=verify-full` in `pg_hba.conf` — cert CN must match the database username
- Non-SSL connections explicitly rejected via `hostnossl ... reject`
- The `agk_backend` database user has the minimum necessary permissions — no superuser, no DDL rights after initialization

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

The schema is applied automatically on first startup via the `initdb` scripts. Subsequent restarts do not re-run the scripts.
