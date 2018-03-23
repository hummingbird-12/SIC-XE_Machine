#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <math.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>

#define CMD_LEN 257
#define CMD_CNT 11
#define ARG_MAX 3
#define MEM_SIZE 1048576 	// 2^20 = 1MB
#define HASH_SIZE 20

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
	bool arg;
} COMMAND;

typedef struct {
	CMD_FUNC cmd;
	short arg_cnt;
	char arg[ARG_MAX][10];
} USR_CMD;

typedef struct hist_struct {
	char str[CMD_LEN];
	struct hist_struct* next;
} HIST_NODE;

typedef struct hash_struct {
	int codeVal;
	char code[3];
	char inst[CMD_LEN];
	enum { m1, m2, m34 } mode;
	struct hash_struct* next;
} HASH_ENTRY;

int hexToDec(char*);
void hash_free();
