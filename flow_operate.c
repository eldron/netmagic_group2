/*..........新增&删除流表项..........*/
#include"nmac.h"

int Add_flow_entry(int netmagic_id, FlowEntry entry) //成功返回1，超时返回0
{
	//取得要操作的netmagic的mac地址，ip地址
	netmagic_net_ip = libnet_name2addr4(l, (&NM[netmagic_id])->netmagic_ip,
			LIBNET_DONT_RESOLVE);
	memcpy(&netmagic_mac, (&NM[netmagic_id])->netmagic_mac, 6);

	//构造nmac写报文中的nmac数据部分
	u_int32_t *flow_entry_buf;
	flow_entry_buf = (u_int32_t*) malloc(2 * sizeof(u_int32_t)); //申请2个地址空间，来存放一个表项
	flow_entry_buf[0] = htonl(entry.dst_ip); //第一个地址存目的ip
	flow_entry_buf[1] = ((u_int32_t) entry.trans_port) << 8
			| (entry.forward_port); //第二个地址后8位为转发端口
	//中间16位为UDP端口号，前8位补0
	nmac_write_data(entry.addr, 2, flow_entry_buf); //发送nmac写报文
	if (timeout_flag) {
		timeout_flag = 0;
		return 0;
	}
	printf("Add a new entry success!\n");
	return 1;
}

int Delete_flow_entry(int netmagic_id, u_int8_t entry_id) //成功返回1，超时返回0
{
	//取得要操作的netmagic的mac地址，ip地址
	netmagic_net_ip = libnet_name2addr4(l, (&NM[netmagic_id])->netmagic_ip,
			LIBNET_DONT_RESOLVE);
	memcpy(&netmagic_mac, (&NM[netmagic_id])->netmagic_mac, 6);

	u_int32_t *flow_entry_buf;
	flow_entry_buf = (u_int32_t*) malloc(2 * sizeof(u_int32_t));
	flow_entry_buf[0] = 0; //删除就用全0覆盖
	flow_entry_buf[1] = 0;
	nmac_write_data(0x14000000 + 2 * (entry_id - 1), 2, flow_entry_buf); //0x14000000+2*(entry_id -1)为表项地址
	//0x14000000为表初始地址，每个表项占2个地址空间
	if (timeout_flag) {
		timeout_flag = 0;
		return 0;
	}
	printf("Delete a new entry success!\n");
	return 1;
}
