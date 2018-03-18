#define MAX_LEN 11

char cmd[MAX_LEN];
enum CMD_TYPE { shell, memory, opcode, invalid };

CMD_TYPE cmd_type();
