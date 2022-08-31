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
	// Serial.setDebugOutput(false);

	for(int i=0; i<40; i++)
		Serial.println();
	Serial.println("Ready");
	
	LittleFS.begin();

	WiFi.mode(WIFI_STA);
	WiFi.begin(STASSID, STAPSK);

	while(WiFi.status() != WL_CONNECTED)
		delay(500);
	
	xTaskCreatePinnedToCore(webserver_thread,	"webserver",	10240,	NULL,	1,	&webserver_thread_handle,	PRO_CPU);
	xTaskCreatePinnedToCore(evaluate_thread,	"evaluate",		10240,	NULL,	1,	&evaluate_thread_handle,	APP_CPU);

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
		
		req->send(200, "text/html", res);
	});

	server.serveStatic("/", LittleFS, "/");

	AsyncElegantOTA.begin(&server);
	server.begin();

	for(;;)
		vTaskDelay(1 / portTICK_PERIOD_MS);		//One ms delay.
}

void evaluate_thread(void *parameters){
	Serial.printf("dijkstra: %s\r\n", graph.dijkstra(0, 0) ? "true" : "false");

	std::pair<float, float> shortest_path[FIXED_GRAPH_SIZE];
	int len;

	graph.clearVertex(2.5, 2.5);
	graph.getShortestPath(4.5, 4.5, shortest_path, len);

	Serial.printf("len: %d\r\n", len);

	//Exclude the source.
	for(int i=len-2; i>=0; i--)
		Serial.printf("(%0.1f, %0.1f)\r\n", shortest_path[i].first, shortest_path[i].second);

	Serial.println("Done");

	for(;;)
		vTaskDelay(1 / portTICK_PERIOD_MS);
}
