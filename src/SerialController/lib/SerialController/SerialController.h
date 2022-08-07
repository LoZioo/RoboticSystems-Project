/**
 * @file SerialController.h
 * @author Davide Scalisi
 * @brief 
 * @version 0.1
 * @date 2022-07-05
 * 
 * @copyright Copyright (c) 2022
 * 
 * UART ASCII interface library between two boards.
 */

#ifndef SERIAL_CONTROLLER_H
#define SERIAL_CONTROLLER_H

#include <Arduino.h>

//Buffer max length.
#define SERIAL_CONTROLLER_BUF_MAX_LEN 128	//Bytes.

//Single message max length (16 bytes).
#define SERIAL_CONTROLLER_MSG_MAX_LEN	SERIAL_CONTROLLER_BUF_MAX_LEN / 4

//Max decoded args count.
#define SERIAL_CONTROLLER_ARG_MAX_LEN	3

//ATmega328P.
#ifdef __AVR_ATmega328P__

#include <NeoSWSerial.h>

typedef enum : uint8_t{
} SerialController_command_t;

struct SerialController_data_t{
	SerialController_command_t com;
	uint8_t arg[SERIAL_CONTROLLER_ARG_MAX_LEN];
};

//Target class:	used to retrive ASCII data.
class SerialController_target{
	friend void ss_RX_ISR(uint8_t);

	private:
		NeoSWSerial *ss;

		//RX data circular buffer.
		static uint8_t *buf;

		//ISR buffer index.
		static uint16_t buf_cur;

		//Class buffer index.
		uint16_t k = 0;

		//Command, args, args_len, return.
		bool __decode(SerialController_data_t&, String&, String* = NULL, uint8_t = 0);

	public:
		//ss_rx, ss_tx.
		SerialController_target(uint8_t, uint8_t);
		~SerialController_target();

		void begin(uint32_t);

		bool recv(SerialController_data_t&);
		void send(SerialController_data_t&);
};

//ESP32.
#else

//Host class:	used to send ASCII data.
class SerialController_host{
	private:
	public:
};

#endif
#endif
