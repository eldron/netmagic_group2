/*..............��ں���.................*/
#include<stdio.h>
#include<pthread.h>
#include "nmac.h"
#include "group2.h"
int main() {
//DEBUG_FILE = fopen("table.txt", "w");
//	pt1_num = 0;
	//��ʼ����������Ϣ
	if(contr_init()){
		printf("Initialized controller successfully!\n");
	} else {
		printf("Failed to initialize controller!\n");
		return 1;
	}

	pthread_t pt;
	int ret = -1;
	ret = pthread_create(&pt, NULL, (void *) (&packet_capture), NULL); //ץ���߳�
	if (ret != 0) {
		printf("Failed to create packet capture thread!\n");
		return 1;
	} else {
		//pt1_num = 1;
		printf("created packet capture thread\n");
	}

	ret = pthread_create(&pt, NULL, (void *) (&write_data_thread), NULL);// д�����߳�
	if(ret){
		printf("failed to create write data thread!\n");
	} else {
		printf("created write data thread\n");
	}
	
	ret = pthread_create(&pt, NULL, (void *) (&console), NULL); //Demo��ʾ�߳�
	if (ret != 0) {
		printf("Failed to create console thread!\n");
		return 1; 
	} else {
		printf("created console thread\n");
	}

	ret = pthread_create(&pt, NULL, (void *) (&eraseThread), NULL); // erase thread
	if (ret != 0) {
		printf("Failed to create erase thread!\n");
		return 1; 
	} else {
		printf("created erase thread\n");
	}


	ourparse();
}

