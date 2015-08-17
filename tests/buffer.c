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

#include <utest.h>

#include "../src/internal.h"

#define C_TEST_BUFFER_EMPTY(buf_)           \
    TEST_UINT_EQ(c_buffer_length(buf_), 0)

#define C_TEST_BUFFER_EQ(buf_, data_, sz_)                      \
    do {                                                        \
        TEST_UINT_EQ(c_buffer_length(buf_), sz_);               \
        TEST_MEM_EQ(c_buffer_data(buf_), c_buffer_length(buf_), \
                    data_, sz_);                                \
    } while (0)

TEST(initialization) {
    struct c_buffer *buf;

    buf = c_buffer_new();
    C_TEST_BUFFER_EMPTY(buf);
    c_buffer_delete(buf);
}

TEST(insert) {
    struct c_buffer *buf;

    buf = c_buffer_new();

    c_buffer_insert(buf, 0, "abc", 3);
    C_TEST_BUFFER_EQ(buf, "abc", 3);

    c_buffer_insert(buf, 2, "123", 3);
    C_TEST_BUFFER_EQ(buf, "ab123c", 6);

    c_buffer_insert(buf, 6, "de", 2);
    C_TEST_BUFFER_EQ(buf, "ab123cde", 8);

    c_buffer_delete(buf);
}

TEST(add) {
    struct c_buffer *buf;

    buf = c_buffer_new();

    c_buffer_add_string(buf, "abc");
    C_TEST_BUFFER_EQ(buf, "abc", 3);

    c_buffer_add_string(buf, "defgh");
    C_TEST_BUFFER_EQ(buf, "abcdefgh", 8);

    c_buffer_clear(buf);
    C_TEST_BUFFER_EMPTY(buf);

    c_buffer_add_printf(buf, "hello: %d", 42);
    C_TEST_BUFFER_EQ(buf, "hello: 42", 9);

    c_buffer_delete(buf);
}

TEST(skip) {
    struct c_buffer *buf;

    buf = c_buffer_new();

    c_buffer_skip(buf, 3);
    C_TEST_BUFFER_EMPTY(buf);

    c_buffer_add_string(buf, "abcde");
    c_buffer_skip(buf, 2);
    C_TEST_BUFFER_EQ(buf, "cde", 3);

    c_buffer_skip(buf, 3);
    C_TEST_BUFFER_EMPTY(buf);

    c_buffer_add_string(buf, "fgh");
    c_buffer_skip(buf, 6);
    C_TEST_BUFFER_EMPTY(buf);

    c_buffer_delete(buf);
}

TEST(remove) {
    struct c_buffer *buf;

    buf = c_buffer_new();

    TEST_UINT_EQ(c_buffer_remove(buf, 2), 0);
    C_TEST_BUFFER_EMPTY(buf);

    c_buffer_add_string(buf, "abcde");
    TEST_UINT_EQ(c_buffer_remove(buf, 2), 2);
    C_TEST_BUFFER_EQ(buf, "abc", 3);
    TEST_UINT_EQ(c_buffer_remove(buf, 5), 3);
    C_TEST_BUFFER_EMPTY(buf);

    c_buffer_add_string(buf, "abcde");
    TEST_UINT_EQ(c_buffer_remove_before(buf, 3, 2), 2);
    C_TEST_BUFFER_EQ(buf, "ade", 3);
    TEST_UINT_EQ(c_buffer_remove_before(buf, 1, 3), 1);
    C_TEST_BUFFER_EQ(buf, "de", 2);

    c_buffer_clear(buf);
    c_buffer_add_string(buf, "abcde");
    TEST_UINT_EQ(c_buffer_remove_after(buf, 0, 2), 2);
    C_TEST_BUFFER_EQ(buf, "cde", 3);

    c_buffer_clear(buf);
    c_buffer_add_string(buf, "abcde");
    TEST_UINT_EQ(c_buffer_remove_after(buf, 1, 2), 2);
    C_TEST_BUFFER_EQ(buf, "ade", 3);

    c_buffer_clear(buf);
    c_buffer_add_string(buf, "abcde");
    TEST_UINT_EQ(c_buffer_remove_after(buf, 4, 2), 1);
    C_TEST_BUFFER_EQ(buf, "abcd", 4);

    c_buffer_delete(buf);
}

