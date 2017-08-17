
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
			if(server.hasClient())
			Serial.println("Encontrado cliente");
	if(client.available()>0){
		msg=client.readString();
		if(msg.length()>0){
			return true;
		//}
		}
	}		
	else{
		//client.flush();
		//client.stop();*/	
		if(server.hasClient()){
			//Serial.println("Encontrado cliente");
			client=server.available();
			//if(client.connected() && client.available()){
				msg=client.readString();
				if(msg.length()>0){
					return true;
				//}
			}
		}		
	}

	return false;
}

void broadcastNode::clearServer(){
	while(server.hasClient()){
		Serial.println("Encontrado cliente para cerrar");
		client=server.available();
		client.flush();
		client.stop();		
	}
}

void broadcastNode::closingServer(){
		clearServer();
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

void broadcastNode::tryConnect(String &_ssid){
	WiFi.begin(_ssid.c_str());	
}

bool broadcastNode::Connected(){
	return WiFi.isConnected();
}

void broadcastNode::InitClient(){
	IPAddress ip(192, 168, 4, 1);
	client.connect(ip, SERVER_PORT);
}

void broadcastNode::stopClient(){
	client.flush();
	client.stop();
}

bool broadcastNode::connectedToServer(){
	return client.connected();	
}

void broadcastNode::sendToServer(String &_msg){
	client.println(_msg);
}
