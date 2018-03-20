#include "common.h"
#define MAX_LEN 12
#define CMD_CNT 11

typedef enum {
	shell, memory, opcode, invalid
} CMD_TYPE;

typedef enum {
	help, dir, quit, hist, dump, edit, fill, reset, op, oplist, inv
} CMD_FUNC; 

typedef enum { false, true } bool;

typedef struct {
	char str[MAX_LEN];
	char abb[MAX_LEN];
	CMD_TYPE type;
	CMD_FUNC func;
	bool param;
} COMMAND;

COMMAND cmdList[CMD_CNT] = {
	{ "help", "h", shell, help, false }, { "dir", "d", shell, dir, false },
	{ "quit", "q", shell, quit, false }, { "history", "hi", shell, hist, false },
	{ "dump", "du", memory, dump, true }, { "edit", "e", memory, edit, true },
	{ "fill", "f", memory, fill, true }, { "reset", "reset", memory, reset, false },
	{ "opcode", "opcode", opcode, op, true }, { "opcodelist", "opcodelist", opcode, oplist, false},
	{ "invalid", "invalid", invalid, inv, true }
};

char cmd[MAX_LEN];

CMD_TYPE cmd_type();
