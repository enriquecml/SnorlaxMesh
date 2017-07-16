#include "SubBase.h"

  SubBase::SubBase(){
	channel = String("INFO_STATS");  
  }

  String urlencode(String &str)
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
  
  bool SubBase::read(String &sJson){

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
    return 0;
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