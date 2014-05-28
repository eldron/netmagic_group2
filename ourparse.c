// replace the original parsing.c

#include "nmac.h"

void ourparse(){
	while(!halt_flag){
		Message msg;
		dequeue(&mqueue, &msg);
		printf("dequeued a message, %d messages in queue\n", mqueue.count);

		nmac_hdr = (struct nmac_header *) (msg.content + 34);
		int type = (int) nmac_hdr->nmac_type;
		switch(type){
		case 1:
			printf("form ourparse: connection set\n");
			con_flag = 1;
			pause_flag = 1;
			break;
		case 5:
			printf("from ourparse: received read response\n");
			break;
		case 6:
			printf("from ourparse: received write response\n");
			write_flag = 1;
			pause_flag = 1;
			break;
		case 7:
			printf("from ourparse: received asy message");
			pause_flag = 1;
			// our logic
			// 接收硬件查表没命中的包，修改控制器和netmagic中的转发表，构造type为8的数据包，发送给netmagic

			break;
		default:
			break;
		}
	}

	printf("Bye...");
}