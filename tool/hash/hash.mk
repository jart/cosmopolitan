#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#───vi: set et ft=make ts=8 tw=8 fenc=utf-8 :vi───────────────────────┘

PKGS += TOOL_HASH

TOOL_HASH_SRCS := $(wildcard tool/hash/*.c)

TOOL_HASH_OBJS =				\
	$(TOOL_HASH_SRCS:%=o/$(MODE)/%.zip.o)	\
	$(TOOL_HASH_SRCS:%.c=o/$(MODE)/%.o)

TOOL_HASH_COMS =				\
	$(TOOL_HASH_SRCS:%.c=o/$(MODE)/%.com)

TOOL_HASH_BINS =				\
	$(TOOL_HASH_COMS)			\
	$(TOOL_HASH_COMS:%=%.dbg)

TOOL_HASH_DIRECTDEPS =				\
	LIBC_FMT				\
	LIBC_NEXGEN32E				\
	LIBC_RUNTIME				\
	LIBC_STR				\
	LIBC_STUBS				\
	LIBC_STDIO

TOOL_HASH_DEPS :=				\
	$(call uniq,$(foreach x,$(TOOL_HASH_DIRECTDEPS),$($(x))))

o/$(MODE)/tool/hash/hash.pkg:			\
		$(TOOL_HASH_OBJS)		\
		$(foreach x,$(TOOL_HASH_DIRECTDEPS),$($(x)_A).pkg)

o/$(MODE)/tool/hash/%.com.dbg:			\
		$(TOOL_HASH_DEPS)		\
		o/$(MODE)/tool/hash/%.o		\
		o/$(MODE)/tool/hash/hash.pkg	\
		$(CRT)				\
		$(APE)
	@$(APELINK)

$(TOOL_HASH_OBJS):				\
		$(BUILD_FILES)			\
		tool/hash/hash.mk

.PHONY: o/$(MODE)/tool/hash
o/$(MODE)/tool/hash: $(TOOL_HASH_BINS) $(TOOL_HASH_CHECKS)
