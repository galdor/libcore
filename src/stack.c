/*
 * Copyright (c) 2014-2015 Nicolas Martyanoff
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

struct c_stack_entry {
    void *value;

    struct c_stack_entry *prev; /* upward */
    struct c_stack_entry *next; /* downward */
};

static struct c_stack_entry *c_stack_entry_new(void *);

struct c_stack {
    struct c_stack_entry *top;
    struct c_stack_entry *bottom;
    size_t length;
};

static void c_stack_add_entry(struct c_stack *, struct c_stack_entry *);

struct c_stack *
c_stack_new(void) {
    struct c_stack *stack;

    stack = c_malloc0(sizeof(struct c_stack));
    if (!stack)
        return NULL;

    return stack;
}

void
c_stack_delete(struct c_stack *stack) {
    if (!stack)
        return;

    c_stack_clear(stack);

    c_free0(stack, sizeof(struct c_stack));
}

size_t
c_stack_length(const struct c_stack *stack) {
    return stack->length;
}

bool
c_stack_is_empty(const struct c_stack *stack) {
    return stack->length == 0;
}

void
c_stack_clear(struct c_stack *stack) {
    struct c_stack_entry *entry;

    entry = stack->top;
    while (entry) {
        struct c_stack_entry *next;

        next = entry->next;
        c_stack_entry_delete(entry);
        entry = next;
    }

    stack->top = NULL;
    stack->bottom = NULL;
    stack->length = 0;
}

int
c_stack_push(struct c_stack *stack, void *value) {
    struct c_stack_entry *entry;

    entry = c_stack_entry_new(value);
    if (!entry)
        return -1;

    c_stack_add_entry(stack, entry);
    return 0;
}

void *
c_stack_peek(const struct c_stack *stack) {
    if (!stack->top)
        return NULL;

    return stack->top->value;
}

void *
c_stack_pop(struct c_stack *stack) {
    struct c_stack_entry *entry;
    void *value;

    entry = stack->top;
    if (!entry)
        return NULL;

    value = entry->value;

    c_stack_remove_entry(stack, entry);
    c_stack_entry_delete(entry);

    return value;
}

struct c_stack_entry *
c_stack_top_entry(struct c_stack *stack) {
    return stack->top;
}

struct c_stack_entry *
c_stack_bottom_entry(struct c_stack *stack) {
    return stack->bottom;
}

void
c_stack_entry_delete(struct c_stack_entry *entry) {
    if (!entry)
        return;

    c_free0(entry, sizeof(struct c_stack_entry));
}

struct c_stack_entry *
c_stack_entry_prev(struct c_stack_entry *entry) {
    return entry->prev;
}

struct c_stack_entry *
c_stack_entry_next(struct c_stack_entry *entry) {
    return entry->next;
}

void *
c_stack_entry_value(const struct c_stack_entry *entry) {
    return entry->value;
}

void
c_stack_remove_entry(struct c_stack *stack, struct c_stack_entry *entry) {
    if (entry->prev)
        entry->prev->next = entry->next;
    if (entry->next)
        entry->next->prev = entry->prev;

    if (entry == stack->top)
        stack->top = entry->next;
    if (entry == stack->bottom)
        stack->bottom = entry->prev;

    entry->prev = NULL;
    entry->next = NULL;

    stack->length--;
}

static struct c_stack_entry *
c_stack_entry_new(void *value) {
    struct c_stack_entry *entry;

    entry = c_malloc0(sizeof(struct c_stack_entry));
    if (!entry)
        return NULL;

    entry->value = value;

    return entry;
}

static void
c_stack_add_entry(struct c_stack *stack, struct c_stack_entry *entry) {
    assert(!entry->prev);
    assert(!entry->next);

    entry->next = stack->top;
    if (stack->top)
        stack->top->prev = entry;

    if (!stack->bottom)
        stack->bottom = entry;
    stack->top = entry;

    stack->length++;
}
