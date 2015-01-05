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
    if (!nstr)
        return NULL;

    memcpy(nstr, str, len);
    nstr[len] = '\0';

    return nstr;
}

void *
c_memdup(const void *ptr, size_t sz) {
    void *nptr;

    nptr = c_malloc(sz);
    if (!nptr)
        return NULL;

    memcpy(nptr, ptr, sz);
    return nptr;
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

void *
c_memory_search(const void *haystack, size_t haystack_sz,
                const void *needle, size_t needle_sz) {
    const unsigned char *nptr, *hptr;
    size_t nlen, hlen;

    size_t skip_table[256];

    /* Boyer - Moore - Horspool */

    hptr = (const unsigned char *)haystack;
    hlen = haystack_sz;

    nptr = (const unsigned char *)needle;
    nlen = needle_sz;

    /* An empty needle matches the beginning of the haystack */
    if (nlen == 0)
        return (void *)hptr;

    /* Fill the skip table */
    for (size_t i = 0; i <= 256; i++)
        skip_table[i] = nlen;

    for (size_t i = 0; i < nlen - 1; i++)
        skip_table[nptr[i]] = nlen - 1 - i;

    /* Search for the needle */
    while (nlen <= hlen) {
        size_t skip;

        if (memcmp(hptr, nptr, nlen) == 0)
            return (void *)hptr;

        skip = skip_table[hptr[nlen - 1]];
        if (skip > hlen)
            break;

        hlen -= skip;
        hptr += skip;
    }

    return NULL;
}

char *
c_memory_search_string(const void *haystack, size_t haystack_sz,
                       const char *needle) {
    return c_memory_search(haystack, haystack_sz, needle, strlen(needle));
}

char *
c_string_search(const char *haystack, const char *needle) {
    return c_memory_search(haystack, strlen(haystack), needle, strlen(needle));
}
