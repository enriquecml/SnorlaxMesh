#include "SchedulerNode.h"

SchedulerNode::SchedulerNode(){
	
}

void SchedulerNode::machineStates(){
	switch(state){
		case ADVISE:
			state=ACTIONS;
		break;
		case SCAN:
		break;
		case ACTIONS:
			if(time_of_advise())
				state=ADVISE;
		break;
		case SEND:
		break;
		case SLEEP:
		break;		
	}
}



void SchedulerNode::make_Advise(){
	
	time_setup_next_advise_ms=time_now();
	next_time_advise_ms+=period_ms;
	//Do advise
}

void SchedulerNode::do_Advise(){
	unsigned long init_time=time_now();
	String msg;
	bool flag=true;
	
	node->upWiFi();
	
	node->modeAp();
	node->initServer();	

	while(flag){
		
		if(node->readMessage(msg))
			messages->addMessageToReviewQueue(msg);
		
		if(time_now()-init_time>=duration_advise_ms)
			flag=false;
	}
	
	node->downWiFi();	
	
	while(node->readMessage(msg))
		messages->addMessageToReviewQueue(msg);
	
	node->closingServer();
}

unsigned long SchedulerNode::calculate_period(){
	int number_tasks;
	unsigned long amount_ms;

	amount_ms=more_random_time_advise_ms+duration_advise_ms;
	if(tasks!=NULL){
		number_tasks=tasks->size();
		for(int i=0;i<number_tasks;i++){
			amount_ms+=tasks->get(i)->duration_ms;
		}
	}
	return amount_ms;
}

unsigned long SchedulerNode::time_now(){
	return total_time+millis();
}

bool SchedulerNode::time_of_advise(){
	return time_now()-time_setup_next_advise_ms>=period_ms;	
}

void SchedulerNode::save_configuration(){
	DynamicJsonBuffer jsonBuffer;

	JsonObject& root = jsonBuffer.createObject();

	root["total_time"]=total_time;
	root["period_ms"]=period_ms;
	root["more_random_time_advise_ms"]=more_random_time_advise_ms;
	root["duration_advise_ms"]=duration_advise_ms;
	root["time_setup_next_advise_ms"]=time_setup_next_advise_ms;
	root["state"]=state;

	SPIFFS.begin();
	File configFile = SPIFFS.open("/config.json", "w");
	root.printTo(Serial);
	root.printTo(configFile);
	configFile.close();
	SPIFFS.end();
}

void SchedulerNode::load_configuration(){
	SPIFFS.begin();
	File configFile = SPIFFS.open("/config.json", "r");
	String line=configFile.readString();
	Serial.println("...");
	Serial.println(line);
	Serial.println("...");	
	configFile.close();
	SPIFFS.end();
	
}

void SchedulerNode::set_APs(LinkedList<AP*> *_APs){
	APs=_APs;
}

void SchedulerNode::set_messageBroker(messageBroker * _messages){
	messages=_messages;
}

void SchedulerNode::set_node(broadcastNode * _node){
	node=_node;
}

void SchedulerNode::TestLoadSave(){
	save_configuration();
	load_configuration();
}

void SchedulerNode::Init(){
	
}