#ifndef __MESSAGEBROKER_H__
#define __MESSAGEBROKER_H__

#include <Arduino.h>
#include <LinkedList.h>

class messageBroker{

private:

  LinkedList<String> messages_without_review;
  LinkedList<String> messages_without_read;
  LinkedList<String> messages_ready_to_send;
 
public:

  messageBroker();
  void addMessageToReviewQueue(String &msg);
};

#endif
