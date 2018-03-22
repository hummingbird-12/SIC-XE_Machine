#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <math.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>

#define CMD_LEN 101
#define CMD_CNT 11
#define PAR_MAX 3
#define MEM_SIZE 1048576 	// 2^20 = 1MB
#define MEM_VLEN 65536 		// 2^16
#define MEM_HLEN 32 		// 2^5

typedef struct dirent ENTRY;
typedef struct stat STBUF;
typedef enum { false, true } bool;

typedef enum { 
	SAFE, FORMAT, HEX, VALUE
} ER_CODE;

typedef enum {
	shell, memory, opcode, invalid
} CMD_TYPE;

typedef enum {
	help, dir, quit, hist, dump, edit, fill, reset, op, oplist, invFormat, invHex, invVal
} CMD_FUNC;

typedef struct {
	char str[CMD_LEN];
	char abb[CMD_LEN];
	CMD_TYPE type;
	CMD_FUNC func;
	bool param;
} COMMAND;

typedef struct {
	CMD_FUNC cmd;
	short param_cnt;
	char param[PAR_MAX][10];
} USR_CMD;

typedef struct hist_struct {
	char str[CMD_LEN];
	struct hist_struct* next;
} HIST_NODE;

COMMAND cmdList[CMD_CNT] = {
	{ "help", "h", shell, help, false  }, { "dir", "d", shell, dir, false  },
	{ "quit", "q", shell, quit, false  }, { "history", "hi", shell, hist, false  },
	{ "dump", "du", memory, dump, true  }, { "edit", "e", memory, edit, true  },
	{ "fill", "f", memory, fill, true  }, { "reset", "reset", memory, reset, false  },
	{ "opcode", "opcode", opcode, op, true  }, { "opcodelist", "opcodelist", opcode, oplist, false },
	{ "invalid", "invalid", invalid, invFormat, true  }

};

HIST_NODE* hist_head = NULL;

char mem[MEM_VLEN * MEM_HLEN];
short mem2[MEM_SIZE];

USR_CMD findCMD(char*);
ER_CODE testValidInput(USR_CMD, COMMAND);

void helpCMD();
void dirCMD();
void quitCMD();
void histCMD();
void dumpCMD(USR_CMD);
void editCMD(USR_CMD);
void fillCMD(USR_CMD);
void resetCMD();
void opCMD(USR_CMD);
void oplistCMD();
void invFormatCMD();
void invHexCMD();
void invValCMD();

void hist_add(char*);
void hist_free();

//char* decToHex(int);
int hexToDec(char*);
//bool isValidHex(char*);
bool testValidAdr(char*, char*);
