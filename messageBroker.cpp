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

void messageBroker::removeMessageOfReadQueue(int position){
	messages_without_read.remove(position);
}

void messageBroker::addMessageToSendQueue(String &msg){
	messages_ready_to_send.add(msg);
}
void messageBroker::addMessageToSendQueue(int position,String &msg){
	messages_ready_to_send.set(position,msg);
}

int messageBroker::sizeOfMessagesWithoutReview(){
	return messages_without_review.size();	
}

int messageBroker::sizeOfMessagesWithoutRead(){
	return messages_without_read.size();	
}

int messageBroker::sizeOfMessagesReadyToSend(){
	return messages_ready_to_send.size();
}


void messageBroker::getMessageWithoutReview(int position,String &_msg){
	_msg=messages_without_review.get(position);
}

void messageBroker::getMessageWithoutRead(int position,String &_msg){
	_msg=messages_without_read.get(position);
}

void messageBroker::getMessageReadyToSend(int position,String &_msg){
	_msg=messages_ready_to_send.get(position);
}

bool messageBroker::messageInList(LinkedList<String> *listMessages,String &_msg){
	String msg_cmp;
	int n=listMessages->size();
	Serial.println(n);
	for(int i=0;i<n;i++){
		msg_cmp=listMessages->get(i);
		if(_msg.equals(msg_cmp))
			return true;
	}
	return false;
}

bool messageBroker::existMessage(String &_msg){
	if( messageInList(&messages_without_read,_msg) || messageInList(&messages_ready_to_send,_msg)){
		SingletonStats::instance()->n_messages_repeatly++;		
		return true;
	}
	return false;
}