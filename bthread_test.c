#include "bthread.h"
#include <assert.h>
#include <stdio.h>

bthread_t t1, t2;

void *thread1(void *arg){
    return (void*) 42;

}

void *thread2(void *arg){
    return NULL;
}


void testCreate(){
    bthread_create(&t1,NULL,&thread1,NULL);
    bthread_create(&t2,NULL,&thread2,NULL);
    assert(t1 == 0);
    assert(t2 == t1+1);
}


void testJoin(){
    int status;
    bthread_join(t1,  (void**) &status);
    assert(status == 42);
}

int main(){
    testCreate();
    testJoin();

    return 0;
}