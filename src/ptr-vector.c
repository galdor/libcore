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

struct c_ptr_vector {
    void **entries;
    size_t nb_entries;
    size_t entries_sz;
};

struct c_ptr_vector *
c_ptr_vector_new(void) {
    struct c_ptr_vector *vector;

    vector = c_malloc0(sizeof(struct c_ptr_vector));
    if (!vector)
        return NULL;

    return vector;
}

void
c_ptr_vector_delete(struct c_ptr_vector *vector) {
    if (!vector)
        return;

    c_free(vector->entries);

    c_free0(vector, sizeof(struct c_ptr_vector));
}

void
c_ptr_vector_clear(struct c_ptr_vector *vector) {
    vector->nb_entries = 0;
}

void **
c_ptr_vector_entries(const struct c_ptr_vector *vector) {
    return vector->entries;
}

size_t
c_ptr_vector_length(const struct c_ptr_vector *vector) {
    return vector->nb_entries;
}

bool
c_ptr_vector_is_empty(const struct c_ptr_vector *vector) {
    return vector->nb_entries == 0;
}

int
c_ptr_vector_append(struct c_ptr_vector *vector, const void *value) {
    size_t entries_sz;
    void **entries;

    if (vector->entries_sz == 0
     || vector->nb_entries > vector->entries_sz - 1) {
        entries_sz = (vector->entries_sz == 0) ? 4 : (vector->entries_sz * 2);
        entries = c_realloc(vector->entries, entries_sz * sizeof(void *));
        if (!entries)
            return -1;
    } else {
        entries_sz = vector->entries_sz;
        entries = vector->entries;
    }

    entries[vector->nb_entries] = (void *)value;

    vector->entries_sz = entries_sz;
    vector->entries = entries;
    vector->nb_entries++;
    return 0;
}

void
c_ptr_vector_set(struct c_ptr_vector *vector, size_t index, const void *value) {
    assert(index < vector->nb_entries);

    vector->entries[index] = (void *)value;
}

void
c_ptr_vector_remove(struct c_ptr_vector *vector, size_t index) {
    assert(index < vector->nb_entries);

    if (index < vector->nb_entries - 1) {
        memmove(vector->entries + index,
                vector->entries + index + 1,
                (vector->nb_entries - index - 1) * sizeof(void *));
    }

    vector->nb_entries--;
}

void *
c_ptr_vector_entry(const struct c_ptr_vector *vector, size_t index) {
    assert(index < vector->nb_entries);

    return vector->entries[index];
}
