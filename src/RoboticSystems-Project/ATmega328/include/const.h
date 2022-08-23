//PIN.
#define ODOMETRY_SWITCH	7
#define RESERVED_PIN		8		//Unused GPIO.

#define LEFT_ENCODER_A	2
#define LEFT_ENCODER_B	11

#define RIGHT_ENCODER_A	3
#define RIGHT_ENCODER_B	12

#define LEFT_DIRECTION	4
#define RIGHT_DIRECTION	5

//NeoSWSerial.
#define SS_RX			A0
#define SS_TX			A1
#define SS_SPEED	9600

//Control system sampling rate (8ms -> 125Hz).
#define DELTA_T	8e-3

//Number of plotted samples per second.
#define N_SAMPLES	30

//Encoder wheel parameters.
#define ENC_RADIUS			0.021
#define ENC_TICKS				1000
#define ENC_WHEELBASE		0.305

//Locomotion wheel parameters.
// #define LOC_RADIUS			0.0315
// #define LOC_WHEELBASE	0.182

//Robot traits.
// #define ROB_WEIGHT			6			//Kg

// #define ROB_LIN_AIR_FRICTION	0.8
// #define ROB_ROT_AIR_FRICTION	0.8

//EEPROM settings (default values are for testing).
struct settings_t{
	//Speed controller PID constants.
	float s_kp = 1000;
	float s_ki = 3000;

	//Position controller PID constants.
	float p_module_kp = 1;
	float p_phase_kp = 1;

	//Tollerance errors definition (m).
	float tol_rho = 0.1;

	//Robot max speeds (m/s, rad/s).
	float max_linear_speed = 0.5;
	float max_angular_speed = radians(180);
};
