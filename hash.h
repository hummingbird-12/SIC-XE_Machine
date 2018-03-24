HASH_ENTRY* hashTable[HASH_SIZE]; // hash table pointer array

void opCMD(INPUT_CMD); // COMMAND: opcode
void oplistCMD();    // COMMAND: opcodelist

void hashCreate();						// create hash table
int hashFunction(char*);				// hash function
void hashAddBucket(int, HASH_ENTRY*); // add bucket to hash table
