/*
 * Copyright (c) 2013-2014 Nicolas Martyanoff
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

TEST(insert) {
    struct c_hash_table *table;
    const char *str;

    table = c_hash_table_new(c_hash_string, c_equal_string);

    TEST_TRUE(c_hash_table_is_empty(table));

    c_hash_table_insert(table, "a", "abc");
    TEST_UINT_EQ(c_hash_table_nb_entries(table), 1);

    c_hash_table_insert(table, "d", "def");
    c_hash_table_insert(table, "g", "ghi");
    TEST_UINT_EQ(c_hash_table_nb_entries(table), 3);
    TEST_INT_EQ(c_hash_table_get(table, "a", (void **)&str), 1);
    TEST_STRING_EQ(str, "abc");
    TEST_INT_EQ(c_hash_table_get(table, "g", (void **)&str), 1);
    TEST_STRING_EQ(str, "ghi");

    c_hash_table_insert(table, "g", "foo");
    TEST_UINT_EQ(c_hash_table_nb_entries(table), 3);
    TEST_INT_EQ(c_hash_table_get(table, "g", (void **)&str), 1);
    TEST_STRING_EQ(str, "foo");

    TEST_INT_EQ(c_hash_table_get(table, "k", (void **)&str), 0);

    c_hash_table_delete(table);
}

TEST(insert2) {
    struct c_hash_table *table;
    const char *str, *key, *value;

    table = c_hash_table_new(c_hash_string, c_equal_string);

    c_hash_table_insert2(table, "a", "abc", (void **)&key, (void **)&value);
    TEST_INT_EQ(c_hash_table_get(table, "a", (void **)&str), 1);
    TEST_STRING_EQ(str, "abc");
    TEST_PTR_NULL(key);
    TEST_PTR_NULL(value);

    c_hash_table_insert2(table, "a", "def", (void **)&key, (void **)&value);
    TEST_INT_EQ(c_hash_table_get(table, "a", (void **)&str), 1);
    TEST_STRING_EQ(str, "def");
    TEST_STRING_EQ(key, "a");
    TEST_STRING_EQ(value, "abc");

    c_hash_table_delete(table);
}

TEST(remove) {
    struct c_hash_table *table;

    table = c_hash_table_new(c_hash_string, c_equal_string);
    TEST_INT_EQ(c_hash_table_remove(table, "a"), 0);

    c_hash_table_insert(table, "a", "abc");
    TEST_TRUE(c_hash_table_contains(table, "a"));
    c_hash_table_remove(table, "a");
    TEST_FALSE(c_hash_table_contains(table, "a"));
    TEST_INT_EQ(c_hash_table_remove(table, "a"), 0);

    c_hash_table_insert(table, "a", "abc");
    c_hash_table_insert(table, "d", "def");
    c_hash_table_insert(table, "g", "ghi");
    TEST_TRUE(c_hash_table_contains(table, "a"));
    TEST_FALSE(c_hash_table_contains(table, "j"));
    TEST_INT_EQ(c_hash_table_remove(table, "d"), 1);
    TEST_INT_EQ(c_hash_table_remove(table, "j"), 0);

    c_hash_table_delete(table);
}

TEST(remove2) {
    struct c_hash_table *table;
    const char *key, *value;

    table = c_hash_table_new(c_hash_string, c_equal_string);

    c_hash_table_insert(table, "a", "abc");
    TEST_INT_EQ(c_hash_table_remove2(table, "a",
                                 (void **)&key, (void **)&value), 1);
    TEST_STRING_EQ(key, "a");
    TEST_STRING_EQ(value, "abc");

    c_hash_table_delete(table);
}

TEST(clear) {
    struct c_hash_table *table;

    table = c_hash_table_new(c_hash_string, c_equal_string);

    c_hash_table_insert(table, "a", "abc");
    c_hash_table_clear(table);
    TEST_TRUE(c_hash_table_is_empty(table));
    TEST_FALSE(c_hash_table_contains(table, "a"));

    c_hash_table_delete(table);
}

TEST(resize) {
    struct c_hash_table *table;

    size_t nb_entries = 100;
    size_t nb_removed = 90;

    table = c_hash_table_new(c_hash_int32, c_equal_int32);

    for (size_t i = 0; i < nb_entries; i++)
        c_hash_table_insert(table, C_INT32_TO_POINTER(i), C_INT32_TO_POINTER(1));

    for (size_t i = 0; i < nb_entries; i++)
        TEST_TRUE(c_hash_table_contains(table, C_INT32_TO_POINTER(i)));

    for (size_t i = 0; i < nb_removed; i++)
        c_hash_table_remove(table, C_INT32_TO_POINTER(i));

    TEST_UINT_EQ(c_hash_table_nb_entries(table), nb_entries - nb_removed);

    for (size_t i = 0; i < nb_entries; i++) {
        if (i < nb_removed) {
            TEST_FALSE(c_hash_table_contains(table, C_INT32_TO_POINTER(i)));
        } else {
            TEST_TRUE(c_hash_table_contains(table, C_INT32_TO_POINTER(i)));
        }
    }

    c_hash_table_delete(table);
}

TEST(iterate) {
    struct c_hash_table *table;
    struct c_hash_table_iterator *it;
    void *key, *value;

    size_t nb_values;
    struct {
        int key;
        char *value;
        bool found;
    } values[] = {
        {0, "abc", false},
        {1, "def", false},
        {2, "ghi", false},
        {3, "jkl", false},
        {4, "mno", false},
        {5, "pqr", false},
        {6, "stu", false},
        {7, "vwx", false},
        {8, "yz",  false}
    };

    nb_values = sizeof(values) / sizeof(values[0]);

    table = c_hash_table_new(c_hash_int32, c_equal_int32);

    it = c_hash_table_iterate(table);
    TEST_INT_EQ(c_hash_table_iterator_next(it, &key, &value), 0);
    c_hash_table_iterator_delete(it);

    for (size_t i = 0; i < nb_values; i++) {
        c_hash_table_insert(table, C_INT32_TO_POINTER(values[i].key),
                        values[i].value);
    }

    it = c_hash_table_iterate(table);

    for (size_t i = 0; i < nb_values; i++) {
        bool found;

        TEST_INT_EQ(c_hash_table_iterator_next(it, &key, &value), 1);

        found = false;
        for (size_t i = 0; i < nb_values; i++) {
            if (C_POINTER_TO_INT32(key) == values[i].key) {
                found = true;
                TEST_FALSE(values[i].found);
                TEST_INT_EQ(strcmp(value, values[i].value), 0);
                values[i].found = true;
                break;
            }
        }

        TEST_TRUE(found);
    }

    for (size_t i = 0; i < nb_values; i++)
        TEST_TRUE(values[i].found);

    TEST_INT_EQ(c_hash_table_iterator_next(it, &key, &value), 0);

    c_hash_table_iterator_delete(it);
    c_hash_table_delete(table);
}

TEST(iterate_set_value) {
    struct c_hash_table *table;
    struct c_hash_table_iterator *it;
    void *key, *value;

    table = c_hash_table_new(c_hash_string, c_equal_string);

    c_hash_table_insert(table, "a", "abc");
    c_hash_table_insert(table, "d", "def");
    c_hash_table_insert(table, "g", "ghi");

    it = c_hash_table_iterate(table);

    while (c_hash_table_iterator_next(it, &key, &value)) {
        if (strcmp(key, "g") == 0)
            c_hash_table_iterator_set_value(it, "foo");
    }

    c_hash_table_iterator_delete(it);

    TEST_TRUE(c_hash_table_contains(table, "a"));
    TEST_INT_EQ(c_hash_table_get(table, "g", &value), 1);
    TEST_STRING_EQ(value, "foo");

    c_hash_table_delete(table);
}

TEST(iterate_remove) {
    struct c_hash_table *table;
    struct c_hash_table_iterator *it;
    void *key, *value;

    table = c_hash_table_new(c_hash_string, c_equal_string);

    c_hash_table_insert(table, "a", "abc");
    c_hash_table_insert(table, "d", "def");
    c_hash_table_insert(table, "g", "ghi");
    c_hash_table_insert(table, "j", "jkl");
    c_hash_table_insert(table, "m", "mno");
    c_hash_table_insert(table, "p", "qrs");
    c_hash_table_insert(table, "t", "tuv");

    it = c_hash_table_iterate(table);

    while (c_hash_table_iterator_next(it, &key, &value)) {
        if (strcmp(key, "a") == 0
         || strcmp(key, "g") == 0
         || strcmp(key, "m") == 0
         || strcmp(key, "t") == 0) {
            c_hash_table_remove(table, key);
        }
    }

    c_hash_table_iterator_delete(it);

    TEST_UINT_EQ(c_hash_table_nb_entries(table), 3);
    TEST_FALSE(c_hash_table_contains(table, "a"));
    TEST_TRUE(c_hash_table_contains(table, "d"));
    TEST_FALSE(c_hash_table_contains(table, "g"));
    TEST_TRUE(c_hash_table_contains(table, "j"));
    TEST_FALSE(c_hash_table_contains(table, "m"));
    TEST_TRUE(c_hash_table_contains(table, "p"));
    TEST_FALSE(c_hash_table_contains(table, "t"));

    it = c_hash_table_iterate(table);
    while (c_hash_table_iterator_next(it, &key, &value))
        c_hash_table_remove(table, key);
    c_hash_table_iterator_delete(it);

    TEST_UINT_EQ(c_hash_table_nb_entries(table), 0);

    c_hash_table_delete(table);
}

int
main(int argc, char **argv) {
    struct test_suite *suite;

    suite = test_suite_new("hash-table");
    test_suite_initialize_from_args(suite, argc, argv);

    test_suite_start(suite);

    TEST_RUN(suite, insert);
    TEST_RUN(suite, insert2);
    TEST_RUN(suite, remove);
    TEST_RUN(suite, remove2);
    TEST_RUN(suite, clear);
    TEST_RUN(suite, resize);
    TEST_RUN(suite, iterate);
    TEST_RUN(suite, iterate_set_value);
    TEST_RUN(suite, iterate_remove);

    test_suite_print_results_and_exit(suite);
}
