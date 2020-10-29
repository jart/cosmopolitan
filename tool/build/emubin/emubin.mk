#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#───vi: set et ft=make ts=8 tw=8 fenc=utf-8 :vi───────────────────────┘

PKGS += TOOL_BUILD_EMUBIN

TOOL_BUILD_EMUBIN_BINS =				\
	o/$(MODE)/tool/build/emubin/sha256.bin		\
	o/$(MODE)/tool/build/emubin/sha256.bin.dbg	\
	o/$(MODE)/tool/build/emubin/mips.bin		\
	o/$(MODE)/tool/build/emubin/mips.bin.dbg	\
	o/$(MODE)/tool/build/emubin/prime.bin		\
	o/$(MODE)/tool/build/emubin/prime.bin.dbg	\
	o/$(MODE)/tool/build/emubin/pi.bin		\
	o/$(MODE)/tool/build/emubin/pi.bin.dbg		\
	o/$(MODE)/tool/build/emubin/linmap.elf

TOOL_BUILD_EMUBIN_A = o/$(MODE)/tool/build/emubin/emubin.a
TOOL_BUILD_EMUBIN_FILES := $(wildcard tool/build/emubin/*)
TOOL_BUILD_EMUBIN_SRCS = $(filter %.c,$(TOOL_BUILD_EMUBIN_FILES))
TOOL_BUILD_EMUBIN_HDRS = $(filter %.h,$(TOOL_BUILD_EMUBIN_FILES))

TOOL_BUILD_EMUBIN_OBJS =				\
	$(TOOL_BUILD_EMUBIN_SRCS:%=o/$(MODE)/%.zip.o)	\
	$(TOOL_BUILD_EMUBIN_SRCS:%.c=o/$(MODE)/%.o)

TOOL_BUILD_EMUBIN_CHECKS =				\
	$(TOOL_BUILD_EMUBIN_HDRS:%=o/$(MODE)/%.ok)

TOOL_BUILD_EMUBIN_DIRECTDEPS =				\
	LIBC_STUBS					\
	LIBC_NEXGEN32E					\
	LIBC_TINYMATH

TOOL_BUILD_EMUBIN_DEPS :=				\
	$(call uniq,$(foreach x,$(TOOL_BUILD_EMUBIN_DIRECTDEPS),$($(x))))

$(TOOL_BUILD_EMUBIN_A):					\
		tool/build/emubin/			\
		$(TOOL_BUILD_EMUBIN_A).pkg		\
		$(TOOL_BUILD_EMUBIN_OBJS)

$(TOOL_BUILD_EMUBIN_A).pkg:				\
		$(TOOL_BUILD_EMUBIN_OBJS)		\
		$(foreach x,$(TOOL_BUILD_EMUBIN_DIRECTDEPS),$($(x)_A).pkg)

o/$(MODE)/tool/build/emubin/%.bin.dbg:			\
		$(TOOL_BUILD_EMUCRT)			\
		$(TOOL_BUILD_EMUBIN_DEPS)		\
		$(TOOL_BUILD_EMUBIN_A)			\
		o/$(MODE)/tool/build/emubin/%.o		\
		$(TOOL_BUILD_EMUBIN_A).pkg
	@$(ELFLINK) -e emucrt -z max-page-size=0x10

o/$(MODE)/tool/build/emubin/%.elf:			\
		$(TOOL_BUILD_EMUBIN_DEPS)		\
		$(TOOL_BUILD_EMUBIN_A)			\
		o/$(MODE)/tool/build/emubin/%.o		\
		$(ELF)
	@$(ELFLINK)

o/tiny/tool/build/emubin/spiral.bin.dbg:		\
		$(TOOL_BUILD_EMUBIN_DEPS)		\
		o/tiny/tool/build/emubin/spiral.real.o
	@$(ELFLINK) -z max-page-size=0x10 -T tool/build/emucrt/real.lds

o/tiny/tool/build/emubin/mdatest.bin.dbg:		\
		$(TOOL_BUILD_EMUBIN_DEPS)		\
		o/tiny/tool/build/emubin/mdatest.real.o
	@$(ELFLINK) -z max-page-size=0x10 -T tool/build/emucrt/real.lds

.PHONY: o/$(MODE)/tool/build/emubin
o/$(MODE)/tool/build/emubin:				\
		$(TOOL_BUILD_EMUBIN_BINS)		\
		$(TOOL_BUILD_EMUBIN_CHECKS)
