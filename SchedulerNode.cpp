#include "SchedulerNode.h"

SchedulerNode::SchedulerNode(){

}

void SchedulerNode::machineStates(){
	switch(state){
		case SETUP:
			while(ESP8266TrueRandom.random(MAX_PERIOD_MS)!=0)
				yield();
			state=ADVISE;			
		break;
		case ADVISE:
		
			if(scan)
				state=SCAN;
			else
				if(time_of_send())
					state=SEND;
				else
					state=ACTIONS;
		break;
		case SCAN:
			if(time_of_advise())
				state=ADVISE;
		break;
		case ACTIONS:
		
			if(time_of_advise()){
				state=ADVISE;
			}
			else{
				if(time_of_send())
					state=SEND;
				else{
					if(iterator_task==tasks->size()){
						
						SingletonStats::instance()->n_sleeps++;						
						state=SLEEP;
						messages->show_queues();
					}
				}
			}
		break;
		case SEND:
		
			if(time_of_advise())
				state=ADVISE;
			else	
				state=ACTIONS;
		break;
		case SLEEP:
		
			if(time_of_advise())
				state=ADVISE;
			else
				if(time_of_send())
					state=SEND;		
		break;
		
	}
}

void SchedulerNode::run(){
	
	switch(state){
		case SETUP:

		break;
		case ADVISE:
		
			make_Advise();
			do_Advise();
			make_new_send=true;
			iterator_task=0;			
		break;
		case SCAN:
		
			do_Scan();
		break;
		case ACTIONS:
		
			if(listAPs->numberAPs()==0 || lostConnection){
				make_Scan();
			}
			else
				make_Send();
					
				Serial.print(iterator_task);
				Serial.print("==");
				Serial.println(tasks->size());
				
				if(iterator_task<tasks->size()){
					
					tasks->get(iterator_task)->execute();
					iterator_task++;
				}
				
		break;
		case SEND:		
			do_Send();	
		break;
		case SLEEP:
			
		break;		
	}
	machineStates();
	
}



void SchedulerNode::make_Advise(){

	time_setup_next_advise_ms=time_now();
	next_time_advise_ms=time_setup_next_advise_ms+period_ms;
	
}

void SchedulerNode::do_Advise(){
	
	unsigned long init_time=time_now();
	DEBUG_SNORLAX_SCHEDULERNODE(Serial.print(String("init_time:")));
	DEBUG_SNORLAX_SCHEDULERNODE(Serial.println(init_time));	
	unsigned long timeAdvise=duration_advise_ms+ESP8266TrueRandom.random(2)*more_random_time_advise_ms;
	DEBUG_SNORLAX_SCHEDULERNODE(Serial.print(String("timeAdvise:")));
	DEBUG_SNORLAX_SCHEDULERNODE(Serial.println(timeAdvise));		
	String msg;
	bool flag=true;
	
	node->upWiFi();
	node->modeAp();
	node->initServer();	

	while(flag){
		node->addNewClientConnection();
		if(node->readMessage(msg)){
			messages->addMessageToReviewQueue(msg);
			SingletonStats::instance()->n_messages_received++;
		}
		
		if(time_now()-init_time>=timeAdvise)
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
		lostConnection=false;
	}
}
	
void SchedulerNode::make_Scan(){
	if(!scan){
		scan=true;
		time_scanned=0;
		SingletonStats::instance()->n_scanned++;
	}
}

void SchedulerNode::updateAP(String & sAP,unsigned long time_saw){
	Serial.print("actualizar AP:");
	Serial.println(sAP);
	AP * aux =listAPs->giveAP(sAP);
	if(aux==NULL){		
		aux = new AP();
		aux->ssid=String(sAP);
		sAP.remove(0,sAP.indexOf('S')+1);
		aux->period_s=1000*sAP.toInt();		
		listAPs->addAP(aux);
		aux->time_saw=time_saw;
		aux->nTry=0;		
	}

}

bool SchedulerNode::time_of_send(){
	return send && time_now()-time_setup_next_send_ms>=next_time_send_ms;
}

