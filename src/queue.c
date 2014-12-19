/*
 * Copyright (c) 2014 Nicolas Martyanoff
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <assert.h>

#include "internal.h"

struct c_queue_entry {
    void *value;

    struct c_queue_entry *prev;
    struct c_queue_entry *next;
};

static struct c_queue_entry *c_queue_entry_new(void *);

struct c_queue {
    struct c_queue_entry *first;
    struct c_queue_entry *last;
    size_t length;
};

static void c_queue_add_entry(struct c_queue *, struct c_queue_entry *);

struct c_queue *
c_queue_new(void) {
    struct c_queue *queue;

    queue = c_malloc0(sizeof(struct c_queue));
    if (!queue)
        return NULL;

    return queue;
}

void
c_queue_delete(struct c_queue *queue) {
    if (!queue)
        return;

    c_queue_clear(queue);

    c_free0(queue, sizeof(struct c_queue));
}

size_t
c_queue_length(const struct c_queue *queue) {
    return queue->length;
}

bool
c_queue_is_empty(const struct c_queue *queue) {
    return queue->length == 0;
}

void
c_queue_clear(struct c_queue *queue) {
    struct c_queue_entry *entry;

    entry = queue->first;
    while (entry) {
        struct c_queue_entry *next;

        next = entry->next;
        c_queue_entry_delete(entry);
        entry = next;
    }

    queue->first = NULL;
    queue->last = NULL;
    queue->length = 0;
}

int
c_queue_push(struct c_queue *queue, void *value) {
    struct c_queue_entry *entry;

    entry = c_queue_entry_new(value);
    if (!entry)
        return -1;

    c_queue_add_entry(queue, entry);
    return 0;
}

void *
c_queue_peek(const struct c_queue *queue) {
    if (!queue->first)
        return NULL;

    return queue->first->value;
}

void *
c_queue_pop(struct c_queue *queue) {
    struct c_queue_entry *entry;
    void *value;

    entry = queue->first;
    if (!entry)
        return NULL;

    value = entry->value;

    c_queue_remove_entry(queue, entry);
    c_queue_entry_delete(entry);

    return value;
}

struct c_queue_entry *
c_queue_first_entry(struct c_queue *queue) {
    return queue->first;
}

struct c_queue_entry *
c_queue_last_entry(struct c_queue *queue) {
    return queue->last;
}

void
c_queue_entry_delete(struct c_queue_entry *entry) {
    if (!entry)
        return;

    c_free0(entry, sizeof(struct c_queue_entry));
}

struct c_queue_entry *
c_queue_entry_prev(struct c_queue_entry *entry) {
    return entry->prev;
}

struct c_queue_entry *
c_queue_entry_next(struct c_queue_entry *entry) {
    return entry->next;
}

void *
c_queue_entry_value(const struct c_queue_entry *entry) {
    return entry->value;
}

void
c_queue_remove_entry(struct c_queue *queue, struct c_queue_entry *entry) {
    if (entry->prev)
        entry->prev->next = entry->next;
    if (entry->next)
        entry->next->prev = entry->prev;

    if (entry == queue->first)
        queue->first = entry->next;
    if (entry == queue->last)
        queue->last = entry->prev;

    entry->prev = NULL;
    entry->next = NULL;

    queue->length--;
}

static struct c_queue_entry *
c_queue_entry_new(void *value) {
    struct c_queue_entry *entry;

    entry = c_malloc0(sizeof(struct c_queue_entry));
    if (!entry)
        return NULL;

    entry->value = value;

    return entry;
}

static void
c_queue_add_entry(struct c_queue *queue, struct c_queue_entry *entry) {
    assert(!entry->prev);
    assert(!entry->next);

    entry->prev = queue->last;
    if (queue->last)
        queue->last->next = entry;

    if (!queue->first)
        queue->first = entry;
    queue->last = entry;

    queue->length++;
}
