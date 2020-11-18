#include <assert.h>
#include <stdio.h>
#include "bthread.h"

bthread_t t1, t2, t3, t4;

void *thread1(void *arg){
    return (void*) 42;
}

void *thread2(void *arg){
    return NULL;
}

void *thread3(void *arg){
    bthread_sleep(5000);
    return NULL;
}

void *thread4(void *arg){
    while(1){
        bthread_testcancel();
    }
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

int main(){
    testCreateAndJoin();
    testSleep();
    testCancel();
    return 0;
}
