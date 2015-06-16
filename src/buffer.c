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

#include <unistd.h>

#include "internal.h"

static void c_buffer_repack(struct c_buffer *);
static int c_buffer_resize(struct c_buffer *, size_t);
static int c_buffer_grow(struct c_buffer *, size_t);
static int c_buffer_ensure_free_space(struct c_buffer *, size_t);

/*
 *                       sz
 *   <----------------------------------------->
 *
 *    skip             len
 *   <----> <------------------------>
 *
 *  +------+--------------------------+--------+
 *  |      |         content          |        |
 *  +------+--------------------------+--------+
 */

struct c_buffer {
    char *data;
    size_t sz;
    size_t skip;
    size_t len;
};

struct c_buffer *
c_buffer_new(void) {
    struct c_buffer *buf;

    buf = c_malloc0(sizeof(struct c_buffer));
    if (!buf)
        return NULL;

    return buf;
}

void
c_buffer_delete(struct c_buffer *buf) {
    if (!buf)
        return;

    c_free(buf->data);
    buf->data = NULL;

    c_free(buf);
}

void *
c_buffer_data(const struct c_buffer *buf) {
    return buf->data + buf->skip;
}

size_t
c_buffer_length(const struct c_buffer *buf) {
    return buf->len;
}

size_t
c_buffer_size(const struct c_buffer *buf) {
    return buf->sz;
}

size_t
c_buffer_free_space(const struct c_buffer *buf) {
    return buf->sz - buf->len - buf->skip;
}

void
c_buffer_reset(struct c_buffer *buf) {
    c_free(buf->data);
    buf->data = NULL;

    buf->sz = 0;
    buf->skip = 0;
    buf->len = 0;
}

void
c_buffer_clear(struct c_buffer *buf) {
    buf->skip = 0;
    buf->len = 0;
}

void
c_buffer_truncate(struct c_buffer *buf, size_t sz) {
    if (sz > buf->len)
        sz = buf->len;

    buf->len = sz;

    if (buf->len == 0)
        buf->skip = 0;
}

void *
c_buffer_reserve(struct c_buffer *buf, size_t sz) {
    if (c_buffer_ensure_free_space(buf, sz) == -1)
        return NULL;

    return buf->data + buf->skip + buf->len;
}

int
c_buffer_increase_length(struct c_buffer *buf, size_t n) {
    if (n > buf->sz - buf->len - buf->skip) {
        c_set_error("length increment too large");
        return -1;
    }

    buf->len += n;
    return 0;
}

int
c_buffer_insert(struct c_buffer *buf, size_t offset, const void *data,
                 size_t sz) {
    char *ndata;
    size_t nsz;

    if (sz == 0)
        return 0;

    if (offset > buf->len) {
        c_set_error("invalid offset");
        return -1;
    }

    if (!buf->data) {
        nsz = sz;
        if (nsz < C_BUFFER_MIN_SIZE)
            nsz = C_BUFFER_MIN_SIZE;

        buf->sz = nsz;
        buf->data = c_malloc(buf->sz);
        if (!buf->data)
            return -1;
    } else if (c_buffer_free_space(buf) < sz) {
        c_buffer_repack(buf);

        if (c_buffer_free_space(buf) < sz) {
            if (sz > buf->sz) {
                nsz = buf->sz + sz;
            } else {
                nsz = buf->sz * 2;
            }

            if (c_buffer_resize(buf, nsz) == -1)
                return -1;
        }
    }

    ndata = buf->data + buf->skip + offset;

    if (offset < buf->len)
        memmove(ndata + sz, ndata, buf->len - offset);
    memcpy(ndata, data, sz);

    buf->len += sz;
    return 0;
}

int
c_buffer_add(struct c_buffer *buf, const void *data, size_t sz) {
    return c_buffer_insert(buf, buf->len, data, sz);
}

int
c_buffer_add_buffer(struct c_buffer *buf, const struct c_buffer *src) {
    return c_buffer_add(buf, src->data + src->skip, src->len);
}

int
c_buffer_add_string(struct c_buffer *buf, const char *str) {
    return c_buffer_insert(buf, buf->len, str, strlen(str));
}

int
c_buffer_add_vprintf(struct c_buffer *buf, const char *fmt, va_list ap) {
    size_t fmt_len, free_space;
    char *ptr;

    fmt_len = strlen(fmt);
    if (fmt_len == 0) {
        /* If there is no free space in the buffer after its content and if
         * the format string is empty, the pointer to this free space will be
         * invalid. We may as well return right now. */
        c_set_error("empty format string");
        return -1;
    }

    /* We need to make space for \0 because vsnprintf() needs it, even
     * though we will ignore it. */
    c_buffer_ensure_free_space(buf, fmt_len + 1);

    for (;;) {
        int ret;
        va_list local_ap;

        ptr = buf->data + buf->skip + buf->len;
        free_space = c_buffer_free_space(buf);

        va_copy(local_ap, ap);
        ret = vsnprintf(ptr, free_space, fmt, local_ap);
        va_end(local_ap);

        if (ret == -1) {
            c_set_error("cannot format string: %s", strerror(errno));
            return -1;
        }

        if ((size_t)ret < free_space) {
            buf->len += (size_t)ret;
            return 0;
        }

        c_buffer_ensure_free_space(buf, (size_t)ret + 1);
    }
}

