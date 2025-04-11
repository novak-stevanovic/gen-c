ifndef LTYPE
    LTYPE = dynamic
endif

# ifneq ($(LTYPE),static)
# ifneq ($(LTYPE),dynamic)
# 	$(error LTYPE invalid. USAGE: make)
# endif
# endif

ifneq ($(LTYPE),dynamic)
    ifneq ($(LTYPE),static)
        $(error Invalid LTYPE. USAGE: make [TARGET] [LTYPE=dynamic/static])
    endif
endif

# -----------------------------------------------------------------------------
# Settings
# -----------------------------------------------------------------------------

CC = gcc

C_SRC = $(shell find src -name "*.c")
C_OBJ = $(patsubst src/%.c,build/%.o,$(C_SRC))

LIB_NAME = genc
STATIC_LIB_FILE = lib$(LIB_NAME).a
DYNAMIC_LIB_FILE = lib$(LIB_NAME).so

# ---------------------------------------------------------
# pkgconfig flags
# ---------------------------------------------------------

PKG_DEPS = $(shell PKG_CONFIG_PATH=$(PWD):$PKG_CONFIG_PATH \
pkg-config --print-requires $(LIB_NAME))

PKG_DEPS_CFLAGS = $(foreach dep, $(PKG_DEPS), \
$(shell PKG_CONFIG_PATH=$(PWD):$PKG_CONFIG_PATH \
pkg-config --cflags-only-I $(dep)))

PKG_DEPS_LIBS = $(foreach dep, $(PKG_DEPS), \
$(shell PKG_CONFIG_PATH=$(PWD):$PKG_CONFIG_PATH \
pkg-config --libs $(dep)))

# ---------------------------------------------------------
# Base flags
# ---------------------------------------------------------

BASE_CFLAGS_DEBUG = -g
BASE_CFLAGS_OPTIMIZATION = -O0
BASE_CFLAGS_WARN = -Wall
BASE_CFLAGS_MAKE = -MMD -MP
BASE_CFLAGS_INCLUDE = -Iinclude

BASE_CFLAGS = -c -fPIC $(BASE_CFLAGS_INCLUDE) $(BASE_CFLAGS_MAKE) \
$(BASE_CFLAGS_WARN) $(BASE_CFLAGS_DEBUG) $(BASE_CFLAGS_OPTIMIZATION)

BASE_CFLAGS += $(PKG_DEPS_CFLAGS)

# ---------------------------------------------------------
# C source flags
# ---------------------------------------------------------

SRC_CFLAGS = $(BASE_CFLAGS)

# ---------------------------------------------------------
# Test flags
# ---------------------------------------------------------

TEST_CFLAGS = $(BASE_CFLAGS)

TEST_LFLAGS = -L. -l$(LIB_NAME)

ifeq ($(LTYPE),static)
LIB_FILE = $(STATIC_LIB_FILE)
LIB_MAKE_COMMAND = ar rcs $(LIB_FILE) $(C_OBJ)
else 
LIB_FILE = $(DYNAMIC_LIB_FILE)
TEST_LFLAGS += -Wl,-rpath,.
LIB_MAKE_COMMAND = gcc -shared $(C_OBJ) -o $(LIB_FILE)
endif

TEST_LFLAGS += $(PKG_DEPS_LIBS)

# ---------------------------------------------------------
# Install
# ---------------------------------------------------------

# To change the prefix, a change in the .pc file is needed as well.
INSTALL_PREFIX = /usr/local

PKGCONF_FILE = $(LIB_NAME).pc
PKGCONF_PREFIX = /usr/local/lib/pkgconfig

# -----------------------------------------------------------------------------
# Rules
# -----------------------------------------------------------------------------

.PHONY: all clean install uninstall dirs

all: dirs $(LIB_FILE)

$(LIB_FILE): $(C_OBJ)
	$(LIB_MAKE_COMMAND)

$(C_OBJ): build/%.o: src/%.c
	@mkdir -p $(dir $@)
	$(CC) $(SRC_CFLAGS) $< -o $@

# test -----------------------------------------------------

test: dirs $(C_OBJ) build/tests.o $(LIB_FILE)
	gcc build/tests.o -o test $(TEST_LFLAGS)

build/tests.o: tests.c
	gcc $(TEST_CFLAGS) tests.c -o build/tests.o

# dirs -----------------------------------------------------

dirs: | build

build:
	mkdir -p build/

# install --------------------------------------------------

install:
	@mkdir -p $(INSTALL_PREFIX)/lib
	cp $(LIB_FILE) $(INSTALL_PREFIX)/lib

	@mkdir -p $(INSTALL_PREFIX)/include/$(LIB_NAME)
	cp -r include/* $(INSTALL_PREFIX)/include/$(LIB_NAME)

	@mkdir -p $(PKGCONF_PREFIX)
	cp $(PKGCONF_FILE) $(PKGCONF_PREFIX)


uninstall:
	rm -f $(INSTALL_PREFIX)/lib/$(LIB_FILE)
	rm -rf $(INSTALL_PREFIX)/include/$(LIB_NAME)
	rm -f $(INSTALL_PREFIX)/lib/pkgconf/$(PKGCONF_FILE)

# clean ----------------------------------------------------

clean:
	rm -rf build
	rm -f $(STATIC_LIB_FILE)
	rm -f $(DYNAMIC_LIB_FILE)
	rm -f test
	rm -f compile_commands.json

# ----------------------------------------------------------

-include $(wildcard build/dependencies/*.d)
