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

#ifndef LIBCORE_COMMAND_LINE_H
#define LIBCORE_COMMAND_LINE_H

#include <stdbool.h>

struct c_command_line *c_command_line_new(void);
void c_command_line_delete(struct c_command_line *);

int c_command_line_set_program_description(struct c_command_line *,
                                           const char *);
int c_command_line_set_trailing_text(struct c_command_line *, const char *);

int c_command_line_add_flag(struct c_command_line *,
                            const char *, const char *, const char *);
int c_command_line_add_option(struct c_command_line *,
                              const char *, const char *, const char *,
                              const char *, const char *);

int c_command_line_add_argument(struct c_command_line *,
                                const char *, const char *);
int c_command_line_add_trailing_arguments(struct c_command_line *,
                                          const char *, const char *);

int c_command_line_parse(struct c_command_line *, int, char **);

char *c_command_line_usage_string(const struct c_command_line *);
int c_command_line_usage_print(const struct c_command_line *, FILE *);

bool c_command_line_is_option_set(struct c_command_line *, const char *);
const char *c_command_line_option_value(struct c_command_line *,
                                            const char *);

const char *c_command_line_argument_value(struct c_command_line *, size_t);
size_t c_command_line_nb_trailing_arguments(struct c_command_line *);
const char **c_command_line_trailing_argument_values(struct c_command_line *);
const char *c_command_line_trailing_argument_value(struct c_command_line *,
                                                   size_t);

#endif
