int progAddr;

void pAddrCMD(INPUT_CMD);
bool loaderCMD(INPUT_CMD);

int linkLoaderPass1(FILE**);
int linkLoaderPass2(FILE**);

void fcloseObj(FILE**);
bool searchCS(char*);
bool searchES(char*, CNT_SEC*);
