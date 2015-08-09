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

#include "internal.h"

struct c_command_line_option {
    char *short_name;
    char *long_name;

    char *description;

    bool can_have_value;
    char *value_name;
    char *default_value;

    bool is_set;
    char *value;
};

struct c_command_line_argument {
    char *description;
    char *value_name;
    char *value;

    bool is_trailing;
    size_t nb_trailing_values;
    char **trailing_values;
};

struct c_command_line {
    char *program_name;
    char *program_description;
    char *trailing_text;

    struct c_command_line_option **options;
    size_t nb_options;
    struct c_hash_table *option_table;

    struct c_command_line_argument **arguments;
    size_t nb_arguments;
};

static void c_command_line_reset(struct c_command_line *);
static int c_command_line_register_option(struct c_command_line *,
                                          struct c_command_line_option *);
static int c_command_line_register_argument(struct c_command_line *,
                                          struct c_command_line_argument *);


static struct c_command_line_option *
c_command_line_option_new(const char *, const char *, const char *);
static void c_command_line_option_delete(struct c_command_line_option *);
static void c_command_line_option_reset(struct c_command_line_option *);

static int c_command_line_option_set_value(struct c_command_line_option *,
                                           const char *, const char *);
static struct c_command_line_option *
c_command_line_get_option(const struct c_command_line *, const char *);
static int c_command_line_parse_option(const struct c_command_line *,
                                       const char *, char **, int, int);

static struct c_command_line_argument *
c_command_line_argument_new(const char *, const char *);
static void c_command_line_argument_delete(struct c_command_line_argument *);
static void c_command_line_argument_reset(struct c_command_line_argument *);

struct c_command_line *
c_command_line_new(void) {
    struct c_command_line *cmdline;

    cmdline = c_malloc0(sizeof(struct c_command_line));
    if (!cmdline)
        return NULL;

    cmdline->option_table = c_hash_table_new(c_hash_string, c_equal_string);
    if (!cmdline->option_table)
        goto error;

    if (c_command_line_add_flag(cmdline, "h", "help", "display help") == -1)
        goto error;

    return cmdline;

error:
    c_command_line_delete(cmdline);
    return NULL;
}

void
c_command_line_delete(struct c_command_line *cmdline) {
    if (!cmdline)
        return;

    c_command_line_reset(cmdline);

    c_free(cmdline->program_description);
    c_free(cmdline->trailing_text);

    for (size_t i = 0; i < cmdline->nb_options; i++)
        c_command_line_option_delete(cmdline->options[i]);
    c_free(cmdline->options);

    c_hash_table_delete(cmdline->option_table);

    for (size_t i = 0; i < cmdline->nb_arguments; i++)
        c_command_line_argument_delete(cmdline->arguments[i]);
    c_free(cmdline->arguments);

    c_free0(cmdline, sizeof(struct c_command_line));
}

int
c_command_line_set_program_description(struct c_command_line *cmdline,
                                       const char *description) {
    char *program_description;

    program_description = c_strdup(description);
    if (!program_description)
        return -1;

    c_free(cmdline->program_description);
    cmdline->program_description = program_description;

    return 0;
}

int
c_command_line_set_trailing_text(struct c_command_line *cmdline,
                                 const char *text) {
    char *trailing_text;

    trailing_text = c_strdup(text);
    if (!trailing_text)
        return -1;

    c_free(cmdline->trailing_text);
    cmdline->trailing_text = trailing_text;

    return 0;
}

int
c_command_line_add_flag(struct c_command_line *cmdline,
                        const char *short_name, const char *long_name,
                        const char *description) {
    struct c_command_line_option *option;

    option = c_command_line_option_new(short_name, long_name, description);
    if (!option)
        return -1;

    if (c_command_line_register_option(cmdline, option) == -1) {
        c_command_line_option_delete(option);
        return -1;
    }

    return 0;
}

int
c_command_line_add_option(struct c_command_line *cmdline,
                          const char *short_name, const char *long_name,
                          const char *description, const char *value_name,
                          const char *default_value) {
    struct c_command_line_option *option;

    option = c_command_line_option_new(short_name, long_name, description);
    if (!option)
        return -1;

    if (c_command_line_option_set_value(option, value_name,
                                        default_value) == -1) {
        c_command_line_option_delete(option);
        return -1;
    }

    if (c_command_line_register_option(cmdline, option) == -1) {
        c_command_line_option_delete(option);
        return -1;
    }

    return 0;
}

