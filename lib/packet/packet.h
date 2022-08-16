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

#define PACKET_ARGV_MAXLEN	4

//Basic commands.
typedef enum : uint8_t{
	CONTROL_OK,
	CONTROL_INVALID_MSG,

	COMMAND_RESET,
	COMMAND_RESET_ROUTINE,
	COMMAND_POSE,
	COMMAND_GOTO,
	COMMAND_KPID_GET,
	COMMAND_KPID_SET
} packet_command_t;

/*
	__attribute((__packed__)) is needed because we're using that
	struct to communicate with a different CPU arcitecture.

	With __attribute((__packed__)), the size of the struct will
	be the same across any CPU architecture.
*/
template<class T = float> struct __attribute((__packed__)) packet_t{
	packet_command_t com;
	uint8_t argc;
	T argv[PACKET_ARGV_MAXLEN];
};
