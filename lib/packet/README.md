# packet.h: binary packet definition
## Commands

|	Command	|	Parameters	|	Return values	|	Description	|
|---------------|---------------|---------------|---------------|
|	`COMMAND_RESET`	|	N/A	|	N/A	|	Set the zero of the ortogonal system in the current point of the space.	|
|	`COMMAND_RESET_ROUTINE`	|	N/A	|	N/A	|	Execute the automatic reset routine by using the two front switches.	|
|	`COMMAND_POSE`	|	N/A	|	&lt;x, y, theta&gt;	|	Get the pose of the robot.	|
|	`COMMAND_GOTO`	|	&lt;x, y, theta&gt;	|	N/A	|	Go to the &lt;x, y&gt; point and keep the orientation angle at &lt;theta&gt; radiands.	|
|	`COMMAND_KPID_GET\|SET`	|	N/A \| &lt;p_module_kp, p_phase_kp, s_kp, s_ki&gt;	|	&lt;p_module_kp, p_phase_kp, s_kp, s_ki&gt;	|	Get \| Set the various PID constants.	|
|	`COMMAND_KPID_LOAD\|SAVE`	|	N/A	|	&lt;p_module_kp, p_phase_kp, s_kp, s_ki&gt;	|	Load \| Save PID constants from \| to EEPROM.	|

**NB**:	Every command returns `CONTROL_OK` if it was accomplished correctly.

## Control codes

|	Control code	|	Description	|
|---------------|-------------|
|	`CONTROL_OK`	|	Default reply message.	|
|	`CONTROL_ERROR`	|	Generic error.	|
|	`CONTROL_INVALID_MSG`	|	Invalid command.	|
