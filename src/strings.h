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

#ifndef LIBCORE_STRINGS_H
#define LIBCORE_STRINGS_H

#include <stdarg.h>
#include <stdlib.h>

char *c_strdup(const char *);
char *c_strndup(const char *, size_t);
void *c_memdup(const void *, size_t);

int c_vasprintf(char **, const char *, va_list);
int c_asprintf(char **, const char *, ...)
    __attribute__((format(printf, 2, 3)));

size_t c_strlcpy(char *, const char *, size_t);

void *c_memory_search(const void *, size_t, const void *, size_t);
char *c_memory_search_string(const void *, size_t, const char *);
char *c_string_search(const char *, const char *);

size_t c_memspn(const void *, size_t, const char *);
size_t c_memcspn(const void *, size_t, const char *);

#endif
