#include "shell_cmd.h"

void SHELL(char* cmd) {
	if(!strcmp(cmd, "help") || !strcmp(cmd, "h"))
		helpCMD();
	else if(!strcmp(cmd, "d") || !strcmp(cmd, "dir"))
		dirCMD();
	else if(!strcmp(cmd, ""))
}

void helpCMD() {

}

void dirCMD() {

}

void quitCMD() {

}

void histCMD() {

}
