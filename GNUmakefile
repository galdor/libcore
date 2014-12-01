# Common
prefix= /usr/local
libdir= $(prefix)/lib
incdir= $(prefix)/include
bindir= $(prefix)/bin

CC=   clang

CFLAGS+= -std=c99
CFLAGS+= -Wall -Wextra -Werror -Wsign-conversion
CFLAGS+= -Wno-unused-parameter -Wno-unused-function

LDFLAGS=

PANDOC_OPTS= -s --toc --email-obfuscation=none

# Platform specific
platform= $(shell uname -s)

ifeq ($(platform), Linux)
	CFLAGS+= -DC_PLATFORM_LINUX
	CFLAGS+= -D_POSIX_C_SOURCE=200809L
endif

# Debug
debug=0
ifeq ($(debug), 1)
	CFLAGS+= -g -ggdb
else
	CFLAGS+= -O2
endif

# Coverage
coverage?= 0
ifeq ($(coverage), 1)
	CC= gcc
	CFLAGS+= -fprofile-arcs -ftest-coverage
	LDFLAGS+= --coverage
endif

# Target: libcore
libcore_LIB= libcore.a
libcore_SRC= $(wildcard src/*.c)
libcore_INC= $(wildcard src/*.h)
libcore_MAININC= src/core.h
libcore_PRIVINC= $(libcore_MAININC) src/internal.h
libcore_PUBINC= $(filter-out $(libcore_PRIVINC),$(libcore_INC))
libcore_OBJ= $(subst .c,.o,$(libcore_SRC))

$(libcore_LIB): CFLAGS+=

# Target: tests
tests_SRC= $(wildcard tests/*.c)
tests_OBJ= $(subst .c,.o,$(tests_SRC))
tests_BIN= $(subst .o,,$(tests_OBJ))

$(tests_BIN): CFLAGS+= -Isrc
$(tests_BIN): LDFLAGS+= -L.
$(tests_BIN): LDLIBS+= -lutest -lcore

# Target: examples
examples_SRC= $(wildcard examples/*.c)
examples_OBJ= $(subst .c,.o,$(examples_SRC))
examples_BIN= $(subst .o,,$(examples_OBJ))

$(examples_BIN): CFLAGS+= -Isrc
$(examples_BIN): LDFLAGS+= -L.
$(examples_BIN): LDLIBS+= -lcore

# Target: doc
doc_SRC= $(wildcard doc/*.mkd)
doc_HTML= $(subst .mkd,.html,$(doc_SRC))

# Rules
all: lib tests examples doc

lib: $(libcore_LIB)

tests: lib $(tests_BIN)

examples: lib $(examples_BIN)

doc: $(doc_HTML)

$(libcore_LIB): $(libcore_OBJ)
	$(AR) cr $@ $(libcore_OBJ)

$(tests_OBJ): $(libcore_LIB) $(libcore_INC)
tests/%: tests/%.o
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)

$(examples_OBJ): $(libcore_LIB) $(libcore_INC)
examples/%: examples/%.o
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)

doc/%.html: doc/%.mkd
	pandoc $(PANDOC_OPTS) -t html5 -o $@ $<

clean:
	$(RM) $(libcore_LIB) $(wildcard src/*.o)
	$(RM) $(tests_BIN) $(wildcard tests/*.o)
	$(RM) $(examples_BIN) $(wildcard examples/*.o)
	$(RM) $(wildcard **/*.gc??)
	$(RM) -r coverage
	$(RM) -r $(doc_HTML)

coverage:
	lcov -o /tmp/libcore.info -c -d . -b .
	genhtml -o coverage -t libcore /tmp/libcore.info
	rm /tmp/libcore.info

install: lib
	mkdir -p $(libdir)
	install -m 644 $(libcore_LIB) $(libdir)
	mkdir -p $(incdir)/core
	rm -f $(incdir)/core/*
	install -m 644 $(libcore_MAININC) $(incdir)
	install -m 644 $(libcore_PUBINC) $(incdir)/core
	mkdir -p $(bindir)

uninstall:
	$(RM) $(addprefix $(libdir)/,$(libcore_LIB))
	$(RM) -r $(incdir)/core

tags:
	ctags -o .tags $(wildcard src/*.[hc])

.PHONY: all lib tests examples doc clean coverage install uninstall tags
