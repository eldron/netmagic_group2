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

// init a message queue, return 0 if successful, return -1 instead
int init_mqueue(MQueue * queue){
	queue->count = 0;
	queue->head = queue->tail = NULL;
	// initialize mutex and cond here
	pthread_mutex_init(&queue->mutex, NULL);
	pthread_cond_init(&queue->cond, NULL);
	return 0;
}

// delete a queue,  free the allocated space
void delete_mqueue(MQueue * queue){
	queue->count = 0;
	Node * tmp = NULL;
	while(queue->head){
		tmp = queue->head;
		queue->head = queue->head->next;
		free(tmp);
	}
	queue->tail = NULL;
	// may also destroy mutex and cond here
	pthread_mutex_destroy(&queue->mutex);
	pthread_cond_destroy(&queue->cond);
}

int is_empty(MQueue * queue){
	return queue->count == 0;
}

// return 0 if successful, return -1 otherwise
// 值传递，而不是指针操作，效率略低，但安全
int enqueue(MQueue * queue, Message * msg){
	pthread_mutex_lock(&(queue->mutex));

	if(queue->count == 0){
		queue->head = queue->tail = (Node *) malloc(sizeof(Node));
		queue->tail->next = NULL;
		queue->tail->msg = *msg;
	} else {
		queue->tail->next = (Node *) malloc(sizeof(Node));
		queue->tail = queue->tail->next;
		queue->tail->next = NULL;
		queue->tail->msg = *msg;
	}
	queue->count++;

	pthread_cond_signal(&(queue->cond));
	pthread_mutex_unlock(&(queue->mutex));
	
	return 0;
}

// 值传递，而不是指针操作，效率略低，但安全
int dequeue(MQueue * queue, Message * msg){
	pthread_mutex_lock(&(queue->mutex));
	while(queue->count == 0){
		pthread_cond_wait(&(queue->cond), &(queue->mutex));
	}

	Node * tmp = queue->head;
	queue->head = queue->head->next;
	*msg = tmp->msg;
	queue->count--;
	free(tmp);

	pthread_mutex_unlock(&(queue->mutex));
	return 0;
}
#endif
