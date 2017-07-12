
#include "broadcastNode.h"

broadcastNode::broadcastNode()
{
  _chip_id = ESP.getChipId();
  _ssid = String( String( SSID_PREFIX ) + String( _chip_id ) );
  
  pub=false;
}

   void broadcastNode::scan(unsigned long duration_ms,messageBroker * messages,unsigned long max_range,unsigned long min_range){
  	Ticker t;
	int in = 1;
	t.once_ms(duration_ms,int_to_cero,&in);
	unsigned long pointTime1;
	unsigned long pointTime2;	
	String filter=String(SSID_PREFIX);   
	WiFi.mode(WIFI_STA);
	WiFi.disconnect();
	while(in){
	pointTime1=millis();	
    int n = WiFi.scanNetworks();
	pointTime2=millis();
    for (int i = 0; i < n; ++i) {
      if(WiFi.SSID(i).startsWith(filter)){
		messages->updateAP(WiFi.SSID(i),substract(pointTime2,(pointTime2-pointTime1)/2),max_range,min_range);
      }
    }
	}
  }
/*
void broadcastNode::searchNewNode(unsigned long duration,messageBroker * messages,unsigned long next_time_receive,unsigned long duration_time_receive,unsigned long period_receive){

  	Ticker t;
	int in = 1;
	t.once_ms(duration,int_to_cero,&in);

	int state=0;
	bool sent_schedule=false;
	LinkedList<String> apsFilter;
	while(in && !sent_schedule){
		switch(state){
			case 0:
			//Init
				_client.flush();
				_client.stop();
				_client.setNoDelay(true);
				state++;
			break;
			case 1:
				//Serial.println("Escaneando..");
				apsFilter.clear();
				scan(apsFilter);
				if(apsFilter.size()>0){
						if(messages->givePositionNode(apsFilter.get(0))==-1){
							state++;		
						}	
				}				
			break;
			case 2:
				WiFi.begin( apsFilter.get(0).c_str() );
				Serial.println("Intentando conectar con");
				Serial.println(apsFilter.get(0));
				
			    if(WiFi.waitForConnectResult() == WL_CONNECTED)
					state++;
				else
					state--;
			break;
			case 3:
				Serial.println("Conectado-");
				Serial.println("");
				Serial.println("WiFi connected");  
				Serial.println("IP address: ");
				//Serial.println(WiFi.localIP());
				if(WiFi.status() == WL_CONNECTED){
					if(_client.connect(SERVER_IP_ADDR, SERVER_PORT)){
						state++;
						
					}					
				}
				else{
					state--;
				}

			break;
			case 4:
				if(_client.connected()){
					String m;
					createSchedule(millis(),next_time_receive,duration_time_receive,period_receive,m);											
					_client.println(m);
					sent_schedule=true;					
				}
				else{
					state--;
				}
			break;
		}
	}
	WiFi.disconnect(true);
	messages->resetConnected();	
  }    
*/


  
void broadcastNode::trySendMessages(unsigned long duration,messageBroker * messages,String ssid){
	 SingletonStats::instance()->n_try_connections++;
  	Ticker t;	
	int in = 1;
	t.once_ms(duration,int_to_cero,&in);
	WiFiClient _client;
	//unsigned long pointTime;
	int state=0;
	bool sent=false;
	bool connected=false;
	bool sent_rate=false;
	while(in && !sent){
		switch(state){
			case 0:

				_client.setNoDelay(true);
				state++;
			break;
			case 1:
				WiFi.begin( ssid.c_str() );
			    if(WiFi.waitForConnectResult() == WL_CONNECTED){
					//pointTime=millis();
					state++;
					DEBUG_MSG("Conectado al ssid %s",ssid.c_str());
				}
				else{
					state--;
				}
			break;
			case 2:
				if(WiFi.status() == WL_CONNECTED){
					if(_client.connect(String(SERVER_IP_ADDR).c_str(), SERVER_PORT)){
						state++;
						connected=true;
					}					
				}
				else{
					state--;
				}

			break;
			case 3:
				if(_client.connected()){
					String m;
					if(!sent_rate){
						SingletonStats::instance()->n_messages_sent++;
						createRate(m);
						if(m.length()>0){
							Serial.println(m);						
							_client.println(m);
						}
						sent_rate=true;
					}else{
						messages->nextMessage(ssid,m);
						if(m.length()>0){
							Serial.println(m);						
							_client.println(m);
						}
						else{
							sent=true;
						}						
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
}  


/*void broadcastNode::createSchedule(unsigned long pointTime,unsigned long next_time_receive,unsigned long duration_time_receive,unsigned long period_receive,String &sJSON){
	DynamicJsonBuffer jsonBuffer;

	JsonObject& root = jsonBuffer.createObject();
	root["next_time_receive"]=substract(next_time_receive,pointTime);
	root["duration_time_receive"]=duration_time_receive;
	root["period_receive"]=period_receive;
	root["channel"]=String("_SCHEDULE");
	root["id"]=_ssid;
	root.printTo(sJSON);	
}*/

void broadcastNode::modeCatchDataMessages(unsigned long duration,messageBroker *messages){
	Ticker t;
	int in = 1;
	t.once_ms(duration,int_to_cero,&in);
	WiFiClient _client;
	int state=0;
	//bool yetClient=false;
	WiFiServer _server(SERVER_PORT);
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
						messages->entryMessage(line);//Dont forget Global Time(Singleton or global Variable) or across time for parameter
						Serial.println("Recibido");
						Serial.println(line);
					}				
				}
				else{
				state--;
				}
			break;

		}
	}

		while(_server.hasClient()){
			DEBUG_MSG("Parando un cliente");
			_client=_server.available();
			_client.flush();
			_client.stop();					
		}
		//_server.stop();
	WiFi.softAPdisconnect(true);		
}
 