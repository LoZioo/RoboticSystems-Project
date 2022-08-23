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

//Basic controls and commands.
typedef enum : uint8_t{
	CONTROL_OK,
	CONTROL_ERROR,
	CONTROL_INVALID_COM,

	COMMAND_RESET,
	COMMAND_RESET_ROUTINE,

	COMMAND_POSE,
	COMMAND_GOTO,

	COMMAND_START,
	COMMAND_STOP,

	COMMAND_KPID_GET,
	COMMAND_KPID_SET,

	COMMAND_TOL_GET,
	COMMAND_TOL_SET,

	COMMAND_MAX_SPEED_GET,
	COMMAND_MAX_SPEED_SET,

	COMMAND_SAVE,
	COMMAND_LOAD
} packet_data_t;

#define PACKET_ARGV_MAXLEN	4

/*
	__attribute((__packed__)) is needed because we're using that
	struct to communicate with a different CPU arcitecture.

	With __attribute((__packed__)), the size of the struct will
	be the same across any CPU architecture.
*/
template<class T = float> struct __attribute((__packed__)) packet_t{
	packet_data_t com;

	uint8_t argc;
	T argv[PACKET_ARGV_MAXLEN];
};
