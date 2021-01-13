#include <assert.h>
#include <stdio.h>
#include "../src/bthread.h"
#include "../src/tmutex.h"
#include "../src/tbarrier.h"
#include "../src/tcondition.h"
#include "../src/tsemaphore.h"

bthread_t t1, t2, t3, t4,t5;
unsigned int counter1, counter2;
bthread_mutex_t mutex;
bthread_barrier_t barrier;
bthread_cond_t condition;
bthread_sem_t semaphore;

void *printStringThread(void *arg){
    bthread_printf("%s\n", (char*)arg);
    return (void*) 42;
}

void *sleepingThread(void *arg){
    bthread_sleep(2000);
    return NULL;
}

void *testCancelThread(void *arg){
    while(1){
        bthread_testcancel();
    }
}


void *cancelThread(void *arg){
    bthread_cancel(t5);
}

void *counterThread1(void *arg){
    while(1){
        counter1++;
        bthread_testcancel();
    }
}

void *counterThread2(void *arg){
    while(1){
        counter2++;
        bthread_testcancel();
    }
}

void *cancelAllThread(void *arg){
    bthread_sleep(3000);
    bthread_cancel(t1);
    bthread_cancel(t2);
}



void testCreateAndJoin(){
    bthread_printf("--- Test Create and Join ---\n You should see two thread printf\n");
    bthread_create(&t1, NULL, &printStringThread, "Thread 1");
    bthread_create(&t2, NULL, &printStringThread, "Thread 2");
    assert(t1 == 0);
    assert(t2 == t1+1);
    int status;
    bthread_join(t1,  (void**) &status);
    bthread_join(t2,NULL);
    assert(status == 42);
}

void testSleep(){
    bthread_printf("--- Test Sleep --- \n It should ends in 2 seconds\n");
    bthread_create(&t1, NULL, &sleepingThread, NULL);
    bthread_join(t1,NULL);
}

void testCancel(){
    bthread_printf("--- Test Cancel --- \n If it ends it works\n");
    int status;
    bthread_create(&t1, NULL, &testCancelThread, NULL);
    bthread_cancel(t1);
    bthread_join(t1,(void**) &status);
    assert(status == -1);
}

void testPreemption(){
    bthread_printf("--- Test Preemption --- \n If it ends it works\n");
    bthread_create(&t4, NULL, &cancelThread, NULL);
    bthread_create(&t5,NULL,&testCancelThread,NULL);

    bthread_join(t4,NULL);
    bthread_join(t5,NULL);

}


void testRandomScheduling(){
    bthread_printf("--- Test Random Scheduling --- \n Thread should be scheduled in different order each time\n");
    bthread_create(&t1, NULL, &printStringThread, "Thread 1");
    bthread_create(&t2, NULL, &printStringThread, "Thread 2");

    bthread_set_scheduling_policy(RANDOM);
    bthread_join(t1,NULL);
    bthread_join(t2,NULL);
}

void testPriorityScheduling(){
    bthread_printf("--- Test Priority Scheduling --- \n");
    bthread_create(&t1, NULL, &counterThread1, NULL);
    bthread_create(&t2, NULL, &counterThread2, NULL);
    bthread_create(&t3, NULL, &cancelAllThread, NULL);

    bthread_set_priority(t1,2);
    bthread_set_priority(t2,MAX_PRIORITY);
    //thread t3 has default priority
    bthread_set_scheduling_policy(PRIORITY);
    bthread_join(t1,NULL);
    bthread_join(t2,NULL);
    bthread_join(t3,NULL);

    assert(counter1 < counter2);

}
void *threadMutex(void *arg){
    bthread_mutex_lock(&mutex);
    bthread_printf("Thread %d \n", (int*)arg);
    bthread_sleep(1000);
    bthread_mutex_unlock(&mutex);
}

void testMutex(){
    bthread_printf("--- Test Mutex --- \n It should execute both thread\n");
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
    bthread_printf("--- Test Barrier --- \n It should execute both thread. A thread wait for the other\n");
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
    bthread_printf("--- Test Condition --- \n t1, t2 and t3 should wait on condition. t4 should signal and wake t1. t5 should broadcast and qait t2 and t3\n");
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
    bthread_printf("--- Test Semaphore --- \n It should execute all threads\n");
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
