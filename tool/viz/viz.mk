#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#───vi: set et ft=make ts=8 tw=8 fenc=utf-8 :vi───────────────────────┘

PKGS += TOOL_VIZ

TOOL_VIZ_SRCS := $(wildcard tool/viz/*.c)

TOOL_VIZ_OBJS =					\
	$(TOOL_VIZ_SRCS:%=o/$(MODE)/%.zip.o)	\
	$(TOOL_VIZ_SRCS:%.c=o/$(MODE)/%.o)

TOOL_VIZ_COMS =					\
	$(TOOL_VIZ_SRCS:%.c=o/$(MODE)/%.com)

TOOL_VIZ_BINS =					\
	$(TOOL_VIZ_COMS)			\
	$(TOOL_VIZ_COMS:%=%.dbg)

TOOL_VIZ_DIRECTDEPS =				\
	DSP_CORE				\
	DSP_MPEG				\
	DSP_SCALE				\
	DSP_TTY					\
	LIBC_BITS				\
	LIBC_CALLS				\
	LIBC_CALLS_HEFTY			\
	LIBC_DNS				\
	LIBC_FMT				\
	LIBC_LOG				\
	LIBC_MEM				\
	LIBC_NEXGEN32E				\
	LIBC_NT_COMDLG32			\
	LIBC_NT_GDI32				\
	LIBC_NT_KERNEL32			\
	LIBC_NT_USER32				\
	LIBC_RAND				\
	LIBC_RUNTIME				\
	LIBC_SOCK				\
	LIBC_STDIO				\
	LIBC_STR				\
	LIBC_STUBS				\
	LIBC_SYSV				\
	LIBC_SYSV_CALLS				\
	LIBC_TESTLIB				\
	LIBC_TIME				\
	LIBC_TINYMATH				\
	LIBC_UNICODE				\
	LIBC_X					\
	NET_HTTP				\
	THIRD_PARTY_DLMALLOC			\
	THIRD_PARTY_GDTOA			\
	THIRD_PARTY_GETOPT			\
	THIRD_PARTY_STB				\
	THIRD_PARTY_XED				\
	TOOL_DECODE_LIB				\
	TOOL_VIZ_LIB				\
	THIRD_PARTY_ZLIB

TOOL_VIZ_DEPS :=				\
	$(call uniq,$(foreach x,$(TOOL_VIZ_DIRECTDEPS),$($(x))))

o/$(MODE)/tool/viz/viz.pkg:			\
		$(TOOL_VIZ_OBJS)		\
		$(foreach x,$(TOOL_VIZ_DIRECTDEPS),$($(x)_A).pkg)

o/$(MODE)/tool/viz/%.com.dbg:			\
		$(TOOL_VIZ_DEPS)		\
		o/$(MODE)/tool/viz/%.o		\
		o/$(MODE)/tool/viz/viz.pkg	\
		$(CRT)				\
		$(APE)
	@$(APELINK)

o/$(MODE)/tool/viz/derasterize.o:		\
		OVERRIDE_CFLAGS +=		\
			-DSTACK_FRAME_UNLIMITED	\
			$(MATHEMATICAL)

o/$(MODE)/tool/viz/magikarp.o:			\
		OVERRIDE_CFLAGS +=		\
			$(MATHEMATICAL)

$(TOOL_VIZ_OBJS):				\
		$(BUILD_FILES)			\
		tool/viz/viz.mk

# ifeq (,$(MODE))
# $(TOOL_VIZ_OBJS):				\
# 		OVERRIDE_CFLAGS +=		\
# 			-fsanitize=address
# endif

.PHONY: o/$(MODE)/tool/viz
o/$(MODE)/tool/viz:				\
		o/$(MODE)/tool/viz/lib		\
		$(TOOL_VIZ_BINS)		\
		$(TOOL_VIZ_CHECKS)
