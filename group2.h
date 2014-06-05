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

int mac_equals(Mac * a, Mac * b);
int id_to_priority(uint8_t id);
uint8_t pri_to_id(int pri);

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

void binary_print_byte(uint8_t value);
char dec_to_hex(uint8_t value);
uint8_t cal_byte_value(char * s);
void cal_mac(char * s, Mac * mac);
void hex_print_decimal(uint8_t value);
void print_item(FIBItem * item);

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
int hw_write_item(FIBItem * item);
