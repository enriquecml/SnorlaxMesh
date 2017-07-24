#include "messageBroker.h"

messageBroker::messageBroker(){

}

void messageBroker::addMessageToReviewQueue(String &msg){
	messages_without_review.add(msg);
} 

int messageBroker::sizeOfMessagesReadyToSend(){
	return messages_ready_to_send.size();
}

void messageBroker::getMessageReadyToSend(int position,String &_msg){
	_msg=messages_ready_to_send.get(position);
}