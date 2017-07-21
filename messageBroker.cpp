#include "messageBroker.h"

messageBroker::messageBroker(){

}

void messageBroker::addMessageToReviewQueue(String &msg){
	messages_without_review.add(msg);
} 
