#include "common.h"

COMMAND cmdList[CMD_CNT] = {
	{ "help", "h", shell, help, false }, { "dir", "d", shell, dir, false },
	{ "quit", "q", shell, quit, false }, { "history", "hi", shell, hist, false },
	{ "dump", "du", memory, dump, true }, { "edit", "e", memory, edit, true },
	{ "fill", "f", memory, fill, true }, { "reset", "reset", memory, reset, false },
	{ "opcode", "opcode", opcode, op, true }, { "opcodelist", "opcodelist", opcode, oplist, false},
	{ "invalid", "invalid", invalid, inv, true }
};

char cmd[CMD_LEN];
