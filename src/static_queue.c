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
