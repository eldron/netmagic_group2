#pragma once
/*..............ȫ�ֱ�������������.................*/
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

#define NMAC_PROTO 253 		//����NMACЭ���253
#define BUFFER 366 			//�����ݻ����С366*4�ֽ�
u_int32_t * nmac_errbuf; //��Ŵ������

key_t key; //��Ϣ���еļ�ֵ
#define key 3323 //33
int msg_id; //��Ϣ���е�ID

//��Ϣ�����б�����Ϣ�Ļ������ṹ
//������ϵͳ�е���Ϣ���У�����Լ�ʵ��һ��
//struct msgmbuf {
//	int mtype; /*��Ϣ���ͣ�����ֵ��������Ϊ10*/
//	char mtext[1500]; /*��Ϣ���ݣ�������1024�ֽ�*/
//};

////�������
//typedef struct flow_entry {
//	u_int8_t id; //����id
//	u_int32_t addr; //������ʼ��ַ
//	u_int32_t dst_ip; //Ŀ��ip
//	u_int16_t trans_port; //UDP�˿ں�
//	u_int8_t forward_port; //ת���˿ں�
//} FlowEntry;

////�õ�����ʵ�ֿ������ڹ�������ά��
////������ڵ�
//typedef struct node {
//	FlowEntry entry;
//	struct node *next;
//} Node, *Link;

//�������豸���ݽṹ��������ſ�������ַ��Ϣ
struct ctrl_dev {
	char ctrl_ip[16]; //���ʮ����IP
	u_char ctrl_mac[6]; //ʮ������MAC
};

//NetMagic�豸���ݽṹ
struct nmac_dev {
	int netmagic_id; //�ô�ID�ڳ�ʼ�������б�ʶ�豸
	char netmagic_ip[16];
	u_char netmagic_mac[6];
	//int Addr_flag[100]; //������Чλ��ʶ�����ٶ����Դ�99������, Addr_flag[0]����ָʾ������Ƿ�Ϊ��
	//Link head; //��������ͷָ��
};

//NMACͷ�����ݽṹ����1�ֽڳ��ȶ���
#pragma pack(push)
#pragma pack(1)
struct nmac_header {
	u_int8_t count; //�����а�����������Ŀ��
	u_int8_t reserve8_1; //������0
	u_int16_t seq; //16λ�������к�
	u_int16_t reserve16_2; //������0
	u_int8_t nmac_type; //NMAC��������
	u_int16_t parameter; //depend on the NMAC type
	u_int8_t reserve8_3; //������0
};
#pragma pack(pop)

//nmac������������
//���趨���µı�������
enum nmac_type {
	CONNECT = 1, //��������
	RELEASE, //�����ͷ�
	READ_REQUEST, //д����
	WRITE_REQUEST, //д��Ӧ
	READ_RESPONSE, //������
	WRITE_RESPONSE, //����Ӧ
	ASY_MESSAGE, //�첽��Ϣ
//C_2_NM_MESSAGE,
//NM_2_C_MESSAGE,
};
//define message type between netmagic and controller

//nmac�첽��Ϣ����
enum ASY_MESSAGE_TYPE {
	packet_in = 1, flow_mod, port_mod, error,
};

//struct msgmbuf pkt_buf; //��ӻ���
//struct msgmbuf pkt_read_buf; //���ӻ���
struct ctrl_dev controller; //����ctrl_dev�ṹ����
struct nmac_dev NM[10]; //����nmac_dev�ṹ����ʵ��ٶ�������10��NetMagic

struct timeval start, end; //���ʱ��
struct nmac_header * nmac_hdr; //ָ��NMACͷ��ָ�룬����������
struct nmac_header nmac_head; //NMACͷ�ṹ��
struct libnet_ether_addr *eth_addr; //���������ĵ���̫��ͷ
struct libnet_ipv4_hdr * ip_protocol; //���������ĵ�IPͷ

int timeout_flag; //�ȴ���Ӧ���ĳ�ʱ��־λ
int halt_flag; //Pcapץ��ֹͣ��ʶλ
int pause_flag; //��ͣȡ���ı�־λ
int con_flag; //���Ӿ�����־λ
int read_flag; //����Ӧ������־λ
int write_flag; //д��Ӧ������־λ
//int asy_flag; //�첽��Ϣ������־λ
//int packet_num; //��������е�ǰ������
int pt1_num;

char *con_pkt_buf; //���ӱ��Ļ���
char *read_pkt_buf; //����Ӧ���Ļ���
char *write_pkt_buf; //д��Ӧ���Ļ���
char *asy_pkt_buf; //�첽��Ϣ���Ļ���
u_int32_t read_data_buf[BUFFER];

u_int32_t ctrl_net_ip; //Controller��32λ�����ֽ���IP
u_int32_t netmagic_net_ip; //NetMagic��32λ�����ֽ���IP
u_char netmagic_mac[6]; //NetMagic��48λ16����MAC��ַ
char * adapter; //����ӿ�����
bpf_u_int32 net_mask; //��������
bpf_u_int32 net_ip; //�����

pcap_t* pcap_handle; //Pcap���
struct bpf_program bpf_filter_nmac; //BPF���˹���

// edited by lijie
//char bpf_filter_string[100]; //BPF�����ַ���

char pcap_errbuf[PCAP_ERRBUF_SIZE];
libnet_t * l; //Libnet���
libnet_ptag_t ip_protocol_tag; //IP���ݿ�Э����
libnet_ptag_t ether_protocol_tag; // ��̫�����ݿ�Э����

// ��ܸ��ˡ����������ݣ�����Ҫmalloc
u_char payload[1480]; //���NMAC����

u_int16_t payload_size; //NMAC���ش�С
int packet_size; //���͵����ݰ���С

//����Ϊϵͳ�ж���ĸ��ֺ���
int contr_init(); //��ʼ�����������ò���
void nm_init(); //��ʼ��NetMagic��Ϣ
void packet_capture(); //ץ������
//void parsing(); //����&�ַ����ĺ���
//void demo_test(); //��ʾUI
void console();
//int hextodec(char x); //��ʮ������ת��Ϊʮ���ƴ���

//4�������ӿں���
int nmac_init(int netmagic_id); //��ʼ����netmagic������
void nmac_write_data(u_int32_t addr, int num, u_int32_t* data); //д����

//��������������
//int Add_flow_entry(int netmagic_id, FlowEntry entry); //����������
//int Delete_flow_entry(int netmagic_id, u_int8_t entry_id); //ɾ��������

//�����������
//Link Creat_List(); //����յ�����
//void Destroy_List(Link H); //���ٵ�����
//void Clear_List(Link H); //��յ�������Ϊ�ձ�
//int List_Empty(Link H); //�ж������Ƿ�Ϊ�ձ�
//int Length_List(Link H); //��������
//Link Locate_List_Value(Link H, u_int8_t id); //������id����
//void Delete_Elem(Link H, u_int8_t id); //��idɾ������
//void Add_Elem(
//		Link H, //����������
//		u_int8_t id, u_int32_t addr, u_int32_t dst_ip, u_int16_t trans_port,
//		u_int8_t forward_port);


// added by Li Jie on 2014.5.27, set the value in demo_test.c, used by function hw_write_item in group2.h
// in fact it is 0
int selected_nid = 0;

// our message queue
MQueue mqueue;

int ERASE_THRESHOLD_S = 30;
int ERASE_SLEEP_INTERVAL_MS = 300;
