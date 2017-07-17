#include "messageBroker.h"

  void ICACHE_RAM_ATTR int_to_cero(volatile int *flag){
	  *flag=0;
  }
  
    void ICACHE_RAM_ATTR int_to_cero( int *flag){
	  *flag=0;
  }
  
  
  unsigned long globalTime(unsigned long time){
	return time+millis();  
  }
  

unsigned long substract(unsigned long a,unsigned long b){
	if(b>a)
		return 0;
	return a-b;
}

unsigned long min_time( unsigned long a,unsigned long b){
	if(a<b)
		return a;
	else
		return b;
}

messageBroker::messageBroker(){
	//
	total_access=0;
	lostConnectionWithAP=false;
}

bool messageBroker::nextMessage(String &node,String &message){

	int i=givePositionNode(node);
	
	if(_messages_ready_to_send.size()>0 && APs.get(i)->positionMessage<_messages_ready_to_send.size() ){
		SingletonStats::instance()->n_messages_sent++;		
		message=_messages_ready_to_send.get(APs.get(i)->positionMessage);
		APs.get(i)->positionMessage++;
		return true;
	}
	return false;
}

bool messageBroker::removeMessage(LinkedList<PubBase*> &publicators,String &ownSSID){
	if(_messages_ready_to_send.size()>0 && APs.size()>0){
		for(int i=0;i<APs.size();i++){
			if(APs.get(i)->positionMessage==0)
				return false;
		}
	String channel1;
	String origin1;
	String saux;
	unsigned long sequence1;
	saux=_messages_ready_to_send.get(0);
	extractChannelAndSequence(saux,channel1,sequence1,origin1);
	for(int i=0;i<publicators.size();i++){
		if(publicators.get(i)->channel.equals(channel1) && origin1.equals(ownSSID) ){
			publicators.get(i)->publicated=false;
		}
	}	
		_messages_ready_to_send.remove(0);
		for(int i=0;i<APs.size();i++){
			APs.get(i)->positionMessage--;
		}
		SingletonStats::instance()->n_messages_removed++;
		return true;
	}
	return false;
}

bool messageBroker::reviewMessages(String &ssid){
	while(_messages_without_review.size()>0){
		String m=_messages_without_review.get(0);
		Serial.println(String("Mensaje a revisar"));
		Serial.println(m);
		if(!wasHere(m,ssid) && !existMessage(m)){
			DEBUG_MSG("mensaje no existente");
			addNodeToMessage(m,ssid);
			_messages_without_read.add(m);
		}
	_messages_without_review.remove(0);
	
	}
	return false;
}

bool messageBroker::existMessage(String &sJson){
	String channel1;
	String channel2;
	String origin1;
	String origin2;
	String saux;
	unsigned long sequence1;
	unsigned long sequence2;
	extractChannelAndSequence(sJson,channel1,sequence1,origin1);

	for(int i =0;i<_messages_ready_to_send.size();i++){
		saux=_messages_ready_to_send.get(i);
		extractChannelAndSequence(saux,channel2,sequence2,origin2);
					yield();

		if(sequence1 == sequence2 && channel1.equals(channel2) && origin1.equals(origin2)){
			SingletonStats::instance()->n_messages_repeatly++;			
			return true;
		}
	}

	for(int i=0;i<_messages_without_read.size();i++){
		saux=_messages_without_read.get(i);
		extractChannelAndSequence(saux,channel2,sequence2,origin2);
				yield();		

		if(sequence1 == sequence2 && channel1.equals(channel2) && origin1.equals(origin2)){
			SingletonStats::instance()->n_messages_repeatly++;			
			return true;
		}
	}
		
	for(int i = 1;i<_messages_without_review.size();i++){
		saux=_messages_without_review.get(i);
		extractChannelAndSequence(saux,channel2,sequence2,origin2);
		yield();
		if(sequence1 == sequence2 && channel1.equals(channel2) && origin1.equals(origin2)){
			SingletonStats::instance()->n_messages_repeatly++;			
			return true;
		}			
	}
	
	
	return false;
}

void messageBroker::extractChannelAndSequence(String &sJson,String &Channel,unsigned long &sequence,String &Origin){
 DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(sJson);
  JsonArray& ids = root["ids"];
  
  const char * channel = root["channel"];
  Channel=String(channel);
  const char * origin = root["ids"][0];
  Origin=String(origin);
  sequence=root["sequence"];  
	
}

bool messageBroker::wasHere(String &sJson,String &ssid){
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(sJson);
  JsonArray& ids = root["ids"];

  const char * ssid2;
  
  int n_ids=root["n_ids"];
  for(int i=0;i<n_ids;i++){
	  ssid2=root["ids"][i];
	  if(String(ssid2).equals(ssid)){
		SingletonStats::instance()->n_messages_wasHere++;					  
		return true;		
	  }
  }
	return false;
}

  void messageBroker::addNodeToMessage(String &sJson,String &ssid){
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(sJson);
  JsonArray& ids = root["ids"];

  int n_ids=root["n_ids"];
  ids.add(ssid);
  n_ids++;
  root["n_ids"]=n_ids;  
  
  sJson=String("");
  root.printTo(sJson);
  DEBUG_MSG("Nodo agregado resultado:\n");
  Serial.println(sJson);
  }

