/**
 * @file SerialPlotter.h
 * @author Davide Scalisi
 * @brief Serial plotter API library.
 * 				https://hackaday.io/project/5334-serialplot-realtime-plotting-software
 * 
 * 				SerialPlot custom frame settings:
 * 					- Frame start:	AA BB
 * 					- # Channels:		# of push() calls.
 * 					- Frame size:		First byte of the payload is size.
 * 					- Number Type:	Type of T (on the template).
 * 					- Endianness:		Litte Endian.
 * 					- Checksum:			Disabled.
 * 
 * @version 0.1
 * @date 2022-07-28
 * 
 * @copyright Copyright (c) 2022
 */

#ifndef SERIALPLOTTER_H
#define SERIALPLOTTER_H

//Maximum number of channels.
#define PAYLOAD_MAXCHANNELS	10

#include <Arduino.h>

//SerialPlot data type.
template<class T> class SerialPlotter{
	private:
		Stream& serial;

		uint8_t sp;
struct SerialPlotter_frame_t{
	const uint16_t frame_start = 0xBBAA;

	uint8_t payload_length;
	T payload[PAYLOAD_MAXCHANNELS];
} data;

		bool append = false;

	public:
		//Serial used to transmit data.
		SerialPlotter(Stream& serial) : serial(serial){}

		void start();
		void add(T);
		void plot();
};

#endif
