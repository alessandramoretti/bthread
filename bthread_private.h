//
// Created by alemo on 05.10.2020.
//

#include "bthread.h"
#include <setjmp.h>

#ifndef BTHREAD_BTHREAD_PRIVATE_H
#define BTHREAD_BTHREAD_PRIVATE_H

#endif //BTHREAD_BTHREAD_PRIVATE_H

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