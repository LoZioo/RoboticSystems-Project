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
void webserver_thread(void*), evaluate_thread(void*);
TaskHandle_t webserver_thread_handle, evaluate_thread_handle;

//WebServer.
AsyncWebServer server(80);

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
	xTaskCreatePinnedToCore(webserver_thread,	"webserver",	10240,	NULL,	1,	&webserver_thread_handle,	PRO_CPU);
	// xTaskCreatePinnedToCore(evaluate_thread,	"evaluate",		10240,	NULL,	1,	&evaluate_thread_handle,	APP_CPU);

	//Delete setup task.
	vTaskDelete(NULL);
}

void loop(){}

void webserver_thread(void *parameters){
	server.on("/test", HTTP_GET, [](AsyncWebServerRequest *req){
		graph.clearVertex(0.2, 0);
		graph.dijkstra(0, 0);

		std::pair<float, float> shortest_path[FIXED_GRAPH_SIZE];
		int len;
		
		graph.getShortestPath(1, 0, shortest_path, len);
		len--;	//There are len - 1 elements; (0, 0) isn't an element!

		int j = 0;
		xy_t xy[len];

		//Exclude the source.
		for(int i=len-1; i>=0; i--){
			xy[j].x = shortest_path[i].first;
			xy[j].y = shortest_path[i].second;

			j++;
		}

		//Sending data.
		packet_t<> packet;

		packet.com = COMMAND_WAIT_XY_ARRAY;
		packet.argc = 1;
		packet.argv[0] = len;

		Serial.write((uint8_t*) &PACKET_PAYLOAD, sizeof(PACKET_PAYLOAD));
		Serial.write((uint8_t*) &packet, sizeof(packet));
		Serial.write((uint8_t*) &xy, sizeof(xy));

		DynamicJsonDocument doc(1024);
		String json;
		
		for(int i=0; i<len; i++){
			doc["data"][i]["x"] = xy[i].x;
			doc["data"][i]["y"] = xy[i].y;
		}

		serializeJson(doc, json);
		req->send(200, "application/json", json);
	});
	///////////////////////////////////////////////////////////////////////

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
		DynamicJsonDocument doc(1024);
		
		deserializeJson(doc, req->arg("data"));

		String res;
		serializeJson(doc, res);

		req->send(200, "application/json", res);

		//CONTINUARE IMPLEMENTANDO GLI ENDPOINT RIMANENTI

		// deserializeJson(doc, data);

		// //Set parameters number.
		// packet.argc = doc["data"].size();

		// //Set parameters.
		// for(int i=0; i<packet.argc; i++)
		// 	packet.argv[i] = doc["data"][i];

		// //Flush incoming bytes.
		// while(Serial.available())
		// 	Serial.read();
		
		// //Send request.
		// Serial.write((uint8_t*) &packet, sizeof(packet));

		// //Wait response.
		// while(!Serial.available())
		// 	asm("nop");
		
		// //Read response.
		// Serial.readBytes((uint8_t*) &packet, sizeof(packet));

		// //Encode response.
		// doc.clear();
		// doc["com"] = packet.com;

		// //Set parameters.
		// for(int i=0; i<packet.argc; i++)
		// 	doc["data"][i] = packet.argv[i];
		
		// serializeJson(doc, res);

		// //Send JSON.
		// WebSerial.println(res);

		// req->send(200, "text/plain", settings.ssid);
	});

	server.serveStatic("/settings.bin", LittleFS, "/settings.bin");
	server.serveStatic("/", LittleFS, "/public/");

	AsyncElegantOTA.begin(&server);
	server.begin();

	for(;;)
		vTaskDelay(1 / portTICK_PERIOD_MS);		//One ms delay.
}

void evaluate_thread(void *parameters){
	for(;;)
		vTaskDelay(1 / portTICK_PERIOD_MS);
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
