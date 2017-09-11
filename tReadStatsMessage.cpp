#include "tReadStatsMessage.h"


tReadStatsMessage::tReadStatsMessage(messageBroker * _messages){
	duration_ms=0;
	own_sequence=0;
	messages=_messages;	
}

bool tReadStatsMessage::isMessageStats(String &_msg){
	DynamicJsonBuffer jsonBuffer;
	JsonObject& root = jsonBuffer.parseObject(_msg);

	const char * channel = root["channel"];
	String Channel=String(channel);
	Serial.println(String("canal del mensaje analizado"));
	Serial.println(_msg);
	Serial.println(Channel);
	return Channel.equals(String("_STATS"));	
}

void tReadStatsMessage::execute(){
	String msg;
	for(int i=0;i<messages->sizeOfMessagesWithoutRead();i++){
		messages->getMessageWithoutRead(i,msg);
		if(isMessageStats(msg)){
			messages->removeMessageOfReadQueue(i);
			i--;			
			Serial.println(msg);
			messages->addMessageToSendQueue(msg);						
		}
		yield();
	}	
}

