/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                               *
 *                    Sogang University                          *
 *         Department of Computer Science and Engineering        *
 *                                                               *
 * Subject name: System Programming                              *
 * Project title: [1] SIC/XE Machine - The Basics                *
 *                                                               *
 * Author: Inho Kim                                              *
 * Student ID: 20161577                                          *
 *                                                               *
 * File name: 20161577.h                                         *
 * File description: Main header file for the project.           *
 *                                                               *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <math.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>

#define CMD_LEN 257         // maximum length of input string
#define CMD_CNT 14          // list of command formats
#define ARG_MAX 3           // maximum argument count
#define MEM_SIZE 1048576    // 2^20 = 1MB
#define HASH_SIZE 20        // hash table size
#define ASM_LEN 31          // maximum length of single asm source line

typedef struct dirent ENTRY;        // for dir command
typedef struct stat STBUF;          // for dir command
typedef enum { false, true } bool;  // workaround for bool type in C

// error codes, for better readability
typedef enum { 
    SAFE, FORMAT, HEX, VALUE, FILENAME
} ERROR_CODE;

// command types, for better readability
typedef enum {
    shell, memory, opcode, assembler, invalid
} CMD_TYPE;

// command functions, for better readablitiy
typedef enum {
    help, dir, quit, hist, dump, edit, fill, reset, op, oplist, type, assemble, symbol, invFormat, invHex, invVal, invFile
} CMD_FUNC;

// command format structure
typedef struct {
    char str[CMD_LEN];
    char abb[CMD_LEN];
    CMD_TYPE category;
    CMD_FUNC func;
    bool arg;
} COMMAND;

// user input command parsed structure
typedef struct {
    CMD_FUNC cmd;
    short argCnt;
    char arg[ARG_MAX][10];
} INPUT_CMD;

// history node structure
typedef struct HIST_STRUCT {
    char str[CMD_LEN];
    struct HIST_STRUCT* next;
} HIST_NODE;

// hash table bucket structure
typedef struct HASH_STRUCT {
    int codeVal;
    char code[3];
    char inst[CMD_LEN];
    enum { f1, f2, f34 } format;
    int operandCnt;
    struct HASH_STRUCT* next;
} HASH_ENTRY;

typedef struct {
    char label[ASM_LEN];
    char inst[ASM_LEN];
    char operand[2][ASM_LEN];
    int location;
    enum { ERROR, inst, direc, comment } type;
    bool hasLabel;
    enum { SAFE, SYMBOL, OPCODE, OPERAND } errorCode;
} ASM_SRC;
    
// symbol table entry structure
typedef struct SYMBOL_STRUCT {
    char symbol[ASM_LEN];
    int address;
    struct SYMBOL_STRUCT* next;
} SYMBOL_ENTRY;

int hexToDec(char*); // function to check for vaild hex value and return converted decimal value
void hashFree();     // function to free hash table memory allocation

HASH_ENTRY* bucketFound(char*);                // function to search bucket
