/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                               *
 *                    Sogang University                          *
 *         Department of Computer Science and Engineering        *
 *                                                               *
 * Subject name: System Programming                              *
 * Project title: [2] SIC/XE Machine - The Assembler             *
 *                                                               *
 * Author: Inho Kim                                              *
 * Student ID: 20161577                                          *
 *                                                               *
 * File name: shell.c                                            *
 * File description: Tasks processed in shell environment.       *
 *                                                               *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "20161577.h"
#include "shell.h"

HIST_NODE* histHead = NULL; // head of history linked list

void helpCMD() {
    printf("h[elp]\n"
            "d[ir]\n"
            "q[uit]\n"
            "hi[story]\n"
            "du[mp] [start, end]\n"
            "e[dit] address, value\n"
            "f[ill] start, end, value\n"
            "reset\n"
            "opcode mnemonic\n"
            "opcodelist\n"
            "assemble filename\n"
            "type filename\n"
            "symbol\n");
}

void dirCMD() {
    DIR* dir = opendir("."); // current directory
    char* entStr;
    char path[258] = "./"; // entry path string
    ENTRY* ent; // entry
    STBUF buf; // stat

    if(!dir) {
        puts("ERROR opening directory...");
        return;
    }
    ent = readdir(dir); // read entry
    while(ent) {
        path[2] = '\0'; // clear path string
        entStr = ent->d_name; // entry name
        stat(strcat(path, entStr), &buf);
        printf("%-s", entStr); // print entry name

        if(S_ISDIR(buf.st_mode)) // check for directory
            printf("/");
        else if(buf.st_mode & S_IXUSR) // check for exec file
            printf("*");

        ent = readdir(dir); // read next entry
        if(ent)
            puts("");
    }
    closedir(dir);
    puts("");
}

void quitCMD() {
    puts("Exiting SIC...");
    histFree();         // free history linked list
    hashFree();         // free hash table
    symTableFree();     // free SYMTAB
    parseListFree();    // free ASM parse list
    objListFree();      // free object code list
    modListFree();      // free modification record list
    exit(0);
}

void histCMD() {
    HIST_NODE* cur = histHead;
    int cnt = 1;

    while(cur) {
        printf("%-3d  ", cnt++);
        puts(cur->str); // print command in history
        cur = cur->next;
    }
}

void typeCMD(INPUT_CMD ipcmd) {
    FILE* fp = fopen(ipcmd.arg[0], "r");
    char c;

    if(!fp) {
        puts("ERROR: File not found.");
        return;
    }
    while((c = fgetc(fp)) != EOF)
        putchar(c);

    if(fclose(fp))
        puts("WARNING: Error closing file.");
}

void histAdd(char* str) {
    HIST_NODE* cur = histHead;
    HIST_NODE* newHist = (HIST_NODE*) malloc(sizeof(HIST_NODE));
    strcpy(newHist->str, str);
    newHist->next = NULL;

    if(!histHead) { // if history linked list is empty
        histHead = newHist;
        return;
    }
    while(cur->next)
        cur = cur->next;
    cur->next = newHist;
}

void histFree() {
    HIST_NODE* cur = histHead;
    HIST_NODE* nex;
    while(cur) {
        nex = cur->next;
        free(cur);
        cur = nex;
    }
    histHead = NULL;
}
