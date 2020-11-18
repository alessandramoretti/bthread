#include <assert.h>
#include <stdio.h>
#include "bthread.h"

bthread_t t1, t2, t3;

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

int main(){
    testCreateAndJoin();
    testSleep();

    return 0;
}
