/*..........初始化参数信息..........*/
/*..........获取controller的IP地址、MAC地址..........*/
/*..........配置bpf过滤规则..........*/
/*..........创建报文缓存队列..........*/
#include"nmac.h"

#undef ether_addr_octet

int contr_init() {
	eth_addr = (struct libnet_ether_addr *) malloc(
			sizeof(struct libnet_ether_addr)); //存放MAC地址
	//char *IP_string; //存放点分十进制IP字符串
	//IP_string = (char *) malloc(16 * sizeof(char));
	//int str_len1, str_len2, str_len3;
	char errbuf[LIBNET_ERRBUF_SIZE];
	char mac_tmp[18];
	//payload = (u_char*) malloc(1480 * sizeof(u_char)); //为NMAC报文数据部分分配内存
	//bpf_filter_string = (char *) malloc(100 * sizeof(char)); //为BPF过滤字符串分配内存100个字节

	//bpf_filter_string[0] = 'i'; //开始部分配置bpf规则
	//bpf_filter_string[1] = 'p'; //完整规则为目的IP和IP类型
	//bpf_filter_string[2] = ' '; //即ip dst "控制器ip" and ip proto 253
	//bpf_filter_string[3] = 'd';
	//bpf_filter_string[4] = 's';
	//bpf_filter_string[5] = 't';
	//bpf_filter_string[6] = ' ';

	adapter = pcap_lookupdev(errbuf); //获得网络接口名称
	if (adapter == NULL) {
		printf("%s\n", errbuf);
		printf("Local adapter not found!\n");
		return 0;
	} else {
		pcap_handle = pcap_open_live(adapter, BUFSIZ, 0, 2000, errbuf); //打开Libpcap句柄，1为混杂模式,2000毫秒超时
		l = libnet_init(LIBNET_LINK, adapter, errbuf); //打开libnet句柄

		ctrl_net_ip = libnet_get_ipaddr4(l); //获取本机32位网络字节序IP地址
		//将32位网络字节序地址转换为点分十进制地址
		//IP_string = libnet_addr2name4(ctrl_net_ip, LIBNET_DONT_RESOLVE);
		//str_len1 = strlen(IP_string);
		//memcpy(bpf_filter_string+7, IP_string, str_len1);
		//将本机IP地址复制到BPF过滤字符串中

		eth_addr = libnet_get_hwaddr(l); //获取本机的MAC地址
		int i, j;
		for (i = 0; i < 6; i++) {
			(&controller)->ctrl_mac[i] = eth_addr->ether_addr_octet[i];
		}

		char * string = " and ip proto 253"; //NMAC的IP类型为253
		//memcpy(bpf_filter_string+7+str_len1, string, strlen(string));
		//完成BPF字符串赋值

		//pcap_compile(pcap_handle, &bpf_filter_nmac, bpf_filter_string, 0,
				net_ip); //编译过滤规则
		pcap_compile(pcap_handle, &bpf_filter_nmac, "ip proto 253", 0, net_ip);
		pcap_setfilter(pcap_handle, &bpf_filter_nmac); //设置过滤规则
	}

	////建立消息队列
	////msgctl(32768, IPC_RMID, NULL);
	//msg_id = msgget(key, IPC_CREAT | 0x0666); //申请创建系统的消息队列
	//if (-1 == msg_id) {
	//	printf("Wrong!Creating failed, id=%d\n", msg_id);
	//} else {
	//	printf("Creating queqe successed! msg_id=%d\n", msg_id);
	//}
	init_mqueue(&mqueue);

	return 1;
}

