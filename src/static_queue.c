/**
 * @file:       static_queue.c
 * @author:     Lucas Wennerholm <lucas.wennerholm@gmail.com>
 * @brief:      Implementation of static queue module
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

#include "static_queue.h"

int32_t staticQueueInit(staticQueue_t*     queue,
                        uint32_t           queue_size,
                        uint32_t           node_size,
                        staticQueueItem_t* first_item)
{
    queue->head         = first_item;
    queue->tail         = first_item;
    queue->first_item   = first_item;
    queue->queue_length = queue_size;

    staticQueueItem_t* item = first_item;
    for (uint32_t i = 0; i < queue_size - 1; i++) {
        item->next       = (staticQueueItem_t*)((uint8_t*)item + node_size);
        item->next->last = item;
        item             = item->next;
    }

    first_item->last = item;
    item->next       = first_item;

    return STATIC_QUEUE_SUCCESS;
}

bool staticQueuefull(staticQueue_t* queue)
{
    return (queue->head == queue->tail) && queue->head->active;
}

bool staticQueueEmpty(staticQueue_t* queue)
{
    return (queue->head == queue->tail) && !queue->head->active;
}

int32_t staticQueuePutFirst(staticQueue_t* queue, staticQueueItem_t** next_item)
{
    if (staticQueuefull(queue)) {
        return STATIC_QUEUE_FULL;
    }

    // Move the tail one step back
    queue->tail         = queue->tail->last;
    *next_item          = queue->tail;
    queue->tail->active = true;

    return STATIC_QUEUE_SUCCESS;
}

int32_t staticQueuePut(staticQueue_t* queue, staticQueueItem_t** next_item)
{
    if (staticQueuefull(queue)) {
        return STATIC_QUEUE_FULL;
    }

    *next_item          = queue->head;
    queue->head->active = true;
    queue->head         = queue->head->next;

    return STATIC_QUEUE_SUCCESS;
}

int32_t staticQueuePop(staticQueue_t* queue, staticQueueItem_t** pop_item)
{
    if (staticQueueEmpty(queue)) {
        return STATIC_QUEUE_EMPTY;
    }

    *pop_item           = queue->tail;
    queue->tail->active = false;
    queue->tail         = queue->tail->next;

    return STATIC_QUEUE_SUCCESS;
}

int32_t staticQueuePeak(staticQueue_t* queue, staticQueueItem_t** peak_item) {
    if (staticQueueEmpty(queue)) {
        return STATIC_QUEUE_EMPTY;
    }

    *peak_item = queue->tail;

    return STATIC_QUEUE_SUCCESS;
}

int32_t staticQueueClear(staticQueue_t* queue)
{
    queue->head = queue->first_item;
    for (uint32_t i = 0; i < queue->queue_length; i++) {
        queue->head->active = false;
        queue->head         = queue->head->next;
    }

    queue->head = queue->first_item;
    queue->tail = queue->first_item;
    return STATIC_QUEUE_SUCCESS;
}

int32_t staticQueueErase(staticQueue_t* queue, staticQueueItem_t* item)
{
    // Check if the item is active
    if (!item->active) {
        return STATIC_QUEUE_EMPTY;
    }

    // Mark the item as inactive
    item->active = false;

    // Special case: if this was the only item in the queue
    if (queue->tail == queue->head->last && queue->tail == item) {
        // Queue is now empty, reset pointers
        queue->head = queue->first_item;
        queue->tail = queue->first_item;
        return STATIC_QUEUE_SUCCESS;
    } else if (item == queue->tail) {
        // If erasing the tail item (oldest item), just move tail to next
        queue->tail = queue->tail->next;

        // Skip over any remaining inactive items at tail
        while (queue->tail != queue->head && !queue->tail->active) {
            queue->tail = queue->tail->next;
        }

        // Check if tail caught up to head with exactly one active item
        if (queue->tail == queue->head && queue->tail->active) {
            // Move head forward to maintain tail != head invariant for single item
            queue->head = queue->head->next;
        }

        return STATIC_QUEUE_SUCCESS;
    } else if (item->next == queue->head) {
        // If erasing the item just before head (newest item), move head backward
        queue->head = item;

        // Check if head caught up to tail with exactly one active item
        if (queue->tail == queue->head && queue->tail->active) {
            // Move head forward to maintain tail != head invariant for single item
            queue->head = queue->head->next;
        }

        return STATIC_QUEUE_SUCCESS;
    }

    // For items in the middle: first verify the item is actually in the queue
    // We can start from the element one from the tail as we have allready checked the tail item above.
    staticQueueItem_t* current = queue->tail->next;
    bool found = false;

    // Traverse from tail to head to find the item
    while (current != queue->head) {
        if (current == item) {
            found = true;
            break;
        }
        current = current->next;
    }

    // If not found, the item is not in this queue
    if (!found) {
        return STATIC_QUEUE_NOT_IN_QUEUE;
    }

    // Step 1: Remove item from its current position in the list
    staticQueueItem_t* prev_item = item->last;
    staticQueueItem_t* next_item = item->next;

    prev_item->next = next_item;
    next_item->last = prev_item;

    // Step 2: Reinsert the item immediately before tail
    staticQueueItem_t* before_tail = queue->tail->last;

    // Insert: before_tail <-> item <-> tail
    before_tail->next = item;
    item->last = before_tail;
    item->next = queue->tail;
    queue->tail->last = item;

    // Step 3: If queue was full, move head to point to the erased item (now inactive)
    if (queue->head == queue->tail && queue->head->active) {
        queue->head = item;
    }

    return STATIC_QUEUE_SUCCESS;
}
