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

#include <utest.h>

#include "internal.h"

TEST(parse_integer) {
#define C_TEST_INT(string_, min_, max_, expected_value_, expected_sz_) \
    do {                                                               \
        int64_t value;                                                 \
        size_t sz;                                                     \
                                                                       \
        if (c_parse_integer(string_, min_, max_, &value, &sz) == -1)   \
            TEST_ABORT("cannot parse integer: %s", c_get_error());     \
                                                                       \
        TEST_INT_EQ(value, expected_value_);                           \
        TEST_UINT_EQ(sz, expected_sz_);                                \
    } while (0)

    C_TEST_INT("0", INT64_MIN, INT64_MAX, 0, 1);
    C_TEST_INT("-0", INT64_MIN, INT64_MAX, 0, 2);
    C_TEST_INT("0foo", INT64_MIN, INT64_MAX, 0, 1);
    C_TEST_INT("-1", INT64_MIN, INT64_MAX, -1, 2);
    C_TEST_INT("42", INT64_MIN, INT64_MAX, 42, 2);
    C_TEST_INT("9223372036854775807", INT64_MIN, INT64_MAX, INT64_MAX, 19);
    C_TEST_INT("-9223372036854775808", INT64_MIN, INT64_MAX, INT64_MIN, 20);
    C_TEST_INT("9223372036854775807foo", INT64_MIN, INT64_MAX, INT64_MAX, 19);
    C_TEST_INT("-9223372036854775808foo", INT64_MIN, INT64_MAX, INT64_MIN, 20);

    C_TEST_INT("-10", -10, 20, -10, 3);
    C_TEST_INT("0", -10, 20, 0, 1);
    C_TEST_INT("20", -10, 20, 20, 2);

#undef C_TEST_INT


#define C_TEST_INVALID_INT(string_, min_, max_)                    \
    do {                                                           \
        if (c_parse_integer(string_, min_, max_, NULL, NULL) == 0) \
            TEST_ABORT("parsed invalid integer");                  \
    } while (0)

    C_TEST_INVALID_INT("", INT64_MIN, INT64_MAX);
    C_TEST_INVALID_INT("f", INT64_MIN, INT64_MAX);
    C_TEST_INVALID_INT("foo", INT64_MIN, INT64_MAX);
    C_TEST_INVALID_INT("9223372036854775808", INT64_MIN, INT64_MAX);
    C_TEST_INVALID_INT("-9223372036854775809", INT64_MIN, INT64_MAX);

    C_TEST_INVALID_INT("9", 10, 20);
    C_TEST_INVALID_INT("21", 10, 20);

#undef C_TEST_INVALID_INT
}

TEST(parse_unsigned_integer) {
#define C_TEST_UINT(string_, min_, max_, expected_value_, expected_sz_)       \
    do {                                                                      \
        uint64_t value;                                                       \
        size_t sz;                                                            \
                                                                              \
        if (c_parse_unsigned_integer(string_, min_, max_, &value, &sz) == -1) \
            TEST_ABORT("cannot parse unsigned integer: %s", c_get_error());   \
                                                                              \
        TEST_UINT_EQ(value, expected_value_);                                 \
        TEST_UINT_EQ(sz, expected_sz_);                                       \
    } while (0)

    C_TEST_UINT("0", 0, UINT64_MAX, 0, 1);
    C_TEST_UINT("-0", 0, UINT64_MAX, 0, 2);
    C_TEST_UINT("0foo", 0, UINT64_MAX, 0, 1);
    C_TEST_UINT("42", 0, UINT64_MAX, 42, 2);
    C_TEST_UINT("18446744073709551615", 0, UINT64_MAX, UINT64_MAX, 20);
    C_TEST_UINT("18446744073709551615foo", 0, UINT64_MAX, UINT64_MAX, 20);

    C_TEST_UINT("10", 10, 20, 10, 2);
    C_TEST_UINT("15", 10, 20, 15, 2);
    C_TEST_UINT("20", 10, 20, 20, 2);

#undef C_TEST_UINT


#define C_TEST_INVALID_UINT(string_, min_, max_)                            \
    do {                                                                    \
        if (c_parse_unsigned_integer(string_, min_, max_, NULL, NULL) == 0) \
            TEST_ABORT("parsed invalid unsigned integer");                  \
    } while (0)

    C_TEST_INVALID_UINT("", 0, UINT64_MAX);
    C_TEST_INVALID_UINT("f", 0, UINT64_MAX);
    C_TEST_INVALID_UINT("foo", 0, UINT64_MAX);
    C_TEST_INVALID_UINT("18446744073709551616", 0, UINT64_MAX);
    C_TEST_INVALID_UINT("-1", 0, UINT64_MAX);

    C_TEST_INVALID_UINT("9", 10, 20);
    C_TEST_INVALID_UINT("21", 10, 20);

#undef C_TEST_INVALID_UINT
}

int
main(int argc, char **argv) {
    struct test_suite *suite;

    suite = test_suite_new("numbers");
    test_suite_initialize_from_args(suite, argc, argv);

    test_suite_start(suite);

    TEST_RUN(suite, parse_integer);
    TEST_RUN(suite, parse_unsigned_integer);

    test_suite_print_results_and_exit(suite);
}
