#include "common.h"
#include "main.h"
#include "shell_cmd.c"
#include "memory_cmd.c"
#include "opcode_cmd.c"

int main() {
	while(1) {
		printf("sicsim> ");
		fgets(cmd, MAX_LEN, stdin);
		switch(cmd_type()) {
			case shell:
				puts("Shell Command");
				break;
			case memory:
				puts("Memory Command");
				break;
			case opcode:
				puts("Opcode Command");
				break;
			case invalid:
				puts("Invalid Command");
		}
	}
	return 0;
}

enum CMD_TYPE cmd_type() {
	CMD_TYPE type;
	
	switch(cmd[0]) {
		case 'h':
		case 'q':
		case 'd':
			type = shell;
			break;
		case 'e':
		case 'f':
		case 'r':
			type = memory;
			break;
		case 'o':
			type = opcode;
			break;
		default:
			type = invalid;
	}
	if(type == shell && cmd[1] == 'u')
		type = memory;
	return type;
}
