#ifndef __TU_GENERATEACELEROMETER_H__
#define __TU_GENERATEACELEROMETER_H__

#include "Task.h"
#include <ArduinoJson.h>
#include "messageBroker.h"
#include "broadcastNode.h"
#include "Wire.h"

//Direccion I2C de la IMU
#define MPU 0x68
 

class tUGenerateAcelerometer: public Task{

public:
	tUGenerateAcelerometer(messageBroker * _messages,broadcastNode * _node);
	void execute();
private:

	unsigned long last_time;
	unsigned long own_sequence;	
	messageBroker * messages;	
	broadcastNode *node;

};
#endif