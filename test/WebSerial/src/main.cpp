#include <Arduino.h>

#if defined(ESP8266)
  #include <ESP8266WiFi.h>
  #include <ESPAsyncTCP.h>

#elif defined(ESP32)
  #include <WiFi.h>
  #include <AsyncTCP.h>

#endif

#include <ESPAsyncWebServer.h>
#include <AsyncElegantOTA.h>
#include <WebSerial.h>

#define STA_SSID	"The Lab"
#define STA_PSK		"3179j215"

void recvMsg(uint8_t *data, size_t len);

AsyncWebServer server(80);

void setup() {
    Serial.begin(9600);		//Debug.
		Serial2.begin(9600);	//Data.

    WiFi.mode(WIFI_STA);
    WiFi.begin(STA_SSID, STA_PSK);

    if(WiFi.waitForConnectResult() != WL_CONNECTED){
			Serial.printf("WiFi Failed!\n");
			ESP.restart();
    }

    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());

		AsyncElegantOTA.begin(&server);		// AsyncElegantOTA is accessible at "<IP Address>/update" in browser
    WebSerial.begin(&server);					// WebSerial is accessible at "<IP Address>/webserial" in browser

    /* Attach Message Callback */
    WebSerial.msgCallback(recvMsg);

		server.on("/", HTTP_GET, [](AsyncWebServerRequest *req){
			String tmp;

			tmp += "<a href='/webserial'> Serial monitor </a>";
			tmp += "<br>";
			tmp += "<a href='/update'> Update </a>";
			
			req->send(200, "text/html", tmp);
		});

    server.begin();
}

void loop(){
	if(Serial2.available()){
		// String s;

		// while(Serial2.available())
		// 	s += (char) Serial2.read();
		
		// WebSerial.print(s);

		WebSerial.println(Serial2.readString());
	}
}

/* Message callback of WebSerial */
void recvMsg(uint8_t *data, size_t len){
	Serial2.write(data, len);
	Serial2.println();
}
