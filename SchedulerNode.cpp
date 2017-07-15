#include "SchedulerNode.h"

SchedulerNode::SchedulerNode(){
	WiFi.setAutoConnect(false);
	//WiFi.setAutoReconnect(false);
	publicators.add(new PubBase());
	subscriptors.add(new SubBase());
    position_publicator_generate=0;	
	randomSeed(ESP.getChipId());
	
	state=1;//load

	min_time_ms=8000;
	random_more_time_s=8;
	time_of_receive_ms=min_time_ms+random(random_more_time_s)*1000;
	
	period_ms=120000;	
	signal_of_receive=0;//load
	
	scanning=false;//load
	total_time_scanning=period_ms;//at moment same that period advise
	total_time_scanned=0;

	waiting_for_send=false;//load
	signal_of_send=1;//load
	next_time_receive_ms=millis();//load

	duration_send_ms=8000;
	
}


unsigned long min(unsigned long a,unsigned long b){
	if(a>b)
		return b;
	return a;
}

void SchedulerNode::stateMachine(){
	switch(state){
		case ADVISE:
			if(scanning){
				state=SCAN;//SCAN
				break;
			}
			if(signal_of_send==0){
				state=4;//SEND MESSAGES
				break;
			}
			state=3;//PROCESS
		break;
		case 2:
			if(signal_of_receive==0){
				state=1;
				break;
			}
			state=3;
			
		break;
		case 3:
			if(signal_of_receive==0){
				state=1;//ADVISE
				break;
			}
			if(signal_of_send==0){
				state=4;//SEND MESSAGES
				break;
			}				
		break;
		case 4:
			if(signal_of_receive==0){
				state=1;//ADVISE
			}
			else{
				state=3;				
			}			
		break;
		case 5:
			if(signal_of_receive==0){
				state=1;//ADVISE
				break;
			}
			if(signal_of_send==0){
				state=4;//SEND MESSAGES
				break;
			}									
		break;
	}
}

bool SchedulerNode::prepare_scanning(){
	
	if( (scanning == false)	&& (_messages.isLostConnectionWithAP() || _messages.numberOfAPs()==0)  ){
		total_time_scanned=0;		
		scanning=true;
		return true;
	}
	return false;
}

bool SchedulerNode::all_publicates(){
	DEBUG_MSG("Generando publicaciones");

	for(int i=0;i<publicators.size();i++){
		if(!publicators.get(i)->publicated){
			String sJson;
			publicators.get(i)->generatePubMessage(sJson,node._ssid);
			_messages._messages_ready_to_send.add(sJson);
			return true;			
		}
	}
	return false;
}

bool SchedulerNode::subscriptors_read_messages(){
	if(_messages._messages_without_read.size()>0){
		DEBUG_MSG("Intentando leer mensaje");
		String channel;
		String origin;
		unsigned long sequence;
		_messages.extractChannelAndSequence(_messages._messages_without_read.get(0),channel,sequence,origin);
		for(int i=0;i<subscriptors.size();i++){
			if(subscriptors.get(i)->channel.equals(channel)){
				if(subscriptors.get(i)->read(_messages._messages_without_read.get(0))){
					_messages._messages_ready_to_send.add(_messages._messages_without_read.get(0));
					_messages._messages_without_read.remove(0);
					return true;					
				}
			}		
		}//we should look all messages for subscriptors
		_messages._messages_ready_to_send.add(_messages._messages_without_read.get(0));
		_messages._messages_without_read.remove(0);
		return true;			
	}
	return false;
}

bool SchedulerNode::prepare_send(){

		if(scanning == false && waiting_for_send==false){
			waiting_for_send=true;
			_messages.nextTimeSend(ssid_to_send,time_next_send,duration_send_ms,period_ms,min_time_ms+random_more_time_s*1000);
			signal_of_send=1;
			alarm_of_send.once_ms(time_next_send,int_to_cero,&signal_of_send);
			DEBUG_MSG("Preparado envio para dentro de %d milisegundos",time_next_send);		
			
			return true;
		}
	return false;
}



bool SchedulerNode::process_messages(){
	if(_messages.reviewMessages(node._ssid)){
		DEBUG_MSG("Mensajes revisados mensaje");
		return true;
	}
	else{
		if(all_publicates()){
			return true;
		}
		else{
			if(subscriptors_read_messages()){
				return true;
			}
			else{
				DEBUG_MSG("Eliminando mensajes");
				if(_messages.removeMessage(publicators))
					return true;
			}			
		}		
	}
	SingletonStats::instance()->n_sleeps++;	
	return false;
}

void SchedulerNode::advise(){
	next_time_receive_ms=millis()+period_ms;
	alarm_of_receive.once_ms(period_ms,int_to_cero,&signal_of_receive);				
	node.modeCatchDataMessages(time_of_receive_ms,&_messages);
	time_of_receive_ms=min_time_ms+random(random_more_time_s)*1000;
	signal_of_receive=1;	
}

void SchedulerNode::scan(){
			
			unsigned long time_of_scan=min(substract(total_time_scanning,total_time_scanned),substract(next_time_receive_ms,millis()));
			if(time_of_scan!=0){
				total_time_scanned+=time_of_scan;				
				node.scan(time_of_scan,&_messages,min_time_ms+random_more_time_s*1000,min_time_ms);
			}	
			else{
				SingletonStats::instance()->n_scanned++;								
				scanning=false;
			}	
}

void SchedulerNode::send(){

				unsigned long duration=min(duration_send_ms,substract(next_time_receive_ms,millis()));
				if(duration!=0 && _messages.isLostConnectionWithAP()==false){
					if(duration!=duration_send_ms)//send interrupted for advise
						duration_send_ms=substract(duration_send_ms,time_of_receive_ms);
					duration_send_ms=substract(duration_send_ms,duration);
					node.trySendMessages(duration,&_messages,ssid_to_send);					
				}
				else{
					waiting_for_send=false;
					signal_of_send=1;	
				}
}				


 void SchedulerNode::do_run(){
	switch(state){
		case 1://Receive messages period	
				advise();
				stateMachine();
				DEBUG_MSG("state %i\n", state);
		break;
		case 2:
			scan();
			stateMachine();
		break;
		case 3:
			
			if(prepare_scanning()){
				
			}
			else{
				if(process_messages()){
					//process maybe duplicate
					//process_message_stamp
					//generatePub					
					//sub
					//process_eliminate							
				}
				else{
					if(prepare_send()){
						//generateNextTimeSend
					}				
					else{
						DEBUG_MSG("Dormir tras ...");
						state=5;
					}					
				}
				}			
			stateMachine();
		break;
		case 4:
			send();
			stateMachine();		
		break;
		case 5:
			//sleep
			stateMachine();
		break;
	}
 }
