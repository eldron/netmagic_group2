/******************************************************************************************
group2's console, use this to replace demo_test() in test.c

��������shell�����еķ�ʽ����                     	:��ʾ֧�ֵ���������set poll_time [seconds]	: �����ϻ���ѯʱ����set expire_time [seconds]	:�����ϻ�ʱ��init forward_table  		: ��ʼ��ת����ȫд0��add [��������]      		: дָ��ת�����ͬʱдӲ��)��������dispaly all        		: ����ת����,��ʾ�������display [damc/dport]		: ����������dmac/dport������ת����,��ʾ��ѯ����
exit
******************************************************************************************/

#include "nmac.h"

typedef struct {
	int count;// the number of tokens in a command
	int len[100];// length of each command
	char token[100][100];// store tokens
} Tokens;

void get_tokens(char * command, Tokens * tokens){
	memset(tokens, 0, sizeof(Tokens));
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

}

void do_command(char * command, Tokens * tokens){
	// parse a command and execute it
	// add an 'exit' command, set halt_flag = 1

	// default action
	printf("Command not found!\n");
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
			printf("$:");
		} else {
			halt_flag = 1;
			break;
		}
	}
}