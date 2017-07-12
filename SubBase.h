#ifndef __SUBBASE_H__
#define __SUBBASE_H__

#include <Arduino.h>
#include <LinkedList.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>

#define DEBUG 1
class SubBase{


public:
  String channel;
  SubBase();
  bool read(String sJson);
 //void generatePubMessage(String &sJson);
};
#endif
