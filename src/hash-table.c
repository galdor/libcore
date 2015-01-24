/*
 * Copyright (c) 2013-2015 Nicolas Martyanoff
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
#include <inttypes.h>

#include "internal.h"

#define C_UNUSED_HASH 0

struct c_hash_table_entry {
    void *key;
    void *value;
    uint32_t hash;
};

#define C_TABLE_ENTRY_IS_USED(entry_) ((entry_)->hash != C_UNUSED_HASH)

struct c_hash_table_bucket {
    struct c_hash_table_entry *entries;
    size_t sz;
};

struct c_hash_table {
    size_t nb_entries;

    struct c_hash_table_bucket *buckets;
    size_t buckets_sz;

    c_hash_func hash_func;
    c_equal_func equal_func;

    int nb_iterators;
};

struct c_hash_table_iterator {
    struct c_hash_table *table;
    size_t bucket;
    size_t entry;
};

static int c_hash_table_resize(struct c_hash_table *, size_t);
static int c_hash_table_insert_in(struct c_hash_table *,
                                  struct c_hash_table_bucket *, size_t,
                                  void *, void *, uint32_t, bool);
static struct c_hash_table_entry *c_hash_table_entry(struct c_hash_table *,
                                                     const void *);


struct c_hash_table *
c_hash_table_new(c_hash_func hash_func, c_equal_func equal_func) {
    struct c_hash_table *table;

    table = c_malloc(sizeof(struct c_hash_table));
    if (!table) {
        c_set_error("cannot allocate table: %m");
        return NULL;
    }

    memset(table, 0, sizeof(struct c_hash_table));

    table->buckets_sz = 4;
    table->buckets = c_calloc(table->buckets_sz,
                              sizeof(struct c_hash_table_bucket));
    if (!table->buckets) {
        c_set_error("cannot allocate buckets: %m");
        c_hash_table_delete(table);
        return NULL;
    }

    table->hash_func = hash_func;
    table->equal_func = equal_func;

    return table;
}

void
c_hash_table_delete(struct c_hash_table *table) {
    if (!table)
        return;

    assert(table->nb_iterators == 0);

    for (size_t i = 0; i < table->buckets_sz; i++)
        c_free(table->buckets[i].entries);

    c_free(table->buckets);

    memset(table, 0, sizeof(struct c_hash_table));
    c_free(table);
}

size_t
c_hash_table_nb_entries(const struct c_hash_table *table) {
    return table->nb_entries;
}

bool
c_hash_table_is_empty(const struct c_hash_table *table) {
    return table->nb_entries == 0;
}

void
c_hash_table_clear(struct c_hash_table *table) {
    assert(table->nb_iterators == 0);

    for (size_t b = 0; b < table->buckets_sz; b++) {
        struct c_hash_table_bucket *bucket;

        bucket = table->buckets + b;
        memset(bucket->entries, 0,
               bucket->sz * sizeof(struct c_hash_table_entry));
    }

    table->nb_entries = 0;
}

int
c_hash_table_insert(struct c_hash_table *table, void *key, void *value) {
    uint32_t hash;
    int ret;

    assert(table->nb_iterators == 0);

    if (table->nb_entries >= table->buckets_sz) {
        if (c_hash_table_resize(table, table->buckets_sz * 2) == -1)
            return -1;
    }

    hash = table->hash_func(key);
    if (hash == C_UNUSED_HASH)
        hash++;

    ret = c_hash_table_insert_in(table, table->buckets, table->buckets_sz,
                                 key, value, hash, false);
    if (ret == -1)
        return -1;

    if (ret == 1)
        table->nb_entries++;

    return ret;
}

int
c_hash_table_insert2(struct c_hash_table *table, void *key, void *value,
                     void **old_key, void **old_value) {
    struct c_hash_table_entry *entry;

    assert(table->nb_iterators == 0);

    entry = c_hash_table_entry(table, key);
    if (entry) {
        if (old_key)
            *old_key = entry->key;
        if (old_value)
            *old_value = entry->value;

        entry->key = key;
        entry->value = value;

        return 0;
    } else {
        if (old_key)
            *old_key = NULL;
        if (old_value)
            *old_value = NULL;

        return c_hash_table_insert(table, key, value);
    }
}

int
c_hash_table_remove(struct c_hash_table *table, const void *key) {
    return c_hash_table_remove2(table, key, NULL, NULL);
}

int
c_hash_table_remove2(struct c_hash_table *table, const void *key,
                     void **old_key, void **old_value) {
    struct c_hash_table_entry *entry;
    bool should_resize;

    entry = c_hash_table_entry(table, key);
    if (!entry)
        return 0;

    if (old_key)
        *old_key = entry->key;
    if (old_value)
        *old_value = entry->value;

    entry->key = NULL;
    entry->value = NULL;
    entry->hash = C_UNUSED_HASH;

    should_resize = table->buckets_sz > 4
        && table->nb_entries * 4 <= table->buckets_sz;
    if (table->nb_iterators == 0 && should_resize) {
        if (c_hash_table_resize(table, table->buckets_sz / 2) == -1)
            return -1;
    }

    table->nb_entries--;
    return 1;
}

int
c_hash_table_get(struct c_hash_table *table, const void *key, void **value) {
    struct c_hash_table_entry *entry;

    entry = c_hash_table_entry(table, key);
    if (!entry)
        return 0;

    *value = entry->value;
    return 1;
}

bool
c_hash_table_contains(struct c_hash_table *table, const void *key) {
    return c_hash_table_entry(table, key) != NULL;
}

struct c_hash_table_iterator *
c_hash_table_iterate(struct c_hash_table *table) {
    struct c_hash_table_iterator *it;

    it = c_malloc(sizeof(struct c_hash_table_iterator));
    if (!it) {
        c_set_error("cannot allocate iterator: %m");
        return NULL;
    }

    it->table = table;
    it->bucket = SIZE_MAX;
    it->entry = 0;

    table->nb_iterators++;

    return it;
}

void
c_hash_table_iterator_delete(struct c_hash_table_iterator *it) {
    if (!it)
        return;

    assert(it->table->nb_iterators > 0);
    it->table->nb_iterators--;

    memset(it, 0, sizeof(struct c_hash_table_iterator));
    c_free(it);
}

int
c_hash_table_iterator_next(struct c_hash_table_iterator *it,
                           void **key, void **value) {
    if (it->bucket == SIZE_MAX) {
        it->bucket = 0;
        it->entry = 0;
    } else {
        it->entry++;
    }

    for (;;) {
        struct c_hash_table_bucket *bucket;
        struct c_hash_table_entry *entry;

        bucket = it->table->buckets + it->bucket;
        if (it->entry >= bucket->sz) {
            it->bucket++;
            bucket++;

            if (it->bucket >= it->table->buckets_sz) {
                it->bucket = SIZE_MAX;
                it->entry = 0;
                return 0;
            }

            it->entry = 0;
        }

        if (!bucket->entries)
            continue;
        entry = bucket->entries + it->entry;

        if (C_TABLE_ENTRY_IS_USED(entry)) {
            if (key)
                *key = entry->key;
            if (value)
                *value = entry->value;
            break;
        }

        it->entry++;
    }

    return 1;
}

void
c_hash_table_iterator_set_value(struct c_hash_table_iterator *it, void *value) {
    struct c_hash_table_bucket *bucket;
    struct c_hash_table_entry *entry;

    if (it->bucket == SIZE_MAX)
        return;

    bucket = it->table->buckets + it->bucket;
    entry = bucket->entries + it->entry;

    entry->value = value;
}

int
c_hash_table_keys(struct c_hash_table *table, void ***pkeys, size_t *p_nb_keys) {
    struct c_hash_table_iterator *it;
    size_t nb_keys;
    void **keys, *key;
    size_t idx;

    nb_keys = table->nb_entries;
    if (nb_keys == 0) {
        *pkeys = NULL;
        *p_nb_keys = 0;
        return 0;
    }

    keys = c_calloc(nb_keys, sizeof(void *));
    if (!keys)
        return -1;

    it = c_hash_table_iterate(table);
    if (!it) {
        c_free(keys);
        return -1;
    }

    idx = 0;
    while (c_hash_table_iterator_next(it, &key, NULL) == 1)
        keys[idx++] = key;

    c_hash_table_iterator_delete(it);

    *pkeys = keys;
    *p_nb_keys = nb_keys;
    return 0;
}

uint32_t
c_hash_int32(const void *key) {
    int32_t integer;
    unsigned char *bytes;
    uint32_t hash;

    integer = C_POINTER_TO_INT32(key);
    bytes = (unsigned char *)&integer;

    hash = 5381;
    for (int i = 0; i < 4; i++)
        hash = ((hash << 5) + hash) ^ bytes[i];

    return hash;
}

bool
c_equal_int32(const void *k1, const void *k2) {
    return C_POINTER_TO_INT32(k1) == C_POINTER_TO_INT32(k2);
}

uint32_t
c_hash_string(const void *key) {
    const unsigned char *str;
    uint32_t hash;

    hash = 5381;
    for (str = key; *str; str++)
        hash = ((hash << 5) + hash) ^ *str;

    return hash;
}

bool
c_equal_string(const void *k1, const void *k2) {
    return strcmp(k1, k2) == 0;
}

static struct c_hash_table_entry *
c_hash_table_entry(struct c_hash_table *table, const void *key) {
    struct c_hash_table_bucket *bucket;
    uint32_t hash;

    hash = table->hash_func(key);
    if (hash == C_UNUSED_HASH)
        hash++;

    bucket = table->buckets + (hash % table->buckets_sz);
    if (!bucket->entries)
        return NULL;

    for (size_t i = 0; i < bucket->sz; i++) {
        struct c_hash_table_entry *entry;

        entry = bucket->entries + i;
        if (!C_TABLE_ENTRY_IS_USED(entry))
            continue;

        if (entry->hash == hash && table->equal_func(key, entry->key))
            return entry;
    }

    return NULL;
}

void
c_hash_table_print(struct c_hash_table *table, FILE *file) {
    fprintf(file, "entries: %zu\n", table->nb_entries);
    fprintf(file, "buckets: %zu\n", table->buckets_sz);

    for (size_t b = 0; b < table->buckets_sz; b++) {
        struct c_hash_table_bucket *bucket;

        bucket = table->buckets + b;

        fprintf(file, "bucket %04zu\n", b);

        for (size_t e = 0; e < bucket->sz; e++) {
            struct c_hash_table_entry *entry;

            entry = bucket->entries + e;

            fprintf(file, "  entry %02zu  ", e);

            if (C_TABLE_ENTRY_IS_USED(entry)) {
                fprintf(file, "key=%08"PRIxPTR" value=%08"PRIxPTR
                        " hash=%"PRIu32,
                        (intptr_t)entry->key, (intptr_t)entry->value,
                        entry->hash);
            }

            fputc('\n', file);
        }
    }
}

static int
c_hash_table_resize(struct c_hash_table *table, size_t sz) {
    struct c_hash_table_bucket *buckets;

    buckets = c_calloc(sz, sizeof(struct c_hash_table_bucket));
    if (!buckets) {
        c_set_error("cannot allocate buckets: %m");
        return -1;
    }

    for (size_t b = 0; b < table->buckets_sz; b++) {
        struct c_hash_table_bucket *bucket;

        bucket = table->buckets + b;
        if (!bucket->entries)
            continue;

        for (size_t e = 0; e < bucket->sz; e++) {
            struct c_hash_table_entry *entry;

            entry = bucket->entries + e;
            if (!C_TABLE_ENTRY_IS_USED(entry))
                continue;

            if (c_hash_table_insert_in(table, buckets, sz,
                                       entry->key, entry->value,
                                       entry->hash, true) == -1) {
                for (size_t i = 0; i < sz; i++)
                    c_free(buckets[i].entries);
                c_free(buckets);
                return -1;
            }
        }
    }

    for (size_t b = 0; b < table->buckets_sz; b++)
        c_free(table->buckets[b].entries);
    c_free(table->buckets);

    table->buckets_sz = sz;
    table->buckets = buckets;

    return 0;
}

static int
c_hash_table_insert_in(struct c_hash_table *table,
                       struct c_hash_table_bucket *buckets, size_t sz,
                       void *key, void *value, uint32_t hash,
                       bool is_resizing) {
    struct c_hash_table_bucket *bucket;
    struct c_hash_table_entry *entry;
    bool new_entry_inserted;

    bucket = buckets + (hash % sz);

    entry = NULL;
    new_entry_inserted = true;

    if (bucket->entries) {
        for (size_t i = 0; i < bucket->sz; i++) {
            struct c_hash_table_entry *curr_entry;

            curr_entry = bucket->entries + i;

            if (!C_TABLE_ENTRY_IS_USED(curr_entry)) {
                if (!entry)
                    entry = curr_entry;

                if (is_resizing)
                    break;
                continue;
            }

            if (hash == curr_entry->hash
                && table->equal_func(key, curr_entry->key)) {
                entry = curr_entry;
                new_entry_inserted = false;
                break;
            }
        }
    } else {
        bucket->sz = 1;
        bucket->entries = c_calloc(bucket->sz, sizeof(struct c_hash_table_entry));
        if (!bucket->entries) {
            c_set_error("cannot allocate entries: %m");
            return -1;
        }

        entry = bucket->entries;
    }

    if (!entry) {
        struct c_hash_table_entry *entries;
        size_t sz;

        sz = bucket->sz + 1;
        entries = c_realloc(bucket->entries,
                            sz * sizeof(struct c_hash_table_entry));
        if (!entries) {
            c_set_error("cannot reallocate entries: %m");
            return -1;
        }

        memset(entries + bucket->sz, 0,
               (sz - bucket->sz) * sizeof(struct c_hash_table_entry));

        entry = entries + bucket->sz;

        bucket->entries = entries;
        bucket->sz = sz;
    }

    entry->key = key;
    entry->value = value;
    entry->hash = hash;

    return new_entry_inserted ? 1 : 0;
}
