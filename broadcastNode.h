#ifndef __BROADCASTNODE_H__
#define __BROADCASTNODE_H__

#include <ESP8266WiFi.h>
#include <WiFiServer.h>
#include <WiFiClient.h>


#define SSID_PREFIX      		"BN"
#define SERVER_IP_ADDR			"192.168.4.1"
#define SERVER_IP_NET           "192.168.4."
#define SERVER_PORT				 4005

class broadcastNode{

private:

	String ssid;
	uint32_t chip_id;

	WiFiClient client;
	WiFiServer server;

public:
  
  broadcastNode();
  
  void upWiFi();
  void downWiFi();
  
  void modeAp();
  
  void initServer();
  bool readMessage(String &msg);
  bool closingServer(String &msg);
  
};

#endif
