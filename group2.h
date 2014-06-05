#pragma once
#include "nmac.h"
#include "FFT.h"

#define FIBSIZE 8
//#define ERASE_SLEEP_INTERVAL_MS 300
//#define ERASE_THRESHOLD_S 30

#define ITEM_1 100
#define ITEM_2 101
#define ITEM_3 102

// the hard ware address for the item 0
// addresses for item 1 through 7 are 0x14000010, 0x14000020, ... , 0x14000070 respectively
#define ITEM_BASE_ADDR 0x14000000

//typedef uint32_t Mac;
typedef struct {
	uint8_t mac[6];
} Mac;

// return 1 if two Mac addresses are equal, return 0 otherwise
int mac_equals(Mac * a, Mac * b){
	int i = 0;
	for(i = 0;i < 6;i++){
		if(a->mac[i] != b->mac[i]){
			return 0;
		}
	}
	return 1;
}

// 硬件中表项的优先级，大值代表高优先级
// 在写第三类表项的时候，需要将它写到高优先级的地址去
int id_to_priority(uint8_t id){
	int a[8];
	a[0] = 7;
	a[4] = 6;
	a[1] = 5;
	a[5] = 4;
	a[2] = 3;
	a[6] = 2;
	a[3] = 1;
	a[7] = 0;

	return a[id];
}

uint8_t pri_to_id(int pri){
	uint8_t i;
	uint8_t a[8];
	a[7] = 0;
	a[6] = 4;
	a[5] = 1;
	a[4] = 5;
	a[3] = 2;
	a[2] = 6;
	a[1] = 3;
	a[0] = 7;

	return a[pri];
}

enum Type {
		SMAC, DMAC,
	};

typedef struct _FIBItem {
	uint8_t id;
	uint8_t port;// bitmap
	Mac smac;
	Mac dmac;
	time_t ts; // timestamp
	enum Type type;
	uint8_t valid;// 1 valid, 0 not valid
	uint8_t item_type;
} FIBItem;

// print a byte in binary form
void binary_print_byte(uint8_t value){
	uint8_t tmp = 1;
	int i = 7;
	for(i = 7;i >= 0;i--){
		if(value & (tmp << i)){
			printf("1");
		} else {
			printf("0");
		}
	}
}

char dec_to_hex(uint8_t value){
	if(value <= 9){
		return '0' + value;
	} else {
		return 'A' + value - 10;
	}
}

uint8_t cal_byte_value(char * s){
	uint8_t value = 0;
	uint8_t one = 1;
	int i = 0;
	for(i = 0;i < 8;i++){
		if(s[i] == '1'){
			value += one << (7 - i);
		}
	}

	return value;
}

// 0 1  3 4  6 7  9 10  12 13  15 16
void cal_mac(char * s, Mac * mac){
	int i;
	for(i = 0;i < 6;i++){
		uint8_t value = 0;
		char c = s[3 * i];
		if('0' <= c && c <= '9'){
			value += 16 * (c - '0');
		} else if('a' <= c && c <= 'f'){
			value += 16 * (c - 'a' + 10);
		} else {
			value += 16 * (c - 'A' + 10);
		}
		
		c = s[3 * i + 1];
		if('0' <= c && c <= '9'){
			value += c - '0';
		} else if('a' <= c && c <= 'f'){
			value += c - 'a';
		} else {
			value += c - 'A';
		}

		mac->mac[i] = value;
	}
}

// print a decimal value in hex form
void hex_print_decimal(uint8_t value){
	printf("%c", dec_to_hex(value / 16));
	printf("%c", dec_to_hex(value % 16));
}

// print an item, if valid
void print_item(FIBItem * item){
	if(item && item->valid){
		printf("id:%d\t", item->id);
		if(item->type == SMAC){
			printf("SMAC: ");
			int i = 0;
			for(i = 0;i < 5;i++){
				hex_print_decimal(item->smac.mac[i]);
				printf(".");
			}
			hex_print_decimal(item->smac.mac[5]);
			printf("\t");
		} else {
			printf("DMAC: ");
			int i = 0;
			for(i = 0;i < 5;i++){
				hex_print_decimal(item->dmac.mac[i]);
				printf(".");
			}
			hex_print_decimal(item->dmac.mac[5]);
			printf("\t");
		}
		printf("port: ");
		binary_print_byte(item->port);
		printf("\n");
	}
}

// Forwarding Information Base
typedef struct _FIB {
	FIBItem at[FIBSIZE];
} FIB;

FIB g_fib;

void FIB_init();
uint8_t FIB_insert(FIBItem* item);
FIBItem* FIB_find(Mac mac, int type);
FIBItem * FIB_find_by_port(uint8_t value);
void FIB_delete(FIBItem * item);

///////////////////////////////////////////////////////////////////////////////
// Software
void eraseThread(void* p);

typedef struct _PacketDigest {
	Mac dmac;
	Mac smac;
	uint8_t sport;// bitmap
} PacketDigest;

void onPacketArrival(PacketDigest* d);
///////////////////////////////////////////////////////////////////////////////
// Hardware communication interfaces
//void HWFIB_insert(FIBItem* item);
//void HWFIB_update(FIBItem* item);
//void HWFIB_remove(FIBItem* item);

// write an item into netmagic
// the id in FIBItem represents the hardware address in netmagic
// return 1 if successful, return 0 instead
int hw_write_item(FIBItem * item){
	FFT fft;
	memset(&fft, 0, sizeof(FFT));
	memcpy(fft.d_mac, (uint8_t *)(&item->dmac), 6);// set dmac, assume already in network endian
	memcpy(fft.s_mac, (uint8_t *)(&item->smac), 6);// set smac, assume already in network endian
	// set mask
	int i = 0;
	if(item->type == SMAC){
		for(i = 0;i < 6;i++){
			fft.smac_mask[i] = ~(fft.smac_mask[i]);
		}
	} else {
		for(i = 0;i < 6;i++){
			fft.dmac_mask[i] = ~(fft.dmac_mask[i]);
		}
	}
	fft.valid = item->valid;// set valid
	fft.base_action = 1;// set base_action
	// no need to set postprocessor_bitmap
	fft.output = item->port;// set output port
	// no need to set user_define

	//取得要操作的netmagic的mac地址，ip地址
	netmagic_net_ip = libnet_name2addr4(l, (&NM[selected_nid])->netmagic_ip,
			LIBNET_DONT_RESOLVE);
	memcpy(&netmagic_mac, (&NM[selected_nid])->netmagic_mac, 6);

	nmac_write_data(ITEM_BASE_ADDR + 16 * item->id, sizeof(FFT) / 4, (uint32_t *) &fft); //发送nmac写报文
	if (timeout_flag) {
		timeout_flag = 0;
		return 0;
	}
	printf("Written the item %d successfully!\n", item->id);
	return 1;
}
