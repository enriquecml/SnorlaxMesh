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
	int16_t AcX, AcY, AcZ, GyX, GyY, GyZ;
 
	//Angulos
	float Acc[2];
	float Gy[2];
	float Angle[2];
	
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
   Wire.requestFrom(MPU,6,true); //A partir del 0x3B, se piden 6 registros
   AcX=Wire.read()<<8|Wire.read(); //Cada valor ocupa 2 registros
   AcY=Wire.read()<<8|Wire.read();
   AcZ=Wire.read()<<8|Wire.read();
 
    //A partir de los valores del acelerometro, se calculan los angulos Y, X
    //respectivamente, con la formula de la tangente.
   Acc[1] = atan(-1*(AcX/A_R)/sqrt(pow((AcY/A_R),2) + pow((AcZ/A_R),2)))*RAD_TO_DEG;
   Acc[0] = atan((AcY/A_R)/sqrt(pow((AcX/A_R),2) + pow((AcZ/A_R),2)))*RAD_TO_DEG;

    //Leer los valores del Giroscopio
   Wire.beginTransmission(MPU);
   Wire.write(0x43);
   Wire.endTransmission(false);
   Wire.requestFrom(MPU,4,true); //A diferencia del Acelerometro, solo se piden 4 registros
   GyX=Wire.read()<<8|Wire.read();
   GyY=Wire.read()<<8|Wire.read();
 
   //Calculo del angulo del Giroscopio
   Gy[0] = GyX/G_R;
   Gy[1] = GyY/G_R;
 
   //Aplicar el Filtro Complementario
   Angle[0] = 0.98 *(Angle[0]+Gy[0]*60) + 0.02*Acc[0];
   Angle[1] = 0.98 *(Angle[1]+Gy[1]*60) + 0.02*Acc[1];
 
   //Mostrar los valores por consola
   Serial.print("Angle X: "); Serial.print(Angle[0]); Serial.print("\n");
   Serial.print("Angle Y: "); Serial.print(Angle[1]); Serial.print("\n------------\n");

	DynamicJsonBuffer jsonBuffer;

	JsonObject& root = jsonBuffer.createObject();
	JsonObject& data = root.createNestedObject("data");
	JsonArray& ids = root.createNestedArray("ids");	   
	
	data["angleX"]=Angle[0];
	data["angleY"]=Angle[1];
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