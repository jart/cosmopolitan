#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#───vi: set et ft=make ts=8 tw=8 fenc=utf-8 :vi───────────────────────┘

PKGS += TOOL_APE

TOOL_APE_ARTIFACTS += TOOL_APE_A
TOOL_APE = $(TOOL_APE_DEPS) $(TOOL_APE_A)
TOOL_APE_A = o/$(MODE)/tool/ape/ape.a
TOOL_APE_FILES := $(wildcard tool/ape/*)
TOOL_APE_HDRS = $(filter %.h,$(TOOL_APE_FILES))
TOOL_APE_INCS = $(filter %.inc,$(TOOL_APE_FILES))
TOOL_APE_SRCS = $(filter %.c,$(TOOL_APE_FILES))
TOOL_APE_OBJS = $(TOOL_APE_SRCS:%.c=o/$(MODE)/%.o)

TOOL_APE_DIRECTDEPS =				\
	LIBC_CALLS				\
	LIBC_DNS				\
	LIBC_FMT				\
	LIBC_INTRIN				\
	LIBC_LOG				\
	LIBC_MEM				\
	LIBC_NEXGEN32E				\
	LIBC_RUNTIME				\
	LIBC_SOCK				\
	LIBC_STDIO				\
	LIBC_STR				\
	LIBC_SYSV				\
	LIBC_TIME				\
	LIBC_X					\
	LIBC_ZIPOS				\
	NET_HTTP				\
	NET_HTTPS				\
	THIRD_PARTY_GETOPT			\
	THIRD_PARTY_MBEDTLS

TOOL_APE_DEPS :=				\
	$(call uniq,$(foreach x,$(TOOL_APE_DIRECTDEPS),$($(x))))

TOOL_APE_CHECKS =				\
	$(TOOL_APE_A).pkg			\
	$(TOOL_APE_HDRS:%=o/$(MODE)/%.ok)

$(TOOL_APE_A):					\
		tool/ape/			\
		$(TOOL_APE_A).pkg		\
		$(TOOL_APE_OBJS)

$(TOOL_APE_A).pkg:				\
		$(TOOL_APE_OBJS)		\
		$(foreach x,$(TOOL_APE_DIRECTDEPS),$($(x)_A).pkg)

o/$(MODE)/tool/ape/ape.com.dbg:			\
		$(TOOL_APE)			\
		o/$(MODE)/tool/ape/ape.o	\
		$(CRT)				\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

TOOL_APE_LIBS = $(TOOL_APE_A)
TOOL_APE_BINS = $(TOOL_APE_COMS) $(TOOL_APE_COMS:%=%.dbg)
TOOL_APE_COMS = o/$(MODE)/tool/ape/ape.com
$(TOOL_APE_OBJS): $(BUILD_FILES) tool/ape/ape.mk

.PHONY: o/$(MODE)/tool/ape
o/$(MODE)/tool/ape:				\
		$(TOOL_APE_BINS)		\
		$(TOOL_APE_CHECKS)
