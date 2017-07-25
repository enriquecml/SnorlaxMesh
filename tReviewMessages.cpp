#include "tReviewMessages.h"

tReviewMessages::tReviewMessages(){
	
}

bool tReviewMessages::wasHere(String &_msg){
	DynamicJsonBuffer jsonBuffer;
	JsonObject& root = jsonBuffer.parseObject(_msg);
	JsonArray& ids = root["ids"];

	const char * ssid2;
	String ssid;
	node->getSSID(ssid);
	int n_ids=root["n_ids"];

	for(int i=0;i<n_ids;i++){
		ssid2=root["ids"][i];
		if(ssid.equals(ssid2)){
			return true;		
		}
	}

	return false;
}

void tReviewMessages::addNodeToMessage(String &_msg){
	DynamicJsonBuffer jsonBuffer;
	JsonObject& root = jsonBuffer.parseObject(_msg);
	JsonArray& ids = root["ids"];

	String ssid;
	node->getSSID(ssid);
  
	int n_ids=root["n_ids"];

	ids.add(ssid);
	n_ids++;
	root["n_ids"]=n_ids;  

	_msg=String("");
	root.printTo(_msg);
}


void tReviewMessages::execute(){
	String msg;
	while(messages->sizeOfMessagesWithoutReview()>0){
		messages->getMessageWithoutReview(0,msg);

		if(!wasHere(msg) && !messages->existMessage(msg)){
			addNodeToMessage(msg);
			messages->addMessageToReadQueue(msg);
		}
		
	messages->removeMessageOfReviewQueue(0);
	}	
}