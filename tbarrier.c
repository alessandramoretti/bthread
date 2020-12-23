//
// Created by alemo on 23.12.2020.
//

#include <assert.h>
#include "tbarrier.h"
#include "bthread_private.h"
#include <stdio.h>

int bthread_barrier_init(bthread_barrier_t* b, const bthread_barrierattr_t* attr, unsigned count){
    assert(b != NULL);
    b->barrier_size = 0;
    b->count = count;
    b->waiting_list = NULL;
    return 0;
}

int bthread_barrier_destroy(bthread_barrier_t* b){
    assert(tqueue_size(b->waiting_list) == 0);
    return 0;
}

int bthread_barrier_wait(bthread_barrier_t* b){
    bthread_block_timer_signal();
    trace("BARRIER WAIT\n");
    __bthread_scheduler_private* scheduler = bthread_get_scheduler();

    b->barrier_size ++;
    if(b->barrier_size == b->count){
        while(tqueue_size(b->waiting_list)){
            __bthread_private* waiting_thread = (__bthread_private*)tqueue_pop(&b->waiting_list);
            waiting_thread->state = __BTHREAD_READY;
        }
        bthread_unblock_timer_signal();
    }else{
        volatile __bthread_private* bthread = (__bthread_private*)tqueue_get_data(scheduler->current_item);
        tqueue_enqueue(&b->waiting_list, bthread);
        bthread->state = __BTHREAD_BLOCKED;
        bthread_yield();
    }
    return 0;
}

