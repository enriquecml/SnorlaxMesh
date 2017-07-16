
#include "broadcastNode.h"

broadcastNode::broadcastNode():_server(SERVER_PORT)
{
  _chip_id = ESP.getChipId();
  _ssid = String( String( SSID_PREFIX ) + String( _chip_id ) );
  
  pub=false;
}

   void broadcastNode::scan(unsigned long duration_ms,messageBroker * messages,unsigned long max_range,unsigned long min_range){
  	Ticker t;
	volatile int in = 1;
	t.once_ms(duration_ms,int_to_cero,&in);
	unsigned long pointTime1;
	unsigned long pointTime2;	
	String saux;
	String filter=String(SSID_PREFIX);   
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
	  yield();
    }
	}
	WiFi.disconnect(true);
	
  }

  void broadcastNode::addPeriodToSSID(unsigned long period_s){
	_ssid=String(_ssid+String("S")+String(period_s));  
  }
  
void broadcastNode::trySendMessages(unsigned long duration,messageBroker * messages,String & ssid){
	 SingletonStats::instance()->n_try_connections++;
  	Ticker t;	
	volatile int in = 1;
	t.once_ms(duration,int_to_cero,&in);
	//unsigned long pointTime;
	int state=0;
	bool sent=false;
	bool connected=false;
	bool sent_rate=false;
	IPAddress ip(192, 168, 4, 1);
	//String sip=String(SERVER_IP_ADDR);
	while(in && !sent){
		yield();
		switch(state){
			case 0:
				_client.setNoDelay(true);
				state++;
			break;
			case 1:
				WiFi.begin( ssid.c_str() );
				//WiFi.setAutoReconnect(false);
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
						createRate(m);
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
	WiFi.disconnect(true);
	if(connected){
		//messages->updateAP(ssid,substract(pointTime,1000),0,0);
		//connected true
		messages->putConnected(ssid);
	}
	else{
		SingletonStats::instance()->n_failed_connections++;
		messages->incrementTry(ssid);
	}

  }  

void broadcastNode::createRate(String &sJson){
	DynamicJsonBuffer jsonBuffer;

	JsonObject& root = jsonBuffer.createObject();
	root["rate"]=SingletonStats::instance()->n_messages_received/SingletonStats::instance()->n_messages_sent;
	root["channel"]=String("_RATE");
	root["id"]=_ssid;
	root.printTo(sJson);
	sJson.trim();
}  

void broadcastNode::modeCatchDataMessages(unsigned long duration,messageBroker *messages){
	Ticker t;
	volatile int in = 1;
	t.once_ms(duration,int_to_cero,&in);

	int state=0;
	//bool yetClient=false;
	while(in){
		yield();
		switch(state){
			case 0:
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
						String line = _client.readStringUntil('\n');
						Serial.println("Recibido");
						Serial.println(line);
						line.trim();
						if(line.length()>0)
							messages->entryMessage(line);//Dont forget Global Time(Singleton or global Variable) or across time for parameter

					}				
				}
				else{
				state--;
				}
			break;

		}
	}
	WiFi.softAPdisconnect(true);
		while(_server.hasClient()){
			DEBUG_MSG("Parando un cliente");			
			_client=_server.available();
					if (_client.available()){
						String line = _client.readStringUntil('\n');
						Serial.println("Recibido");
						Serial.println(line);						
						messages->entryMessage(line);//Dont forget Global Time(Singleton or global Variable) or across time for parameter
						line.trim();
						if(line.length()>0)
							messages->entryMessage(line);//Dont forget Global Time(Singleton or global Variable) or across time for parameter
					}				
			_client.flush();
			_client.stop();					
		}
		//_server.stop();
		
}
 