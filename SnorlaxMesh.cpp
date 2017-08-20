#include "SnorlaxMesh.h"

SnorlaxMesh::SnorlaxMesh(){
	scheduler.set_APs(&listAPs);
	scheduler.set_messageBroker(&messages);
	scheduler.set_node(&node);
	listTasks.add(new tReviewMessages(&messages,&node));
	listTasks.add(new tRemoveMessages(&listAPs,&messages));
	listTasks.add(new tGenerateRateMessage(&messages,&node,&listAPs));
	listTasks.add(new tReadRateMessage(&messages,&listAPs));	
	listTasks.add(new tUGenerateMessage(&messages,&node));
	scheduler.set_tasks(&listTasks);
	scheduler.Init();	
}

void SnorlaxMesh::run(){
	scheduler.run();
}