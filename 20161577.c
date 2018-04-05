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
 * File name: 20161577.c                                         *
 * File description: Main file for the project.                  *
 *                                                               *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "20161577.h"
#include "cmdProc.h"
#include "shell.h"
#include "memory.h"
#include "hash.h"

SYMBOL_ENTRY* symTable;

void assembleCMD(INPUT_CMD);
ASM_SRC* parseASM(char*);
void symbolCMD();
void symTableAdd(ASM_SRC);
void symTableFree();

int main() {
    char inp[CMD_LEN];  // input string
    char tmp[CMD_LEN];  // temporary string to copy input
    int i, j;
    INPUT_CMD input;    // storage for parsed input

    resetCMD(); // initialize memory
    hashCreate(); // create hash table of opcodes

    while(1) {
        printf("sicsim> ");
        fgets(inp, CMD_LEN, stdin); // get input string
        inp[strlen(inp) - 1] = '\0'; // replace \n with null character
        
        // copy input string to tmp but place one space before and after comma ','
        j = 0;
        for(i = 0; inp[i]; i++) {
            if(inp[i] == ',') {
                strcpy(tmp + j, " , "); // place space around commma ','
                j += 3;
            }
            else
                tmp[j++] = inp[i];
        }
        tmp[j] = '\0';

        input = findCMD(tmp); // find the command format from input string
        if(input.cmd < invFormat)
            histAdd(inp); // if command is not invalid add to history

        // call function for each command
        switch(input.cmd) {
            case help:
                helpCMD();
                break;
            case dir:
                dirCMD();
                break;
            case quit:
                quitCMD();
                break;
            case hist:
                histCMD();
                break;
            case dump:
                dumpCMD(input);
                break;
            case edit:
                editCMD(input);
                break;
            case fill:
                fillCMD(input);
                break;
            case reset:
                resetCMD();
                break;
            case op:
                opCMD(input);
                break;
            case oplist:
                oplistCMD();
                break;
            case type:
                typeCMD(input);
                break;
            case assemble:
                assembleCMD(input);
                break;
            case symbol:
                symbolCMD();
                break;
            case invFormat:
                invFormatCMD();
                break;
            case invHex:
                invHexCMD();
                break;
            case invVal:
                invValCMD();
                break;
            case invFile:
                invFileCMD();
                break;
        }
    }
    return 0;
}

void assembleCMD(INPUT_CMD ipcmd) {
    FILE* fp = fopen(ipcmd.arg[0], "r");
    char source[ASM_LEN];
    ASM_SRC* parsed = NULL;

    if(!fp) {
        puts("ERROR: File not found.");
        return;
    }
    fgets(source, ASM_LEN, fp);
    if((parsed = parseASM(source)) == NULL)
        puts("HAHA");
    else {
        puts("GOOD!");
        printf("%s\n", parsed->inst);
    }
    if(fclose(fp))
        puts("WARNING: Error closing file.");
}

void symbolCMD() {
}

ASM_SRC* parseASM(char* source) {
    char delim[] = " \t\n";
    char tokens[5][ASM_LEN] = {'\0'};
    char str[ASM_LEN] = {'\0'};
    char *tok;
    int i, j, tokCnt = 0;
    ASM_SRC* parseResult = NULL;
    HASH_ENTRY* bucket = NULL;

    j = 0;
    for(i = 0; source[i]; i++) {
        if(source[i] == ',') {
            strcpy(str + j, " , ");
            j += 3;
        }
        else
            str[j++] = source[i];
    }

    tok = strtok(str, delim);
    if(!strcmp(tok, ".")) { // check for comment
        parseResult = (ASM_SRC*) malloc(sizeof(ASM_SRC));
        parseResult->label[0] = '.';
        return parseResult;
    }
    while(tok) {
        strcpy(tokens[tokCnt++], tok);
        tok = strtok(NULL, delim);
        if(tokCnt == 5)
            break;
    }
    if(tok) // invalid field exists
        return parseResult; // returns NULL pointer

    parseResult = (ASM_SRC*) malloc(sizeof(ASM_SRC));
    if((bucket = bucketFound(tokens[0]))) { // if first token is the instruction
        strcpy(parseResult->inst, tokens[0]);
        i = 1;
    }
    else if ((bucket = bucketFound(tokens[1]))) { // if second token is the instruction
        strcpy(parseResult->label, tokens[0]);
        strcpy(parseResult->inst, tokens[1]);
        i = 2;
    }
    else if(!bucket) {
        free(parseResult);
        parseResult = NULL;
        return parseResult;
    }

    for(j = 0; i + j < tokCnt; j++) {
        strcpy(parseResult->operand[j], tokens[i + j]);
        if(i + j + 1 < tokCnt && strcmp(tokens[i + j + 1], ",")) { // no comma in between operands
            free(parseResult);
            parseResult = NULL;
            return parseResult;
        }
        j++;
    }

    return parseResult;
}

void symTableAdd(ASM_SRC source) {
    SYMBOL_ENTRY* cur = symTable;
    SYMBOL_ENTRY* newEntry = (SYMBOL_ENTRY*) malloc(sizeof(SYMBOL_ENTRY));
    strcpy(newEntry->symbol, source.label);
    newEntry->address = source.location;
    newEntry->next = NULL;

    if(!symTable) {
        symTable = newEntry;
        return;
    }
    while(cur->next)
        cur = cur->next;
    cur->next = newEntry;
}

void symTableFree() {
    SYMBOL_ENTRY *cur, *next;
    cur = symTable;
    while(cur) {
        next = cur->next;
        free(cur);
        cur = next;
    }
    symTable = NULL;
}