int
c_command_line_parse(struct c_command_line *cmdline, int argc, char **argv) {
    struct c_command_line_argument *trailing_argument;
    int i;

    if (argc < 1) {
        c_set_error("invalid argument count (minimum is 1)");
        return -1;
    }

    c_command_line_reset(cmdline);

    cmdline->program_name = c_strdup(argv[0]);
    if (!cmdline->program_name)
        goto error;

    i = 1;
    while (i < argc) {
        const char *arg, *ptr;

        arg = argv[i];

        if (arg[0] == '-') {
            const char *name;
            int ret;

            if (arg[1] == '-' && arg[2] != '\0') {
                /* Long argument name */
                name = arg + 2;

                ret = c_command_line_parse_option(cmdline, name, argv, argc, i);
                if (ret == -1)
                    goto error;

                if (ret == 1) {
                    /* Value read */
                    i++;
                }
            } else if (arg[1] != '-' && arg[1] != '\0') {
                /* Short option name */
                for (ptr = arg + 1; *ptr != '\0'; ptr++) {
                    char short_name[2];

                    short_name[0] = *ptr;
                    short_name[1] = '\0';
                    name = short_name;

                    ret = c_command_line_parse_option(cmdline, name, argv, argc, i);
                    if (ret == -1)
                        goto error;

                    if (ret == 1) {
                        /* A value was read */
                        i++;
                    }
                }
            } else if (arg[1] == '-') {
                /* '--' separator */
                i++;
                goto end_of_options;
            } else {
                /* '-' */
                goto end_of_options;
            }
        } else {
            goto end_of_options;
        }

        i++;
    }

end_of_options:
    if (c_command_line_is_option_set(cmdline, "help")) {
        c_command_line_usage_print(cmdline, stdout);
        c_command_line_delete(cmdline);
        exit(0);
    }

    if ((size_t)(argc - i) < cmdline->nb_arguments) {
        c_set_error("missing argument(s)");
        goto error;
    }

    trailing_argument = NULL;
    for (size_t idx = 0; idx < cmdline->nb_arguments; idx++, i++) {
        struct c_command_line_argument *argument;

        argument = cmdline->arguments[idx];
        if (argument->is_trailing) {
            trailing_argument = argument;
            break;
        }

        argument->value = c_strdup(argv[i]);
        if (!argument->value)
            goto error;
    }

    if (trailing_argument) {
        size_t nb_values;
        char **values;

        nb_values = (size_t)(argc - i);
        values = c_calloc(nb_values, sizeof(char *));
        if (!values)
            goto error;

        for (size_t idx = 0; idx < nb_values; idx++, i++) {
            values[idx] = c_strdup(argv[i]);
            if (!values[idx]) {
                for (size_t j = 0; j < idx; j++)
                    c_free(values[j]);
                c_free(values);
                goto error;
            }
        }

        trailing_argument->nb_trailing_values = nb_values;
        trailing_argument->trailing_values = values;
    }

    return i;

error:
    c_command_line_reset(cmdline);
    return -1;
}

