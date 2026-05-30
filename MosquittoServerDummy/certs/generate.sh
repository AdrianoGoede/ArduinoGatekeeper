# User input
echo "Enter the hostname or IP address: ";
read hostname;

# CA Cert & key
mkdir root;
openssl ecparam -name prime256v1 -genkey -noout -out ./root/ca.key;
openssl req -new -x509 -days 3650 -key ./root/ca.key -out ./root/ca.crt -subj "/CN=MosquittoCA";

# Server Cert & key
mkdir server;
openssl ecparam -name prime256v1 -genkey -noout -out ./server/server.key;
openssl req -new -key ./server/server.key -out ./server/server.csr -subj "/CN=${hostname}";
openssl x509 -req -days 3650 -in ./server/server.csr -CA ./root/ca.crt -CAkey ./root/ca.key -CAcreateserial -out ./server/server.crt;
rm ./server/server.csr;

# Admin Cert & key
mkdir admin;
openssl ecparam -name prime256v1 -genkey -noout -out ./admin/admin.key;
openssl req -new -key ./admin/admin.key -out ./admin/admin.csr -subj "/CN=admin";
openssl x509 -req -days 3650 -in ./admin/admin.csr -CA ./root/ca.crt -CAkey ./root/ca.key -CAcreateserial -out ./admin/admin.crt;
rm ./admin/admin.csr;

# Cleanup
rm ./root/*.srl;