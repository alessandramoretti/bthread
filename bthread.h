//
// Created by alemo on 05.10.2020.
//

#ifndef BTHREAD_BTHREAD_H
#define BTHREAD_BTHREAD_H

#endif //BTHREAD_BTHREAD_H


typedef unsigned long int bthread_t;

typedef enum { __BTHREAD_READY = 0, __BTHREAD_BLOCKED, __BTHREAD_SLEEPING,
    __BTHREAD_ZOMBIE} bthread_state;

typedef struct {
} bthread_attr_t;

typedef void *(*bthread_routine) (void *);