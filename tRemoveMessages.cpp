#include "tRemoveMessages.h"

tRemoveMessages::tRemoveMessages(){
	
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
			messages->removeMessageOfReadyToSendQueue(0);
			for(int i=0;i<n;i++){
				listAPs->giveAP(i)->positionMessage--;
			}
		}

	}

}