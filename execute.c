#include "20161577.h"
#include "linkedList.h"
#include "execute.h"

void bpCMD(INPUT_CMD ipcmd) {
    BREAK_PNT* newBP;

    if(!ipcmd.argCnt) {
        printf("\tbreakpoint\n");
        printf("\t----------\n");
        printList(breakPntList, printBreakPntList);
        return;
    }
    if(!strcmp(ipcmd.arg[0], "clear")) {
        printf("\t[ok] clear all breakpoints\n");
        freeList(&breakPntList);
        return;
    }
    newBP = (BREAK_PNT*) malloc(sizeof(BREAK_PNT));
    newBP->address = hexToDec(ipcmd.arg[0]);
    addToList(&breakPntList, (void*) newBP);
    printf("\t[ok] create breakpoint %04X\n", hexToDec(ipcmd.arg[0]));
}
