/*..........demo演示程序、用户交互UI..........*/

#include"nmac.h"

void demo_test() {
	int i, nid, exit_flag, flag;
	char *IP_string; //存放点分十进制IP字符串
	IP_string = (char *) malloc(16 * sizeof(char));
	nm_init(); //配置netmagic的ip，mac
	if (!nmac_init(0)) {
		halt_flag = 1;
		return;
	}
	//打印netmagic表
	printf("\n");
	printf("-----------The Connected NetMagic List----------\n");
	printf("  ID         IP Adress             MAC Adress     \n");
	printf("   %d      %s      ", (&NM[0])->netmagic_id, (&NM[0])->netmagic_ip);
	for (i = 0; i < 5; i++)
		printf("%02x:", (&NM[0])->netmagic_mac[i]);
	printf("%02x\n", (&NM[0])->netmagic_mac[5]);
	printf("\n");

	//选择netmagic
	printf("Please Select the NetMagic ID (0 to exit!): \n");
	scanf("\n%d", &nid);
	if (nid == 0) {
		halt_flag = 1; //停止抓包
		printf("exit!\n");
	} else {
		nid = nid - 1; //netmagic编号从0开始
		selected_nid = nid;
		//printf("nid: %d\n",nid);
		while (1) {
			if (exit_flag == 1)
				break;
			else if (!NM[nid].Addr_flag[0]) //规则表为空
				printf("*****There is no flow entry now!*****\n");
			else {
				//打印已有表项
				printf("\n");
				printf("-----------The Flow Table----------\n");
				printf(
						"ID        Dst_ip         Trans_port   Forward_port     \n");
				Link p = (&NM[0])->head;
				p = p->next;
				i = 1;
				while (p) {
					IP_string = libnet_addr2name4(p->entry.dst_ip,
							LIBNET_DONT_RESOLVE);
					printf(" %d   %s       %d          %d\n", i, IP_string,
							p->entry.trans_port, p->entry.forward_port);
					p = p->next;
					i++;
				}
			}

			printf("\n");
			printf("1:Add a new flow entry;\n");
			printf("2:Delete a flow entry;\n");
			printf("3:Modify a flow entry;\n");
			printf("0:Exit.\n");
			//选择操作
			printf("Enter Your Choice:");
			int opt;
			scanf("\n%d", &opt);
			switch (opt) {
			case 0: //退出
			{
				halt_flag = 1; //停止抓包
				//msgctl(msg_id, IPC_RMID, NULL);//删除队列
				printf("exit!\n");
				exit_flag = 1;
				break;
			}
			case 1: //增加表项
			{
				char ip_addr[16];
				u_int16_t port_1;
				u_int8_t port_2;
				u_int32_t ip, addr;
				printf("Please enter the new Entry: \n");

				//获取输入的表项
				scanf("\n%s", ip_addr);
				char dst_ip[16];
				memcpy(dst_ip, ip_addr, 16);
				scanf("%d", &port_1);
				u_int16_t trans_port = port_1;
				scanf("%d", &port_2);
				u_int8_t forward_port = port_2;

				//寻找未占用地址
				i = 1;
				while (NM[nid].Addr_flag[i]) //未做表满处理!!!
				{
					i++; //i不能超过99!!!
				}

				printf("The new entry id is:%d\n", i);
				addr = 0x14000000 + 2 * (i - 1); //表项id是和地址绑定在一起的

				ip = libnet_name2addr4(l, dst_ip, LIBNET_DONT_RESOLVE); //转化为32位网络地址
				//printf("The ip is:%x\n", ip);

				//构造新表项
				FlowEntry new_entry;
				new_entry.id = i;
				new_entry.addr = addr;
				new_entry.dst_ip = ip;
				new_entry.trans_port = trans_port;
				new_entry.forward_port = forward_port;

				flag = Add_flow_entry(nid, new_entry); //增加表项
				if (flag) //成功，则更新控制器内部规则表
				{
					Add_Elem((&NM[nid])->head, i, addr, ip, trans_port,
							forward_port);
					printf("-----add----\n");
					NM[nid].Addr_flag[0]++;
					NM[nid].Addr_flag[i] = 1;
					break;
				} else if (!flag)
					break;
			}
			case 2: //删除表项
			{
				printf("Please enter the entry id: \n");
				u_int8_t list_id, id;
				scanf("\n%d", &list_id); //list_id为显示在终端上的表项编号
				id = Locate_list_Pos((&NM[nid])->head, list_id); //id为表项的属性，与地址相关联
				flag = Delete_flow_entry(nid, id); //删除表项
				if (flag) //成功，则更新控制器内部规则表
				{
					Delete_Elem((&NM[nid])->head, id);
					NM[nid].Addr_flag[0]--;
					NM[nid].Addr_flag[id] = 0;
					break;
				} else if (!flag)
					break;
			}

			case 3: //未实现
			{
				printf("Sorry...\n");
				break;
			}
			}
		}
	}
}
