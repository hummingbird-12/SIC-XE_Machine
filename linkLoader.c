#include "20161577.h"
#include "linkedList.h"
#include "linkLoader.h"

void pAddrCMD(INPUT_CMD ipcmd) {
    progAddr = hexToDec(ipcmd.arg[0]);
}
