/******************************************************************************************
group2's console, use this to replace demo_test() in test.c

采用类似shell命令行的方式：？                     	:显示支持的所有命令set poll_time [seconds]	: 设置老化轮询时间间隔set expire_time [seconds]	:设置老化时间init forward_table  		: 初始化转发表（全写0）add [表项内容]      		: 写指定转发表项（同时写硬件)（加锁）dispaly all        		: 遍历转发表,显示遍历结果display [dmac/dport]		: 根据条件（dmac/dport）查找转发表,显示查询表项
exit
******************************************************************************************/

#include "nmac.h"
#include "group2.h"
//#include "utils.h"

#define SET_POLL_TIME_COMMAND 200
#define SET_EXPIRE_TIME_COMMAND 201
#define INIT_FORWARD_TABLE_COMMAND 202
#define ADD_TABLE_COMMAND 203
#define DISPLAY_ALL_COMMAND 204
#define DISPLAY_DMAC_COMMAND 205
#define DISPLAY_DPORT_COMMAND 206
#define EXIT_COMMAND 207
#define DISPLAY_HELP_COMMAND 208
#define COMMAND_NOT_FOUND 209

typedef struct {
	int count;// the number of tokens in a command
	int len[100];// length of each command
	char token[100][100];// store tokens
} Tokens;

void get_tokens(char * command, Tokens * tokens){
	memset(tokens, '\0', sizeof(Tokens));
	tokens->count = 0;
	int i = 0;
	while(command[i] != '\0' && command[i] != '\n'){
		// skip all the spaces and tabs
		while(command[i] == ' ' || command[i] == '\t'){
			i++;
			if(command[i] == '\0' || command[i] == '\n'){
				return;
			}
		}

		// found a token
		tokens->len[tokens->count] = 0;
		while(command[i] != ' ' && command[i] != '\t' && command[i] != '\0' && command[i] != '\n'){
			tokens->token[tokens->count][tokens->len[tokens->count]] = command[i];
			i++;
			tokens->len[tokens->count]++;
		}
		tokens->count++;
	}
}

// execute "?"
void do_display_help(){
	printf("Available commands:\n");
	printf("?	:	print all available commands\n");
	printf("set poll_time [seconds]	:	set poll time for erase thread\n");
	printf("set expire_time [seconds]	:	set expire time for an item\n");
	printf("init forward_table	:	initialize forward table\n");
	printf("add [table content]	:	add an item in the forward table\n");
	printf("display all	:	display all items in the forward table\n");
	printf("display [dmac/dport]	:	loop up an item in the forward table\n");
	printf("exit	:	exit\n");
	printf("\n");
}

// execute "set poll_time [seconds]"
void do_set_poll_time(Tokens * tokens){
	// set ERASE_SLEEP_INTERVAL_MS
	ERASE_SLEEP_INTERVAL_MS = 1000 * atoi(tokens->token[2]);
	printf("Poll time set.\n");
}

// execute "set expire_time [seconds]"
void do_set_expire_time(Tokens * tokens){
	// set ERASE_THRESHOLD_S
	ERASE_THRESHOLD_S = atoi(tokens->token[2]);
	printf("Expire time set.\n");
}

// execute "init forward_table"
void do_init_forward_table(){
	FIB_init();
	printf("Forward table initialized.\n");
}

// execute "add [table content]"
void do_add_table(Tokens * tokens){
	FIBItem item;
	item.port = cal_byte_value(tokens->token[2]);
	Mac mac;
	cal_mac(tokens->token[1], &mac);
	item.dmac = mac;
	item.type = DMAC;
	item.valid = 1;
	item.item_type = ITEM_1;
	FIB_insert(&item);
	printf("Table content added.\n");
}

// execute "display all"
void do_display_all(){
	int i = 0;
	for(i = 0;i < FIBSIZE;i++){
		print_item(&(g_fib.at[i]));
	}
	printf("\n");
}

// execute "display dmac"
void do_display_dmac(Tokens * tokens){
	Mac mac;
	cal_mac(tokens->token[1], &mac);
	FIBItem * item = FIB_find(mac, DMAC);
	if(!item){
		item = FIB_find(mac, SMAC);
	}
	print_item(item);
	printf("\n");
}

