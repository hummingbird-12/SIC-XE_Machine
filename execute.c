#include "20161577.h"
#include "memory.h"
#include "linkedList.h"
#include "execute.h"

bool bpCMD(INPUT_CMD ipcmd) {
    BREAK_PNT* newBP;

    if(!ipcmd.argCnt) {
        printf("\tbreakpoint\n");
        printf("\t----------\n");
        printList(breakPntList, printBreakPntList);
        return true;
    }
    if(!strcmp(ipcmd.arg[0], "clear")) {
        printf("\t[ok] clear all breakpoints\n");
        freeList(&breakPntList);
        return true;
    }
    if(searchBP(hexToDec(ipcmd.arg[0]))) {
        printf("\t[warning] breakpoint already at %04X\n", hexToDec(ipcmd.arg[0]));
        return false;
    }
    newBP = (BREAK_PNT*) malloc(sizeof(BREAK_PNT));
    newBP->address = hexToDec(ipcmd.arg[0]);
    addToList(&breakPntList, (void*) newBP);
    printf("\t[ok] create breakpoint %04X\n", hexToDec(ipcmd.arg[0]));
    return true;
}

bool searchBP(int address) {
    NODE* curBP = breakPntList;
    while(curBP) {
        if(((BREAK_PNT*)(curBP->data))->address == address)
            return true;
        curBP = curBP->next;
    }
    return false;
}

