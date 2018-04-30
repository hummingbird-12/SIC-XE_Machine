#include "20161577.h"
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
    while(curBP)
        if(((BREAK_PNT*)(curBP->data))->address == address)
            return true;
    return false;
}
