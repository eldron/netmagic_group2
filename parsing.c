/*..........���߳�..........*/
/*..........�ӻ�����������ȡ������..........*/
/*..........��������������̽��д���..........*/
#include"nmac.h"

void parsing() {
	while (1) {
		if (halt_flag == 1) {
			printf("Ready to close!\n");
			break;
		} else if ((packet_num < 1) || (pause_flag == 1)) { //����Ϊ��
			continue;
		} else {
			int ret = -1;
			ret = msgrcv(msg_id, &pkt_read_buf, sizeof(struct msgmbuf), 0,
					IPC_NOWAIT);
			if (-1 == ret) {
				printf("Wrong!Can not read!!!\n");
				halt_flag = 1;
				break;
			} else {
				packet_num--;
				printf("The queue len is %d.\n", packet_num);
				//u_int8_t ip_type;
				nmac_hdr = (struct nmac_header *) ((&pkt_read_buf)->mtext + 34);
				int type = (int) (nmac_hdr->nmac_type);
				//u_int16_t seq;
				printf("type:%d\n", type);
				switch (type) {
				case 1: //���Ӿ���
				{
					con_flag = 1;/*
					con_pkt_buf = (char *) malloc(ret * sizeof(char));
					con_pkt_buf = (char *) ((&pkt_read_buf)->mtext);*/
					pause_flag = 1;
					break;
				}
				case 5: //����Ӧ
				{
					printf("Recive Read response!\n");
					break;
				}
				case 6: //д��Ӧ
				{
					write_flag = 1;/*
					write_pkt_buf = (char *) malloc(ret * sizeof(char));
					write_pkt_buf = (char *) ((&pkt_read_buf)->mtext);*/
					pause_flag = 1;
					break;
				}
				case 7: //�첽��Ϣ
				{
					/*asy_flag = 1;
					asy_pkt_buf = (char *) malloc(ret * sizeof(char));
					asy_pkt_buf = (char*) ((&pkt_read_buf)->mtext);*/
					pause_flag = 1;

					// our logic here
					// ����Ӳ�����û���еİ����޸Ŀ�������netmagic�е�ת��������typeΪ8�����ݰ������͸�netmagic
					break;
				}
				default:
					break;
				}
			}
		}
	}
}
