/*
 * Copyright (c) 2015 Nicolas Martyanoff
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

struct c_heap {
    c_heap_cmp_func cmp;

    void **entries;
    size_t nb_entries;
    size_t entries_sz;
};

static void c_heap_swap(struct c_heap *, size_t, size_t);
static void c_heap_bubble_up(struct c_heap *, size_t);
static void c_heap_sink_down(struct c_heap *, size_t);

struct c_heap *
c_heap_new(c_heap_cmp_func cmp) {
    struct c_heap *heap;

    heap = c_malloc0(sizeof(struct c_heap));
    if (!heap)
        return NULL;

    heap->cmp = cmp;

    return heap;
}

void
c_heap_delete(struct c_heap *heap) {
    if (!heap)
        return;

    c_heap_reset(heap);

    c_free0(heap, sizeof(struct c_heap));
}

size_t
c_heap_nb_entries(const struct c_heap *heap) {
    return heap->nb_entries;
}

bool
c_heap_is_empty(const struct c_heap *heap) {
    return heap->nb_entries == 0;
}

void
c_heap_clear(struct c_heap *heap) {
    heap->nb_entries = 0;
}

void
c_heap_reset(struct c_heap *heap) {
    c_free(heap->entries);
    heap->nb_entries = 0;
    heap->entries_sz = 0;
}

int
c_heap_add(struct c_heap *heap, void *entry) {
    if (!heap->entries || heap->nb_entries + 1 > heap->entries_sz) {
        size_t entries_sz;
        void **entries;

        if (!heap->entries) {
            entries_sz = 4;
        } else {
            entries_sz = heap->entries_sz * 2;
        }

        entries = c_realloc(heap->entries, entries_sz * sizeof(void *));
        if (!entries)
            return -1;

        heap->entries = entries;
        heap->entries_sz = entries_sz;
    }

    heap->entries[heap->nb_entries++] = entry;
    c_heap_bubble_up(heap, heap->nb_entries - 1);

    return 0;
}

void *
c_heap_peek(const struct c_heap *heap) {
    if (heap->nb_entries == 0)
        return NULL;

    return heap->entries[0];
}

void *
c_heap_pop(struct c_heap *heap) {
    void *entry;

    if (heap->nb_entries == 0)
        return NULL;

    entry = heap->entries[0];

    if (heap->nb_entries > 1)
        heap->entries[0] = heap->entries[heap->nb_entries - 1];

    heap->nb_entries--;

    if (heap->nb_entries > 0)
        c_heap_sink_down(heap, 0);

    return entry;
}

void *
c_heap_entry(struct c_heap *heap, size_t idx) {
    assert(idx < heap->nb_entries);

    return heap->entries[idx];
}

static void
c_heap_swap(struct c_heap *heap, size_t i, size_t j) {
    void *tmp;

    assert(i < heap->nb_entries);
    assert(j < heap->nb_entries);

    tmp = heap->entries[i];
    heap->entries[i] = heap->entries[j];
    heap->entries[j] = tmp;
}

static void
c_heap_bubble_up(struct c_heap *heap , size_t idx) {
    assert(idx < heap->nb_entries);

    while (idx > 0) {
        size_t parent_idx;

        parent_idx = (idx - 1) / 2;

        if (heap->cmp(heap->entries[idx], heap->entries[parent_idx]) >= 0)
            break;

        c_heap_swap(heap, idx, parent_idx);
        idx = parent_idx;
    }
}

static void
c_heap_sink_down(struct c_heap *heap, size_t idx) {
    assert(idx < heap->nb_entries);

    for (;;) {
        size_t left_idx, right_idx, min_idx;

        left_idx = idx * 2 + 1;
        right_idx = idx * 2 + 2;

        if (right_idx >= heap->nb_entries)
            break;

        min_idx = idx;
        if (heap->cmp(heap->entries[min_idx], heap->entries[left_idx]) > 0)
            min_idx = left_idx;
        if (heap->cmp(heap->entries[min_idx], heap->entries[right_idx]) > 0)
            min_idx = right_idx;

        if (min_idx == idx)
            break;

        c_heap_swap(heap, idx, min_idx);
        idx = min_idx;
    }
}
