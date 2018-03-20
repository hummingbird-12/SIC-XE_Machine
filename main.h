#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>

#define CMD_LEN 12
#define CMD_CNT 11
#define PAR_MAX 3

typedef struct dirent ENTRY;
typedef struct stat STBUF;
typedef enum { false, true } bool;

typedef enum {
	shell, memory, opcode, invalid
} CMD_TYPE;

typedef enum {
	help, dir, quit, hist, dump, edit, fill, reset, op, oplist, inv
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
	short par_cnt;
	char par[PAR_MAX];
} INPUT;

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
	{ "invalid", "invalid", invalid, inv, true  }

};

HIST_NODE* hist_head = NULL;

char mem[65536][32];

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

void hist_add(char*);
void hist_free();
