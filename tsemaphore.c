//
// Created by alemo on 23.12.2020.
//

#include "tsemaphore.h"
#include "bthread_private.h"
#include <stdio.h>
#include <assert.h>

int bthread_sem_init(bthread_sem_t* m, int pshared, int value){
    assert(m != NULL);
    m->value = value;
    m->waiting_list = NULL;
    return 0;
}

int bthread_sem_destroy(bthread_sem_t* m){
    assert(tqueue_size(m->waiting_list) == 0);
    return 0;
}

int bthread_sem_wait(bthread_sem_t* m){
    bthread_block_timer_signal();
    trace("SEM WAIT\n");
    __bthread_scheduler_private* scheduler = bthread_get_scheduler();
    volatile __bthread_private* bthread = (__bthread_private*)tqueue_get_data(scheduler->current_item);

    if(m->value == 0){
        bthread->state = __BTHREAD_BLOCKED;
        tqueue_enqueue(&m->waiting_list, bthread);
        bthread_yield();
    }else{
        m->value --;
    }
    bthread_unblock_timer_signal();
}

int bthread_sem_post(bthread_sem_t* m){
    bthread_block_timer_signal();
    trace("SEM POST\n");
    __bthread_scheduler_private* scheduler = bthread_get_scheduler();
    volatile __bthread_private* bthread = (__bthread_private*)tqueue_get_data(scheduler->current_item);
    __bthread_private* unlock = tqueue_pop(&m->waiting_list);
    if (unlock != NULL) {
        unlock->state = __BTHREAD_READY;
        bthread_yield();
        return 0;
    }else{
        m->value ++;
    }
    bthread_unblock_timer_signal();
    return 0;
}