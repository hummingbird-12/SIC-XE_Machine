LIST histList;
LIST opCodeTable[HASH_SIZE];
LIST extSymTable;

void addToList(LIST*, void*);

void freeList(LIST*);
void opCodeTableFree();
void extSymTableFree();

void printList(LIST, void (void*));
void printHistory(void*);
void printOpList(void*);
void printCntSecTable(void*);
void printExtSym(void*);
