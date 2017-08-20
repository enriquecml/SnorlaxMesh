#ifndef __SNORLAXMESH_H__
#define __SNORLAXMESH_H__

#include "SchedulerNode.h"
//Tasks...
#include "tRemoveMessages.h"
#include "tReviewMessages.h"
#include "tGenerateRateMessage.h"
#include "tReadRateMessage.h"
#include "tGenerateStatsMessage.h"
#include "tUGenerateMessage.h"
class SnorlaxMesh{

private:
	SchedulerNode scheduler;

	broadcastNode node;
	messageBroker messages;

	APs listAPs;

	LinkedList<Task*> listTasks;	
public:
	SnorlaxMesh();
	void run();
};

#endif