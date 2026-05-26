# Copyright (c) The mldsa-native project authors
# SPDX-License-Identifier: Apache-2.0 OR ISC OR MIT

PLATFORM_PATH:=test/baremetal/platform/m55-an547

CROSS_PREFIX=arm-none-eabi-
CC=gcc

CFLAGS += \
	-O3 \
	-Wall -Wextra -Wshadow \
	-Wno-pedantic \
	-Wno-redundant-decls \
	-Wno-missing-prototypes \
	-fno-common \
	-ffunction-sections \
	-fdata-sections \
	--sysroot=$(SYSROOT) \
	-DDEVICE=an547 \
	-I$(M55_AN547_PATH) \
	-DARMCM55 \
	-DSEMIHOSTING

ARCH_FLAGS += \
	-march=armv8.1-m.main+mve.fp \
	-mcpu=cortex-m55 \
	-mthumb \
	-mfloat-abi=hard -mfpu=fpv4-sp-d16

CFLAGS += \
	$(ARCH_FLAGS) \
	--specs=nosys.specs

CFLAGS += $(CFLAGS_EXTRA)

LDSCRIPT = $(M55_AN547_PATH)/mps3.ld

LDFLAGS += \
	-Wl,--gc-sections \
	-Wl,--no-warn-rwx-segments \
	-L.

LDFLAGS += \
	--specs=nosys.specs \
	-Wl,--wrap=_open \
	-Wl,--wrap=_close \
	-Wl,--wrap=_read \
	-Wl,--wrap=_write \
	-Wl,--wrap=_fstat \
	-Wl,--wrap=_getpid \
	-Wl,--wrap=_isatty \
	-Wl,--wrap=_kill \
	-Wl,--wrap=_lseek \
	-Wl,--wrap=main \
	-ffreestanding \
	-T$(LDSCRIPT) \
	$(ARCH_FLAGS)

# Extra sources to be included in test binaries
EXTRA_SOURCES = $(wildcard $(M55_AN547_PATH)/*.c)
# The CMSIS files fail compilation if conversion warnings are enabled
EXTRA_SOURCES_CFLAGS = -Wno-conversion -Wno-sign-conversion

EXEC_WRAPPER := $(realpath $(PLATFORM_PATH)/exec_wrapper.py)
