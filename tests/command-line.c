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

#define C_TEST_CMDLINE_PARSE(cmdline_, nb_, ...)                        \
    do {                                                                \
        char *argv[] = {__VA_ARGS__};                                   \
        int argc, ret;                                                  \
                                                                        \
        argc = sizeof(argv) / sizeof(char *);                           \
                                                                        \
        ret = c_command_line_parse(cmdline_, argc, argv);               \
        if (ret == -1)                                                  \
            TEST_ABORT("cannot parse command line: %s", c_get_error()); \
                                                                        \
        TEST_INT_EQ(ret, nb_);                                          \
    } while (0)

TEST(option_flags) {
    struct c_command_line *cmdline;

    cmdline = c_command_line_new();
    c_command_line_add_flag(cmdline, "a", "opt-a", "argument a");
    c_command_line_add_flag(cmdline, NULL, "opt-b", "argument b");
    c_command_line_add_flag(cmdline, "c", NULL, "test c");

    C_TEST_CMDLINE_PARSE(cmdline, 1, "");
    TEST_BOOL_EQ(c_command_line_is_option_set(cmdline, "a"), false);
    TEST_BOOL_EQ(c_command_line_is_option_set(cmdline, "opt-a"), false);
    TEST_BOOL_EQ(c_command_line_is_option_set(cmdline, "opt-b"), false);
    TEST_BOOL_EQ(c_command_line_is_option_set(cmdline, "c"), false);

    C_TEST_CMDLINE_PARSE(cmdline, 1, "test");
    TEST_BOOL_EQ(c_command_line_is_option_set(cmdline, "a"), false);
    TEST_BOOL_EQ(c_command_line_is_option_set(cmdline, "opt-a"), false);
    TEST_BOOL_EQ(c_command_line_is_option_set(cmdline, "opt-b"), false);
    TEST_BOOL_EQ(c_command_line_is_option_set(cmdline, "c"), false);
    TEST_BOOL_EQ(c_command_line_is_option_set(cmdline, "d"), false);
    TEST_BOOL_EQ(c_command_line_is_option_set(cmdline, "opt-d"), false);

    C_TEST_CMDLINE_PARSE(cmdline, 4, "test", "-a", "--opt-b", "-c");
    TEST_BOOL_EQ(c_command_line_is_option_set(cmdline, "a"), true);
    TEST_BOOL_EQ(c_command_line_is_option_set(cmdline, "opt-a"), true);
    TEST_BOOL_EQ(c_command_line_is_option_set(cmdline, "opt-b"), true);
    TEST_BOOL_EQ(c_command_line_is_option_set(cmdline, "c"), true);
    TEST_BOOL_EQ(c_command_line_is_option_set(cmdline, "d"), false);
    TEST_BOOL_EQ(c_command_line_is_option_set(cmdline, "opt-d"), false);

    C_TEST_CMDLINE_PARSE(cmdline, 3, "test", "-ac", "--opt-b");
    TEST_BOOL_EQ(c_command_line_is_option_set(cmdline, "a"), true);
    TEST_BOOL_EQ(c_command_line_is_option_set(cmdline, "opt-a"), true);
    TEST_BOOL_EQ(c_command_line_is_option_set(cmdline, "opt-b"), true);
    TEST_BOOL_EQ(c_command_line_is_option_set(cmdline, "c"), true);
    TEST_BOOL_EQ(c_command_line_is_option_set(cmdline, "d"), false);
    TEST_BOOL_EQ(c_command_line_is_option_set(cmdline, "opt-d"), false);

    C_TEST_CMDLINE_PARSE(cmdline, 2, "test", "-a", "foo", "-c");
    TEST_BOOL_EQ(c_command_line_is_option_set(cmdline, "a"), true);
    TEST_BOOL_EQ(c_command_line_is_option_set(cmdline, "c"), false);

    C_TEST_CMDLINE_PARSE(cmdline, 4, "test", "-a", "-c", "--");
    TEST_BOOL_EQ(c_command_line_is_option_set(cmdline, "a"), true);
    TEST_BOOL_EQ(c_command_line_is_option_set(cmdline, "c"), true);

    C_TEST_CMDLINE_PARSE(cmdline, 2, "test", "--", "-a", "-c");
    TEST_BOOL_EQ(c_command_line_is_option_set(cmdline, "a"), false);
    TEST_BOOL_EQ(c_command_line_is_option_set(cmdline, "c"), false);

    c_command_line_delete(cmdline);
}

