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
ASM_SRC* parseList = NULL;
char directives[7][6] = {
    "START",
    "END",
    "BASE",
    "BYTE",
    "WORD",
    "RESB",
    "RESW"
};
char registers[7] = {
    'A', 'X', 'L', 'B', 'S', 'T', 'F'
};

void assembleCMD(INPUT_CMD);
ASM_SRC* parseASM(char*);
bool assemblerPass1(FILE*);
void assemblerPass2(FILE*, FILE*, FILE*);
void setError(ASM_SRC*, ASM_ERROR); 
void printASMError(int, ASM_ERROR);
bool isRegister(char);
void symbolCMD();
void symTableAdd(char*, int);
bool symTableSearch(char*);

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
    FILE *srcFile, *lstFile, *objFile;
    char lstName[CMD_LEN], objName[CMD_LEN];

    symTableFree();

    lstFile = objFile = NULL;
    srcFile = fopen(ipcmd.arg[0], "r");
    if(!srcFile) {
        puts("ERROR: File not found.");
        return;
    }

    strncpy(lstName, ipcmd.arg[0], strlen(ipcmd.arg[0]) - 3);
    strcat(lstName, "lst");
    strncpy(objName, ipcmd.arg[0], strlen(ipcmd.arg[0]) - 3);
    strcat(objName, "obj");

    //lstFile = fopen(lstName, "w");
    //objFile = fopen(objName, "w");

    if(!lstFile || !objFile) {
        //puts("ERROR: Problem while creating .list and .obj files.");
        if(lstFile) fclose(lstFile);
        if(objFile) fclose(objFile);
        //return;
    }

    if(!assemblerPass1(srcFile))
        return;
    assemblerPass2(srcFile, NULL, NULL);

    printf("\toutput file : [%s], [%s]\n", lstName, objName);

    //if(fclose(fp) || fclose(lstFile) || fclose(objFile))
    //puts("WARNING: Error closing file.");
}

bool assemblerPass1(FILE* src) {
    ASM_SRC *curParse, *prevParse;
    char source[ASM_LEN];
    int location = -1;
    int lineNum = 5;
    bool errorFlag = false;

    // process ASM source file line by line
    while(fgets(source, ASM_LEN, src) != NULL) {
        curParse = parseASM(source); // get parsed line
        // START directive found
        if(!strcmp(curParse->inst, "START")) {
            // if START directive appears for the second time
            if(location != -1) {
                puts("ERROR: START directive appeared twice.");
                errorFlag = true;
                break;
            }
            // initialize LOCCTR
            location = hexToDec(curParse->operand[0]);
            curParse->location = location;
            parseList = curParse;
            prevParse = NULL;
        }
        // no START directive, set start address as 0
        if(location == -1) {
            location = 0;
            parseList = curParse;
            prevParse = NULL;
            curParse->location = location;
        }
        else if(prevParse)
            prevParse->next = curParse; // link node to list

        printf("%5d\t", lineNum);

        curParse->location = location;

        // if current line is NOT a comment
        if(curParse->type != COMMENT) {
            // if current line has symbol in label field
            if(curParse->hasLabel) {
                // if symbol is already in SYMTAB
                if(symTableSearch(curParse->label)) {
                    setError(curParse, SYMBOL);
                    errorFlag = true;
                    break;
                }
                // new symbol found
                else
                    symTableAdd(curParse->label, location);
            }
            switch(curParse->type) {
                case ERROR:
                    errorFlag = true;
                    break;
                case INST:
                    printf("%04X\t", location);
                    location += curParse->format;
                    break;
                case PSEUDO:
                    switch(curParse->direcName) {
                        case START:
                            printf("%04X\t", location);
                            break;
                        case END:
                        case BASE:
                            printf("\t");
                            break;
                        case BYTE:
                            printf("%04X\t", location);
                            if(curParse->operand[0][0] == 'X')
                                location += (strlen(curParse->operand[0]) - 3 + 1) / 2;
                            else if(curParse->operand[0][0] == 'C')
                                location += strlen(curParse->operand[0]) - 3;
                            break;
                        case WORD:
                            printf("%04X\t", location);
                            location += 3;
                            break;
                        case RESB:
                            printf("%04X\t", location);
                            location += atoi(curParse->operand[0]);
                            break;
                        case RESW:
                            printf("%04X\t", location);
                            location += atoi(curParse->operand[0]) * 3;
                            break;
                        default:
                            break;
                    }
                    break;
                case COMMENT:
                    printf("\t");
                    break;
                default:
                    break;
            }

        }
        // if line is a comment
        else
            printf("\t");

        if(errorFlag)
            break;

        if(source[strlen(source) - 1] == '\n')
            source[strlen(source) - 1] = '\0';
        puts(source);
        lineNum += 5;
        prevParse = curParse;
        if(!strcmp(curParse->inst, "END"))
            break;
    }

    if(errorFlag) {
        printASMError(lineNum, curParse->errorCode);
        if(source[strlen(source) - 1] == '\n')
            source[strlen(source) - 1] = '\0';
        puts(source);
        symTableFree();
        return false;
    }
    return true;
}

