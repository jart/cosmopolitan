#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#── vi: set noet ft=make ts=8 sw=8 fenc=utf-8 :vi ────────────────────┘

PKGS += TOOL_LAMBDA

TOOL_LAMBDA_SRCS := $(wildcard tool/lambda/*.c)

TOOL_LAMBDA_OBJS =						\
	$(TOOL_LAMBDA_SRCS:%.c=o/$(MODE)/%.o)

TOOL_LAMBDA_COMS :=						\
	$(TOOL_LAMBDA_SRCS:%.c=o/$(MODE)/%.com)

TOOL_LAMBDA_BINS =						\
	$(TOOL_LAMBDA_COMS)					\
	$(TOOL_LAMBDA_COMS:%=%.dbg)

TOOL_LAMBDA_DIRECTDEPS =					\
	LIBC_CALLS						\
	LIBC_FMT						\
	LIBC_INTRIN						\
	LIBC_LOG						\
	LIBC_MEM						\
	LIBC_NEXGEN32E						\
	LIBC_RUNTIME						\
	LIBC_STDIO						\
	LIBC_STR						\
	LIBC_SYSV						\
	LIBC_X							\
	THIRD_PARTY_GETOPT					\
	TOOL_LAMBDA_LIB

TOOL_LAMBDA_DEPS :=						\
	$(call uniq,$(foreach x,$(TOOL_LAMBDA_DIRECTDEPS),$($(x))))

o/$(MODE)/tool/lambda/lambda.pkg:				\
		$(TOOL_LAMBDA_OBJS)				\
		$(foreach x,$(TOOL_LAMBDA_DIRECTDEPS),$($(x)_A).pkg)

o/$(MODE)/tool/lambda/%.com.dbg:				\
		$(TOOL_LAMBDA_DEPS)				\
		o/$(MODE)/tool/lambda/%.o			\
		o/$(MODE)/tool/lambda/lambda.pkg		\
		$(CRT)						\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

o/$(MODE)/tool/lambda/tromp.o: private				\
		CFLAGS +=					\
			-w

$(TOOL_LAMBDA_OBJS):						\
		$(BUILD_FILES)					\
		tool/lambda/BUILD.mk

.PHONY: o/$(MODE)/tool/lambda
o/$(MODE)/tool/lambda: $(TOOL_LAMBDA_BINS) $(TOOL_LAMBDA_CHECKS)
