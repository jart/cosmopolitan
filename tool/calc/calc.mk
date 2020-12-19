#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#───vi: set et ft=make ts=8 tw=8 fenc=utf-8 :vi───────────────────────┘

PKGS += TOOL_CALC

TOOL_CALC = $(TOOL_LIB_A_DEPS) $(TOOL_LIB_A)
TOOL_CALC_A = o/$(MODE)/tool/calc/calc.a
TOOL_CALC_FILES := $(wildcard tool/calc/*)
TOOL_CALC_COMS = $(TOOL_CALC_OBJS:%.o=%.com)
TOOL_CALC_SRCS = $(filter %.c,$(TOOL_CALC_FILES))
TOOL_CALC_HDRS = $(filter %.h,$(TOOL_CALC_FILES))

TOOL_CALC_OBJS =				\
	$(TOOL_CALC_SRCS:%=o/$(MODE)/%.zip.o)	\
	$(TOOL_CALC_SRCS:%.c=o/$(MODE)/%.o)

TOOL_CALC_COMS =				\
	$(TOOL_CALC_SRCS:%.c=o/$(MODE)/%.com)

TOOL_CALC_BINS =				\
	$(TOOL_CALC_COMS)			\
	$(TOOL_CALC_COMS:%=%.dbg)

TOOL_CALC_CHECKS =				\
	$(TOOL_CALC_HDRS:%=o/$(MODE)/%.ok)

TOOL_CALC_DIRECTDEPS =				\
	LIBC_BITS				\
	LIBC_CALLS				\
	LIBC_FMT				\
	LIBC_LOG				\
	LIBC_MEM				\
	LIBC_NEXGEN32E				\
	LIBC_RAND				\
	LIBC_RUNTIME				\
	LIBC_STDIO				\
	LIBC_STR				\
	LIBC_STUBS				\
	LIBC_SYSV				\
	LIBC_TINYMATH				\
	LIBC_X					\
	THIRD_PARTY_COMPILER_RT			\
	THIRD_PARTY_GDTOA

TOOL_CALC_DEPS :=				\
	$(call uniq,$(foreach x,$(TOOL_CALC_DIRECTDEPS),$($(x))))

$(TOOL_CALC_A):					\
		tool/calc/			\
		$(TOOL_CALC_A).pkg		\
		$(TOOL_CALC_OBJS)

$(TOOL_CALC_A).pkg:				\
		$(TOOL_CALC_OBJS)		\
		$(foreach x,$(TOOL_CALC_DIRECTDEPS),$($(x)_A).pkg)

o/tool/calc/calc.h.inc: o/tool/calc/calc.c.inc
o/tool/calc/calc.c.inc:				\
		tool/calc/calc.y		\
		$(THIRD_PARTY_LEMON)
	@$(LEMON) -l -d$(@D) $<

o/$(MODE)/tool/calc/%.com.dbg:			\
		$(TOOL_CALC_DEPS)		\
		$(TOOL_CALC_A)			\
		o/$(MODE)/tool/calc/%.o		\
		$(TOOL_CALC_A).pkg		\
		$(CRT)				\
		$(APE)
	@$(APELINK)

tool/calc/calc.c:				\
		o/tool/calc/calc.c.inc		\
		o/tool/calc/calc.h.inc

.PHONY: o/$(MODE)/tool/calc
o/$(MODE)/tool/calc:				\
		$(TOOL_CALC_BINS)		\
		$(TOOL_CALC_CHECKS)
