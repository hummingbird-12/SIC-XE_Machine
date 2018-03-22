#include "main.h"

int main() {
	char inp[CMD_LEN];
	USR_CMD cmdExec;

	resetCMD();
	hash_create();

	while(1) {
		printf("sicsim> ");
		fgets(inp, CMD_LEN, stdin);
		inp[strlen(inp) - 1] = '\0';
		cmdExec = findCMD(inp);
		if(cmdExec.cmd < invFormat)
			hist_add(inp);

		switch(cmdExec.cmd) {
			case help:
				helpCMD();
				break;
			case dir:
				dirCMD();
				break;
			case quit:
				quitCMD();
				break;
			case hist:
				histCMD();
				break;
			case dump:
				dumpCMD(cmdExec);
				break;
			case edit:
				editCMD(cmdExec);
				break;
			case fill:
				fillCMD(cmdExec);
				break;
			case reset:
				resetCMD();
				break;
			case op:
				opCMD(cmdExec);
				break;
			case oplist:
				oplistCMD();
				break;
			case invFormat:
				invFormatCMD();
				break;
			case invHex:
				invHexCMD();
				break;
			case invVal:
				invValCMD();
				break;
		}
	}
	return 0;
}

USR_CMD findCMD(char* str) {
	int i, j;
	char delim[] = " ,\t\n";
	char inp[CMD_LEN];
	char* tok;
	USR_CMD u_cmd;

	strcpy(inp, str);

	u_cmd.cmd = invFormat; // initialize as invalid
	u_cmd.param_cnt = 0;
	
	if(!strlen(str))
		return u_cmd;

	tok = strtok(inp, delim); // first word of input
	if(!tok)
		return u_cmd;
	for(i = 0; i < CMD_CNT - 1; i++)
		if(!strcmp(tok, cmdList[i].str) || !strcmp(tok, cmdList[i].abb)) {
			u_cmd.cmd = cmdList[i].func;
			break;
		}

	if(u_cmd.cmd == invFormat) // invalid command
		return u_cmd;

	// get parameters
	j = 0;
	while((tok = strtok(NULL, delim)))
		strcpy(u_cmd.param[j++], tok);
	u_cmd.param_cnt = j;

	switch(testValidInput(u_cmd, cmdList[i])) {
		case FORMAT:
			u_cmd.cmd = invFormat;
			break;
		case HEX:
			u_cmd.cmd = invHex;
			break;
		case VALUE:
			u_cmd.cmd = invVal;
			break;
		default:
			break;
	}
	return u_cmd;
}

ER_CODE testValidInput(USR_CMD usr_cmd, COMMAND format) {
	int i;
	int arg[3];
	ER_CODE code = SAFE;
	if(usr_cmd.cmd == invFormat)
		return FORMAT;

	// check argument count
	switch(usr_cmd.cmd) {
		// strictly 0 arguments
		case help:
		case dir:
		case quit:
		case hist:
		case reset:
		case oplist:
			if(usr_cmd.param_cnt)
				code = FORMAT;
			break;
		// strictly 1 argument
		case op:
			if(usr_cmd.param_cnt != 1)
				code = FORMAT;
			break;
		// strictly 2 arguments
		case edit:
			if(usr_cmd.param_cnt != 2)
				code = FORMAT;
			break;
		// strictly 3 arguments
		case fill:
			if(usr_cmd.param_cnt != 3)
				code = FORMAT;
			break;
		// need less than 3
		case dump:
			if(usr_cmd.param_cnt > 2)
				code = FORMAT;
			break;
		default:
			break;
	}
	if(code == FORMAT)
		return code;

	// check hexadecimal number
	if(format.type == memory) {
		for(i = 0; i < usr_cmd.param_cnt; i++)
			if((arg[i] = hexToDec(usr_cmd.param[i])) == -1)
				code = HEX;
		switch(usr_cmd.cmd) {
			case edit:
				if(arg[0] >= MEM_SIZE || arg[1] > 255)
					code = VALUE;
				break;
			case fill:
				if(arg[0] >= MEM_SIZE || arg[1] >= MEM_SIZE || arg[0] > arg[1] || arg[2] > 255)
					code = VALUE;
				break;
			case dump:
				switch(usr_cmd.param_cnt) {
					case 2:
						if(arg[1] >= MEM_SIZE || arg[0] > arg[1])
							code = VALUE;
					case 1:
						if(arg[0] >= MEM_SIZE)
							code = VALUE;
						break;
				}
				break;
			default:
				break;
		}
	}
	return code;
}

void helpCMD() {
	printf("h[elp]\n"
			"d[ir]\n"
			"q[uit]\n"
			"hi[story]\n"
			"du[mp] [start, end]\n"
			"e[dit] address, value\n"
			"f[ill] start, end, value\n"
			"reset\n"
			"opcode mnemonic\n"
			"opcodelist\n");
}

void dirCMD() {
	DIR* dir = opendir("."); // current directory
	char* e_str;
	char path[258] = "./"; // entry path string
	ENTRY* ent; // entry
	STBUF buf; // stat

	if(!dir) {
		puts("ERROR opening directory...");
		return;
	}
	ent = readdir(dir); // read entry
	while(ent) {
		path[2] = '\0'; // clear path string
		e_str = ent->d_name; // entry name
		stat(strcat(path, e_str), &buf);
		printf("\t%-s", e_str); // print entry name

		if(S_ISDIR(buf.st_mode)) // check for directory
			printf("/");
		else if(buf.st_mode & S_IXUSR) // check for exec file
			printf("*");

		ent = readdir(dir); // read next entry
	}
	closedir(dir);
	puts("");
}

void quitCMD() {
	puts("Exiting SIC...");
	hist_free();
	exit(0);
}

