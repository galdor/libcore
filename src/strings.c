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

char *
c_strdup(const char *str) {
    return c_strndup(str, strlen(str));
}

char *
c_strndup(const char *str, size_t len) {
    char *nstr;

    nstr = c_malloc(len + 1);
    memcpy(nstr, str, len);
    nstr[len] = '\0';

    return nstr;
}

int
c_vasprintf(char **pstr, const char *fmt, va_list ap) {
    size_t sz, fmt_len;
    char *str;

    fmt_len = strlen(fmt);

    sz = fmt_len + 1;
    str = c_malloc(sz);
    if (!str)
        return -1;

    for (;;) {
        va_list apc;
        char *nstr;
        int ret;

        va_copy(apc, ap);
        ret = vsnprintf(str, sz, fmt, apc);
        va_end(apc);

        if (ret == -1) {
            c_set_error("cannot format string: %s", strerror(errno));
            c_free(str);
            return -1;
        }

        if ((size_t)ret < sz) {
            /* Success */
            *pstr = str;
            return ret;
        }

        sz = (size_t)ret + 1;
        nstr = c_realloc(str, sz);
        if (!nstr) {
            c_free(str);
            return -1;
        }

        str = nstr;
    }
}

int
c_asprintf(char **pstr, const char *fmt, ...) {
    va_list ap;
    int ret;

    va_start(ap, fmt);
    ret = c_vasprintf(pstr, fmt, ap);
    va_end(ap);

    return ret;
}

/*
 * Copyright (c) 1998 Todd C. Miller <Todd.Miller@courtesan.com>
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
size_t
c_strlcpy(char *dst, const char *src, size_t siz)
{
    char *d = dst;
    const char *s = src;
    size_t n = siz;

    /* Copy as many bytes as will fit */
    if (n != 0) {
        while (--n != 0) {
            if ((*d++ = *s++) == '\0')
                break;
        }
    }

    /* Not enough room in dst, add NUL and traverse rest of src */
    if (n == 0) {
        if (siz != 0)
            *d = '\0';      /* NUL-terminate dst */
        while (*s++)
            ;
    }

    return (size_t)(s - src - 1);    /* count does not include NUL */
}
