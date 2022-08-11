/**
 * @file packet.h
 * @author Davide Scalisi
 * @brief 
 * @version 0.1
 * @date 2022-08-11
 * 
 * @copyright Copyright (c) 2022
 * 
 * Definitions of the various binary data packets.
 */

#include <Arduino.h>
#include <CRC.h>

#define PACKET_DEFAULT_ARG_TYPE	float
#define PACKET_ARGV_MAXLEN			4

//Basic commands.
typedef enum : uint8_t{
	CONTROL_OK,
	CONTROL_WRONG_CRC,
	CONTROL_INVALID_MSG,

	COMMAND_RESET,
	COMMAND_RESET_ROUTINE,
	COMMAND_POSE,
	COMMAND_GOTO,
	COMMAND_KPID_GET,
	COMMAND_KPID_SET
} packet_command_t;

//Payload of the packet.
template<class T = PACKET_DEFAULT_ARG_TYPE> struct packet_payload_t{
	packet_command_t msg;
	uint8_t argc;
	T argv[PACKET_ARGV_MAXLEN];
};

//Entire packet.
template<class T = PACKET_DEFAULT_ARG_TYPE> struct packet_t{
	packet_payload_t<T> payload;
	uint8_t crc8;
};

//Get the CRC8 of the packet.
template<class T = PACKET_DEFAULT_ARG_TYPE> uint8_t crc8_packet(packet_t<T> &packet){
	return crc8((uint8_t*) &packet.payload, sizeof(packet.payload));
}
