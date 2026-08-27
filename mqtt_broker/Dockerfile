FROM eclipse-mosquitto
WORKDIR /mosquitto

COPY config                 config
COPY entrypoint.sh          entrypoint.sh

COPY ssl/ca.crt             certs/ca.crt
COPY ssl/mqtt_broker.crt    certs/mqtt_broker.crt
COPY ssl/mqtt_broker.key    certs/mqtt_broker.key

RUN chmod +x entrypoint.sh
ENTRYPOINT [ "sh", "/mosquitto/entrypoint.sh" ]