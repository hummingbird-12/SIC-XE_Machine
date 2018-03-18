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
	char* cmd_str;
	CMD_TYPE type;
	CMD exec;
	bool param;
} COMMAND;



char cmd[MAX_LEN];

CMD_TYPE cmd_type();
