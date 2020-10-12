//
// Created by alemo on 05.10.2020.
//

#include "tqueue.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct TQueueNode {
    struct TQueueNode* next;
    void* data;
} TQueueNode;

unsigned long int tqueue_enqueue(TQueue* q, void* data){
    TQueueNode* newNode=(TQueueNode*) malloc(sizeof(TQueueNode));
    newNode->data=data;
    int position=0;
    if(*q == NULL){
        *q = newNode;
    }else{
        ++position;
        TQueueNode* currentNode=*q;
        while(currentNode->next != *q){
            ++position;
            currentNode=currentNode->next;
        }
        currentNode->next=newNode;
    }
    newNode->next=*q;
    return position;
}

void *tqueue_pop(TQueue* q){
    if(*q == NULL){
        return NULL;
    } else{
        TQueueNode* head = *q;
        void* data = head->data;

        while((*q)->next != head){
            *q = (*q)->next;
        }
        TQueueNode* tail = *q;
        tail->next = head->next;
        free(head);

        return data;
    }
}

unsigned long int tqueue_size(TQueue q){
    unsigned long int size = 0;

    if(q == NULL){
        return size;
    } else{
        ++size;
        TQueueNode* head = q;

        while(q->next != head){
            q = q->next;
            size++;
        }

        return size;
    }
}

TQueue tqueue_at_offset(TQueue q, unsigned long int offset){
    if(q == NULL){
        return NULL;
    } else{
        while(offset > 0){
            q = q->next;
            offset--;
        }

        return q;
    }
}

void* tqueue_get_data(TQueue q){
    if(q == NULL){
        return NULL;
    } else{
        return q->data;
    }
}