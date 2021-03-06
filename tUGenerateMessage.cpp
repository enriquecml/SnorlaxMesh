#include "tUGenerateMessage.h"

tUGenerateMessage::tUGenerateMessage(messageBroker * _messages,broadcastNode * _node){
	duration_ms=0;
	own_sequence=0;
	messages=_messages;
	node=_node;
}

void tUGenerateMessage::execute(){
	Serial.println("Generando Mensaje");
	DynamicJsonBuffer jsonBuffer;

	JsonObject& root = jsonBuffer.createObject();
	JsonObject& data = root.createNestedObject("data");
	JsonArray& ids = root.createNestedArray("ids");

	data["temperature"]=32.5;
		
	root["channel"]=String("TEMPERATURE");
	own_sequence++;
	root["sequence"]=own_sequence;
	root["n_ids"]=1;
	
	String ssid;
	node->getSSID(ssid);
	ids.add(ssid);
	String msg;
	root.printTo(msg);
	messages->addMessageToSendQueue(msg);
}