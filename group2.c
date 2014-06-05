#include "group2.h"

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

	// 根据smac和sport更新或者添加表项
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

	// 写入第二类和第三类表项
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
