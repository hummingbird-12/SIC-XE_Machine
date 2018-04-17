LIST histList;
LIST opCodeTable[HASH_SIZE];

void addToList(LIST*, void*);
void freeList(LIST*);

void opCodeTableFree();

void printList(LIST, void (void*));

void printHistory(void*);
void printOpList(void*);
