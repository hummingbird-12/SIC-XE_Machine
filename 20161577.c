#include "main.h"

COMMAND findCMD(char*);
void helpCMD();

int main() {
	COMMAND cmdExec;
	while(1) {
		printf("sicsim> ");
		fgets(cmd, MAX_LEN, stdin);
		cmd[strlen(cmd) - 1] = '\0';
		cmdExec = findCMD(cmd);

		switch(cmdExec.exec) {
			case help:
				helpCMD();
				break;
		}
		/*
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
		   */
	}
	return 0;
}

COMMAND findCMD(char* str) {
	int i;
	for(i = 0; i < CMD_CNT - 1; i++)
		if(!strcmp(str, cmdList[i].str) || !strcmp(str, cmdList[i].abb))
			return cmdList[i];
	return cmdList[CMD_CNT - 1];
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

void helpCMD() {
	printf("h[elp]\n"
			"d[ir]\n"
			"q[uit]\n"
			"hi[story]\n"
			"du[mp] [start, end]\n"
			"e[dit] address, value\n"
			"f[ill] start, end, value\n"
			"reset\n"
			"opcode mnemonic\n"
			"opcodelist\n");
}
