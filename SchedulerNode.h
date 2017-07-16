#ifndef __SCHEDULERNODE_H__
#define __SCHEDULERNODE_H__

#include "broadcastNode.h"
#include "PubBase.h"
#include "SubBase.h"
#include "SingletonStats.h"
//10 MINUTES
#define PERIOD_MAX_MS 150000
#define PERIOD_MIN_MS 90000
//2 MINUTES
enum States{NONE,ADVISE,SCAN,ACTIONS,SEND,SLEEP};
class SchedulerNode{

private:

	int state;

	unsigned long min_time_ms;
	unsigned long random_more_time_s;
	unsigned long time_of_receive_ms;
	
	unsigned long period_ms;	
	volatile int signal_of_receive;
	Ticker alarm_of_receive;
	unsigned long next_time_receive_ms;	
	
	Ticker alarm_of_send;
	bool waiting_for_send;
	volatile  int signal_of_send;
    unsigned long time_next_send;
	unsigned long duration_send_ms;
    String ssid_to_send;

	bool scanning;
	unsigned long total_time_scanned;
	unsigned long total_time_scanning;
	
	broadcastNode node;
	messageBroker _messages;	

	LinkedList<PubBase*> publicators;
	int position_publicator_generate;
	
	LinkedList<SubBase*> subscriptors;
	void advise();
	void scan();
	void send();
	
bool subscriptors_read_messages();	
	
public:

bool all_publicates();

	SchedulerNode();
	void stateMachine();	
	void do_run();
	
bool prepare_scanning();

bool prepare_send();

bool process_messages();	
};

#endif
