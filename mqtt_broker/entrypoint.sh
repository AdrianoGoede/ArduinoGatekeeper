#!/bin/sh

# Prepare files
chown mosquitto -R *;
chgrp mosquitto -R *;

# Start Mosquitto
exec mosquitto -c /mosquitto/config/mosquitto.conf;