void assemblerPass2(FILE* srcFile, FILE* lstFile, FILE* objFile) {
}

void printASMError(int lineNum, ASM_ERROR errorCode) {
    printf("ERROR: Invalid assembly source:\n");
    printf("[Line %d] Error in ", lineNum);
    switch(errorCode) {
        case SYMBOL:
            puts("symbol field.");
            break;
        case INSTRUCTION:
            puts("instruction field.");
            break;
        case OPERAND:
            puts("operand field.");
            break;
        default:
            break;
    }
}

void symbolCMD() {
    SYMBOL_ENTRY* cur = symTable;
    if(!symTable) {
        puts("Symbol table is empty.");
        return;
    }
    while(cur) {
        printf("\t%s\t%04X\n", cur->symbol, cur->address);
        cur = cur->next;
    }
}

ASM_SRC* parseASM(char* source) {
    char delim[] = " \t\n";
    char tmp[ASM_LEN] = {'\0'};
    //char opr[ASM_LEN] = {'\0'};
    char *tok;
    int i, j;
    ASM_SRC* parseResult;
    HASH_ENTRY* bucket;

    parseResult = (ASM_SRC*) malloc(sizeof(ASM_SRC));
    parseResult->hasLabel = false;
    parseResult->type = INST;
    parseResult->errorCode = OK;
    parseResult->indexing = false;
    parseResult->operandCnt = 0;
    parseResult->next = NULL;

    j = 0;
    for(i = 0; source[i]; i++) {
        if(source[i] == ',') {
            strcpy(tmp + j, " , ");
            j += 3;
        }
        else
            tmp[j++] = source[i];
    }

    tok = strtok(tmp, delim);

    if(tok[0] == '.') { // comment found
        parseResult->type = COMMENT;
        parseResult->format = NONE;
        return parseResult;
    }
    else if(!strcmp(tok, ",")) { // comma should not be present
        setError(parseResult, INSTRUCTION);
        return parseResult;
    }

    // look for directives
    for(i = 0; i < 7; i++)
        if(!strcmp(tok, directives[i])) { // pseudo instruction found
            parseResult->type = PSEUDO;
            parseResult->format = NONE;
            parseResult->direcName = i;
            strcpy(parseResult->inst, tok);
            break;
        }

    if(parseResult->type != PSEUDO) { // if not a pseudo instruction
        bucket = bucketSearch(tok + (tok[0] == '+' ? 1 : 0) );

        // if first field is a label
        if(!bucket) {
            if(symTableSearch(tok)) { // symbol already exists
                setError(parseResult, SYMBOL);
                return parseResult;
            }
            else { // new symbol found, add to SYMTAB and move on
                parseResult->hasLabel = true;
                strcpy(parseResult->label, tok);
                tok = strtok(NULL, delim);
                bucket = bucketSearch(tok + (tok[0] == '+' ? 1 : 0) );

                if(!strcmp(tok, ",")) { // comma should not be present
                    setError(parseResult, INSTRUCTION);
                    return parseResult;
                }
            }
        }

        // look for directives
        for(i = 0; i < 7; i++)
            if(!strcmp(tok, directives[i])) { // pseudo instruction found
                parseResult->type = PSEUDO;
                strcpy(parseResult->inst, tok);
                parseResult->format = NONE;
                parseResult->direcName = i;
                break;
            }

        // if field is an instruction
        if(bucket) {
            parseResult->type = INST;
            parseResult->format = bucket->format + 1; // get format from hash table
            if(tok[0] == '+') {
                // not a format 3/4 instruction
                if(bucket->format != f34) {
                    setError(parseResult, INSTRUCTION);
                    return parseResult;
                }
                // found a format 4 instruction
                parseResult->format = format4;
            }
            strcpy(parseResult->inst, tok);
        }
        // invalid instruction
        else if(parseResult->type != PSEUDO){
            setError(parseResult, INSTRUCTION);
            return parseResult;
        }
    }

    // tokenize the operand field part
    tok = strtok(NULL, delim); // first operand
    if(bucket && ((bucket->operandCnt && !tok) || (!bucket->operandCnt && tok))) {
        setError(parseResult, OPERAND);
        return parseResult;
    }
    else if(bucket && !bucket->operandCnt)
        return parseResult;
    if(tok && !strcmp(tok, ",")) { // comma should not be present
        setError(parseResult, OPERAND);
        return parseResult;
    }
    strcpy(parseResult->operand[0], tok);
    parseResult->operandCnt = 1;
    tok = strtok(NULL, delim); // expected a comma if there's second operand
    if(tok && strcmp(tok, ",")) { // comma should be present
        setError(parseResult, OPERAND);
        return parseResult;
    }
    if(tok) {
        tok = strtok(NULL, delim); // second operand
        if(!tok || !strcmp(tok, ",")) {
            setError(parseResult, OPERAND);
            return parseResult;
        }
        strcpy(parseResult->operand[1], tok);
        parseResult->operandCnt = 2;
    }

    // check operand format for instructions
    if(parseResult->type == INST) {
        switch(parseResult->format) {
            case format2:
                // first operand must be a Register
                if(strlen(parseResult->operand[0]) != 1 || !isRegister(parseResult->operand[0][0])) {
                    setError(parseResult, OPERAND);
                    return parseResult;
                }
                // second operand for SHIFTL/R must be a number
                if(!strncmp(parseResult->inst, "SHIFT", 5)) {
                    for(i = 0; parseResult->operand[1][i]; i++)
                        if(!isdigit(parseResult->operand[1][i])) {
                            setError(parseResult, OPERAND);
                            return parseResult;
                        }
                }
                // second operand must be a Register
                else if(bucket->operandCnt != 1){
                    if(strlen(parseResult->operand[1]) != 1 || !isRegister(parseResult->operand[1][0])) {
                        setError(parseResult, OPERAND);
                        return parseResult;
                    }
                }
                break;
            case format3:
            case format4:
                // one non-register operand or second operand is not X (indexing)
                if((strlen(parseResult->operand[0]) == 1 && isRegister(parseResult->operand[0][0])) || (strlen(parseResult->operand[1]) && strcmp(parseResult->operand[1], "X")) ) {
                    setError(parseResult, OPERAND);
                    return parseResult;
                }
                parseResult->indexing = true;
                break;
            default:
                break;
        }
    }
    /*
    // check operand format for pseudo-instructions
    else if(parseResult->type == PSEUDO) {
// pseudo-instructions must have only one operand
if(parseResult->operandCnt != 1) {
setError(parseResult, OPERAND);
return parseResult;
}
switch(parseResult->operand[0][0]) {
case '#':
j = -1;
case '@':
if(symTableSearch(parseResult->operand[0] + 1))
break;
for(i = 1; parseResult->operand[0][i]; i++)
if(!(j == -1 ? isdigit(parseResult->operand[0][i]) : isxdigit(parseResult->operand[0][i]))) {
setError(parseResult, OPERAND);
return parseResult;
}
break;
case 'X':
j = -2;
case 'C':
for(i = 2; parseResult->operand[0][i + 1]; i++)
if(!(j == -2 ? isxdigit(parseResult->operand[0][i]) : isdigit(parseResult->operand[0][i]))) {
setError(parseResult, OPERAND);
return parseResult;
}
if(parseResult->operand[0][0] != '\'' || parseResult->operand[0][i] != '\'') {
setError(parseResult, OPERAND);
return parseResult;
}
break;
default:
if(!symTableSearch(parseResult->operand[0]))
for(i = 0; parseResult->operand[0][i]; i++)
if(!isxdigit(parseResult->operand[0][i])) {
setError(parseResult, OPERAND);
return parseResult;
}
break;
}
}
*/
return parseResult;
}

