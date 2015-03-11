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

#include "internal.h"

int
c_parse_integer(const char *string, int64_t min, int64_t max,
                int64_t *pvalue, size_t *psz) {
    long long ll;
    char *end;

    if (!((string[0] >= '0' && string[0] <= '9')
       || (string[0] == '-')
       || (string[0] == '+'))) {
        c_set_error("invalid value");
        return -1;
    }

    errno = 0;
    ll = strtoll(string, &end, 10);
    if (errno) {
        c_set_error("%s", strerror(errno));
        return -1;
    }

    if (end == string) {
        c_set_error("empty value");
        return -1;
    }

    if (ll < min) {
        c_set_error("value too small");
        return -1;
    }

    if (ll > max) {
        c_set_error("value too large");
        return -1;
    }

    if (pvalue)
        *pvalue = ll;

    if (psz)
        *psz = (size_t)(end - string);
    return 0;
}

int
c_parse_i8(const char *string, int8_t *pvalue, size_t *psz) {
    int64_t value;

    if (c_parse_integer(string, INT8_MIN, INT8_MAX, &value, psz) == -1)
        return -1;

    if (pvalue)
        *pvalue = value;
    return 0;
}

int
c_parse_i16(const char *string, int16_t *pvalue, size_t *psz) {
    int64_t value;

    if (c_parse_integer(string, INT16_MIN, INT16_MAX, &value, psz) == -1)
        return -1;

    if (pvalue)
        *pvalue = value;
    return 0;
}

int
c_parse_i32(const char *string, int32_t *pvalue, size_t *psz) {
    int64_t value;

    if (c_parse_integer(string, INT32_MIN, INT32_MAX, &value, psz) == -1)
        return -1;

    if (pvalue)
        *pvalue = value;
    return 0;
}

int
c_parse_i64(const char *string, int64_t *pvalue, size_t *psz) {
    int64_t value;

    if (c_parse_integer(string, INT64_MIN, INT64_MAX, &value, psz) == -1)
        return -1;

    if (pvalue)
        *pvalue = value;
    return 0;
}


int
c_parse_unsigned_integer(const char *string, uint64_t min, uint64_t max,
                         uint64_t *pvalue, size_t *psz) {
    unsigned long long ull;
    char *end;

    if (!((string[0] >= '0' && string[0] <= '9')
       || (string[0] == '-')
       || (string[0] == '+'))) {
        c_set_error("invalid value");
        return -1;
    }

    if (string[0] == '-' && string[1] != '0') {
        c_set_error("negative value");
        return -1;
    }

    errno = 0;
    ull = strtoull(string, &end, 10);
    if (errno) {
        c_set_error("%s", strerror(errno));
        return -1;
    }

    if (end == string) {
        c_set_error("empty value");
        return -1;
    }

    if (ull < min) {
        c_set_error("value too small");
        return -1;
    }

    if (ull > max) {
        c_set_error("value too large");
        return -1;
    }

    if (pvalue)
        *pvalue = ull;

    if (psz)
        *psz = (size_t)(end - string);
    return 0;
}

int
c_parse_u8(const char *string, uint8_t *pvalue, size_t *psz) {
    uint64_t value;

    if (c_parse_unsigned_integer(string, 0, UINT8_MAX, &value, psz) == -1)
        return -1;

    if (pvalue)
        *pvalue = value;
    return 0;
}

int
c_parse_u16(const char *string, uint16_t *pvalue, size_t *psz) {
    uint64_t value;

    if (c_parse_unsigned_integer(string, 0, UINT16_MAX, &value, psz) == -1)
        return -1;

    if (pvalue)
        *pvalue = value;
    return 0;
}

int
c_parse_u32(const char *string, uint32_t *pvalue, size_t *psz) {
    uint64_t value;

    if (c_parse_unsigned_integer(string, 0, UINT32_MAX, &value, psz) == -1)
        return -1;

    if (pvalue)
        *pvalue = value;
    return 0;
}

int
c_parse_u64(const char *string, uint64_t *pvalue, size_t *psz) {
    uint64_t value;

    if (c_parse_unsigned_integer(string, 0, UINT64_MAX, &value, psz) == -1)
        return -1;

    if (pvalue)
        *pvalue = value;
    return 0;
}
