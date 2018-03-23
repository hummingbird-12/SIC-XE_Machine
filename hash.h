HASH_ENTRY* hash_table[HASH_SIZE]; // hash table pointer array

void opCMD(USR_CMD); // COMMAND: opcode
void oplistCMD();    // COMMAND: opcodelist

void hash_create();						// create hash table
int hash_function(char*);				// hash function
void hash_add_bucket(int, HASH_ENTRY*); // add bucket to hash table
