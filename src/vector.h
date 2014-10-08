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

#ifndef LIBCORE_VECTOR_H
#define LIBCORE_VECTOR_H

#include <stdbool.h>
#include <stdlib.h>

struct c_vector *c_vector_new(size_t);
void c_vector_delete(struct c_vector *);

void *c_vector_entries(const struct c_vector *);
size_t c_vector_length(const struct c_vector *);
bool c_vector_is_empty(const struct c_vector *);
void *c_vector_entry(const struct c_vector *, size_t);

void c_vector_clear(struct c_vector *);
int c_vector_append(struct c_vector *, const void *);
void c_vector_set(struct c_vector *, size_t, const void *);
void c_vector_remove(struct c_vector *, size_t);

#endif
