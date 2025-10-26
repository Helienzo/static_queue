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

// Global counters for ForEach callbacks
static int32_t g_foreach_counter = 0;
static int32_t g_foreach_sum = 0;

// ForEach callback functions
static int32_t countCallback(staticQueue_t *q, staticQueueItem_t *item) {
    (void)q;  // Unused
    myList_t* list_item = CONTAINER_OF(item, myList_t, node);
    g_foreach_counter++;
    g_foreach_sum += list_item->number;
    return STATIC_QUEUE_CB_NEXT;
}

static int32_t stopCallback(staticQueue_t *q, staticQueueItem_t *item) {
    (void)q;  // Unused
    (void)item;  // Unused
    g_foreach_counter++;
    if (g_foreach_counter >= 2) {
        return STATIC_QUEUE_CB_STOP;
    }
    return STATIC_QUEUE_CB_NEXT;
}

static int32_t eraseCallback(staticQueue_t *q, staticQueueItem_t *item) {
    (void)q;  // Unused
    myList_t* list_item = CONTAINER_OF(item, myList_t, node);
    if (list_item->number > 25) {
        return STATIC_QUEUE_CB_ERASE;
    }
    return STATIC_QUEUE_CB_NEXT;
}

static int32_t eraseAllCallback(staticQueue_t *q, staticQueueItem_t *item) {
    (void)q;  // Unused
    (void)item;  // Unused
    return STATIC_QUEUE_CB_ERASE;
}

static int32_t eraseEvenCallback(staticQueue_t *q, staticQueueItem_t *item) {
    (void)q;  // Unused
    myList_t* list_item = CONTAINER_OF(item, myList_t, node);
    if (list_item->number % 2 == 0) {
        return STATIC_QUEUE_CB_ERASE;
    }
    return STATIC_QUEUE_CB_NEXT;
}

static int32_t errorCallback(staticQueue_t *q, staticQueueItem_t *item) {
    (void)q;  // Unused
    (void)item;  // Unused
    g_foreach_counter++;
    if (g_foreach_counter == 3) {
        return -999;  // Custom error
    }
    return STATIC_QUEUE_CB_NEXT;
}

