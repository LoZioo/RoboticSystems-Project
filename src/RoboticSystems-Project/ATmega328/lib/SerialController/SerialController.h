/**
 * @file SerialController.h
 * @author Davide Scalisi
 * @brief 
 * @version 0.1
 * @date 2022-07-05
 * 
 * @copyright Copyright (c) 2022
 * 
 * This library is used to send serial commands
 * to the low-level ATmega328 control board. 
 */

#ifndef SERIAL_CONTROLLER_H
#define SERIAL_CONTROLLER_H

#include <Arduino.h>
#include <NeoSWSerial.h>

#include <LMD18200.h>
#include <RI32.h>

#define MAX_PARAMS_COUNT 3

typedef enum{
} SerialController_command_t;

//Struct for passing binary data using the serial port.
struct SerialController_data_t{
	SerialController_command_t	command;
	uint8_t											params[MAX_PARAMS_COUNT];
};

//Send data to the target board.
class SerialController_Host{
};

//Process the data sended by the host.
class SerialController_Target{
	private:
		HardwareSerial	*debug;
		NeoSWSerial			&ss;

		LMD18200&	motor;
		RI32&			encoder;

		uint8_t	left_switch, right_switch;
		bool		AT_mode;

	public:
		SerialController_Target(
			NeoSWSerial&,							//Command serial.
			LMD18200&,								//Engine driver.
			RI32&,										//Encoder driver.
			uint8_t,									//Left switch pin.
			uint8_t,									//Right switch pin.
			bool = false,							//AT mode.
			HardwareSerial* = NULL		//Debug serial.
		);
};

#endif
