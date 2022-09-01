# packet.h: binary packet definition
## Commands

|	Command	|	Parameters	|	Return values	|	Description	|
|---------------|---------------|---------------|---------------|
|	`COMMAND_RESET`	|	N/A	|	N/A	|	Set the zero of the ortogonal system in the current point of the 2D space.	|
|	`COMMAND_RESET_ROUTINE`	|	N/A	|	N/A	|	Execute the automatic reset routine by using the two front switches.	|
|	`COMMAND_POSE`	|	N/A	|	&lt;x, y, theta&gt;	|	Get the pose of the robot.	|
|	`COMMAND_GOTO`	|	&lt;x, y&gt;	|	N/A	|	Go to the &lt;x, y&gt; point.	|
|	`COMMAND_WAIT_XY_ARRAY`	|	&lt;len&gt;	|	N/A	|	Await for an array of &lt;x, y&gt; points (floats) of length &lt;len&gt;.	|
|	`COMMAND_START\|STOP`	|	N/A	|	N/A	|	Start \| Stop engine.	|
|	`COMMAND_KPID_GET\|SET`	|	N/A \| &lt;p_module_kp, p_phase_kp, s_kp, s_ki&gt;	|	&lt;p_module_kp, p_phase_kp, s_kp, s_ki&gt;	|	Get \| Set the various PID constants.	|
|	`COMMAND_TOL_GET\|SET`	|	N/A \| &lt;tol_rho&gt;	|	&lt;tol_rho&gt;	|	Get \| Set the distance tollerance (m).	|
|	`COMMAND_MAX_SPEED_GET\|SET`	|	N/A \| &lt;max_linear_speed, max_angular_speed&gt;	|	&lt;max_linear_speed, max_angular_speed&gt;	|	Get \| Set robot maximum speeds (m/s, rad/s).	|
|	`COMMAND_LOAD\|SAVE`	|	N/A	|	N/A	|	Load \| Save current settings from \| to EEPROM.	|

**NB**:	Every command returns `CONTROL_OK` if it's accomplished correctly.

**NBB**:	Every angle is measured in radians by default.

## Control codes

|	Control code	|	Description	|
|---------------|-------------|
|	`CONTROL_OK`	|	Default reply message.	|
|	`CONTROL_ERROR`	|	Generic error.	|
|	`CONTROL_INVALID_MSG`	|	Invalid command.	|
