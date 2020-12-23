#include <assert.h>
#include <stdio.h>
#include "bthread.h"

bthread_t t1, t2, t3, t4,t5,t6,t7,t8,t9;
unsigned int counter1, counter2;

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
    bthread_sleep(5000);
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

int main(){
    testCreateAndJoin();
    testSleep();
    testCancel();
    testPreemption();
    testRandomScheduling();
    testPriorityScheduling();
    return 0;
}
