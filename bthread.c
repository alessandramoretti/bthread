//
// Created by alemo on 19.10.2020.
//

#include "bthread_private.h"
#include <stdlib.h>
#include "stdint.h"
#include <string.h>

#define STACK_SIZE 1000


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
    __bthread_scheduler_private* scheduler = bthread_get_scheduler();
    __bthread_private* current_thread = (__bthread_private*)tqueue_get_data(scheduler->current_item) ;
    memcpy(current_thread->retval,retval,sizeof(retval));

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
            memcpy(*retval,thread->retval, sizeof(thread->retval));
            free(thread->stack);
            tqueue_pop(&queue);
        }
        return 1;
    }
        return 0;
}

int bthread_join(bthread_t bthread, void **retval)
{
    volatile __bthread_scheduler_private* scheduler = bthread_get_scheduler();
    scheduler->current_item = scheduler->queue;
    save_context(scheduler->context);
    if (bthread_check_if_zombie(bthread, retval)) return 0;
    __bthread_private* tp;
    do {
        scheduler->current_item = tqueue_at_offset(scheduler->current_item, 1);
        tp = (__bthread_private*) tqueue_get_data(scheduler->current_item);
    } while (tp->state != __BTHREAD_READY);
    if (tp->stack) {
        restore_context(tp->context);
    } else {
        tp->stack = (char*) malloc(sizeof(char) * STACK_SIZE);
        unsigned long target = tp->stack + STACK_SIZE - 1;
#if __APPLE__
        // OSX requires 16 bytes aligned stack
 target -= (target % 16);
#endif
#if __x86_64__
        asm __volatile__("movq %0, %%rsp" :: "r"((intptr_t) target));
#else
        asm __volatile__("movl %0, %%esp" :: "r"((intptr_t) target));
#endif
        bthread_exit(tp->body(tp->arg));
    }

}

