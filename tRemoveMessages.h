#ifndef __T_REMOVEMESSAGES_H__
#define __T_REMOVEMESSAGES_H__

#include "Task.h"
#include "messageBroker.h"
#include "APs.h"

class tRemoveMessages: public Task{

public:
	tRemoveMessages();
	void execute();
private:
	APs * listAPs;
	messageBroker * messages;	
};
#endif
