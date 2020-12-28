#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#───vi: set et ft=make ts=8 tw=8 fenc=utf-8 :vi───────────────────────┘

PKGS += LIBC_TIME

LIBC_TIME_ARTIFACTS += LIBC_TIME_A
LIBC_TIME_ZONEINFOS = $(wildcard usr/share/zoneinfo/*)
LIBC_TIME = $(LIBC_TIME_A_DEPS) $(LIBC_TIME_A)
LIBC_TIME_A = o/$(MODE)/libc/time/time.a
LIBC_TIME_A_FILES := $(wildcard libc/time/struct/*) $(wildcard libc/time/*)
LIBC_TIME_A_HDRS := $(filter %.h,$(LIBC_TIME_A_FILES))
LIBC_TIME_A_SRCS_S = $(filter %.S,$(LIBC_TIME_A_FILES))
LIBC_TIME_A_SRCS_C = $(filter %.c,$(LIBC_TIME_A_FILES))

LIBC_TIME_A_SRCS =				\
	$(LIBC_TIME_A_SRCS_S)			\
	$(LIBC_TIME_A_SRCS_C)

LIBC_TIME_A_OBJS =				\
	$(LIBC_TIME_A_SRCS:%=o/$(MODE)/%.zip.o)	\
	$(LIBC_TIME_A_SRCS_S:%.S=o/$(MODE)/%.o)	\
	$(LIBC_TIME_A_SRCS_C:%.c=o/$(MODE)/%.o)	\
	$(LIBC_TIME_A_SRCS_C:%.c=o/$(MODE)/%.o)	\
	$(LIBC_TIME_ZONEINFOS:%=o/$(MODE)/%.zip.o)

LIBC_TIME_A_CHECKS =				\
	$(LIBC_TIME_A).pkg			\
	$(LIBC_TIME_A_HDRS:%=o/$(MODE)/%.ok)

LIBC_TIME_A_DIRECTDEPS =			\
	LIBC_CALLS				\
	LIBC_FMT				\
	LIBC_MEM				\
	LIBC_NEXGEN32E				\
	LIBC_NT_KERNEL32			\
	LIBC_NT_NTDLL				\
	LIBC_RUNTIME				\
	LIBC_STR				\
	LIBC_STUBS				\
	LIBC_SYSV				\
	LIBC_SYSV_CALLS				\
	LIBC_ZIPOS

LIBC_TIME_A_DEPS :=				\
	$(call uniq,$(foreach x,$(LIBC_TIME_A_DIRECTDEPS),$($(x))))

$(LIBC_TIME_A):	libc/time/			\
		$(LIBC_TIME_A).pkg		\
		$(LIBC_TIME_A_OBJS)

$(LIBC_TIME_A).pkg:				\
		$(LIBC_TIME_A_OBJS)		\
		$(foreach x,$(LIBC_TIME_A_DIRECTDEPS),$($(x)_A).pkg)

o/$(MODE)/libc/time/localtime.o:		\
		OVERRIDE_CFLAGS +=		\
			$(OLD_CODE)

o/$(MODE)/libc/time/strftime.o:			\
		OVERRIDE_CFLAGS +=		\
			-fno-jump-tables

o/$(MODE)/libc/time/localtime.o			\
o/$(MODE)/libc/time/strftime.o:			\
		OVERRIDE_CFLAGS +=		\
			-fdata-sections		\
			-ffunction-sections

o/$(MODE)/libc/time/now.o:			\
		OVERRIDE_CFLAGS +=		\
			-O3

LIBC_TIME_LIBS = $(foreach x,$(LIBC_TIME_ARTIFACTS),$($(x)))
LIBC_TIME_SRCS = $(foreach x,$(LIBC_TIME_ARTIFACTS),$($(x)_SRCS))
LIBC_TIME_HDRS = $(foreach x,$(LIBC_TIME_ARTIFACTS),$($(x)_HDRS))
LIBC_TIME_CHECKS = $(foreach x,$(LIBC_TIME_ARTIFACTS),$($(x)_CHECKS))
LIBC_TIME_OBJS = $(foreach x,$(LIBC_TIME_ARTIFACTS),$($(x)_OBJS))
$(LIBC_TIME_OBJS): $(BUILD_FILES) libc/time/time.mk

.PHONY: o/$(MODE)/libc/time
o/$(MODE)/libc/time: $(LIBC_TIME_CHECKS)
