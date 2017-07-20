
#include "broadcastNode.h"

broadcastNode::broadcastNode():_server(SERVER_PORT)
{
	chip_id = ESP.getChipId();
	ssid = String( String( SSID_PREFIX ) + String( _chip_id ) );  
}

void broadcastNode::upWiFi(){
	WiFi.forceSleepWake();
}

void broadcastNode::downWiFi(){
  WiFi.forceSleepBegin();
}
  
void broadcastNode::modeAp(){
	WiFi.softAP(ssid.c_str());
}

void broadcastNode::initServer(){
	server.begin();
}

bool broadcastNode::readMessage(String &msg){
	if(server.hasClient()){
		client=server.available();
		if(client.connected() && client.available()){
			msg=client.readString();
			if(msg.length()>0){
				return true;
			}
		}
	}
	return false;
}

void broadcastNode::closingServer(){
	while(server.hasClient()){
		client=server.available();
		client.flush();
		client.stop();		
	}
		server.stop();	
}