#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#── vi: set noet ft=make ts=8 sw=8 fenc=utf-8 :vi ────────────────────┘

PKGS += LIBC_TIME

LIBC_TIME_ARTIFACTS += LIBC_TIME_A
LIBC_TIME_ZONEINFOS = $(wildcard usr/share/zoneinfo/*)
LIBC_TIME = $(LIBC_TIME_A_DEPS) $(LIBC_TIME_A)
LIBC_TIME_A = o/$(MODE)/libc/time/time.a
LIBC_TIME_A_FILES := $(wildcard libc/time/struct/*) $(wildcard libc/time/*)
LIBC_TIME_A_HDRS := $(filter %.h,$(LIBC_TIME_A_FILES))
LIBC_TIME_A_SRCS_S = $(filter %.S,$(LIBC_TIME_A_FILES))
LIBC_TIME_A_SRCS_C = $(filter %.c,$(LIBC_TIME_A_FILES))

LIBC_TIME_A_SRCS =					\
	$(LIBC_TIME_A_SRCS_S)				\
	$(LIBC_TIME_A_SRCS_C)

LIBC_TIME_A_OBJS =					\
	$(LIBC_TIME_A_SRCS_S:%.S=o/$(MODE)/%.o)		\
	$(LIBC_TIME_A_SRCS_C:%.c=o/$(MODE)/%.o)		\
	$(LIBC_TIME_A_SRCS_C:%.c=o/$(MODE)/%.o)		\
	$(LIBC_TIME_ZONEINFOS:%=o/$(MODE)/%.zip.o)	\
	o/$(MODE)/usr/share/zoneinfo/.zip.o

LIBC_TIME_A_CHECKS =					\
	$(LIBC_TIME_A).pkg				\
	$(LIBC_TIME_A_HDRS:%=o/$(MODE)/%.ok)

LIBC_TIME_A_DIRECTDEPS =				\
	LIBC_CALLS					\
	LIBC_FMT					\
	LIBC_INTRIN					\
	LIBC_MEM					\
	LIBC_NEXGEN32E					\
	LIBC_NT_KERNEL32				\
	LIBC_RUNTIME					\
	LIBC_STDIO					\
	LIBC_STR					\
	LIBC_SYSV					\
	THIRD_PARTY_COMPILER_RT

LIBC_TIME_A_DEPS :=					\
	$(call uniq,$(foreach x,$(LIBC_TIME_A_DIRECTDEPS),$($(x))))

# offer assurances about the stack safety of cosmo libc
$(LIBC_TIME_A_OBJS): private COPTS += -Wframe-larger-than=4096 -Walloca-larger-than=4096

$(LIBC_TIME_A):	libc/time/				\
		$(LIBC_TIME_A).pkg			\
		$(LIBC_TIME_A_OBJS)

$(LIBC_TIME_A).pkg:					\
		$(LIBC_TIME_A_OBJS)			\
		$(foreach x,$(LIBC_TIME_A_DIRECTDEPS),$($(x)_A).pkg)

o/$(MODE)/libc/time/strftime.o: private			\
		CFLAGS +=				\
			-fno-jump-tables

o/$(MODE)/libc/time/localtime.o: private		\
		CFLAGS +=				\
			-fdata-sections			\
			-ffunction-sections

# we need -O3 because:
#   we're dividing by constants
o/$(MODE)/libc/time/iso8601.o				\
o/$(MODE)/libc/time/iso8601us.o: private		\
		CFLAGS +=				\
			-O3

o/$(MODE)/usr/share/zoneinfo/.zip.o:			\
	usr/share/zoneinfo

o/$(MODE)/libc/time/kmonthname.o: libc/time/kmonthname.S
	@$(COMPILE) -AOBJECTIFY.S $(OBJECTIFY.S) $(OUTPUT_OPTION) -c $<
o/$(MODE)/libc/time/kmonthnameshort.o: libc/time/kmonthnameshort.S
	@$(COMPILE) -AOBJECTIFY.S $(OBJECTIFY.S) $(OUTPUT_OPTION) -c $<
o/$(MODE)/libc/time/kweekdayname.o: libc/time/kweekdayname.S
	@$(COMPILE) -AOBJECTIFY.S $(OBJECTIFY.S) $(OUTPUT_OPTION) -c $<
o/$(MODE)/libc/time/kweekdaynameshort.o: libc/time/kweekdaynameshort.S
	@$(COMPILE) -AOBJECTIFY.S $(OBJECTIFY.S) $(OUTPUT_OPTION) -c $<

LIBC_TIME_LIBS = $(foreach x,$(LIBC_TIME_ARTIFACTS),$($(x)))
LIBC_TIME_SRCS = $(foreach x,$(LIBC_TIME_ARTIFACTS),$($(x)_SRCS))
LIBC_TIME_HDRS = $(foreach x,$(LIBC_TIME_ARTIFACTS),$($(x)_HDRS))
LIBC_TIME_CHECKS = $(foreach x,$(LIBC_TIME_ARTIFACTS),$($(x)_CHECKS))
LIBC_TIME_OBJS = $(foreach x,$(LIBC_TIME_ARTIFACTS),$($(x)_OBJS))
$(LIBC_TIME_OBJS): $(BUILD_FILES) libc/time/BUILD.mk

.PHONY: o/$(MODE)/libc/time
o/$(MODE)/libc/time: $(LIBC_TIME_CHECKS)
