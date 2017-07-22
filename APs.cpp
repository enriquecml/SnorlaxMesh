#include "APs.h"

APs::APs(){
	
}

void APs::addAP(AP * ap){
	list_APs.add(ap);
}

AP * APs::giveAP(String &sAP){
	if(list_APs.size()>0){
		int i=0;
		bool found=false;
		while(i<list_APs.size() && !found){
			if(list_APs.get(i)->ssid.equals(sAP)){
				return list_APs.get(i);
			}
			else{
				i++;
			}
		}
	}
	return NULL;	
}

AP * APs::giveAP(int position){
	return list_APs.get(position);
}

int APs::numberAPs(){
	return list_APs.size();
}
	