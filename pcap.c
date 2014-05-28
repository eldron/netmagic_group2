/*..........子线程不停抓包并存入缓存队列..........*/
#include"nmac.h"

///*回调函数*/
//void packet_callback(u_char *argument, const struct pcap_pkthdr* packet_header,
//		const u_char* packet_content) {
//	int ret = -1;
//	int len = packet_header->len; //捕获的报文长度
//
//	pkt_buf.mtype = 10;
//	memcpy(&pkt_buf.mtext, packet_content, len);
//	ret = msgsnd(msg_id, &pkt_buf, len, IPC_NOWAIT);
//	if (-1 == ret) {
//		printf("Wrong! Can not join the queue!!!\n");
//		halt_flag = 1;
//	} else {
//		packet_num++;
//		printf("The queue len is %d . \n", packet_num);
//	}
//}

// our callback function
void packet_callback(u_char * argument, const struct pcap_pkthdr * packet_header, const u_char * packet_content){
	Message msg;
	msg.length = packet_header->len;
	memcpy(&(msg.content), packet_content, msg.length);
	if(enqueue(&mqueue, &msg)){
		printf("enqueued a message, %d messages in queue\n", mqueue.count);
	}
}

void packet_capture() {
	while (1) {
		if (halt_flag == 1){ //系统关闭,停止
			break;
		}
		pcap_dispatch(pcap_handle, //抓包,注册回调函数
				1, packet_callback, NULL);
	}

	//msgctl(msg_id, IPC_RMID, NULL); //删除队列
	// delete message queue
	delete_mqueue(&mqueue);
	pcap_close(pcap_handle); //关闭Libpcap操作
	pt1_num = 0; //抓包线程已关闭，消息队列已删除
}
