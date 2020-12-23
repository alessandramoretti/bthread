//
// Created by alemo on 23.12.2020.
//
#include "tcondition.h"
#include "bthread_private.h"
#include <stdio.h>
#include <assert.h>

int bthread_cond_init(bthread_cond_t* c, const bthread_condattr_t *attr){
    assert(c != NULL);
    c->waiting_list = NULL;
    return 0;
}

int bthread_cond_destroy(bthread_cond_t* c){
    assert(tqueue_size(c->waiting_list) == 0);
    return 0;
}

int bthread_cond_wait(bthread_cond_t* c, bthread_mutex_t* mutex){
    bthread_block_timer_signal();
    __bthread_scheduler_private* scheduler = bthread_get_scheduler();

    volatile __bthread_private* bthread = (__bthread_private*)tqueue_get_data(scheduler->current_item);

    tqueue_enqueue(&c->waiting_list, bthread);
    bthread->state = __BTHREAD_BLOCKED;
    bthread_yield();

    return 0;
}

int bthread_cond_signal(bthread_cond_t* c){
    bthread_block_timer_signal();
    __bthread_private* signal = tqueue_pop(&c->waiting_list);
    signal->state = __BTHREAD_READY;
    bthread_yield(); //TODO perchè devo fare yield?
    bthread_unblock_timer_signal();
    return 0;
}

int bthread_cond_broadcast(bthread_cond_t* c){
    bthread_block_timer_signal();
    while(tqueue_size(c->waiting_list)){
        __bthread_private* signal = tqueue_pop(&c->waiting_list);
        signal->state = __BTHREAD_READY;
    }

    bthread_yield();
    return 0;

}