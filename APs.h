#ifndef __APS_H__
#define __APS_H__

#include <Arduino.h>
#include <LinkedList.h>
#include "AP.h"

class APs{

private:
	LinkedList<AP*> list_APs;
	
public:
	APs();
	void addAP(AP * ap);
	AP * giveAP(String &sAP);
	int givePositionAP(String &sAP);
	AP * giveAP(int position);
	
	int numberAPs();
	
};

#endif