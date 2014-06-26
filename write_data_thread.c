/*
	一个单独的线程，用来向netmagic写数据
	nmac_write_data() 函数不应该和调用消息队列调用dequeue的代码跑在一个线程里面
*/

#include "mqueue.h"
#include "nmac.h"
#include "FFT.h"

void write_data_thread(){
	Data data;
	while(!halt_flag){
		dequeue(&data_queue, &data);
#ifdef DEBUG
		printf("dequeued from data queue, %d left\n", data_queue.count);
#endif
		if(data.length == sizeof(FFT)){
			nmac_write_data(data.addr, data.length / 4, (u_int32_t *) (data.content));
		} else {
#ifdef DEBUG
			fprintf(stderr, "from write data thread: data length incorrect!\n");
#endif
		}
	}
}

