/**
 * @file:       static_queue.h
 * @author:     Lucas Wennerholm <lucas.wennerholm@gmail.com>
 * @brief:      Header file for static queue module
 *
 * @license: MIT License
 *
 * Copyright (c) 2024 Lucas Wennerholm
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
*/

#ifndef INC_STATIC_QUEUE_H_
#define INC_STATIC_QUEUE_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/**
 * The static Queue makes it possbile to implement a generic circular queue that is statically
 * allocated. To use this module: Create a queue item struct, that contains a staticQueueItem_t.
 *
 *      typedef struct {
 *         unsigned          my_data;
 *         staticQueueItem_t node;
 *     } myItem_t;
 *
 * Allocate an array of myItem_t
 *     myItem_t my_queue_array[QUEUE_SIZE] = {0};
 *
 * Create an instance of the staticQueue_t
 *     staticQueue_t my_queue = {0};
 *
 * Init the queue using the init MACRO, not the init function, it is safer.
 *     STATIC_QUEUE_INIT(my_queue, my_queue_array, QUEUE_SIZE);
 *
 * To use the queue, use the CONTAINTER_OF macro like this:
 *
 *   staticQueueItem_t* item;
 *   int32_t            result = staticQueuePut(&my_queue, &item);
 *   if (result == STATIC_QUEUE_SUCCESS) {
 *       myItem_t* queue_item = container_of(item, myItem_t, node);
 *       // The Item returned is the one you should write your data to
 *       queue_item->my_data = 1337;
 *   }
 *
 *   staticQueueItem_t* item;
 *   int32_t            result = staticQueuePop(&my_queue, &item);
 *   if (result == STATIC_QUEUE_SUCCESS) {
 *         myItem_t* queue_item = container_of(item, myItem_t, node);
 *         printf("My data %u", queue_item->my_data);
 *   }
 *
 *   The queue is two way connected end to end, meaning that it behaves like a circular buffer of
 * myItem_t's.
 *
 */

#ifndef CONTAINER_OF
#define CONTAINER_OF(ptr, type, member)	(type *)((char *)(ptr) - offsetof(type,member))
#endif

// Package queue
typedef enum {
    STATIC_QUEUE_SUCCESS      = 0,
    STATIC_QUEUE_FULL         = -401,
    STATIC_QUEUE_EMPTY        = -402,
    STATIC_QUEUE_NOT_IN_QUEUE = -403,
} queueErr_t;

typedef struct staticQueueItem staticQueueItem_t;

struct staticQueueItem {
    staticQueueItem_t* next;
    staticQueueItem_t* last;
    bool               active;
};

typedef struct {
    staticQueueItem_t* head;
    staticQueueItem_t* tail;
    staticQueueItem_t* first_item;
    uint32_t           queue_length;
} staticQueue_t;

/**
 * Initialize a static queue
 * Input: Queue instance
 * Input: Number of items in the queue
 * Input: The sizeof a specific item
 * Input: Pointer to the first item in the array
 * Returns: queueErr_t
 */
int32_t staticQueueInit(staticQueue_t*     queue,
                        uint32_t           queue_size,
                        uint32_t           node_size,
                        staticQueueItem_t* first_item);

/**
 * Put an item at the start of the queue, will be pop'ed next
 * Input: Queue instance
 * Input: This pointer wil be populated with the pointer to the relevant item to write data to
 * Returns: queueErr_t
 */
int32_t staticQueuePutFirst(staticQueue_t* queue, staticQueueItem_t** next_item);

/**
 * Put an item at the end of the queue
 * Input: Queue instance
 * Input: This pointer wil be populated with the pointer to the relevant item to write data to
 * Returns: queueErr_t
 */
int32_t staticQueuePut(staticQueue_t* queue, staticQueueItem_t** next_item);

/**
 * Get and remove the next Item in the queue
 * Input: Queue instance
 * Input: This pointer will be populated with the pop'ed item
 * Returns: queueErr_t
 */
int32_t staticQueuePop(staticQueue_t* queue, staticQueueItem_t** pop_item);

/**
 * Get the next item in the queue, but do not remove it
 * Input: Queue instance
 * Input: This pointer will be populated with the pop'ed item
 * Returns: queueErr_t
 */
int32_t staticQueuePeak(staticQueue_t* queue, staticQueueItem_t** peak_item);

/**
 * Clear the Queue and reset the pointers
 * Input: Queue instance
 * Returns: queueErr_t
 */
int32_t staticQueueClear(staticQueue_t* queue);

/**
 * Erase a specific item from the queue
 * Input: Queue instance
 * Input: Pointer to the item to erase
 * Returns: queueErr_t
 */
int32_t staticQueueErase(staticQueue_t* queue, staticQueueItem_t* item);

/**
 * Check it the queue is full
 * Input: Queue instance
 * Returns: true if full
 */
bool staticQueuefull(staticQueue_t* queue);

/**
 * Check it the queue is empty
 * Input: Queue instance
 * Returns: true if empty
 */
bool staticQueueEmpty(staticQueue_t* queue);

/**
 * This is a macro that makes it more safe to initialize a queue
 */
#define STATIC_QUEUE_INIT(queue, list, size) \
    staticQueueInit((queue), (size), sizeof((list)[0]), &list->node)

#endif /* INC_STATIC_QUEUE_H_ */