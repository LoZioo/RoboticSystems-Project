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

//Sampling rate (8ms -> 125Hz)
#define DELTA_T	8e-3

//Sampling rate (1ms -> 1kHz)
// #define DELTA_T	1e-3

//RI32 wheel parameters.
#define ENC_RADIUS			0.021
#define ENC_TICKS				1000
#define ENC_WHEELBASE		0.305

//Locomotion wheel parameters.
#define LOC_RADIUS			0.0315
#define LOC_WHEELBASE		0.182

//Robot traits.
#define ROB_WEIGHT						6			//Kg
#define ROB_MAX_SPEED					1			//m/s

#define ROB_LIN_AIR_FRICTION	0.8
#define ROB_ROT_AIR_FRICTION	0.8
