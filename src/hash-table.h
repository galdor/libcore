/*
 * Copyright (c) 2013-2014 Nicolas Martyanoff
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

#ifndef LIBCORE_HASH_TABLE_H
#define LIBCORE_HASH_TABLE_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#define C_INT32_TO_POINTER(i_) ((void *)(intptr_t)(int32_t)(i_))
#define C_POINTER_TO_INT32(p_) ((int32_t)(intptr_t)(p_))

typedef uint32_t (*c_hash_func)(const void *);
typedef bool (*c_equal_func)(const void *, const void *);

struct c_hash_table *c_hash_table_new(c_hash_func, c_equal_func);
void c_hash_table_delete(struct c_hash_table *);
size_t c_hash_table_nb_entries(const struct c_hash_table *);
bool c_hash_table_is_empty(const struct c_hash_table *);
void c_hash_table_clear(struct c_hash_table *);
int c_hash_table_insert(struct c_hash_table *, void *, void *);
int c_hash_table_insert2(struct c_hash_table *, void *, void *,
                         void **, void **);
int c_hash_table_remove(struct c_hash_table *, const void *);
int c_hash_table_remove2(struct c_hash_table *, const void *,
                         void **, void **);
int c_hash_table_get(struct c_hash_table *, const void *, void **);
bool c_hash_table_contains(struct c_hash_table *, const void *);
void c_hash_table_print(struct c_hash_table *, FILE *);

struct c_hash_table_iterator *c_hash_table_iterate(struct c_hash_table *);
void c_hash_table_iterator_delete(struct c_hash_table_iterator *);
int c_hash_table_iterator_next(struct c_hash_table_iterator *,
                               void **, void **);
void c_hash_table_iterator_set_value(struct c_hash_table_iterator *, void *);

uint32_t c_hash_int32(const void *);
bool c_equal_int32(const void *, const void *);

uint32_t c_hash_string(const void *);
bool c_equal_string(const void *, const void *);

#endif
