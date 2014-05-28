/*..........demo��ʾ�����û�����UI..........*/

#include"nmac.h"

void demo_test() {
	int i, nid, exit_flag, flag;
	char *IP_string; //��ŵ��ʮ����IP�ַ���
	IP_string = (char *) malloc(16 * sizeof(char));
	nm_init(); //����netmagic��ip��mac
	if (!nmac_init(0)) {
		halt_flag = 1;
		return;
	}
	//��ӡnetmagic��
	printf("\n");
	printf("-----------The Connected NetMagic List----------\n");
	printf("  ID         IP Adress             MAC Adress     \n");
	printf("   %d      %s      ", (&NM[0])->netmagic_id, (&NM[0])->netmagic_ip);
	for (i = 0; i < 5; i++)
		printf("%02x:", (&NM[0])->netmagic_mac[i]);
	printf("%02x\n", (&NM[0])->netmagic_mac[5]);
	printf("\n");

	//ѡ��netmagic
	printf("Please Select the NetMagic ID (0 to exit!): \n");
	scanf("\n%d", &nid);
	if (nid == 0) {
		halt_flag = 1; //ֹͣץ��
		printf("exit!\n");
	} else {
		nid = nid - 1; //netmagic��Ŵ�0��ʼ
		selected_nid = nid;
		//printf("nid: %d\n",nid);
		while (1) {
			if (exit_flag == 1)
				break;
			else if (!NM[nid].Addr_flag[0]) //�����Ϊ��
				printf("*****There is no flow entry now!*****\n");
			else {
				//��ӡ���б���
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
			//ѡ�����
			printf("Enter Your Choice:");
			int opt;
			scanf("\n%d", &opt);
			switch (opt) {
			case 0: //�˳�
			{
				halt_flag = 1; //ֹͣץ��
				//msgctl(msg_id, IPC_RMID, NULL);//ɾ������
				printf("exit!\n");
				exit_flag = 1;
				break;
			}
			case 1: //���ӱ���
			{
				char ip_addr[16];
				u_int16_t port_1;
				u_int8_t port_2;
				u_int32_t ip, addr;
				printf("Please enter the new Entry: \n");

				//��ȡ����ı���
				scanf("\n%s", ip_addr);
				char dst_ip[16];
				memcpy(dst_ip, ip_addr, 16);
				scanf("%d", &port_1);
				u_int16_t trans_port = port_1;
				scanf("%d", &port_2);
				u_int8_t forward_port = port_2;

				//Ѱ��δռ�õ�ַ
				i = 1;
				while (NM[nid].Addr_flag[i]) //δ����������!!!
				{
					i++; //i���ܳ���99!!!
				}

				printf("The new entry id is:%d\n", i);
				addr = 0x14000000 + 2 * (i - 1); //����id�Ǻ͵�ַ����һ���

				ip = libnet_name2addr4(l, dst_ip, LIBNET_DONT_RESOLVE); //ת��Ϊ32λ�����ַ
				//printf("The ip is:%x\n", ip);

				//�����±���
				FlowEntry new_entry;
				new_entry.id = i;
				new_entry.addr = addr;
				new_entry.dst_ip = ip;
				new_entry.trans_port = trans_port;
				new_entry.forward_port = forward_port;

				flag = Add_flow_entry(nid, new_entry); //���ӱ���
				if (flag) //�ɹ�������¿������ڲ������
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
			case 2: //ɾ������
			{
				printf("Please enter the entry id: \n");
				u_int8_t list_id, id;
				scanf("\n%d", &list_id); //list_idΪ��ʾ���ն��ϵı�����
				id = Locate_list_Pos((&NM[nid])->head, list_id); //idΪ��������ԣ����ַ�����
				flag = Delete_flow_entry(nid, id); //ɾ������
				if (flag) //�ɹ�������¿������ڲ������
				{
					Delete_Elem((&NM[nid])->head, id);
					NM[nid].Addr_flag[0]--;
					NM[nid].Addr_flag[id] = 0;
					break;
				} else if (!flag)
					break;
			}

			case 3: //δʵ��
			{
				printf("Sorry...\n");
				break;
			}
			}
		}
	}
}
