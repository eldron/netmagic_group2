/*..........��ʼ��������Ϣ..........*/
/*..........��ȡcontroller��IP��ַ��MAC��ַ..........*/
/*..........����bpf���˹���..........*/
/*..........�������Ļ������..........*/
#include"nmac.h"

#undef ether_addr_octet

int contr_init() {
	eth_addr = (struct libnet_ether_addr *) malloc(
			sizeof(struct libnet_ether_addr)); //���MAC��ַ
	//char *IP_string; //��ŵ��ʮ����IP�ַ���
	//IP_string = (char *) malloc(16 * sizeof(char));
	//int str_len1, str_len2, str_len3;
	char errbuf[LIBNET_ERRBUF_SIZE];
	char mac_tmp[18];
	//payload = (u_char*) malloc(1480 * sizeof(u_char)); //ΪNMAC�������ݲ��ַ����ڴ�
	//bpf_filter_string = (char *) malloc(100 * sizeof(char)); //ΪBPF�����ַ��������ڴ�100���ֽ�

	//bpf_filter_string[0] = 'i'; //��ʼ��������bpf����
	//bpf_filter_string[1] = 'p'; //��������ΪĿ��IP��IP����
	//bpf_filter_string[2] = ' '; //��ip dst "������ip" and ip proto 253
	//bpf_filter_string[3] = 'd';
	//bpf_filter_string[4] = 's';
	//bpf_filter_string[5] = 't';
	//bpf_filter_string[6] = ' ';

	adapter = pcap_lookupdev(errbuf); //�������ӿ�����
	if (adapter == NULL) {
		printf("%s\n", errbuf);
		printf("Local adapter not found!\n");
		return 0;
	} else {
		pcap_handle = pcap_open_live(adapter, BUFSIZ, 0, 2000, errbuf); //��Libpcap�����1Ϊ����ģʽ,2000���볬ʱ
		l = libnet_init(LIBNET_LINK, adapter, errbuf); //��libnet���

		ctrl_net_ip = libnet_get_ipaddr4(l); //��ȡ����32λ�����ֽ���IP��ַ
		//��32λ�����ֽ����ַת��Ϊ���ʮ���Ƶ�ַ
		//IP_string = libnet_addr2name4(ctrl_net_ip, LIBNET_DONT_RESOLVE);
		//str_len1 = strlen(IP_string);
		//memcpy(bpf_filter_string+7, IP_string, str_len1);
		//������IP��ַ���Ƶ�BPF�����ַ�����

		eth_addr = libnet_get_hwaddr(l); //��ȡ������MAC��ַ
		int i, j;
		for (i = 0; i < 6; i++) {
			(&controller)->ctrl_mac[i] = eth_addr->ether_addr_octet[i];
		}

		char * string = " and ip proto 253"; //NMAC��IP����Ϊ253
		//memcpy(bpf_filter_string+7+str_len1, string, strlen(string));
		//���BPF�ַ�����ֵ

		//pcap_compile(pcap_handle, &bpf_filter_nmac, bpf_filter_string, 0,
				net_ip); //������˹���
		pcap_compile(pcap_handle, &bpf_filter_nmac, "ip proto 253", 0, net_ip);
		pcap_setfilter(pcap_handle, &bpf_filter_nmac); //���ù��˹���
	}

	////������Ϣ����
	////msgctl(32768, IPC_RMID, NULL);
	//msg_id = msgget(key, IPC_CREAT | 0x0666); //���봴��ϵͳ����Ϣ����
	//if (-1 == msg_id) {
	//	printf("Wrong!Creating failed, id=%d\n", msg_id);
	//} else {
	//	printf("Creating queqe successed! msg_id=%d\n", msg_id);
	//}
	init_mqueue(&mqueue);

	return 1;
}

