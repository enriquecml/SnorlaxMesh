#include "tRemoveMessages.h"

tRemoveMessages::tRemoveMessages(APs *_listAPs,messageBroker *_messages){
	duration_ms=0;
	listAPs=_listAPs;
	messages=_messages;
}

void tRemoveMessages::execute(){
	int n=listAPs->numberAPs();
	bool cantRemove=false;
	if(messages->sizeOfMessagesReadyToSend()>0){
		for(int i=0;i<n && cantRemove==false;i++){
			if(listAPs->giveAP(i)->positionMessage==0)
				cantRemove=true;
		}
		if(cantRemove==false){
			SingletonStats::instance()->n_messages_removed++;
			messages->removeMessageOfReadyToSendQueue(0);
			for(int i=0;i<n;i++){
				listAPs->giveAP(i)->positionMessage--;
			}
		}

	}

}