INPUT_CMD findCMD(char*); 					// function to find command type of input
ERROR_CODE testValidInput(INPUT_CMD, COMMAND); 	// function to check validity of input command

void invFormatCMD();	// called when invalid format input
void invHexCMD();		// called when invalid hexadecimal value
void invValCMD();		// called when invalid value
