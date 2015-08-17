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

#include "../src/internal.h"

static int c_test_heap_cmp(const void *, const void *);

#define C_TEST_HEAP_EQ(heap_, ...)                                  \
    do {                                                            \
        int entries[] = {__VA_ARGS__};                              \
        size_t nb_entries = sizeof(entries) / sizeof(int);          \
                                                                    \
        TEST_UINT_EQ(c_heap_nb_entries(heap_), nb_entries);         \
                                                                    \
        for (size_t i = 0; i < c_heap_nb_entries(heap_); i++) {     \
            int value;                                              \
                                                                    \
            value = C_POINTER_TO_INT32(c_heap_entry(heap_, i));     \
            if (value != entries[i]) {                              \
                TEST_ABORT("entry %zu has value %d but should "     \
                           "have value %d", i, value, entries[i]);  \
            }                                                       \
        }                                                           \
    } while (0)

TEST(add) {
    struct c_heap *heap;

    heap = c_heap_new(c_test_heap_cmp);
    C_TEST_HEAP_EQ(heap);

    c_heap_add(heap, C_INT32_TO_POINTER(9));
    C_TEST_HEAP_EQ(heap, 9);

    c_heap_add(heap, C_INT32_TO_POINTER(3));
    C_TEST_HEAP_EQ(heap, 3, 9);

    c_heap_add(heap, C_INT32_TO_POINTER(4));
    C_TEST_HEAP_EQ(heap, 3, 9, 4);

    c_heap_add(heap, C_INT32_TO_POINTER(2));
    C_TEST_HEAP_EQ(heap, 2, 3, 4, 9);

    c_heap_add(heap, C_INT32_TO_POINTER(5));
    C_TEST_HEAP_EQ(heap, 2, 3, 4, 9, 5);

    c_heap_add(heap, C_INT32_TO_POINTER(3));
    C_TEST_HEAP_EQ(heap, 2, 3, 3, 9, 5, 4);

    c_heap_delete(heap);
}

TEST(pop) {
    struct c_heap *heap;

    heap = c_heap_new(c_test_heap_cmp);

    c_heap_add(heap, C_INT32_TO_POINTER(9));
    c_heap_add(heap, C_INT32_TO_POINTER(3));
    c_heap_add(heap, C_INT32_TO_POINTER(4));
    c_heap_add(heap, C_INT32_TO_POINTER(2));
    c_heap_add(heap, C_INT32_TO_POINTER(5));
    c_heap_add(heap, C_INT32_TO_POINTER(3));
    C_TEST_HEAP_EQ(heap, 2, 3, 3, 9, 5, 4);

    TEST_INT_EQ(C_POINTER_TO_INT32(c_heap_pop(heap)), 2);
    C_TEST_HEAP_EQ(heap, 3, 4, 3, 9, 5);

    TEST_INT_EQ(C_POINTER_TO_INT32(c_heap_pop(heap)), 3);
    C_TEST_HEAP_EQ(heap, 3, 4, 5, 9);

    TEST_INT_EQ(C_POINTER_TO_INT32(c_heap_pop(heap)), 3);
    C_TEST_HEAP_EQ(heap, 4, 9, 5);

    TEST_INT_EQ(C_POINTER_TO_INT32(c_heap_pop(heap)), 4);
    C_TEST_HEAP_EQ(heap, 5, 9);

    TEST_INT_EQ(C_POINTER_TO_INT32(c_heap_pop(heap)), 5);
    C_TEST_HEAP_EQ(heap, 9);

    TEST_INT_EQ(C_POINTER_TO_INT32(c_heap_pop(heap)), 9);
    C_TEST_HEAP_EQ(heap);

    TEST_INT_EQ(C_POINTER_TO_INT32(c_heap_pop(heap)), 0);

    c_heap_delete(heap);


    heap = c_heap_new(c_test_heap_cmp);

    c_heap_add(heap, C_INT32_TO_POINTER(3));
    c_heap_add(heap, C_INT32_TO_POINTER(2));
    c_heap_add(heap, C_INT32_TO_POINTER(4));
    C_TEST_HEAP_EQ(heap, 2, 3, 4);

    TEST_INT_EQ(C_POINTER_TO_INT32(c_heap_pop(heap)), 2);
    C_TEST_HEAP_EQ(heap, 3, 4);

    TEST_INT_EQ(C_POINTER_TO_INT32(c_heap_pop(heap)), 3);
    C_TEST_HEAP_EQ(heap, 4);

    TEST_INT_EQ(C_POINTER_TO_INT32(c_heap_pop(heap)), 4);
    C_TEST_HEAP_EQ(heap);

    c_heap_delete(heap);
}

