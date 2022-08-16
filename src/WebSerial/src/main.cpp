#include <Arduino.h>

#include <WiFi.h>
#include <AsyncTCP.h>

#include <ESPAsyncWebServer.h>
#include <AsyncElegantOTA.h>
#include <WebSerial.h>

#include <ArduinoJSON.h>

#include <packet.h>
#include <password.h>

void serialCallback(uint8_t *data, size_t len);

AsyncWebServer server(80);
DynamicJsonDocument doc(1024);

packet_t<> packet;

void setup(){
	Serial2.begin(9600);

	pinMode(LED_BUILTIN, OUTPUT);

  WiFi.mode(WIFI_STA);
  WiFi.begin(STA_SSID, STA_PSK);

  while(WiFi.status() != WL_CONNECTED){
		digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
		delay(500);
	}
	digitalWrite(LED_BUILTIN, HIGH);

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *req){
		String res;

		res += "Hi! This is a sample response!";

		res += "<br>";
		res += "<a href='/webserial'> WebSerial </a>";

		res += "<br>";
		res += "<a href='/update'> Update </a>";

    req->send(200, "text/html", res);
  });

	WebSerial.msgCallback(serialCallback);

  AsyncElegantOTA.begin(&server);
	WebSerial.begin(&server);
  server.begin();
}

void loop(){}

void serialCallback(uint8_t *data, size_t len){
	deserializeJson(doc, data);

	//Identify command.
	if(doc["com"] == "COMMAND_RESET"){
		packet.com = COMMAND_RESET;
		packet.argc = 0;
	}		

	else if(doc["com"] == "COMMAND_RESET_ROUTINE"){
		packet.com = COMMAND_RESET_ROUTINE;
		packet.argc = 0;
	}

	else if(doc["com"] == "COMMAND_POSE"){
		packet.com = COMMAND_POSE;
		packet.argc = 0;
	}

	else if(doc["com"] == "COMMAND_GOTO"){
		packet.com = COMMAND_GOTO;
		packet.argc = 3;
	}

	else if(doc["com"] == "COMMAND_KPID_GET"){
		packet.com = COMMAND_KPID_GET;
		packet.argc = 0;
	}

	else if(doc["com"] == "COMMAND_KPID_SET"){
		packet.com = COMMAND_KPID_SET;
		packet.argc = 4;
	}

	else{
		WebSerial.println("CONTROL_INVALID_MSG");
		return;
	}

	//Set parameters.
	for(int i=0; i<packet.argc; i++)
		packet.argv[i] = doc["data"][i];

	//Flush incoming bytes.
	while(Serial2.available())
		Serial2.read();
	
	//Send request.
	Serial2.write((uint8_t*) &packet, sizeof(packet));

	//Wait response.
	while(!Serial2.available())
		asm("nop");
	
	//Read response.
	Serial2.readBytes((uint8_t*) &packet, sizeof(packet));

	//Encode response.
	doc.clear();
	doc["com"] = packet.com;

	//Set parameters.
	for(int i=0; i<packet.argc; i++)
		doc["data"][i] = packet.argv[i];

	String res;
	serializeJson(doc, res);

	//Send JSON.
	WebSerial.println(res);
}
