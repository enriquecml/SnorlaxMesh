#include "tGenerateRateMessage.h"

tGenerateRateMessage::tGenerateRateMessage(messageBroker * _messages,broadcastNode * _node,APs * _aps){
	duration_ms=0;
	own_sequence=0;
	messages=_messages;
	node=_node;
	aps=_aps;
	lastNumberAPS=0;
}

bool tGenerateRateMessage::isMessageOwnAboutRate(String &_msg){
	DynamicJsonBuffer jsonBuffer;
	JsonObject& root = jsonBuffer.parseObject(_msg);

	const char * channel = root["channel"];
	String Channel=String(channel);
	
	if(Channel.equals(String("_RATE"))){
		const char * origin = root["id"];
		String Origin=String(origin);
		String ssid;		
		node->getSSID(ssid);			
		return Origin.equals(ssid);
	}
	
	return false;		
}

void tGenerateRateMessage::execute(){

	int nMessagesSend=messages->sizeOfMessagesReadyToSend();
	bool found=false;
	String msg;
	int i;
	for(i=0;i<nMessagesSend && !found;i++){
		msg=String("");
		messages->getMessageReadyToSend(i,msg);
		if(isMessageOwnAboutRate(msg)){
			found=true;
		}
	}
	DynamicJsonBuffer jsonBuffer;

	JsonObject& root = jsonBuffer.createObject();
	root["rate"]=aps->numberAPs();
	root["channel"]=String("_RATE");
	String ssid;
	node->getSSID(ssid);
	root["id"]=String(ssid);
	root.printTo(msg);	
	if(found){
		messages->addMessageToSendQueue(i,msg);
	}
	else{
		messages->addMessageToSendQueue(msg);		
	}
	

}