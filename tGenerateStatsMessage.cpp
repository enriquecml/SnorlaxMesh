#include "tGenerateStatsMessage.h"

tGenerateStatsMessage::tGenerateStatsMessage(messageBroker * _messages,broadcastNode * _node){
	duration_ms=0;
	own_sequence=0;
	messages=_messages;
	node=_node;
}

void tGenerateStatsMessage::execute(){
	DynamicJsonBuffer jsonBuffer;

	JsonObject& root = jsonBuffer.createObject();
	JsonObject& data = root.createNestedObject("data");
	JsonArray& ids = root.createNestedArray("ids");

	data["n_try_connections"]=SingletonStats::instance()->n_try_connections;
	data["n_failed_connections"]=SingletonStats::instance()->n_failed_connections;
	data["n_scanned"]=SingletonStats::instance()->n_scanned;
	data["n_messages_sent"]=SingletonStats::instance()->n_messages_sent;
	data["n_messages_received"]=SingletonStats::instance()->n_messages_received;
	data["n_messages_repeatly"]=SingletonStats::instance()->n_messages_repeatly;
	data["n_messages_wasHere"]=SingletonStats::instance()->n_messages_wasHere;
	data["n_messages_removed"]=SingletonStats::instance()->n_messages_removed;
	data["n_sleeps"]=SingletonStats::instance()->n_sleeps;

	root["channel"]=String("_STATS");
	root["sequence"]=millis();
	root["n_ids"]=1;
	String ssid;
	node->getSSID(ssid);
	ids.add(ssid);
	String msg;
	root.printTo(msg);
	messages->addMessageToSendQueue(msg);				
}