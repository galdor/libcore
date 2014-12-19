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

#ifndef LIBCORE_QUEUE_H
#define LIBCORE_QUEUE_H

#include <stdlib.h>

struct c_queue *c_queue_new(void);
void c_queue_delete(struct c_queue *);

size_t c_queue_length(const struct c_queue *);
bool c_queue_is_empty(const struct c_queue *);

void c_queue_clear(struct c_queue *);
int c_queue_push(struct c_queue *, void *);
void *c_queue_peek(const struct c_queue *);
void *c_queue_pop(struct c_queue *);

struct c_queue_entry *c_queue_first_entry(struct c_queue *);
struct c_queue_entry *c_queue_last_entry(struct c_queue *);

void c_queue_entry_delete(struct c_queue_entry *);
struct c_queue_entry *c_queue_entry_prev(struct c_queue_entry *);
struct c_queue_entry *c_queue_entry_next(struct c_queue_entry *);
void *c_queue_entry_value(const struct c_queue_entry *);
void c_queue_remove_entry(struct c_queue *, struct c_queue_entry *);

#endif
