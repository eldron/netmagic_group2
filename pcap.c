/*..........���̲߳�ͣץ�������뻺�����..........*/
#include"nmac.h"
#include "mqueue.h"

///*�ص�����*/
void packet_callback(u_char * argument, const struct pcap_pkthdr * packet_header, const u_char * packet_content){
	Message msg;
	msg.length = packet_header->len;
	memcpy(&(msg.content), packet_content, msg.length);
#ifdef DEBUG
	printf("from packet callback: msg.length = %d\n", msg.length);
#endif
	if(enqueue(&mqueue, &msg)){
		//printf("enqueued a message, %d messages in queue\n", mqueue.count);
	}
}

void packet_capture() {
	while (1) {
		if (halt_flag == 1){ //ϵͳ�ر�,ֹͣ
			break;
		}
		pcap_dispatch(pcap_handle, //ץ��,ע��ص�����
				1, packet_callback, NULL);
	}

	// delete message queue
	delete_mqueue(&mqueue);
	pcap_close(pcap_handle); //�ر�Libpcap����
//	pt1_num = 0; //ץ���߳��ѹرգ���Ϣ������ɾ��
}
