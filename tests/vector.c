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

TEST(initialization) {
    struct c_vector *vector;

    vector = c_vector_new(4);

    TEST_UINT_EQ(c_vector_length(vector), 0);
    TEST_TRUE(c_vector_is_empty(vector));

    c_vector_delete(vector);
}

TEST(append) {
    struct c_vector *vector;
    int value;

    vector = c_vector_new(sizeof(int));

    value = 1;
    c_vector_append(vector, &value);
    TEST_UINT_EQ(c_vector_length(vector), 1);
    TEST_INT_EQ(*(int *)c_vector_entry(vector, 0), 1);

    value = 2;
    c_vector_append(vector, &value);
    value = 3;
    c_vector_append(vector, &value);
    TEST_UINT_EQ(c_vector_length(vector), 3);
    TEST_INT_EQ(*(int *)c_vector_entry(vector, 0), 1);
    TEST_INT_EQ(*(int *)c_vector_entry(vector, 1), 2);
    TEST_INT_EQ(*(int *)c_vector_entry(vector, 2), 3);

    c_vector_delete(vector);
}

TEST(remove) {
    struct c_vector *vector;
    int value;

    vector = c_vector_new(sizeof(int));
    value = 1;
    c_vector_append(vector, &value);
    c_vector_remove(vector, 0);
    TEST_UINT_EQ(c_vector_length(vector), 0);
    c_vector_delete(vector);

    vector = c_vector_new(sizeof(int));
    value = 1;
    c_vector_append(vector, &value);
    value = 2;
    c_vector_append(vector, &value);
    value = 3;
    c_vector_append(vector, &value);
    c_vector_remove(vector, 0);
    TEST_UINT_EQ(c_vector_length(vector), 2);
    TEST_INT_EQ(*(int *)c_vector_entry(vector, 0), 2);
    TEST_INT_EQ(*(int *)c_vector_entry(vector, 1), 3);
    c_vector_delete(vector);

    vector = c_vector_new(sizeof(int));
    value = 1;
    c_vector_append(vector, &value);
    value = 2;
    c_vector_append(vector, &value);
    value = 3;
    c_vector_append(vector, &value);
    c_vector_remove(vector, 1);
    TEST_UINT_EQ(c_vector_length(vector), 2);
    TEST_INT_EQ(*(int *)c_vector_entry(vector, 0), 1);
    TEST_INT_EQ(*(int *)c_vector_entry(vector, 1), 3);
    c_vector_delete(vector);

    vector = c_vector_new(sizeof(int));
    value = 1;
    c_vector_append(vector, &value);
    value = 2;
    c_vector_append(vector, &value);
    value = 3;
    c_vector_append(vector, &value);
    c_vector_remove(vector, 2);
    TEST_UINT_EQ(c_vector_length(vector), 2);
    TEST_INT_EQ(*(int *)c_vector_entry(vector, 0), 1);
    TEST_INT_EQ(*(int *)c_vector_entry(vector, 1), 2);
    c_vector_delete(vector);
}

TEST(clear) {
    struct c_vector *vector;
    int value;

    vector = c_vector_new(sizeof(int));
    c_vector_clear(vector);
    TEST_UINT_EQ(c_vector_length(vector), 0);
    c_vector_delete(vector);

    vector = c_vector_new(sizeof(int));
    value = 1;
    c_vector_append(vector, &value);
    c_vector_clear(vector);
    TEST_UINT_EQ(c_vector_length(vector), 0);
    c_vector_delete(vector);

    vector = c_vector_new(sizeof(int));
    value = 1;
    c_vector_append(vector, &value);
    value = 2;
    c_vector_append(vector, &value);
    value = 3;
    c_vector_append(vector, &value);
    c_vector_clear(vector);
    TEST_UINT_EQ(c_vector_length(vector), 0);
    c_vector_delete(vector);
}

int
main(int argc, char **argv) {
    struct test_suite *suite;

    suite = test_suite_new("vector");
    test_suite_initialize_from_args(suite, argc, argv);

    test_suite_start(suite);

    TEST_RUN(suite, initialization);
    TEST_RUN(suite, append);
    TEST_RUN(suite, remove);
    TEST_RUN(suite, clear);

    test_suite_print_results_and_exit(suite);
}
