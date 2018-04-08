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
OBJ_CODE* objList = NULL;
MOD_RECORD* modList = NULL;

char directives[8][6] = {
    "NOTDR",
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

bool assembleCMD(INPUT_CMD);
ASM_SRC* parseASM(char*);
bool assemblerPass1(FILE*, int*, int*);
bool assemblerPass2(FILE*, FILE*, int, int);
void printLST(FILE*, ASM_SRC*, int, bool, bool);
void printOBJ(FILE*, ASM_SRC*, int, int);
void printOBJList();
void printModList();
void addOBJList(ASM_SRC*);
void addModList(int, int);
void setError(ASM_SRC*, ASM_ERROR); 
void printASMError(ASM_SRC*);
bool isRegister(char);
void symbolCMD();
void symTableAdd(char*, int);
SYMBOL_ENTRY* symTableSearch(char*);

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
        if(input.cmd < invFormat && input.cmd != assemble)
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
                if(assembleCMD(input))
                    histAdd(inp);
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

bool assembleCMD(INPUT_CMD ipcmd) {
    FILE *srcFile, *lstFile, *objFile;
    char lstName[CMD_LEN] = {'\0'}, objName[CMD_LEN] = {'\0'};
    int maxSrcLen = 0, progLen = 0;

    symTableFree();
    parseListFree();
    objListFree();
    modListFree();

    lstFile = objFile = NULL;
    srcFile = fopen(ipcmd.arg[0], "r");
    if(!srcFile) {
        puts("ERROR: File not found.");
        return false;
    }

    strncpy(lstName, ipcmd.arg[0], strlen(ipcmd.arg[0]) - 3);
    strncpy(objName, ipcmd.arg[0], strlen(ipcmd.arg[0]) - 3);
    strcat(lstName, "lst");
    strcat(objName, "obj");

    lstFile = fopen(lstName, "w");
    objFile = fopen(objName, "w");

    if(!lstFile || !objFile) {
        puts("ERROR: Problem while creating .list and .obj files.");
        if(lstFile) fclose(lstFile);
        if(objFile) fclose(objFile);
        return false;
    }

    if(!assemblerPass1(srcFile, &maxSrcLen, &progLen) || !assemblerPass2(lstFile, objFile, maxSrcLen, progLen)) {
        puts(".lst file and .obj file were not created.");
        remove(lstName);
        remove(objName);
        if(lstFile) fclose(lstFile);
        if(objFile) fclose(objFile);
       return false;
    }
    else
        printf("\toutput file : [%s], [%s]\n", lstName, objName);

    if(fclose(srcFile) || fclose(lstFile) || fclose(objFile))
        puts("WARNING: Error closing file.");
    return true;
}

bool assemblerPass1(FILE* srcFile, int* maxSrcLen, int* progLen) {
    ASM_SRC *curParse = NULL, *prevParse = NULL;
    char source[ASM_LEN] = {'\0'};
    int location = -1;
    int lineNum = 5;
    bool errorFlag = false;
    int mult;

    // process ASM source file line by line
    while(fgets(source, ASM_LEN, srcFile) != NULL) {
        // remove '\n' at the end of string
        if(source[strlen(source) - 1] == '\n')
            source[strlen(source) - 1] = '\0';

        curParse = parseASM(source); // get parsed line
        // START directive found
        if(curParse->direcName == START) {
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

        curParse->lineNum = lineNum;
        curParse->location = location;

        // if current line is NOT a comment
        if(curParse->type != COMMENT) {
            // remember longest ASM source line
            if(*maxSrcLen < strlen(source))
                *maxSrcLen = strlen(source);

            // if current line has symbol in label field
            if(curParse->hasLabel && curParse->direcName != START) {
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
                    curParse->byteSize = curParse->format;
                    break;
                case PSEUDO:
                    switch(curParse->direcName) {
                        case START:
                            break;
                        case END:
                        case BASE:
                            break;
                        case BYTE:
                            switch(curParse->operand[0][0]) {
                                case 'X':
                                    curParse->byteSize = (strlen(curParse->operand[0]) - 3 + 1) / 2;
                                    break;
                                case 'C':
                                    curParse->byteSize = strlen(curParse->operand[0]) - 3;
                                    break;
                                default:
                                    mult = INC_BYTE;
                                    for(curParse->byteSize = 1;
                                            mult <= atoi(curParse->operand[0]);
                                            mult *= INC_BYTE)
                                        curParse->byteSize++;
                                    break;
                            }
                            break;
                        case WORD:
                            curParse->byteSize = 3;
                            break;
                        case RESB:
                            curParse->byteSize = atoi(curParse->operand[0]);
                            break;
                        case RESW:
                            curParse->byteSize = atoi(curParse->operand[0]) * 3;
                            break;
                        default:
                            break;
                    }
                    break;
                case COMMENT:
                    break;
                default:
                    break;
            }
        }

        if(errorFlag)
            break;

        location += curParse->byteSize;
        lineNum += 5;
        prevParse = curParse;

        if(!strcmp(curParse->inst, "END"))
            break;
    }

    *progLen = location;

    if(errorFlag) {
        printASMError(curParse);
        symTableFree();
        return false;
    }
    return true;
}

bool assemblerPass2(FILE* lstFile, FILE* objFile, int maxSrcLen, int progLen) {
    ASM_SRC *curParse;
    SYMBOL_ENTRY *curSymbol;
    int pcReg, baseReg;
    bool errorFlag = false;
    bool locFlag, objFlag;
    bool nFlag, iFlag, xFlag, bFlag, pFlag, eFlag;
    int i, firstExec;

    curParse = parseList;
    baseReg = 0;
    pcReg = parseList->location;

    while(curParse) {
        locFlag = objFlag = true;
        curSymbol = NULL;

        if(curParse->next)
            pcReg = curParse->next->location;

        if(curParse == parseList && curParse->direcName != START) {
            printf("H%6s%06X%06X\n", " ", 0, progLen);
            firstExec = curParse->location;
        }

        switch(curParse->type) {
            case INST:
                nFlag = iFlag = true;
                xFlag = bFlag = pFlag = eFlag = false;
                curParse->objCode = bucketSearch(curParse->inst + (curParse->inst[0] == '+' ? 1 : 0) )->codeVal;
                switch(curParse->format) {
                    case format1:
                        break;
                    case format2:
                        for(i = 0; i < 2; i++) {
                            curParse->objCode *= 16;
                            switch(curParse->operand[i][0]) {
                                case 'A':
                                    curParse->objCode += 0;
                                    break;
                                case 'X':
                                    curParse->objCode += 1;
                                    break;
                                case 'L':
                                    curParse->objCode += 2;
                                    break;
                                case 'B':
                                    curParse->objCode += 3;
                                    break;
                                case 'S':
                                    curParse->objCode += 4;
                                    break;
                                case 'T':
                                    curParse->objCode += 5;
                                    break;
                                case 'F':
                                    curParse->objCode += 6;
                                    break;
                                default:
                                    break;
                            }
                        }
                        break;
                    case format3:
                        if(!curParse->operandCnt) {
                            curParse->objCode += 3;
                            curParse->objCode *= INC_BYTE * INC_BYTE;
                            break;
                        }
                        // indexing mode
                        if(curParse->indexing)
                            xFlag = true;
                        // immediate addressing
                        if(curParse->operand[0][0] == '#')
                            nFlag = false;
                        // indirect addressing
                        else if(curParse->operand[0][0] == '@')
                            iFlag = false;

                        // if there is a symbol in operand
                        if((curSymbol = symTableSearch(curParse->operand[0] + ((!nFlag || !iFlag) ? 1 : 0)))) {
                            // if pc relative addressing is possible
                            if(curSymbol->address - pcReg + MAX12DISP / 2 >= 0) {
                                curParse->objCode += (!nFlag ? 1 : (!iFlag ? 2  : 3));
                                curParse->objCode *= INC_HBYTE;
                                curParse->objCode += (xFlag ? 10 : 2);
                                curParse->objCode *= INC_BYTE * INC_HBYTE;
                                curParse->objCode += (curSymbol->address - pcReg) & 0xFFF;
                            }
                            // if base relative addressing is possible
                            else if(curSymbol->address - baseReg < MAX12DISP) {
                                curParse->objCode += (!nFlag ? 1 : (!iFlag ? 2  : 3));
                                curParse->objCode *= INC_HBYTE;
                                curParse->objCode += (xFlag ? 12 : 4);
                                curParse->objCode *= INC_BYTE * INC_HBYTE;
                                curParse->objCode += (curSymbol->address - baseReg) & 0xFFF;
                            }
                            // approach as SIC instruction
                            else if(curSymbol->address < MAX15ADDR){
                                curParse->objCode *= INC_HBYTE;
                                curParse->objCode += (xFlag ? 8 : 0);
                                curParse->objCode *= INC_BYTE * INC_HBYTE;
                                curParse->objCode += curSymbol->address;
                            }
                            // NOT able to make reference
                            else {
                                setError(curParse, INSTRUCTION);
                                return false;
                            }
                        }
                        // if there is a value in operand
                        else if(pcReg - atoi(curParse->operand[0] + 1) + MAX12DISP / 2 >= 0) {
                            curParse->objCode += (!nFlag ? 1 : 2);
                            curParse->objCode *= INC_HBYTE;
                            curParse->objCode += 0;
                            curParse->objCode *= INC_BYTE * INC_HBYTE;
                            curParse->objCode += atoi(curParse->operand[0] + 1);
                        }
                        break;
                    case format4:
                        eFlag = true;
                        curSymbol = symTableSearch(curParse->operand[0] + (!isalnum(curParse->operand[0][0]) ? 1 : 0));
                        if(curSymbol) {
                            curParse->objCode += 3;
                            curParse->objCode *= INC_HBYTE;
                            curParse->objCode += 1;
                            curParse->objCode *= INC_BYTE * INC_BYTE * INC_HBYTE;
                            curParse->objCode += curSymbol->address & 0xFFFFF;
                        }
                        else {
                            curParse->objCode += 1;
                            curParse->objCode *= INC_HBYTE;
                            curParse->objCode += 1;
                            curParse->objCode *= INC_BYTE * INC_BYTE * INC_HBYTE;
                            curParse->objCode += atoi(curParse->operand[0] + 1) & 0xFFFFF;
                        }
                        break;
                    default:
                        break;
                }
                break;
            case PSEUDO:
                switch(curParse->direcName) {
                    case START:
                        objFlag = false;
                        baseReg = curParse->location;
                        firstExec = curParse->location;
                        printOBJ(objFile, curParse, progLen, firstExec);
                        break;
                    case BASE:
                        locFlag = objFlag = false;
                        baseReg = symTableSearch(curParse->operand[0])->address;
                        break;
                    case END:
                        locFlag = objFlag = false;
                        printOBJ(objFile, curParse, progLen, firstExec);
                        break;
                    case BYTE:
                        switch(curParse->operand[0][0]) {
                            case 'C':
                                for(i = 2; curParse->operand[0][i] != '\''; i++) {
                                    curParse->objCode *= INC_BYTE;
                                    curParse->objCode += curParse->operand[0][i];
                                }
                                break;
                            case 'X':
                                curParse->objCode += strtol(curParse->operand[0] + 2, NULL, 16);
                                break;
                            default:
                                curParse->objCode = atoi(curParse->operand[0]);
                                break;
                        }
                        break;
                    case WORD:
                        curParse->objCode = atoi(curParse->operand[0]);
                        break;
                    case RESB:
                    case RESW:
                        objFlag = false;
                        break;
                    default:
                        break;
                }
                break;
            case COMMENT:
                locFlag = objFlag = false;
            default:
                break;
        }

        if(curParse->objCode)
            printOBJ(objFile, curParse, progLen, firstExec);
        printLST(lstFile, curParse, maxSrcLen, locFlag, objFlag);
        curParse = curParse->next;
    }

    if(errorFlag) {
        printASMError(curParse);
        symTableFree();
        return false;
    }

    return true;
}

void printLST(FILE* lstFile, ASM_SRC* parsedASM, int maxSrcLen, bool printLOC, bool printOBJ) {
    int i;
    fprintf(lstFile, "%4d\t", parsedASM->lineNum);
    (printLOC ? fprintf(lstFile, "%04X\t", parsedASM->location) : fprintf(lstFile, "\t"));
    for(i = 0; parsedASM->source[i]; i++)
        fprintf(lstFile, "%c", parsedASM->source[i]);
    for(;i < maxSrcLen; i++)
        fprintf(lstFile, " ");
    if(printOBJ) {
        fprintf(lstFile, "\t\t");
        switch(parsedASM->byteSize) {
            case 1:
                fprintf(lstFile, "%02X", parsedASM->objCode);
                break;
            case 2:
                fprintf(lstFile, "%04X", parsedASM->objCode);
                break;
            case 3:
                fprintf(lstFile, "%06X", parsedASM->objCode);
                break;
            case 4:
                fprintf(lstFile, "%08X", parsedASM->objCode);
                break;
            default:
                break;
        }
    }
    fprintf(lstFile, "\n");
}

void printOBJ(FILE* objFile, ASM_SRC* parsedASM, int progLen, int firstExec) {
    static int lineLen = 0;
    if(parsedASM->direcName == START) {
        printf("H %-6s %06X %06X\n", parsedASM->label, parsedASM->location, progLen);
        return;
    }
    if(parsedASM->direcName == END) {
        printModList();
        printf("E %06X\n", firstExec);
        return;
    }
    if(parsedASM->format == format4 && symTableSearch(parsedASM->operand[0] + (!isalnum(parsedASM->operand[0][0]) ? 1 : 0)))
        addModList(parsedASM->location - firstExec + 1, 5);
    if(lineLen + parsedASM->byteSize > 0x1E) {
        printf("T %06X %02X ", objList ? objList->location : parsedASM->location, lineLen);
        printOBJList();
        objListFree();
        lineLen = 0;
    }

    addOBJList(parsedASM);
    lineLen += parsedASM->byteSize;
    if(parsedASM->direcName == BYTE || parsedASM->direcName == WORD) {
        printf("T %06X %02X ", objList ? objList->location : parsedASM->location, lineLen);
        printOBJList();
        objListFree();
        lineLen = 0;
    }
}

void printOBJList() {
    OBJ_CODE* cur = objList;
    while(cur) {
        switch(cur->byteSize) {
            case 1:
                printf("%02X", cur->objCode);
                break;
            case 2:
                printf("%04X", cur->objCode);
                break;
            case 3:
                printf("%06X", cur->objCode);
                break;
            case 4:
                printf("%08X", cur->objCode);
                break;
            default:
                break;
        }
        printf(" ");
        cur = cur->next;
    }
    puts("");
}

void printModList() {
    MOD_RECORD* cur = modList;
    while(cur) {
        printf("M %06X %02d\n", cur->location, cur->lenHB);
        cur = cur->next;
    }
}

void addOBJList(ASM_SRC* parsedASM) {
    OBJ_CODE *newOBJ, *cur = objList;
    newOBJ = (OBJ_CODE*) malloc(sizeof(OBJ_CODE));
    newOBJ->objCode = parsedASM->objCode;
    newOBJ->location = parsedASM->location;
    newOBJ->byteSize = parsedASM->byteSize;
    newOBJ->next = NULL;
    if(!objList) {
        objList = newOBJ;
        return;
    }
    while(cur->next)
        cur = cur->next;
    cur->next = newOBJ;
}

void addModList(int location, int length) {
    MOD_RECORD *newRec, *cur = modList;
    newRec = (MOD_RECORD*) malloc(sizeof(MOD_RECORD));
    newRec->lenHB = length;
    newRec->location = location;
    newRec->next = NULL;
    if(!modList) {
        modList = newRec;
        return;
    }
    while(cur->next)
        cur = cur->next;
    cur->next = newRec;
}

void parseListFree() {
    ASM_SRC *cur, *next;
    cur = parseList;
    while(cur) {
        next = cur->next;
        free(cur);
        cur = next;
    }
    parseList = NULL;
}

void objListFree() {
    OBJ_CODE *cur, *next;
    cur = objList;
    while(cur) {
        next = cur->next;
        free(cur);
        cur = next;
    }
    objList = NULL;
}

void modListFree() {
    MOD_RECORD *cur, *next;
    cur = modList;
    while(cur) {
        next = cur->next;
        free(cur);
        cur = next;
    }
    modList = NULL;
}

void printASMError(ASM_SRC* parsedASM) {
    printf("ERROR: Invalid assembly source.\n");
    printf("[Line %d] Error in ", parsedASM->lineNum);
    switch(parsedASM->errorCode) {
        case SYMBOL:
            puts("symbol field:");
            break;
        case INSTRUCTION:
            puts("instruction field:");
            break;
        case OPERAND:
            puts("operand field:");
            break;
        default:
            break;
    }
    puts(parsedASM->source);
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
    char *tok;
    int i, j;
    ASM_SRC* parseResult;
    HASH_ENTRY* bucket = NULL;

    parseResult = (ASM_SRC*) malloc(sizeof(ASM_SRC));
    strcpy(parseResult->source, source);
    parseResult->hasLabel = false;
    parseResult->type = INST;
    parseResult->errorCode = OK;
    parseResult->indexing = false;
    parseResult->operandCnt = 0;
    parseResult->byteSize = 0;
    parseResult->objCode = 0;
    parseResult->direcName = NOTDR;
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
    for(i = 1; i < 8; i++)
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
                // missing instruction
                if(!tok) {
                    setError(parseResult, INSTRUCTION);
                    return parseResult;
                }

                bucket = bucketSearch(tok + (tok[0] == '+' ? 1 : 0) );

                if(!strcmp(tok, ",")) { // comma should not be present
                    setError(parseResult, INSTRUCTION);
                    return parseResult;
                }
            }
        }

        // look for directives
        for(i = 1; i < 8; i++)
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
                if(strlen(parseResult->operand[1]))
                    parseResult->indexing = true;
                break;
            default:
                break;
        }
    }
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

SYMBOL_ENTRY* symTableSearch(char* symbol) {
    SYMBOL_ENTRY* cur = symTable;
    while(cur) {
        if(!strcmp(symbol, cur->symbol))
            return cur;
        cur = cur->next;
    }
    return NULL;
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
