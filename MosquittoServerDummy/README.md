# ArduinoGatekeeper — Mosquitto MQTT Broker

A secured Mosquitto MQTT broker Docker image for the ArduinoGatekeeper access control system. Supports two separate listeners with different authentication strategies: TLS + credentials for door client devices, and mutual TLS for the admin application. Certificates are baked into the image at build time — `generate.sh` must be run before building.

---

## Requirements

- Docker
- OpenSSL (for certificate generation)

---

## Project Structure

```
.
├── Dockerfile
├── .dockerignore
├── entrypoint.sh
├── certs/
│   ├── generate.sh                  ← run before building
│   ├── root/
│   │   ├── ca.crt                   ← baked into image
│   │   └── ca.key                   ← never commit, not copied into image
│   ├── server/
│   │   ├── server.crt               ← baked into image
│   │   └── server.key               ← baked into image
│   └── admin/
│       ├── admin.crt                ← distribute to admin app - never commit, not copied into image
│       └── admin.key                ← distribute to admin app - never commit, not copied into image
└── config/
    ├── mosquitto.conf
    ├── acl.conf
    └── passwd_plain.conf            ← deleted after first run
```

---

## Setup

### 1. Generate Certificates

Run the certificate generation script before building the image. It will prompt for the broker's hostname or IP address, which must match what clients use to connect:

```bash
chmod +x generate.sh
./generate.sh
```

This generates:
- `certs/root/ca.crt` — CA certificate (distribute to all clients)
- `certs/server/server.crt` + `server.key` — broker certificate
- `certs/admin/admin.crt` + `admin.key` — admin app client certificate

> **Keep `ca.key` and `admin.key` secure. Never commit them to version control.**

### 2. Configure Credentials

Before building, populate `config/passwd_plain.conf` with one door client per line in `username:password` format:

```
Agk_1:password_for_door_1
Agk_2:password_for_door_2
Agk_3:password_for_door_3
```

Each username must follow the `Agk_` pattern and match the client ID the corresponding Arduino will use to connect. The entrypoint script will hash these into a proper Mosquitto password file on first container start and then delete the plaintext file.

> **Passwords must not contain `:` as it is used as the delimiter.**

### 3. Build and Run

Build the image — certificates and config are baked in at this step:

```bash
docker build -t arduinogatekeeper-broker .
```

Run the container:

```bash
docker run -d \
  --name mosquitto \
  -p 8883:8883 \
  -p 8884:8884 \
  arduinogatekeeper-broker
```

---

## Listeners

| Port | Auth | Used by |
|------|------|---------|
| `8883` | TLS + username/password | Arduino door clients (`Agk_*`) |
| `8884` | Mutual TLS (client certificate) | Admin desktop application |

---

## MQTT Topic Structure

| Topic | Publisher | Subscriber |
|-------|-----------|------------|
| `ArduinoGatekeeper/Admin/UserList` | Admin app | Door clients |
| `ArduinoGatekeeper/{id}/Status` | Door client | Admin app |
| `ArduinoGatekeeper/{id}/Log` | Door client | Admin app |

Door clients subscribe to `UserList` to perform local authorization. The admin app reads `Status` and `Log` from all doors using the `+` single-level wildcard.

---

## Access Control

- **Admin** (`CN=admin`) — full read/write on `UserList`, read-only on all door `Status` and `Log` topics.
- **Door clients** (`Agk_*`) — write-only on their own `Status` and `Log` topics, read-only on `UserList`.

Door clients are restricted to their own topics via the `%u` username pattern — a client authenticated as `Agk_1` cannot publish to `Agk_2`'s topics.

---

## Certificate Rotation

Since certificates are baked into the image, rotating them requires a rebuild:

1. Run `./generate.sh` to produce new certificates
2. Stop and remove the current container:

```bash
docker stop mosquitto
docker rm mosquitto
```

3. Rebuild and run:

```bash
docker build -t arduinogatekeeper-broker .
docker run -d \
  --name mosquitto \
  -p 8883:8883 \
  -p 8884:8884 \
  arduinogatekeeper-broker
```

---

## Logs

```bash
docker logs mosquitto
```

Mosquitto logs to stdout, which Docker captures. All event types are logged (`log_type all`).

---

## Adding or Removing Door Clients

To add or remove door clients, update `config/passwd_plain.conf` and `config/acl.conf`, then do a full rebuild:

```bash
docker stop mosquitto
docker rm mosquitto
docker build -t arduinogatekeeper-broker .
docker run -d \
  --name mosquitto \
  -p 8883:8883 \
  -p 8884:8884 \
  arduinogatekeeper-broker
```
