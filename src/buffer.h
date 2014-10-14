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

#ifndef LIBCORE_BUFFER_H
#define LIBCORE_BUFFER_H

#include <sys/types.h>

#include <stdlib.h>

struct c_buffer *c_buffer_new(void);
void c_buffer_delete(struct c_buffer *);

void *c_buffer_data(const struct c_buffer *);
size_t c_buffer_length(const struct c_buffer *);
size_t c_buffer_size(const struct c_buffer *);
size_t c_buffer_free_space(const struct c_buffer *);

void c_buffer_reset(struct c_buffer *);
void c_buffer_clear(struct c_buffer *);
void c_buffer_truncate(struct c_buffer *, size_t);

void *c_buffer_reserve(struct c_buffer *, size_t);
int c_buffer_increase_length(struct c_buffer *, size_t);
int c_buffer_insert(struct c_buffer *, size_t, const void *, size_t);
int c_buffer_add(struct c_buffer *, const void *, size_t);
int c_buffer_add_buffer(struct c_buffer *, const struct c_buffer *);
int c_buffer_add_string(struct c_buffer *, const char *);
int c_buffer_add_vprintf(struct c_buffer *, const char *, va_list);
int c_buffer_add_printf(struct c_buffer *, const char *, ...)
    __attribute__((format(printf, 2, 3)));

void c_buffer_skip(struct c_buffer *, size_t);
size_t c_buffer_remove_before(struct c_buffer *, size_t, size_t);
size_t c_buffer_remove_after(struct c_buffer *, size_t, size_t);
size_t c_buffer_remove(struct c_buffer *, size_t);

void *c_buffer_extract(struct c_buffer *, size_t *);
char *c_buffer_extract_string(struct c_buffer *, size_t *);
void *c_buffer_dup(const struct c_buffer *);
char *c_buffer_dup_string(const struct c_buffer *);

ssize_t c_buffer_read(struct c_buffer *, int, size_t);
ssize_t c_buffer_write(struct c_buffer *, int);

#endif
