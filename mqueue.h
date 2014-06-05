#ifndef __MQUEUE_H
#define __MQUEUE_H

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

typedef struct {
	int length;
	char content[1500];
} Message;

typedef struct _Node {
	Message msg;
	struct _Node * next;
} Node;

typedef struct {
	pthread_mutex_t mutex;
	pthread_cond_t cond;
	int count;
	Node * head;
	Node * tail;
} MQueue;

int init_mqueue(MQueue * queue);
void delete_mqueue(MQueue * queue);
int is_empty(MQueue * queue);
int enqueue(MQueue * queue, Message * msg);
int dequeue(MQueue * queue, Message * msg);
#endif
