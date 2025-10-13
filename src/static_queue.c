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
    if (queue->tail == queue->head->last) {
        // Queue is now empty, reset pointers
        queue->head = queue->first_item;
        queue->tail = queue->first_item;
        return STATIC_QUEUE_SUCCESS;
    }

    // If erasing the tail item (oldest item), just move tail to next
    if (item == queue->tail) {
        queue->tail = queue->tail->next;
        return STATIC_QUEUE_SUCCESS;
    }

    // If erasing the item just before head (newest item), move head backward
    if (item->next == queue->head) {
        queue->head = item;
        return STATIC_QUEUE_SUCCESS;
    }

    // For items in the middle: remove from current position and relink at head
    // This keeps the item reachable in the circular buffer for future reuse.

    // Step 1: Remove item from its current position in the list
    staticQueueItem_t* prev_item = item->last;
    staticQueueItem_t* next_item = item->next;

    prev_item->next = next_item;
    next_item->last = prev_item;

    // Step 2: Reinsert the item just before head and move head to point at it
    // This keeps the item in the circular structure (reachable) and maintains
    // the invariant that head points to an inactive slot ready for put().

    staticQueueItem_t* before_head = queue->head->last;

    // Insert: before_head <-> item <-> head
    before_head->next = item;
    item->last = before_head;
    item->next = queue->head;
    queue->head->last = item;

    // Move head back to point at the newly inserted (inactive) item
    // This way head always points at an inactive slot, ready for put()
    queue->head = item;

    return STATIC_QUEUE_SUCCESS;
}
