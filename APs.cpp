#include "APs.h"

APs::APs(){

}

LinkedList<AP *> * APs::getListAPs(){
	return &_APs;
}
