#include "common.h"
#define MAX_LEN 12

typedef enum {
	shell, memory, opcode
} CMD_TYPE;

typedef enum {
	help, dir, quit, hist, dump, edit, fill, reset, op, oplist
} CMD; 

typedef enum { false, true } bool;

typedef struct {
	char str[MAX_LEN];
	char abb[MAX_LEN];
	CMD_TYPE type;
	CMD exec;
	bool param;
} COMMAND;

COMMAND[] = {
	{ "help", "h", shell, help, false }, { "dir", "d", shell, dir, false },
	{ "quit", "q", shell, quit, false }, { "history", "hi", shell, hist, false },
	{ "dump", "du", memory, dump, true }, { "edit", "e", memory, edit, true },
	{ "fill", "f", memory, fill, true }, { "reset", "\0", memory, reset, false },
	{ "opcode", "\0", opcode, op, true }, { "opcodelist", "\0", opcode, oplist, false}
}

char cmd[MAX_LEN];

CMD_TYPE cmd_type();