//execute "display dport"
// dport should be in bitmap format
void do_display_dport(Tokens * tokens){
	uint8_t value = 0;
	uint8_t one = 1;
	int i = 7;
	for(i = 7;i >= 0;i--){
		if(tokens->token[1][7 - i] == '1'){
			value += (one << i);
		}
	}

	FIBItem * item = FIB_find_by_port(value);
	print_item(item);
	printf("\n");
}

// execute "exit"
void do_exit(){
	halt_flag = 1;
	printf("Bye:)\n");
	exit(0);
}

void do_command_not_found(Tokens * tokens){
	printf("Command not found!\n");
	do_display_help();
}

// get command type
int get_command_type(Tokens * tokens){
	if(tokens->count == 1){
		if(strcmp(tokens->token[0], "?") == 0){
			return DISPLAY_HELP_COMMAND;
		} else if(strcmp(tokens->token[0], "exit") == 0){
			return EXIT_COMMAND;
		} else {
			return COMMAND_NOT_FOUND;
		}
	} else if(tokens->count == 2){
		if(strcmp(tokens->token[0], "init") == 0 && strcmp(tokens->token[1], "forward_table") == 0){
			return INIT_FORWARD_TABLE_COMMAND;
		} else if(strcmp(tokens->token[0], "display") == 0 && strcmp(tokens->token[1], "all") == 0){
			return DISPLAY_ALL_COMMAND;
		} else if(strcmp(tokens->token[0], "display") == 0){
			// check if contains '.'
			int i = 0;
			int flag = 0;
			for(i = 0;i < tokens->len[1];i++){
				if(tokens->token[1][i] == '.'){
					flag = 1;
					break;
				}
			}

			if(flag){
				return DISPLAY_DMAC_COMMAND;
			} else {
				return DISPLAY_DPORT_COMMAND;
			}
		} else {
			return COMMAND_NOT_FOUND;
		}
	} else if(tokens->count == 3){
		if(strcmp(tokens->token[0], "set") == 0 && strcmp(tokens->token[1], "poll_time") == 0){
			return SET_POLL_TIME_COMMAND;
		} else if(strcmp(tokens->token[0], "set") == 0 && strcmp(tokens->token[1], "expire_time") == 0){
			return SET_EXPIRE_TIME_COMMAND;
		} else if(strcmp(tokens->token[0], "add") == 0){
			return ADD_TABLE_COMMAND;
		} else {
			return COMMAND_NOT_FOUND;
		}
	} else {
		return COMMAND_NOT_FOUND;
	}
}

// execute a command
void do_command(char * command, Tokens * tokens){
	// parse a command and execute it
	// add an 'exit' command, set halt_flag = 1
	get_tokens(command, tokens);
	int type = get_command_type(tokens);
	switch(type){
	case SET_POLL_TIME_COMMAND:
		do_set_poll_time(tokens);
		break;
	case SET_EXPIRE_TIME_COMMAND:
		do_set_expire_time(tokens);
		break;
	case INIT_FORWARD_TABLE_COMMAND:
		do_init_forward_table();
		break;
	case ADD_TABLE_COMMAND:
		do_add_table(tokens);
		break;
	case DISPLAY_ALL_COMMAND:
		do_display_all();
		break;
	case DISPLAY_DMAC_COMMAND:
		do_display_dmac(tokens);
		break;
	case DISPLAY_DPORT_COMMAND:
		do_display_dport(tokens);
		break;
	case EXIT_COMMAND:
		do_exit();
		break;
	case DISPLAY_HELP_COMMAND:
		do_display_help();
		break;
	default:
		do_command_not_found(tokens);
		break;
	}
}

void console(){
	nm_init();// set netmagic's ip and mac
	if(!nmac_init(0)){
		halt_flag = 1;
		return;
	}

	printf("Connected to NetMagic successfully!\n");
	FIB_init();
	printf("FIB initialized\n");
	// our logic
	char command[100];
	Tokens tokens;
	while(!halt_flag){
		memset(command, '\0', 100);
		printf("\n$:");
		if(fgets(command, 100, stdin)){
			// execute each command
			do_command(command, &tokens);
		} else {
			halt_flag = 1;
			break;
		}
	}
}
