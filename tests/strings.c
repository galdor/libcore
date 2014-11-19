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

#include <utest.h>

#include "internal.h"

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

int
main(int argc, char **argv) {
    struct test_suite *suite;

    suite = test_suite_new("strings");
    test_suite_initialize_from_args(suite, argc, argv);

    test_suite_start(suite);

    TEST_RUN(suite, strndup);
    TEST_RUN(suite, asprintf);

    test_suite_print_results_and_exit(suite);
}
