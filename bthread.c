//
// Created by alemo on 19.10.2020.
//

#include "bthread_private.h"
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <signal.h>
#include <pthread.h>
#include <sys/time.h>
#include <stdarg.h>
#include <unistd.h>
#define STACK_SIZE 60000


__bthread_scheduler_private* bthread_get_scheduler(){
    static __bthread_scheduler_private* scheduler=NULL;  //singleton pattern
    if(scheduler == NULL){
        scheduler = (__bthread_scheduler_private*)malloc(sizeof(__bthread_scheduler_private));
        scheduler->queue = NULL;
        scheduler->current_item = NULL;
        scheduler->current_tid = 0;
        sigemptyset(&(scheduler->sig_set));
        sigaddset(&(scheduler->sig_set), SIGVTALRM);
        scheduler->scheduling_routine = &round_robin;
        srand( time(NULL));
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
    newThread->priority = DEFAULT_PRIORITY;
    newThread->quantum_counter = 0;
    return 0;
}


void bthread_yield(){
    bthread_block_timer_signal();
    __bthread_scheduler_private *scheduler = bthread_get_scheduler();
    __bthread_private *currentThread = (__bthread_private*)tqueue_get_data(scheduler->current_item);
    if(!save_context(currentThread->context)){
        restore_context(scheduler->context);
    }
    bthread_unblock_timer_signal();
}

void bthread_exit(void *retval){
    volatile __bthread_scheduler_private* scheduler = bthread_get_scheduler();
    volatile __bthread_private* current_thread = (__bthread_private*)tqueue_get_data(scheduler->current_item) ;
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
    bthread_block_timer_signal();
    bthread_setup_timer();
    volatile __bthread_scheduler_private* scheduler = bthread_get_scheduler();
    scheduler->current_item = scheduler->queue;
    save_context(scheduler->context);
    if (bthread_check_if_zombie(bthread, retval)) return 0;
    __bthread_private* tp;
    do {
        (scheduler->scheduling_routine)();
        tp = (__bthread_private*) tqueue_get_data(scheduler->current_item);
        if(tp->state == __BTHREAD_SLEEPING && get_current_time_millis() >= tp->wake_up_time) {
            tp->state = __BTHREAD_READY;
        }
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

        bthread_unblock_timer_signal();
        bthread_exit(tp->body(tp->arg));
    }

}

void bthread_sleep(double ms){
    volatile __bthread_scheduler_private* scheduler = bthread_get_scheduler();
    volatile __bthread_private *thread = (__bthread_private*) tqueue_get_data(scheduler->current_item);
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
    volatile __bthread_private* thread = (__bthread_private*) tqueue_get_data(bthread_get_queue_at(bthread));
    thread->cancel_req=1;

}

void bthread_testcancel(void){
    volatile __bthread_private* thread = (__bthread_private*) tqueue_get_data(bthread_get_scheduler()->current_item);
    if(thread->cancel_req)
        bthread_exit((void**)-1);
}

static void bthread_setup_timer()
{
    static bool initialized = false;
    if (!initialized) {
        signal(SIGVTALRM, (void (*)()) bthread_yield);
        struct itimerval time;
        time.it_interval.tv_sec = 0;
        time.it_interval.tv_usec = QUANTUM_USEC;
        time.it_value.tv_sec = 0;
        time.it_value.tv_usec = QUANTUM_USEC;
        initialized = true;
        setitimer(ITIMER_VIRTUAL, &time, NULL);
    }
}

void bthread_block_timer_signal(){
    sigprocmask(SIG_BLOCK, &(bthread_get_scheduler()->sig_set), NULL);
}

void bthread_unblock_timer_signal(){
    sigprocmask(SIG_UNBLOCK, &(bthread_get_scheduler()->sig_set), NULL);
}

void bthread_printf(const char* format, ...)
{
    bthread_block_timer_signal();
    va_list args;
    va_start (args, format);
    vprintf (format, args);
    va_end (args);
    bthread_unblock_timer_signal();
}

void round_robin(){
    __bthread_scheduler_private* scheduler = bthread_get_scheduler();
    scheduler->current_item = tqueue_at_offset(scheduler->current_item, 1);
}

void random_scheduling(){
    __bthread_scheduler_private* scheduler = bthread_get_scheduler();
    unsigned int offset = rand() % tqueue_size(scheduler->queue);
    scheduler->current_item = tqueue_at_offset(scheduler->current_item, offset);
}


void priority_scheduling(){
    __bthread_scheduler_private* scheduler = bthread_get_scheduler();
    __bthread_private* currentThread = (__bthread_private*)tqueue_get_data((scheduler->current_item));
    if(currentThread->quantum_counter >= currentThread->priority){
        currentThread->quantum_counter = 0;
        scheduler->current_item = tqueue_at_offset(scheduler->current_item, 1);
    }
    __bthread_private* chosenThread = (__bthread_private*)tqueue_get_data((scheduler->current_item));
    chosenThread->quantum_counter++;
}

void bthread_set_scheduling_policy(scheduling_policy policy){
    __bthread_scheduler_private* scheduler = bthread_get_scheduler();
    switch(policy){
        case ROUND_ROBIN:
            scheduler->scheduling_routine = &round_robin;
            break;
        case RANDOM:
            scheduler->scheduling_routine = &random_scheduling;
            break;
        case PRIORITY:
            scheduler->scheduling_routine = &priority_scheduling;
            break;
    }
}

void bthread_set_priority(bthread_t tid, unsigned int priority){
    if(priority > MAX_PRIORITY)
        priority = MAX_PRIORITY;
    __bthread_scheduler_private* scheduler = bthread_get_scheduler();
    __bthread_private* thread = (__bthread_private*)tqueue_get_data(bthread_get_queue_at(tid));
    thread->priority = priority;
}
