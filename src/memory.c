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

#include "internal.h"

#define C_DEFAULT_ALLOCATOR  \
    {                        \
        .malloc = malloc,    \
        .free = free,        \
        .calloc = calloc,    \
        .realloc = realloc   \
    }

static const struct c_memory_allocator c_default_memory_allocator_data =
    C_DEFAULT_ALLOCATOR;

static struct c_memory_allocator c_memory_allocator = C_DEFAULT_ALLOCATOR;

const struct c_memory_allocator *c_default_memory_allocator =
    &c_default_memory_allocator_data;

void
c_set_memory_allocator(const struct c_memory_allocator *allocator) {
    c_memory_allocator = *allocator;
}

void *
c_malloc(size_t sz) {
    void *ptr;

    ptr = c_memory_allocator.malloc(sz);
    if (!ptr) {
        c_set_error("cannot allocate %zu bytes: %s", sz, strerror(errno));
        return NULL;
    }

    return ptr;
}

void *
c_malloc0(size_t sz) {
    void *ptr;

    ptr = c_malloc(sz);
    if (!ptr)
        return NULL;

    memset(ptr, 0, sz);
    return ptr;
}

void
c_free(void *ptr) {
    c_memory_allocator.free(ptr);
}

void
c_free0(void *ptr, size_t sz) {
    if (!ptr)
        return;

    memset(ptr, 0, sz);
    c_free(ptr);
}

void *
c_calloc(size_t nb, size_t sz) {
    void *ptr;

    ptr = c_memory_allocator.calloc(nb, sz);
    if (!ptr) {
        c_set_error("cannot allocate %zux%zu bytes: %s",
                     nb, sz, strerror(errno));
        return NULL;
    }

    return ptr;
}

void *
c_realloc(void *ptr, size_t sz) {
    void *nptr;

    nptr = c_memory_allocator.realloc(ptr, sz);
    if (!nptr) {
        c_set_error("cannot reallocate %zu bytes: %s", sz, strerror(errno));
        return NULL;
    }

    return nptr;
}
