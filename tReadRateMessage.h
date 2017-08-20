#ifndef __T_READRATEMESSAGE_H__
#define __T_READRATEMESSAGE_H__

#include "Task.h"
#include <ArduinoJson.h>
#include "messageBroker.h"
#include "APs.h"

class tReadRateMessage: public Task{

public:
	tReadRateMessage(messageBroker * _messages,APs * _aps);
	void execute();
private:
	unsigned long own_sequence;
bool isMessageRate(String &_msg);	
	messageBroker * messages;	
		APs *aps;

};
#endif