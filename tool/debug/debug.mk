#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#───vi: set et ft=make ts=8 tw=8 fenc=utf-8 :vi───────────────────────┘

PKGS += TOOL_DEBUG

TOOL_DEBUG_SRCS := $(wildcard tool/debug/*.c)
TOOL_DEBUG_OBJS = $(TOOL_DEBUG_SRCS:%.c=o/$(MODE)/%.o)
TOOL_DEBUG_COMS = $(TOOL_DEBUG_OBJS:%.o=%.com)

TOOL_DEBUG_DEPS := $(call uniq,			\
	$(LIBC_STR)				\
	$(LIBC_RUNTIME)				\
	$(LIBC_STDIO)				\
	$(LIBC_X)				\
	$(LIBC_LOG))

TOOL_DEBUG_BINS  =				\
	$(TOOL_DEBUG_COMS)			\
	$(TOOL_DEBUG_COMS:%=%.dbg)

o/$(MODE)/tool/debug/%.com.dbg:			\
		$(TOOL_DEBUG_DEPS)		\
		o/$(MODE)/tool/debug/%.o	\
		$(CRT)				\
		$(APE)
	@$(APELINK)

$(TOOL_DEBUG_OBJS):				\
		$(BUILD_FILES)			\
		tool/debug/debug.mk

.PHONY: o/$(MODE)/tool/debug
o/$(MODE)/tool/debug: $(TOOL_DEBUG_BINS) $(TOOL_DEBUG_CHECKS)
