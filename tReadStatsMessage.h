#ifndef __T_READSTATSMESSAGE_H__
#define __T_READSTATSMESSAGE_H__

#include "Task.h"
#include <ArduinoJson.h>
#include "messageBroker.h"

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