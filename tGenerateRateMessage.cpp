#include "tGenerateRateMessage.h"

tGenerateRateMessage::tGenerateRateMessage(messageBroker * _messages,broadcastNode * _node,APs * _aps){
	duration_ms=0;
	own_sequence=0;
	messages=_messages;
	node=_node;
	aps=_aps;
	lastNumberAPS=0;
}

void tGenerateRateMessage::execute(){

	int nMessagesSend=messages->sizeOfMessagesReadyToSend();
	String msg;

	DynamicJsonBuffer jsonBuffer;

	
	JsonObject& root = jsonBuffer.createObject();
	root["rate"]=SingletonStats::instance()->n_messages_received/(SingletonStats::instance()->n_messages_removed +1); //aps->numberAPs();
	root["channel"]=String("_RATE");
	String ssid;
	node->getSSID(ssid);
	root["id"]=String(ssid);
	root.printTo(msg);	

	if(messages->sizeOfMessagesReadyToSend()==0)
		messages->addMessageToSendQueue(msg);	
	else
		messages->addMessageToSendQueue(0,msg);	
	
	

}