TEST(option_values) {
    struct c_command_line *cmdline;

    cmdline = c_command_line_new();

    c_command_line_add_option(cmdline, "a", "opt-a", "argument a",
                                     "A value", "A");
    c_command_line_add_option(cmdline, NULL, "opt-b", "argument b",
                                     "B value", "B");
    c_command_line_add_flag(cmdline, "c", NULL, "argument c");

    C_TEST_CMDLINE_PARSE(cmdline, 1, "test");
    TEST_BOOL_EQ(c_command_line_is_option_set(cmdline, "a"), false);
    TEST_STRING_EQ(c_command_line_option_value(cmdline, "a"), "A");
    TEST_BOOL_EQ(c_command_line_is_option_set(cmdline, "opt-b"), false);
    TEST_STRING_EQ(c_command_line_option_value(cmdline, "opt-b"), "B");
    TEST_BOOL_EQ(c_command_line_is_option_set(cmdline, "c"), false);
    TEST_PTR_NULL(c_command_line_option_value(cmdline, "c"));

    C_TEST_CMDLINE_PARSE(cmdline, 5, "test", "-a", "1", "--opt-b", "2");
    TEST_BOOL_EQ(c_command_line_is_option_set(cmdline, "a"), true);
    TEST_STRING_EQ(c_command_line_option_value(cmdline, "a"), "1");
    TEST_BOOL_EQ(c_command_line_is_option_set(cmdline, "opt-b"), true);
    TEST_STRING_EQ(c_command_line_option_value(cmdline, "opt-b"), "2");
    TEST_BOOL_EQ(c_command_line_is_option_set(cmdline, "c"), false);
    TEST_PTR_NULL(c_command_line_option_value(cmdline, "c"));

    C_TEST_CMDLINE_PARSE(cmdline, 5, "test", "-ca", "1", "--opt-b", "2");
    TEST_BOOL_EQ(c_command_line_is_option_set(cmdline, "a"), true);
    TEST_STRING_EQ(c_command_line_option_value(cmdline, "a"), "1");
    TEST_BOOL_EQ(c_command_line_is_option_set(cmdline, "opt-b"), true);
    TEST_STRING_EQ(c_command_line_option_value(cmdline, "opt-b"), "2");
    TEST_BOOL_EQ(c_command_line_is_option_set(cmdline, "c"), true);
    TEST_PTR_NULL(c_command_line_option_value(cmdline, "c"));

    C_TEST_CMDLINE_PARSE(cmdline, 5, "test", "-a", "-1", "--opt-b", "--opt-b");
    TEST_BOOL_EQ(c_command_line_is_option_set(cmdline, "a"), true);
    TEST_STRING_EQ(c_command_line_option_value(cmdline, "a"), "-1");
    TEST_BOOL_EQ(c_command_line_is_option_set(cmdline, "opt-b"), true);
    TEST_STRING_EQ(c_command_line_option_value(cmdline, "opt-b"), "--opt-b");

    C_TEST_CMDLINE_PARSE(cmdline, 3, "test", "-a", "-1", "-", "--opt-b", "2");
    TEST_BOOL_EQ(c_command_line_is_option_set(cmdline, "a"), true);
    TEST_STRING_EQ(c_command_line_option_value(cmdline, "a"), "-1");
    TEST_BOOL_EQ(c_command_line_is_option_set(cmdline, "opt-b"), false);
    TEST_STRING_EQ(c_command_line_option_value(cmdline, "opt-b"), "B");

    C_TEST_CMDLINE_PARSE(cmdline, 2, "test", "--", "-a", "1", "--opt-b", "2");
    TEST_BOOL_EQ(c_command_line_is_option_set(cmdline, "a"), false);
    TEST_STRING_EQ(c_command_line_option_value(cmdline, "opt-a"), "A");
    TEST_BOOL_EQ(c_command_line_is_option_set(cmdline, "opt-b"), false);
    TEST_STRING_EQ(c_command_line_option_value(cmdline, "opt-b"), "B");

    C_TEST_CMDLINE_PARSE(cmdline, 3, "test", "-a", "1", "foo", "--opt-b", "2");
    TEST_BOOL_EQ(c_command_line_is_option_set(cmdline, "a"), true);
    TEST_STRING_EQ(c_command_line_option_value(cmdline, "a"), "1");
    TEST_BOOL_EQ(c_command_line_is_option_set(cmdline, "opt-b"), false);
    TEST_STRING_EQ(c_command_line_option_value(cmdline, "opt-b"), "B");

    c_command_line_delete(cmdline);
}

