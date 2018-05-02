int progAddr;                   // program start address set by progaddr command

void pAddrCMD(INPUT_CMD);       // progaddr COMMAND
bool loaderCMD(INPUT_CMD);      // loader COMMAND

int linkLoaderPass1(FILE**);    // Pass1 of linking loader
int linkLoaderPass2(FILE**);    // Pass2 of linking loader

void fcloseObj(FILE**);         // close obj file pointers
bool searchCS(char*);           // search for control section in ESTAB
int searchES(char*);            // search for external symbol in ESTAB