static int32_t eraseOnly20Callback(staticQueue_t *q, staticQueueItem_t *item) {
    (void)q;  // Unused
    myList_t* list_item = CONTAINER_OF(item, myList_t, node);
    if (list_item->number == 20) {
        return STATIC_QUEUE_CB_ERASE;
    }
    return STATIC_QUEUE_CB_NEXT;
}

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

    // Test 9: Multiple consecutive middle erases
    printf("\nTest 9: Multiple consecutive middle item erases\n");
    queueClear(&queue);

    staticQueueItem_t* items_9[LIST_LEN];
    for (int i = 0; i < LIST_LEN; i++) {
        result = staticQueuePut(&queue, &items_9[i]);
        myList_t* list_item = CONTAINER_OF(items_9[i], myList_t, node);
        list_item->number = (i + 1) * 100;  // 100, 200, 300, 400
    }

    // Erase items 1 and 2 (200 and 300) - both middle items
    result = staticQueueErase(&queue, items_9[1]);
    if (result != STATIC_QUEUE_SUCCESS) {
        printf("Failed to erase first middle item\n");
        return 1;
    }

    result = staticQueueErase(&queue, items_9[2]);
    if (result != STATIC_QUEUE_SUCCESS) {
        printf("Failed to erase second middle item\n");
        return 1;
    }

    // Should pop 100 then 400
    result = queuePop(&queue, &data);
    if (result != STATIC_QUEUE_SUCCESS || data != 100) {
        printf("Expected 100, got %u\n", data);
        return 1;
    }

    result = queuePop(&queue, &data);
    if (result != STATIC_QUEUE_SUCCESS || data != 400) {
        printf("Expected 400, got %u\n", data);
        return 1;
    }

    if (!staticQueueEmpty(&queue)) {
        printf("Queue should be empty\n");
        return 1;
    }
    printf("Test 9 passed: Multiple middle erases handled correctly\n");

    // Test 10: Erase from full queue, then refill
    printf("\nTest 10: Erase from full queue and refill\n");
    queueClear(&queue);

    // Fill queue completely
    for (int i = 0; i < LIST_LEN; i++) {
        result = queuePut(&queue, (i + 1) * 10);
        if (result != STATIC_QUEUE_SUCCESS) {
            printf("Failed to fill queue\n");
            return 1;
        }
    }

    if (!staticQueuefull(&queue)) {
        printf("Queue should be full\n");
        return 1;
    }

    // Get reference to second item and erase it
    staticQueueItem_t* second_item = queue.tail->next;
    result = staticQueueErase(&queue, second_item);
    if (result != STATIC_QUEUE_SUCCESS) {
        printf("Failed to erase from full queue\n");
        return 1;
    }

    // Queue should no longer be full
    if (staticQueuefull(&queue)) {
        printf("Queue should not be full after erase\n");
        return 1;
    }

    // Should be able to add one more item
    result = queuePut(&queue, 999);
    if (result != STATIC_QUEUE_SUCCESS) {
        printf("Should be able to add item after erase\n");
        return 1;
    }

    printf("Test 10 passed: Can refill queue after erase from full\n");

    // Test 11: Wrap-around with erase
    printf("\nTest 11: Circular buffer wrap-around with erase\n");
    queueClear(&queue);

    // Fill queue
    queuePut(&queue, 1);
    queuePut(&queue, 2);
    queuePut(&queue, 3);
    queuePut(&queue, 4);

    // Pop two items (wrap tail forward)
    queuePop(&queue, &data);
    queuePop(&queue, &data);

    // Add two more items (wrap head forward)
    queuePut(&queue, 5);
    queuePut(&queue, 6);

    // Now queue has: 3, 4, 5, 6 (but circular buffer is wrapped)
    // Erase item 4
    staticQueueItem_t* item_to_erase_11 = queue.tail->next;
    result = staticQueueErase(&queue, item_to_erase_11);
    if (result != STATIC_QUEUE_SUCCESS) {
        printf("Failed to erase in wrapped state\n");
        return 1;
    }

    // Pop remaining items in order: 3, 5, 6
    result = queuePop(&queue, &data);
    if (result != STATIC_QUEUE_SUCCESS || data != 3) {
        printf("Expected 3, got %u\n", data);
        return 1;
    }

    result = queuePop(&queue, &data);
    if (result != STATIC_QUEUE_SUCCESS || data != 5) {
        printf("Expected 5, got %u\n", data);
        return 1;
    }

    result = queuePop(&queue, &data);
    if (result != STATIC_QUEUE_SUCCESS || data != 6) {
        printf("Expected 6, got %u\n", data);
        return 1;
    }

    printf("Test 11 passed: Wrap-around with erase works correctly\n");

    // Test 12: Peak after multiple erases
    printf("\nTest 12: Peak after multiple erases\n");
    queueClear(&queue);

    staticQueueItem_t* items_12[LIST_LEN];
    for (int i = 0; i < LIST_LEN; i++) {
        result = staticQueuePut(&queue, &items_12[i]);
        myList_t* list_item = CONTAINER_OF(items_12[i], myList_t, node);
        list_item->number = (i + 1) * 10;  // 10, 20, 30, 40
    }

    // Peak should return 10
    result = queuePeak(&queue, &data);
    if (result != STATIC_QUEUE_SUCCESS || data != 10) {
        printf("Peak should return 10, got %u\n", data);
        return 1;
    }

    // Erase first item (10)
    result = staticQueueErase(&queue, items_12[0]);
    if (result != STATIC_QUEUE_SUCCESS) {
        printf("Failed to erase first item\n");
        return 1;
    }

    // Peak should now return 20
    result = queuePeak(&queue, &data);
    if (result != STATIC_QUEUE_SUCCESS || data != 20) {
        printf("Peak should return 20 after erase, got %u\n", data);
        return 1;
    }

    // Erase second item (20)
    result = staticQueueErase(&queue, items_12[1]);
    if (result != STATIC_QUEUE_SUCCESS) {
        printf("Failed to erase second item\n");
        return 1;
    }

    // Peak should now return 30
    result = queuePeak(&queue, &data);
    if (result != STATIC_QUEUE_SUCCESS || data != 30) {
        printf("Peak should return 30 after second erase, got %u\n", data);
        return 1;
    }

    printf("Test 12 passed: Peak always returns active item after erases\n");

    // Test 13: Erase same item twice
    printf("\nTest 13: Erase same item twice (should fail)\n");
    queueClear(&queue);

    staticQueueItem_t* dup_item;
    result = staticQueuePut(&queue, &dup_item);
    myList_t* dup_list = CONTAINER_OF(dup_item, myList_t, node);
    dup_list->number = 777;

    result = staticQueueErase(&queue, dup_item);
    if (result != STATIC_QUEUE_SUCCESS) {
        printf("First erase should succeed\n");
        return 1;
    }

    // Try to erase again - should fail
    result = staticQueueErase(&queue, dup_item);
    if (result != STATIC_QUEUE_EMPTY) {
        printf("Second erase should fail with STATIC_QUEUE_EMPTY, got %i\n", result);
        return 1;
    }

    printf("Test 13 passed: Cannot erase same item twice\n");

    // Test 14: Erase all but one item, then operate
    printf("\nTest 14: Erase all but one, then operations\n");
    queueClear(&queue);

    staticQueueItem_t* items_14[LIST_LEN];
    for (int i = 0; i < LIST_LEN; i++) {
        result = staticQueuePut(&queue, &items_14[i]);
        myList_t* list_item = CONTAINER_OF(items_14[i], myList_t, node);
        list_item->number = (i + 1) * 111;  // 111, 222, 333, 444
    }

    // Erase first 3 items
    for (int i = 0; i < 3; i++) {
        result = staticQueueErase(&queue, items_14[i]);
        if (result != STATIC_QUEUE_SUCCESS) {
            printf("Failed to erase item %i\n", i);
            return 1;
        }
    }

    // Queue should not be empty (one item left)
    if (staticQueueEmpty(&queue)) {
        printf("Queue should not be empty (one item left)\n");
        return 1;
    }

    // Peak should return last item
    result = queuePeak(&queue, &data);
    if (result != STATIC_QUEUE_SUCCESS || data != 444) {
        printf("Peak should return 444, got %u\n", data);
        return 1;
    }

    // Pop should return last item
    result = queuePop(&queue, &data);
    if (result != STATIC_QUEUE_SUCCESS || data != 444) {
        printf("Pop should return 444, got %u\n", data);
        return 1;
    }

    // Now queue should be empty
    if (!staticQueueEmpty(&queue)) {
        printf("Queue should be empty after popping last item\n");
        return 1;
    }

    // Should be able to add items again
    result = queuePut(&queue, 888);
    if (result != STATIC_QUEUE_SUCCESS) {
        printf("Should be able to add after clearing\n");
        return 1;
    }

    printf("Test 14 passed: Single item operations after multiple erases\n");

    // Test 15: Alternating erase and pop operations
    printf("\nTest 15: Alternating erase and pop\n");
    queueClear(&queue);

    staticQueueItem_t* items_15[LIST_LEN];
    for (int i = 0; i < LIST_LEN; i++) {
        result = staticQueuePut(&queue, &items_15[i]);
        myList_t* list_item = CONTAINER_OF(items_15[i], myList_t, node);
        list_item->number = (i + 1) * 5;  // 5, 10, 15, 20
    }

    // Pop first item (5)
    result = queuePop(&queue, &data);
    if (result != STATIC_QUEUE_SUCCESS || data != 5) {
        printf("Expected 5, got %u\n", data);
        return 1;
    }

    // Erase third item (15)
    result = staticQueueErase(&queue, items_15[2]);
    if (result != STATIC_QUEUE_SUCCESS) {
        printf("Failed to erase item\n");
        return 1;
    }

    // Pop second item (10)
    result = queuePop(&queue, &data);
    if (result != STATIC_QUEUE_SUCCESS || data != 10) {
        printf("Expected 10, got %u\n", data);
        return 1;
    }

    // Erase last item (20)
    result = staticQueueErase(&queue, items_15[3]);
    if (result != STATIC_QUEUE_SUCCESS) {
        printf("Failed to erase last item\n");
        return 1;
    }

    // Queue should be empty
    if (!staticQueueEmpty(&queue)) {
        printf("Queue should be empty\n");
        return 1;
    }

    printf("Test 15 passed: Alternating erase and pop works correctly\n");

    // Test 16: Erase after PutFirst
    printf("\nTest 16: Erase after PutFirst operations\n");
    queueClear(&queue);

    result = queuePut(&queue, 100);
    result = queuePutFirst(&queue, 50);  // Should be at tail now
    result = queuePut(&queue, 150);

    // Queue order: 50 (tail), 100, 150 (before head)
    // Peak should return 50
    result = queuePeak(&queue, &data);
    if (result != STATIC_QUEUE_SUCCESS || data != 50) {
        printf("Peak should return 50, got %u\n", data);
        return 1;
    }

    // Erase the tail item (50)
    staticQueueItem_t* putfirst_item = queue.tail;
    result = staticQueueErase(&queue, putfirst_item);
    if (result != STATIC_QUEUE_SUCCESS) {
        printf("Failed to erase PutFirst item\n");
        return 1;
    }

    // Should now pop 100, 150
    result = queuePop(&queue, &data);
    if (result != STATIC_QUEUE_SUCCESS || data != 100) {
        printf("Expected 100, got %u\n", data);
        return 1;
    }

    result = queuePop(&queue, &data);
    if (result != STATIC_QUEUE_SUCCESS || data != 150) {
        printf("Expected 150, got %u\n", data);
        return 1;
    }

    printf("Test 16 passed: Erase works correctly with PutFirst\n");

    // Test 17: Erase tail item when queue is full
    printf("\nTest 17: Erase tail item from full queue\n");
    queueClear(&queue);

    // Fill queue completely
    staticQueueItem_t* items_17[LIST_LEN];
    for (int i = 0; i < LIST_LEN; i++) {
        result = staticQueuePut(&queue, &items_17[i]);
        myList_t* list_item = CONTAINER_OF(items_17[i], myList_t, node);
        list_item->number = (i + 1) * 10;  // 10, 20, 30, 40
    }

    // Verify queue is full
    if (!staticQueuefull(&queue)) {
        printf("Queue should be full before erase\n");
        return 1;
    }

    // Erase tail item (10)
    result = staticQueueErase(&queue, items_17[0]);
    if (result != STATIC_QUEUE_SUCCESS) {
        printf("Failed to erase tail from full queue\n");
        return 1;
    }

    // Queue should no longer be full
    if (staticQueuefull(&queue)) {
        printf("Queue should not be full after erasing tail\n");
        return 1;
    }

    // Should be able to add another item
    result = queuePut(&queue, 999);
    if (result != STATIC_QUEUE_SUCCESS) {
        printf("Should be able to add item after erase\n");
        return 1;
    }

    // Pop should return 20, 30, 40, 999
    result = queuePop(&queue, &data);
    if (result != STATIC_QUEUE_SUCCESS || data != 20) {
        printf("Expected 20, got %u\n", data);
        return 1;
    }

    result = queuePop(&queue, &data);
    if (result != STATIC_QUEUE_SUCCESS || data != 30) {
        printf("Expected 30, got %u\n", data);
        return 1;
    }

    result = queuePop(&queue, &data);
    if (result != STATIC_QUEUE_SUCCESS || data != 40) {
        printf("Expected 40, got %u\n", data);
        return 1;
    }

    result = queuePop(&queue, &data);
    if (result != STATIC_QUEUE_SUCCESS || data != 999) {
        printf("Expected 999, got %u\n", data);
        return 1;
    }

    printf("Test 17 passed: Erase tail from full queue works correctly\n");

    // Test 18: Erase head-1 item (newest) when queue is full
    printf("\nTest 18: Erase newest item from full queue\n");
    queueClear(&queue);

    // Fill queue completely
    staticQueueItem_t* items_18[LIST_LEN];
    for (int i = 0; i < LIST_LEN; i++) {
        result = staticQueuePut(&queue, &items_18[i]);
        myList_t* list_item = CONTAINER_OF(items_18[i], myList_t, node);
        list_item->number = (i + 1) * 100;  // 100, 200, 300, 400
    }

    // Verify queue is full
    if (!staticQueuefull(&queue)) {
        printf("Queue should be full before erase\n");
        return 1;
    }

    // Erase newest item (400 - the one before head)
    result = staticQueueErase(&queue, items_18[3]);
    if (result != STATIC_QUEUE_SUCCESS) {
        printf("Failed to erase newest item from full queue\n");
        return 1;
    }

    // Queue should no longer be full
    if (staticQueuefull(&queue)) {
        printf("Queue should not be full after erasing newest\n");
        return 1;
    }

    // Should be able to add another item
    result = queuePut(&queue, 777);
    if (result != STATIC_QUEUE_SUCCESS) {
        printf("Should be able to add item after erase\n");
        return 1;
    }

    // Pop should return 100, 200, 300, 777 (not 400)
    result = queuePop(&queue, &data);
    if (result != STATIC_QUEUE_SUCCESS || data != 100) {
        printf("Expected 100, got %u\n", data);
        return 1;
    }

    result = queuePop(&queue, &data);
    if (result != STATIC_QUEUE_SUCCESS || data != 200) {
        printf("Expected 200, got %u\n", data);
        return 1;
    }

    result = queuePop(&queue, &data);
    if (result != STATIC_QUEUE_SUCCESS || data != 300) {
        printf("Expected 300, got %u\n", data);
        return 1;
    }

    result = queuePop(&queue, &data);
    if (result != STATIC_QUEUE_SUCCESS || data != 777) {
        printf("Expected 777, got %u\n", data);
        return 1;
    }

    if (!staticQueueEmpty(&queue)) {
        printf("Queue should be empty\n");
        return 1;
    }

    printf("Test 18 passed: Erase newest from full queue works correctly\n");

    // Test 19: Erase multiple items from full queue
    printf("\nTest 19: Erase multiple items from full queue\n");
    queueClear(&queue);

    // Fill queue completely
    staticQueueItem_t* items_19[LIST_LEN];
    for (int i = 0; i < LIST_LEN; i++) {
        result = staticQueuePut(&queue, &items_19[i]);
        myList_t* list_item = CONTAINER_OF(items_19[i], myList_t, node);
        list_item->number = (i + 1) * 50;  // 50, 100, 150, 200
    }

    // Verify queue is full
    if (!staticQueuefull(&queue)) {
        printf("Queue should be full before erase\n");
        return 1;
    }

    // Erase tail (50)
    result = staticQueueErase(&queue, items_19[0]);
    if (result != STATIC_QUEUE_SUCCESS) {
        printf("Failed to erase first item\n");
        return 1;
    }

    // Erase newest (200)
    result = staticQueueErase(&queue, items_19[3]);
    if (result != STATIC_QUEUE_SUCCESS) {
        printf("Failed to erase newest item\n");
        return 1;
    }

    // Erase middle (150)
    result = staticQueueErase(&queue, items_19[2]);
    if (result != STATIC_QUEUE_SUCCESS) {
        printf("Failed to erase middle item\n");
        return 1;
    }

    // Only item 100 should remain
    result = queuePop(&queue, &data);
    if (result != STATIC_QUEUE_SUCCESS || data != 100) {
        printf("Expected 100, got %u\n", data);
        return 1;
    }

    // Queue should be empty
    if (!staticQueueEmpty(&queue)) {
        printf("Queue should be empty after popping last item\n");
        return 1;
    }

    // Should be able to fill queue again
    for (int i = 0; i < LIST_LEN; i++) {
        result = queuePut(&queue, i * 7);
        if (result != STATIC_QUEUE_SUCCESS) {
            printf("Failed to refill queue at position %d\n", i);
            return 1;
        }
    }

    if (!staticQueuefull(&queue)) {
        printf("Queue should be full after refilling\n");
        return 1;
    }

    printf("Test 19 passed: Multiple erases from full queue work correctly\n");

    printf("\n=== All extended tests passed ===\n");

    // ===== Test staticQueueGetNumItems function =====
    printf("\n=== Testing staticQueueGetNumItems ===\n");

    // Test: Empty queue
    printf("\nTest: GetNumItems on empty queue\n");
    queueClear(&queue);
    int32_t num = staticQueueGetNumItems(&queue);
    if (num != 0) {
        printf("Expected 0 items in empty queue, got %i\n", num);
        return 1;
    }
    printf("Passed: Empty queue has 0 items\n");

    // Test: Single item
    printf("\nTest: GetNumItems with single item\n");
    queuePut(&queue, 10);
    num = staticQueueGetNumItems(&queue);
    if (num != 1) {
        printf("Expected 1 item, got %i\n", num);
        return 1;
    }
    printf("Passed: Single item counted correctly\n");

    // Test: Multiple items (not full)
    printf("\nTest: GetNumItems with multiple items\n");
    queueClear(&queue);
    queuePut(&queue, 10);
    queuePut(&queue, 20);
    queuePut(&queue, 30);
    num = staticQueueGetNumItems(&queue);
    if (num != 3) {
        printf("Expected 3 items, got %i\n", num);
        return 1;
    }
    printf("Passed: Multiple items counted correctly\n");

    // Test: Full queue
    printf("\nTest: GetNumItems on full queue\n");
    queueClear(&queue);
    queuePut(&queue, 10);
    queuePut(&queue, 20);
    queuePut(&queue, 30);
    queuePut(&queue, 40);
    if (!staticQueuefull(&queue)) {
        printf("Queue should be full\n");
        return 1;
    }
    num = staticQueueGetNumItems(&queue);
    if (num != LIST_LEN) {
        printf("Expected %i items in full queue, got %i\n", LIST_LEN, num);
        return 1;
    }
    printf("Passed: Full queue counted correctly (%i items)\n", LIST_LEN);

    // Test: After pop
    printf("\nTest: GetNumItems after pop\n");
    queueClear(&queue);
    queuePut(&queue, 10);
    queuePut(&queue, 20);
    queuePut(&queue, 30);
    queuePop(&queue, &data);
    num = staticQueueGetNumItems(&queue);
    if (num != 2) {
        printf("Expected 2 items after pop, got %i\n", num);
        return 1;
    }
    printf("Passed: Count correct after pop\n");

    // Test: After erase
    printf("\nTest: GetNumItems after erase\n");
    queueClear(&queue);
    staticQueueItem_t* erase_items[3];
    staticQueuePut(&queue, &erase_items[0]);
    staticQueuePut(&queue, &erase_items[1]);
    staticQueuePut(&queue, &erase_items[2]);

    staticQueueErase(&queue, erase_items[1]);  // Erase middle item
    num = staticQueueGetNumItems(&queue);
    if (num != 2) {
        printf("Expected 2 items after erase, got %i\n", num);
        return 1;
    }
    printf("Passed: Count correct after erase\n");

    printf("\n=== All staticQueueGetNumItems tests passed ===\n");

    // ===== Test staticQueueForEach function =====
    printf("\n=== Testing staticQueueForEach ===\n");

    // Test 20: Basic iteration - count all items
    printf("\nTest 20: ForEach - basic iteration\n");
    queueClear(&queue);
    g_foreach_counter = 0;
    g_foreach_sum = 0;

    queuePut(&queue, 10);
    queuePut(&queue, 20);
    queuePut(&queue, 30);
    queuePut(&queue, 40);

    result = staticQueueForEach(&queue, countCallback);
    if (result != STATIC_QUEUE_SUCCESS) {
        printf("ForEach failed: %i\n", result);
        return 1;
    }

    if (g_foreach_counter != 4) {
        printf("Expected to iterate 4 items, got %i\n", g_foreach_counter);
        return 1;
    }

    if (g_foreach_sum != 100) {
        printf("Expected sum 100, got %i\n", g_foreach_sum);
        return 1;
    }

    printf("Test 20 passed: ForEach iterated all items correctly\n");

    // Test 21: ForEach on empty queue
    printf("\nTest 21: ForEach on empty queue\n");
    queueClear(&queue);
    g_foreach_counter = 0;

    result = staticQueueForEach(&queue, countCallback);
    if (result != STATIC_QUEUE_SUCCESS) {
        printf("ForEach on empty queue failed: %i\n", result);
        return 1;
    }

    if (g_foreach_counter != 0) {
        printf("Expected 0 iterations on empty queue, got %i\n", g_foreach_counter);
        return 1;
    }

    printf("Test 21 passed: ForEach handles empty queue\n");

    // Test 22: ForEach with STOP
    printf("\nTest 22: ForEach with STOP\n");
    queueClear(&queue);
    g_foreach_counter = 0;

    queuePut(&queue, 10);
    queuePut(&queue, 20);
    queuePut(&queue, 30);
    queuePut(&queue, 40);

    result = staticQueueForEach(&queue, stopCallback);
    if (result != STATIC_QUEUE_SUCCESS) {
        printf("ForEach stop failed: %i\n", result);
        return 1;
    }

    if (g_foreach_counter != 2) {
        printf("Expected to stop after 2 items, got %i\n", g_foreach_counter);
        return 1;
    }

    printf("Test 22 passed: ForEach stops correctly\n");

    // Test 23: ForEach with ERASE
    printf("\nTest 23: ForEach with ERASE\n");
    queueClear(&queue);

    queuePut(&queue, 10);
    queuePut(&queue, 30);
    queuePut(&queue, 20);
    queuePut(&queue, 40);

    result = staticQueueForEach(&queue, eraseCallback);
    if (result != STATIC_QUEUE_SUCCESS) {
        printf("ForEach erase failed: %i\n", result);
        return 1;
    }

    // Should only have 10 and 20 left
    result = queuePop(&queue, &data);
    if (result != STATIC_QUEUE_SUCCESS || data != 10) {
        printf("Expected 10, got %u\n", data);
        return 1;
    }

    result = queuePop(&queue, &data);
    if (result != STATIC_QUEUE_SUCCESS || data != 20) {
        printf("Expected 20, got %u\n", data);
        return 1;
    }

    if (!staticQueueEmpty(&queue)) {
        printf("Queue should be empty after erasing 30 and 40\n");
        return 1;
    }

    printf("Test 23 passed: ForEach erases items correctly\n");

    // Test 24: ForEach erase all items
    printf("\nTest 24: ForEach erase all items\n");
    queueClear(&queue);

    queuePut(&queue, 100);
    queuePut(&queue, 200);
    queuePut(&queue, 300);

    result = staticQueueForEach(&queue, eraseAllCallback);
    if (result != STATIC_QUEUE_SUCCESS) {
        printf("ForEach erase all failed: %i\n", result);
        return 1;
    }

    if (!staticQueueEmpty(&queue)) {
        printf("Queue should be empty after erasing all\n");
        return 1;
    }

    printf("Test 24 passed: ForEach can erase all items\n");

    // Test 25: ForEach with single item
    printf("\nTest 25: ForEach with single item\n");
    queueClear(&queue);
    g_foreach_counter = 0;

    queuePut(&queue, 999);

    result = staticQueueForEach(&queue, countCallback);
    if (result != STATIC_QUEUE_SUCCESS) {
        printf("ForEach single item failed: %i\n", result);
        return 1;
    }

    if (g_foreach_counter != 1) {
        printf("Expected 1 iteration, got %i\n", g_foreach_counter);
        return 1;
    }

    printf("Test 25 passed: ForEach handles single item\n");

    // Test 26: ForEach after pop operations
    printf("\nTest 26: ForEach after pop operations\n");
    queueClear(&queue);
    g_foreach_counter = 0;

    queuePut(&queue, 10);
    queuePut(&queue, 20);
    queuePut(&queue, 30);
    queuePut(&queue, 40);

    // Pop first two items
    queuePop(&queue, &data);
    queuePop(&queue, &data);

    result = staticQueueForEach(&queue, countCallback);
    if (result != STATIC_QUEUE_SUCCESS) {
        printf("ForEach after pop failed: %i\n", result);
        return 1;
    }

    if (g_foreach_counter != 2) {
        printf("Expected 2 items after popping 2, got %i\n", g_foreach_counter);
        return 1;
    }

    printf("Test 26 passed: ForEach works after pop operations\n");

    // Test 27: ForEach selective erase
    printf("\nTest 27: ForEach selective erase (even numbers)\n");
    queueClear(&queue);

    queuePut(&queue, 1);
    queuePut(&queue, 2);
    queuePut(&queue, 3);
    queuePut(&queue, 4);

    result = staticQueueForEach(&queue, eraseEvenCallback);
    if (result != STATIC_QUEUE_SUCCESS) {
        printf("ForEach selective erase failed: %i\n", result);
        return 1;
    }

    // Should have 1 and 3 left
    result = queuePop(&queue, &data);
    if (result != STATIC_QUEUE_SUCCESS || data != 1) {
        printf("Expected 1, got %u\n", data);
        return 1;
    }

    result = queuePop(&queue, &data);
    if (result != STATIC_QUEUE_SUCCESS || data != 3) {
        printf("Expected 3, got %u\n", data);
        return 1;
    }

    if (!staticQueueEmpty(&queue)) {
        printf("Queue should be empty\n");
        return 1;
    }

    printf("Test 27 passed: ForEach selectively erases items\n");

    // Test 28: ForEach with NULL callback
    printf("\nTest 28: ForEach with NULL callback (error handling)\n");
    queueClear(&queue);
    queuePut(&queue, 10);

    result = staticQueueForEach(&queue, NULL);
    if (result != STATIC_QUEUE_EMPTY) {
        printf("Expected error with NULL callback, got %i\n", result);
        return 1;
    }

    printf("Test 28 passed: ForEach handles NULL callback\n");

    // Test 29: ForEach callback returning error
    printf("\nTest 29: ForEach callback returning error\n");
    queueClear(&queue);
    g_foreach_counter = 0;

    queuePut(&queue, 10);
    queuePut(&queue, 20);
    queuePut(&queue, 30);
    queuePut(&queue, 40);

    result = staticQueueForEach(&queue, errorCallback);
    if (result != -999) {
        printf("Expected callback error -999, got %i\n", result);
        return 1;
    }

    if (g_foreach_counter != 3) {
        printf("Expected to process 3 items before error, got %i\n", g_foreach_counter);
        return 1;
    }

    printf("Test 29 passed: ForEach propagates callback errors\n");

    // Test 30: Erase single item in a queue with one item
    printf("\nTest 30: ForEach erase single item from single-item queue\n");
    queueClear(&queue);
    queuePut(&queue, 999);

    result = staticQueueForEach(&queue, eraseAllCallback);
    if (result != STATIC_QUEUE_SUCCESS) {
        printf("ForEach failed: %i\n", result);
        return 1;
    }

    if (!staticQueueEmpty(&queue)) {
        printf("Queue should be empty after erasing single item\n");
        return 1;
    }

    printf("Test 30 passed: Single item erased from single-item queue\n");

    // Test 31: Erase ONE element in middle, keep the rest
    printf("\nTest 31: ForEach erase only middle element, keep others\n");
    queueClear(&queue);

    queuePut(&queue, 10);
    queuePut(&queue, 20);
    queuePut(&queue, 30);
    queuePut(&queue, 40);

    result = staticQueueForEach(&queue, eraseOnly20Callback);
    if (result != STATIC_QUEUE_SUCCESS) {
        printf("ForEach failed: %i\n", result);
        return 1;
    }

    // Should have 10, 30, 40 left (not 20)
    result = queuePop(&queue, &data);
    if (result != STATIC_QUEUE_SUCCESS || data != 10) {
        printf("Expected 10, got %u\n", data);
        return 1;
    }

    result = queuePop(&queue, &data);
    if (result != STATIC_QUEUE_SUCCESS || data != 30) {
        printf("Expected 30, got %u\n", data);
        return 1;
    }

    result = queuePop(&queue, &data);
    if (result != STATIC_QUEUE_SUCCESS || data != 40) {
        printf("Expected 40, got %u\n", data);
        return 1;
    }

    if (!staticQueueEmpty(&queue)) {
        printf("Queue should be empty\n");
        return 1;
    }

    printf("Test 31 passed: Only one middle element erased, others kept\n");

    printf("\n=== All staticQueueForEach tests passed ===\n");

    // Connect first driver and app
    printf("\nTest Done\n");
}