char *
c_command_line_usage_string(const struct c_command_line *cmdline) {
    struct c_buffer *buf;
    char *string;
    size_t width;

    if (!cmdline->program_name) {
        c_set_error("no command line parsed");
        return NULL;
    }

    buf = c_buffer_new();
    if (!buf)
        return NULL;

    /* First column width */
    width = 0;

    for (size_t i = 0; i < cmdline->nb_options; i++) {
        struct c_command_line_option *option;
        size_t length;

        option = cmdline->options[i];

        length = 4; /* short name + ", " */

        if (option->long_name)
            length += strlen(option->long_name) + 2; /* "--" */

        if (option->value_name)
            length += strlen(option->value_name) + 3; /* " <>" */

        if (length > width)
            width = length;
    }

    for (size_t i = 0; i < cmdline->nb_arguments; i++) {
        struct c_command_line_argument *argument;
        size_t length;

        argument = cmdline->arguments[i];
        length = strlen(argument->value_name);

        if (length > width)
            width = length;
    }

    /* Usage line */
    c_buffer_add_printf(buf, "Usage: %s", cmdline->program_name);
    if (cmdline->nb_options > 0)
        c_buffer_add_printf(buf, " OPTIONS");
    if (cmdline->nb_arguments > 0) {
        for (size_t i = 0; i < cmdline->nb_arguments; i++) {
            struct c_command_line_argument *argument;

            argument = cmdline->arguments[i];

            if (argument->is_trailing) {
                c_buffer_add_printf(buf, " [<%s>, ...]", argument->value_name);
            } else {
                c_buffer_add_printf(buf, " <%s>", argument->value_name);
            }
        }
    }
    c_buffer_add_string(buf, "\n\n");

    /* Description */
    if (cmdline->program_description)
        c_buffer_add_printf(buf, "%s\n\n", cmdline->program_description);

    /* Options */
    if (cmdline->nb_options > 0) {
        char *tmp;
        size_t tmp_size;

        tmp_size = width + 1;
        tmp = c_malloc(tmp_size);
        if (!tmp) {
            c_set_error("cannot allocate temporary buffer: %s", c_get_error());
            goto error;
        }

        c_buffer_add_string(buf, "OPTIONS\n\n");

        for (size_t i = 0; i < cmdline->nb_options; i++) {
            struct c_command_line_option *option;

            option = cmdline->options[i];

            if (option->short_name) {
                c_buffer_add_printf(buf, "-%s%s ",
                                    option->short_name,
                                    option->long_name ? "," : " ");
            } else {
                c_buffer_add_string(buf, "    ");
            }

            if (option->long_name) {
                if (option->value_name) {
                    snprintf(tmp, tmp_size, "--%s <%s>",
                             option->long_name, option->value_name);
                } else {
                    snprintf(tmp, tmp_size, "--%s",
                             option->long_name);
                }
            } else {
                if (option->value_name) {
                    snprintf(tmp, tmp_size, "<%s>",
                             option->value_name);
                } else {
                    snprintf(tmp, tmp_size, "");
                }
            }

            c_buffer_add_printf(buf, "%-*s  %s",
                                (int)width - 4, /* the short name is not
                                                   in tmp */
                                tmp, option->description);

            if (option->default_value) {
                c_buffer_add_printf(buf, " (default: %s)",
                                    option->default_value);
            }

            c_buffer_add_string(buf, "\n");
        }

        c_free(tmp);
    }

    /* Arguments */
    if (cmdline->nb_arguments > 0) {
        c_buffer_add_string(buf, "\nARGUMENTS\n\n");

        for (size_t i = 0; i < cmdline->nb_arguments; i++) {
            struct c_command_line_argument *argument;

            argument = cmdline->arguments[i];

            c_buffer_add_printf(buf, "%-*s  %s\n",
                                (int)width, argument->value_name,
                                argument->description);
        }
    }

    /* Trailing text */
    if (cmdline->trailing_text)
        c_buffer_add_printf(buf, "\n%s\n", cmdline->trailing_text);

    string = c_buffer_extract_string(buf, NULL);
    if (!string)
        goto error;
    c_buffer_delete(buf);
    return string;

error:
    c_buffer_delete(buf);
    return NULL;
}

int
c_command_line_usage_print(const struct c_command_line *cmdline, FILE *file) {
    char *string;

    string = c_command_line_usage_string(cmdline);
    if (!string)
        return -1;

    if (fputs(string, file) == EOF) {
        c_set_error("cannot write string to file: %s", strerror(errno));
        c_free(string);
        return -1;
    }

    c_free(string);
    return 0;
}

bool
c_command_line_is_option_set(struct c_command_line *cmdline, const char *name) {
    struct c_command_line_option *option;

    option = c_command_line_get_option(cmdline, name);
    if (!option)
        return false;

    return option->is_set;
}

const char *
c_command_line_option_value(struct c_command_line *cmdline,
                            const char *name) {
    struct c_command_line_option *option;

    option = c_command_line_get_option(cmdline, name);
    if (!option) {
        c_set_error("unknown option '%s'", name);
        return NULL;
    }

    if (!option->can_have_value) {
        c_set_error("option '%s' cannot have a value", name);
        return NULL;
    }

    return option->value ? option->value : option->default_value;
}

