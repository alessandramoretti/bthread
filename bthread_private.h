//
// Created by alemo on 05.10.2020.
//

#include "bthread.h"
#include "tqueue.h"
#include <setjmp.h>
#include <signal.h>

#ifndef BTHREAD_BTHREAD_PRIVATE_H
#define BTHREAD_BTHREAD_PRIVATE_H



static int bthread_check_if_zombie(bthread_t bthread, void **retval);
static TQueue bthread_get_queue_at(bthread_t bthread);
#define save_context(CONTEXT) sigsetjmp(CONTEXT, 1)
#define restore_context(CONTEXT)  siglongjmp(CONTEXT, 1)
#define QUANTUM_USEC 100000


typedef struct {
    bthread_t tid;
    bthread_routine body;
    void* arg;
    bthread_state state;
    bthread_attr_t attr;
    char* stack;
    jmp_buf context;
    void* retval;
    double wake_up_time;
    int cancel_req;
} __bthread_private;

typedef struct {
    TQueue queue;
    TQueue current_item;
    jmp_buf context;
    bthread_t current_tid;
    sigset_t sig_set;
} __bthread_scheduler_private;

__bthread_scheduler_private* bthread_get_scheduler();
void bthread_cleanup();
double get_current_time_millis();

static void bthread_setup_timer();
void bthread_block_timer_signal();
void bthread_unblock_timer_signal();

#endif //BTHREAD_BTHREAD_PRIVATE_H
