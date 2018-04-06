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
 * File name: hash.c                                             *
 * File description: Tasks handling opcode and hash table.       *
 *                                                               *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "20161577.h"
#include "hash.h"

// Format 2 instruction but 1 operand
char exceptionFmt2[][OPCODE_LEN] = {
    "CLEAR",
    "SVC",
    "TIXR"

};
// Format 3 instruction but NO operand
char exceptionFmt3[][OPCODE_LEN] = {
    "RSUB"
};

void opCMD(INPUT_CMD ipcmd) {
    HASH_ENTRY* bucket = bucketFound(ipcmd.arg[0]);

    if(bucket) // target found!
        printf("opcode is %s\n", bucket->code);
    else { // target NOT found..
        puts("ERROR: mnemonic not found.");
        puts("Type \"opcodelist\" for list of available opcodes.");
    }
}

void oplistCMD() {
    HASH_ENTRY* bucket;
    int i;

    for(i = 0; i < HASH_SIZE; i++) {
        printf("%2d : ", i); // print table index
        bucket = hashTable[i]; // get front bucket
        while(bucket) {
            printf("[%s,%s]", bucket->inst, bucket->code);
            if((bucket = bucket->next)) // if there exists next bucket
                printf(" -> ");
        }
        puts("");
    }
}

void hashCreate() {
    FILE* fp = fopen("opcode.txt", "r"); // open file
    char cd[3], ins[10], md[4];
    HASH_ENTRY* bucket;

    if(!fp) {
        puts("ERROR: Unable to load \"opcode.txt\".");
        return;
    }

    while(fscanf(fp, "%s %s %s", cd, ins, md) == 3) {
        bucket = (HASH_ENTRY*) malloc(sizeof(HASH_ENTRY));
        strcpy(bucket->code, cd);
        strcpy(bucket->inst, ins);
        bucket->codeVal = hexToDec(cd);
        bucket->format = md[0] - '1';
        checkOperandCnt(bucket);
        bucket->next = NULL;

        hashAddBucket(hashFunction(bucket->inst), bucket);
    }

    if(fclose(fp)) {
        puts("WARNING: Error closing \"opcode.txt\".");
        return;
    }
}

void checkOperandCnt(HASH_ENTRY* bucket) {
    int i;
    switch(bucket->format) {
        case f1: // Format 1 instruction
            bucket->operandCnt = 0;
            break;
        case f2: // Format 2 instruction
            bucket->operandCnt = 2; // Default is 2 operands
            for(i = 0; i < 3; i++) // Exception check (CLEAR, SVC, TIXR)
                if(!strcmp(bucket->inst, exceptionFmt2[i])) {
                    bucket->operandCnt = 1;
                    break;
                }
            break;
        case f34: // Format 3 or 4 instruction
            bucket->operandCnt = 1; // Default is 1 operand
            for(i = 0; i < 1; i++) // Exception check (RSUB)
                if(!strcmp(bucket->inst, exceptionFmt3[i])) {
                    bucket->operandCnt = 0;
                    break;
                }
            break;
    }
}

int hashFunction(char* inst) {
    return abs( (int) inst[0] * 2 + abs(inst[0] + inst[1] + inst[2]) ) % HASH_SIZE;
}

void hashAddBucket(int hash, HASH_ENTRY* bucket) {
    HASH_ENTRY* cur = hashTable[hash];

    if(!cur) { // if empty hash table index
        hashTable[hash] = bucket;
        return;
    }
    while(cur->next)
        cur = cur->next; // go to the end of list
    cur->next = bucket;
}

void hashFree() {
    HASH_ENTRY* cur;
    HASH_ENTRY* nex;
    int i;
    for(i = 0; i < HASH_SIZE; i++) {
        cur = hashTable[i]; // front bucket in each hash table index
        while(cur) {
            nex = cur->next;
            free(cur);
            cur = nex;
        }
        hashTable[i] = NULL; // reset pointer to NULL
    }
}

HASH_ENTRY* bucketFound(char* inst) {
    HASH_ENTRY* bucket;

    bucket = hashTable[hashFunction(inst)]; // get front bucket from hash function
    while(bucket && strcmp(bucket->inst, inst)) // search till match or end
        bucket = bucket->next;
    return bucket;
}
