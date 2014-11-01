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

TEST(codepoint_read) {
#define C_TEST_CODEPOINT_EQ(bytes_, codepoint_, length_)                 \
    do {                                                                 \
        uint32_t codepoint;                                              \
        size_t length;                                                   \
                                                                         \
        if (c_utf8_read_codepoint(bytes_, &codepoint, &length) == -1)    \
            TEST_ABORT("cannot read utf-8 sequence: %s", c_get_error()); \
                                                                         \
        TEST_UINT_EQ(codepoint, codepoint_);                             \
        TEST_UINT_EQ(length, length_);                                   \
    } while (0)

    C_TEST_CODEPOINT_EQ("\x61", 0x61, 1);
    C_TEST_CODEPOINT_EQ("\x00", 0x00, 1);
    C_TEST_CODEPOINT_EQ("\x7f", 0x7f, 1);

    C_TEST_CODEPOINT_EQ("\xc3\xa9", 0x0e9, 2);
    C_TEST_CODEPOINT_EQ("\xc2\x80", 0x080, 2);
    C_TEST_CODEPOINT_EQ("\xdf\xbf", 0x7ff, 2);

    C_TEST_CODEPOINT_EQ("\xe2\x82\xac", 0x20ac, 3);
    C_TEST_CODEPOINT_EQ("\xe0\xa0\x80", 0x0800, 3);
    C_TEST_CODEPOINT_EQ("\xef\xbf\xbf", 0xffff, 3);

    C_TEST_CODEPOINT_EQ("\xf0\x9b\x80\x80", 0x01b000, 4);
    C_TEST_CODEPOINT_EQ("\xf0\x90\x80\x80", 0x010000, 4);
    C_TEST_CODEPOINT_EQ("\xf4\x8f\xbf\xbf", 0x10ffff, 4);

#undef C_TEST_CODEPOINT_EQ


#define C_TEST_INVALID_CODEPOINT(bytes_)                             \
    do {                                                             \
        uint32_t codepoint;                                          \
        size_t length;                                               \
                                                                     \
        if (c_utf8_read_codepoint(bytes_, &codepoint, &length) == 0) \
            TEST_ABORT("read invalid codepoint");                    \
    } while (0)

    /* Overlong encodings */
    C_TEST_INVALID_CODEPOINT("\xc0\xaf");
    C_TEST_INVALID_CODEPOINT("\xe0\x9f\x80");
    C_TEST_INVALID_CODEPOINT("\xf4\x90\x83\x92");

    /* Truncated sequences */
    C_TEST_INVALID_CODEPOINT("\xc3");

    C_TEST_INVALID_CODEPOINT("\xe2");
    C_TEST_INVALID_CODEPOINT("\xe2\x82");

    C_TEST_INVALID_CODEPOINT("\xf0");
    C_TEST_INVALID_CODEPOINT("\xf0\x9b");
    C_TEST_INVALID_CODEPOINT("\xf0\x9b\x80");

    /* Invalid codepoints */
    C_TEST_INVALID_CODEPOINT("\xf7\xbf\xbf\xb");
    C_TEST_INVALID_CODEPOINT("\xed\xa0\x80");

#undef C_TEST_INVALID_CODEPOINT
}

TEST(utf8_decode) {
    uint32_t *ustring;

#define C_TEST_UTF8_DECODE(string_, ...)                                 \
    do {                                                                 \
        uint32_t expected_ustring[] = { __VA_ARGS__ };                   \
                                                                         \
        ustring = c_utf8_decode(string_);                                \
        if (!ustring)                                                    \
            TEST_ABORT("cannot decode utf8 string: %s", c_get_error());  \
                                                                         \
        TEST_UINT_EQ(c_ustring_length(ustring),                          \
                     c_ustring_length(expected_ustring));                \
        TEST_TRUE(c_ustring_equal(ustring, expected_ustring));           \
    } while (0)

    C_TEST_UTF8_DECODE("", 0x0);
    C_TEST_UTF8_DECODE("\x61", 0x61, 0x0);
    C_TEST_UTF8_DECODE("\x66\x6f\x6f", 0x66, 0x6f, 0x6f, 0x0);
    C_TEST_UTF8_DECODE("\xc3\xa9", 0xe9, 0x0);
    C_TEST_UTF8_DECODE("\xe2\x82\xac", 0x20ac, 0x0);
    C_TEST_UTF8_DECODE("\xe1\x83\x90\xe1\x83\xae\xe1\x83\x9a",
                       0x10d0, 0x10ee, 0x10da, 0x0);

#undef C_TEST_UTF8_DECODE

#define C_TEST_INVALID_UTF8(string_)                                     \
    do {                                                                 \
        ustring = c_utf8_decode(string_);                                \
        if (ustring)                                                     \
            TEST_ABORT("decoded invalid utf8 string");                   \
    } while (0)

    /* Invalid bytes */
    C_TEST_INVALID_UTF8("\x80");
    C_TEST_INVALID_UTF8("\x61\xff");

    /* Truncated sequences */
    C_TEST_INVALID_UTF8("\xe2");
    C_TEST_INVALID_UTF8("\x61\xe2");
    C_TEST_INVALID_UTF8("\x61\xe2\x82");

    /* Invalid sequences */
    C_TEST_INVALID_UTF8("\xe0\x90\x80");
    C_TEST_INVALID_UTF8("\xe1\x70\x80");
    C_TEST_INVALID_UTF8("\x61\xe0\x1f\x70");
    C_TEST_INVALID_UTF8("\x61\xe0\xa0\xd0");
    C_TEST_INVALID_UTF8("\xf1\xe0\x80\x80");

#undef C_TEST_INVALID_UTF8
}

int
main(int argc, char **argv) {
    struct test_suite *suite;

    suite = test_suite_new("unicode");
    test_suite_initialize_from_args(suite, argc, argv);

    test_suite_start(suite);

    TEST_RUN(suite, codepoint_read);
    TEST_RUN(suite, utf8_decode);

    test_suite_print_results_and_exit(suite);
}
