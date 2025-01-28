#include "static_queue.h"
#include <stdio.h>

typedef struct {
    int32_t number;
    staticQueueItem_t node;
} myList_t;

#define LIST_LEN 4

static int32_t queuePut(staticQueue_t* queue,
                        uint32_t       data)
{
    // Get the next item to which we want to write data
    staticQueueItem_t* item;
    int32_t            result = staticQueuePut(queue, &item);

    // Add write data to the queue item
    if (result == STATIC_QUEUE_SUCCESS) {
        myList_t* next = CONTAINER_OF(item, myList_t, node);
        next->number = data;
    }

    return result;
}

static int32_t queuePutFirst(staticQueue_t* queue,
                             uint32_t       data)
{
    // Get relevant item from the queue
    staticQueueItem_t* item;
    int32_t            result = staticQueuePutFirst(queue, &item);

    // Copy the data to the queue
    if (result == STATIC_QUEUE_SUCCESS) {
        myList_t* next = CONTAINER_OF(item, myList_t, node);
        next->number = data;
    }

    return result;
}

static int32_t queuePop(staticQueue_t* queue,
                        uint32_t*      data)
{
    // Get the next item from the queue
    staticQueueItem_t* item;
    int32_t            result = staticQueuePop(queue, &item);

    // Copy the data to the input parameter
    if (result == STATIC_QUEUE_SUCCESS) {
        myList_t* queue_item = CONTAINER_OF(item, myList_t, node);
        *data = queue_item->number;
    }

    return result;
}

static int32_t queuePeak(staticQueue_t* queue,
                               uint32_t*      data)
{
    // Get the next item from the queue witout removing it
    staticQueueItem_t* item;
    int32_t            result = staticQueuePeak(queue, &item);

    // Copy the data to the input parameter
    if (result == STATIC_QUEUE_SUCCESS) {
        myList_t* queue_item = CONTAINER_OF(item, myList_t, node);
        *data = queue_item->number;
    }

    return result;
}

static int32_t queueClear(staticQueue_t* queue)
{
    return staticQueueClear(queue);
}

#define FIRST_DATA  10
#define SECOND_DATA 11
#define THIRD_DATA  1337
#define FOURTH_DATA 59

