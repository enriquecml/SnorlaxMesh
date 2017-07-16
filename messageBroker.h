#ifndef __MESSAGEBROKER_H__
#define __MESSAGEBROKER_H__

#include "AP.h"
#include <Arduino.h>
#include <LinkedList.h>
#include <ArduinoJson.h>
#include "SingletonStats.h"
#include "PubBase.h"
void ICACHE_RAM_ATTR int_to_cero(volatile int *flag);
void ICACHE_RAM_ATTR int_to_cero( int *flag);

  unsigned long globalTime(unsigned long time);
 unsigned long substract(unsigned long a,unsigned long b);
unsigned long min_time( unsigned long a,unsigned long b);

#define DEBUG 1
#define DEBUG_ESP_PORT Serial
#ifdef DEBUG_ESP_PORT
#define DEBUG_MSG(...) DEBUG_ESP_PORT.printf( __VA_ARGS__ )
#else
#define DEBUG_MSG(...)
#endif
class messageBroker{

private:

  LinkedList<AP *> APs;
  
  LinkedList<String> _messages_without_review;
  
   

  int total_access;
 
  bool lostConnectionWithAP;
 
public:
  LinkedList<String> _messages_without_read;

LinkedList<String> _messages_ready_to_send; 
  messageBroker();
  //int giveMeNumberNodesKnow();
  bool nextMessage(String &node,String &message);

  bool removeMessage(LinkedList<PubBase*> &publicators);
  
  void extractChannelAndSequence(String &sJson,String &Channel,unsigned long &sequence,String &Origin);

bool wasHere(String &sJson,String &ssid);  
  
  bool existMessage(String &sJson);
  bool reviewMessages(String &ssid);
  void addNodeToMessage(String &sJson,String &ssid);  
 void updateAP(String &name_node,unsigned long time_saw,bool scan,unsigned long max_range,unsigned long min_range); 
  void entryMessage(String &sJson);

  bool connected_all();

  void resetConnected();

void putConnected(String &ssid);

void incrementTry(String &ssid);
  
  int givePositionNode(String &name_node);

void nextTimeSend(String &ssid,unsigned long &time_next_send,unsigned long &duration_send,unsigned long period_ms,unsigned long min_time_receive);

bool isLostConnectionWithAP();
int numberOfAPs();

bool updateRate(String &sJson);
  
};

#endif
