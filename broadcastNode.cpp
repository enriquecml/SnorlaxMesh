
#include "broadcastNode.h"

broadcastNode::broadcastNode():server(SERVER_PORT)
{
	WiFi.setAutoConnect(false);
 	WiFi.persistent(false);
	downWiFi();	
	chip_id = ESP.getChipId();
	ssid = String( String( SSID_PREFIX ) + String( chip_id ) ); 
	index_client_connected=0;	
}

void broadcastNode::upWiFi(){
	WiFi.forceSleepWake();
	delay(1);
}

void broadcastNode::downWiFi(){
	WiFi.mode(WIFI_OFF); 
	WiFi.forceSleepBegin();
	delay(1);
}
  
void broadcastNode::modeAp(){
	WiFi.softAP(ssid.c_str());
}

void broadcastNode::initServer(){
	server.begin();
}

bool broadcastNode::addNewClientConnection(){
	WiFiClient * new_client;
	if(server.hasClient()){
		new_client=new WiFiClient();
		*new_client=server.available();
		clients_connected.add(new_client);
		return true;
	}
	return false;
}

bool broadcastNode::readMessage(String &msg){
	char endline='\n';
	if(clients_connected.size()>0){
	WiFiClient * aux_client=clients_connected.get(index_client_connected);
	index_client_connected++;
	if(index_client_connected==clients_connected.size())
		index_client_connected=0;
	if(aux_client->available()>0){
		Serial.println(String("Anterior Mensaje"));
		Serial.println(msg);
		Serial.println(String("Mensaje recibido"));
		msg=aux_client->readStringUntil(endline);
		Serial.println(msg);		
		if(msg.length()>2){				
			return true;
		}
	}	
	}
	return false;
}

void broadcastNode::clearServer(){
	WiFiClient * aux_client;
	for(int i=0;i<clients_connected.size();i++){
		aux_client=clients_connected.get(i);
		aux_client->flush();
		aux_client->stop();			
		delete(aux_client);
		clients_connected.remove(i);	
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
