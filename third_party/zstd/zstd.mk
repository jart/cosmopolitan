#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#───vi: set et ft=make ts=8 tw=8 fenc=utf-8 :vi───────────────────────┘

PKGS += THIRD_PARTY_ZSTD

THIRD_PARTY_ZSTD_ARTIFACTS += THIRD_PARTY_ZSTD_A
THIRD_PARTY_ZSTD = $(THIRD_PARTY_ZSTD_A_DEPS) $(THIRD_PARTY_ZSTD_A)
THIRD_PARTY_ZSTD_A = o/$(MODE)/third_party/zstd/zstd.a
THIRD_PARTY_ZSTD_A_FILES := $(shell find ./third_party/zstd/lib -type f) $(wildcard third_party/zstd/programs/*)
THIRD_PARTY_ZSTD_A_HDRS = $(filter %.h,$(THIRD_PARTY_ZSTD_A_FILES))
THIRD_PARTY_ZSTD_A_SRCS = $(filter %.c,$(THIRD_PARTY_ZSTD_A_FILES))
THIRD_PARTY_ZSTD_A_INCS = $(filter %.inc,$(THIRD_PARTY_ZSTD_A_FILES))
THIRD_PARTY_ZSTD_A_OBJS = $(THIRD_PARTY_ZSTD_A_SRCS:%.c=o/$(MODE)/%.o)

THIRD_PARTY_ZSTD_A_CHECKS =				\
	$(THIRD_PARTY_ZSTD_A).pkg			\
	$(THIRD_PARTY_ZSTD_A_HDRS:%=o/$(MODE)/%.ok)

THIRD_PARTY_ZSTD_A_DIRECTDEPS =				\
	LIBC_INTRIN					\
	LIBC_NEXGEN32E					\
	LIBC_STR					\
	LIBC_MEM					\
	LIBC_STDIO					\
	LIBC_TIME					\
	LIBC_CALLS							\
	LIBC_RUNTIME					\
	LIBC_FMT						\
	LIBC_LOG						\
	LIBC_SYSV

THIRD_PARTY_ZSTD_A_DEPS :=				\
	$(call uniq,$(foreach x,$(THIRD_PARTY_ZSTD_A_DIRECTDEPS),$($(x))))

$(THIRD_PARTY_ZSTD_A):					\
		third_party/zstd/			\
		$(THIRD_PARTY_ZSTD_A).pkg		\
		$(THIRD_PARTY_ZSTD_A_OBJS)

$(THIRD_PARTY_ZSTD_A).pkg:				\
		$(THIRD_PARTY_ZSTD_A_OBJS)		\
		$(foreach x,$(THIRD_PARTY_ZSTD_A_DIRECTDEPS),$($(x)_A).pkg)

$(THIRD_PARTY_ZSTD_A_CHECKS): private			\
		CFLAGS +=				\
			-Wframe-larger-than=262144	\
			-Wno-comment

$(THIRD_PARTY_ZSTD_A_OBJS): private			\
		CFLAGS +=				\
			-Wframe-larger-than=262144	\
			-Wno-comment

o/$(MODE)/third_party/zstd/zstd.com.dbg:			\
		$(THIRD_PARTY_ZSTD)			\
		o/$(MODE)/third_party/zstd/programs/zstdcli.o		\
		$(CRT)					\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)


THIRD_PARTY_ZSTD_BINS = $(THIRD_PARTY_ZSTD_COMS) $(THIRD_PARTY_ZSTD_COMS:%=%.dbg)
THIRD_PARTY_ZSTD_COMS = o/$(MODE)/third_party/zstd/zstd.com
THIRD_PARTY_ZSTD_LIBS = $(THIRD_PARTY_ZSTD_A)

THIRD_PARTY_ZSTD_CHECKS = $(foreach x,$(THIRD_PARTY_ZSTD_ARTIFACTS),$($(x)_CHECKS))
THIRD_PARTY_ZSTD_OBJS = $(foreach x,$(THIRD_PARTY_ZSTD_ARTIFACTS),$($(x)_OBJS))
$(THIRD_PARTY_ZSTD_OBJS): $(BUILD_FILES) third_party/zstd/zstd.mk

.PHONY: o/$(MODE)/third_party/zstd
o/$(MODE)/third_party/zstd:				\
		$(THIRD_PARTY_ZSTD_BINS)			\
		$(THIRD_PARTY_ZSTD_CHECKS)