TEST(find) {
    struct c_heap *heap;
    size_t idx;

    heap = c_heap_new(c_test_heap_cmp);

    c_heap_add(heap, C_INT32_TO_POINTER(9));
    c_heap_add(heap, C_INT32_TO_POINTER(3));
    c_heap_add(heap, C_INT32_TO_POINTER(4));
    c_heap_add(heap, C_INT32_TO_POINTER(2));
    c_heap_add(heap, C_INT32_TO_POINTER(5));
    c_heap_add(heap, C_INT32_TO_POINTER(1));

    C_TEST_HEAP_EQ(heap, 1, 3, 2, 9, 5, 4);

    TEST_INT_EQ(c_heap_find(heap, C_INT32_TO_POINTER(1), &idx), 1);
    TEST_UINT_EQ(idx, 0);

    TEST_INT_EQ(c_heap_find(heap, C_INT32_TO_POINTER(3), &idx), 1);
    TEST_UINT_EQ(idx, 1);

    TEST_INT_EQ(c_heap_find(heap, C_INT32_TO_POINTER(2), &idx), 1);
    TEST_UINT_EQ(idx, 2);

    TEST_INT_EQ(c_heap_find(heap, C_INT32_TO_POINTER(9), &idx), 1);
    TEST_UINT_EQ(idx, 3);

    TEST_INT_EQ(c_heap_find(heap, C_INT32_TO_POINTER(5), &idx), 1);
    TEST_UINT_EQ(idx, 4);

    TEST_INT_EQ(c_heap_find(heap, C_INT32_TO_POINTER(4), &idx), 1);
    TEST_UINT_EQ(idx, 5);

    TEST_INT_EQ(c_heap_find(heap, C_INT32_TO_POINTER(6), &idx), 0);
    TEST_INT_EQ(c_heap_find(heap, C_INT32_TO_POINTER(7), &idx), 0);
    TEST_INT_EQ(c_heap_find(heap, C_INT32_TO_POINTER(8), &idx), 0);

    c_heap_delete(heap);
}

TEST(remove) {
    struct c_heap *heap;

    heap = c_heap_new(c_test_heap_cmp);

    c_heap_add(heap, C_INT32_TO_POINTER(9));
    c_heap_add(heap, C_INT32_TO_POINTER(3));
    c_heap_add(heap, C_INT32_TO_POINTER(4));
    c_heap_add(heap, C_INT32_TO_POINTER(2));
    c_heap_add(heap, C_INT32_TO_POINTER(5));
    c_heap_add(heap, C_INT32_TO_POINTER(1));

    C_TEST_HEAP_EQ(heap, 1, 3, 2, 9, 5, 4);

    c_heap_remove(heap, C_INT32_TO_POINTER(1));
    C_TEST_HEAP_EQ(heap, 2, 3, 4, 9, 5);

    c_heap_remove(heap, C_INT32_TO_POINTER(5));
    C_TEST_HEAP_EQ(heap, 2, 3, 4, 9);

    c_heap_remove(heap, C_INT32_TO_POINTER(2));
    C_TEST_HEAP_EQ(heap, 3, 9, 4);

    c_heap_remove(heap, C_INT32_TO_POINTER(9));
    C_TEST_HEAP_EQ(heap, 3, 4);

    c_heap_remove(heap, C_INT32_TO_POINTER(4));
    C_TEST_HEAP_EQ(heap, 3);

    c_heap_remove(heap, C_INT32_TO_POINTER(3));
    C_TEST_HEAP_EQ(heap);
}

int
main(int argc, char **argv) {
    struct test_suite *suite;

    suite = test_suite_new("heap");
    test_suite_initialize_from_args(suite, argc, argv);

    test_suite_start(suite);

    TEST_RUN(suite, add);
    TEST_RUN(suite, pop);
    TEST_RUN(suite, find);
    TEST_RUN(suite, remove);

    test_suite_print_results_and_exit(suite);
}

static int
c_test_heap_cmp(const void *arg1, const void *arg2) {
    int i1, i2;

    i1 = C_POINTER_TO_INT32(arg1);
    i2 = C_POINTER_TO_INT32(arg2);

    if (i1 < i2) {
        return -1;
    } else if (i2 < i1) {
        return 1;
    } else {
        return 0;
    }
}
