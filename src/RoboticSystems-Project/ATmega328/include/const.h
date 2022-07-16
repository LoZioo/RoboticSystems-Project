#define ODOMETRY_SWITCH	7
#define RESERVED_PIN		8		//Unused GPIO.

#define LEFT_ENCODER_A	2
#define LEFT_ENCODER_B	11

#define RIGHT_ENCODER_A	3
#define RIGHT_ENCODER_B	12

#define LEFT_DIRECTION	4
#define RIGHT_DIRECTION	5

//NeoSWSerial
#define SS_RX	A0
#define SS_TX	A1

//Sampling rate (1kHz)
#define DELTA_T	1e-3

//RI32 wheel parameters.
#define ENC_RADIUS			0.021
#define ENC_TICKS				1000
#define ENC_WHEELBASE		0.1525
// #define ENC_WHEELBASE		0.305
