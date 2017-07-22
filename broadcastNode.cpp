
#include "broadcastNode.h"

broadcastNode::broadcastNode():server(SERVER_PORT)
{
	WiFi.setAutoConnect(false);
 	WiFi.persistent(false);
	downWiFi();	
	chip_id = ESP.getChipId();
	ssid = String( String( SSID_PREFIX ) + String( chip_id ) );  
}

void broadcastNode::upWiFi(){
	WiFi.forceSleepWake();
		delay(1);
}

void broadcastNode::downWiFi(){
WiFi.disconnect(true); 
WiFi.forceSleepBegin();
		delay(1);

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

int broadcastNode::scan(LinkedList<String> &APs_filtered){
	String filter=String(SSID_PREFIX);
    int n = WiFi.scanNetworks();
    for (int i = 0; i < n; ++i) {
		if(WiFi.SSID(i).startsWith(filter))
			APs_filtered.add(WiFi.SSID(i));  
    }
	return APs_filtered.size();
}

void broadcastNode::getSSID(String &_ssid){
	_ssid=ssid;
}

void broadcastNode::setSSID(String &_ssid){
	ssid=_ssid;
}