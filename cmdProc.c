#include "main.h"
#include "cmdProc.h"

// store command format in structure array to compare
COMMAND cmdList[CMD_CNT] = {
	{ "help", "h", shell, help, false  }, { "dir", "d", shell, dir, false  },
	{ "quit", "q", shell, quit, false  }, { "history", "hi", shell, hist, false  },
	{ "dump", "du", memory, dump, true  }, { "edit", "e", memory, edit, true  },
	{ "fill", "f", memory, fill, true  }, { "reset", "reset", memory, reset, false  },
	{ "opcode", "opcode", opcode, op, true  }, { "opcodelist", "opcodelist", opcode, oplist, false },
	{ "invalid", "invalid", invalid, invFormat, true  }

};

USR_CMD findCMD(char* str) {
	int i, j;
	char delim[] = " \t\n"; // characters used to tokenize
	char inp[CMD_LEN];
	char* tok;
	USR_CMD u_cmd;

	strcpy(inp, str); // copy input string

	// initialize as invalid
	u_cmd.cmd = invFormat;
	u_cmd.arg_cnt = 0;
	
	if(!strlen(str))
		return u_cmd; // if empty string, return as invalid

	tok = strtok(inp, delim); // first word of input
	if(!tok)
		return u_cmd; // emtpy token
	for(i = 0; i < CMD_CNT - 1; i++)
		if(!strcmp(tok, cmdList[i].str) || !strcmp(tok, cmdList[i].abb)) {
			u_cmd.cmd = cmdList[i].func; // if input command matches one of hard coded commands
			break;
		}

	if(u_cmd.cmd == invFormat) // invalid command
		return u_cmd;

	// get arguments
	j = 0;
	while(tok) {
		tok = strtok(NULL, delim); // next token (NOT expected comma if valid command)
		if(!j && !tok) // no argument for command 
			break;
		if((j && !tok) || tok[0] == ',') { // there was a previous argument but empty token or comma found
			u_cmd.cmd = invFormat; //invalid command
			return u_cmd;
		}
		strcpy(u_cmd.arg[j++], tok); // copy argument to input command structure
		tok = strtok(NULL, delim); // next token (expected a comma if valid command)
		if(tok && tok[0] != ',') { // if token not empty, expected a comma
			u_cmd.cmd = invFormat;
			return u_cmd;
		}
	}
	u_cmd.arg_cnt = j; // save argument count

	// after input string parsed, do further check for validity
	switch(testValidInput(u_cmd, cmdList[i])) { // get error type, if any
		case FORMAT:
			u_cmd.cmd = invFormat;
			break;
		case HEX:
			u_cmd.cmd = invHex;
			break;
		case VALUE:
			u_cmd.cmd = invVal;
			break;
		default: // no error found
			break;
	}
	return u_cmd;
}

ER_CODE testValidInput(USR_CMD usr_cmd, COMMAND format) {
	int i;
	int arg[3];
	ER_CODE code = SAFE; // initialize as correct command
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

	// check hexadecimal number if command is memory-related
	if(format.type == memory) {
		for(i = 0; i < usr_cmd.arg_cnt; i++)
			if((arg[i] = hexToDec(usr_cmd.arg[i])) == -1)
				code = HEX;
		// check with each command's criteria
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

void invFormatCMD() {
	puts("ERROR: Invalid command.");
	puts("Type \"help\" for list and formats of commands.");
}

void invHexCMD() {
	puts("ERROR: Incorrect hexadecimal.");
}

void invValCMD() {
	puts("ERROR: Invalid address.");
	puts("Memory size:\t\t1MB [0x00000 ~ 0xFFFFF]");
	puts("Edit value range:\t 1B [0x00 ~ 0xFF]");
}

int hexToDec(char* hex) {
	int i, dec = 0, multiplier = 1;
	for(i = strlen(hex) - 1; i >= 0; i--) {
		if(!isxdigit(hex[i])) // check if character if hexadecimal digit
			return -1;
		dec += multiplier * (isdigit(hex[i]) ? (hex[i] - '0') : (toupper(hex[i]) - 'A' + 10));
		multiplier *= 16;
	}
	return dec;
}
