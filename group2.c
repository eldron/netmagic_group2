#include "group2.h"

///////////////////////////////////////////////////////////////////////////////
static FIB g_fib;
void FIB_init() {
	memset(&g_fib, 0, sizeof(g_fib));
}

//uint8_t FIB_insert(FIBItem* item) {
//	time_t ts = -1;
//	size_t i;
//	uint8_t id = 0;
//	for (i = 0; i < FIBSIZE; ++i) {
//		FIBItem* x = &g_fib.at[i];
//		if (!x->ts) {
//			id = i;
//			break;
//		}
//
//		if (x->ts <= ts) {
//			id = i;
//		}
//	}
//	item->id = id;
//	item->ts = time(0);
//	memcpy(&g_fib.at[id], item, sizeof(*item));
//	return id;
//}

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
			if (x->dmac == mac) {
				return x;
			}
		} else if (type == SMAC) {
			if (x->smac == mac) {
				return x;
			}
		}
	}
	return 0;
}

void FIB_delete(FIBItem * item){
	uint8_t original_id = item->id;
	uint8_t original_item_type = item->item_type;
	
	if(original_item_type == ITEM_3){
		int original_pri = id_to_priority(original_id);
		int pri = original_pri;
		while(1){
			if(g_fib.at[pri_to_id(pri - 1)].valid){
				--pri;
			}
			if(pri == 0){
				break;
			}
		}
	} else {
		int original_pri = id_to_priority(original_id);
		int pri = original_pri;
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

/////////////////////////////////////////////////////////////////////////////////
//void eraseThread(void* p) {
//	size_t i;
//	time_t ts;
//	while (!halt_flag) {
//		ts = time(0);
//		for (i = 0; i < FIBSIZE; ++i) {
//			FIBItem* x = &g_fib.at[i];
//			if (!x->dmac) {
//				continue;
//			}
//			if (ts > x->ts + ERASE_THRESHOLD_S) {
//				HWFIB_remove(x);
//				memset(x, 0, sizeof(*x));
//			}
//		}
//		sleep(ERASE_SLEEP_INTERVAL_MS);
//	}
//}
void eraseThread(void * p){
	int i;
	time_t ts;
	while(!halt_flag){
		ts = time(0);
		for(i = 0;i < FIBSIZEL; ++i){
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
		x.port = ;// set port to all, except cpu port and d->sport
		x.dmac = d->dmac;
		x.ts = time(0);
		x.type = DMAC;
		x.valid = 1;
		x.item_type = ITEM_2;
		FIB_insert(&x);

		x.port = ;// set to cup port
		x.smac = d->dmac;
		x.ts = time(0);
		x.type = SMAC;
		x.valid = 1;
		x.item_type = ITEM_3;
		FIB_insert(&x);
	}
}

//void onPacketArrival(PacketDigest* d) {
//	FIBItem* item;
//
//	// Item 1
//	item = FIB_find(d->smac, DMAC);
//	if (item) {
//		// Item already in FIB, update it
//		if (item->port != d->sport) {
//			item->port = d->sport;
//			HWFIB_update(item);
//		}
//	} else {
//		FIBItem x;
//		x.smac = 0;
//		x.dmac = d->smac;
//		x.port = d->sport;
//		FIB_insert(&x);
//		HWFIB_insert(&x);
//	}
//
//	// Item 2
//	item = FIB_find(d->dmac, DMAC);
//	if (!item) {
//		// HWFIB_broadcast
//	}
//
//	// Item 3
//	item = FIB_find(d->dmac, SMAC);
//	if (!item) {
//		FIBItem x;
//		x.smac = d->dmac;
//		x.dmac = 0;
//		x.port = PORT_CPU;
//		FIB_insert(&x);
//		HWFIB_insert(&x);
//	}
//}
///////////////////////////////////////////////////////////////////////////////
void HWFIB_insert(FIBItem* item) {
}
void HWFIB_update(FIBItem* item) {
}
void HWFIB_remove(FIBItem* item) {
}
