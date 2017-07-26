#ifndef __TU_GENERATEMESSAGE_H__
#define __TU_GENERATEMESSAGE_H__

#include "Task.h"
#include <ArduinoJson.h>
#include "messageBroker.h"
#include "APs.h"

class tUGenerateMessage: public Task{

public:
	tUGenerateMessage(messageBroker * _messages,broadcastNode * _node);
	void execute();
private:
	unsigned long own_sequence;
	
	messageBroker * messages;	
	broadcastNode * node;
};
#endif
