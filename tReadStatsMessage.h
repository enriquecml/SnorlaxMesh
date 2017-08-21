#ifndef __T_READSTATSMESSAGE_H__
#define __T_READSTATSMESSAGE_H__

#include "Task.h"
#include <ArduinoJson.h>
#include "messageBroker.h"

#define DEBUG_SNORLAX_READSTATSMESSAGE 1
#ifdef  DEBUG_SNORLAX_READSTATSMESSAGE
#define DEBUG_SNORLAX_READSTATSMESSAGE(...) __VA_ARGS__
#endif

class tReadStatsMessage: public Task{

public:
	tReadStatsMessage(messageBroker * _messages);
	void execute();
private:
	unsigned long own_sequence;
	bool isMessageStats(String &_msg);	
	messageBroker * messages;	

};
#endif