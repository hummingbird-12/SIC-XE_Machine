#define Areg 0
#define Xreg 1
#define Lreg 2
#define PCreg 3
#define Breg 4
#define Sreg 5
#define Treg 6

typedef enum {
    ADD,
    ADDF,
    ADDR,
    AND,
    CLEAR,
    COMP,
    COMPF,
    COMPR,
    DIV,
    DIVF,
    DIVR,
    FIX,
    FLOAT,
    HIO,
    J,
    JEQ,
    JGT,
    JLT,
    JSUB,
    LDA,
    LDB,
    LDCH,
    LDF,
    LDL,
    LDS,
    LDT,
    LDX,
    LPS,
    MUL,
    MULF,
    MULR,
    NORM,
    OR,
    RD,
    RMO,
    RSUB,
    SHIFTL,
    SIO,
    SSK,
    STA,
    STB,
    STCH,
    STF,
    STI,
    STL,
    STS,
    STSW,
    STT,
    STX,
    SUB,
    SUBF,
    SUBR,
    SVC,
    TD,
    TIO,
    TIX,
    TIXR,
    WD
} OPmnemonic;

typedef struct {
    enum { constant, fmt1, fmt2, fmt3, fmt4 } format;
    OPmnemonic mnemonic;
    union {
        int target;
        int immediate;
        REG reg[2];
    } operand;
} OBJ;

bool bpCMD(INPUT_CMD);
bool searchBP(int);

void runCMD();
void dumpReg();
