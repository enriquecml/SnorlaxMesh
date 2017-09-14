#include "tUReadAcelerometer.h"

tUReadAcelerometer::tUReadAcelerometer(messageBroker * _messages,broadcastNode * _node){
	duration_ms=0;
	own_sequence=0;
	messages=_messages;
	node=_node;
}

String urlencode(String str)
{
    String encodedString="";
    char c;
    char code0;
    char code1;
    char code2;
    for (int i =0; i < str.length(); i++){
      c=str.charAt(i);
      if (c == ' '){
        encodedString+= '+';
      } else if (isalnum(c)){
        encodedString+=c;
      } else{
        code1=(c & 0xf)+'0';
        if ((c & 0xf) >9){
            code1=(c & 0xf) - 10 + 'A';
        }
        c=(c>>4)&0xf;
        code0=c+'0';
        if (c > 9){
            code0=c - 10 + 'A';
        }
        code2='\0';
        encodedString+='%';
        encodedString+=code0;
        encodedString+=code1;
        //encodedString+=code2;
      }
      yield();
    }
    return encodedString;
    
}
  
  bool tUReadAcelerometer::sendToCloud(String &sJson){


  WiFi.begin(String("MOVISTAR_0234").c_str(), String("85C4117DAA30EFFEF5D4").c_str());
  
  // Use the WiFi.status() function to check if the ESP8266
  // is connected to a WiFi network.
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(100);
  }

  WiFiClient client;
  int httpPort = 80;
  if (!client.connect(String("dweet.io").c_str(), httpPort)) 
  {
    // If we fail to connect, return 0.
    Serial.println("No conectado con el servidor");
    return false;
  }
  // If we successfully connected, print our Phant post:
client.print(String("GET /dweet/for/myesp8266PFC")+String(ESP.getChipId())+String("?message=") + urlencode(sJson) + " HTTP/1.1\r\n" +
  "Host: " + String("dweet.io") + "\r\n" + 
  "Connection: close\r\n\r\n");

	client.flush();
	client.stop();
	WiFi.disconnect(true);  
  
	//Serial.println("Leido");
	//Serial.println(sJson);
	
	return true;
  }

bool tUReadAcelerometer::isMessageAcelerometer(String &_msg){
	DynamicJsonBuffer jsonBuffer;
	JsonObject& root = jsonBuffer.parseObject(_msg);

	const char * channel = root["channel"];
	String Channel=String(channel);
	
	if(Channel.equals(String("_ACELEROMETER"))){		
		return true;
	}
	
	return false;		
}

void tUReadAcelerometer::execute(){
	
	String msg;
	bool found=false;
	for(int i=0;i<messages->sizeOfMessagesWithoutRead();i++){
		messages->getMessageWithoutRead(i,msg);
		if(isMessageAcelerometer(msg)){
			messages->removeMessageOfReadQueue(i);
			i--;
			messages->addMessageToSendQueue(msg);
		}
		yield();
	}

}
/*
void tUReadAcelerometer::execute(){
	
	String msg;
	bool found=false;
	for(int i=0;i<messages->sizeOfMessagesWithoutRead() && !found;i++){
		messages->getMessageWithoutRead(i,msg);
		if(isMessageAcelerometer(msg)){
			node->upWiFi();
			sendToCloud(msg);
			node->downWiFi();
			messages->removeMessageOfReadQueue(i);
			i--;
			found=true;
		}
	}

}*/