unsigned long distance(unsigned long a,unsigned long b){
	unsigned long distance;
	if(a>b)
		distance=a-b;
	else
		distance=b-a;
	return distance;
}  
  
void messageBroker::updateAP(String &name_node,unsigned long time_saw,bool scan,unsigned long max_range,unsigned long min_range){
	int position=givePositionNode(name_node);
	if(position!=-1){
		unsigned long d=distance(APs.get(position)->time_saw,time_saw);
		if(d<=max_range){
			APs.get(position)->uncertainty=substract(max_range,d);
		}
		else{
			APs.get(position)->time_saw=time_saw;
			APs.get(position)->uncertainty=max_range;			
		}
	}
	else{
		AP * aux;		
		aux = new AP();
		aux->ssid=String(name_node);
		name_node.remove(0,name_node.indexOf('S')+1);
		Serial.println(name_node);
		aux->period_s=name_node.toInt();
		aux->time_saw=time_saw;
		aux->uncertainty=max_range;		
		APs.add(aux);		
	}
	lostConnectionWithAP=false;
}

bool messageBroker::updateRate(String &sJson){
 String Channel;
 String Origin;
 int p;
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(sJson);
  
  const char * channel = root["channel"];
  Channel=String(channel);
  Serial.println(Channel);
  if(Channel.equals(String("_RATE"))){
	const char * origin = root["id"];
	Origin=String(origin);
	p=givePositionNode(Origin);
	if(p!=-1){
		APs.get(p)->rate=root["rate"];
	}
	return true;
  }
  return false;	
}

void messageBroker::entryMessage(String &sJson){
	SingletonStats::instance()->n_messages_received++;
	if(!updateRate(sJson)){
		DEBUG_MSG("MENSAJE DE DATOS");
		Serial.println(sJson);
	_messages_without_review.add(sJson);
	}
}


  int messageBroker::givePositionNode(String &name_node){
  if(APs.size()>0 && !name_node.equals("")){
    int i=0;
    bool found=false;

    while(i<APs.size() && !found){
      if(APs.get(i)->ssid.equals(name_node)){
        return i;
      }
      else{
        i++;
      }
  }
  }
 return -1;
 }
 
 bool messageBroker::connected_all(){
	/*for(int i=0;i<APs.size();i++){
		if(!APs.get(i)->connected)
			return false;
	}
	return true;*/
}

void messageBroker::resetConnected(){
	AP * aux;
	for(int i=0;i<APs.size();i++){
		aux=APs.get(i);
		aux->connected=false;
	}
}

void messageBroker::putConnected(String &ssid){
	int position= givePositionNode(ssid);
	AP * aux=APs.get(position);
	aux->connected=true;
}

void messageBroker::incrementTry(String &ssid){
	int position= givePositionNode(ssid);
	AP * aux=APs.get(position);
	aux->nTry++;
	if(aux->nTry>5){
		delete(aux);//Eliminate node not accesible
		APs.remove(position);
		lostConnectionWithAP=true;
	}
}

void messageBroker::nextTimeSend(String &ssid,unsigned long &time_next_send,unsigned long &duration_send,unsigned long period_ms,unsigned long min_time_receive){
	int positionAP = -1;
	for(int i=0;i<APs.size();i++){
				yield();
		if(!APs.get(i)->connected){
			positionAP = i;			
		}
	}

	if(positionAP==-1){
		resetConnected();
		positionAP=0;
	}
		ssid=APs.get(positionAP)->ssid;
	

	unsigned long uncertainty=APs.get(positionAP)->uncertainty;
	duration_send=min_time_receive+uncertainty;
	unsigned long time_saw;
	unsigned long period_s=APs.get(positionAP)->period_s;
	period_s=period_s*1000;
	DEBUG_MSG("periodo del ssid a enviar datos %i",period_s);
	time_saw=APs.get(positionAP)->time_saw-uncertainty;	
	if(millis()<=time_saw){
		time_next_send=substract(time_saw,millis());		
	}
	else{
		time_next_send=period_s-((millis()-time_saw)%period_s);
			if(time_next_send<100)//little time now, then put the next period
				time_next_send+=period_s;
	}
if(0!=random(APs.get(positionAP)->rate))
	duration_send=0;
	
} 

bool messageBroker::isLostConnectionWithAP(){
	return lostConnectionWithAP;
}

int messageBroker::numberOfAPs(){
	return APs.size();
}
 
 