bool isRegister(char reg) {
    int i;
    for(i = 0; i < 7; i++)
        if(registers[i] == reg)
            return true;
    return false;
}

void setError(ASM_SRC* parsedResult, ASM_ERROR error) {
    parsedResult->type = ERROR;
    parsedResult->format = NONE;
    parsedResult->errorCode = error;
}

void symTableAdd(char* symbol, int address) {
    SYMBOL_ENTRY* cur = symTable;
    SYMBOL_ENTRY* newEntry = (SYMBOL_ENTRY*) malloc(sizeof(SYMBOL_ENTRY));
    strcpy(newEntry->symbol, symbol);
    newEntry->address = address;
    newEntry->next = NULL;

    if(!symTable) {
        symTable = newEntry;
        return;
    }
    if(strcmp(symTable->symbol, symbol) < 0) {
        newEntry->next = symTable;
        symTable = newEntry;
        return;
    }
    while(cur->next && strcmp(cur->next->symbol, symbol) > 0)
        cur = cur->next;
    newEntry->next = cur->next;
    cur->next = newEntry;
}

bool symTableSearch(char* symbol) {
    SYMBOL_ENTRY* cur = symTable;
    while(cur) {
        if(!strcmp(symbol, cur->symbol))
            return true;
        cur = cur->next;
    }
    return false;
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
