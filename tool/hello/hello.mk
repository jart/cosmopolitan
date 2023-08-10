#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#───vi: set et ft=make ts=8 tw=8 fenc=utf-8 :vi───────────────────────┘

PKGS += TOOL_HELLO

TOOL_HELLO_FILES := $(wildcard tool/hello/*)
TOOL_HELLO_HDRS = $(filter %.h,$(TOOL_HELLO_FILES))
TOOL_HELLO_SRCS_C = $(filter %.c,$(TOOL_HELLO_FILES))
TOOL_HELLO_SRCS_S = $(filter %.S,$(TOOL_HELLO_FILES))
TOOL_HELLO_SRCS = $(TOOL_HELLO_SRCS_C) $(TOOL_HELLO_SRCS_S)
TOOL_HELLO_OBJS = $(TOOL_HELLO_SRCS_C:%.c=o/$(MODE)/%.o) $(TOOL_HELLO_SRCS_S:%.S=o/$(MODE)/%.o)
TOOL_HELLO_BINS = o/$(MODE)/tool/hello/hello.com.dbg

o/$(MODE)/tool/hello/hello.com.dbg:			\
		o/$(MODE)/tool/hello/systemcall.o	\
		o/$(MODE)/tool/hello/hello.o		\
		o/$(MODE)/tool/hello/start.o
	@$(COMPILE) -ALINK.elf $(LINK) $(LINKARGS) $(OUTPUT_OPTION) -q -zmax-page-size=4096

o/$(MODE)/tool/hello/hello.com:				\
		o/$(MODE)/tool/hello/hello.com.dbg	\
		o/$(MODE)/tool/build/elf2pe.com
	o/$(MODE)/tool/build/elf2pe.com -o $@ $<

$(TOOL_HELLO_OBJS): tool/hello/hello.mk

.PHONY: o/$(MODE)/tool/hello
o/$(MODE)/tool/hello: $(TOOL_HELLO_BINS)
