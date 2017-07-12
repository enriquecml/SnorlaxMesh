#include "SchedulerNode.h"

SchedulerNode::SchedulerNode(){
	WiFi.setAutoConnect(false);
	publicators.add(new PubBase());
	subscriptors.add(new SubBase());
    position_publicator_generate=0;	
	randomSeed(millis());
	
	state=1;//load

	min_time_ms=5000;
	random_more_time_s=5;
	time_of_receive_ms=min_time_ms+random(random_more_time_s)*1000;
	
	period_ms=120000;	
	signal_of_receive=0;//load
	
	scanning=false;//load
	total_time_scanning=0;	

	waiting_for_send=false;//load
	signal_of_send=1;//load
	next_time_receive_ms=millis();//load

	duration_send_ms=5000;
	
}


unsigned long min(unsigned long a,unsigned long b){
	if(a>b)
		return b;
	return a;
}

void SchedulerNode::stateMachine(){
	switch(state){
		case 1:
			if(scanning){
				state=2;//SCAN
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
				break;
			}
			state=3;
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
	
	if(_messages.isLostConnectionWithAP() || _messages.numberOfAPs()==0 ){
		total_time_scanning=0;		
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
		}
	}
	return true;
}

bool SchedulerNode::prepare_send(){

		if(!waiting_for_send){
			waiting_for_send=true;
			_messages.nextTimeSend(ssid_to_send,time_next_send,duration_send_ms,period_ms,min_time_ms);
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
		if(!all_publicates()){
			return true;
		}
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
					}
				return true;
				}		
			}
			_messages._messages_ready_to_send.add(_messages._messages_without_read.get(0));
			_messages._messages_without_read.remove(0);
			return true;			
		}
		else{
			DEBUG_MSG("Eliminando mensajes");
			if(_messages.removeMessage(publicators))
				return true;
		}	
		
	}
	SingletonStats::instance()->n_sleeps++;	
	return false;
}

 void SchedulerNode::do_run(){
	switch(state){
		case 1://Receive messages period	
				next_time_receive_ms=millis()+period_ms;
				alarm_of_receive.once_ms(period_ms,int_to_cero,&signal_of_receive);				
				node.modeCatchDataMessages(time_of_receive_ms,&_messages);
				randomSeed(millis());
				time_of_receive_ms=min_time_ms+random(random_more_time_s)*1000;
				signal_of_receive=1;
				stateMachine();
				DEBUG_MSG("state %i\n", state);
		break;
		case 2:
			if(total_time_scanning==0){
		SingletonStats::instance()->n_scanned++;
				
				total_time_scanning=substract(next_time_receive_ms,millis());
				node.scan(total_time_scanning,&_messages,min_time_ms+random_more_time_s*1000,min_time_ms);
			}
			else{
				DEBUG_MSG("segundo escaneo %d en segundos",period_ms-total_time_scanning);
				node.scan(period_ms-total_time_scanning,&_messages,min_time_ms+random_more_time_s*1000,min_time_ms);
				scanning=false;
			}
			stateMachine();
		break;
		case 3:
			
			if(prepare_scanning()){
				
			}
			else{
				if(process_messages()){
					//process maybe duplicate
					//process_message_stamp
					//sub
					//process_eliminate							
				}
				else{
					if(prepare_send()){
						//generatePub
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
			
			node.trySendMessages(min_time(duration_send_ms,substract(next_time_receive_ms,millis())),&_messages,ssid_to_send);
			waiting_for_send=false;
			signal_of_send=1;
			if(signal_of_receive==0){
				state=1;
			}
			else{
				state=3;				
			}			
		break;
		/*case 3:
			waiting_for_send=true;
			_messages.nextTimeSend(time_next_send,ssid_to_send);
			Serial.println("FALTAN DE SEGUNDOS:");
			Serial.println(time_next_send);			
			alarm_of_send.once_ms(time_next_send,int_to_cero,&signal_of_send);
			if(signal_of_receive==0){
				state=1;
			}
			else{
				state=6;
			}			
		break;
		case 4:			
			node.trySendMessages(5000,&_messages,ssid_to_send,next_time_receive_ms,min_time_ms,period_ms);
			waiting_for_send=false;
			signal_of_send=1;
			if(signal_of_receive==0){
				state=1;
			}
			else{
				state=6;				
			}			
		break;*/
		case 5:
			//sleep
			stateMachine();
		break;
	}
 }