void histCMD() {
	HIST_NODE* cur = hist_head;
	int cnt = 1;

	while(cur) {
		printf("\t%-3d  ", cnt++);
		puts(cur->str);
		cur = cur->next;
	}
}

void dumpCMD(USR_CMD uscmd) {
	static int start = 0;
	int end, i, j;

	if(start >= MEM_SIZE)
		start = 0;
	end = start + 159;
	if(end >= MEM_SIZE)
		end = MEM_SIZE - 1;
	if(uscmd.param_cnt) {
		start = hexToDec(uscmd.param[0]);
		end = start + 159;
		if(uscmd.param_cnt == 2)
			end = hexToDec(uscmd.param[1]);
	}

	for(i = start / 16 * 16; i < (end / 16 + 1) * 16; i++) {
		if(!(i % 16))
			printf("%05X ", i);
		if(i < start || i > end)
			printf("   ");
		else
			printf("%02X ", mem2[i]);
		if(!((i + 1) % 16)) {
			printf("; ");
			for(j = i - 15; j <= i; j++)
				printf("%c", ((j >= start && j <= end) && mem2[j] >= 32 && mem2[j] <= 126) ? mem2[j] : '.');
			puts("");
		}
	}
	start = end + 1;
}

void editCMD(USR_CMD uscmd) {
	int add, val;
	add = hexToDec(uscmd.param[0]);
	val = hexToDec(uscmd.param[1]);
	mem2[add] = val;
}

void fillCMD(USR_CMD uscmd) {
	int i, start, end, val;
	start = hexToDec(uscmd.param[0]);
	end = hexToDec(uscmd.param[1]);
	val = hexToDec(uscmd.param[2]);
	for(i = start; i <= end; i++)
		mem2[i] = val;
}

void resetCMD() {
	int i, j = 0;
	for(i = 0; i < MEM_SIZE; i++) {
		mem2[i] = j++;
		if(j == 128)
			j = 0;
	}
}

void opCMD(USR_CMD uscmd) {
	HASH_ENTRY* bucket;
	bucket = hash_table[hash_function(uscmd.param[0])];
	while(bucket && strcmp(bucket->inst, uscmd.param[0]))
		bucket = bucket->next;
	if(bucket)
		printf("opcode is %s\n", bucket->code);
	else {
		puts("ERROR: opcode not found.");
		puts("Type \"opcodelist\" for list of available opcodes.");
	}
}

void oplistCMD() {
	HASH_ENTRY* bucket;
	int i;

	for(i = 0; i < HASH_SIZE; i++) {
		printf("%2d : ", i);
		bucket = hash_table[i];
		while(bucket) {
			printf("[%s,%s]", bucket->inst, bucket->code);
			if((bucket = bucket->next))
				printf(" -> ");
		}
		puts("");
	}
}

void invFormatCMD() {
	puts("ERROR: Invalid command.");
	puts("Type \"help\" for list and formats of commands.");
}

void invHexCMD() {
	puts("ERROR: Incorrect hexadecimal.");
}

void invValCMD() {
	puts("ERROR: Invalid address.");
	puts("Memory size:\t\t1MB [0x00000 ~ 0xFFFFF]");
	puts("Edit value range:\t 1B [0x00 ~ 0xFF]");
}

void hash_create() {
	FILE* fp = fopen("opcode.txt", "r");
	char cd[3], ins[10], md[4];
	HASH_ENTRY* bucket;

	if(!fp) {
		puts("ERROR: Unable to load \"opcode.txt\".");
		return;
	}

	while(fscanf(fp, "%s %s %s", cd, ins, md) == 3) {
		bucket = malloc(sizeof(HASH_ENTRY));
		strcpy(bucket->code, cd);
		strcpy(bucket->inst, ins);
		bucket->codeVal = hexToDec(cd);
		bucket->mode = md[0] - '1';
		bucket->next = NULL;
		
		hash_add_bucket(hash_function(bucket->inst), bucket);
	}

	if(fclose(fp)) {
		puts("WARNING: Error closing \"opcode.txt\".");
		return;
	}
}

int hash_function(char* inst) {
	return abs( (int) inst[0] * 2 + abs(inst[0] + inst[1] + inst[2]) ) % HASH_SIZE;
}

void hash_add_bucket(int hash, HASH_ENTRY* bucket) {
	HASH_ENTRY* cur = hash_table[hash];

	if(!cur) {
		hash_table[hash] = bucket;
		return;
	}
	while(cur->next)
		cur = cur->next;
	cur->next = bucket;
}

void hist_add(char* str) {
	HIST_NODE* cur = hist_head;
	HIST_NODE* new_hist = malloc(sizeof(HIST_NODE));
	strcpy(new_hist->str, str);
	new_hist->next = NULL;

	if(!hist_head) {
		hist_head = new_hist;
		return;
	}
	while(cur->next)
		cur = cur->next;
	cur->next = new_hist;
}

void hist_free() {
	HIST_NODE* cur = hist_head;
	HIST_NODE* nex;
	while(cur) {
		nex = cur->next;
		free(cur);
		cur = nex;
	}
	hist_head = NULL;
}

int hexToDec(char* hex) {
	int i, dec = 0, multiplier = 1;
	for(i = strlen(hex) - 1; i >= 0; i--) {
		if(!isxdigit(hex[i]))
			return -1;
		dec += multiplier * (isdigit(hex[i]) ? (hex[i] - '0') : (toupper(hex[i]) - 'A' + 10));
		multiplier *= 16;
	}
	return dec;
}

bool testValidAdr(char* start, char* end) {
	int st = hexToDec(start), ed = hexToDec(end);
	if(st + ed < 0 || st > ed || st > MEM_VLEN * MEM_HLEN)
		return false;
	return true;
}
