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

#include "internal.h"

static __thread char c_error_buf[C_ERROR_BUFSZ];

const char *
c_get_error(void) {
    return c_error_buf;
}

void
c_set_error(const char *fmt, ...) {
    char buf[C_ERROR_BUFSZ];
    va_list ap;
    int ret;

    va_start(ap, fmt);
    ret = vsnprintf(buf, C_ERROR_BUFSZ, fmt, ap);
    va_end(ap);

    if ((size_t)ret >= C_ERROR_BUFSZ)
        ret = C_ERROR_BUFSZ - 1;

    memcpy(c_error_buf, buf, (size_t)ret);
    c_error_buf[ret] = '\0';
}
