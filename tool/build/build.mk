#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#───vi: set et ft=make ts=8 tw=8 fenc=utf-8 :vi───────────────────────┘

PKGS += TOOL_BUILD

TOOL_BUILD_FILES := $(wildcard tool/build/*)
TOOL_BUILD_SRCS = $(filter %.c,$(TOOL_BUILD_FILES))
TOOL_BUILD_HDRS = $(filter %.h,$(TOOL_BUILD_FILES))
TOOL_BUILD_CTESTS = $(filter %.ctest,$(TOOL_BUILD_FILES))
TOOL_BUILD_BINS = $(TOOL_BUILD_COMS) $(TOOL_BUILD_COMS:%=%.dbg)
TOOL_BUILD_CALCULATOR = o/$(MODE)/tool/build/calculator.com

TOOL_BUILD_OBJS =					\
	$(TOOL_BUILD_SRCS:%=o/$(MODE)/%.zip.o)		\
	$(TOOL_BUILD_SRCS:%.c=o/$(MODE)/%.o)

TOOL_BUILD_COMS =					\
	$(TOOL_BUILD_SRCS:%.c=o/$(MODE)/%.com)

TOOL_BUILD_CHECKS =					\
	$(TOOL_BUILD).pkg				\
	$(TOOL_BUILD_HDRS:%=o/$(MODE)/%.ok)		\
	$(TOOL_BUILD_CTESTS:%=o/$(MODE)/%.ok)

TOOL_BUILD_DIRECTDEPS =					\
	DSP_CORE					\
	DSP_SCALE					\
	DSP_TTY						\
	LIBC_ALG					\
	LIBC_BITS					\
	LIBC_CALLS					\
	LIBC_CALLS_HEFTY				\
	LIBC_DNS					\
	LIBC_ELF					\
	LIBC_FMT					\
	LIBC_INTRIN					\
	LIBC_LOG					\
	LIBC_LOG_ASAN					\
	LIBC_MEM					\
	LIBC_NEXGEN32E					\
	LIBC_NT_USER32					\
	LIBC_RAND					\
	LIBC_RUNTIME					\
	LIBC_SOCK					\
	LIBC_STDIO					\
	LIBC_STR					\
	LIBC_STUBS					\
	LIBC_SYSV					\
	LIBC_SYSV_CALLS					\
	LIBC_TIME					\
	LIBC_TINYMATH					\
	LIBC_UNICODE					\
	LIBC_X						\
	TOOL_BUILD_LIB					\
	THIRD_PARTY_COMPILER_RT				\
	THIRD_PARTY_GDTOA				\
	THIRD_PARTY_GETOPT				\
	THIRD_PARTY_XED					\
	THIRD_PARTY_ZLIB				\
	THIRD_PARTY_STB

TOOL_BUILD_DEPS :=					\
	$(call uniq,$(foreach x,$(TOOL_BUILD_DIRECTDEPS),$($(x))))

o/$(MODE)/tool/build/build.pkg:				\
		$(TOOL_BUILD_OBJS)			\
		$(foreach x,$(TOOL_BUILD_DIRECTDEPS),$($(x)_A).pkg)

o/$(MODE)/%.ctest.ok:					\
		%.ctest					\
		$(TOOL_BUILD_CALCULATOR)
	@TARGET=$@ ACTION=MKWIDES build/do		\
	$(TOOL_BUILD_CALCULATOR) $< && \
	touch $@

o/$(MODE)/tool/build/%.com.dbg:				\
		$(TOOL_BUILD_DEPS)			\
		o/$(MODE)/tool/build/build.pkg		\
		o/$(MODE)/tool/build/%.o		\
		$(CRT)					\
		$(APE)
	-@$(APELINK)

o/$(MODE)/tool/build/emulator.o:			\
		OVERRIDE_COPTS +=			\
			-fno-sanitize=pointer-overflow

.PHONY: o/$(MODE)/tool/build
o/$(MODE)/tool/build:					\
		o/$(MODE)/tool/build/emucrt		\
		o/$(MODE)/tool/build/emubin		\
		o/$(MODE)/tool/build/lib		\
		$(TOOL_BUILD_BINS)			\
		$(TOOL_BUILD_CHECKS)
