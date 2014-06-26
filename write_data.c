/*..........��NetMagic��д������..........*/

#include"nmac.h"

/*
	�˺�����Ӧ�ú���Ϣ���е�dequeue  ������һ���߳��������
	��Ҫ���͵�������ӵ�data_queue  ��ȥ
*/
void nmac_write_data(u_int32_t addr, int num, u_int32_t* data) {
	struct timeval start, end; //���ʱ��
	ip_protocol_tag = 0; //IP���ݿ�Э����
	ether_protocol_tag = 0; // ��̫�����ݿ�Э����
	u_int32_t w_addr; //��������ֽ����д���ݵ�ַ

	w_addr = htonl(addr); //��д��ַת��Ϊ�����ֽ���
	int i;
	u_int32_t * data_net;
	data_net = (u_int32_t*) malloc(num * sizeof(u_int32_t));
	for (i = 0; i < num; i++)
		//data_net[i] = htonl(data[i]);
		data_net[i] = data[i];

	if (num * 4 > 1466) //д�����ݳ����������
			{
#ifdef DEBUG
		printf("from nmac_write_data: ERROR! Write_Data Size Overflowing!\n");
#endif
		//timeout_flag = 1; //Ϊ�������㣬ͳһ��timeout_flag��Ϊ���ɹ���־λ
		return;
	}
	u_int16_t nmac_seq = 0; //��Ϊ�����,������������ı���
	nmac_head.count = 1;
	nmac_head.reserve8_1 = 0;
	nmac_head.seq = htons(nmac_seq);
	nmac_head.reserve16_2 = 0;
	nmac_head.nmac_type = 0x04;
	nmac_head.parameter = htons(num);
	nmac_head.reserve8_3 = 0;
	//����IP����
	memcpy(payload, &nmac_head, sizeof(struct nmac_header));
	memcpy(payload+sizeof(struct nmac_header), &w_addr, sizeof(u_int32_t));
	memcpy(payload+sizeof(struct nmac_header)+sizeof(u_int32_t), data_net,
			num*sizeof(u_int32_t));
	payload_size = sizeof(struct nmac_header) + sizeof(u_int32_t)
			+ num * sizeof(u_int32_t);

	//����IPЭ���
	ip_protocol_tag = libnet_build_ipv4(LIBNET_IPV4_H + payload_size,
			0, //��������
			nmac_seq, //IP���
			0, //ƫ��
			64, //TTL
			NMAC_PROTO, //�ϲ�Э������
			0, //CRC,0��ʾ��LIBNET�Զ�����
			ctrl_net_ip, netmagic_net_ip, payload, payload_size, l,
			ip_protocol_tag);
	//������̫��Э���
	ether_protocol_tag = libnet_build_ethernet(netmagic_mac,
			controller.ctrl_mac, ETHERTYPE_IP, NULL, 0, l, ether_protocol_tag);
	//����д����������
	gettimeofday(&start, NULL);
	packet_size = libnet_write(l);
	libnet_clear_packet(l);
	
	while (1) {
		gettimeofday(&end, NULL);
		//�жϳ�ʱ
		/*
		if ((float) ((1000000 * (end.tv_sec - start.tv_sec)
				+ (end.tv_usec - start.tv_usec)) / 1000000) > 2.5) {
			printf("ERROR! Time Out!\n");
			timeout_flag = 1;
			return;
		}*/
		if(end.tv_sec - start.tv_sec > 2){
#ifdef DEBUG
			printf("from nmac write data: time out\n");
#endif
			timeout_flag = 1;
			return;
		}

		else if (write_flag == 1) //�յ�����Ӧ����
				{
			nmac_hdr = (struct nmac_header *) (write_pkt_buf + 34);
			write_flag = 0;
			if (nmac_seq == ntohs(nmac_hdr->seq)) //���к�һ��
			{
#ifdef DEBUG
				printf("from nmac_write_data: Write data success!\n");
#endif
				//write_flag = 0;
				//pause_flag = 0;
				//libnet_clear_packet(l);
				return;
			}
		}
	}
}
