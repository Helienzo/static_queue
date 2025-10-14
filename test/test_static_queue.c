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

    // ===== Test staticQueueErase function =====
    printf("\n=== Testing staticQueueErase ===\n");

    // Test 1: Erase from a queue with single item
    printf("Test 1: Erase single item from queue\n");
    result = queuePut(&queue, FIRST_DATA);
    if (result != STATIC_QUEUE_SUCCESS) {
        printf("queue put failed %i\n", result);
        return 1;
    }

    staticQueueItem_t* item_to_erase;
    result = staticQueuePeak(&queue, &item_to_erase);
    if (result != STATIC_QUEUE_SUCCESS) {
        printf("queue peak failed %i\n", result);
        return 1;
    }

    result = staticQueueErase(&queue, item_to_erase);
    if (result != STATIC_QUEUE_SUCCESS) {
        printf("queue erase failed %i\n", result);
        return 1;
    }

    if (!staticQueueEmpty(&queue)) {
        printf("queue should be empty after erasing single item\n");
        return 1;
    }
    printf("Test 1 passed: Single item erased successfully\n");

    // Test 2: Erase tail item from queue with multiple items
    printf("\nTest 2: Erase tail (oldest) item\n");
    queueClear(&queue);
    queuePut(&queue, FIRST_DATA);
    queuePut(&queue, SECOND_DATA);
    queuePut(&queue, THIRD_DATA);

    result = staticQueuePeak(&queue, &item_to_erase);
    if (result != STATIC_QUEUE_SUCCESS) {
        printf("queue peak failed %i\n", result);
        return 1;
    }

    result = staticQueueErase(&queue, item_to_erase);
    if (result != STATIC_QUEUE_SUCCESS) {
        printf("queue erase failed %i\n", result);
        return 1;
    }

    // Next pop should return SECOND_DATA (not FIRST_DATA)
    result = queuePop(&queue, &data);
    if (result != STATIC_QUEUE_SUCCESS || data != SECOND_DATA) {
        printf("Expected SECOND_DATA (%i) after erasing tail, got %i\n", SECOND_DATA, data);
        return 1;
    }
    printf("Test 2 passed: Tail item erased, next item is correct\n");

    // Test 3: Erase head item (newest item before head)
    printf("\nTest 3: Erase newest item (before head)\n");
    queueClear(&queue);
    queuePut(&queue, FIRST_DATA);
    queuePut(&queue, SECOND_DATA);
    queuePut(&queue, THIRD_DATA);

    // Get reference to the last item we put (THIRD_DATA)
    staticQueueItem_t* newest_item = queue.head->last;

    result = staticQueueErase(&queue, newest_item);
    if (result != STATIC_QUEUE_SUCCESS) {
        printf("queue erase failed %i\n", result);
        return 1;
    }

    // Should still be able to pop FIRST_DATA and SECOND_DATA
    result = queuePop(&queue, &data);
    if (result != STATIC_QUEUE_SUCCESS || data != FIRST_DATA) {
        printf("Expected FIRST_DATA (%i), got %i\n", FIRST_DATA, data);
        return 1;
    }

    result = queuePop(&queue, &data);
    if (result != STATIC_QUEUE_SUCCESS || data != SECOND_DATA) {
        printf("Expected SECOND_DATA (%i), got %i\n", SECOND_DATA, data);
        return 1;
    }

    // Queue should be empty now
    if (!staticQueueEmpty(&queue)) {
        printf("queue should be empty\n");
        return 1;
    }
    printf("Test 3 passed: Newest item erased successfully\n");

    // Test 4: Erase middle item
    printf("\nTest 4: Erase middle item from queue\n");
    queueClear(&queue);
    staticQueueItem_t* first_item;
    staticQueueItem_t* middle_item;
    staticQueueItem_t* third_item;

    result = staticQueuePut(&queue, &first_item);
    myList_t* first = CONTAINER_OF(first_item, myList_t, node);
    first->number = FIRST_DATA;

    result = staticQueuePut(&queue, &middle_item);
    myList_t* middle = CONTAINER_OF(middle_item, myList_t, node);
    middle->number = SECOND_DATA;

    result = staticQueuePut(&queue, &third_item);
    myList_t* third = CONTAINER_OF(third_item, myList_t, node);
    third->number = THIRD_DATA;

    // Erase the middle item
    result = staticQueueErase(&queue, middle_item);
    if (result != STATIC_QUEUE_SUCCESS) {
        printf("queue erase failed %i\n", result);
        return 1;
    }

    // Pop should return FIRST_DATA
    result = queuePop(&queue, &data);
    if (result != STATIC_QUEUE_SUCCESS || data != FIRST_DATA) {
        printf("Expected FIRST_DATA (%i), got %i\n", FIRST_DATA, data);
        return 1;
    }

    // Next pop should return THIRD_DATA (skipping the erased SECOND_DATA)
    result = queuePop(&queue, &data);
    if (result != STATIC_QUEUE_SUCCESS || data != THIRD_DATA) {
        printf("Expected THIRD_DATA (%i), got %i\n", THIRD_DATA, data);
        return 1;
    }

    // Queue should be empty
    if (!staticQueueEmpty(&queue)) {
        printf("queue should be empty\n");
        return 1;
    }
    printf("Test 4 passed: Middle item erased successfully\n");

    // Test 5: Try to erase already inactive item
    printf("\nTest 5: Erase inactive item (should fail)\n");
    queueClear(&queue);
    queuePut(&queue, FIRST_DATA);

    staticQueueItem_t* popped_item;
    result = staticQueuePop(&queue, &popped_item);
    staticQueueItem_t* inactive_item = popped_item;

    result = staticQueueErase(&queue, inactive_item);
    if (result != STATIC_QUEUE_EMPTY) {
        printf("Expected STATIC_QUEUE_EMPTY when erasing inactive item, got %i\n", result);
        return 1;
    }
    printf("Test 5 passed: Cannot erase inactive item\n");

    // Test 6: Erase all items one by one
    printf("\nTest 6: Erase all items sequentially\n");
    queueClear(&queue);
    staticQueueItem_t* items[3];

    result = staticQueuePut(&queue, &items[0]);
    myList_t* list_item0 = CONTAINER_OF(items[0], myList_t, node);
    list_item0->number = FIRST_DATA;

    result = staticQueuePut(&queue, &items[1]);
    myList_t* list_item1 = CONTAINER_OF(items[1], myList_t, node);
    list_item1->number = SECOND_DATA;

    result = staticQueuePut(&queue, &items[2]);
    myList_t* list_item2 = CONTAINER_OF(items[2], myList_t, node);
    list_item2->number = THIRD_DATA;

    // Erase all items
    for (int i = 0; i < 3; i++) {
        result = staticQueueErase(&queue, items[i]);
        if (result != STATIC_QUEUE_SUCCESS) {
            printf("Failed to erase item %i, error: %i\n", i, result);
            return 1;
        }
    }

    // Queue should be empty
    if (!staticQueueEmpty(&queue)) {
        printf("queue should be empty after erasing all items\n");
        return 1;
    }
    printf("Test 6 passed: All items erased successfully\n");

    printf("\n=== All staticQueueErase tests passed ===\n");

    // Test 7: Verify queue capacity is maintained after erase operations
    printf("\nTest 7: Verify full capacity after erase operations\n");
    queueClear(&queue);

    // Fill queue partially
    queuePut(&queue, FIRST_DATA);
    queuePut(&queue, SECOND_DATA);
    queuePut(&queue, THIRD_DATA);

    // Erase middle item
    result = staticQueuePeak(&queue, &item_to_erase);
    item_to_erase = item_to_erase->next; // Get second item
    result = staticQueueErase(&queue, item_to_erase);

    // Pop remaining items
    queuePop(&queue, &data); // Should be FIRST_DATA
    queuePop(&queue, &data); // Should be THIRD_DATA

    // Now queue should be empty and we should be able to fill it completely
    if (!staticQueueEmpty(&queue)) {
        printf("Queue should be empty before filling\n");
        return 1;
    }

    // Fill queue to maximum capacity (LIST_LEN = 4) - add items one by one
    result = queuePut(&queue, 100);
    if (result != STATIC_QUEUE_SUCCESS) {
        printf("Failed to add item 1\n");
        return 1;
    }

    result = queuePut(&queue, 200);
    if (result != STATIC_QUEUE_SUCCESS) {
        printf("Failed to add item 2\n");
        return 1;
    }

    result = queuePut(&queue, 300);
    if (result != STATIC_QUEUE_SUCCESS) {
        printf("Failed to add item 3\n");
        return 1;
    }

    result = queuePut(&queue, 400);
    if (result != STATIC_QUEUE_SUCCESS) {
        printf("Failed to add item 4\n");
        return 1;
    }

    // Verify queue is full
    if (!staticQueuefull(&queue)) {
        printf("Queue should be full after filling all 4 slots\n");
        return 1;
    }

    // Try to add one more (should fail)
    result = queuePut(&queue, 500);
    if (result != STATIC_QUEUE_FULL) {
        printf("Expected STATIC_QUEUE_FULL when adding 5th item, got %i\n", result);
        return 1;
    }

    // Pop all items one by one and verify order
    result = queuePop(&queue, &data);
    if (result != STATIC_QUEUE_SUCCESS || data != 100) {
        printf("Expected first item to be 100, got %u (result: %i)\n", data, result);
        return 1;
    }

    result = queuePop(&queue, &data);
    if (result != STATIC_QUEUE_SUCCESS || data != 200) {
        printf("Expected second item to be 200, got %u (result: %i)\n", data, result);
        return 1;
    }

    result = queuePop(&queue, &data);
    if (result != STATIC_QUEUE_SUCCESS || data != 300) {
        printf("Expected third item to be 300, got %u (result: %i)\n", data, result);
        return 1;
    }

    result = queuePop(&queue, &data);
    if (result != STATIC_QUEUE_SUCCESS || data != 400) {
        printf("Expected fourth item to be 400, got %u (result: %i)\n", data, result);
        return 1;
    }

    // Verify queue is empty
    if (!staticQueueEmpty(&queue)) {
        printf("Queue should be empty after popping all items\n");
        return 1;
    }

    printf("Test 7 passed: Full capacity maintained, correct order preserved\n");

    // Test 8: Verify tail never points to inactive item
    printf("\nTest 8: Tail advancement after erasing middle then tail\n");
    queueClear(&queue);

    staticQueueItem_t* item_a;
    staticQueueItem_t* item_b;
    staticQueueItem_t* item_c;

    // Add three items
    result = staticQueuePut(&queue, &item_a);
    myList_t* list_a = CONTAINER_OF(item_a, myList_t, node);
    list_a->number = 100;

    result = staticQueuePut(&queue, &item_b);
    myList_t* list_b = CONTAINER_OF(item_b, myList_t, node);
    list_b->number = 200;

    result = staticQueuePut(&queue, &item_c);
    myList_t* list_c = CONTAINER_OF(item_c, myList_t, node);
    list_c->number = 300;

    // Erase middle item (B)
    result = staticQueueErase(&queue, item_b);
    if (result != STATIC_QUEUE_SUCCESS) {
        printf("Failed to erase middle item\n");
        return 1;
    }

    // Now: A(tail, active) -> B(inactive) -> C(active)
    // Pop A - tail should skip over B and point to C
    result = queuePop(&queue, &data);
    if (result != STATIC_QUEUE_SUCCESS || data != 100) {
        printf("Failed to pop A correctly\n");
        return 1;
    }

    // Peak should return C (not the inactive B)
    result = queuePeak(&queue, &data);
    if (result != STATIC_QUEUE_SUCCESS || data != 300) {
        printf("Peak should return C (300), got %u\n", data);
        return 1;
    }

    // Pop C
    result = queuePop(&queue, &data);
    if (result != STATIC_QUEUE_SUCCESS || data != 300) {
        printf("Failed to pop C correctly\n");
        return 1;
    }

    // Queue should now be empty
    if (!staticQueueEmpty(&queue)) {
        printf("Queue should be empty\n");
        return 1;
    }

    printf("Test 8 passed: Tail correctly skips inactive items\n");

    printf("\n=== All staticQueueErase tests passed ===\n");

    // Connect first driver and app
    printf("\nTest Done\n");
}