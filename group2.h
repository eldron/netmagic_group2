#pragma once
#include "nmac.h"
#include "FFT.h"

#define FIBSIZE 8
#define ERASE_SLEEP_INTERVAL_MS 300
#define ERASE_THRESHOLD_S 30

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
#define PORT_ALL -1
#define PORT_CPU -2
	uint8_t id;
	uint8_t port;// bitmap
	Mac smac;
	Mac dmac;
	time_t ts; // timestamp
	enum Type type;
	uint8_t valid;// 1 valid, 0 not valid
	uint8_t item_type;
} FIBItem;

// Forwarding Information Base
typedef struct _FIB {
	FIBItem at[FIBSIZE];
} FIB;

void FIB_init();
uint8_t FIB_insert(FIBItem* item);
FIBItem* FIB_find(Mac mac, int type);
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
void HWFIB_insert(FIBItem* item);
void HWFIB_update(FIBItem* item);
void HWFIB_remove(FIBItem* item);

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
	fft.outport = item->port;// set output port
	// no need to set user_define

	//取得要操作的netmagic的mac地址，ip地址
	netmagic_net_ip = libnet_name2addr4(l, (&NM[selected_nid])->netmagic_ip,
			LIBNET_DONT_RESOLVE);
	memcpy(&netmagic_mac, (&NM[selected_nid])->netmagic_mac, 6);

	nmac_write_data(ITEM_BASE_ADDR + 16 * id, sizeof(FFT) / 4, (uint32_t *) &fft); //发送nmac写报文
	if (timeout_flag) {
		timeout_flag = 0;
		return 0;
	}
	printf("Written the item %d successfully!\n", item->id);
	return 1;
}

