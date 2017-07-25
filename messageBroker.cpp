#include "messageBroker.h"

messageBroker::messageBroker(){

}

void messageBroker::addMessageToReviewQueue(String &msg){
	messages_without_review.add(msg);
}

void messageBroker::removeMessageOfReviewQueue(int position){
	messages_without_review.remove(position);
} 

void messageBroker::removeMessageOfReadyToSendQueue(int position){
	messages_ready_to_send.remove(position);	
}

void messageBroker::addMessageToReadQueue(String &msg){
	messages_without_read.add(msg);
} 

void addMessageToSendQueue(String &msg){
	messages_ready_to_send.add(msg);
}


int messageBroker::sizeOfMessagesWithoutReview(){
	messages_without_review.size();	
}

int messageBroker::sizeOfMessagesReadyToSend(){
	return messages_ready_to_send.size();
}


void messageBroker::getMessageWithoutReview(int position,String &_msg){
	_msg=messages_without_review.get(position);
}

void messageBroker::getMessageReadyToSend(int position,String &_msg){
	_msg=messages_ready_to_send.get(position);
}

bool messageBroker::messageInList(LinkedList<String> *listMessages,String &_msg){
	int n=listMessages->size();
	for(int i=0;i<n;i++){
		if(_msg.equals(listMessages->get(i)))
			return true;
	}
	return false;
}

bool messageBroker::existMessage(String &_msg){
	return messageInList(&messages_without_review,_msg) || messageInList(&messages_without_read,_msg) || messageInList(&messages_ready_to_send,_msg);
}