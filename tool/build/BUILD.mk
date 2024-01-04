#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#── vi: set noet ft=make ts=8 sw=8 fenc=utf-8 :vi ────────────────────┘

PKGS += TOOL_BUILD

TOOL_BUILD_FILES := $(wildcard tool/build/*)
TOOL_BUILD_SRCS = $(filter %.c,$(TOOL_BUILD_FILES))
TOOL_BUILD_HDRS = $(filter %.h,$(TOOL_BUILD_FILES))

TOOL_BUILD_BINS =							\
	$(TOOL_BUILD_COMS)						\
	$(TOOL_BUILD_COMS:%=%.dbg)

TOOL_BUILD_OBJS =							\
	$(TOOL_BUILD_SRCS:%.c=o/$(MODE)/%.o)

TOOL_BUILD_COMS =							\
	$(TOOL_BUILD_SRCS:%.c=o/$(MODE)/%.com)

TOOL_BUILD_CHECKS =							\
	$(TOOL_BUILD).pkg						\
	$(TOOL_BUILD_HDRS:%=o/$(MODE)/%.ok)

TOOL_BUILD_DIRECTDEPS =							\
	DSP_CORE							\
	DSP_SCALE							\
	DSP_TTY								\
	LIBC_CALLS							\
	LIBC_DLOPEN							\
	LIBC_ELF							\
	LIBC_FMT							\
	LIBC_INTRIN							\
	LIBC_LOG							\
	LIBC_MEM							\
	LIBC_NEXGEN32E							\
	LIBC_NT_KERNEL32						\
	LIBC_NT_PSAPI							\
	LIBC_NT_USER32							\
	LIBC_NT_WS2_32							\
	LIBC_PROC							\
	LIBC_RUNTIME							\
	LIBC_SOCK							\
	LIBC_STDIO							\
	LIBC_STR							\
	LIBC_SYSV							\
	LIBC_SYSV_CALLS							\
	LIBC_THREAD							\
	LIBC_TIME							\
	LIBC_TINYMATH							\
	LIBC_X								\
	NET_HTTP							\
	NET_HTTPS							\
	THIRD_PARTY_COMPILER_RT						\
	THIRD_PARTY_GDTOA						\
	THIRD_PARTY_GETOPT						\
	THIRD_PARTY_MBEDTLS						\
	THIRD_PARTY_MUSL						\
	THIRD_PARTY_REGEX						\
	THIRD_PARTY_STB							\
	THIRD_PARTY_XED							\
	THIRD_PARTY_ZLIB						\
	THIRD_PARTY_ZLIB_GZ						\
	TOOL_BUILD_LIB

TOOL_BUILD_DEPS :=							\
	$(call uniq,$(foreach x,$(TOOL_BUILD_DIRECTDEPS),$($(x))))

o/$(MODE)/tool/build/build.pkg:						\
		$(TOOL_BUILD_OBJS)					\
		$(foreach x,$(TOOL_BUILD_DIRECTDEPS),$($(x)_A).pkg)

o/$(MODE)/tool/build/%.com.dbg:						\
		$(TOOL_BUILD_DEPS)					\
		o/$(MODE)/tool/build/build.pkg				\
		o/$(MODE)/tool/build/%.o				\
		$(CRT)							\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

o/$(MODE)/tool/build/dso/sandbox-$(ARCH).so.zip.o			\
o/$(MODE)/tool/build/false.com.zip.o					\
o/$(MODE)/tool/build/echo.com.zip.o					\
o/$(MODE)/tool/build/cocmd.com.zip.o: private				\
		ZIPOBJ_FLAGS +=						\
			-B

# we need pic because:
#   so it can be an LD_PRELOAD payload
o/$(MODE)/tool/build/dso/sandbox.o: private				\
		CFLAGS +=						\
			-fPIC

o/$(MODE)/tool/build/dso/sandbox.o:					\
		libc/calls/calls.h					\
		tool/build/dso/sandbox.c				\
		libc/calls/pledge.h					\
		libc/runtime/runtime.h					\
		libc/calls/pledge.internal.h				\
		libc/intrin/promises.internal.h				\
		tool/build/BUILD.mk

o/$(MODE)/tool/build/dso/sandbox-$(ARCH).so:				\
		o/$(MODE)/tool/build/dso/sandbox.o			\
		o/$(MODE)/libc/calls/pledge-linux.o			\
		o/$(MODE)/libc/sysv/restorert.o
	@$(COMPILE) -ALINK.so						\
		$(CC)							\
		-s							\
		-shared							\
		-nostdlib						\
		-fuse-ld=bfd						\
		-Wl,--gc-sections					\
		o/$(MODE)/tool/build/dso/sandbox.o			\
		o/$(MODE)/libc/calls/pledge-linux.o			\
		o/$(MODE)/libc/sysv/restorert.o				\
		$(OUTPUT_OPTION)

o/$(MODE)/tool/build/pledge.com.dbg:					\
		$(TOOL_BUILD_DEPS)					\
		o/$(MODE)/tool/build/build.pkg				\
		o/$(MODE)/tool/build/dso/sandbox-$(ARCH).so.zip.o	\
		o/$(MODE)/tool/build/pledge.o				\
		$(CRT)							\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

o/$(MODE)/tool/build/dso/dlopen_helper.so:				\
		o/$(MODE)/tool/build/dso/dlopen_helper.o
	@$(COMPILE) -ALINK.so						\
		$(CC)							\
		-s							\
		-shared							\
		-nostdlib						\
		-fuse-ld=bfd						\
		o/$(MODE)/tool/build/dso/dlopen_helper.o		\
		$(OUTPUT_OPTION)

o/$(MODE)/tool/build/dlopen_test.com.runs:				\
		o/$(MODE)/tool/build/dlopen_test.com			\
		o/$(MODE)/tool/build/dso/dlopen_helper.so
	$< o/$(MODE)/tool/build/dso/dlopen_helper.so

.PHONY: o/$(MODE)/tool/build
o/$(MODE)/tool/build:							\
		o/$(MODE)/tool/build/lib				\
		$(TOOL_BUILD_BINS)					\
		$(TOOL_BUILD_CHECKS)
