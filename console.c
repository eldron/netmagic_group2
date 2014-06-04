/******************************************************************************************
group2's console, use this to replace demo_test() in test.c

��������shell�����еķ�ʽ����                     	:��ʾ֧�ֵ���������set poll_time [seconds]	: �����ϻ���ѯʱ����set expire_time [seconds]	:�����ϻ�ʱ��init forward_table  		: ��ʼ��ת����ȫд0��add [��������]      		: дָ��ת�����ͬʱдӲ��)��������dispaly all        		: ����ת����,��ʾ�������display [dmac/dport]		: ����������dmac/dport������ת����,��ʾ��ѯ����
exit
******************************************************************************************/

#include "nmac.h"

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
		while(command[i] != ' ' && command[i] != '\t' && command[i] != '\0' && command[i] != '\n'){
			tokens->len[tokens->count] = 0;
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
	printf("?	:	��ʾ֧�ֵ���������\n");
	printf("set poll_time [seconds]	:	�����ϻ���ѯʱ����\n");
	printf("set expire_time [seconds]	:	�����ϻ�ʱ��\n");
	printf("init forward_table	:	��ʼ��ת����\n");
	printf("add [table content]	:	дָ��ת������\n");
	printf("display all	:	��ʾ����ת������\n");
	printf("display [dmac/dport]	:	������������ת����\n");
	printf("exit	:	�˳�\n");
	printf("\n");
}

// execute "set poll_time [seconds]"
void do_set_poll_time(Tokens * tokens){
	
	printf("Poll time set.\n");
}

// execute "set expire_time [seconds]"
void do_set_expire_time(Tokens * tokens){
	
	printf("Expire time set.\n");
}

// execute "init forward_table"
void do_init_forward_table(){
	
	printf("Forward table initialized.\n");
}

// execute "add [table content]"
void do_add_table(Tokens * tokens){

	printf("Table content added.\n");
}

// execute "display all"
void do_display_all(){

	printf("\n");
}

// execute "display dmac"
void do_display_dmac(Tokens * tokens){

	printf("\n");
}

//execute "display dport"
void do_display_dport(Tokens * tokens){
	
	printf("\n");
}

// execute "exit"
void do_exit(){
	halt_flag = 1;
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
			return SET_RXPIRE_TIME_COMMAND;
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
	case SET_EXPIRE_TIME_COMMAND:
		do_set_expire_time(tokens);
	case INIT_FORWARD_TABLE_COMMAND:
		do_init_forward_table();
	case ADD_TABLE_COMMAND:
		do_add_table(tokens);
	case DISPLAY_ALL_COMMAND:
		do_display_all();
	case DISPLAY_DMAC_COMMAND:
		do_display_dmac(tokens);
	case DISPLAY_DPORT_COMMAND:
		do_display_dport(tokens);
	case EXIT_COMMAND:
		do_exit();
	case DISPLAY_HELP_COMMAND:
		do_display_help();
	default:
		do_command_not_found(tokens);
	}
}

void console(){
	nm_init();// set netmagic's ip and mac
	if(!nmac_init(0)){
		halt_flag = 1;
		return;
	}

	printf("Connected to NetMagic successfully!\n");
	// our logic
	char command[100];
	Tokens tokens;
	while(!halt_flag){
		memset(command, '\0', 100);
		if(fgets(command, 100, stdin)){
			// execute each command
			do_command(command, &tokens);
			printf("\n$:");
		} else {
			halt_flag = 1;
			break;
		}
	}
}