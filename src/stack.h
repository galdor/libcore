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

#ifndef LIBCORE_STACK_H
#define LIBCORE_STACK_H

struct c_stack *c_stack_new(void);
void c_stack_delete(struct c_stack *);

size_t c_stack_length(const struct c_stack *);
bool c_stack_is_empty(const struct c_stack *);

void c_stack_clear(struct c_stack *);
int c_stack_push(struct c_stack *, void *);
void *c_stack_peek(const struct c_stack *);
void *c_stack_pop(struct c_stack *);

struct c_stack_entry *c_stack_top_entry(struct c_stack *);
struct c_stack_entry *c_stack_bottom_entry(struct c_stack *);

void c_stack_entry_delete(struct c_stack_entry *);
struct c_stack_entry *c_stack_entry_prev(struct c_stack_entry *);
struct c_stack_entry *c_stack_entry_next(struct c_stack_entry *);
void *c_stack_entry_value(const struct c_stack_entry *);
void c_stack_remove_entry(struct c_stack *, struct c_stack_entry *);

#endif
