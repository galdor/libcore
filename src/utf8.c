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

#include "internal.h"

bool
c_utf8_is_valid_codepoint(uint32_t codepoint) {
    return (codepoint <= 0x10ffff)
        && !(codepoint >= 0xd800 && codepoint <= 0xdfff); /* surrogate pairs */
}

int
c_utf8_read_codepoint(const char *string, uint32_t *pcodepoint,
                      size_t *plength) {
    uint32_t codepoint;
    const unsigned char *ptr;
    size_t length;

    ptr = (const unsigned char *)string;

    /* Reference: Unicode 7.0 - Table 3.7 */

    if (ptr[0] <= 0x7f) {
        /* 0xxxxxxx */
        length = 1;
        codepoint = (uint32_t)ptr[0];
    } else if ((ptr[0] >= 0xc2) && (ptr[0] <= 0xdf)
            && (ptr[1] >= 0x80) && (ptr[1] <= 0xbf)) {
        /* 110yyyyy 10xxxxxx */
        length = 2;
        codepoint  = (((uint32_t)ptr[0] & 0x1f) << 6);
        codepoint |=  ((uint32_t)ptr[1] & 0x3f);
    } else if (((ptr[0] == 0xe0)
             && (ptr[1] >= 0xa0) && (ptr[1] <= 0xbf)
             && (ptr[2] >= 0x80) && (ptr[2] <= 0xbf))
            || ((ptr[0] >= 0xe1) && (ptr[0] <= 0xec)
             && (ptr[1] >= 0x80) && (ptr[1] <= 0xbf)
             && (ptr[2] >= 0x80) && (ptr[2] <= 0xbf))
            || ((ptr[0] == 0xed)
             && (ptr[1] >= 0x80) && (ptr[1] <= 0xef)
             && (ptr[2] >= 0x80) && (ptr[2] <= 0xbf))
            || ((ptr[0] >= 0xee) && (ptr[0] <= 0xef)
             && (ptr[1] >= 0x80) && (ptr[1] <= 0xbf)
             && (ptr[2] >= 0x80) && (ptr[2] <= 0xbf))) {
        /* 1110zzzz 10yyyyyy 10xxxxxx */
        length = 3;
        codepoint  = (((uint32_t)ptr[0] & 0x0f) << 12);
        codepoint |= (((uint32_t)ptr[1] & 0x3f) <<  6);
        codepoint |=  ((uint32_t)ptr[2] & 0x3f);
    } else if (((ptr[0] == 0xf0)
             && (ptr[1] >= 0x90) && (ptr[1] <= 0xbf)
             && (ptr[2] >= 0x80) && (ptr[2] <= 0xbf)
             && (ptr[3] >= 0x80) && (ptr[3] <= 0xbf))
            || ((ptr[0] >= 0xf1) && (ptr[0] <= 0xf3)
             && (ptr[1] >= 0x80) && (ptr[1] <= 0xbf)
             && (ptr[2] >= 0x80) && (ptr[2] <= 0xbf)
             && (ptr[3] >= 0x80) && (ptr[3] <= 0xbf))
            || ((ptr[0] == 0xf4)
             && (ptr[1] >= 0x80) && (ptr[1] <= 0x8f)
             && (ptr[2] >= 0x80) && (ptr[2] <= 0xbf)
             && (ptr[3] >= 0x80) && (ptr[3] <= 0xbf))) {
        /* 11110uuu 10uuzzzz 10yyyyyy 10xxxxxx */
        length = 4;
        codepoint  = (((uint32_t)ptr[0] & 0x07) << 18);
        codepoint |= (((uint32_t)ptr[1] & 0x3f) << 12);
        codepoint |= (((uint32_t)ptr[2] & 0x3f) <<  6);
        codepoint |=  ((uint32_t)ptr[3] & 0x3f);
    } else {
        c_set_error("invalid byte sequence");
        return -1;
    }

    if (!c_utf8_is_valid_codepoint(codepoint)) {
        c_set_error("invalid codepoint U+%X", codepoint);
        return -1;
    }

    *pcodepoint = codepoint;
    if (plength)
        *plength = length;
    return 0;
}

bool
c_utf8_is_valid_string(const char *string) {
    const char *ptr;

    ptr = string;
    while (*ptr != '\0') {
        uint32_t codepoint;
        size_t length;

        if (c_utf8_read_codepoint(ptr, &codepoint, &length) == -1)
            return false;

        ptr += length;
    }

    return true;
}