void runCMD() {
    static int curAddress = -1;
    static int lastBP = -1;
    int i;
    int targetVal;
    OBJ curObj;

    CCstatus = 4;
    if(curAddress == -1)
        curAddress = execAddress; // start from beginning
    registers[PCreg] = curAddress; // set PC register
    for(; curAddress < endAddress; ) {
        curObj.mnemonic = mem[curAddress] & 0xFC; // decode opcode

        // check instruction format
        switch(mem[curAddress] / 0x10) {
            case 0:
            case 1:
            case 2:
            case 3:
            case 4:
            case 5:
            case 6:
            case 7:
            case 8:
            case 0xD:
            case 0xE:
                curObj.format = fmt3; // format 3
                if(mem[curAddress + 1] & 0x10) // e bit is set
                    curObj.format = fmt4; // format 4
                break;
            case 9:
            case 0xA:
            case 0xB:
                curObj.mnemonic += mem[curAddress] & 0x3;
                curObj.format = fmt2; // format 2
                break;
            case 0xC:
            case 0xF:
                curObj.mnemonic += mem[curAddress] & 0x3;
                curObj.format = fmt1; // format 1
                break;
            default:
                break;
        }
        if(!mem[curAddress])
            curObj.format = 1;

        // check for debugging point
        for(i = curAddress; i < curAddress + curObj.format; i++)
            if(i > lastBP && searchBP(i)) {
                lastBP = i;
                dumpReg();
                printf("\n\tStop at checkpoint[%04X]\n", i);
                return;
            }

        // skip empty memory
        if(!mem[curAddress]) {
            registers[PCreg] += 1; // increase PC
            curAddress = registers[PCreg];
            continue;
        }

        registers[PCreg] += curObj.format; // increase PC
        // get target address
        curObj.addrMode = simple; // initialize as simple addressing mode
        switch(curObj.format) {
            case fmt3:
            case fmt4:
                curObj.operand.target = getTargetAddress(curAddress, curObj.format); // get target address
                curObj.addrMode = mem[curAddress] & 3; // find addressing mode
                if(mem[curAddress] & 0x80) // indexing mode
                    curObj.operand.target += registers[Xreg];
                if(curObj.addrMode == immediate) // if immediate mode
                    curObj.operand.immediate = curObj.operand.target;
                break;
            case fmt2:
                curObj.operand.reg[0] = mem[curAddress + 1] / 0x10;
                curObj.operand.reg[1] = mem[curAddress + 1] % 0x10;
                break;
            case fmt1:
                break;
            default:
                break;
        }

        targetVal = (curObj.addrMode == immediate ? curObj.operand.immediate : mem[curObj.operand.target]);
        // execute instruction
        switch(curObj.mnemonic) {
            case ADD:
                registers[Areg] += targetVal;
                break;
            case ADDF:
                registers[Freg] += targetVal;
                break;
            case ADDR:
                registers[curObj.operand.reg[1]] += registers[curObj.operand.reg[0]];
                break;
            case AND:
                registers[Areg] &= targetVal;
                break;
            case CLEAR:
                registers[curObj.operand.reg[0]] = 0;
                break;
            case COMP:
                if(registers[Areg] < targetVal)
                    CCstatus = lt;
                else if(registers[Areg] == targetVal)
                    CCstatus = eq;
                else
                    CCstatus = gt;
                break;
            case COMPF:
                if(registers[Freg] < targetVal)
                    CCstatus = lt;
                else if(registers[Freg] == targetVal)
                    CCstatus = eq;
                else
                    CCstatus = gt;
               break;
            case COMPR:
                if(registers[curObj.operand.reg[0]] < registers[curObj.operand.reg[1]])
                    CCstatus = lt;
                else if(registers[curObj.operand.reg[0]] == registers[curObj.operand.reg[1]])
                    CCstatus = eq;
                else
                    CCstatus = gt;
                break;
            case DIV:
                registers[Areg] /= targetVal;
                break;
            case DIVF:
                registers[Freg] /= targetVal;
                break;
            case DIVR:
                registers[curObj.operand.reg[1]] /= registers[curObj.operand.reg[0]];
                break;
            case FIX:
                registers[Areg] = registers[Freg];
                break;
            case FLOAT:
                registers[Freg] = registers[Areg];
                break;
            case HIO:
                break;
            case J:
                registers[PCreg] = targetVal;
                break;
            case JEQ:
                if(CCstatus == eq)
                    registers[PCreg] = targetVal;
                break;
            case JGT:
                if(CCstatus == gt)
                    registers[PCreg] = targetVal;
                break;
            case JLT:
                if(CCstatus == lt)
                    registers[PCreg] = targetVal;
                break;
            case JSUB:
                registers[Lreg] = registers[PCreg];
                registers[PCreg] = targetVal;
                break;
            case LDA:
                registers[Areg] = targetVal;
                break;
            case LDB:
                registers[Breg] = targetVal;
                break;
            case LDCH:
                registers[Areg] = (registers[Areg] & 0xF0) + targetVal;
                break;
            case LDF:
                registers[Freg] = targetVal;
                break;
            case LDL:
                registers[Lreg] = targetVal;
                break;
            case LDS:
                registers[Sreg] = targetVal;
                break;
            case LDT:
                registers[Treg] = targetVal;
                break;
            case LDX:
                registers[Xreg] = targetVal;
                break;
            case LPS:
                break;
            case MUL:
                registers[Areg] *= targetVal;
                break;
            case MULF:
                registers[Freg] *= targetVal;
                break;
            case MULR:
                registers[curObj.operand.reg[1]] *= registers[curObj.operand.reg[0]];
                break;
            case NORM:
                break;
            case OR:
                registers[Areg] |= targetVal;
                break;
            case RD:
                break;
            case RMO:
                registers[curObj.operand.reg[1]] = registers[curObj.operand.reg[0]];
                break;
            case RSUB:
                registers[PCreg] = registers[Lreg];
                break;
            case SHIFTL:
                registers[curObj.operand.reg[0]] = registers[curObj.operand.reg[0]] << registers[curObj.operand.reg[1]];
                break;
            case SIO:
                break;
            case SSK:
                break;
            case STA:
                putMem(targetVal, 3, registers[Areg]);
                break;
            case STB:
                putMem(targetVal, 3, registers[Breg]);
                break;
            case STCH:
                mem[targetVal] = registers[Areg] & 0xF;
                break;
            case STF:
                putMem(targetVal, 6, registers[Freg]);
                break;
            case STI:
                break;
            case STL:
                putMem(targetVal, 3, registers[Lreg]);
                break;
            case STS:
                putMem(targetVal, 3, registers[Sreg]);
                break;
            case STSW:
                putMem(targetVal, 3, registers[SWreg]);
                break;
            case STT:
                putMem(targetVal, 3, registers[Treg]);
                break;
            case STX:
                putMem(targetVal, 3, registers[Xreg]);
                break;
            case SUB:
                registers[Areg] -= targetVal;
                break;
            case SUBF:
                registers[Freg] -= targetVal;
                break;
            case SUBR:
                registers[curObj.operand.reg[1]] -= registers[curObj.operand.reg[0]];
                break;
            case SVC:
                break;
            case TD:
                CCstatus = lt;
                break;
            case TIO:
                break;
            case TIX:
                registers[Xreg]++;
                if(registers[Xreg] < targetVal)
                    CCstatus = lt;
                else if(registers[Xreg] == targetVal)
                    CCstatus = eq;
                else
                    CCstatus = gt;
                break;
            case TIXR:
                registers[Xreg]++;
                if(registers[Xreg] < registers[curObj.operand.reg[0]])
                    CCstatus = lt;
                else if(registers[Xreg] == registers[curObj.operand.reg[1]])
                    CCstatus = eq;
                else
                    CCstatus = gt;
                break;
            case WD:
                break;
            default: // not an opcode
                registers[PCreg] = registers[PCreg] - curObj.format + 1; // increase PC
                curAddress = registers[PCreg];
                continue;
                break;
        }
        curAddress = registers[PCreg]; // increase curAddress
    }
    // program ended, dump registers
    registers[PCreg] = endAddress;
    dumpReg(); 
    printf("\n\tEnd program.\n");
    curAddress = -1;
    lastBP = -1;
}

