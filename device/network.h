#ifndef __ARDUINO_GATEKEEPER_NETWORK__
#define __ARDUINO_GATEKEEPER_NETWORK__

#include <WiFi.h>
#include <time.h>
#include "config.h"

class Network {
  private:
    static char _strBuffer[STRING_BUFFER_SIZE];
    static bool connectWiFi();
    static bool synchronizeClock();
  public:
    static bool begin();
};

#endif