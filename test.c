/*..............入口函数.................*/
#include<stdio.h>
#include<pthread.h>
#include "nmac.h"
#include "group2.h"
#include "threads.h"

int main() {
//DEBUG_FILE = fopen("table.txt", "w");
//	pt1_num = 0;
	//初始化控制器信息
	if(contr_init()){
		printf("Initialized controller successfully!\n");
	} else {
		printf("Failed to initialize controller!\n");
		return 1;
	}

	int ret = -1;
	ret = pthread_create(&parse_pt, NULL, (void *) (&ourparse), NULL);
	if(ret != 0){
		printf("Failed to create parse thread!\n");
		return 1;
	} else {
		printf("created parses thread!\n");
	}

	ret = pthread_create(&packet_capture_pt, NULL, (void *) (&packet_capture), NULL); //抓包线程
	if (ret != 0) {
		printf("Failed to create packet capture thread!\n");
		return 1;
	} else {
		//pt1_num = 1;
		printf("created packet capture thread\n");
	}

	ret = pthread_create(&write_data_pt, NULL, (void *) (&write_data_thread), NULL);// 写数据线程
	if(ret){
		printf("failed to create write data thread!\n");
	} else {
		printf("created write data thread\n");
	}
	
//	ret = pthread_create(&pt, NULL, (void *) (&console), NULL); //Demo演示线程
//	if (ret != 0) {
//		printf("Failed to create console thread!\n");
//		return 1;
//	} else {
//		printf("created console thread\n");
//	}

	ret = pthread_create(&erase_pt, NULL, (void *) (&eraseThread), NULL); // erase thread
	if (ret != 0) {
		printf("Failed to create erase thread!\n");
		return 1; 
	} else {
		printf("created erase thread\n");
	}


	//ourparse();
	console();
}

