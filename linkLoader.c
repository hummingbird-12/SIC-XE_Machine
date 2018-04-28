#include "20161577.h"
#include "linkedList.h"
#include "linkLoader.h"

void pAddrCMD(INPUT_CMD ipcmd) {
    progAddr = strtol(ipcmd.arg[0], NULL, 16);
}

bool loaderCMD(INPUT_CMD ipcmd) {
    FILE* objFptr[3];
    int progLen;
    int i;

    objFptr[0] = objFptr[1] = objFptr[2] = NULL;
    extSymTableFree();

    for(i = 0; i < ipcmd.argCnt; i++) {
        if(strcmp(ipcmd.arg[i] + strlen(ipcmd.arg[i]) - 4, ".obj")) {
            puts("ERROR: Non-object file selected.");
            fcloseObj(objFptr);
            return false;
        }
        if(!(objFptr[i] = fopen(ipcmd.arg[i], "r"))) {
            printf("ERROR: %s not found.\n", ipcmd.arg[i]);
            fcloseObj(objFptr);
            return false;
        }
    }
    progLen = linkLoaderPass1(objFptr);
    if(!progLen)
        return false;
    printList(extSymTable, printCntSecTable);
    printf("---------------------------------------------------------\n");
    printf("\t\t\t\tTotal length\t%04X\n", progLen);
    fcloseObj(objFptr);
    return true;
}

int linkLoaderPass1(FILE** objFptr) {
    int i = 0, j;
    int accLen = 0;
    char record[101];
    char csName[CS_LEN], esName[CS_LEN], addr[CS_LEN];
    CNT_SEC* newCntSec = NULL;
    EXT_SYMBOL* newExtSym = NULL;

    newCntSec = (CNT_SEC*) malloc(sizeof(CNT_SEC));
    newCntSec->stAddress = accLen = progAddr; // start address of first control section

    while(objFptr[i]) {
        fgets(record, 101, objFptr[i]); // header record
        if(record[strlen(record) - 1] == '\n')
            record[strlen(record) - 1] = '\0';
        newCntSec->length = hexToDec(record + strlen(record) - 6); // control section length
        strncpy(csName, record + 1, CS_LEN - 1); // get control section name
        if(searchCS(csName)) { // same name for multiple control sections
            puts("ERROR: overlapping control section name.");
            extSymTableFree();
            free(newCntSec);
            return 0;
        }
        else { // add new control section to ESTAB
            strcpy(newCntSec->csName, csName);
            addToList(&extSymTable, (void*) newCntSec);
        }

        fgets(record, 101, objFptr[i]); // read next record
        if(record[strlen(record) - 1] == '\n')
            record[strlen(record) - 1] = '\0';
        while(record[0] != 'E') {
            if(record[0] == 'D') { // if a D record is found
                for(j = 1; record[j]; j += 12) {
                    strncpy(esName, record + j, CS_LEN - 1); // get symbol name
                    strncpy(addr, record + j + 6, CS_LEN - 1); // get symbol address
                    if(searchES(esName, newCntSec)) { // same name for multiple symbols in single control section
                        puts("ERROR: overlapping symbol name.");
                        extSymTableFree();
                        return 0;
                    }
                    newExtSym = (EXT_SYMBOL*) malloc(sizeof(EXT_SYMBOL));
                    strncpy(newExtSym->symName, esName, CS_LEN - 1);
                    newExtSym->address = hexToDec(addr) + accLen;
                    addToList(&(newCntSec->extSym), (void*) newExtSym);
                }
            }
            fgets(record, 101, objFptr[i]); // read next record
            if(record[strlen(record) - 1] == '\n')
                record[strlen(record) - 1] = '\0';
        }

        accLen += newCntSec->length;
        i++;
        if(i == CS_MAX)
            break;
        if(objFptr[i]) { // next control section exists
            newCntSec = (CNT_SEC*) malloc(sizeof(CNT_SEC));
            newCntSec->stAddress = accLen;
        }
    }
    return accLen - progAddr;
}

bool linkLoaderPass2(FILE** objFptr) {
    return true;
}

void fcloseObj(FILE** objFptr) {
    int i;
    for(i = 0; i < 3; i++) {
        if(objFptr[i])
            fclose(objFptr[i]);
        objFptr[i] = NULL;
    }
}

bool searchCS(char* csName) {
    NODE* cur = extSymTable;
    CNT_SEC* data;
    while(cur) {
        data = (CNT_SEC*) cur->data;
        if(!strcmp(data->csName, csName))
            return true;
        cur = cur->next;
    }
    return false;
}

bool searchES(char* symName, CNT_SEC* curCS) {
    NODE* cur = curCS->extSym;
    EXT_SYMBOL* data;
    while(cur) {
        data = (EXT_SYMBOL*) cur->data;
        if(!strcmp(data->symName, symName))
            return true;
        cur = cur->next;
    }
    return false;
}
