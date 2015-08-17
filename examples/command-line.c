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

#include "../src/internal.h"

int
main(int argc, char **argv) {
    struct c_command_line *cmdline;
    const char *value, *string_value;
    size_t nb_trailing_args;

    cmdline = c_command_line_new();

    c_command_line_set_program_description(cmdline,
                                           "A short program showing how to use "
                                           "the command line module.");

    c_command_line_add_flag(cmdline, "s", NULL,
                            "a boolean option without long name");
    c_command_line_add_flag(cmdline, NULL, "long",
                            "a boolean option without short name");

    c_command_line_add_option(cmdline, "v", NULL,
                              "an option with a value and no long name",
                              "value", NULL);
    c_command_line_add_option(cmdline, NULL, "string-value",
                              "an option with a string value",
                              "string", NULL);

    c_command_line_add_argument(cmdline, "the first argument", "arg-1");
    c_command_line_add_argument(cmdline, "the second argument", "arg-2");
    c_command_line_add_trailing_arguments(cmdline, "trailing arguments",
                                          "trailing-arg");

    if (c_command_line_parse(cmdline, argc, argv) == -1) {
        fprintf(stderr, "%s\n", c_get_error());
        return 1;
    }

    printf("'s' option set: %s\n",
           c_command_line_is_option_set(cmdline, "s") ? "yes" : "no");

    printf("'long' option set: %s\n",
           c_command_line_is_option_set(cmdline, "long") ? "yes" : "no");

    value = c_command_line_option_value(cmdline, "v");
    printf("'value' option: %s\n", (value ? value : "NULL"));

    string_value = c_command_line_option_value(cmdline, "string-value");
    printf("'string-value' option: %s\n",
           (string_value ? string_value : "NULL"));

    printf("first argument: %s\n",
           c_command_line_argument_value(cmdline, 0));
    printf("second argument: %s\n",
           c_command_line_argument_value(cmdline, 1));

    nb_trailing_args = c_command_line_nb_trailing_arguments(cmdline);
    for (size_t i = 0; i < nb_trailing_args; i++) {
        printf("trailing arg %zu: %s\n", i,
               c_command_line_trailing_argument_value(cmdline, i));
    }

    c_command_line_delete(cmdline);
    return 0;
}
