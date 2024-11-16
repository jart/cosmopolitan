#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#── vi: set noet ft=make ts=8 sw=8 fenc=utf-8 :vi ────────────────────┘

PKGS += LIBC_SYSTEM

LIBC_SYSTEM_ARTIFACTS += LIBC_SYSTEM_A
LIBC_SYSTEM = $(LIBC_SYSTEM_A_DEPS) $(LIBC_SYSTEM_A)
LIBC_SYSTEM_A = o/$(MODE)/libc/system/system.a
LIBC_SYSTEM_A_FILES := $(wildcard libc/system/*)
LIBC_SYSTEM_A_HDRS = $(filter %.h,$(LIBC_SYSTEM_A_FILES))
LIBC_SYSTEM_A_INCS = $(filter %.inc,$(LIBC_SYSTEM_A_FILES))
LIBC_SYSTEM_A_SRCS_S = $(filter %.S,$(LIBC_SYSTEM_A_FILES))
LIBC_SYSTEM_A_SRCS_C = $(filter %.c,$(LIBC_SYSTEM_A_FILES))

LIBC_SYSTEM_A_SRCS =					\
	$(LIBC_SYSTEM_A_SRCS_S)				\
	$(LIBC_SYSTEM_A_SRCS_C)

LIBC_SYSTEM_A_OBJS =					\
	$(LIBC_SYSTEM_A_SRCS_S:%.S=o/$(MODE)/%.o)	\
	$(LIBC_SYSTEM_A_SRCS_C:%.c=o/$(MODE)/%.o)

LIBC_SYSTEM_A_CHECKS =					\
	$(LIBC_SYSTEM_A).pkg				\
	$(LIBC_SYSTEM_A_HDRS:%=o/$(MODE)/%.ok)

LIBC_SYSTEM_A_DIRECTDEPS =				\
	LIBC_CALLS					\
	LIBC_FMT					\
	LIBC_INTRIN					\
	LIBC_NEXGEN32E					\
	LIBC_PROC					\
	LIBC_RUNTIME					\
	LIBC_STDIO					\
	LIBC_STR					\
	LIBC_SYSV					\
	THIRD_PARTY_MUSL				\
	THIRD_PARTY_SED					\
	THIRD_PARTY_TR					\

LIBC_SYSTEM_A_DEPS :=					\
	$(call uniq,$(foreach x,$(LIBC_SYSTEM_A_DIRECTDEPS),$($(x))))

$(LIBC_SYSTEM_A):libc/system/				\
		$(LIBC_SYSTEM_A).pkg			\
		$(LIBC_SYSTEM_A_OBJS)

$(LIBC_SYSTEM_A).pkg:					\
		$(LIBC_SYSTEM_A_OBJS)			\
		$(foreach x,$(LIBC_SYSTEM_A_DIRECTDEPS),$($(x)_A).pkg)

# offer assurances about the stack safety of cosmo libc
$(LIBC_SYSTEM_A_OBJS): private COPTS += -Wframe-larger-than=4096 -Walloca-larger-than=4096

$(LIBC_SYSTEM_A_OBJS): private				\
		CFLAGS +=				\
			-fno-sanitize=all		\
			-Wframe-larger-than=4096	\
			-Walloca-larger-than=4096

o/$(MODE)/libc/system/fputc.o: private			\
		CFLAGS +=				\
			-O3

o//libc/system/appendw.o: private			\
		CFLAGS +=				\
			-Os

o/$(MODE)/libc/system/dirstream.o			\
o/$(MODE)/libc/system/mt19937.o: private			\
		CFLAGS +=				\
			-ffunction-sections

LIBC_SYSTEM_LIBS = $(foreach x,$(LIBC_SYSTEM_ARTIFACTS),$($(x)))
LIBC_SYSTEM_SRCS = $(foreach x,$(LIBC_SYSTEM_ARTIFACTS),$($(x)_SRCS))
LIBC_SYSTEM_HDRS = $(foreach x,$(LIBC_SYSTEM_ARTIFACTS),$($(x)_HDRS))
LIBC_SYSTEM_INCS = $(foreach x,$(LIBC_SYSTEM_ARTIFACTS),$($(x)_INCS))
LIBC_SYSTEM_CHECKS = $(foreach x,$(LIBC_SYSTEM_ARTIFACTS),$($(x)_CHECKS))
LIBC_SYSTEM_OBJS = $(foreach x,$(LIBC_SYSTEM_ARTIFACTS),$($(x)_OBJS))
$(LIBC_SYSTEM_OBJS): $(BUILD_FILES) libc/system/BUILD.mk

.PHONY: o/$(MODE)/libc/system
o/$(MODE)/libc/system: $(LIBC_SYSTEM_CHECKS)
