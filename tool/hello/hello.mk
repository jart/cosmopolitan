#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#───vi: set et ft=make ts=8 tw=8 fenc=utf-8 :vi───────────────────────┘

PKGS += TOOL_HELLO

TOOL_HELLO_SRCS := $(wildcard tool/hello/*.c)
TOOL_HELLO_OBJS = $(TOOL_HELLO_SRCS:%.c=o/$(MODE)/%.o)
TOOL_HELLO_COMS = $(TOOL_HELLO_SRCS:%.c=o/$(MODE)/%.com)
TOOL_HELLO_BINS = $(TOOL_HELLO_COMS) $(TOOL_HELLO_COMS:%=%.dbg)

TOOL_HELLO_DIRECTDEPS =				\
	LIBC_CALLS				\
	LIBC_INTRIN				\
	LIBC_NEXGEN32E				\
	LIBC_RUNTIME				\
	LIBC_STR				\
	LIBC_SYSV

TOOL_HELLO_DEPS :=				\
	$(call uniq,$(foreach x,$(TOOL_HELLO_DIRECTDEPS),$($(x))))

o/$(MODE)/tool/hello/hello.pkg:			\
		$(TOOL_HELLO_OBJS)		\
		$(foreach x,$(TOOL_HELLO_DIRECTDEPS),$($(x)_A).pkg)

o/$(MODE)/tool/hello/%.com.dbg:			\
		$(TOOL_HELLO_DEPS)		\
		o/$(MODE)/tool/hello/%.o	\
		o/$(MODE)/tool/hello/hello.pkg	\
		$(CRT)				\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

$(TOOL_HELLO_OBJS):				\
		$(BUILD_FILES)			\
		tool/hello/hello.mk

.PHONY: o/$(MODE)/tool/hello
o/$(MODE)/tool/hello: $(TOOL_HELLO_BINS) $(TOOL_HELLO_CHECKS)
