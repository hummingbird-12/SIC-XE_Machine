#include "main.h"
#include "shell.h"

HIST_NODE* hist_head = NULL;

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
		if(ent)
			puts("");
	}
	closedir(dir);
	puts("");
}

void quitCMD() {
	puts("Exiting SIC...");
	hist_free();
	hash_free();
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
