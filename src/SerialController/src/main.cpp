#include <Arduino.h>
#include <SerialController.h>

#define SS_RX	A0
#define SS_TX	A1

SerialController_target controller(SS_RX, SS_TX);
SerialController_data_t data;

void setup(){
	controller.begin(9600);
}

void loop(){
	controller.recv(data);
	delay(100);
}
