#ifndef __AP_H__
#define __AP_H__

#include <Arduino.h>
class AP{

public:

  String ssid;
  int nTry;
  unsigned long time_saw;
  unsigned long rate;
  unsigned long period_s;
  bool connected;
  int positionMessage;
  AP();

};

#endif
