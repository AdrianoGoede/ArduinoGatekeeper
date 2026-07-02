# Generate CA cert
mkdir ssl_root && cd ssl_root;
openssl ecparam -name prime256v1 -genkey -noout -out ca.key;
openssl req -new -x509 -days 3650 -key ca.key -out ca.crt -subj "/CN=ArduinoGatekeeperCA";

# Generate database cert
cd ../database;
mkdir ssl && cd ssl;
openssl ecparam -name prime256v1 -genkey -noout -out database.key;
openssl req -new -key database.key -out database.csr -subj "/CN=AGK_Database";
openssl x509 -req -days 3650 -in database.csr -CA ../../ssl_root/ca.crt -CAkey ../../ssl_root/ca.key -CAcreateserial -out database.crt;
cp ../../ssl_root/ca.crt ca.crt && rm database.csr && cd ..;

# Generate MQTT boker cert
cd ../mqtt_broker;
mkdir ssl && cd ssl;
openssl ecparam -name prime256v1 -genkey -noout -out mqtt_broker.key;
openssl req -new -key mqtt_broker.key -out mqtt_broker.csr -subj "/CN=AGK_MQTT_Broker";
openssl x509 -req -days 3650 -in mqtt_broker.csr -CA ../../ssl_root/ca.crt -CAkey ../../ssl_root/ca.key -CAcreateserial -out mqtt_broker.crt;
cp ../../ssl_root/ca.crt ca.crt && rm mqtt_broker.csr && cd ..;