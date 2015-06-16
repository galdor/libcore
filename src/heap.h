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

#ifndef LIBCORE_HEAP_H
#define LIBCORE_HEAP_H

#include <stdlib.h>

typedef int (*c_heap_cmp_func)(const void *, const void *);

struct c_heap *c_heap_new(c_heap_cmp_func);
void c_heap_delete(struct c_heap *);

size_t c_heap_nb_entries(const struct c_heap *);
bool c_heap_is_empty(const struct c_heap *);

void c_heap_clear(struct c_heap *);
void c_heap_reset(struct c_heap *);

int c_heap_add(struct c_heap *, void *);
int c_heap_remove(struct c_heap *, const void *);

int c_heap_find(const struct c_heap *, const void *, size_t *);
bool c_heap_contains(const struct c_heap *, const void *);

void *c_heap_peek(const struct c_heap *);
void *c_heap_pop(struct c_heap *);

void *c_heap_entry(const struct c_heap *, size_t);

#endif
