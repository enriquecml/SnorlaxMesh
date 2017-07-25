#include "tUGenerateMessage.h"

tUGenerateMessage::tUGenerateMessage(){
	own_sequence=0;
}

void tUGenerateMessage::execute(){
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
	node->getSSID(ssid)
	ids.add(ssid);
	String msg;
	root.printTo(msg);
	messages_ready_to_send.add(msg);
}