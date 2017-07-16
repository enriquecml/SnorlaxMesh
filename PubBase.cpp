#include "PubBase.h"

  PubBase::PubBase(){
	publicated=false;
	 channel=String("INFO_STATS");  
  }
  
  void PubBase::generatePubMessage(String &sJson,String &origin_ssid){
	DynamicJsonBuffer jsonBuffer;

	JsonObject& root = jsonBuffer.createObject();
	JsonObject& data = root.createNestedObject("data");
	JsonArray& ids = root.createNestedArray("ids");

	data["n_try_connections"]=SingletonStats::instance()->n_try_connections++;
	data["n_failed_connections"]=SingletonStats::instance()->n_failed_connections++;
	data["n_scanned"]=SingletonStats::instance()->n_scanned++;
	data["n_messages_sent"]=SingletonStats::instance()->n_messages_sent++;
	data["n_messages_received"]=SingletonStats::instance()->n_messages_received++;
	data["n_messages_repeatly"]=SingletonStats::instance()->n_messages_repeatly++;
	data["n_messages_wasHere"]=SingletonStats::instance()->n_messages_wasHere++;
	data["n_messages_removed"]=SingletonStats::instance()->n_messages_removed++;
	data["n_scanned2"]=SingletonStats::instance()->n_scanned;
	data["n_messages_sent2"]=SingletonStats::instance()->n_messages_sent;
	data["n_messages_received2"]=SingletonStats::instance()->n_messages_received;
	data["n_messages_repeatly2"]=SingletonStats::instance()->n_messages_repeatly;
	data["n_messages_wasHere2"]=SingletonStats::instance()->n_messages_wasHere;
	data["n_messages_removed2"]=SingletonStats::instance()->n_messages_removed;	
	
	data["n_sleeps"]=SingletonStats::instance()->n_sleeps++;
	
	root["channel"]=String(channel);
	root["sequence"]=millis();
	root["n_ids"]=1;
	ids.add(origin_ssid);
	root.printTo(sJson);
	publicated=true;
  }