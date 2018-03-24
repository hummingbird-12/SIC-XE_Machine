#include "20161577.h"
#include "hash.h"

void opCMD(INPUT_CMD ipcmd) {
	HASH_ENTRY* bucket;
	bucket = hashTable[hashFunction(ipcmd.arg[0])]; // get front bucket from hash function
	while(bucket && strcmp(bucket->inst, ipcmd.arg[0])) // search till match or end
		bucket = bucket->next;
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
		bucket = malloc(sizeof(HASH_ENTRY));
		strcpy(bucket->code, cd);
		strcpy(bucket->inst, ins);
		bucket->codeVal = hexToDec(cd);
		bucket->mode = md[0] - '1';
		bucket->next = NULL;
		
		hashAddBucket(hashFunction(bucket->inst), bucket);
	}

	if(fclose(fp)) {
		puts("WARNING: Error closing \"opcode.txt\".");
		return;
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