const char *
c_command_line_argument_value(struct c_command_line *cmdline, size_t idx) {
    return cmdline->arguments[idx]->value;
}

size_t
c_command_line_nb_trailing_arguments(struct c_command_line *cmdline) {
    struct c_command_line_argument *argument;

    argument = cmdline->arguments[cmdline->nb_arguments - 1];
    return argument->nb_trailing_values;
}

const char **
c_command_line_trailing_argument_values(struct c_command_line *cmdline) {
    struct c_command_line_argument *argument;

    argument = cmdline->arguments[cmdline->nb_arguments - 1];
    return (const char **)argument->trailing_values;
}

const char *
c_command_line_trailing_argument_value(struct c_command_line *cmdline,
                                       size_t idx) {
    struct c_command_line_argument *argument;

    argument = cmdline->arguments[cmdline->nb_arguments - 1];
    return argument->trailing_values[idx];
}

static void
c_command_line_reset(struct c_command_line *cmdline) {
    c_free(cmdline->program_name);
    cmdline->program_name = NULL;

    for (size_t i = 0; i < cmdline->nb_options; i++)
        c_command_line_option_reset(cmdline->options[i]);

    for (size_t i = 0; i < cmdline->nb_arguments; i++)
        c_command_line_argument_reset(cmdline->arguments[i]);
}

static int
c_command_line_register_option(struct c_command_line *cmdline,
                               struct c_command_line_option *option) {
    struct c_command_line_option **options;
    size_t nsz;

    if (!option->short_name && !option->long_name) {
        c_set_error("null short and long name");
        return -1;
    }

    if (option->short_name) {
        if (c_hash_table_contains(cmdline->option_table, option->short_name)) {
            c_set_error("duplicate option name");
            return -1;
        }
    }

    if (option->long_name) {
        if (c_hash_table_contains(cmdline->option_table, option->long_name)) {
            c_set_error("duplicate option name");
            return -1;
        }
    }

    nsz = (cmdline->nb_options + 1) * sizeof(struct c_command_line_option *);
    options = c_realloc(cmdline->options, nsz);
    if (!options)
        return -1;

    if (option->short_name) {
        if (c_hash_table_insert(cmdline->option_table, option->short_name,
                                option) == -1) {
            c_free(options);
            return -1;
        }
    }

    if (option->long_name) {
        if (c_hash_table_insert(cmdline->option_table, option->long_name,
                                option) == -1) {
            c_free(options);
            return -1;
        }
    }

    cmdline->options = options;
    cmdline->options[cmdline->nb_options++] = option;
    return 0;
}

static int
c_command_line_register_argument(struct c_command_line *cmdline,
                                 struct c_command_line_argument *argument) {
    struct c_command_line_argument **arguments;
    size_t nsz;

    nsz = (cmdline->nb_arguments + 1)
        * sizeof(struct c_command_line_argument *);
    arguments = c_realloc(cmdline->arguments, nsz);
    if (!arguments)
        return -1;

    cmdline->arguments = arguments;
    cmdline->arguments[cmdline->nb_arguments++] = argument;
    return 0;
}

static struct c_command_line_option *
c_command_line_option_new(const char *short_name, const char *long_name,
                          const char *description) {
    struct c_command_line_option *option;

    option = c_malloc0(sizeof(struct c_command_line_option));
    if (!option)
        return NULL;

    if (short_name) {
        if (strlen(short_name) != 1) {
            c_set_error("invalid short name length");
            goto error;
        }

        option->short_name = c_strdup(short_name);
        if (!option->short_name)
            goto error;
    }

    if (long_name) {
        if (strlen(long_name) <= 1) {
            c_set_error("invalid long name length");
            goto error;
        }

        option->long_name = c_strdup(long_name);
        if (!option->long_name)
            goto error;
    }

    option->description = c_strdup(description);
    if (!option->description)
        goto error;

    return option;

error:
    c_command_line_option_delete(option);
    return NULL;
}

static void
c_command_line_option_delete(struct c_command_line_option *option) {
    if (!option)
        return;

    c_free(option->short_name);
    c_free(option->long_name);
    c_free(option->description);

    c_free(option->value_name);
    c_free(option->default_value);

    c_free0(option, sizeof(struct c_command_line_option));
}

