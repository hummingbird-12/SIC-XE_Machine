#include "main.h"

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
		}
	}
	return 0;
}

CMD_TYPE cmd_type() {
	CMD_TYPE tp;
	
	switch(cmd[0]) {
		case 'h':
		case 'q':
		case 'd':
			tp = shell;
			break;
		case 'e':
		case 'f':
		case 'r':
			tp = memory;
			break;
		case 'o':
			tp = opcode;
			break;
	}
	if(tp == shell && !strncmp(cmd, "du", 2))
		tp = memory;
	return tp;
}
