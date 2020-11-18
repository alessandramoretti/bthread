//
// Created by alemo on 05.10.2020.
//

#ifndef BTHREAD_BTHREAD_H
#define BTHREAD_BTHREAD_H




typedef unsigned long int bthread_t;

typedef enum { __BTHREAD_READY = 0, __BTHREAD_BLOCKED, __BTHREAD_SLEEPING,
    __BTHREAD_ZOMBIE} bthread_state;

typedef struct {
} bthread_attr_t;

typedef void *(*bthread_routine) (void *);

int bthread_create(bthread_t *bthread, const bthread_attr_t *attr,
                   void *(*start_routine) (void *), void *arg);

int bthread_join(bthread_t bthread, void **retval);

void bthread_yield();

void bthread_exit(void *retval);

void bthread_sleep(double ms);

int bthread_cancel(bthread_t bthread);

void bthread_testcancel(void);

#endif //BTHREAD_BTHREAD_H