static void
c_command_line_option_reset(struct c_command_line_option *option) {
    c_free(option->value);
    option->value = NULL;

    option->is_set = false;
}

static int
c_command_line_option_set_value(struct c_command_line_option *option,
                                const char *value_name,
                                const char *default_value) {
    char *name, *value;

    name = c_strdup(value_name);
    if (!name)
        return -1;

    if (default_value) {
        value = c_strdup(default_value);
        if (!value) {
            c_free(name);
            return -1;
        }
    } else {
        value = NULL;
    }

    if (option->value_name)
        c_free(option->value_name);
    if (option->default_value)
        c_free(option->default_value);

    option->can_have_value = true;
    option->value_name = name;
    option->default_value = value;

    return 0;
}

static struct c_command_line_option *
c_command_line_get_option(const struct c_command_line *cmdline, const char *name) {
    struct c_command_line_option *option;

    if (c_hash_table_get(cmdline->option_table, name, (void **)&option) == 0)
        return NULL;

    return option;
}

static int
c_command_line_parse_option(const struct c_command_line *cmdline,
                            const char *name, char **argv, int argc, int i) {
    struct c_command_line_option *option;

    option = c_command_line_get_option(cmdline, name);
    if (!option) {
        c_set_error("unknown option '%s'", name);
        return -1;
    }

    if (option->can_have_value) {
        if (i == argc - 1) {
            c_set_error("missing value for option '%s'", name);
            return -1;
        }

        option->value = c_strdup(argv[i + 1]);
        if (!option->value)
            return -1;
        option->is_set = true;

        return 1;
    } else {
        option->is_set = true;
        return 0;
    }
}

static struct c_command_line_argument *
c_command_line_argument_new(const char *description, const char *value_name) {
    struct c_command_line_argument *argument;

    argument = c_malloc0(sizeof(struct c_command_line_argument));
    if (!argument)
        return NULL;

    argument->description = c_strdup(description);
    if (!argument->description)
        goto error;

    argument->value_name = c_strdup(value_name);
    if (!argument->value_name)
        goto error;

    return argument;

error:
    c_command_line_argument_delete(argument);
    return NULL;
}

static void
c_command_line_argument_delete(struct c_command_line_argument *argument) {
    if (!argument)
        return;

    c_command_line_argument_reset(argument);

    c_free(argument->description);
    c_free(argument->value_name);

    c_free0(argument, sizeof(struct c_command_line_argument));
}

static void
c_command_line_argument_reset(struct c_command_line_argument *argument) {
    c_free(argument->value);
    argument->value = NULL;

    if (argument->is_trailing) {
        for (size_t i = 0; i < argument->nb_trailing_values; i++)
            c_free(argument->trailing_values[i]);
        c_free(argument->trailing_values);

        argument->nb_trailing_values = 0;
        argument->trailing_values = NULL;
    }
}

int
c_command_line_add_argument(struct c_command_line *cmdline,
                            const char *description, const char *value_name) {
    struct c_command_line_argument *argument;

    if (cmdline->nb_arguments > 0
     && cmdline->arguments[cmdline->nb_arguments - 1]->is_trailing) {
        c_set_error("cannot add argument after trailing arguments");
        return -1;
    }

    argument = c_command_line_argument_new(description, value_name);
    if (!argument)
        return -1;

    if (c_command_line_register_argument(cmdline, argument) == -1) {
        c_command_line_argument_delete(argument);
        return -1;
    }

    return 0;
}

int
c_command_line_add_trailing_arguments(struct c_command_line *cmdline,
                                      const char *description,
                                      const char *value_name) {
    struct c_command_line_argument *argument;

    if (cmdline->nb_arguments > 0
     && cmdline->arguments[cmdline->nb_arguments - 1]->is_trailing) {
        c_set_error("trailing arguments already defined");
        return -1;
    }

    argument = c_command_line_argument_new(description, value_name);
    if (!argument)
        return -1;

    argument->is_trailing = true;

    if (c_command_line_register_argument(cmdline, argument) == -1) {
        c_command_line_argument_delete(argument);
        return -1;
    }

    return 0;
}
