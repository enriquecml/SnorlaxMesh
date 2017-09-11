#ifndef __PUBBASE_H__
#define __PUBBASE_H__

#include <Arduino.h>
#include <LinkedList.h>
#include <ArduinoJson.h>
#include "SingletonStats.h"
#define DEBUG 1
class PubBase{

public:
  bool publicated;
  String channel;
  PubBase();
 void generatePubMessage(String &sJson,String origin_ssid);
};
#endif
