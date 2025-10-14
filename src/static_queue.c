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

    // Skip over any inactive items at tail (from previous erase operations)
    while (queue->tail != queue->head && !queue->tail->active) {
        queue->tail = queue->tail->next;
    }

    // If tail caught up to head with exactly one active item, move head forward
    if (queue->tail == queue->head && queue->tail->active) {
        queue->head = queue->head->next;
    }

    // Check again if empty after skipping
    if (staticQueueEmpty(queue)) {
        queue->head = queue->first_item;
        queue->tail = queue->first_item;
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

    // Skip over any inactive items at tail (from previous erase operations)
    staticQueueItem_t* temp_tail = queue->tail;
    while (temp_tail != queue->head && !temp_tail->active) {
        temp_tail = temp_tail->next;
    }

    // Check if we reached head without finding an active item
    if (temp_tail == queue->head && !temp_tail->active) {
        queue->head = queue->first_item;
        queue->tail = queue->first_item;
        return STATIC_QUEUE_EMPTY;
    }

    *peak_item = temp_tail;

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
    // Pop/peak will skip over inactive items
    if (item == queue->tail) {
        queue->tail = queue->tail->next;

        // Check if tail caught up to head with exactly one active item
        if (queue->tail == queue->head && queue->tail->active) {
            // Move head forward to maintain tail != head invariant for single item
            queue->head = queue->head->next;
        }

        return STATIC_QUEUE_SUCCESS;
    }

    // If erasing the item just before head (newest item), move head backward
    if (item->next == queue->head) {
        queue->head = item;

        // Check if head caught up to tail with exactly one active item
        if (queue->tail == queue->head && queue->tail->active) {
            // Move head forward to maintain tail != head invariant for single item
            queue->head = queue->head->next;
        }

        return STATIC_QUEUE_SUCCESS;
    }

    // For items in the middle: remove from current position and relink immediately after tail
    // This ensures tail never points to an inactive item.

    // Step 1: Remove item from its current position in the list
    staticQueueItem_t* prev_item = item->last;
    staticQueueItem_t* next_item = item->next;

    prev_item->next = next_item;
    next_item->last = prev_item;

    // Step 2: Reinsert the item immediately after tail (but before the next active item)
    // This keeps it in the circular structure but ensures tail always points to an active item.
    staticQueueItem_t* after_tail = queue->tail->next;

    // Insert: tail <-> item <-> after_tail
    queue->tail->next = item;
    item->last = queue->tail;
    item->next = after_tail;
    after_tail->last = item;

    // Don't move tail or head - the inactive item sits right after tail,
    // and will be naturally encountered when tail advances during pop.

    return STATIC_QUEUE_SUCCESS;
}
