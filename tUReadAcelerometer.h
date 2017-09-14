#ifndef __TU_READACELEROMETER_H__
#define __TU_READACELEROMETER_H__

#include "Task.h"
#include <ArduinoJson.h>
#include "messageBroker.h"
#include "broadcastNode.h"




class tUReadAcelerometer: public Task{

public:
	tUReadAcelerometer(messageBroker * _messages,broadcastNode * _node);
	void execute();
 bool sendToCloud(String &sJson);	
 bool isMessageAcelerometer(String &_msg);
private:

	unsigned long own_sequence;	
	messageBroker * messages;	
	broadcastNode *node;

};
#endif