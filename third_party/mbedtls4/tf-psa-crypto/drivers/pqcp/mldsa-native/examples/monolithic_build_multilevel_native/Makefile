# Copyright (c) The mlkem-native project authors
# Copyright (c) The mldsa-native project authors
# SPDX-License-Identifier: Apache-2.0 OR ISC OR MIT

.PHONY: build run clean
.DEFAULT_GOAL := all

CC  ?= gcc
AR  ?= ar

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
	-Wno-unused-function \
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

ifeq (,$(findstring $(CROSS_PREFIX),$(AR)))
AR  := $(CROSS_PREFIX)$(AR)
endif

# Part A:
#
# mldsa-native source and header files
#
# Here, the monolithic C file for mldsa-native is directly included in main.c,
# However, we still need to incldue the monolithic assembly file.
MLD_SOURCE_ASM = mldsa_native/mldsa_native.S

INC=-Imldsa_native/ -I./

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
APP_SOURCE=$(RNG_SOURCE) main.c

BUILD_DIR=build
BIN=test_binary

#
# Configuration adjustments
#

ASMFLAGS = -DMLD_CONFIG_MULTILEVEL_WITH_SHARED

BINARY_NAME_FULL=$(BUILD_DIR)/$(BIN)

MLD_OBJ_C=$(patsubst %,$(BUILD_DIR)/%.o,$(MLD_SOURCE_C))
MLD_OBJ_ASM=$(patsubst %,$(BUILD_DIR)/%.o,$(MLD_SOURCE_ASM))

Q ?= @

$(BUILD_DIR)/%.c.o: %.c
	$(Q)echo "CC  $^"
	$(Q)[ -d $(@D) ] || mkdir -p $(@D)
	$(Q)$(CC) -c $(CFLAGS) $(INC) $^ -o $@

$(BUILD_DIR)/%.S.o: %.S
	$(Q)echo "AS  $^"
	$(Q)[ -d $(@D) ] || mkdir -p $(@D)
	$(Q)$(CC) -c $(CFLAGS) $(ASMFLAGS) $(INC) $^ -o $@

$(BINARY_NAME_FULL): $(APP_SOURCE) $(MLD_OBJ_ASM)
	$(Q)echo "CC  $@"
	$(Q)[ -d $(@D) ] || mkdir -p $(@D)
	$(Q)$(CC) $(CFLAGS) $(INC) $^ -o $@
	$(Q)strip -S $@

all: build

build: $(BINARY_NAME_FULL)

run: $(BINARY_NAME_FULL)
	$(EXEC_WRAPPER) ./$(BINARY_NAME_FULL)

clean:
	rm -rf $(BUILD_DIR)
