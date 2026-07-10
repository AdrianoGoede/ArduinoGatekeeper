# Generate CA cert
mkdir ssl_root && cd ssl_root;
openssl ecparam -name prime256v1 -genkey -noout -out ca.key;
openssl req -new -x509 -days 3650 -key ca.key -out ca.crt -subj "/CN=ArduinoGatekeeperCA";

# Generate database cert
cd ../database;
mkdir ssl && cd ssl;
openssl ecparam -name prime256v1 -genkey -noout -out database.key;
openssl req -new -key database.key -out database.csr -subj "/CN=agk_database";
openssl x509 -req -days 3650 -in database.csr -CA ../../ssl_root/ca.crt -CAkey ../../ssl_root/ca.key -CAcreateserial -out database.crt;
cp ../../ssl_root/ca.crt ca.crt && rm database.csr && cd ..;

# Generate Backend cert
cd ../backend;
mkdir ssl && cd ssl;
openssl ecparam -name prime256v1 -genkey -noout -out backend.key;
openssl req -new -key backend.key -out backend.csr -subj "/CN=agk_backend";
openssl x509 -req -days 3650 -in backend.csr -CA ../../ssl_root/ca.crt -CAkey ../../ssl_root/ca.key -CAcreateserial -out backend.crt;
cp ../../ssl_root/ca.crt ca.crt && rm backend.csr && cd ..;

# Generate MQTT broker cert
cd ../mqtt_broker;
mkdir ssl && cd ssl;
openssl ecparam -name prime256v1 -genkey -noout -out mqtt_broker.key;
openssl req -new -key mqtt_broker.key -out mqtt_broker.csr -subj "/CN=agk_mqtt_broker";
openssl x509 -req -days 3650 -in mqtt_broker.csr -CA ../../ssl_root/ca.crt -CAkey ../../ssl_root/ca.key -CAcreateserial -out mqtt_broker.crt;
cp ../../ssl_root/ca.crt ca.crt && rm mqtt_broker.csr && cd ..;

# Generate Arduino cert
cd ../device;
mkdir ssl && cd ssl;
openssl ecparam -name prime256v1 -genkey -noout -out device.key;
openssl req -new -key device.key -out device.csr -subj "/CN=agk_door_1";
openssl x509 -req -days 3650 -in device.csr -CA ../../ssl_root/ca.crt -CAkey ../../ssl_root/ca.key -CAcreateserial -out device.crt;
cp ../../ssl_root/ca.crt ca.crt && rm device.csr && cd ..;