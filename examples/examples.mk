#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#───vi: set et ft=make ts=8 tw=8 fenc=utf-8 :vi───────────────────────┘

PKGS += EXAMPLES

EXAMPLES_FILES := $(wildcard examples/*)
EXAMPLES_MAINS_S = $(filter %.S,$(EXAMPLES_FILES))
EXAMPLES_MAINS_C = $(filter %.c,$(EXAMPLES_FILES))
EXAMPLES_MAINS_CC = $(filter %.cc,$(EXAMPLES_FILES))

EXAMPLES_SRCS =							\
	$(EXAMPLES_MAINS_S)					\
	$(EXAMPLES_MAINS_C)					\
	$(EXAMPLES_MAINS_CC)

EXAMPLES_MAINS =						\
	$(EXAMPLES_MAINS_S)					\
	$(EXAMPLES_MAINS_C)					\
	$(EXAMPLES_MAINS_CC)

EXAMPLES_OBJS =							\
	$(EXAMPLES_SRCS:%=o/$(MODE)/%.zip.o)			\
	$(EXAMPLES_MAINS_S:%.S=o/$(MODE)/%.o)			\
	$(EXAMPLES_MAINS_C:%.c=o/$(MODE)/%.o)			\
	$(EXAMPLES_MAINS_CC:%.cc=o/$(MODE)/%.o)

EXAMPLES_COMS =							\
	$(EXAMPLES_MAINS_S:%.S=o/$(MODE)/%.com)			\
	$(EXAMPLES_MAINS_C:%.c=o/$(MODE)/%.com)			\
	$(EXAMPLES_MAINS_CC:%.cc=o/$(MODE)/%.com)

EXAMPLES_ELFS =							\
	$(EXAMPLES_OBJS:%.o=%.elf)

EXAMPLES_BINS =							\
	$(EXAMPLES_ELFS)					\
	$(EXAMPLES_COMS)					\
	$(EXAMPLES_COMS:%=%.dbg)

EXAMPLES_DIRECTDEPS =						\
	APE_LIB							\
	DSP_CORE						\
	DSP_SCALE						\
	DSP_TTY							\
	LIBC_ALG						\
	LIBC_BITS						\
	LIBC_CALLS						\
	LIBC_CALLS_HEFTY					\
	LIBC_FMT						\
	LIBC_LOG						\
	LIBC_LOG_ASAN						\
	LIBC_MEM						\
	LIBC_NEXGEN32E						\
	LIBC_NT_NTDLL						\
	LIBC_NT_USER32						\
	LIBC_NT_WS2_32						\
	LIBC_OHMYPLUS						\
	LIBC_RAND						\
	LIBC_RUNTIME						\
	LIBC_SOCK						\
	LIBC_STDIO						\
	LIBC_STR						\
	LIBC_STUBS						\
	LIBC_SYSV						\
	LIBC_SYSV_CALLS						\
	LIBC_TESTLIB						\
	LIBC_TIME						\
	LIBC_TINYMATH						\
	LIBC_UNICODE						\
	LIBC_X							\
	LIBC_ZIPOS						\
	THIRD_PARTY_COMPILER_RT					\
	THIRD_PARTY_DLMALLOC					\
	THIRD_PARTY_GDTOA					\
	THIRD_PARTY_GETOPT					\
	THIRD_PARTY_MUSL					\
	THIRD_PARTY_STB						\
	THIRD_PARTY_XED						\
	THIRD_PARTY_ZLIB					\
	TOOL_VIZ_LIB

EXAMPLES_DEPS :=						\
	$(call uniq,$(foreach x,$(EXAMPLES_DIRECTDEPS),$($(x))))

o/$(MODE)/examples/examples.pkg:				\
		$(EXAMPLES_OBJS)				\
		$(THIRD_PARTY_DUKTAPE_A).pkg			\
		$(foreach x,$(EXAMPLES_DIRECTDEPS),$($(x)_A).pkg)

o/$(MODE)/examples/unbourne.o:					\
		OVERRIDE_CPPFLAGS +=				\
			-DSTACK_FRAME_UNLIMITED

o/$(MODE)/examples/%.com.dbg:					\
		$(EXAMPLES_DEPS)				\
		o/$(MODE)/examples/%.o				\
		o/$(MODE)/examples/examples.pkg			\
		$(CRT)						\
		$(APE)
	@$(APELINK)

o/$(MODE)/examples/%.elf:					\
		$(EXAMPLES_DEPS)				\
		$(THIRD_PARTY_DUKTAPE)				\
		o/$(MODE)/examples/%.o				\
		$(CRT)						\
		$(ELF)
	@$(ELFLINK)

o/$(MODE)/examples/tiny-raw-linux-tutorial.elf:			\
		o/$(MODE)/examples/tiny-raw-linux-tutorial.o	\
		$(ELF)
	@$(ELFLINK) -N -z max-page-size=0x10

$(EXAMPLES_OBJS): examples/examples.mk

o/$(MODE)/examples/hellojs.com.dbg:				\
		$(EXAMPLES_DEPS)				\
		$(THIRD_PARTY_DUKTAPE)				\
		o/$(MODE)/examples/hellojs.o			\
		o/$(MODE)/examples/hello.js.zip.o		\
		o/$(MODE)/examples/examples.pkg			\
		$(CRT)						\
		$(APE)
	@$(APELINK)

o/$(MODE)/examples/ispell.com.dbg:				\
		$(EXAMPLES_DEPS)				\
		o/$(MODE)/examples/ispell.o			\
		o/$(MODE)/usr/share/dict/words.zip.o		\
		o/$(MODE)/examples/examples.pkg			\
		$(CRT)						\
		$(APE)
	@$(APELINK)

o/$(MODE)/examples/nesemu1.com.dbg:				\
		$(EXAMPLES_DEPS)				\
		$(THIRD_PARTY_DUKTAPE)				\
		o/$(MODE)/examples/nesemu1.o			\
		o/$(MODE)/usr/share/rom/mario.nes.zip.o		\
		o/$(MODE)/usr/share/rom/zelda.nes.zip.o		\
		o/$(MODE)/usr/share/rom/tetris.nes.zip.o	\
		o/$(MODE)/examples/examples.pkg			\
		$(CRT)						\
		$(APE)
	@$(APELINK)

usr/share/dict/words: usr/share/dict/words.gz
	@$(MKDIR) $(dir $@)
	@$(GZ) $(ZFLAGS) -d <$< >$@

o/$(MODE)/examples/ugh.ok: o/$(MODE)/examples/wut.com
	$<
	touch $@

.PHONY: o/$(MODE)/examples
o/$(MODE)/examples:						\
		o/$(MODE)/examples/package			\
		$(EXAMPLES_BINS)