int
c_buffer_add_printf(struct c_buffer *buf, const char *fmt, ...) {
    va_list ap;
    int ret;

    va_start(ap, fmt);
    ret = c_buffer_add_vprintf(buf, fmt, ap);
    va_end(ap);

    return ret;
}

void
c_buffer_skip(struct c_buffer *buf, size_t n) {
    if (n > buf->len)
        n = buf->len;

    buf->len -= n;
    buf->skip += n;

    if (buf->len == 0)
        buf->skip = 0;
}

size_t
c_buffer_remove_before(struct c_buffer *buf, size_t offset, size_t n) {
    if (offset > buf->len)
        offset = buf->len;

    if (n > offset)
        n = offset;

    if (n == 0)
        return 0;

    if (offset < buf->len) {
        char *ptr;

        ptr = buf->data + buf->skip + offset;
        memmove(ptr - n, ptr, buf->len - offset);
    }

    buf->len -= n;

    if (buf->len == 0)
        buf->skip = 0;

    return n;
}

size_t
c_buffer_remove_after(struct c_buffer *buf, size_t offset, size_t n) {
    char *ptr;

    if (offset > buf->len)
        offset = buf->len;

    if (n > buf->len - offset)
        n = buf->len - offset;

    if (n == 0)
        return 0;

    ptr = buf->data + buf->skip + offset;
    memmove(ptr, ptr + n, buf->len - offset - n);

    buf->len -= n;

    if (buf->len == 0)
        buf->skip = 0;

    return n;
}

size_t
c_buffer_remove(struct c_buffer *buf, size_t n) {
    return c_buffer_remove_before(buf, buf->len, n);
}

void *
c_buffer_extract(struct c_buffer *buf, size_t *plen) {
    void *data;

    if (!buf->data || buf->len == 0) {
        c_set_error("cannot extract content from an empty buffer");
        return NULL;
    }

    c_buffer_repack(buf);

    data = c_realloc(buf->data, buf->len);
    if (!data)
        return NULL;

    if (plen)
        *plen = buf->len;

    buf->data = NULL;
    buf->sz = 0;
    buf->len = 0;

    return data;
}

char *
c_buffer_extract_string(struct c_buffer *buf, size_t *plen) {
    char *string;
    size_t len;

    c_buffer_add(buf, "\0", 1);

    string = c_buffer_extract(buf, &len);
    if (!string) {
        c_buffer_remove(buf, 1);
        return NULL;
    }

    if (plen)
        *plen = len - 1;
    return string;
}

void *
c_buffer_dup(const struct c_buffer *buf) {
    char *tmp;

    if (!buf->data || buf->len == 0) {
        c_set_error("cannot duplicate an empty buffer");
        return NULL;
    }

    tmp = c_malloc(buf->len);
    if (!tmp)
        return NULL;

    memcpy(tmp, buf->data + buf->skip, buf->len);
    return tmp;
}

char *
c_buffer_dup_string(const struct c_buffer *buf) {
    char *str;

    str = c_malloc(buf->len + 1);
    if (!str)
        return NULL;

    if (buf->data)
        memcpy(str, buf->data + buf->skip, buf->len);
    str[buf->len] = '\0';

    return str;
}

ssize_t
c_buffer_read(struct c_buffer *buf, int fd, size_t n) {
    ssize_t ret;
    char *ptr;

    ptr = c_buffer_reserve(buf, n);
    if (!ptr)
        return -1;

    ret = read(fd, ptr, n);
    if (ret == -1) {
        c_set_error("%s", strerror(errno));
        return -1;
    }

    buf->len += (size_t)ret;
    return ret;
}

ssize_t
c_buffer_write(struct c_buffer *buf, int fd) {
    ssize_t ret;

    ret = write(fd, buf->data + buf->skip, buf->len);
    if (ret == -1) {
        c_set_error("%s", strerror(errno));
        return -1;
    }

    buf->len -= (size_t)ret;
    buf->skip += (size_t)ret;

    if (buf->len == 0)
        buf->skip = 0;

    return ret;
}

static void
c_buffer_repack(struct c_buffer *buf) {
    if (buf->skip == 0)
        return;

    memmove(buf->data, buf->data + buf->skip, buf->len);
    buf->skip = 0;
}

static int
c_buffer_resize(struct c_buffer *buf, size_t sz) {
    char *ndata;

    if (buf->data) {
        ndata = c_realloc(buf->data, sz);
    } else {
        ndata = c_malloc(sz);
    }

    if (!ndata)
        return -1;

    buf->data = ndata;
    buf->sz = sz;
    return 0;
}

static int
c_buffer_grow(struct c_buffer *buf, size_t sz) {
    return c_buffer_resize(buf, buf->sz + sz);
}

static int
c_buffer_ensure_free_space(struct c_buffer *buf, size_t sz) {
    size_t free_space;

    free_space = c_buffer_free_space(buf);
    if (free_space < sz)
        return c_buffer_grow(buf, sz - free_space);

    return 0;
}