TEST(dup) {
    struct c_buffer *buf;
    char *tmp;

    buf = c_buffer_new();

    TEST_PTR_NULL(c_buffer_dup(buf));

    c_buffer_add_string(buf, "abcde");
    tmp = c_buffer_dup(buf);
    TEST_MEM_EQ(tmp, 5, "abcde", 5);
    free(tmp);

    tmp = c_buffer_dup_string(buf);
    TEST_MEM_EQ(tmp, 6, "abcde\0", 6);
    free(tmp);

    c_buffer_clear(buf);
    tmp = c_buffer_dup_string(buf);
    TEST_MEM_EQ(tmp, 1, "\0", 1);
    free(tmp);

    c_buffer_delete(buf);
}

TEST(free_space_after_skip) {
    struct c_buffer *buf;

    buf = c_buffer_new();

    /* Make sure that the skip counter is reset when skipping/removing the
     * last byte of content of the buffer */

    c_buffer_add_string(buf, "abcdefgh");
    TEST_UINT_EQ(c_buffer_free_space(buf), C_BUFFER_MIN_SIZE - 8);
    c_buffer_clear(buf);
    TEST_UINT_EQ(c_buffer_free_space(buf), C_BUFFER_MIN_SIZE);

    c_buffer_add_string(buf, "hello");
    TEST_UINT_EQ(c_buffer_free_space(buf), C_BUFFER_MIN_SIZE - 5);
    c_buffer_skip(buf, 5);
    TEST_UINT_EQ(c_buffer_free_space(buf), C_BUFFER_MIN_SIZE);

    c_buffer_add_string(buf, "hello");
    TEST_UINT_EQ(c_buffer_free_space(buf), C_BUFFER_MIN_SIZE - 5);
    c_buffer_skip(buf, 3);
    TEST_UINT_EQ(c_buffer_free_space(buf), C_BUFFER_MIN_SIZE - 5);
    c_buffer_skip(buf, 2);
    TEST_UINT_EQ(c_buffer_free_space(buf), C_BUFFER_MIN_SIZE);

    c_buffer_add_string(buf, "hello");
    TEST_UINT_EQ(c_buffer_free_space(buf), C_BUFFER_MIN_SIZE - 5);
    c_buffer_skip(buf, 2);
    TEST_UINT_EQ(c_buffer_free_space(buf), C_BUFFER_MIN_SIZE - 5);
    c_buffer_remove_before(buf, 3, 3);
    TEST_UINT_EQ(c_buffer_free_space(buf), C_BUFFER_MIN_SIZE);

    c_buffer_add_string(buf, "hello");
    TEST_UINT_EQ(c_buffer_free_space(buf), C_BUFFER_MIN_SIZE - 5);
    c_buffer_skip(buf, 2);
    TEST_UINT_EQ(c_buffer_free_space(buf), C_BUFFER_MIN_SIZE - 5);
    c_buffer_remove_after(buf, 0, 3);
    TEST_UINT_EQ(c_buffer_free_space(buf), C_BUFFER_MIN_SIZE);
}

int
main(int argc, char **argv) {
    struct test_suite *suite;

    suite = test_suite_new("buffer");
    test_suite_initialize_from_args(suite, argc, argv);

    test_suite_start(suite);

    TEST_RUN(suite, initialization);
    TEST_RUN(suite, insert);
    TEST_RUN(suite, add);
    TEST_RUN(suite, remove);
    TEST_RUN(suite, dup);
    TEST_RUN(suite, free_space_after_skip);

    test_suite_print_results_and_exit(suite);
}
