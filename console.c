// group2's console, use this to replace demo_test() in test.c

#include "nmac.h"

void do_command(char * command){
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
	while(!halt_flag){
		memset(command, '\0', 100);
		if(fgets(command, 100, stdin)){
			// execute each command
			do_command(command);
		} else {
			halt_flag = 1;
			break;
		}
	}
}