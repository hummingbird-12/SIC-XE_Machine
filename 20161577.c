#include "main.h"
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>

typedef struct dirent ENTRY;
typedef struct stat STBUF;

COMMAND findCMD(char*);
void helpCMD();
void dirCMD();

int main() {
	COMMAND cmdExec;
	while(1) {
		printf("sicsim> ");
		fgets(cmd, MAX_LEN, stdin);
		cmd[strlen(cmd) - 1] = '\0';
		cmdExec = findCMD(cmd);

		switch(cmdExec.func) {
			case help:
				helpCMD();
				break;
			case dir:
				dirCMD();
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

bool isValidCMD(char* str, COMMAND CMDformat) {
//	if(CMD)
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

void dirCMD() {
	DIR* dir = opendir(".");
	char* e_str;
	char path[258] = "./";
	ENTRY* ent;
	STBUF buf;

	if(!dir) {
		puts("ERROR opening directory...");
		return;
	}
	ent = readdir(dir);
	while(ent) {
		path[2] = '\0';
		e_str = ent->d_name;
		stat(strcat(path, e_str), &buf);
		printf("\t%-s", e_str);
		if(S_ISDIR(buf.st_mode))
			printf("/");
		else if(buf.st_mode & S_IXUSR) //if(!strcmp(e_str + strlen(e_str) - 4, ".out"))
			printf("*");
		ent = readdir(dir);
	}
	closedir(dir);
	puts("");
}