TEST(invalid_options) {
    struct c_command_line *cmdline;

    cmdline = c_command_line_new();
    c_command_line_add_option(cmdline, "a", "opt-a", "argument a",
                                     "A value", "A");
    c_command_line_add_option(cmdline, NULL, "opt-b", "argument b",
                                     "B value", "b");
    c_command_line_add_flag(cmdline, "c", NULL, "argument c");

#define C_TEST_CMDLINE_INVALID(cmdline_, ...)                \
    do {                                                     \
        char *argv[] = {__VA_ARGS__};                        \
        int argc;                                            \
                                                             \
        argc = sizeof(argv) / sizeof(char *);                \
                                                             \
        if (c_command_line_parse(cmdline_, argc, argv) == 0) \
            TEST_ABORT("parsed invalid command line");       \
    } while (0)

    C_TEST_CMDLINE_INVALID(cmdline, "test", "b");
    C_TEST_CMDLINE_INVALID(cmdline, "test", "--opt-c");
    C_TEST_CMDLINE_INVALID(cmdline, "test", "-c", "1");
    C_TEST_CMDLINE_INVALID(cmdline, "test", "-ac", "1");
    C_TEST_CMDLINE_INVALID(cmdline, "test", "-ca");
    C_TEST_CMDLINE_INVALID(cmdline, "test", "-a");
    C_TEST_CMDLINE_INVALID(cmdline, "test", "--opt-a");

    c_command_line_delete(cmdline);
}

TEST(arguments) {
    struct c_command_line *cmdline;

    cmdline = c_command_line_new();
    c_command_line_add_argument(cmdline, "first argument", "arg-1");
    c_command_line_add_argument(cmdline, "second argument", "arg-2");

    C_TEST_CMDLINE_PARSE(cmdline, 3, "test", "a", "b");
    TEST_STRING_EQ(c_command_line_argument_value(cmdline, 0), "a");
    TEST_STRING_EQ(c_command_line_argument_value(cmdline, 1), "b");

    C_TEST_CMDLINE_PARSE(cmdline, 3, "test", "a", "-");
    TEST_STRING_EQ(c_command_line_argument_value(cmdline, 0), "a");
    TEST_STRING_EQ(c_command_line_argument_value(cmdline, 1), "-");

    C_TEST_CMDLINE_PARSE(cmdline, 3, "test", "a", "--");
    TEST_STRING_EQ(c_command_line_argument_value(cmdline, 0), "a");
    TEST_STRING_EQ(c_command_line_argument_value(cmdline, 1), "--");

    c_command_line_delete(cmdline);


    cmdline = c_command_line_new();
    c_command_line_add_argument(cmdline, "first argument", "arg-1");
    c_command_line_add_argument(cmdline, "second argument", "arg-2");
    c_command_line_add_trailing_arguments(cmdline, "trailing arguments", "arg");

    C_TEST_CMDLINE_PARSE(cmdline, 4, "test", "a", "b", "c");
    TEST_STRING_EQ(c_command_line_argument_value(cmdline, 0), "a");
    TEST_STRING_EQ(c_command_line_argument_value(cmdline, 1), "b");
    TEST_PTR_NULL(c_command_line_argument_value(cmdline, 2));
    TEST_UINT_EQ(c_command_line_nb_trailing_arguments(cmdline), 1);
    TEST_STRING_EQ(c_command_line_trailing_argument_values(cmdline)[0], "c");

    C_TEST_CMDLINE_PARSE(cmdline, 6, "test", "a", "b", "x", "y", "z");
    TEST_STRING_EQ(c_command_line_argument_value(cmdline, 0), "a");
    TEST_STRING_EQ(c_command_line_argument_value(cmdline, 1), "b");
    TEST_PTR_NULL(c_command_line_argument_value(cmdline, 2));
    TEST_UINT_EQ(c_command_line_nb_trailing_arguments(cmdline), 3);
    TEST_STRING_EQ(c_command_line_trailing_argument_values(cmdline)[0], "x");
    TEST_STRING_EQ(c_command_line_trailing_argument_values(cmdline)[1], "y");
    TEST_STRING_EQ(c_command_line_trailing_argument_values(cmdline)[2], "z");

    c_command_line_delete(cmdline);
}

TEST(invalid_arguments) {
    struct c_command_line *cmdline;

    cmdline = c_command_line_new();
    c_command_line_add_argument(cmdline, "first argument", "arg-1");
    c_command_line_add_argument(cmdline, "second argument", "arg-2");
    c_command_line_add_trailing_arguments(cmdline, "trailing arguments", "arg");

    C_TEST_CMDLINE_INVALID(cmdline, "");
    C_TEST_CMDLINE_INVALID(cmdline, "test");
    C_TEST_CMDLINE_INVALID(cmdline, "test", "a");
    C_TEST_CMDLINE_INVALID(cmdline, "test", "a", "b");

    c_command_line_delete(cmdline);
}

int
main(int argc, char **argv) {
    struct test_suite *suite;

    suite = test_suite_new("command-line");
    test_suite_initialize_from_args(suite, argc, argv);

    test_suite_start(suite);

    TEST_RUN(suite, option_flags);
    TEST_RUN(suite, option_values);
    TEST_RUN(suite, invalid_options);
    TEST_RUN(suite, arguments);
    TEST_RUN(suite, invalid_arguments);

    test_suite_print_results_and_exit(suite);
}