int main() {

    staticQueue_t queue;
    myList_t      my_list[LIST_LEN] = {0};

    int32_t result = staticQueueInit(&queue, LIST_LEN, sizeof(myList_t), &my_list->node);

    uint32_t data = 0;

    result = queuePut(&queue, FIRST_DATA);
    if (result != STATIC_QUEUE_SUCCESS) {
        printf("queue put failed %i\n", result);
        return 1;
    } else {
        printf("Put Data %i\n", FIRST_DATA);
    }

    result = queuePut(&queue, SECOND_DATA);
    if (result != STATIC_QUEUE_SUCCESS) {
        printf("queue put failed %i\n", result);
        return 1;
    } else {
        printf("Put Data %i\n", SECOND_DATA);
    }

    result = queuePut(&queue, THIRD_DATA);
    if (result != STATIC_QUEUE_SUCCESS) {
        printf("queue put failed %i\n", result);
        return 1;
    } else {
        printf("Put Data %i\n", THIRD_DATA);
    }

    result = queuePut(&queue, FOURTH_DATA);
    if (result != STATIC_QUEUE_SUCCESS) {
        printf("queue put failed %i\n", result);
        return 1;
    } else {
        printf("Put Data %i\n", FOURTH_DATA);
    }

    result = queuePut(&queue, FOURTH_DATA);
    if (result != STATIC_QUEUE_FULL) {
        printf("queue put failed, expected full %i\n", result);
        return 1;
    } else {
        printf("Success Queue full\n");
    }

    bool full = staticQueuefull(&queue);
    if (!full) {
        printf("queue check full failed, expected full %i\n", full);
    }

    result = queuePop(&queue, &data);
    if (result != STATIC_QUEUE_SUCCESS) {
        printf("queue pop failed %i\n", result);
        return 1;
    } else {
        if (data == FIRST_DATA) {
            printf("Pop Data %i\n", data);
        } else {
            printf("Pop Data mismatch %i\n", data);
            return 1;
        }
    }

    result = queuePop(&queue, &data);
    if (result != STATIC_QUEUE_SUCCESS) {
        printf("queue pop failed %i\n", result);
        return 1;
    } else {
        if (data == SECOND_DATA) {
            printf("Pop Data %i\n", data);
        } else {
            printf("Pop Data mismatch %i\n", data);
            return 1;
        }
    }

    result = queuePop(&queue, &data);
    if (result != STATIC_QUEUE_SUCCESS) {
        printf("queue pop failed %i\n", result);
        return 1;
    } else {
        if (data == THIRD_DATA) {
            printf("Pop Data %i\n", data);
        } else {
            printf("Pop Data mismatch %i\n", data);
            return 1;
        }
    }

    result = queuePop(&queue, &data);
    if (result != STATIC_QUEUE_SUCCESS) {
        printf("queue pop failed %i\n", result);
        return 1;
    } else {
        if (data == FOURTH_DATA) {
            printf("Pop Data %i\n", data);
        } else {
            printf("Pop Data mismatch %i\n", data);
            return 1;
        }
    }

    result = queuePop(&queue, &data);
    if (result != STATIC_QUEUE_EMPTY) {
        printf("queue pop empty failed, expeced empty %i\n", result);
        return 1;
    } else {
        printf("Queue check Empty Success %i\n", result);
    }

    bool empty = staticQueueEmpty(&queue);
    if (!empty) {
        printf("queue check empty failed, expected full %i\n", empty);
    }

    result = queuePut(&queue, FIRST_DATA);
    if (result != STATIC_QUEUE_SUCCESS) {
        printf("queue put failed %i\n", result);
        return 1;
    } else {
        printf("Put Data %i\n", FIRST_DATA);
    }

    result = queuePutFirst(&queue, SECOND_DATA);
    if (result != STATIC_QUEUE_SUCCESS) {
        printf("queue put First failed %i\n", result);
        return 1;
    } else {
        printf("Put First Data %i\n", SECOND_DATA);
    }

    result = queuePut(&queue, THIRD_DATA);
    if (result != STATIC_QUEUE_SUCCESS) {
        printf("queue put failed %i\n", result);
        return 1;
    } else {
        printf("Put Data %i\n", THIRD_DATA);
    }

    result = queuePutFirst(&queue, FOURTH_DATA);
    if (result != STATIC_QUEUE_SUCCESS) {
        printf("queue put First failed %i\n", result);
        return 1;
    } else {
        printf("Put First Data %i\n", FOURTH_DATA);
    }

    result = queuePop(&queue, &data);
    if (result != STATIC_QUEUE_SUCCESS) {
        printf("queue pop failed %i\n", result);
        return 1;
    } else {
        if (data == FOURTH_DATA) {
            printf("Pop Data %i\n", data);
        } else {
            printf("Pop Data mismatch %i\n", data);
            return 1;
        }
    }

    result = queuePop(&queue, &data);
    if (result != STATIC_QUEUE_SUCCESS) {
        printf("queue pop failed %i\n", result);
        return 1;
    } else {
        if (data == SECOND_DATA) {
            printf("Pop Data %i\n", data);
        } else {
            printf("Pop Data mismatch %i\n", data);
            return 1;
        }
    }

    result = queuePop(&queue, &data);
    if (result != STATIC_QUEUE_SUCCESS) {
        printf("queue pop failed %i\n", result);
        return 1;
    } else {
        if (data == FIRST_DATA) {
            printf("Pop Data %i\n", data);
        } else {
            printf("Pop Data mismatch %i\n", data);
            return 1;
        }
    }

    result = queuePop(&queue, &data);
    if (result != STATIC_QUEUE_SUCCESS) {
        printf("queue pop failed %i\n", result);
        return 1;
    } else {
        if (data == THIRD_DATA) {
            printf("Pop Data %i\n", data);
        } else {
            printf("Pop Data mismatch %i\n", data);
            return 1;
        }
    }

    result = queuePut(&queue, THIRD_DATA);
    if (result != STATIC_QUEUE_SUCCESS) {
        printf("queue put failed %i\n", result);
        return 1;
    } else {
        printf("Put Data %i\n", THIRD_DATA);
    }

    result = queuePeak(&queue, &data);
    if (result != STATIC_QUEUE_SUCCESS) {
        printf("queue peak failed %i\n", result);
        return 1;
    } else {
        if (data == THIRD_DATA) {
            printf("Peak Data %i\n", data);
        } else {
            printf("Peak Data mismatch %i\n", data);
            return 1;
        }
    }

    result = queuePutFirst(&queue, FOURTH_DATA);
    if (result != STATIC_QUEUE_SUCCESS) {
        printf("queue put First failed %i\n", result);
        return 1;
    } else {
        printf("Put First Data %i\n", FOURTH_DATA);
    }

    result = queuePeak(&queue, &data);
    if (result != STATIC_QUEUE_SUCCESS) {
        printf("queue Peak failed %i\n", result);
        return 1;
    } else {
        if (data == FOURTH_DATA) {
            printf("Peak Data %i\n", data);
        } else {
            printf("Peak Data mismatch %i\n", data);
            return 1;
        }
    }

    result = queueClear(&queue);
    if (result != STATIC_QUEUE_SUCCESS) {
        printf("Queue clear fail %i\n", result);
    }

    result = queuePeak(&queue, &data);
    if (result != STATIC_QUEUE_EMPTY) {
        printf("queue Peak failed , expected emtpy %i\n", result);
        return 1;
    } else {
        printf("Peak Data empty success %i\n", result);
    }


    // Connect first driver and app
    printf("Test Done\n");
}