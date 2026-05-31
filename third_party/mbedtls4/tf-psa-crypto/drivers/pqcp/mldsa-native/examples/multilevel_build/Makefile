# Copyright (c) The mldsa-native project authors
# SPDX-License-Identifier: Apache-2.0 OR ISC OR MIT

.PHONY: build run clean mldsa44_objs mldsa65_objs mldsa87_objs mldsa_objs
.DEFAULT_GOAL := all

Q ?= @

CC  ?= gcc

# Adjust CFLAGS if needed
CFLAGS := \
	-Wall \
	-Wextra \
	-Werror=unused-result \
	-Wpedantic \
	-Werror \
	-Wmissing-prototypes \
	-Wshadow \
	-Wpointer-arith \
	-Wredundant-decls \
	-Wconversion \
	-Wsign-conversion \
	-Wno-long-long \
	-Wno-unknown-pragmas \
	-Wno-unused-command-line-argument \
	-O3 \
	-fomit-frame-pointer \
	-Imldsa_native/ \
	-DMLD_CONFIG_NAMESPACE_PREFIX=mldsa \
	-std=c99 \
	-pedantic \
	-MMD \
	$(CFLAGS)

# If you want to use the native backends, the compiler needs to know about
# the target architecture. Here, we import the default host detection from
# mldsa-native's tests, but you can write your own or specialize accordingly.
AUTO ?= 1
include auto.mk

# The following only concerns the cross-compilation tests.
# You can likely ignore the following for your application.
#
# Append cross-prefix for cross compilation
# When called from the root Makefile, CROSS_PREFIX has already been added here
ifeq (,$(findstring $(CROSS_PREFIX),$(CC)))
CC  := $(CROSS_PREFIX)$(CC)
endif

# Part A:
#
# mldsa-native source and header files
#
# In this example, we compile the individual mldsa-native source files directly.
# Alternatively, you can compile the 'monobuild' source file mldsa_native.c.
# See examples/monolithic_build for that.
MLD_SOURCE_ALL := $(wildcard \
	mldsa_native/src/*.c \
	mldsa_native/src/**/*.c \
	mldsa_native/src/**/**/*.c \
	mldsa_native/src/**/**/**/*.c)
MLD_SOURCE:=$(foreach S,$(MLD_SOURCE_ALL),\
  $(if $(findstring /native/,$S),,$S))

BUILD_DIR=build
MLDSA44_DIR = $(BUILD_DIR)/mldsa44
MLDSA65_DIR = $(BUILD_DIR)/mldsa65
MLDSA87_DIR = $(BUILD_DIR)/mldsa87

MLDSA44_OBJS=$(patsubst %,$(MLDSA44_DIR)/%.o,$(MLD_SOURCE))
MLDSA65_OBJS=$(patsubst %,$(MLDSA65_DIR)/%.o,$(MLD_SOURCE))
MLDSA87_OBJS=$(patsubst %,$(MLDSA87_DIR)/%.o,$(MLD_SOURCE))

$(MLDSA44_OBJS): $(MLDSA44_DIR)/%.c.o: %.c
	$(Q)[ -d $(@D) ] || mkdir -p $(@D)
	$(Q)$(CC)  -DMLD_CONFIG_MULTILEVEL_WITH_SHARED -DMLD_CONFIG_PARAMETER_SET=44 $(CFLAGS) -c $^ -o $@

$(MLDSA65_OBJS): $(MLDSA65_DIR)/%.c.o: %.c
	$(Q)[ -d $(@D) ] || mkdir -p $(@D)
	$(Q)$(CC) -DMLD_CONFIG_MULTILEVEL_NO_SHARED -DMLD_CONFIG_PARAMETER_SET=65 $(CFLAGS) -c $^ -o $@

$(MLDSA87_OBJS): $(MLDSA87_DIR)/%.c.o: %.c
	$(Q)[ -d $(@D) ] || mkdir -p $(@D)
	$(Q)$(CC) -DMLD_CONFIG_MULTILEVEL_NO_SHARED -DMLD_CONFIG_PARAMETER_SET=87 $(CFLAGS) -c $^ -o $@

mldsa44_objs: $(MLDSA44_OBJS)
mldsa65_objs: $(MLDSA65_OBJS)
mldsa87_objs: $(MLDSA87_OBJS)
mldsa_objs: mldsa44_objs mldsa65_objs mldsa87_objs

# Part B:
#
# Random number generator
#
# !!! WARNING !!!
#
# The randombytes() implementation used here is for TESTING ONLY.
# You MUST NOT use this implementation outside of testing.
#
# !!! WARNING !!!
RNG_SOURCE=$(wildcard test_only_rng/*.c)

# Part C:
#
# Your application source code
APP_SOURCE=$(wildcard *.c)

BIN=test_binary

BINARY_NAME_FULL=$(BUILD_DIR)/$(BIN)

$(BINARY_NAME_FULL): $(APP_SOURCE) $(RNG_SOURCE) $(MLDSA44_OBJS) $(MLDSA65_OBJS) $(MLDSA87_OBJS)
	echo "$@"
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) $^ -o $@

all: build

build: $(BINARY_NAME_FULL)

run: $(BINARY_NAME_FULL)
	$(EXEC_WRAPPER) ./$(BINARY_NAME_FULL)

clean:
	rm -rf $(BUILD_DIR)
