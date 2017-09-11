#ifndef __BROADCASTNODE_H__
#define __BROADCASTNODE_H__

#include <ESP8266WiFi.h>
#include <WiFiServer.h>
#include <WiFiClient.h>
#include <Arduino.h>
#include <LinkedList.h>

#define SSID_PREFIX      		"BN"
#define SERVER_IP_ADDR			"192.168.4.1"
#define SERVER_IP_NET           "192.168.4."
#define SERVER_PORT				 4005

class broadcastNode{

private:

	String ssid;
	uint32_t chip_id;

	WiFiClient client;
	LinkedList<WiFiClient *> clients_connected;
	int index_client_connected;
	WiFiServer server;

public:
  
  broadcastNode();
  
  void upWiFi();
  void downWiFi();
  
  void modeAp();
  
  void initServer();
  bool readMessage(String &msg);
bool addNewClientConnection();  
void clearServer();
  void closingServer();
  
  int scan(LinkedList<String> &APs_filtered);
  void getSSID(String &_ssid);
  void setSSID(String &_ssid);
  
  
  void tryConnect(String &_ssid);
  bool Connected();
  void InitClient();
  void stopClient();
  bool connectedToServer();
  void sendToServer(String &_msg);
};

#endif
