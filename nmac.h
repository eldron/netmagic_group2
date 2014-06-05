#pragma once
/*..............全局变量、函数定义.................*/
#include<stdio.h>
#include<stdlib.h>
#include<libnet.h>
#include<pcap.h>
#include<errno.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<netinet/if_ether.h>
#include<sys/time.h>
#include<string.h>
#include<unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include "mqueue.h"
#define PORT_ALL 0xFF00
#define PORT_CPU 0x0800

#define NMAC_PROTO 253 		//定义NMAC协议号253
#define BUFFER 366 			//读数据缓存大小366*4字节
u_int32_t * nmac_errbuf; //存放错误代码

key_t key; //消息队列的键值
#define key 3323 //33
int msg_id; //消息队列的ID

//消息队列中报文消息的缓冲区结构
//利用了系统中的消息队列，最好自己实现一个
//struct msgmbuf {
//	int mtype; /*消息类型，任意值，本例中为10*/
//	char mtext[1500]; /*消息内容，不超过1024字节*/
//};

////规则表项
//typedef struct flow_entry {
//	u_int8_t id; //表项id
//	u_int32_t addr; //表项起始地址
//	u_int32_t dst_ip; //目的ip
//	u_int16_t trans_port; //UDP端口号
//	u_int8_t forward_port; //转发端口号
//} FlowEntry;

////用单链表实现控制器内规则表项的维护
////单链表节点
//typedef struct node {
//	FlowEntry entry;
//	struct node *next;
//} Node, *Link;

//控制器设备数据结构，用来存放控制器地址信息
struct ctrl_dev {
	char ctrl_ip[16]; //点分十进制IP
	u_char ctrl_mac[6]; //十六进制MAC
};

//NetMagic设备数据结构
struct nmac_dev {
	int netmagic_id; //用此ID在初始化函数中标识设备
	char netmagic_ip[16];
	u_char netmagic_mac[6];
	//int Addr_flag[100]; //表项有效位标识符，假定可以存99个表项, Addr_flag[0]用来指示规则表是否为空
	//Link head; //规则流表头指针
};

//NMAC头部数据结构，按1字节长度对齐
#pragma pack(push)
#pragma pack(1)
struct nmac_header {
	u_int8_t count; //报文中包含的命令条目数
	u_int8_t reserve8_1; //保留置0
	u_int16_t seq; //16位报文序列号
	u_int16_t reserve16_2; //保留置0
	u_int8_t nmac_type; //NMAC命令类型
	u_int16_t parameter; //depend on the NMAC type
	u_int8_t reserve8_3; //保留置0
};
#pragma pack(pop)

//nmac报文命令类型
//还需定义新的报文类型
enum nmac_type {
	CONNECT = 1, //连接请求
	RELEASE, //连接释放
	READ_REQUEST, //写请求
	WRITE_REQUEST, //写响应
	READ_RESPONSE, //读请求
	WRITE_RESPONSE, //读响应
	ASY_MESSAGE, //异步消息
//C_2_NM_MESSAGE,
//NM_2_C_MESSAGE,
};
//define message type between netmagic and controller

//nmac异步消息类型
enum ASY_MESSAGE_TYPE {
	packet_in = 1, flow_mod, port_mod, error,
};

//struct msgmbuf pkt_buf; //入队缓存
//struct msgmbuf pkt_read_buf; //出队缓存
struct ctrl_dev controller; //定义ctrl_dev结构对象
struct nmac_dev NM[10]; //定义nmac_dev结构对象，实验假定不超过10个NetMagic

struct timeval start, end; //监测时间
struct nmac_header * nmac_hdr; //指向NMAC头的指针，分析捕获报文
struct nmac_header nmac_head; //NMAC头结构体
struct libnet_ether_addr *eth_addr; //分析捕获报文的以太网头
struct libnet_ipv4_hdr * ip_protocol; //分析捕获报文的IP头

int timeout_flag; //等待回应报文超时标志位
int halt_flag; //Pcap抓包停止标识位
int pause_flag; //暂停取报文标志位
int con_flag; //连接就绪标志位
int read_flag; //读响应就绪标志位
int write_flag; //写响应就绪标志位
//int asy_flag; //异步消息就绪标志位
//int packet_num; //缓存队列中当前报文数
int pt1_num;

char *con_pkt_buf; //连接报文缓存
char *read_pkt_buf; //读响应报文缓存
char *write_pkt_buf; //写响应报文缓存
char *asy_pkt_buf; //异步消息报文缓存
u_int32_t read_data_buf[BUFFER];

u_int32_t ctrl_net_ip; //Controller的32位网络字节序IP
u_int32_t netmagic_net_ip; //NetMagic的32位网络字节序IP
u_char netmagic_mac[6]; //NetMagic的48位16进制MAC地址
char * adapter; //网络接口名称
bpf_u_int32 net_mask; //网络掩码
bpf_u_int32 net_ip; //网络号

pcap_t* pcap_handle; //Pcap句柄
struct bpf_program bpf_filter_nmac; //BPF过滤规则

// edited by lijie
//char bpf_filter_string[100]; //BPF过滤字符串

char pcap_errbuf[PCAP_ERRBUF_SIZE];
libnet_t * l; //Libnet句柄
libnet_ptag_t ip_protocol_tag; //IP数据块协议标记
libnet_ptag_t ether_protocol_tag; // 以太网数据块协议标记

// 李杰改了。。定常数据，不需要malloc
u_char payload[1480]; //存放NMAC数据

u_int16_t payload_size; //NMAC负载大小
int packet_size; //发送的数据包大小

//以下为系统中定义的各种函数
int contr_init(); //初始化函数，配置参数
void nm_init(); //初始化NetMagic信息
void packet_capture(); //抓包函数
//void parsing(); //解析&分发报文函数
//void demo_test(); //演示UI
void console();
//int hextodec(char x); //将十六进制转化为十进制处理

//4个基本接口函数
int nmac_init(int netmagic_id); //初始化与netmagic的连接
void nmac_write_data(u_int32_t addr, int num, u_int32_t* data); //写数据

//规则表项操作函数
//int Add_flow_entry(int netmagic_id, FlowEntry entry); //新增流表项
//int Delete_flow_entry(int netmagic_id, u_int8_t entry_id); //删除流表项

//链表操作函数
//Link Creat_List(); //创造空单链表
//void Destroy_List(Link H); //销毁单链表
//void Clear_List(Link H); //清空单链表，置为空表
//int List_Empty(Link H); //判断链表是否为空表
//int Length_List(Link H); //求单链表长度
//Link Locate_List_Value(Link H, u_int8_t id); //按表项id查找
//void Delete_Elem(Link H, u_int8_t id); //按id删除表项
//void Add_Elem(
//		Link H, //新增流表项
//		u_int8_t id, u_int32_t addr, u_int32_t dst_ip, u_int16_t trans_port,
//		u_int8_t forward_port);


// added by Li Jie on 2014.5.27, set the value in demo_test.c, used by function hw_write_item in group2.h
// in fact it is 0
int selected_nid = 0;

// our message queue
MQueue mqueue;

int ERASE_THRESHOLD_S = 30;
int ERASE_SLEEP_INTERVAL_MS = 300;
