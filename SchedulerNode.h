#ifndef __SCHEDULERNODE_H__
#define __SCHEDULERNODE_H__

#include <Arduino.h>

#include <ArduinoJson.h>
#include <LinkedList.h>
#include <FS.h>

#include "Helper.h"

#include "Task.h"

#include "messageBroker.h"
#include "APs.h"
#include "broadcastNode.h"
#include "SingletonStats.h"
#include <ESP8266TrueRandom.h>

#define DEBUG_SNORLAX_SCHEDULERNODE 1
#ifdef  DEBUG_SNORLAX_SCHEDULERNODE
	#define DEBUG_SNORLAX_SCHEDULERNODE(...) __VA_ARGS__
#endif

#define MIN_PERIOD_MS 180000
#define MAX_PERIOD_MS 180000
#define DURATION_SCAN_MS MAX_PERIOD_MS
#define DURATION_ADVISE_MS 8000
#define DURATION_RANDOM_TIME_ADVISE_MS 8000
#define DURATION_SEND_MS 8000

enum States{SETUP,ADVISE,SCAN,ACTIONS,SEND,SLEEP};

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
	
	//Scan
	bool lostConnection;
	bool scan;
	unsigned long time_scanned;
	unsigned long duration_scan_ms;
	void do_Scan();
	void make_Scan();
	
	void updateAP(String & sAP,unsigned long time_saw);
	
	//SEND
	
	bool sentRate;
	bool send;
	bool make_new_send;
	unsigned long next_time_send_ms;
	unsigned long time_setup_next_send_ms;
	unsigned long duration_send_ms;
	unsigned long time_sending;
	bool connectedToServer;
	bool time_of_send();
	
	String ssid_to_send;
	bool nextMessageToSend(String &_msg);
	
	void do_Send();
	void make_Send();
	
	//List of Tasks of User
	LinkedList<Task*> *tasks;
	int iterator_task;
	// Machine States
	int state;
	void machineStates();
	
	//List of APs
	
	APs * listAPs;
	
	//Message Broker
	
	messageBroker * messages;
	
	//Node
	
	broadcastNode * node;
	
	unsigned long calculate_period();	

	unsigned long time_now();
	
	void save_configuration();
	
	void load_configuration();
	

	
	
public:
		SchedulerNode();
		
	//Methods set resources
	void set_APs(APs *_listAPs);
	
	void set_messageBroker(messageBroker * _messages);
	
	void set_node(broadcastNode * _node);

	void set_tasks(LinkedList<Task*> *_tasks);	
	//
	void run();	
	
	void TestLoadSave();
	void Init();
	
};

#endif
