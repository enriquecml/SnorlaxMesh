#ifndef __APS_H__
#define __APS_H__

#include <Arduino.h>
#include <AP.h>
#include <LinkedList.h>
class APs{
private:
	LinkedList<AP *> _APs;
public:
  LinkedList<AP *> * getListAPs();
  APs();

};

#endif
