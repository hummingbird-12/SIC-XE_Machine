#include "20161577.h"
#include "memory.h"
#include "linkedList.h"
#include "execute.h"

bool bpCMD(INPUT_CMD ipcmd) {
    BREAK_PNT* newBP;

    if(!ipcmd.argCnt) {
        printf("\tbreakpoint\n");
        printf("\t----------\n");
        printList(breakPntList, printBreakPntList);
        return true;
    }
    if(!strcmp(ipcmd.arg[0], "clear")) {
        printf("\t[ok] clear all breakpoints\n");
        freeList(&breakPntList);
        return true;
    }
    if(searchBP(hexToDec(ipcmd.arg[0]))) {
        printf("\t[warning] breakpoint already at %04X\n", hexToDec(ipcmd.arg[0]));
        return false;
    }
    newBP = (BREAK_PNT*) malloc(sizeof(BREAK_PNT));
    newBP->address = hexToDec(ipcmd.arg[0]);
    addToList(&breakPntList, (void*) newBP);
    printf("\t[ok] create breakpoint %04X\n", hexToDec(ipcmd.arg[0]));
    return true;
}

bool searchBP(int address) {
    NODE* curBP = breakPntList;
    while(curBP) {
        if(((BREAK_PNT*)(curBP->data))->address == address)
            return true;
        curBP = curBP->next;
    }
    return false;
}

void runCMD() {
    static int curAddress = -1;
    static int lastBP = -1;
    int i;
    OBJ curObj;

    if(curAddress == -1)
        curAddress = execAddress;
    registers[PCreg] = curAddress;
    for(; curAddress < endAddress; ) {
        switch(mem[curAddress] / 0x10) {
            case 0:
            case 1:
            case 2:
            case 3:
            case 4:
            case 5:
            case 6:
            case 7:
            case 8:
            case 0xD:
            case 0xE:
                curObj.format = fmt3;
                if(mem[curAddress + 1] & 1)
                    curObj.format = fmt4;
                break;
            case 9:
            case 0xA:
            case 0xB:
                curObj.format = fmt2;
                break;
            case 0xC:
            case 0xF:
                curObj.format = fmt1;
                break;
            default:
                break;
        }
        for(i = curAddress; i < curAddress + curObj.format; i++)
            if(i > lastBP && searchBP(i)) {
                lastBP = i;
                dumpReg();
                printf("\n\tStop at checkpoint[%04X]\n", i);
                return;
            }
        registers[PCreg] += curObj.format;
        curAddress = registers[PCreg];
    }
    dumpReg();
    printf("\n\tEnd program.\n");
    curAddress = -1;
    lastBP = -1;
}

void dumpReg() {
    int i;
    char r[3];
    for(i = 0; i < 7; i++) {
        if(!(i % 2))
            printf("\t    ");
        memset(r, '\0', 3);
        switch(i) {
            case Areg:
                r[0] = 'A';
                break;
            case Xreg:
                r[0] = 'X';
                break;
            case Lreg:
                r[0] = 'L';
                break;
            case PCreg:
                strcpy(r, "PC");
                break;
            case Breg:
                r[0] = 'B';
                break;
            case Sreg:
                r[0] = 'S';
                break;
            case Treg:
                r[0] = 'T';
                break;
            default:
                break;
        }
        if(i == Xreg)
            printf("%-2s: %08X ", r, registers[i]);
        else
            printf("%-2s: %012X ", r, registers[i]);
        if(i % 2)
            puts("");
    }
}
