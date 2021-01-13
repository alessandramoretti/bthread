#include <assert.h>
#include <stdio.h>
#include "bthread.h"
#include "tmutex.h"
#include "tbarrier.h"
#include "tcondition.h"
#include "tsemaphore.h"

bthread_t t1, t2, t3, t4,t5,t6,t7,t8,t9;
unsigned int counter1, counter2;
bthread_mutex_t mutex;
bthread_barrier_t barrier;
bthread_cond_t condition;
bthread_sem_t semaphore;

void *thread1(void *arg){
    bthread_printf("Thread 1");
    return (void*) 42;
}

void *thread2(void *arg){
    bthread_printf("Thread 2");
    return NULL;
}

void *thread3(void *arg){
    bthread_printf("Thread 3");
    bthread_sleep(2000);
    return NULL;
}

void *thread4(void *arg){
    while(1){
        bthread_testcancel();
    }
}

void *thread5(void *arg){
    while(1){
        bthread_testcancel();
    }
}

void *thread6(void *arg){
    bthread_cancel(t5);
}

void *thread7(void *arg){
    while(1){
        counter1++;
        bthread_testcancel();
    }
}

void *thread8(void *arg){
    while(1){
        counter2++;
        bthread_testcancel();
    }
}

void *thread9(void *arg){
    bthread_sleep(3000);
    bthread_cancel(t7);
    bthread_cancel(t8);
}



void testCreateAndJoin(){
    bthread_create(&t1,NULL,&thread1,NULL);
    bthread_create(&t2,NULL,&thread2,NULL);
    assert(t1 == 0);
    assert(t2 == t1+1);
    int status;
    bthread_join(t1,  (void**) &status);
    bthread_join(t2,NULL);
    assert(status == 42);
}

void testSleep(){
    bthread_create(&t3,NULL,&thread3,NULL);
    bthread_join(t3,NULL);
}

void testCancel(){
    int status;
    bthread_create(&t4,NULL,&thread4,NULL);
    bthread_cancel(t4);
    bthread_join(t4,(void**) &status);
    assert(status == -1);
}

void testPreemption(){
    bthread_create(&t6,NULL,&thread6,NULL);
    bthread_create(&t5,NULL,&thread5,NULL);

    bthread_join(t5,NULL);
    bthread_join(t6,NULL);

}


void testRandomScheduling(){
    printf("TEST RANDOM:\n");
    bthread_create(&t1,NULL,&thread1,NULL);
    bthread_create(&t2,NULL,&thread2,NULL);

    bthread_set_scheduling_policy(RANDOM);
    bthread_join(t1,NULL);
    bthread_join(t2,NULL);
}

void testPriorityScheduling(){
    bthread_create(&t7,NULL,&thread7,NULL);
    bthread_create(&t8,NULL,&thread8,NULL);
    bthread_create(&t9,NULL,&thread9,NULL);

    bthread_set_priority(t7,2);
    bthread_set_priority(t8,MAX_PRIORITY);
    //thread t9 has default priority
    bthread_set_scheduling_policy(PRIORITY);
    bthread_join(t7,NULL);
    bthread_join(t8,NULL);
    bthread_join(t9,NULL);

    assert(counter1 < counter2);

}
void *threadMutex(void *arg){
    bthread_mutex_lock(&mutex);
    bthread_printf("Thread %d \n", (int*)arg);
    bthread_sleep(1000);
    bthread_mutex_unlock(&mutex);
}

void testMutex(){
    bthread_create(&t1,NULL,&threadMutex,(void*)1);
    bthread_create(&t2,NULL,&threadMutex,(void*)2);

    bthread_join(t1,NULL);
    bthread_join(t2,NULL);
}

void *threadBarrier(void *arg){
    bthread_printf("Thread %d reached barrier\n", (int*)arg);
    bthread_barrier_wait(&barrier);
    bthread_printf("Thread %d get over barrier\n", (int*)arg);
}

void testBarrier(){
    bthread_create(&t1,NULL,&threadBarrier,(void*)1);
    bthread_create(&t2,NULL,&threadBarrier,(void*)2);

    bthread_join(t1,NULL);
    bthread_join(t2,NULL);
}

void *threadConditionWait(void *arg){
    bthread_mutex_lock(&mutex);
    bthread_printf("Thread %d wait on condition\n", (int*)arg);
    bthread_cond_wait(&condition,&mutex);
    bthread_printf("Thread %d wake up\n", (int*)arg);
    bthread_mutex_unlock(&mutex);
}

void *threadConditionSignal(void *arg){
    bthread_sleep(1000);
    bthread_printf("Thread %d wait signal on condition\n", (int*)arg);
    bthread_cond_signal(&condition);
}

void *threadConditionBroadcast(void *arg){
    bthread_sleep(2000);
    bthread_printf("Thread %d wait broadcast on condition\n", (int*)arg);
    bthread_cond_broadcast(&condition);
}

void testCondition(){
    bthread_create(&t1,NULL,&threadConditionWait,(void*)1);
    bthread_create(&t2,NULL,&threadConditionWait,(void*)2);
    bthread_create(&t3,NULL,&threadConditionWait,(void*)3);

    bthread_create(&t4,NULL,&threadConditionSignal,(void*)4);
    bthread_create(&t5,NULL,&threadConditionBroadcast,(void*)5);


    bthread_join(t1,NULL);
    bthread_join(t2,NULL);
    bthread_join(t3,NULL);
    bthread_join(t4,NULL);
    bthread_join(t5,NULL);
}

void *threadSemaphore(void *arg){
    bthread_printf("Thread %d wait on semaphore\n", (int*)arg);
    bthread_sem_wait(&semaphore);
    bthread_printf("Thread %d takes semaphore\n", (int*)arg);
    bthread_sleep(2000);
    bthread_sem_post(&semaphore);
    bthread_printf("Thread %d release semaphore\n", (int*)arg);
}

void testSemaphore(){
    bthread_create(&t1,NULL,&threadSemaphore,(void*)1);
    bthread_create(&t2,NULL,&threadSemaphore,(void*)2);
    bthread_create(&t3,NULL,&threadSemaphore,(void*)3);

    bthread_join(t1,NULL);
    bthread_join(t2,NULL);
    bthread_join(t3,NULL);
}


int main(){

    bthread_mutex_init(&mutex, NULL);
    bthread_barrier_init(&barrier,NULL,2);
    bthread_cond_init(&condition,NULL);
    bthread_sem_init(&semaphore, NULL, 2);

    testCreateAndJoin();
    testSleep();
    testCancel();
    testPreemption();
    testRandomScheduling();
    testPriorityScheduling();
    testMutex();
    testBarrier();
    testCondition();
    testSemaphore();

    bthread_mutex_destroy(&mutex);
    bthread_barrier_destroy(&barrier);
    bthread_cond_destroy(&condition);
    bthread_sem_destroy(&semaphore);
    return 0;
}
