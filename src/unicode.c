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

size_t
c_ustring_length(const uint32_t *ustring) {
    const uint32_t *ptr;
    size_t length;

    length = 0;

    ptr = ustring;
    while (*ptr != 0) {
        length++;
        ptr++;
    }

    return length;
}

bool
c_ustring_equal(const uint32_t *ustring1, const uint32_t *ustring2) {
    size_t length1, length2;

    length1 = c_ustring_length(ustring1);
    length2 = c_ustring_length(ustring2);

    if (length1 != length2)
        return false;

    return memcmp(ustring1, ustring2, length1 * 4) == 0;
}

bool
c_codepoint_is_valid(uint32_t codepoint) {
    return (codepoint <= 0x10ffff)
        && !(codepoint >= 0xd800 && codepoint <= 0xdfff); /* surrogate pairs */
}

int
c_utf8_read_codepoint(const char *string, uint32_t *pcodepoint,
                      size_t *plength) {
    static const uint8_t ranges[256] = {
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /* 00 - 0f */
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /* 10 - 1f */
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /* 20 - 2f */
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /* 30 - 3f */
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /* 40 - 4f */
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /* 50 - 5f */
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /* 60 - 6f */
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /* 70 - 7f */
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* 80 - 8f */
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* 90 - 9f */
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* a0 - af */
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* b0 - bf */
        0, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, /* c0 - cf */
        2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, /* d0 - df */
        3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 6, 6, /* e0 - ef */
        7, 8, 8, 8, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* f0 - ff */
    };

    uint32_t codepoint;
    const unsigned char *ptr;
    size_t length;
    int range;

    ptr = (const unsigned char *)string;

    /* Reference: Unicode 7.0 - Table 3.7 */

    range = ranges[*ptr];
    switch (range) {
    case 1:
        /* 0xxxxxxx */
        length = 1;

        codepoint = (uint32_t)ptr[0];
        break;

    case 2:
        /* 110yyyyy 10xxxxxx */
        length = 2;

        if (ptr[1] < 0x80 || ptr[1] > 0xbf)
            goto invalid_byte_sequence;

        codepoint  = (((uint32_t)ptr[0] & 0x1f) << 6);
        codepoint |=  ((uint32_t)ptr[1] & 0x3f);
        break;

    case 3:
    case 4:
    case 5:
    case 6:
        /* 1110zzzz 10yyyyyy 10xxxxxx */
        length = 3;

        if (range == 3) {
            if (ptr[1] < 0xa0 || ptr[1] > 0xbf)
                goto invalid_byte_sequence;
        } else if (range == 4) {
            if (ptr[1] < 0x80 || ptr[1] > 0xbf)
                goto invalid_byte_sequence;
        } else if (range == 5) {
            if (ptr[1] < 0x80 || ptr[1] > 0x9f)
                goto invalid_byte_sequence;
        }

        if (ptr[2] < 0x80 || ptr[2] > 0xbf)
            goto invalid_byte_sequence;

        codepoint  = (((uint32_t)ptr[0] & 0x0f) << 12);
        codepoint |= (((uint32_t)ptr[1] & 0x3f) <<  6);
        codepoint |=  ((uint32_t)ptr[2] & 0x3f);
        break;

    case 7:
    case 8:
    case 9:
        /* 11110uuu 10uuzzzz 10yyyyyy 10xxxxxx */
        length = 4;

        if (range == 7) {
            if (ptr[1] < 0x90 || ptr[1] > 0xbf)
                goto invalid_byte_sequence;
        } else if (range == 8) {
            if (ptr[1] < 0x80 || ptr[1] > 0xbf)
                goto invalid_byte_sequence;
        } else if (range == 9) {
            if (ptr[1] < 0x80 || ptr[1] > 0x8f)
                goto invalid_byte_sequence;
        }

        if (ptr[2] < 0x80 || ptr[2] > 0xbf)
            goto invalid_byte_sequence;

        if (ptr[3] < 0x80 || ptr[3] > 0xbf)
            goto invalid_byte_sequence;

        codepoint  = (((uint32_t)ptr[0] & 0x07) << 18);
        codepoint |= (((uint32_t)ptr[1] & 0x3f) << 12);
        codepoint |= (((uint32_t)ptr[2] & 0x3f) <<  6);
        codepoint |=  ((uint32_t)ptr[3] & 0x3f);
        break;

    default:
        goto invalid_byte_sequence;
    }

    if (!c_codepoint_is_valid(codepoint)) {
        c_set_error("invalid codepoint U+%X", codepoint);
        return -1;
    }

    *pcodepoint = codepoint;
    if (plength)
        *plength = length;
    return 0;

invalid_byte_sequence:
    c_set_error("invalid byte sequence");
    return -1;
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

uint32_t *
c_utf8_decode(const char *string) {
    uint32_t *codepoints;
    size_t nb_codepoints, codepoints_sz, length;
    const char *ptr;

    codepoints = NULL;
    nb_codepoints = 0;

    ptr = string;
    length = strlen(string);

    codepoints_sz = length + 1;
    codepoints = c_calloc(codepoints_sz, sizeof(uint32_t));
    if (!codepoints)
        return NULL;

    while (*ptr != '\0') {
        uint32_t codepoint;
        size_t nb_bytes;

        if (c_utf8_read_codepoint(ptr, &codepoint, &nb_bytes) == -1)
            goto error;

        if (nb_codepoints >= codepoints_sz - 1) {
            uint32_t *ncodepoints;

            codepoints_sz *= 2;
            ncodepoints = c_realloc(codepoints,
                                    codepoints_sz * sizeof(uint32_t));
            if (!ncodepoints)
                goto error;

            codepoints = ncodepoints;
        }

        codepoints[nb_codepoints++] = codepoint;

        ptr += nb_bytes;
    }

    codepoints[nb_codepoints] = 0;
    return codepoints;

error:
    c_free(codepoints);
    return NULL;
}
