#ifndef __T_GENERATERATEMESSAGE_H__
#define __T_GENERATERATEMESSAGE_H__

#include "Task.h"
#include <ArduinoJson.h>
#include "messageBroker.h"
#include "broadcastNode.h"
#include "APs.h"


class tGenerateRateMessage: public Task{

public:
	tGenerateRateMessage(messageBroker * _messages,broadcastNode * _node,APs * _aps);
	void execute();
private:
	unsigned long own_sequence;
	
	messageBroker * messages;	
	broadcastNode * node;
	APs *aps;
	
	int lastNumberAPS;

};
#endif