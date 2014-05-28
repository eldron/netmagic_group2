/*..........����&ɾ��������..........*/
#include"nmac.h"

int Add_flow_entry(int netmagic_id, FlowEntry entry) //�ɹ�����1����ʱ����0
{
	//ȡ��Ҫ������netmagic��mac��ַ��ip��ַ
	netmagic_net_ip = libnet_name2addr4(l, (&NM[netmagic_id])->netmagic_ip,
			LIBNET_DONT_RESOLVE);
	memcpy(&netmagic_mac, (&NM[netmagic_id])->netmagic_mac, 6);

	//����nmacд�����е�nmac���ݲ���
	u_int32_t *flow_entry_buf;
	flow_entry_buf = (u_int32_t*) malloc(2 * sizeof(u_int32_t)); //����2����ַ�ռ䣬�����һ������
	flow_entry_buf[0] = htonl(entry.dst_ip); //��һ����ַ��Ŀ��ip
	flow_entry_buf[1] = ((u_int32_t) entry.trans_port) << 8
			| (entry.forward_port); //�ڶ�����ַ��8λΪת���˿�
	//�м�16λΪUDP�˿ںţ�ǰ8λ��0
	nmac_write_data(entry.addr, 2, flow_entry_buf); //����nmacд����
	if (timeout_flag) {
		timeout_flag = 0;
		return 0;
	}
	printf("Add a new entry success!\n");
	return 1;
}

int Delete_flow_entry(int netmagic_id, u_int8_t entry_id) //�ɹ�����1����ʱ����0
{
	//ȡ��Ҫ������netmagic��mac��ַ��ip��ַ
	netmagic_net_ip = libnet_name2addr4(l, (&NM[netmagic_id])->netmagic_ip,
			LIBNET_DONT_RESOLVE);
	memcpy(&netmagic_mac, (&NM[netmagic_id])->netmagic_mac, 6);

	u_int32_t *flow_entry_buf;
	flow_entry_buf = (u_int32_t*) malloc(2 * sizeof(u_int32_t));
	flow_entry_buf[0] = 0; //ɾ������ȫ0����
	flow_entry_buf[1] = 0;
	nmac_write_data(0x14000000 + 2 * (entry_id - 1), 2, flow_entry_buf); //0x14000000+2*(entry_id -1)Ϊ�����ַ
	//0x14000000Ϊ���ʼ��ַ��ÿ������ռ2����ַ�ռ�
	if (timeout_flag) {
		timeout_flag = 0;
		return 0;
	}
	printf("Delete a new entry success!\n");
	return 1;
}
