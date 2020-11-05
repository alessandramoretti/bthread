//
// Created by alemo on 05.10.2020.
//

#include "bthread.h"
#include "tqueue.h"
#include <setjmp.h>

#ifndef BTHREAD_BTHREAD_PRIVATE_H
#define BTHREAD_BTHREAD_PRIVATE_H

#endif //BTHREAD_BTHREAD_PRIVATE_H

static int bthread_check_if_zombie(bthread_t bthread, void **retval);
static TQueue bthread_get_queue_at(bthread_t bthread);
#define save_context(CONTEXT) setjmp(CONTEXT)
#define restore_context(CONTEXT) longjmp(CONTEXT,1)



typedef struct {
    bthread_t tid;
    bthread_routine body;
    void* arg;
    bthread_state state;
    bthread_attr_t attr;
    char* stack;
    jmp_buf context;
    void* retval;
} __bthread_private;

typedef struct {
    TQueue queue;
    TQueue current_item;
    jmp_buf context;
    bthread_t current_tid;
} __bthread_scheduler_private;

__bthread_scheduler_private* bthread_get_scheduler();
void bthread_cleanup();