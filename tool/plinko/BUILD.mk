#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#── vi: set noet ft=make ts=8 sw=8 fenc=utf-8 :vi ────────────────────┘

ifeq ($(ARCH), x86_64)

PKGS += TOOL_PLINKO

TOOL_PLINKO_SRCS := $(wildcard tool/plinko/*.c)

TOOL_PLINKO_OBJS =						\
	$(TOOL_PLINKO_SRCS:%.c=o/$(MODE)/%.o)

TOOL_PLINKO_COMS =						\
	$(TOOL_PLINKO_SRCS:%.c=o/$(MODE)/%)

TOOL_PLINKO_BINS =						\
	$(TOOL_PLINKO_COMS)					\
	$(TOOL_PLINKO_COMS:%=%.dbg)

TOOL_PLINKO_DIRECTDEPS =					\
	LIBC_INTRIN						\
	LIBC_LOG						\
	LIBC_MEM						\
	LIBC_CALLS						\
	LIBC_RUNTIME						\
	LIBC_SYSV						\
	LIBC_STDIO						\
	LIBC_X							\
	LIBC_NEXGEN32E						\
	TOOL_PLINKO_LIB

TOOL_PLINKO_DEPS :=						\
	$(call uniq,$(foreach x,$(TOOL_PLINKO_DIRECTDEPS),$($(x))))

o/$(MODE)/tool/plinko/plinko.pkg:				\
		$(TOOL_PLINKO_OBJS)				\
		$(foreach x,$(TOOL_PLINKO_DIRECTDEPS),$($(x)_A).pkg)

o/$(MODE)/tool/plinko/%.dbg:					\
		$(TOOL_PLINKO_DEPS)				\
		o/$(MODE)/tool/plinko/%.o			\
		o/$(MODE)/tool/plinko/plinko.pkg		\
		o/$(MODE)/tool/plinko/lib/library.lisp.zip.o	\
		$(CRT)						\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

o/$(MODE)/tool/plinko/plinko.zip.o:				\
		o/$(MODE)/tool/plinko/plinko

o/$(MODE)/tool/plinko/plinko.zip.o				\
o/$(MODE)/tool/plinko/lib/library.lisp.zip.o			\
o/$(MODE)/tool/plinko/lib/binarytrees.lisp.zip.o		\
o/$(MODE)/tool/plinko/lib/algebra.lisp.zip.o			\
o/$(MODE)/tool/plinko/lib/infix.lisp.zip.o			\
o/$(MODE)/tool/plinko/lib/ok.lisp.zip.o:			\
		ZIPOBJ_FLAGS +=					\
			-B

endif

.PHONY: o/$(MODE)/tool/plinko
o/$(MODE)/tool/plinko: $(TOOL_PLINKO_BINS) $(TOOL_PLINKO_CHECKS)
