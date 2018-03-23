#include "main.h"

USR_CMD findCMD(char* str) {
	int i, j;
	char delim[] = " \t\n";
	char inp[CMD_LEN];
	char* tok;
	USR_CMD u_cmd;

	strcpy(inp, str);

	u_cmd.cmd = invFormat; // initialize as invalid
	u_cmd.arg_cnt = 0;
	
	if(!strlen(str))
		return u_cmd;

	tok = strtok(inp, delim); // first word of input
	if(!tok)
		return u_cmd;
	for(i = 0; i < CMD_CNT - 1; i++)
		if(!strcmp(tok, cmdList[i].str) || !strcmp(tok, cmdList[i].abb)) {
			u_cmd.cmd = cmdList[i].func;
			break;
		}

	if(u_cmd.cmd == invFormat) // invalid command
		return u_cmd;

	// get arguments
	j = 0;
	while(tok) {
		tok = strtok(NULL, delim);
		if(!j && !tok)
			break;
		if((j && !tok) || tok[0] == ',') {
			u_cmd.cmd = invFormat;
			return u_cmd;
		}
		strcpy(u_cmd.arg[j++], tok);
		tok = strtok(NULL, delim);
		if(tok && tok[0] != ',') {
			u_cmd.cmd = invFormat;
			return u_cmd;
		}
	}
	u_cmd.arg_cnt = j;

	switch(testValidInput(u_cmd, cmdList[i])) {
		case FORMAT:
			u_cmd.cmd = invFormat;
			break;
		case HEX:
			u_cmd.cmd = invHex;
			break;
		case VALUE:
			u_cmd.cmd = invVal;
			break;
		default:
			break;
	}
	return u_cmd;
}

ER_CODE testValidInput(USR_CMD usr_cmd, COMMAND format) {
	int i;
	int arg[3];
	ER_CODE code = SAFE;
	if(usr_cmd.cmd == invFormat)
		return FORMAT;

	// check argument count
	switch(usr_cmd.cmd) {
		// strictly 0 arguments
		case help:
		case dir:
		case quit:
		case hist:
		case reset:
		case oplist:
			if(usr_cmd.arg_cnt)
				code = FORMAT;
			break;
		// strictly 1 argument
		case op:
			if(usr_cmd.arg_cnt != 1)
				code = FORMAT;
			break;
		// strictly 2 arguments
		case edit:
			if(usr_cmd.arg_cnt != 2)
				code = FORMAT;
			break;
		// strictly 3 arguments
		case fill:
			if(usr_cmd.arg_cnt != 3)
				code = FORMAT;
			break;
		// need less than 3
		case dump:
			if(usr_cmd.arg_cnt > 2)
				code = FORMAT;
			break;
		default:
			break;
	}
	if(code == FORMAT)
		return code;

	// check hexadecimal number
	if(format.type == memory) {
		for(i = 0; i < usr_cmd.arg_cnt; i++)
			if((arg[i] = hexToDec(usr_cmd.arg[i])) == -1)
				code = HEX;
		switch(usr_cmd.cmd) {
			case edit:
				if(arg[0] >= MEM_SIZE || arg[1] > 255)
					code = VALUE;
				break;
			case fill:
				if(arg[0] >= MEM_SIZE || arg[1] >= MEM_SIZE || arg[0] > arg[1] || arg[2] > 255)
					code = VALUE;
				break;
			case dump:
				switch(usr_cmd.arg_cnt) {
					case 2:
						if(arg[1] >= MEM_SIZE || arg[0] > arg[1])
							code = VALUE;
					case 1:
						if(arg[0] >= MEM_SIZE)
							code = VALUE;
						break;
				}
				break;
			default:
				break;
		}
	}
	return code;
}

int hexToDec(char* hex) {
	int i, dec = 0, multiplier = 1;
	for(i = strlen(hex) - 1; i >= 0; i--) {
		if(!isxdigit(hex[i]))
			return -1;
		dec += multiplier * (isdigit(hex[i]) ? (hex[i] - '0') : (toupper(hex[i]) - 'A' + 10));
		multiplier *= 16;
	}
	return dec;
}
