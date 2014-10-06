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

struct c_vector {
    void *entries;
    size_t nb_entries;
    size_t entries_sz;

    size_t entry_sz;
};

struct c_vector *
c_vector_new(size_t entry_sz) {
    struct c_vector *vector;

    assert(entry_sz > 0);

    vector = c_malloc0(sizeof(struct c_vector));
    if (!vector)
        return NULL;

    vector->entry_sz = entry_sz;

    return vector;
}

void
c_vector_delete(struct c_vector *vector) {
    if (!vector)
        return;

    c_free(vector->entries);

    c_free0(vector, sizeof(struct c_vector));
}

void
c_vector_clear(struct c_vector *vector) {
    vector->nb_entries = 0;
}

size_t
c_vector_length(const struct c_vector *vector) {
    return vector->nb_entries;
}

bool
c_vector_is_empty(const struct c_vector *vector) {
    return vector->nb_entries == 0;
}

int
c_vector_append(struct c_vector *vector, const void *value) {
    size_t entries_sz;
    void *entries;

    if (vector->entries_sz == 0
     || vector->nb_entries > vector->entries_sz - 1) {
        entries_sz = (vector->entries_sz == 0) ? 4 : (vector->entries_sz * 2);
        entries = c_realloc(vector->entries, entries_sz * vector->entry_sz);
        if (!entries)
            return -1;
    } else {
        entries_sz = vector->entries_sz;
        entries = vector->entries;
    }

    memcpy(entries + vector->nb_entries * vector->entry_sz,
           value, vector->entry_sz);

    vector->entries_sz = entries_sz;
    vector->entries = entries;
    vector->nb_entries++;
    return 0;
}

void
c_vector_remove(struct c_vector *vector, size_t index) {
    assert(index < vector->nb_entries);

    if (index < vector->nb_entries - 1) {
        void *entry;

        entry = vector->entries + index * vector->entry_sz;
        memmove(entry, entry + vector->entry_sz,
                (vector->nb_entries - index - 1) * vector->entry_sz);
    }

    vector->nb_entries--;
}

void *
c_vector_entry(const struct c_vector *vector, size_t index) {
    assert(index < vector->nb_entries);

    return vector->entries + index * vector->entry_sz;
}
