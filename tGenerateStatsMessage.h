#ifndef __T_GENERATESTATSMESSAGE_H__
#define __T_GENERATESTATSMESSAGE_H__

#include "Task.h"
#include <ArduinoJson.h>
#include "messageBroker.h"
#include "broadcastNode.h"
#include "SingletonStats.h"

class tGenerateStatsMessage: public Task{

public:
	tGenerateStatsMessage(messageBroker * _messages,broadcastNode * _node);
	void execute();
private:

	unsigned long own_sequence;	
	messageBroker * messages;	
	broadcastNode *node;
	bool isMessageOwnAboutStats(String &_msg);

};
#endif