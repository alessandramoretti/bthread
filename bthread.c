//
// Created by alemo on 19.10.2020.
//

#include "bthread_private.h"
#include <stdlib.h>
#include <string.h>


__bthread_scheduler_private* bthread_get_scheduler(){
    static __bthread_scheduler_private* scheduler=NULL;  //singleton pattern
    if(scheduler == NULL){
        scheduler = (__bthread_scheduler_private*)malloc(sizeof(__bthread_scheduler_private));
        scheduler->queue = NULL;
        scheduler->current_item = NULL;
        scheduler->current_tid = 0;
    }
    return scheduler;
}



void bthread_exit(void *retval){
    c
}

static TQueue bthread_get_queue_at(bthread_t bthread){
    __bthread_scheduler_private* scheduler = bthread_get_scheduler();
    TQueue queue = scheduler->queue;
    for(int i=0; i < tqueue_size(queue);i++){
        queue= tqueue_at_offset(queue,i);
        if((( __bthread_private*)tqueue_get_data(queue))->tid == bthread){
            return queue;
        }
    }
    return NULL;
}

static int bthread_check_if_zombie(bthread_t bthread, void **retval){
    TQueue queue = bthread_get_queue_at(bthread);
    __bthread_private* thread = (__bthread_private*)tqueue_get_data(queue);

    if(thread->state == __BTHREAD_ZOMBIE){
        if(retval != NULL){
            int exit_value=0;
            memcpy(*retval,&exit_value, sizeof(exit_value));
            free(thread->stack);
            tqueue_pop(&queue);
        }
        return 1;
    }
        return 0;
}

