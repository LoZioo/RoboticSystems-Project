#include <Arduino.h>
#include <WiFi.h>

// DEFINISCI LE PASSWORD DEL WIFI IN include/password.h
// IL FILE è IGNORATO TRAMITE IL GITIGNORE NELLA ROOT DELLA REPO!

// CONTENUTO DA INSERIRE IN include/password.h

		// WiFi connection info.
		#define STASSID	"MyWiFiSSID"
		#define STAPSK	"MyWiFiPass"

// -------------------------------------------

#include <password.h>

void setup() {
  // put your setup code here, to run once:

	WiFi.begin(STASSID, STAPSK);
}

void loop() {
  // put your main code here, to run repeatedly:
}
