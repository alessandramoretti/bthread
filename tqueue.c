//
// Created by alemo on 05.10.2020.
//

#include "tqueue.h"

typedef struct TQueueNode {
    struct TQueueNode* next;
    void* data;
} TQueueNode;