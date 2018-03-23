#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <math.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>

#define CMD_LEN 257			// maximum length of input string
#define CMD_CNT 11			// list of command formats
#define ARG_MAX 3			// maximum argument count
#define MEM_SIZE 1048576 	// 2^20 = 1MB
#define HASH_SIZE 20		// hash table size

typedef struct dirent ENTRY;		// for dir command
typedef struct stat STBUF;			// for dir command
typedef enum { false, true } bool;	// workaround for bool type in C

// error codes
typedef enum { 
	SAFE, FORMAT, HEX, VALUE
} ER_CODE;

// command types
typedef enum {
	shell, memory, opcode, invalid
} CMD_TYPE;

// command functions
typedef enum {
	help, dir, quit, hist, dump, edit, fill, reset, op, oplist, invFormat, invHex, invVal
} CMD_FUNC;

// command format structure
typedef struct {
	char str[CMD_LEN];
	char abb[CMD_LEN];
	CMD_TYPE type;
	CMD_FUNC func;
	bool arg;
} COMMAND;

// user input command parse structure
typedef struct {
	CMD_FUNC cmd;
	short arg_cnt;
	char arg[ARG_MAX][10];
} USR_CMD;

// history node
typedef struct hist_struct {
	char str[CMD_LEN];
	struct hist_struct* next;
} HIST_NODE;

// hash table bucket
typedef struct hash_struct {
	int codeVal;
	char code[3];
	char inst[CMD_LEN];
	enum { m1, m2, m34 } mode;
	struct hash_struct* next;
} HASH_ENTRY;

int hexToDec(char*); // function that checks for vaild hex value and return converted decimal value
void hash_free();	 // function to free hash table memory allocation
