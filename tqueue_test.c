//
// Created by alemo on 07.10.2020.
//

#include "tqueue.h"
#include <stdio.h>
#include <assert.h>


void tqueue_test(){
    TQueue q=NULL;

    assert(tqueue_pop(&q) == NULL);

    assert(tqueue_at_offset(q,0) == NULL);

    int data1 = 42;
    tqueue_enqueue(&q,(void*)&data1);

   int data2 = 7;
   tqueue_enqueue(&q,(void*)&data2);

    assert(tqueue_get_data(q) == (void*)&data1);

    assert(tqueue_size(q) == 2);

    assert(tqueue_pop(&q) == (void*)&data1);



    assert(tqueue_size(q) == 1);

    assert(tqueue_at_offset(q,0) == q);

    TQueue newQueue=tqueue_at_offset(q,1);
    tqueue_pop(&q);
    assert(newQueue == q);

}

int main(){
    tqueue_test();
    return 0;
}