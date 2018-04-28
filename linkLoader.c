#include "20161577.h"
#include "linkedList.h"
#include "linkLoader.h"

void pAddrCMD(INPUT_CMD ipcmd) {
    progAddr = hexToDec(ipcmd.arg[0]);
}

void loaderCMD(INPUT_CMD ipcmd) {
    FILE* objFptr[3];
    int i;

    objFptr[0] = objFptr[1] = objFptr[2] = NULL;
    extSymTableFree();

    for(i = 0; i < ipcmd.argCnt; i++) {
        if(strcmp(ipcmd.arg[i] + strlen(ipcmd.arg[i]) - 4, ".obj")) {
            puts("ERROR: Non-object file selected.");
            fcloseObj(objFptr);
            return;
        }
        if(!(objFptr[i] = fopen(ipcmd.arg[i], "r"))) {
            printf("ERROR: %s not found.\n", ipcmd.arg[i]);
            fcloseObj(objFptr);
            return;
        }
    }
    fcloseObj(objFptr);
    printList(extSymTable, printCntSecTable);
}

void linkLoaderPass1(FILE** objFptr) {
}

void linkLoaderPass2(FILE** objFptr) {
}

void fcloseObj(FILE** objFptr) {
    int i;
    for(i = 0; i < 3; i++) {
        if(objFptr[i])
            fclose(objFptr[i]);
        objFptr[i] = NULL;
    }
}
