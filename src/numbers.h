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

#ifndef LIBCORE_NUMBERS_H
#define LIBCORE_NUMBERS_H

#include <stdlib.h>
#include <stdint.h>

int c_parse_integer(const char *, int64_t, int64_t,
                    int64_t *, size_t *);

int c_parse_i8(const char *, int8_t *, size_t *);
int c_parse_i16(const char *, int16_t *, size_t *);
int c_parse_i32(const char *, int32_t *, size_t *);
int c_parse_i64(const char *, int64_t *, size_t *);

int c_parse_unsigned_integer(const char *, uint64_t, uint64_t,
                             uint64_t *, size_t *);

int c_parse_u8(const char *, uint8_t *, size_t *);
int c_parse_u16(const char *, uint16_t *, size_t *);
int c_parse_u32(const char *, uint32_t *, size_t *);
int c_parse_u64(const char *, uint64_t *, size_t *);
int c_parse_size(const char *, size_t *, size_t *);

#endif
