/*..............��ں���.................*/
#include<stdio.h>
#include<pthread.h>
#include "nmac.h"
#include "group2.h"
int main() {
	pt1_num = 0;
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
	} else
		pt1_num = 1;

	ret = pthread_create(&pt, NULL, (void *) (&console), NULL); //Demo��ʾ�߳�
	if (ret != 0) {
		printf("Failed to create console thread!\n");
		return 1; 
	}

	ret = pthread_create(&pt, NULL, (void *) (&eraseThread), NULL); // erase thread
	if (ret != 0) {
		printf("Failed to create erase thread!\n");
		return 1; 
	}


	parsing();
	while (1) //�ȴ���Ϣ����ɾ��
	{
		if (pt1_num == 0)
			break;
	}
}

