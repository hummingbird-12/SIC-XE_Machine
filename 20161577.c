#include "main.h"

COMMAND findCMD(char*);
void helpCMD();
void dirCMD();
void quitCMD();
void histCMD();
void dumpCMD();
void editCMD();
void fillCMD();
void resetCMD();
void opCMD();
void oplistCMD();

int main() {
	COMMAND cmdExec;
	while(1) {
		printf("sicsim> ");
		fgets(cmd, CMD_LEN, stdin);
		cmd[strlen(cmd) - 1] = '\0';
		cmdExec = findCMD(cmd);

		switch(cmdExec.func) {
			case help:
				helpCMD();
				break;
			case dir:
				dirCMD();
				break;
			case quit:
				break;
			case hist:
				break;
			case dump:
				break;
			case edit:
				break;
			case fill:
				break;
			case reset:
				break;
			case op:
				break;
			case oplist:
				break;
			case inv:
				break;
		}
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
	DIR* dir = opendir("."); // current directory
	char* e_str;
	char path[258] = "./"; // entry path string
	ENTRY* ent; // entry
	STBUF buf; // stat

	if(!dir) {
		puts("ERROR opening directory...");
		return;
	}
	ent = readdir(dir); // read entry
	while(ent) {
		path[2] = '\0'; // clear path string
		e_str = ent->d_name; // entry name
		stat(strcat(path, e_str), &buf);
		printf("\t%-s", e_str); // print entry name

		if(S_ISDIR(buf.st_mode)) // check for directory
			printf("/");
		else if(buf.st_mode & S_IXUSR) // check for exec file
			printf("*");

		ent = readdir(dir); // read next entry
	}
	closedir(dir);
	puts("");
}

void quitCMD() {

}
