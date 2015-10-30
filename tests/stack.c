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

#define C_TEST_STRING_STACK_EQ(stack_, ...)                           \
    do {                                                              \
        const char *entries[] = {__VA_ARGS__};                        \
        size_t nb_entries = sizeof(entries) / sizeof(const char *);   \
        struct c_stack_entry *entry;                                  \
        size_t idx;                                                   \
                                                                      \
        TEST_UINT_EQ(c_stack_length(stack_), nb_entries);             \
                                                                      \
        idx = 0;                                                      \
        entry = c_stack_top_entry(stack_);                            \
        while (entry) {                                               \
            TEST_STRING_EQ(c_stack_entry_value(entry), entries[idx]); \
                                                                      \
            idx++;                                                    \
            entry = c_stack_entry_next(entry);                        \
        }                                                             \
    } while (0)

TEST(main) {
    struct c_stack *stack;

    stack = c_stack_new();
    TEST_UINT_EQ(c_stack_length(stack), 0);
    c_stack_delete(stack);

    stack = c_stack_new();
    c_stack_push(stack, "1");
    C_TEST_STRING_STACK_EQ(stack, "1");
    c_stack_delete(stack);

    stack = c_stack_new();
    c_stack_push(stack, "2");
    c_stack_push(stack, "1");
    C_TEST_STRING_STACK_EQ(stack, "1", "2");
    c_stack_delete(stack);

    stack = c_stack_new();
    c_stack_push(stack, "3");
    c_stack_push(stack, "2");
    c_stack_push(stack, "1");
    C_TEST_STRING_STACK_EQ(stack, "1", "2", "3");
    c_stack_delete(stack);
}

TEST(clear) {
    struct c_stack *stack;

    stack = c_stack_new();
    c_stack_clear(stack);
    TEST_UINT_EQ(c_stack_length(stack), 0);
    c_stack_delete(stack);

    stack = c_stack_new();
    c_stack_push(stack, "3");
    c_stack_push(stack, "2");
    c_stack_push(stack, "1");
    c_stack_clear(stack);
    TEST_UINT_EQ(c_stack_length(stack), 0);
    c_stack_delete(stack);
}

TEST(peek_pop) {
    struct c_stack *stack;

    stack = c_stack_new();
    TEST_PTR_NULL(c_stack_peek(stack));
    TEST_PTR_NULL(c_stack_pop(stack));
    c_stack_delete(stack);

    stack = c_stack_new();
    c_stack_push(stack, "5");
    c_stack_push(stack, "4");
    c_stack_push(stack, "3");
    TEST_STRING_EQ(c_stack_peek(stack), "3");
    TEST_STRING_EQ(c_stack_pop(stack), "3");
    TEST_STRING_EQ(c_stack_peek(stack), "4");
    TEST_STRING_EQ(c_stack_pop(stack), "4");
    c_stack_push(stack, "2");
    TEST_STRING_EQ(c_stack_peek(stack), "2");
    TEST_STRING_EQ(c_stack_pop(stack), "2");
    TEST_STRING_EQ(c_stack_peek(stack), "5");
    TEST_STRING_EQ(c_stack_pop(stack), "5");
    c_stack_push(stack, "1");
    TEST_STRING_EQ(c_stack_peek(stack), "1");
    TEST_STRING_EQ(c_stack_pop(stack), "1");
    TEST_PTR_NULL(c_stack_peek(stack));
    TEST_PTR_NULL(c_stack_pop(stack));
    c_stack_delete(stack);
}

TEST(entries) {
    struct c_stack *stack;
    struct c_stack_entry *entry;

    stack = c_stack_new();
    c_stack_push(stack, "4");
    c_stack_push(stack, "3");
    c_stack_push(stack, "2");
    c_stack_push(stack, "1");
    C_TEST_STRING_STACK_EQ(stack, "1", "2", "3", "4");

    entry = c_stack_top_entry(stack);
    TEST_PTR_NOT_NULL(entry);
    TEST_STRING_EQ(c_stack_entry_value(entry), "1");

    entry = c_stack_bottom_entry(stack);
    TEST_PTR_NOT_NULL(entry);
    TEST_STRING_EQ(c_stack_entry_value(entry), "4");

    entry = c_stack_entry_next(c_stack_top_entry(stack));
    TEST_PTR_NOT_NULL(entry);
    TEST_STRING_EQ(c_stack_entry_value(entry), "2");
    c_stack_remove_entry(stack, entry);
    c_stack_entry_delete(entry);
    C_TEST_STRING_STACK_EQ(stack, "1", "3", "4");

    entry = c_stack_entry_prev(c_stack_bottom_entry(stack));
    TEST_PTR_NOT_NULL(entry);
    TEST_STRING_EQ(c_stack_entry_value(entry), "3");
    c_stack_remove_entry(stack, entry);
    c_stack_entry_delete(entry);
    C_TEST_STRING_STACK_EQ(stack, "1", "4");

    entry = c_stack_bottom_entry(stack);
    c_stack_remove_entry(stack, entry);
    c_stack_entry_delete(entry);
    C_TEST_STRING_STACK_EQ(stack, "1");

    entry = c_stack_top_entry(stack);
    c_stack_remove_entry(stack, entry);
    c_stack_entry_delete(entry);
    TEST_UINT_EQ(c_stack_length(stack), 0);

    c_stack_delete(stack);
}

int
main(int argc, char **argv) {
    struct test_suite *suite;

    suite = test_suite_new("stack");
    test_suite_initialize_from_args(suite, argc, argv);

    test_suite_start(suite);

    TEST_RUN(suite, main);
    TEST_RUN(suite, clear);
    TEST_RUN(suite, peek_pop);
    TEST_RUN(suite, entries);

    test_suite_print_results_and_exit(suite);
}
