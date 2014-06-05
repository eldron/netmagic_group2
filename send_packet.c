// send a packet to netmagic

#include "nmac.h"

// assume outport is already in network endian
void send_packet(Message * msg, uint16_t outport/* bitmap */){
	// set namc header
	nmac_head.count = 1;
	nmac_head.reserve8_1 = 0;
	nmac_head.seq = 0;
	nmac_head.reserve16_2 = 0;
	nmac_head.nmac_type = 0x08;// 指定硬件转发数据包
	nmac_head.parameter = outport;
	nmac_head.reserve8_3 = 0;

	memcpy(payload, &nmac_head, sizeof(nmac_head));
	memcpy(payload + sizeof(nmac_head), msg->content + 34 + sizeof(nmac_head), msg->length - 34 - sizeof(nmac_head));

	// set ip protocol tag
	ip_protocol_tag = libnet_build_ipv4(
			LIBNET_IPV4_H + msg->length - 34,
			0, //服务质量
			0, //IP标记与NMAC序列号一样
			0, //偏移
			64, //TTL
			NMAC_PROTO, //上层协议类型
			0, //CRC,0表示由LIBNET自动计算
			ctrl_net_ip, netmagic_net_ip, payload, msg->length - 34, l,
			ip_protocol_tag);

	// set ether protocol tag
	ether_protocol_tag = libnet_build_ethernet(
			//构造以太网协议块
			NM[0].netmagic_mac, controller.ctrl_mac, ETHERTYPE_IP,
			NULL, 0, l, ether_protocol_tag);

	// send out, does not confirm weather netmagic received or not
	int size = libnet_write(l);
	libnet_clear_packet(l);
}
