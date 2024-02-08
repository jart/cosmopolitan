#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#── vi: set noet ft=make ts=8 sw=8 fenc=utf-8 :vi ────────────────────┘

PKGS += THIRD_PARTY_AWK

THIRD_PARTY_AWK_ARTIFACTS += THIRD_PARTY_AWK_A
THIRD_PARTY_AWK = $(THIRD_PARTY_AWK_A_DEPS) $(THIRD_PARTY_AWK_A)
THIRD_PARTY_AWK_A = o/$(MODE)/third_party/awk/awk.a
THIRD_PARTY_AWK_FILES := $(wildcard third_party/awk/*)
THIRD_PARTY_AWK_HDRS = $(filter %.h,$(THIRD_PARTY_AWK_FILES))
THIRD_PARTY_AWK_INCS = $(filter %.inc,$(THIRD_PARTY_AWK_FILES))
THIRD_PARTY_AWK_SRCS = $(filter %.c,$(THIRD_PARTY_AWK_FILES))
THIRD_PARTY_AWK_OBJS = $(THIRD_PARTY_AWK_SRCS:%.c=o/$(MODE)/%.o)

THIRD_PARTY_AWK_A_DIRECTDEPS =				\
	LIBC_CALLS					\
	LIBC_FMT					\
	LIBC_INTRIN					\
	LIBC_MEM					\
	LIBC_NEXGEN32E					\
	LIBC_PROC					\
	LIBC_RUNTIME					\
	LIBC_STDIO					\
	LIBC_STR					\
	LIBC_SYSV					\
	LIBC_TINYMATH					\
	TOOL_ARGS					\
	THIRD_PARTY_GDTOA

THIRD_PARTY_AWK_A_DEPS :=				\
	$(call uniq,$(foreach x,$(THIRD_PARTY_AWK_A_DIRECTDEPS),$($(x))))

THIRD_PARTY_AWK_CHECKS =				\
	$(THIRD_PARTY_AWK_A).pkg			\
	$(THIRD_PARTY_AWK_HDRS:%=o/$(MODE)/%.ok)

$(THIRD_PARTY_AWK_A):					\
		third_party/awk/			\
		$(THIRD_PARTY_AWK_A).pkg		\
		$(THIRD_PARTY_AWK_OBJS)

$(THIRD_PARTY_AWK_A).pkg:				\
		$(THIRD_PARTY_AWK_OBJS)		\
		$(foreach x,$(THIRD_PARTY_AWK_A_DIRECTDEPS),$($(x)_A).pkg)

o/$(MODE)/third_party/awk/awk.com.dbg:			\
		$(THIRD_PARTY_AWK)			\
		o/$(MODE)/third_party/awk/cmd.o		\
		o/$(MODE)/third_party/awk/README.zip.o	\
		$(CRT)					\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

o/$(MODE)/third_party/awk/README.zip.o:			\
		ZIPOBJ_FLAGS +=				\
			-B

THIRD_PARTY_AWK_BINS = $(THIRD_PARTY_AWK_COMS) $(THIRD_PARTY_AWK_COMS:%=%.dbg)
THIRD_PARTY_AWK_COMS = o/$(MODE)/third_party/awk/awk.com
THIRD_PARTY_AWK_LIBS = $(THIRD_PARTY_AWK_A)
$(THIRD_PARTY_AWK_OBJS): $(BUILD_FILES) third_party/awk/BUILD.mk

.PHONY: o/$(MODE)/third_party/awk
o/$(MODE)/third_party/awk:				\
		$(THIRD_PARTY_AWK_BINS)			\
		$(THIRD_PARTY_AWK_CHECKS)
