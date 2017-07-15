#ifndef __BROADCASTNODE_H__
#define __BROADCASTNODE_H__

#include "messageBroker.h"

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <WiFiServer.h>
#include <Ticker.h>


#define SSID_PREFIX      		"BN"
#define SSID_PREFIX_SERVICE     "BNS"
#define SERVER_IP_ADDR			"192.168.4.1"
#define SERVER_IP_NET           "192.168.4."
#define SERVER_PORT				 4005
#define DEBUG             		1
#include "SingletonStats.h"

class broadcastNode{

private:

  uint32_t _chip_id;

  //void createSchedule(unsigned long pointTime,unsigned long next_time_receive,unsigned long duration_time_receive,unsigned long period_receive,String &sJSON);
	WiFiClient _client;
	WiFiServer _server;


public:
  unsigned long sent;
  unsigned long received;
  bool pub;
  String _ssid;

  broadcastNode();
  void scan(unsigned long duration_ms,messageBroker * messages,unsigned long max_range,unsigned long min_range);
 void createRate(String &sJson);
 
  void trySendMessages(unsigned long duration,messageBroker * messages,String &ssid); 
  /*void searchNewNode(unsigned long duration,messageBroker * messages,unsigned long next_time_receive,unsigned long duration_time_receive,unsigned long period_receive);

  */
  void modeCatchDataMessages(unsigned long duration,messageBroker *messages);  
  
  


};

#endif
