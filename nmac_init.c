/*..........获取相应NetMagic的IP地址,MAC地址..........*/
/*..........初始化与相应NetMagic的连接..........*/

#include "nmac.h"

int hextodec(char x) {
	int n;
	switch (x) {
	case '0':
		n = 0;
		break;
	case '1':
		n = 1;
		break;
	case '2':
		n = 2;
		break;
	case '3':
		n = 3;
		break;
	case '4':
		n = 4;
		break;
	case '5':
		n = 5;
		break;
	case '6':
		n = 6;
		break;
	case '7':
		n = 7;
		break;
	case '8':
		n = 8;
		break;
	case '9':
		n = 9;
		break;
	case 'a':
		n = 10;
		break;
	case 'b':
		n = 11;
		break;
	case 'c':
		n = 12;
		break;
	case 'd':
		n = 13;
		break;
	case 'e':
		n = 14;
		break;
	case 'f':
		n = 15;
		break;
	}
	return n;
}

void nm_init() {
	//静态配置NetMagic[0]的IP, Mac, 实验演示时只有一台NetMagic
	char mac_tmp[18];
	NM[0].netmagic_id = 0;
	memcpy(&mac_tmp, "88:88:88:88:88:88", 17);
	memcpy((&NM[0])->netmagic_ip, "136.136.136.136", 15);
	int i, j = 0;
	for (i = 0; i < 17; i = i + 3) //获取NetMagic的MAC地址
			{
		(&NM[0])->netmagic_mac[j] = 16 * hextodec(mac_tmp[i])
				+ hextodec(mac_tmp[i + 1]);
		j++;
	}
	/*
	 printf("NetMagic[0] MAC:");
	 for(i=0;i<5;i++)
	 {
	 printf("%02x:",(&NM[0])->netmagic_mac[i]);
	 }
	 printf("%02x\n",(&NM[0])->netmagic_mac[5]);
	 */
}

int nmac_init(int netmagic_id) //连接成功返回1
{
	struct timeval start, end; //监测时间
	//将NetMagic的点分十进制IP转为32位网络字节序
	netmagic_net_ip = libnet_name2addr4(l, (&NM[netmagic_id])->netmagic_ip,
			LIBNET_DONT_RESOLVE);
	char libnet_errbuf[LIBNET_ERRBUF_SIZE]; //存放错误信息
	ip_protocol_tag = 0; //IP数据块协议标记
	ether_protocol_tag = 0; // 以太网数据块协议标记

	//构造nmac报文，连接netmagic
	u_int16_t nmac_seq = 0; //或定为随机数,用来定向回来的报文
	nmac_head.count = 1; //命令数目
	nmac_head.reserve8_1 = 0;
	nmac_head.seq = htons(nmac_seq);
	nmac_head.reserve16_2 = 0;
	nmac_head.nmac_type = 0x01; //连接请求
	nmac_head.parameter = htons(0x0001);
	nmac_head.reserve8_3 = 0;

	memcpy(payload, &nmac_head, sizeof(struct nmac_header));
	//nmac报文作为IP报文的数据部分
	payload_size = sizeof(struct nmac_header); //计算负载大小，8位字节数

	ip_protocol_tag = libnet_build_ipv4( //构造IP协议块
	LIBNET_IPV4_H + payload_size,
			0, //服务质量
			nmac_seq, //IP标记与NMAC序列号一样
			0, //偏移
			64, //TTL
			NMAC_PROTO, //上层协议类型
			0, //CRC,0表示由LIBNET自动计算
			ctrl_net_ip, netmagic_net_ip, payload, payload_size, l,
			ip_protocol_tag);

	ether_protocol_tag = libnet_build_ethernet(
			//构造以太网协议块
			NM[netmagic_id].netmagic_mac, controller.ctrl_mac, ETHERTYPE_IP,
			NULL, 0, l, ether_protocol_tag);

	gettimeofday(&start, NULL);

	packet_size = libnet_write(l); //发送连接请求数据包
	printf("Send a NMAC_Startup_Request!\n");
	//printf("Send len is: %d\n",packet_size);
	while (1) {
		gettimeofday(&end, NULL);
		if ((float) ((1000000 * (end.tv_sec - start.tv_sec)
				+ (end.tv_usec - start.tv_usec)) / 1000000) > 2.5) {
			printf("ERROR! Connect Time Out!\n");
			return 0;
		} else if (con_flag == 1) //收到连接请求确认报文
				{
			nmac_hdr = (struct nmac_header *) (con_pkt_buf + 34);
			if (nmac_seq == ntohs(nmac_hdr->seq)) //序列号一致
			{
				printf("Connecting success!\n");
				con_flag = 0;
				pause_flag = 0;
				//Link H = Creat_List();
				//(&NM[0])->head = H;
				int i;
				for (i = 0; i++; i < 100) {
					//NM[0].Addr_flag[i] = 0;
				}

				nmac_seq++;
				//构造连接确认数据包，生成IP负载
				nmac_head.parameter = htons(0);
				nmac_head.seq = htons(nmac_seq);
				memcpy(payload, &nmac_head, sizeof(struct nmac_header));
				payload_size = sizeof(struct nmac_header);

				ip_protocol_tag = libnet_build_ipv4( //构造IP协议块
				LIBNET_IPV4_H + payload_size,
						0, //服务质量
						nmac_seq, //IP标记
						0,
						64, //TTL
						NMAC_PROTO, //上层协议类型
						0, //CRC,0表示由LIBNET自动计算
						ctrl_net_ip, netmagic_net_ip, payload, payload_size, l,
						ip_protocol_tag);

				ether_protocol_tag = libnet_build_ethernet(
						//构造以太网协议块
						NM[0].netmagic_mac, controller.ctrl_mac, ETHERTYPE_IP,
						NULL, 0, l, ether_protocol_tag);

				packet_size = libnet_write(l); //发送连接确认数据包
				libnet_clear_packet(l);
				break;
			}
		}
	}
	return 1;
}

