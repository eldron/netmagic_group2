#include "group2.h"

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

// print a byte in binary form
void binary_print_byte(FILE * file, uint8_t value){
	uint8_t tmp = 1;
	int i = 7;
	for(i = 7;i >= 0;i--){
		if(value & (tmp << i)){
			fprintf(file, "1");
		} else {
			fprintf(file, "0");
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
void hex_print_decimal(FILE * file, uint8_t value){
	fprintf(file, "%c", dec_to_hex(value / 16));
	fprintf(file, "%c", dec_to_hex(value % 16));
}

// print an item, if valid
void print_item(FIBItem * item){
	if(item && item->valid){
		printf("id:%d\t", item->id);
		if(item->type == SMAC){
			printf("SMAC: ");
			int i = 0;
			for(i = 0;i < 5;i++){
				hex_print_decimal(stdout, item->smac.mac[i]);
				printf(".");
			}
			hex_print_decimal(stdout, item->smac.mac[5]);
			printf("\t");
		} else {
			printf("DMAC: ");
			int i = 0;
			for(i = 0;i < 5;i++){
				hex_print_decimal(stdout, item->dmac.mac[i]);
				printf(".");
			}
			hex_print_decimal(stdout, item->dmac.mac[5]);
			printf("\t");
		}
		printf("port: ");
		binary_print_byte(stdout, item->port);
		printf("\n");
	}
}

void print_item_to_file(FILE * file, FIBItem * item){
	if(item && item->valid){
		fprintf(file, "id:%d\t", item->id);
		if(item->type == SMAC){
			fprintf(file, "SMAC: ");
			int i = 0;
			for(i = 0;i < 5;i++){
				hex_print_decimal(file, item->smac.mac[i]);
				fprintf(file, ".");
			}
			hex_print_decimal(file, item->smac.mac[5]);
			fprintf(file, "\t");
		} else {
			fprintf(file, "DMAC: ");
			int i = 0;
			for(i = 0;i < 5;i++){
				hex_print_decimal(file, item->dmac.mac[i]);
				fprintf(file, ".");
			}
			hex_print_decimal(file, item->dmac.mac[5]);
			fprintf(file, "\t");
		}
		fprintf(file, "port: ");
		binary_print_byte(file, item->port);
		fprintf(file, "\n");
	}
}
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
	//fft.base_action = 1;// set base_action
	// no need to set postprocessor_bitmap
	fft.output = item->port;// set output port
	// set user_define, if ITEM_3
	if(item->item_type == ITEM_3){
		fft.user_define[0] = 1;
		fft.user_define[0] = fft.user_define[0] << 7;
	}

	//取得要操作的netmagic的mac地址，ip地址
	netmagic_net_ip = libnet_name2addr4(l, (&NM[selected_nid])->netmagic_ip,
			LIBNET_DONT_RESOLVE);
	memcpy(&netmagic_mac, (&NM[selected_nid])->netmagic_mac, 6);

	//nmac_write_data(ITEM_BASE_ADDR + 16 * item->id, sizeof(FFT) / 4, (uint32_t *) &fft); //发送nmac写报文
	Data data;
	data.addr = ITEM_BASE_ADDR + 16 * item->id;
	data.length = sizeof(FFT);
	memcpy(data.content, (uint8_t *) (&fft), sizeof(FFT));
	enqueue(&data_queue, &data);
#ifdef DEBUG
	printf("enqueued to data queue, %d in data queue\n", data_queue.count);
#endif
	sleep(TIME_OUT_INTERVAL_MS / 1000);
	if (timeout_flag) {
		timeout_flag = 0;
		return 0;
	}
	
#ifdef DEBUG
	printf("Written the item %d successfully! item type = %d\n", item->id, item->item_type);
	print_item_to_file(stderr, item);
#endif
	return 1;
}
///////////////////////////////////////////////////////////////////////////////
//static FIB g_fib;
void FIB_init() {
	memset(&g_fib, 0, sizeof(g_fib));
	int i = 0;
	for(i = 0;i < FIBSIZE;i++){
		g_fib.at[i].id = i;
		hw_write_item(&(g_fib.at[i]));
	}
	//为广播地址插入一条表项
	FIBItem x;
	x.port = 0;
	x.port = ~x.port;
	for(i = 0;i < 6;i++){
		x.dmac.mac[i] = 0;
		x.dmac.mac[i] = ~x.dmac.mac[i];
	}
	x.ts = time(0);
	x.type = DMAC;
	x.valid = 1;
	x.item_type = ITEM_1;
	FIB_insert(&x);
}

// 根据表项的item_type写入表项，若为第三类表项，从高优先级的地址开始查找空缺位置写入，删除此类表项时，用最后一项替换，需要1或2次硬件写操作
// 若为第一类和第二类表项，从低优先级的地址开始查找空缺位置写入，删除同上
// 已经结合了硬件表的写操作，保证硬件表和软件表总是同步
uint8_t FIB_insert(FIBItem * item){
	uint8_t id;
	int flag = 0;

	if(item->item_type == ITEM_3){
		int i;
		for(i = 7;i >= 0; --i){
			uint8_t tmp_id = pri_to_id(i);
			if(!g_fib.at[tmp_id].valid){
				// found
				flag = 1;
				id = tmp_id;
				break;
			}
		}
		
		if(!flag){
			// 没空位置了，写到0
			id = 0;
		}
	} else {
		int i;
		for(i = 0;i < 8; ++i){
			uint8_t tmp_id = pri_to_id(i);
			if(!g_fib.at[tmp_id].valid){
				// found
				flag = 1;
				id = tmp_id;
				break;
			}
		}

		if(!flag){
			id = 7;
		}
	}

	item->id = id;
	item->ts = time(0);
	memcpy(&g_fib.at[id], item, sizeof(*item));
	hw_write_item(item);
	return id;
}

FIBItem* FIB_find(Mac mac, int type) {
	size_t i;
	for (i = 0; i < FIBSIZE; ++i) {
		FIBItem* x = &g_fib.at[i];
		if (type == DMAC) {
			if (mac_equals(&(x->dmac), &mac)) {
				return x;
			}
		} else if (type == SMAC) {
			if (mac_equals(&(x->smac), &mac)) {
				return x;
			}
		}
	}
	return 0;
}

FIBItem * FIB_find_by_port(uint8_t value){
	int i = 0;
	for(i = 0;i < FIBSIZE;i++){
		if(g_fib.at[i].valid && g_fib.at[i].port == value){
			return &(g_fib.at[i]);
		}
	}
	
	return NULL;
}

void FIB_delete(FIBItem * item){
	uint8_t original_id = item->id;
	uint8_t original_item_type = item->item_type;
	int original_pri;
	int pri;
	if(original_item_type == ITEM_3){
		original_pri = id_to_priority(original_id);
		pri = original_pri;
		while(1){
			if(g_fib.at[pri_to_id(pri - 1)].valid){
				--pri;
			}
			if(pri == 0){
				break;
			}
		}
	} else {
		original_pri = id_to_priority(original_id);
		pri = original_pri;
		while(1){
			if(g_fib.at[pri_to_id(pri + 1)].valid){
				++pri;
			}
			if(pri == 7){
				break;
			}
		}
	}

	if(original_pri == pri){
		item->valid = 0;
		hw_write_item(item);
		memset(&g_fib.at[original_id], 0, sizeof(*item));
	} else {
		FIBItem tmp = g_fib.at[pri_to_id(pri)];
		tmp.id = original_id;
		hw_write_item(&tmp);
		tmp.id = pri_to_id(pri);
		tmp.valid = 0;
		hw_write_item(&tmp);
		
		tmp = g_fib.at[pri_to_id(pri)];
		tmp.id = original_id;
		g_fib.at[original_id] = tmp;
		memset(&g_fib.at[pri_to_id(pri)], 0, sizeof(*item));
	}
#ifdef DEBUG
	printf("deleted an item, id = %d, type = %d, pri = %d\n", original_id, original_item_type, original_pri);
#endif
}

void eraseThread(void * p){
	int i;
	time_t ts;
	while(!halt_flag){
		ts = time(0);
		for(i = 0;i < FIBSIZE; ++i){
			if(g_fib.at[i].valid && g_fib.at[i].type == DMAC){
				if(ts > g_fib.at[i].ts + ERASE_THRESHOLD_S){
					FIB_delete(&g_fib.at[i]);
				}
			}
		}
		sleep(ERASE_SLEEP_INTERVAL_MS / 1000);
	}
}

void print_packet_digest(FILE * file, PacketDigest * d){
	fprintf(file, "DMAC: ");
	int i = 0;
	for(i = 0;i < 6;i++){
		hex_print_decimal(file, d->dmac.mac[i]);
		fprintf(file, ":");
	}
	fprintf(file, "\t SMAC: ");
	for(i = 0;i < 6;i++){
		hex_print_decimal(file, d->smac.mac[i]);
		fprintf(file, ":");
	}
	fprintf(file, "\t S_PORT: ");
	binary_print_byte(file, d->sport);
	fprintf(file, "\n");
}

void onPacketArrival(PacketDigest * d){
	// delete ITEM_3
	FIBItem * item = FIB_find(d->smac, SMAC);
	if(item){
		FIB_delete(item);
	}

	// 根据smac和sport更新或者添加表项
	item = FIB_find(d->smac, DMAC);
	if(item){
		// update
		if(item->port != d->sport){
			item->port = d->sport;
			item->ts = time(0);
			hw_write_item(item);
		} else {
#ifdef DEBUG
			printf("no need to update ITEM 1\n");
#endif
		}
	} else {
		FIBItem x;
		x.port = d->sport;
		x.dmac = d->smac;
		x.ts = time(0);
		x.type = DMAC;
		x.valid = 1;
		x.item_type = ITEM_1;
		FIB_insert(&x);
	}

	// 忽略广播包，目的mac地址全F
	Mac tmpmac;
	int i = 0;
	for(i = 0;i < 6;i++){
		tmpmac.mac[i] = 0;
		tmpmac.mac[i] = ~tmpmac.mac[i];
	}
	if(mac_equals(&tmpmac, &(d->dmac))){
#ifdef DEBUG
		printf("ignore broadcast packet in onPacketArrival\n");
#endif
		return;
	}
	
	// 写入第二类和第三类表项
	item = FIB_find(d->dmac, DMAC);
	if(!item){
		FIBItem x;
		uint8_t all = 0;
		all = ~all;
		//uint8_t cpu = 1;
		x.port = all;// set port to all, except d->sport
		x.port = x.port & (~d->sport);
		x.dmac = d->dmac;
		x.ts = time(0);
		x.type = DMAC;
		x.valid = 1;
		x.item_type = ITEM_2;
		FIB_insert(&x);

		x.port = 1;// set to cup port
		x.smac = d->dmac;
		x.ts = time(0);
		x.type = SMAC;
		x.valid = 1;
		x.item_type = ITEM_3;
		FIB_insert(&x);
	}else {
#ifdef DEBUG
		printf("no need to insert ITEM 2 and ITEM 3 in onPacketArrival\n");
#endif
	}
}
