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
			// ����Ӳ�����û���еİ����޸Ŀ�������netmagic�е�ת��������typeΪ8�����ݰ������͸�netmagic

			break;
		default:
			break;
		}
	}

	printf("Bye...");
}