#!/bin/sh

# Generate hashed passwd file from plaintext
touch /mosquitto/config/passwd.conf;
while IFS=: read -r user pass; do
  mosquitto_passwd -b /mosquitto/config/passwd.conf "$user" "$pass";
done < /mosquitto/config/passwd_plain.conf;

# Prepare files
rm /mosquitto/config/passwd_plain.conf;
chown mosquitto -R *;
chgrp mosquitto -R *;
chmod o-rwx /mosquitto/config/acl.conf;
chmod o-rwx /mosquitto/config/passwd.conf;

# Start Mosquitto
exec mosquitto -c /mosquitto/config/mosquitto.conf;