#include "tRemoveMessages.h"

tRemoveMessages::tRemoveMessages(APs *_listAPs,messageBroker *_messages){
	duration_ms=0;
	listAPs=_listAPs;
	messages=_messages;
}

void tRemoveMessages::execute(){
	int n=listAPs->numberAPs();
	Serial.print(String("Numero de aps:"));
	Serial.println(n);
	
	bool cantRemove=false;
	if(n>0){
		while(!cantRemove){
			if(messages->sizeOfMessagesReadyToSend()>1){
				for(int i=0;i<n && cantRemove==false;i++){
					Serial.print(String("SSID:"));
					Serial.print(listAPs->giveAP(i)->ssid);
					Serial.print(String("|position:"));
					Serial.println(listAPs->giveAP(i)->positionMessage);
					if(listAPs->giveAP(i)->positionMessage<2){
						cantRemove=true;
					}
				}
				if(cantRemove==false){
					SingletonStats::instance()->n_messages_removed++;
					messages->removeMessageOfReadyToSendQueue(1);
					for(int i=0;i<n;i++){
						listAPs->giveAP(i)->positionMessage--;
					}
				}

			}
			else{
				cantRemove=true;
			}
		yield();
		}
	}
}