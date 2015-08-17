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

#include <stddef.h>

#include <utest.h>

#include "../src/internal.h"

TEST(strndup) {
#define C_TEST_STRNDUP(string_, len_, expected_string_) \
    do {                                                \
        char *string;                                   \
                                                        \
        string = c_strndup(string_, len_);              \
        TEST_STRING_EQ(string, expected_string_);       \
        c_free(string);                                 \
    } while (0)

    C_TEST_STRNDUP("", 0, "");
    C_TEST_STRNDUP("", 3, "");

    C_TEST_STRNDUP("a", 0, "");
    C_TEST_STRNDUP("a", 1, "a");
    C_TEST_STRNDUP("a", 3, "a");

    C_TEST_STRNDUP("foo", 0, "");
    C_TEST_STRNDUP("foo", 2, "fo");
    C_TEST_STRNDUP("foo", 3, "foo");
    C_TEST_STRNDUP("foo", 5, "foo");

#undef C_TEST_STRNDUP
}

TEST(asprintf) {
#define C_TEST_ASPRINTF(expected_string_, expected_length_, fmt_, ...) \
    do {                                                               \
        char *string;                                                  \
        int ret;                                                       \
                                                                       \
        ret = c_asprintf(&string, fmt_, ##__VA_ARGS__);                \
        TEST_INT_EQ(ret, expected_length_);                            \
        TEST_STRING_EQ(string, expected_string_);                      \
        c_free(string);                                                \
    } while (0)

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-zero-length"
    C_TEST_ASPRINTF("", 0, "");
#pragma GCC diagnostic pop

    C_TEST_ASPRINTF("a", 1, "a");
    C_TEST_ASPRINTF("foo", 3, "foo");
    C_TEST_ASPRINTF("1", 1, "%d", 1);
    C_TEST_ASPRINTF("42", 2, "%d", 42);
    C_TEST_ASPRINTF("9999", 4, "%d", 9999);
    C_TEST_ASPRINTF("a", 1, "%c", 'a');
    C_TEST_ASPRINTF("", 1, "%c", 0);

#undef C_TEST_ASPRINTF
}

TEST(string_search) {
#define C_TEST_STRING_SEARCH(string_, needle_, expected_offset_) \
    do {                                                         \
        const char *ptr;                                         \
        ptrdiff_t offset;                                        \
                                                                 \
        ptr = c_string_search(string_, needle_);                 \
        if (!ptr) {                                              \
            TEST_ABORT("string \"%s\" not found in \"%s\"",      \
                       string_, needle_);                        \
        }                                                        \
                                                                 \
        offset = ptr - string_;                                  \
        TEST_INT_EQ(offset, expected_offset_);                   \
    } while (0)

    C_TEST_STRING_SEARCH("", "", 0);
    C_TEST_STRING_SEARCH("abc", "a", 0);
    C_TEST_STRING_SEARCH("abc", "b", 1);
    C_TEST_STRING_SEARCH("abc", "bc", 1);
    C_TEST_STRING_SEARCH("abcabc", "abc", 0);
    C_TEST_STRING_SEARCH("fooabc", "abc", 3);
    C_TEST_STRING_SEARCH("abcabcd", "abcd", 3);
    C_TEST_STRING_SEARCH("abcabcd", "d", 6);

#undef C_TEST_STRING_SEARCH


#define C_TEST_STRING_SEARCH_NOT_FOUND(string_, needle_)         \
    do {                                                         \
        const char *ptr;                                         \
                                                                 \
        ptr = c_string_search(string_, needle_);                 \
        if (ptr) {                                               \
            TEST_ABORT("string \"%s\" found in \"%s\"",          \
                       string_, needle_);                        \
        }                                                        \
    } while (0)

    C_TEST_STRING_SEARCH_NOT_FOUND("", "a");
    C_TEST_STRING_SEARCH_NOT_FOUND("abc", "abcd");

#undef C_TEST_STRING_SEARCH_NOT_FOUND
}

TEST(string_starts_with) {
    TEST_TRUE(c_string_starts_with("", ""));
    TEST_TRUE(c_string_starts_with("foo", "foo"));
    TEST_TRUE(c_string_starts_with("foo", ""));
    TEST_TRUE(c_string_starts_with("foo", "fo"));

    TEST_FALSE(c_string_starts_with("", "foo"));
    TEST_FALSE(c_string_starts_with("foo", "fooo"));
    TEST_FALSE(c_string_starts_with("foo", "foo bar"));
    TEST_FALSE(c_string_starts_with("foo", "abc"));
    TEST_FALSE(c_string_starts_with("foo", "a"));
}

TEST(memspn) {
#define C_TEST_MEMSPN(data_, sz_, chars_, ret_) \
    do {                                        \
        size_t ret;                             \
                                                \
        ret = c_memspn(data_, sz_, chars_);     \
        TEST_UINT_EQ(ret, ret_);                \
    } while (0)

    C_TEST_MEMSPN("", 0, "", 0);
    C_TEST_MEMSPN("foo", 3, "", 0);
    C_TEST_MEMSPN("", 0, "abc", 0);
    C_TEST_MEMSPN("abc", 3, "a", 1);
    C_TEST_MEMSPN("aaaaaa", 3, "a", 3);
    C_TEST_MEMSPN("abc", 3, "ab", 2);
    C_TEST_MEMSPN("ababab", 3, "ab", 3);
    C_TEST_MEMSPN("abbcd", 5, "abc", 4);
    C_TEST_MEMSPN("abbca", 5, "abc", 5);
    C_TEST_MEMSPN("defdef", 6, "abc", 0);
    C_TEST_MEMSPN("defabc", 3, "abc", 0);

#undef C_TEST_MEMSPN
}

TEST(memcspn) {
#define C_TEST_MEMCSPN(data_, sz_, chars_, ret_) \
    do {                                         \
        size_t ret;                              \
                                                 \
        ret = c_memcspn(data_, sz_, chars_);     \
        TEST_UINT_EQ(ret, ret_);                 \
    } while (0)

    C_TEST_MEMCSPN("", 0, "", 0);
    C_TEST_MEMCSPN("foo", 3, "", 3);
    C_TEST_MEMCSPN("foo", 3, "abc", 3);
    C_TEST_MEMCSPN("abccd", 5, "abc", 0);
    C_TEST_MEMCSPN("dbcca", 5, "abc", 1);
    C_TEST_MEMCSPN("defdef", 6, "abc", 6);
    C_TEST_MEMCSPN("defdef", 3, "abc", 3);

#undef C_TEST_MEMCSPN
}

int
main(int argc, char **argv) {
    struct test_suite *suite;

    suite = test_suite_new("strings");
    test_suite_initialize_from_args(suite, argc, argv);

    test_suite_start(suite);

    TEST_RUN(suite, strndup);
    TEST_RUN(suite, asprintf);
    TEST_RUN(suite, string_search);
    TEST_RUN(suite, string_starts_with);
    TEST_RUN(suite, memspn);
    TEST_RUN(suite, memcspn);

    test_suite_print_results_and_exit(suite);
}
