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
	LIBC_DNS					\
	LIBC_ELF					\
	LIBC_FMT					\
	LIBC_INTRIN					\
	LIBC_LOG					\
	LIBC_MEM					\
	LIBC_NEXGEN32E					\
	LIBC_NT_KERNEL32				\
	LIBC_NT_USER32					\
	LIBC_NT_WS2_32					\
	LIBC_RAND					\
	LIBC_RUNTIME					\
	LIBC_SOCK					\
	LIBC_STDIO					\
	LIBC_STR					\
	LIBC_STUBS					\
	LIBC_SYSV					\
	LIBC_SYSV_CALLS					\
	LIBC_THREAD					\
	LIBC_TIME					\
	LIBC_TINYMATH					\
	LIBC_UNICODE					\
	LIBC_X						\
	LIBC_ZIPOS					\
	NET_HTTPS					\
	THIRD_PARTY_COMPILER_RT				\
	THIRD_PARTY_GDTOA				\
	THIRD_PARTY_GETOPT				\
	THIRD_PARTY_MBEDTLS				\
	THIRD_PARTY_MUSL				\
	THIRD_PARTY_REGEX				\
	THIRD_PARTY_STB					\
	THIRD_PARTY_XED					\
	THIRD_PARTY_ZLIB				\
	THIRD_PARTY_ZLIB_GZ				\
	TOOL_BUILD_LIB

TOOL_BUILD_DEPS :=					\
	$(call uniq,$(foreach x,$(TOOL_BUILD_DIRECTDEPS),$($(x))))

o/$(MODE)/tool/build/build.pkg:				\
		$(TOOL_BUILD_OBJS)			\
		$(foreach x,$(TOOL_BUILD_DIRECTDEPS),$($(x)_A).pkg)

o/$(MODE)/%.ctest.ok:					\
		%.ctest					\
		$(TOOL_BUILD_CALCULATOR)
	@$(COMPILE) -AMKWIDES -tT$@ $(TOOL_BUILD_CALCULATOR) $<

o/$(MODE)/tool/build/%.com.dbg:				\
		$(TOOL_BUILD_DEPS)			\
		o/$(MODE)/tool/build/build.pkg		\
		o/$(MODE)/tool/build/%.o		\
		$(CRT)					\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

.PRECIOUS: o/$(MODE)/tool/build/blinkenlights.com
o/$(MODE)/tool/build/blinkenlights.com:						\
		o/$(MODE)/tool/build/blinkenlights.com.dbg			\
		o/$(MODE)/third_party/zip/zip.com				\
		o/$(MODE)/tool/build/symtab.com
	@$(COMPILE) -AOBJCOPY -T$@ $(OBJCOPY) -S -O binary $< $@
	@$(COMPILE) -ASYMTAB o/$(MODE)/tool/build/symtab.com			\
		-o o/$(MODE)/tool/build/.blinkenlights/.symtab $<
	@$(COMPILE) -AZIP -T$@ o/$(MODE)/third_party/zip/zip.com -9qj $@	\
		o/$(MODE)/tool/build/.blinkenlights/.symtab

o/$(MODE)/tool/build/emulator.o:			\
		OVERRIDE_COPTS +=			\
			-fno-sanitize=pointer-overflow

o/$(MODE)/tool/build/mkdir.zip.o: o/$(MODE)/tool/build/mkdir
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) -0 -B -Pbin $(OUTPUT_OPTION) $<
o/$(MODE)/tool/build/chmod.zip.o: o/$(MODE)/tool/build/chmod
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) -0 -B -Pbin $(OUTPUT_OPTION) $<
o/$(MODE)/tool/build/cp.zip.o: o/$(MODE)/tool/build/cp
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) -0 -B -Pbin $(OUTPUT_OPTION) $<
o/$(MODE)/tool/build/mv.zip.o: o/$(MODE)/tool/build/mv
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) -0 -B -Pbin $(OUTPUT_OPTION) $<
o/$(MODE)/tool/build/echo.zip.o: o/$(MODE)/tool/build/echo
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) -0 -B -Pbin $(OUTPUT_OPTION) $<
o/$(MODE)/tool/build/gzip.zip.o: o/$(MODE)/tool/build/gzip
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) -0 -B -Pbin $(OUTPUT_OPTION) $<
o/$(MODE)/tool/build/printf.zip.o: o/$(MODE)/tool/build/printf
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) -0 -B -Pbin $(OUTPUT_OPTION) $<
o/$(MODE)/tool/build/dd.zip.o: o/$(MODE)/tool/build/dd
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) -0 -B -Pbin $(OUTPUT_OPTION) $<

# we need pic because:
#   so it can be an LD_PRELOAD payload
o/$(MODE)/tool/build/dso/sandbox.o:			\
		OVERRIDE_CFLAGS +=			\
			-fPIC

o/$(MODE)/tool/build/dso/sandbox.o:			\
		libc/calls/calls.h			\
		tool/build/dso/sandbox.c		\
		libc/calls/pledge.h			\
		libc/runtime/runtime.h			\
		libc/calls/pledge.internal.h		\
		libc/intrin/promises.internal.h		\
		tool/build/build.mk

o/$(MODE)/tool/build/dso/sandbox.so:			\
		o/$(MODE)/tool/build/dso/sandbox.o	\
		o/$(MODE)/libc/calls/pledge-linux.o	\
		o/$(MODE)/libc/sysv/restorert.o
	@$(CC)	-s					\
		-shared					\
		-nostdlib				\
		-Wl,--gc-sections			\
		o/$(MODE)/tool/build/dso/sandbox.o	\
		o/$(MODE)/libc/calls/pledge-linux.o	\
		o/$(MODE)/libc/sysv/restorert.o		\
		$(OUTPUT_OPTION)

o/$(MODE)/tool/build/dso/sandbox.so.zip.o:		\
		o/$(MODE)/tool/build/dso/sandbox.so
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) -B $(OUTPUT_OPTION) $<

o/$(MODE)/tool/build/pledge.com.dbg:			\
		$(TOOL_BUILD_DEPS)			\
		o/$(MODE)/tool/build/build.pkg		\
		o/$(MODE)/tool/build/dso/sandbox.so.zip.o \
		o/$(MODE)/tool/build/pledge.o		\
		$(CRT)					\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

.PHONY: o/$(MODE)/tool/build
o/$(MODE)/tool/build:					\
		o/$(MODE)/tool/build/emucrt		\
		o/$(MODE)/tool/build/emubin		\
		o/$(MODE)/tool/build/lib		\
		$(TOOL_BUILD_BINS)			\
		$(TOOL_BUILD_CHECKS)
