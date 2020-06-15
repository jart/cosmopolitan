#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#───vi: set et ft=make ts=8 tw=8 fenc=utf-8 :vi───────────────────────┘

PKGS += TOOL_BUILD

TOOL_BUILD_FILES := $(wildcard tool/build/*)
TOOL_BUILD_SRCS = $(filter %.c,$(TOOL_BUILD_FILES))
TOOL_BUILD_HDRS = $(filter %.h,$(TOOL_BUILD_FILES))
TOOL_BUILD_COMS = $(TOOL_BUILD_OBJS:%.o=%.com)
TOOL_BUILD_BINS = $(TOOL_BUILD_COMS) $(TOOL_BUILD_COMS:%=%.dbg)

TOOL_BUILD_OBJS =				\
	$(TOOL_BUILD_SRCS:%=o/$(MODE)/%.zip.o)	\
	$(TOOL_BUILD_SRCS:%.c=o/$(MODE)/%.o)

TOOL_BUILD_LINK =				\
	$(TOOL_BUILD_DEPS)			\
	o/$(MODE)/tool/build/%.o		\
	$(CRT)					\
	$(APE)

TOOL_BUILD_DIRECTDEPS =				\
	DSP_CORE				\
	DSP_SCALE				\
	LIBC_ALG				\
	LIBC_CALLS				\
	LIBC_CALLS_HEFTY			\
	LIBC_CONV				\
	LIBC_DNS				\
	LIBC_ELF				\
	LIBC_FMT				\
	LIBC_LOG				\
	LIBC_TINYMATH				\
	LIBC_MEM				\
	LIBC_NEXGEN32E				\
	LIBC_RUNTIME				\
	LIBC_SOCK				\
	LIBC_STDIO				\
	LIBC_STR				\
	LIBC_RAND				\
	LIBC_STUBS				\
	LIBC_SYSV				\
	LIBC_SYSV_CALLS				\
	LIBC_TESTLIB				\
	LIBC_TIME				\
	LIBC_UNICODE				\
	LIBC_X					\
	TOOL_BUILD_LIB				\
	THIRD_PARTY_DTOA			\
	THIRD_PARTY_GETOPT			\
	THIRD_PARTY_XED				\
	THIRD_PARTY_ZLIB			\
	THIRD_PARTY_STB

TOOL_BUILD_DEPS :=				\
	$(call uniq,$(foreach x,$(TOOL_BUILD_DIRECTDEPS),$($(x))))

o/$(MODE)/tool/build/build.pkg:			\
		$(TOOL_BUILD_OBJS)		\
		$(foreach x,$(TOOL_BUILD_DIRECTDEPS),$($(x)_A).pkg)

o/$(MODE)/tool/build/%.com.dbg:			\
		$(TOOL_BUILD_DEPS)		\
		o/$(MODE)/tool/build/build.pkg	\
		o/$(MODE)/tool/build/%.o	\
		$(CRT)				\
		$(APE)
	-@$(APELINK)

o/$(MODE)/tool/build/mkdeps.o: tool/build/mkdeps.c
	-@ACTION=OBJECTIFY.c build/compile $(OBJECTIFY.c) $(OUTPUT_OPTION) $<

o/$(MODE)/tool/build/generatematrix.o		\
o/$(MODE)/tool/build/mkdeps.o:			\
		OVERRIDE_COPTS +=		\
			-O2

.PHONY: o/$(MODE)/tool/build
o/$(MODE)/tool/build:				\
		o/$(MODE)/tool/build/lib	\
		$(TOOL_BUILD_BINS)		\
		$(TOOL_BUILD_CHECKS)
