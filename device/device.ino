#include "network.h"
#include "config.h"

void setup() {
  Serial.begin(SERIAL_BAUD_RATE);
  while (!Serial) {}

  Network::begin();
}

void loop() {
  delay(5000);
  Network::handleConnections();
}