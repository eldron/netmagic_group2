// send a packet to netmagic

#include "nmac.h"

void send_packet(Message * msg, uint16_t outport){
	struct nmac_header nheader;
	// set namc header
	nheader.count = 1;
	nheader.reserve8_1 = 0;
	nheader.seq = 0;
	nheader.reserve16_2 = 0;
	nheader.nmac_type = 0x08;// ָ��Ӳ��ת�����ݰ�
	nheader.parameter = outport;
	nheader.reserve8_3 = 0;
printf("from sent packet: set nheader\n");
	u_char our_payload[1480]; //���NMAC����
	memcpy(our_payload, &nheader, sizeof(struct nmac_header));
	memcpy(our_payload + sizeof(struct nmac_header), msg->content + 34 + sizeof(struct nmac_header), msg->length - 34 - sizeof(struct nmac_header));
printf("from sent packet: copied message\n");
	libnet_ptag_t ip_tag = 0; //IP���ݿ�Э����
	libnet_ptag_t ether_tag =0 ; // ��̫�����ݿ�Э����
	ip_tag = libnet_build_ipv4(
			LIBNET_IPV4_H + msg->length - 34,
			0, //��������
			0, //IP�����NMAC���к�һ��
			0, //ƫ��
			64, //TTL
			NMAC_PROTO, //�ϲ�Э������
			0, //CRC,0��ʾ��LIBNET�Զ�����
			ctrl_net_ip, netmagic_net_ip, our_payload, msg->length - 34, group2_l,
			ip_tag);
	ether_tag = libnet_build_ethernet(
			//������̫��Э���
			NM[0].netmagic_mac, controller.ctrl_mac, ETHERTYPE_IP,
			NULL, 0, group2_l, ether_tag);

	int size = libnet_write(group2_l);
	perror("from send packet:");
	libnet_clear_packet(group2_l);
	printf("sent a packet out, size = %d\n l = %p\n", size, group2_l);
}

// assume outport is already in network endian
void old_send_packet(Message * msg, uint16_t outport/* bitmap */){
	// set namc header
	nmac_head.count = 1;
	nmac_head.reserve8_1 = 0;
	nmac_head.seq = 0;
	nmac_head.reserve16_2 = 0;
	nmac_head.nmac_type = 0x08;// ָ��Ӳ��ת�����ݰ�
	nmac_head.parameter = outport;
	nmac_head.reserve8_3 = 0;

	memcpy(payload, &nmac_head, sizeof(nmac_head));
	memcpy(payload + sizeof(nmac_head), msg->content + 34 + sizeof(nmac_head), msg->length - 34 - sizeof(nmac_head));

	// set ip protocol tag
	ip_protocol_tag = libnet_build_ipv4(
			LIBNET_IPV4_H + msg->length - 34,
			0, //��������
			0, //IP�����NMAC���к�һ��
			0, //ƫ��
			64, //TTL
			NMAC_PROTO, //�ϲ�Э������
			0, //CRC,0��ʾ��LIBNET�Զ�����
			ctrl_net_ip, netmagic_net_ip, payload, msg->length - 34, l,
			ip_protocol_tag);

	// set ether protocol tag
	ether_protocol_tag = libnet_build_ethernet(
			//������̫��Э���
			NM[0].netmagic_mac, controller.ctrl_mac, ETHERTYPE_IP,
			NULL, 0, l, ether_protocol_tag);

	// send out, does not confirm weather netmagic received or not
	int size = libnet_write(l);
	perror("from send packet:");
	libnet_clear_packet(l);
	printf("sent a packet out, size = %d\n l = %p", size, l);
}
