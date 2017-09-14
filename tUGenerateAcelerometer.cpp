#include "tUGenerateAcelerometer.h"

tUGenerateAcelerometer::tUGenerateAcelerometer(messageBroker * _messages,broadcastNode * _node){
	duration_ms=0;
	own_sequence=0;
	messages=_messages;
	node=_node;
}

void tUGenerateAcelerometer::execute(){
	 
	//MPU-6050 da los valores en enteros de 16 bits
	//Valores sin refinar
	int16_t AcX, AcY;

	
	int sda_pin=4;
	int scl_pin=5;
	
	Wire.begin(sda_pin, scl_pin);

	Wire.beginTransmission(MPU);
	Wire.write(0x6B);
	Wire.write(0);
	Wire.endTransmission(true);

   Wire.beginTransmission(MPU);
   Wire.write(0x3B); //Pedir el registro 0x3B - corresponde al AcX
   Wire.endTransmission(false);
   Wire.requestFrom(MPU,4,true); //A partir del 0x3B, se piden 4 registros
   AcX=Wire.read()<<8|Wire.read(); //Cada valor ocupa 2 registros
   AcY=Wire.read()<<8|Wire.read();
 



	DynamicJsonBuffer jsonBuffer;

	JsonObject& root = jsonBuffer.createObject();
	JsonObject& data = root.createNestedObject("data");
	JsonArray& ids = root.createNestedArray("ids");	   
	
	data["AcX"]=AcX;
	data["AcY"]=AcY;
	data["time"]=millis();

	root["channel"]=String("_ACELEROMETER");
	root["sequence"]=own_sequence;
	own_sequence++;
	root["n_ids"]=1;
	String ssid;
	node->getSSID(ssid);
	ids.add(ssid);
	
	String msg;
	root.printTo(msg);
	messages->addMessageToSendQueue(msg);				
}