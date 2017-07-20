#ifndef __SCHEDULERNODE_H__
#define __SCHEDULERNODE_H__

#include <Arduino.h>

#include <ArduinoJson.h>
#include <LinkedList.h>
#include <FS.h>

#include "Task.h"

#include "messageBroker.h"
#include "AP.h"
#include "broadcastNode.h"

enum States{NONE,ADVISE,SCAN,ACTIONS,SEND,SLEEP};

class SchedulerNode{

private:
	//amount time
	unsigned long total_time;
	//Period
	unsigned long period_ms;

	//Advise	
	
	unsigned long more_random_time_advise_ms;
	unsigned long duration_advise_ms;
	unsigned long next_time_advise_ms;
	unsigned long time_setup_next_advise_ms;
	bool time_of_advise();

	void do_Advise();
	void make_Advise();
	
	//List of Tasks of User
	LinkedList<Task*> *tasks;

	// Machine States
	int state;
	void machineStates();
	
	//List of APs
	
	LinkedList<AP*> *APs;
	
	//Message Broker
	
	messageBroker * messages;
	
	//Node
	
	broadcastNode * node;
	
	//Methods set resources
	void set_APs(LinkedList<AP*> *_APs);
	
	void set_messageBroker(messageBroker * _messages);
	
	void set_node(broadcastNode * _node);
	
	//
	unsigned long calculate_period();	

	unsigned long time_now();
	
	void save_configuration();
	
	void load_configuration();
	

	
	
public:
		SchedulerNode();

	void TestLoadSave();
	void Init();
	
};

#endif
