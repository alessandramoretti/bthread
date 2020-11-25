//
// Created by alemo on 19.10.2020.
//

#include "bthread_private.h"
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#define STACK_SIZE 60000


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

int bthread_create(bthread_t *bthread, const bthread_attr_t *attr,
                   void *(*start_routine) (void *), void *arg){
    __bthread_private *newThread = (__bthread_private*) malloc(sizeof(__bthread_private));
    __bthread_scheduler_private* scheduler = bthread_get_scheduler();

    tqueue_enqueue(&scheduler->queue, newThread);
    newThread->tid = scheduler->current_tid++;
    *bthread = newThread->tid;
    newThread->body = start_routine;
    newThread->arg = arg;
    newThread->state = __BTHREAD_READY;
    newThread->attr = *attr;
    newThread->stack = NULL;
    newThread->retval = NULL;
    newThread->cancel_req = 0;

    return 0;
}

void bthread_yield(){
    volatile __bthread_scheduler_private *scheduler = bthread_get_scheduler();
    volatile __bthread_private *currentThread = (__bthread_private*)tqueue_get_data(scheduler->current_item);
    if(!save_context(currentThread->context))
        restore_context(scheduler->context);

}

void bthread_exit(void *retval){
    __bthread_scheduler_private* scheduler = bthread_get_scheduler();
    __bthread_private* current_thread = (__bthread_private*)tqueue_get_data(scheduler->current_item) ;
    if(retval != NULL){
        current_thread->retval =retval;
    }
    current_thread->state = __BTHREAD_ZOMBIE;
    bthread_yield();

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
    __bthread_private *thread = (__bthread_private *) tqueue_get_data(queue);

    if (thread->state != __BTHREAD_ZOMBIE)
        return 0;

    if (retval)
        *retval = (thread->retval);

    free(thread->stack);

    if (thread == tqueue_get_data(bthread_get_scheduler()->queue))
        free(tqueue_pop(&bthread_get_scheduler()->queue));
    else
        free(tqueue_pop(&queue));

    return 1;
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
        if(tp->state == __BTHREAD_SLEEPING && get_current_time_millis() >= tp->wake_up_time)
            tp->state = __BTHREAD_READY;
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

void bthread_sleep(double ms){
    __bthread_scheduler_private* scheduler = bthread_get_scheduler();
    __bthread_private *thread = (__bthread_private*) tqueue_get_data(scheduler->current_item);
    thread->state=__BTHREAD_SLEEPING;
    thread->wake_up_time = get_current_time_millis() + ms;
    bthread_yield();
}


double get_current_time_millis()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (tv.tv_sec) * 1000 + (tv.tv_usec) / 1000 ;
}

int bthread_cancel(bthread_t bthread){
    __bthread_private* thread = (__bthread_private*) tqueue_get_data(bthread_get_queue_at(bthread));
    thread->cancel_req=1;

}

void bthread_testcancel(void){
    __bthread_private* thread = (__bthread_private*) tqueue_get_data(bthread_get_scheduler()->current_item);
    if(thread->cancel_req)
        bthread_exit((void**)-1);
}