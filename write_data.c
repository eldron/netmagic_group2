/*..........向NetMagic中写入数据..........*/

#include"nmac.h"

/*
	此函数不应该和消息队列的dequeue  操作在一个线程里面调用
	将要发送的数据添加到data_queue  中去
*/
void nmac_write_data(u_int32_t addr, int num, u_int32_t* data) {
	struct timeval start, end; //监测时间
	ip_protocol_tag = 0; //IP数据块协议标记
	ether_protocol_tag = 0; // 以太网数据块协议标记
	u_int32_t w_addr; //存放网络字节序的写数据地址

	w_addr = htonl(addr); //将写地址转化为网络字节序
	int i;
	u_int32_t * data_net;
	data_net = (u_int32_t*) malloc(num * sizeof(u_int32_t));
	for (i = 0; i < num; i++)
		//data_net[i] = htonl(data[i]);
		data_net[i] = data[i];

	if (num * 4 > 1466) //写入数据长度溢出处理
			{
#ifdef DEBUG
		printf("from nmac_write_data: ERROR! Write_Data Size Overflowing!\n");
#endif
		//timeout_flag = 1; //为操作方便，统一用timeout_flag作为不成功标志位
		return;
	}
	u_int16_t nmac_seq = 0; //或定为随机数,用来定向回来的报文
	nmac_head.count = 1;
	nmac_head.reserve8_1 = 0;
	nmac_head.seq = htons(nmac_seq);
	nmac_head.reserve16_2 = 0;
	nmac_head.nmac_type = 0x04;
	nmac_head.parameter = htons(num);
	nmac_head.reserve8_3 = 0;
	//构造IP负载
	memcpy(payload, &nmac_head, sizeof(struct nmac_header));
	memcpy(payload+sizeof(struct nmac_header), &w_addr, sizeof(u_int32_t));
	memcpy(payload+sizeof(struct nmac_header)+sizeof(u_int32_t), data_net,
			num*sizeof(u_int32_t));
	payload_size = sizeof(struct nmac_header) + sizeof(u_int32_t)
			+ num * sizeof(u_int32_t);

	//构造IP协议块
	ip_protocol_tag = libnet_build_ipv4(LIBNET_IPV4_H + payload_size,
			0, //服务质量
			nmac_seq, //IP标记
			0, //偏移
			64, //TTL
			NMAC_PROTO, //上层协议类型
			0, //CRC,0表示由LIBNET自动计算
			ctrl_net_ip, netmagic_net_ip, payload, payload_size, l,
			ip_protocol_tag);
	//构造以太网协议块
	ether_protocol_tag = libnet_build_ethernet(netmagic_mac,
			controller.ctrl_mac, ETHERTYPE_IP, NULL, 0, l, ether_protocol_tag);
	//发送写数据请求报文
	gettimeofday(&start, NULL);
	packet_size = libnet_write(l);
	libnet_clear_packet(l);
	
	while (1) {
		gettimeofday(&end, NULL);
		//判断超时
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

		else if (write_flag == 1) //收到读响应报文
				{
			nmac_hdr = (struct nmac_header *) (write_pkt_buf + 34);
			write_flag = 0;
			if (nmac_seq == ntohs(nmac_hdr->seq)) //序列号一致
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
