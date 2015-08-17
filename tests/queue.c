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

#define C_TEST_STRING_QUEUE_EQ(queue_, ...)                           \
    do {                                                              \
        const char *entries[] = {__VA_ARGS__};                        \
        size_t nb_entries = sizeof(entries) / sizeof(const char *);   \
        struct c_queue_entry *entry;                                  \
        size_t idx;                                                   \
                                                                      \
        TEST_UINT_EQ(c_queue_length(queue_), nb_entries);             \
                                                                      \
        idx = 0;                                                      \
        entry = c_queue_first_entry(queue_);                          \
        while (entry) {                                               \
            TEST_STRING_EQ(c_queue_entry_value(entry), entries[idx]); \
                                                                      \
            idx++;                                                    \
            entry = c_queue_entry_next(entry);                        \
        }                                                             \
    } while (0)

TEST(main) {
    struct c_queue *queue;

    queue = c_queue_new();
    TEST_UINT_EQ(c_queue_length(queue), 0);
    c_queue_delete(queue);

    queue = c_queue_new();
    c_queue_push(queue, "1");
    C_TEST_STRING_QUEUE_EQ(queue, "1");
    c_queue_delete(queue);

    queue = c_queue_new();
    c_queue_push(queue, "1");
    c_queue_push(queue, "2");
    C_TEST_STRING_QUEUE_EQ(queue, "1", "2");
    c_queue_delete(queue);

    queue = c_queue_new();
    c_queue_push(queue, "1");
    c_queue_push(queue, "2");
    c_queue_push(queue, "3");
    C_TEST_STRING_QUEUE_EQ(queue, "1", "2", "3");
    c_queue_delete(queue);
}

TEST(clear) {
    struct c_queue *queue;

    queue = c_queue_new();
    c_queue_clear(queue);
    TEST_UINT_EQ(c_queue_length(queue), 0);
    c_queue_delete(queue);

    queue = c_queue_new();
    c_queue_push(queue, "1");
    c_queue_push(queue, "2");
    c_queue_push(queue, "3");
    c_queue_clear(queue);
    TEST_UINT_EQ(c_queue_length(queue), 0);
    c_queue_delete(queue);
}

TEST(peek_pop) {
    struct c_queue *queue;

    queue = c_queue_new();
    TEST_PTR_NULL(c_queue_peek(queue));
    TEST_PTR_NULL(c_queue_pop(queue));
    c_queue_delete(queue);

    queue = c_queue_new();
    c_queue_push(queue, "1");
    c_queue_push(queue, "2");
    c_queue_push(queue, "3");
    TEST_STRING_EQ(c_queue_peek(queue), "1");
    TEST_STRING_EQ(c_queue_pop(queue), "1");
    TEST_STRING_EQ(c_queue_peek(queue), "2");
    TEST_STRING_EQ(c_queue_pop(queue), "2");
    c_queue_push(queue, "4");
    TEST_STRING_EQ(c_queue_peek(queue), "3");
    TEST_STRING_EQ(c_queue_pop(queue), "3");
    TEST_STRING_EQ(c_queue_peek(queue), "4");
    TEST_STRING_EQ(c_queue_pop(queue), "4");
    c_queue_push(queue, "5");
    TEST_STRING_EQ(c_queue_peek(queue), "5");
    TEST_STRING_EQ(c_queue_pop(queue), "5");
    TEST_PTR_NULL(c_queue_peek(queue));
    TEST_PTR_NULL(c_queue_pop(queue));
    c_queue_delete(queue);
}

TEST(entries) {
    struct c_queue *queue;
    struct c_queue_entry *entry;

    queue = c_queue_new();
    c_queue_push(queue, "1");
    c_queue_push(queue, "2");
    c_queue_push(queue, "3");
    c_queue_push(queue, "4");
    C_TEST_STRING_QUEUE_EQ(queue, "1", "2", "3", "4");

    entry = c_queue_first_entry(queue);
    TEST_PTR_NOT_NULL(entry);
    TEST_STRING_EQ(c_queue_entry_value(entry), "1");

    entry = c_queue_last_entry(queue);
    TEST_PTR_NOT_NULL(entry);
    TEST_STRING_EQ(c_queue_entry_value(entry), "4");

    entry = c_queue_entry_next(c_queue_first_entry(queue));
    TEST_PTR_NOT_NULL(entry);
    TEST_STRING_EQ(c_queue_entry_value(entry), "2");
    c_queue_remove_entry(queue, entry);
    c_queue_entry_delete(entry);
    C_TEST_STRING_QUEUE_EQ(queue, "1", "3", "4");

    entry = c_queue_entry_prev(c_queue_last_entry(queue));
    TEST_PTR_NOT_NULL(entry);
    TEST_STRING_EQ(c_queue_entry_value(entry), "3");
    c_queue_remove_entry(queue, entry);
    c_queue_entry_delete(entry);
    C_TEST_STRING_QUEUE_EQ(queue, "1", "4");

    entry = c_queue_last_entry(queue);
    c_queue_remove_entry(queue, entry);
    c_queue_entry_delete(entry);
    C_TEST_STRING_QUEUE_EQ(queue, "1");

    entry = c_queue_first_entry(queue);
    c_queue_remove_entry(queue, entry);
    c_queue_entry_delete(entry);
    TEST_UINT_EQ(c_queue_length(queue), 0);

    c_queue_delete(queue);
}

int
main(int argc, char **argv) {
    struct test_suite *suite;

    suite = test_suite_new("queue");
    test_suite_initialize_from_args(suite, argc, argv);

    test_suite_start(suite);

    TEST_RUN(suite, main);
    TEST_RUN(suite, clear);
    TEST_RUN(suite, peek_pop);
    TEST_RUN(suite, entries);

    test_suite_print_results_and_exit(suite);
}
