#include "mqueue.h"

// init a message queue, return 1 if successful, return 0 instead
int init_mqueue(MQueue * queue){
	queue->count = 0;
	queue->head = queue->tail = NULL;
	// initialize mutex and cond here
	pthread_mutex_init(&queue->mutex, NULL);
	pthread_cond_init(&queue->cond, NULL);
	return 1;
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
	
#ifdef DEBUG
	printf("from enqueue: enqueued a message, %d messages in queue\n", queue->count);
#endif

	pthread_cond_signal(&(queue->cond));
	pthread_mutex_unlock(&(queue->mutex));
	
	return 1;
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

#ifdef DEBUG
	printf("from dequeue: dequeued a message, %d messages in queue\n", queue->count);
#endif

	pthread_mutex_unlock(&(queue->mutex));
	return 1;
}
