# Copyright (c) The mlkem-native project authors
# Copyright (c) The mldsa-native project authors
# SPDX-License-Identifier: Apache-2.0 OR ISC OR MIT

.PHONY: build run clean
.DEFAULT_GOAL := all

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
# If you are not concerned about minimizing for a specific backend,
# you can just include _all_ source files into your build.
#
# In this example, we compile the individual mldsa-native source files directly.
# Alternatively, you can compile the 'monobuild' source file mldsa_native.c.
# See examples/monolithic_build for that.
MLD_SOURCE=$(wildcard				\
	mldsa_native/src/*.c			\
	mldsa_native/src/**/*.c			\
	mldsa_native/src/**/**/*.c		\
	mldsa_native/src/**/**/**/*.c)

INC=-Imldsa_native

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

ALL_SOURCE=$(MLD_SOURCE) $(RNG_SOURCE) $(APP_SOURCE)

BUILD_DIR=build
BIN=test_binary

#
# Configuration adjustments
#

# Pick prefix
CFLAGS += -DMLD_CONFIG_NAMESPACE_PREFIX=mldsa

BINARY_NAME_FULL_44=$(BUILD_DIR)/$(BIN)44
BINARY_NAME_FULL_65=$(BUILD_DIR)/$(BIN)65
BINARY_NAME_FULL_87=$(BUILD_DIR)/$(BIN)87
BINARIES_FULL=$(BINARY_NAME_FULL_44) $(BINARY_NAME_FULL_65) $(BINARY_NAME_FULL_87)

$(BINARY_NAME_FULL_44): CFLAGS += -DMLD_CONFIG_PARAMETER_SET=44
$(BINARY_NAME_FULL_65): CFLAGS += -DMLD_CONFIG_PARAMETER_SET=65
$(BINARY_NAME_FULL_87): CFLAGS += -DMLD_CONFIG_PARAMETER_SET=87

$(BINARIES_FULL): $(ALL_SOURCE)
	echo "$@"
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) $(INC) $^ -o $@

all: build

build: $(BINARIES_FULL)

run: $(BINARIES_FULL)
	$(EXEC_WRAPPER) ./$(BINARY_NAME_FULL_44)
	$(EXEC_WRAPPER) ./$(BINARY_NAME_FULL_65)
	$(EXEC_WRAPPER) ./$(BINARY_NAME_FULL_87)

clean:
	rm -rf $(BUILD_DIR)
