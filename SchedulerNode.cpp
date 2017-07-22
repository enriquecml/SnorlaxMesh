#include "SchedulerNode.h"

SchedulerNode::SchedulerNode(){
	
}

void SchedulerNode::machineStates(){
	switch(state){
		case SETUP:
			state=ADVISE;
		break;
		case ADVISE:
			if(scan)
				state=SCAN;
			else
				state=ACTIONS;
		break;
		case SCAN:
			if(time_of_advise())
				state=ADVISE;
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

void SchedulerNode::run(){
	switch(state){
		case SETUP:
			if(listAPs->numberAPs()==0){
				make_Scan();
			}
		break;
		case ADVISE:
			make_Advise();
			do_Advise();
		break;
		case SCAN:
			do_Scan();
		break;
		case ACTIONS:
			
		break;
		case SEND:
		break;
		case SLEEP:
		break;		
	}
	machineStates();
}



void SchedulerNode::make_Advise(){
	
	time_setup_next_advise_ms=time_now();
	next_time_advise_ms+=period_ms;
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
		yield();
	}
	
	node->downWiFi();	
	while(node->readMessage(msg))
		messages->addMessageToReviewQueue(msg);
	
	node->closingServer();
}

void SchedulerNode::do_Scan(){

	unsigned long duration_ms=min(substract(duration_scan_ms,time_scanned),substract(next_time_advise_ms,time_now()));
	if(duration_ms!=0){
		time_scanned+=duration_ms;				
		unsigned long init_time=time_now();
		bool flag=true;
		int n;
		String saux;
		unsigned long pointTime1;
		unsigned long pointTime2;		
		LinkedList<String> APs_filtered;		
		node->upWiFi();

		while(flag){
			APs_filtered.clear();		
			pointTime1=time_now();
			n=node->scan(APs_filtered);
			pointTime2=time_now();
			for(int i=0;i<n;i++){
				saux=APs_filtered.get(i);
				updateAP(saux,(pointTime2-pointTime1)/2);
			}
			if(time_now()-init_time>=duration_ms)
				flag=false;
		}
		node->downWiFi();
	}
	else{
		scan=false;
	}
}
	
void SchedulerNode::make_Scan(){
	scan=true;
	time_scanned=0;
}

void SchedulerNode::updateAP(String & sAP,unsigned long time_saw){

	AP * aux =listAPs->giveAP(sAP);
	if(aux==NULL){
		AP * aux;		
		aux = new AP();
		aux->ssid=String(sAP);
		sAP.remove(0,sAP.indexOf('S')+1);
		aux->period_s=sAP.toInt();
		aux->time_saw=time_saw;		
		listAPs->addAP(aux);		
	}
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

void SchedulerNode::set_APs(APs *_listAPs){
	listAPs=_listAPs;
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
	state=SETUP;
	more_random_time_advise_ms=DURATION_RANDOM_TIME_ADVISE_MS;
	duration_advise_ms=DURATION_RANDOM_TIME_ADVISE_MS;
	duration_scan_ms=DURATION_SCAN_MS;
	period_ms=max(MIN_PERIOD_MS,calculate_period());
	String ssid;
	node->getSSID(ssid);
	ssid+=String("S")+String(period_ms/1000);
	node->setSSID(ssid);
}