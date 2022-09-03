/**
 * @file main.cpp
 * @author Davide Scalisi, Luigi Seminara
 * @brief Main WebServer software.
 * @version 1.0
 * @date 2022-08-28
 * 
 * @copyright Copyright (c) 2022
 */

#include <Arduino.h>
#include <WiFi.h>

#include <ESPAsyncWebServer.h>	//WebServer.
#include <AsyncElegantOTA.h>		//OTA.

#include <ArduinoJSON.h>				//JSON.
#include <LittleFS.h>						//Filesystem.

#include <FixedGraph.h>					//Fixed graph.

#include <packet.h>
#include <const.h>

//Functions.
inline void settings_save(), settings_load();

//Thread.
void webserver_thread(void*);
TaskHandle_t webserver_thread_handle;

//WebServer.
AsyncWebServer server(80);

//Old positions.
float old_x = 0, old_y = 0;

//Fixed graph.
FixedGraph graph;

//Device settings.
settings_t settings;

void setup(){
	Serial.begin(9600);
	Serial.setDebugOutput(false);

	LittleFS.begin();
	settings_load();

	WiFi.mode(WIFI_STA);
	WiFi.begin(settings.ssid, settings.pass);

	int i = 0;
	while(WiFi.status() != WL_CONNECTED && i < MAX_WIFI_ATTEMPTS){
		i++;
		delay(500);
	}
	
	//WiFi connection failed; entering in stand-alone WiFi mode (192.168.4.1).
	if(i == MAX_WIFI_ATTEMPTS){
		WiFi.disconnect();
		WiFi.softAP(SOFTAP_SSID);
	}

	//Main taks spawn.
	xTaskCreatePinnedToCore(webserver_thread,	"webserver",	10240,	NULL,	1,	&webserver_thread_handle,	APP_CPU);

	//Delete setup task.
	vTaskDelete(NULL);
}

void loop(){}

void webserver_thread(void *parameters){
	server.on("/", HTTP_GET, [](AsyncWebServerRequest *req){
		req->redirect("/index.html");
	});

	server.on("/settings/reset", HTTP_POST, [](AsyncWebServerRequest *req){
		req->send(200);
		ESP.restart();
	});

	server.on("/settings/wifi", HTTP_POST, [](AsyncWebServerRequest *req){
		strcpy(settings.ssid, req->arg("ssid").c_str());
		strcpy(settings.pass, req->arg("pass").c_str());

		settings_save();

		req->send(200);
		ESP.restart();
	});

	server.on("/settings/ssid", HTTP_GET, [](AsyncWebServerRequest *req){
		req->send(200, "text/plain", settings.ssid);
	});

	server.on("/commands/general", HTTP_POST, [](AsyncWebServerRequest *req){
		//Example:	{"com":10,"data":[1,1,1,1]}

		//Read JSON from request.
		DynamicJsonDocument doc(1024);
		deserializeJson(doc, req->arg("data"));

		//Build packet.
		packet_t<> packet;

		packet.com = doc["com"];
		packet.argc = 4;
		
		for(int i=0; i<packet.argc; i++)
			packet.argv[i] = doc["args"][i];
		
		//Flush incoming bytes.
		while(Serial.available())
			Serial.read();
		
		//Send request.
		Serial.write((uint8_t*) &PACKET_PAYLOAD, sizeof(PACKET_PAYLOAD));
		Serial.write((uint8_t*) &packet, sizeof(packet));

		//Read response (no payload check required).
		Serial.readBytes((uint8_t*) &packet, sizeof(packet));

		//Encode response.
		doc.clear();
		doc["com"] = packet.com;

		//Set parameters.
		for(int i=0; i<packet.argc; i++)
			doc["args"][i] = packet.argv[i];
		
		//Fill with zeros.
		for(int i=packet.argc; i<PACKET_ARGV_MAXLEN; i++)
			doc["args"][i] = 0;

		//Encode JSON for the response.
		String res;
		serializeJson(doc, res);

		//Send response.
		req->send(200, "application/json", res);
	});

	server.on("/commands/obstacle/add", HTTP_POST, [](AsyncWebServerRequest *req){
		const float x = req->arg("x").toFloat();
		const float y = req->arg("y").toFloat();

		graph.clearVertex(x, y);
		req->send(200);
	});

	server.on("/commands/obstacle/remove", HTTP_POST, [](AsyncWebServerRequest *req){
		const float x = req->arg("x").toFloat();
		const float y = req->arg("y").toFloat();

		graph.setVertex(x, y);
		req->send(200);
	});

	server.on("/commands/exec_route", HTTP_POST, [](AsyncWebServerRequest *req){
		const float x = req->arg("x").toFloat();	//Destination x.
		const float y = req->arg("y").toFloat();	//Destination y.

		//Update shortest path.
		graph.dijkstra(old_x, old_y);

		//Update source point for the next exec_route.
		old_x = x;
		old_y = y;

		//Get shortest path.
		std::pair<float, float> shortest_path[FIXED_GRAPH_SIZE];
		int len;

		graph.getShortestPath(x, y, shortest_path, len);
		len--;	//There are len - 1 elements; (0, 0) isn't an element!

		//Pack points array.
		int j = 0;
		xy_t xy[len];

		//Debug: also send the result points to the client.
		DynamicJsonDocument doc(1024);

		//len-1: exclude the source.
		for(int i=len-1; i>=0; i--){
			doc["points"][j]["x"] = xy[j].x = shortest_path[i].first;
			doc["points"][j]["y"] = xy[j].y = shortest_path[i].second;

			j++;
		}

		//Send data.
		packet_t<> packet;

		packet.com = COMMAND_WAIT_XY_ARRAY;
		packet.argc = 1;
		packet.argv[0] = len;

		Serial.write((uint8_t*) &PACKET_PAYLOAD, sizeof(PACKET_PAYLOAD));
		Serial.write((uint8_t*) &packet, sizeof(packet));
		Serial.write((uint8_t*) &xy, sizeof(xy));

		//Send response.
		String res;
		serializeJson(doc, res);

		req->send(200, "application/json", res);
	});

	server.serveStatic("/settings.bin", LittleFS, "/settings.bin");
	server.serveStatic("/", LittleFS, "/public/");

	AsyncElegantOTA.begin(&server);
	server.begin();

	for(;;)
		vTaskDelay(1 / portTICK_PERIOD_MS);		//One ms delay.
}

inline void settings_save(){
	File file = LittleFS.open("/settings.bin", "w");

	file.write((uint8_t*) &settings, sizeof(settings));
	file.close();
}

inline void settings_load(){
	File file = LittleFS.open("/settings.bin", "r");

	if(file){
		file.read((uint8_t*) &settings, sizeof(settings));
		file.close();
	}
	
	else{
		strcpy(settings.ssid, "STASSID");
		strcpy(settings.pass, "STAPSK");
	}
}
