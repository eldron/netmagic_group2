// replace the original parsing.c

#include "nmac.h"
#include "group2.h"
#include "mqueue.h"

void ourparse(){
	while(!halt_flag){
		//if(pause_flag){
			//continue;
		//}�#
		Message msg;
		dequeue(&mqueue, &msg);
		//printf("dequeued a message, %d messages in queue\n", mqueue.count);

		nmac_hdr = (struct nmac_header *) (msg.content + 34);
		int type = (int) nmac_hdr->nmac_type;
		switch(type){
		case 1:
			printf("form ourparse: connection set\n");
			con_pkt_buf = (char *) (msg.content);
			con_flag = 1;
			break;
		case 5:
			printf("from ourparse: received read response\n");
			break;
		case 6:
			printf("from ourparse: received write response\n");
			write_pkt_buf = (char *) (msg.content);
			write_flag = 1;
			break;
		case 7:
			printf("from ourparse: received asy message\n");
			// our logic
			// ����Ӳ�����û���еİ����޸Ŀ�������netmagic�е�ת����������typeΪ8�����ݰ������͸�netmagic
			uint16_t outport = 0;
			// set outport here, network endian, set to all, except cpu port and d->sport
			//outport = PORT_ALL & (~PORT_CPU);
			//uint16_t tmp = d.sport;
			//tmp = tmp << 8;
			//outport = outport & (~tmp);
			//outport = htons(outport);
			outport = htons(PORT_ALL);
			// send packet to netmagic
			send_packet(&msg, outport);
			
			PacketDigest d;
			uint8_t * ptr = (uint8_t *) (msg.content + 34 + sizeof(struct nmac_header) + 4);// ָ��������̫�����ĵ�macͷ��
			memcpy(&d.dmac, ptr, 6);
			memcpy(&d.smac, ptr + 6, 6);
/*
int i = 0;
for(i = 0;i < 6;i++){
	hex_print_decimal(stdout, *(ptr + i));
	fprintf(stdout, ":");
}
fprintf(stdout, "\t");
for(i = 0;i < 6;i++){
	hex_print_decimal(stdout, *(ptr + 6 + i));
	fprintf(stdout, ":");
}
fprintf(stdout, "\n");
*/
			ptr = (uint8_t *) (msg.content + 41); // ָ��˿�
			memcpy(&d.sport, ptr, 1);
print_packet_digest(stdout, &d);
			onPacketArrival(&d);
			break;
		default:
			break;
		}
	}

	printf("Bye...");
}
