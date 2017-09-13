#ifndef __TU_GENERATEACELEROMETER_H__
#define __TU_GENERATEACELEROMETER_H__

#include "Task.h"
#include <ArduinoJson.h>
#include "messageBroker.h"
#include "broadcastNode.h"
#include "Wire.h"

//Direccion I2C de la IMU
#define MPU 0x68
 
//Ratios de conversion
#define A_R 16384.0
#define G_R 131.0
 
//Conversion de radianes a grados 180/PI
#define RAD_A_DEG = 57.295779

class tUGenerateAcelerometer: public Task{

public:
	tUGenerateAcelerometer(messageBroker * _messages,broadcastNode * _node);
	void execute();
private:

	unsigned long own_sequence;	
	messageBroker * messages;	
	broadcastNode *node;

};
#endif