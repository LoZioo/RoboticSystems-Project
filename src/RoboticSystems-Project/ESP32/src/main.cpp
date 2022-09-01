#include <Arduino.h>
#include <WiFi.h>

#include <ESPAsyncWebServer.h>	//WebServer.
#include <AsyncElegantOTA.h>		//OTA.

#include <ArduinoJSON.h>				//JSON.
#include <LittleFS.h>						//Filesystem.

#include <FixedGraph.h>					//Fixed graph.

#include <packet.h>
#include <password.h>
#include <const.h>

//Thread.
void webserver_thread(void*), evaluate_thread(void*);
TaskHandle_t webserver_thread_handle, evaluate_thread_handle;

//WebServer.
AsyncWebServer server(80);

//Fixed graph.
FixedGraph graph;

void setup(){
	Serial.begin(9600);

	//ABILITARE!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	Serial.setDebugOutput(false);

	// for(int i=0; i<40; i++)
	// 	Serial.println();
	// Serial.println("Ready");
	
	LittleFS.begin();

	WiFi.mode(WIFI_STA);
	WiFi.begin(STASSID, STAPSK);

	while(WiFi.status() != WL_CONNECTED)
		delay(500);
	
	xTaskCreatePinnedToCore(webserver_thread,	"webserver",	10240,	NULL,	1,	&webserver_thread_handle,	PRO_CPU);
	// xTaskCreatePinnedToCore(evaluate_thread,	"evaluate",		10240,	NULL,	1,	&evaluate_thread_handle,	APP_CPU);

	//Delete setup task.
	vTaskDelete(NULL);
}

void loop(){}

void webserver_thread(void *parameters){
	server.on("/", HTTP_GET, [](AsyncWebServerRequest *req){
		String res;

		res += F("Hello! This is RoboticSystem-project ESP32-CAM!");
		res += F("<br>");
		res += F("<a href='/update'> Update </a>");
		res += F("<br>");
		res += F("<a href='/reset'> Reset </a>");
		res += F("<br><br>");
		res += F("<a href='/test'> Test </a>");
		
		req->send(200, "text/html", res);
	});

	server.on("/reset", HTTP_GET, [](AsyncWebServerRequest *req){
		req->redirect("/");
		ESP.restart();
	});

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

	server.serveStatic("/", LittleFS, "/");

	AsyncElegantOTA.begin(&server);
	server.begin();

	for(;;)
		vTaskDelay(1 / portTICK_PERIOD_MS);		//One ms delay.
}

void evaluate_thread(void *parameters){
	for(;;)
		vTaskDelay(1 / portTICK_PERIOD_MS);
}
