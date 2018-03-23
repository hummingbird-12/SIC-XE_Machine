#include "main.h"

void opCMD(USR_CMD uscmd) {
	HASH_ENTRY* bucket;
	bucket = hash_table[hash_function(uscmd.arg[0])];
	while(bucket && strcmp(bucket->inst, uscmd.arg[0]))
		bucket = bucket->next;
	if(bucket)
		printf("opcode is %s\n", bucket->code);
	else {
		puts("ERROR: opcode not found.");
		puts("Type \"opcodelist\" for list of available opcodes.");
	}
}

void oplistCMD() {
	HASH_ENTRY* bucket;
	int i;

	for(i = 0; i < HASH_SIZE; i++) {
		printf("%2d : ", i);
		bucket = hash_table[i];
		while(bucket) {
			printf("[%s,%s]", bucket->inst, bucket->code);
			if((bucket = bucket->next))
				printf(" -> ");
		}
		puts("");
	}
}

void hash_create() {
	FILE* fp = fopen("opcode.txt", "r");
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
		
		hash_add_bucket(hash_function(bucket->inst), bucket);
	}

	if(fclose(fp)) {
		puts("WARNING: Error closing \"opcode.txt\".");
		return;
	}
}

int hash_function(char* inst) {
	return abs( (int) inst[0] * 2 + abs(inst[0] + inst[1] + inst[2]) ) % HASH_SIZE;
}

void hash_add_bucket(int hash, HASH_ENTRY* bucket) {
	HASH_ENTRY* cur = hash_table[hash];

	if(!cur) {
		hash_table[hash] = bucket;
		return;
	}
	while(cur->next)
		cur = cur->next;
	cur->next = bucket;
}
