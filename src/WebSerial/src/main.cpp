#include <Arduino.h>

#include <WiFi.h>
#include <AsyncTCP.h>

#include <ESPAsyncWebServer.h>
#include <AsyncElegantOTA.h>
#include <WebSerial.h>

#include <ArduinoJSON.h>

#include <packet.h>
#include <password.h>

void webSerialCallback(uint8_t *data, size_t len);

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

	WebSerial.msgCallback(webSerialCallback);

  AsyncElegantOTA.begin(&server);
	WebSerial.begin(&server);
  server.begin();
}

void loop(){}

void webSerialCallback(uint8_t *data, size_t len){
	//Example:	{"com":"COMMAND_KPID_SET","data":[1,1,1,1]}
	//Shell:		./websocat ws://192.168.1.44/webserialws
	
	String res;
	deserializeJson(doc, data);

	//Identify command.
	if(doc["com"] == "COMMAND_RESET")
		packet.com = COMMAND_RESET;

	else if(doc["com"] == "COMMAND_RESET_ROUTINE")
		packet.com = COMMAND_RESET_ROUTINE;

	else if(doc["com"] == "COMMAND_POSE")
		packet.com = COMMAND_POSE;

	else if(doc["com"] == "COMMAND_GOTO")
		packet.com = COMMAND_GOTO;
	
	else if(doc["com"] == "COMMAND_START")
		packet.com = COMMAND_START;
	
	else if(doc["com"] == "COMMAND_STOP")
		packet.com = COMMAND_STOP;

	else if(doc["com"] == "COMMAND_KPID_GET")
		packet.com = COMMAND_KPID_GET;

	else if(doc["com"] == "COMMAND_KPID_SET")
		packet.com = COMMAND_KPID_SET;

	else if(doc["com"] == "COMMAND_TOL_GET")
		packet.com = COMMAND_TOL_GET;

	else if(doc["com"] == "COMMAND_TOL_SET")
		packet.com = COMMAND_TOL_SET;
	
	else if(doc["com"] == "COMMAND_MAX_SPEED_GET")
		packet.com = COMMAND_MAX_SPEED_GET;

	else if(doc["com"] == "COMMAND_MAX_SPEED_SET")
		packet.com = COMMAND_MAX_SPEED_SET;

	else if(doc["com"] == "COMMAND_SAVE")
		packet.com = COMMAND_SAVE;

	else if(doc["com"] == "COMMAND_LOAD")
		packet.com = COMMAND_LOAD;

	else{
		//Encode error response.
		doc.clear();
		doc["com"] = CONTROL_INVALID_COM;

		serializeJson(doc, res);

		//Send JSON.
		WebSerial.println(res);
		return;
	}

	//Set parameters number.
	packet.argc = doc["data"].size();

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
	
	serializeJson(doc, res);

	//Send JSON.
	WebSerial.println(res);
}
