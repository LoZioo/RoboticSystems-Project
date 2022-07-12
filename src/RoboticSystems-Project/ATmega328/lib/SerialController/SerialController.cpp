#include <SerialController.h>

//----------------------------------------- ISR ---------------------------------------//

void ss_RX_ISR(uint8_t ch){
}

//------------------------------- SerialController_Host -------------------------------//

//------------------------------ SerialController_Target ------------------------------//

SerialController_Target::SerialController_Target(NeoSWSerial &ss, LMD18200 &motor, RI32 &encoder, uint8_t left_switch, uint8_t right_switch, bool AT_mode, HardwareSerial *debug):
	ss(ss), motor(motor), encoder(encoder), left_switch(left_switch), right_switch(right_switch), AT_mode(AT_mode){
	this->debug = debug;
	
	pinMode(left_switch, INPUT);
	pinMode(right_switch, INPUT);

	ss.attachInterrupt(ss_RX_ISR);
}
