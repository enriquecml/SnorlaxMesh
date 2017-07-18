
#include "broadcastNode.h"
volatile int in;
void ICACHE_RAM_ATTR in_to_cero(){
	in=0;
}
broadcastNode::broadcastNode():_server(SERVER_PORT)
{
  _chip_id = ESP.getChipId();
  _ssid = String( String( SSID_PREFIX ) + String( _chip_id ) );
  
  pub=false;
}

   void broadcastNode::scan(unsigned long duration_ms,messageBroker * messages,unsigned long max_range,unsigned long min_range){
	in = 1;
	t.once_ms(duration_ms,in_to_cero);
	unsigned long pointTime1;
	unsigned long pointTime2;	
	String saux;
	String filter=String(SSID_PREFIX);
  WiFi.forceSleepWake();
  delay(1);	
	WiFi.mode(WIFI_STA);
	WiFi.disconnect();
	while(in){
	pointTime1=millis();	
    int n = WiFi.scanNetworks();
	pointTime2=millis();
    for (int i = 0; i < n; ++i) {
      if(WiFi.SSID(i).startsWith(filter)){
		saux=WiFi.SSID(i);  
		messages->updateAP(saux,substract(pointTime2,(pointTime2-pointTime1)/2),true,max_range,min_range);
      }
	  
    }
	}
	WiFi.disconnect(true);
					WiFi.forceSleepBegin();                  // turn off ESP8266 RF
				delay(1);  	
		t.detach();	
  }

  void broadcastNode::addPeriodToSSID(unsigned long period_s){
	_ssid=String(_ssid+String("S")+String(period_s));  
  }
  
void broadcastNode::trySendMessages(unsigned long duration,messageBroker * messages,String & ssid,unsigned long next_time_receive){
	 SingletonStats::instance()->n_try_connections++;
  		
	in = 1;
	t.once_ms(duration,in_to_cero);
	unsigned long pointTime;
	int state=0;
	bool sent=false;
	bool connected=false;
	bool sent_rate=false;
	IPAddress ip(192, 168, 4, 1);
	//String sip=String(SERVER_IP_ADDR);
	while(in && !sent){
		
		switch(state){
			case 0:
				_client.setNoDelay(true);
				state++;
			break;
			case 1:
				  WiFi.forceSleepWake();
				  delay(1);
				WiFi.begin( ssid.c_str() );
				WiFi.setAutoReconnect(true);
				DEBUG_MSG("Esperando resultado de la espera de la conexion");				
			    if(WiFi.waitForConnectResult() == WL_CONNECTED){
					//pointTime=millis();
					state++;
					DEBUG_MSG("Conectado al ssid %s",ssid.c_str());
				}
				DEBUG_MSG("Resultado de la conexion");				
				
			break;
			case 2:
				//if(WiFi.status() == WL_CONNECTED){
					if(_client.connect(ip, SERVER_PORT)){
						state++;
						connected=true;						
					}					
				//}
				else{
					state--;
				}

			break;
			case 3:
				if(_client.connected()){
					String m=String("");
					if(sent_rate){
						messages->nextMessage(ssid,m);
						if(m.length()>0){
							Serial.println(m);						
							_client.println(m);
						}
						else{
							sent=true;
						}
					}else{
						SingletonStats::instance()->n_messages_sent++;
						createRate(m,next_time_receive);
						if(m.length()>0){
							Serial.println(m);						
							_client.println(m);
						}
						sent_rate=true;					
					}	
				}
				else{
					state--;
				}
			break;
		}
	}
	_client.flush();
	_client.stop();
	if(state>1){
		WiFi.setAutoReconnect(false);
	WiFi.disconnect(true);}
				WiFi.forceSleepBegin();                  // turn off ESP8266 RF
				delay(1);  	
	if(connected){
		//messages->updateAP(ssid,pointTime,false,0,0);
		//connected true
		messages->putConnected(ssid);
	}
	else{
		SingletonStats::instance()->n_failed_connections++;
		messages->incrementTry(ssid);
	}
		t.detach();	
  }  

void broadcastNode::createRate(String &sJson,unsigned long next_time_receive){
	DynamicJsonBuffer jsonBuffer;

	JsonObject& root = jsonBuffer.createObject();
	root["rate"]=SingletonStats::instance()->n_messages_received/SingletonStats::instance()->n_messages_sent;
	root["channel"]=String("_RATE");
	root["id"]=_ssid;
	root["next_time_receive"]=substract(next_time_receive,millis());	
	root.printTo(sJson);
	sJson.trim();
}  

void broadcastNode::modeCatchDataMessages(unsigned long duration,messageBroker *messages){
	in = 1;
	t.once_ms(duration,in_to_cero);
	unsigned long stamp;
	int state=0;
	//bool yetClient=false;
	while(in){
		yield();
		switch(state){
			case 0:
			  WiFi.forceSleepWake();
  delay(1);
				WiFi.softAP(_ssid.c_str());	
				_server.setNoDelay(true);				
				_server.begin();
				state++;
			break;
			case 1:
					if(_server.hasClient()){
						_client=_server.available();
						state++;
					}
			break;
			case 2:
				if(_client.connected()){
					if (_client.available()){
						stamp=millis();
						String line = _client.readStringUntil('\n');
						Serial.println("Recibido");
						Serial.println(line);
						line.trim();
						if(line.length()>0)
							messages->entryMessage(line,stamp);//Dont forget Global Time(Singleton or global Variable) or across time for parameter

					}				
				}
				else{
				state--;
				}
			break;

		}
	}
	if(state>0){
	WiFi.softAPdisconnect(true);
					WiFi.forceSleepBegin();                  // turn off ESP8266 RF
				delay(1);  	}
		while(_server.hasClient()){
			DEBUG_MSG("Parando un cliente");			
			_client=_server.available();
					if (_client.available()){
						//stamp=millis();
						String line = _client.readStringUntil('\n');
						//Serial.println("Recibido");
						//Serial.println(line);						
						//messages->entryMessage(line);//Dont forget Global Time(Singleton or global Variable) or across time for parameter
						line.trim();
						//if(line.length()>0)
							//messages->entryMessage(line);//Dont forget Global Time(Singleton or global Variable) or across time for parameter
					}				
			_client.flush();
			_client.stop();					
		}
		//_server.stop();
		t.detach();
}
 