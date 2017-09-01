#include "tReadRateMessage.h"

tReadRateMessage::tReadRateMessage(messageBroker * _messages,APs * _aps){
	duration_ms=0;
	own_sequence=0;
	messages=_messages;

	aps=_aps;
}

bool tReadRateMessage::isMessageRate(String &_msg){
	DynamicJsonBuffer jsonBuffer;
	JsonObject& root = jsonBuffer.parseObject(_msg);

	const char * channel = root["channel"];
	String Channel=String(channel);
	
	if(Channel.equals(String("_RATE"))){
		const char * origin = root["id"];
		String Origin=String(origin);
		AP * aux_ap = aps->giveAP(Origin);
		if(aux_ap==NULL){
			aux_ap=new AP();
			aux_ap->time_saw=millis();
			aux_ap->period_s=30;
			aps->addAP(aux_ap);
		}
		aux_ap->rate=root["rate"];
		
		return true;
	}
	
	return false;		
}

void tReadRateMessage::execute(){
	
	String msg;
	bool found=false;
	for(int i=0;i<messages->sizeOfMessagesWithoutRead() && !found;i++){
		messages->getMessageWithoutRead(i,msg);
		if(isMessageRate(msg)){
			messages->removeMessageOfReadQueue(i);
			i--;
			found=true;
		}
	}

}