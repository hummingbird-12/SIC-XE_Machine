// hash

HASH_ENTRY* hash_table[HASH_SIZE];

void opCMD(USR_CMD);
void oplistCMD();

void hash_create();
int hash_function(char*);
void hash_add_bucket(int, HASH_ENTRY*);

