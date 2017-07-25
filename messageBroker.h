#ifndef __MESSAGEBROKER_H__
#define __MESSAGEBROKER_H__

#include <Arduino.h>
#include <LinkedList.h>

class messageBroker{

private:

  LinkedList<String> messages_without_review;
  LinkedList<String> messages_without_read;
  LinkedList<String> messages_ready_to_send;
 
 bool messageInList(LinkedList<String> *listMessages,String &_msg);
 
public:

  messageBroker();
  void addMessageToReviewQueue(String &msg);
  void removeMessageOfReviewQueue(int position);
  void removeMessageOfReadyToSendQueue(int position);

  void addMessageToReadQueue(String &msg);
  void addMessageToSendQueue(String &msg);
  int sizeOfMessagesWithoutReview();

  
  int sizeOfMessagesReadyToSend();
  void getMessageWithoutReview(int position,String &_msg);
  void getMessageReadyToSend(int position,String &_msg);

  bool existMessage(String &_msg);
  
};

#endif
