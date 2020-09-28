#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#───vi: set et ft=make ts=8 tw=8 fenc=utf-8 :vi───────────────────────┘

PKGS += TOOL_TAGS

TOOL_TAGS = $(TOOL_LIB_A_DEPS) $(TOOL_LIB_A)
TOOL_TAGS_A = o/$(MODE)/tool/tags/tags.a
TOOL_TAGS_FILES := $(wildcard tool/tags/*)
TOOL_TAGS_COMS = $(TOOL_TAGS_OBJS:%.o=%.com)
TOOL_TAGS_SRCS = $(filter %.c,$(TOOL_TAGS_FILES))
TOOL_TAGS_HDRS = $(filter %.h,$(TOOL_TAGS_FILES))

TOOL_TAGS_OBJS =				\
	$(TOOL_TAGS_SRCS:%=o/$(MODE)/%.zip.o)	\
	$(TOOL_TAGS_SRCS:%.c=o/$(MODE)/%.o)

TOOL_TAGS_COMS =				\
	$(TOOL_TAGS_SRCS:%.c=o/$(MODE)/%.com)

TOOL_TAGS_BINS =				\
	$(TOOL_TAGS_COMS)			\
	$(TOOL_TAGS_COMS:%=%.dbg)

TOOL_TAGS_CHECKS =				\
	$(TOOL_TAGS_HDRS:%=o/$(MODE)/%.ok)

TOOL_TAGS_DIRECTDEPS =				\
	LIBC_BITS				\
	LIBC_CALLS				\
	LIBC_CONV				\
	LIBC_LOG				\
	LIBC_FMT				\
	LIBC_MEM				\
	LIBC_NEXGEN32E				\
	LIBC_RUNTIME				\
	LIBC_STDIO				\
	LIBC_STUBS				\
	LIBC_STR				\
	LIBC_SYSV				\
	LIBC_TINYMATH				\
	LIBC_X					\
	THIRD_PARTY_COMPILER_RT			\
	THIRD_PARTY_DTOA

TOOL_TAGS_DEPS :=				\
	$(call uniq,$(foreach x,$(TOOL_TAGS_DIRECTDEPS),$($(x))))

$(TOOL_TAGS_A):					\
		tool/tags/			\
		$(TOOL_TAGS_A).pkg		\
		$(TOOL_TAGS_OBJS)

$(TOOL_TAGS_A).pkg:				\
		$(TOOL_TAGS_OBJS)		\
		$(foreach x,$(TOOL_TAGS_DIRECTDEPS),$($(x)_A).pkg)

o/tool/tags/tags.h.inc: o/tool/tags/tags.c.inc
o/tool/tags/tags.c.inc:				\
		tool/tags/tags.y		\
		$(THIRD_PARTY_LEMON)
	@$(LEMON) -l -d$(@D) $<

o/$(MODE)/tool/tags/%.com.dbg:			\
		$(TOOL_TAGS_DEPS)		\
		$(TOOL_TAGS_A)			\
		o/$(MODE)/tool/tags/%.o		\
		$(TOOL_TAGS_A).pkg		\
		$(CRT)				\
		$(APE)
	@$(APELINK)

.PHONY: o/$(MODE)/tool/tags
o/$(MODE)/tool/tags:				\
		$(TOOL_TAGS_BINS)		\
		$(TOOL_TAGS_CHECKS)
