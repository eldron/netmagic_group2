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
			PacketDigest d;
			uint8_t * ptr = (uint8_t *) (msg.content + 34 + sizeof(nmac_hdr) + 4);
			memcpy(&d.dmac, ptr, 6);
			memcpy(&d.smac, ptr + 6, 6);
			ptr = (uint8_t *) (msg.content + 41);
			memcpy(&d.sport, ptr, 1);
			onPacketArrival(&d);

			uint16_t outport = 0;
			// set outport here, network endian, set to all, except cpu port and d->sport
			outport = PORT_ALL & (~PORT_CPU);
			uint16_t tmp = d->sport;
			tmp = tmp << 8;
			outport = outport & (~tmp);
			outport = htons(outport);
			// send packet to netmagic
			send_packet(&msg, outport);
			break;
		default:
			break;
		}
	}

	printf("Bye...");
}