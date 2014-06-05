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

// Ӳ���б�������ȼ�����ֵ��������ȼ�
// ��д����������ʱ����Ҫ����д�������ȼ��ĵ�ַȥ
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

	//ȡ��Ҫ������netmagic��mac��ַ��ip��ַ
	netmagic_net_ip = libnet_name2addr4(l, (&NM[selected_nid])->netmagic_ip,
			LIBNET_DONT_RESOLVE);
	memcpy(&netmagic_mac, (&NM[selected_nid])->netmagic_mac, 6);

	nmac_write_data(ITEM_BASE_ADDR + 16 * item->id, sizeof(FFT) / 4, (uint32_t *) &fft); //����nmacд����
	if (timeout_flag) {
		timeout_flag = 0;
		return 0;
	}
	printf("Written the item %d successfully!\n", item->id);
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
}

// ���ݱ����item_typeд������Ϊ���������Ӹ����ȼ��ĵ�ַ��ʼ���ҿ�ȱλ��д�룬ɾ���������ʱ�������һ���滻����Ҫ1��2��Ӳ��д����
// ��Ϊ��һ��͵ڶ������ӵ����ȼ��ĵ�ַ��ʼ���ҿ�ȱλ��д�룬ɾ��ͬ��
// �Ѿ������Ӳ�����д��������֤Ӳ��������������ͬ��
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
			// û��λ���ˣ�д��0
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
		sleep(ERASE_SLEEP_INTERVAL_MS);
	}
}

void onPacketArrival(PacketDigest * d){
	// delete ITEM_1
	FIBItem * item = FIB_find(d->smac, SMAC);
	if(item){
		FIB_delete(item);
	}

	// ����smac��sport���»�����ӱ���
	item = FIB_find(d->smac, DMAC);
	if(item){
		// update
		if(item->port != d->sport){
			item->port = d->sport;
			item->ts = time(0);
			hw_write_item(item);
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

	// д��ڶ���͵��������
	item = FIB_find(d->dmac, DMAC);
	if(!item){
		FIBItem x;
		uint8_t all = 0;
		all = ~all;
		uint8_t cpu = 8;
		x.port = all & (~cpu);// set port to all, except cpu port and d->sport
		x.port = x.port & (~d->sport);
		x.dmac = d->dmac;
		x.ts = time(0);
		x.type = DMAC;
		x.valid = 1;
		x.item_type = ITEM_2;
		FIB_insert(&x);

		x.port = 8;// set to cup port
		x.smac = d->dmac;
		x.ts = time(0);
		x.type = SMAC;
		x.valid = 1;
		x.item_type = ITEM_3;
		FIB_insert(&x);
	}
}
