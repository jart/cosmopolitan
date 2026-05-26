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

Q ?= @

# Part A:
#
# mldsa-native source and header files
#
# Here, we use just a single C and assembly unit.

MLD_SOURCE=mldsa_native/mldsa_native.c mldsa_native/mldsa_native.S

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
BIN44=test_binary_mldsa44
BIN65=test_binary_mldsa65
BIN87=test_binary_mldsa87
LIB44=libmldsa44.a
LIB65=libmldsa65.a
LIB87=libmldsa87.a

BIN44_FULL=$(BUILD_DIR)/$(BIN44)
BIN65_FULL=$(BUILD_DIR)/$(BIN65)
BIN87_FULL=$(BUILD_DIR)/$(BIN87)

LIB44_FULL=$(BUILD_DIR)/$(LIB44)
LIB65_FULL=$(BUILD_DIR)/$(LIB65)
LIB87_FULL=$(BUILD_DIR)/$(LIB87)

$(LIB44_FULL): $(MLD_SOURCE)
	$(Q)echo "$@"
	$(Q)[ -d $(@) ] || mkdir -p $(@D)
	$(Q)$(CC) -c $(CFLAGS) -DMLD_CONFIG_PARAMETER_SET=44 $(INC) mldsa_native/mldsa_native.c -o $(BUILD_DIR)/mldsa_native44.c.o
	$(Q)$(CC) -c $(CFLAGS) -DMLD_CONFIG_PARAMETER_SET=44 $(INC) mldsa_native/mldsa_native.S -o $(BUILD_DIR)/mldsa_native44.S.o
	$(Q)$(AR) rcs $@ $(BUILD_DIR)/mldsa_native44.c.o $(BUILD_DIR)/mldsa_native44.S.o
	$(Q)strip -S $@

$(LIB65_FULL): $(MLD_SOURCE)
	$(Q)echo "$@"
	$(Q)[ -d $(@) ] || mkdir -p $(@D)
	$(Q)$(CC) -c $(CFLAGS) -DMLD_CONFIG_PARAMETER_SET=65 $(INC) mldsa_native/mldsa_native.c -o $(BUILD_DIR)/mldsa_native65.c.o
	$(Q)$(CC) -c $(CFLAGS) -DMLD_CONFIG_PARAMETER_SET=65 $(INC) mldsa_native/mldsa_native.S -o $(BUILD_DIR)/mldsa_native65.S.o
	$(Q)$(AR) rcs $@ $(BUILD_DIR)/mldsa_native65.c.o $(BUILD_DIR)/mldsa_native65.S.o
	$(Q)strip -S $@

$(LIB87_FULL): $(MLD_SOURCE)
	$(Q)echo "$@"
	$(Q)[ -d $(@) ] || mkdir -p $(@D)
	$(Q)$(CC) -c $(CFLAGS) -DMLD_CONFIG_PARAMETER_SET=87 $(INC) mldsa_native/mldsa_native.c -o $(BUILD_DIR)/mldsa_native87.c.o
	$(Q)$(CC) -c $(CFLAGS) -DMLD_CONFIG_PARAMETER_SET=87 $(INC) mldsa_native/mldsa_native.S -o $(BUILD_DIR)/mldsa_native87.S.o
	$(Q)$(AR) rcs $@ $(BUILD_DIR)/mldsa_native87.c.o $(BUILD_DIR)/mldsa_native87.S.o
	$(Q)strip -S $@

$(BIN44_FULL): $(APP_SOURCE) $(LIB44_FULL)
	$(Q)echo "$@"
	$(Q)[ -d $(@) ] || mkdir -p $(@D)
	$(Q)$(CC) $(CFLAGS) -DMLD_CONFIG_PARAMETER_SET=44 $(INC) $^ -o $@

$(BIN65_FULL): $(APP_SOURCE) $(LIB65_FULL)
	$(Q)echo "$@"
	$(Q)[ -d $(@) ] || mkdir -p $(@D)
	$(Q)$(CC) $(CFLAGS) -DMLD_CONFIG_PARAMETER_SET=65 $(INC) $^ -o $@

$(BIN87_FULL): $(APP_SOURCE) $(LIB87_FULL)
	$(Q)echo "$@"
	$(Q)[ -d $(@) ] || mkdir -p $(@D)
	$(Q)$(CC) $(CFLAGS) -DMLD_CONFIG_PARAMETER_SET=87 $(INC) $^ -o $@

all: build

build: $(BIN44_FULL) $(BIN65_FULL) $(BIN87_FULL)

run: $(BIN44_FULL) $(BIN65_FULL) $(BIN87_FULL)
	$(Q)$(EXEC_WRAPPER) ./$(BIN44_FULL)
	$(Q)$(EXEC_WRAPPER) ./$(BIN65_FULL)
	$(Q)$(EXEC_WRAPPER) ./$(BIN87_FULL)

clean:
	rm -rf $(BUILD_DIR)
