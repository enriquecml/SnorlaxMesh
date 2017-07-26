#ifndef __T_REVIEWMESSAGES_H__
#define __T_REVIEWMESSAGES_H__

#include "Task.h"
#include "messageBroker.h"
#include "broadcastNode.h"
#include <ArduinoJson.h>

class tReviewMessages: public Task{

public:
	tReviewMessages(messageBroker * _messages,broadcastNode * _node);
	void execute();
private:

	messageBroker * messages;
	broadcastNode * node;
	
	bool wasHere(String &_msg);
	void addNodeToMessage(String &_msg);	
	
	
};
#endif