bool SchedulerNode::nextMessageToSend(String &_msg){

	//send RATE CONTROL MESSAGE
	if(!sentRate){
		messages->getMessageReadyToSend(0,_msg);
		sentRate=true;
		return true;
	}
	
	//TRY SEND DATA MESSAGE
	AP * aux =listAPs->giveAP(ssid_to_send);
	int position=aux->positionMessage;
	if(position<messages->sizeOfMessagesReadyToSend()){
		messages->getMessageReadyToSend(position,_msg);
		aux->positionMessage++;
		return true;
	}
	return false;	
}

void SchedulerNode::do_Send(){

	unsigned long time_to_advise=substract(next_time_advise_ms,time_now());
	Serial.print(String("tiempo hasta el siguiente modo AP:"));
	Serial.println(time_to_advise);
	unsigned long left_time=substract(duration_send_ms,time_sending);
	unsigned long duration_ms=min(left_time,time_to_advise);
	if(duration_ms!=0){
		time_sending+=duration_ms;				
		unsigned long init_time=time_now();
		
		
		bool flag=true;
		node->upWiFi();
		node->tryConnect(ssid_to_send);
		while(flag){
			if(node->Connected()){
				if(node->connectedToServer()){
					connectedToServer=true;
					String msg;
					if(nextMessageToSend(msg)){
						SingletonStats::instance()->n_messages_sent++;							
						node->sendToServer(msg);
					}
				}
				else{
					node->InitClient();
				}
			}
			if(time_now()-init_time>=duration_ms)
				flag=false;
			yield();
		}
		node->stopClient();
		node->downWiFi();
	}
	
	if(time_sending==duration_send_ms){
		Serial.println(String("Cerrando envio"));
		AP * aux=listAPs->giveAP(ssid_to_send);
		//Update tries
		if(!connectedToServer){
			SingletonStats::instance()->n_failed_connections++;							
			aux->nTry++;
			if(aux->nTry==3){
				lostConnection=true;
				listAPs->removeAP(ssid_to_send);
			}
		}
		else{
			aux->nTry=0;
			aux->connected=true;			
		}
		SingletonStats::instance()->n_try_connections++;	
		connectedToServer=false;	
		send=false;
	}
	
}

void SchedulerNode::make_Send(){
	if(send==false && make_new_send==true){
		
		make_new_send=false;		
		sentRate=false;
		
		//search next Send
		AP *aux;
		bool foundAP=false;
		int n=listAPs->numberAPs();
		
		for(int i=0;i<n && !foundAP;i++){
			aux=listAPs->giveAP(i);
			if(aux->connected==false)
				foundAP=true;
		}
		
		if(!foundAP){
			//Reset connected
			for(int i=0;i<n;i++){
				aux=listAPs->giveAP(i);
				aux->connected=false;
			}		
		}
		
		bool canConnect=false;
		
		for(int i=0;i<n && !canConnect;i++){
			aux=listAPs->giveAP(i);
			if(!aux->connected && 0==ESP8266TrueRandom.random(aux->rate))
				canConnect=true;
		}		
		
		if(canConnect){
			send=true;
			time_sending=DURATION_SEND_MS;		
			ssid_to_send=aux->ssid;
			time_sending=0;
			time_setup_next_send_ms=time_now();		
			next_time_send_ms=aux->period_s-((time_now()-aux->time_saw)%aux->period_s);
			Serial.print("Milisegundos para enviar:");
			Serial.println(next_time_send_ms);			
		}
		else{
			send=false;
		}
			
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
	unsigned long ms=min(MAX_PERIOD_MS,max(MIN_PERIOD_MS,amount_ms));
	Serial.print("Milisegundos del periodo");
	Serial.println(ms);
	return ms;
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

void SchedulerNode::set_tasks(LinkedList<Task*> *_tasks){
	tasks=_tasks;
}

void SchedulerNode::TestLoadSave(){
	save_configuration();
	load_configuration();
}

void SchedulerNode::Init(){
	state=SETUP;
	more_random_time_advise_ms=DURATION_RANDOM_TIME_ADVISE_MS;
	duration_advise_ms=DURATION_ADVISE_MS;
	duration_scan_ms=DURATION_SCAN_MS;
	duration_send_ms=DURATION_SEND_MS;
	period_ms=calculate_period();
	String ssid;
	node->getSSID(ssid);
	ssid+=String("S")+String(period_ms/1000);
	node->setSSID(ssid);
	send=false;
	scan=false;
	lostConnection=false;
	connectedToServer=false;	
	iterator_task=0;
}