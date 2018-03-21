#include "main.h"

int main() {
	char inp[CMD_LEN];
	USR_CMD cmdExec;

	resetCMD();

	while(1) {
		printf("sicsim> ");
		fgets(inp, CMD_LEN, stdin);
		inp[strlen(inp) - 1] = '\0';
		cmdExec = findCMD(inp);
		if(cmdExec.cmd != inv)
			hist_add(inp);

		switch(cmdExec.cmd) {
			case help:
				helpCMD();
				break;
			case dir:
				dirCMD();
				break;
			case quit:
				quitCMD();
				break;
			case hist:
				histCMD();
				break;
			case dump:
				break;
			case edit:
				break;
			case fill:
				break;
			case reset:
				resetCMD();
				break;
			case op:
				break;
			case oplist:
				break;
			case inv:
				invCMD();
				break;
		}
	}
	return 0;
}

USR_CMD findCMD(char* str) {
	int i, j;
	char delim[] = " ,\t\n";
	char inp[CMD_LEN];
	char* tok;
	USR_CMD u_cmd;

	strcpy(inp, str);

	u_cmd.cmd = inv; // initialize as invalid
	u_cmd.param_cnt = 0;

	tok = strtok(inp, delim); // first word of input
	for(i = 0; i < CMD_CNT - 1; i++)
		if(!strcmp(tok, cmdList[i].str) || !strcmp(tok, cmdList[i].abb)) {
			u_cmd.cmd = cmdList[i].func;
			break;
		}

	if(u_cmd.cmd == inv) // invalid command
		return u_cmd;

	if(cmdList[i].param) { // get parameters
		j = 0;
		while((tok = strtok(NULL, delim)))
			strcpy((u_cmd.param)[j++], tok);
		u_cmd.param_cnt = j;
		if(j > 2)
			u_cmd.cmd = inv;
	}
	else if((tok = strtok(NULL, delim))) // not expected parameter
		u_cmd.cmd = inv; // set as invalid

	return u_cmd;
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
	puts("Exiting SIC...");
	hist_free();
	exit(0);

}

void histCMD() {
	HIST_NODE* cur = hist_head;
	int cnt = 1;

	while(cur) {
		printf("\t%-3d  ", cnt++);
		puts(cur->str);
		cur = cur->next;
	}
}

void dumpCMD(USR_CMD cmd) {

}

void editCMD(USR_CMD cmd) {

}

void fillCMD(USR_CMD cmd) {

}

void resetCMD() {
	int i, j;
	for(i = 0; i < MEM_VLEN; i++)
		for(j = 0; j < MEM_HLEN; j++)
			mem[i][j] = '0';
}

void opCMD(USR_CMD cmd) {

}

void oplistCMD() {

}

void invCMD() {
	puts("ERROR: Invalid command.");
	puts("Type \"help\" for list and formats of commands.");
}

void hist_add(char* str) {
	HIST_NODE* cur = hist_head;
	HIST_NODE* new_hist = malloc(sizeof(HIST_NODE));
	strcpy(new_hist->str, str);
	new_hist->next = NULL;

	if(!hist_head) {
		hist_head = new_hist;
		return;
	}
	while(cur->next)
		cur = cur->next;
	cur->next = new_hist;
}

void hist_free() {
	HIST_NODE* cur = hist_head;
	HIST_NODE* nex;
	while(cur) {
		nex = cur->next;
		free(cur);
		cur = nex;
	}
	hist_head = NULL;
}