int getTargetAddress(int curAddress, FMT format) {
    ADR_MODE addrMode = mem[curAddress] & 3;
    int target;
    switch(addrMode) {
        case SIC:
            target = SICAddress(curAddress);
            break;
        case immediate:
            target = immediateAddress(curAddress, format);
            break;
        case indirect:
            target = indirectAddress(curAddress, format);
            break;
        case simple:
            target = simpleAddress(curAddress, format);
            break;
        default:
            break;
    }
    if(mem[curAddress + 1] & 0x80) // indexing mode
        target += registers[Xreg];
    return target;
}

int SICAddress(int curAddress) {
    return getMem(curAddress, 5) & 0x7FFF;
}

int immediateAddress(int curAddress, FMT format) {
    return simpleAddress(curAddress, format);
}

int indirectAddress(int curAddress, FMT format) {
    int target = simpleAddress(curAddress, format);
    return getMem(target, 6);
}

int simpleAddress(int curAddress, FMT format) {
    int setBit = (mem[curAddress + 1] / 0x10) & 6; // mask over b and p bits
    int target = getMem(curAddress + 1, format == fmt3 ? 3 : 5);  

    if(setBit == 2) { // PC relative
        if(target & (format == fmt3 ? 0x800 : 0x80000))
            target = target | (format == fmt3 ? 0xFFFFF000 : 0xFFF00000);
        target += registers[PCreg];
    }
    else if(setBit == 4) // Base relative
        target += registers[Breg];
    return target;
}

int getMem(int address, int hBytes) {
    int val = 0;
    int i;
    val = mem[address] % (hBytes % 2 ? 0x10 : 0x100);
    for(i = 1; i <= (hBytes - 1) / 2; i++) {
        val *= 0x100;
        val += mem[address + i];
    }
    return val;
}

void putMem(int address, int bytes, int value) {
    int i;
    for(i = address + bytes - 1; i >= address; i--) {
        mem[i] = value & 0xFF;
        value /= 0x100;
    }
}

void dumpReg() {
    printf("\t    A : %012X X : %08X\n", registers[Areg], registers[Xreg]);
    printf("\t    L : %012X PC: %012X\n", registers[Lreg], registers[PCreg]);
    printf("\t    B : %012X S : %012X\n", registers[Breg], registers[Sreg]);
    printf("\t    T : %012X", registers[Treg]);
}
