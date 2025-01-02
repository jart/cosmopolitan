#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#── vi: set noet ft=make ts=8 sw=8 fenc=utf-8 :vi ────────────────────┘

PKGS += TOOL_HELLO

TOOL_HELLO_FILES := $(wildcard tool/hello/*)
TOOL_HELLO_HDRS = $(filter %.h,$(TOOL_HELLO_FILES))
TOOL_HELLO_SRCS_C = $(filter %.c,$(TOOL_HELLO_FILES))
TOOL_HELLO_SRCS_S = $(filter %.S,$(TOOL_HELLO_FILES))
TOOL_HELLO_SRCS = $(TOOL_HELLO_SRCS_C) $(TOOL_HELLO_SRCS_S)
TOOL_HELLO_OBJS = $(TOOL_HELLO_SRCS_C:%.c=o/$(MODE)/%.o) $(TOOL_HELLO_SRCS_S:%.S=o/$(MODE)/%.o)
TOOL_HELLO_BINS = $(TOOL_HELLO_COMS) $(TOOL_HELLO_COMS:%=%.dbg)

TOOL_HELLO_COMS =					\
	o/$(MODE)/tool/hello/hello			\
	o/$(MODE)/tool/hello/hello-pe			\
	o/$(MODE)/tool/hello/hello-elf			\
	o/$(MODE)/tool/hello/hello-unix

TOOL_HELLO_DIRECTDEPS =					\
	LIBC_CALLS					\
	LIBC_RUNTIME

TOOL_HELLO_DEPS :=					\
	$(call uniq,$(foreach x,$(TOOL_HELLO_DIRECTDEPS),$($(x))))

o/$(MODE)/tool/hello/hello.pkg:				\
		$(TOOL_HELLO_OBJS)			\
		$(foreach x,$(TOOL_HELLO_DIRECTDEPS),$($(x)_A).pkg)

# generates debuggable executable using gcc
o/$(MODE)/tool/hello/hello.dbg:				\
		$(TOOL_HELLO_DEPS)			\
		o/$(MODE)/tool/hello/hello.o		\
		o/$(MODE)/tool/hello/hello.pkg		\
		$(CRT)					\
		$(APE)
	@$(APELINK)

# uses apelink to turn it into an ape executable
# support vector is set to all operating systems
o/$(MODE)/tool/hello/hello.ape:				\
		o/$(MODE)/tool/hello/hello.dbg		\
		o/$(MODE)/tool/build/apelink		\
		o/$(MODE)/tool/build/pecheck		\
		o/$(MODE)/ape/ape.elf
	@$(COMPILE) -ALINK.ape o/$(MODE)/tool/build/apelink -o $@ -l o/$(MODE)/ape/ape.elf $<
	@$(COMPILE) -APECHECK -wT$@ o/$(MODE)/tool/build/pecheck $@

# uses apelink to generate elf-only executable
# support vector = linux/freebsd/openbsd/netbsd/metal
o/$(MODE)/tool/hello/hello-elf.ape:			\
		o/$(MODE)/tool/hello/hello.dbg		\
		o/$(MODE)/tool/build/apelink		\
		o/$(MODE)/ape/ape.elf
	@$(COMPILE) -ALINK.ape o/$(MODE)/tool/build/apelink -V elf -o $@ -l o/$(MODE)/ape/ape.elf $<

# uses apelink to generate non-pe ape executable
# support vector = macos/linux/freebsd/openbsd/netbsd
# - great way to avoid attention from bad virus scanners
# - creates tinier executable by reducing alignment requirement
o/$(MODE)/tool/hello/hello-unix.ape:			\
		o/$(MODE)/tool/hello/hello.dbg		\
		o/$(MODE)/tool/build/apelink		\
		o/$(MODE)/ape/ape.elf
	@$(COMPILE) -ALINK.ape o/$(MODE)/tool/build/apelink -V unix -o $@ -l o/$(MODE)/ape/ape.elf $<

# elf2pe generates optimal pe binaries
# windows is the only platform supported
# doesn't depend on ape or the cosmopolitan c library
o/$(MODE)/tool/hello/hello-pe.dbg:			\
		o/$(MODE)/tool/hello/hello-pe.o
	@$(COMPILE) -ALINK.elf $(LINK) $(LINKARGS) $(OUTPUT_OPTION) -q -e WinMain
o/$(MODE)/tool/hello/hello-pe.ape:			\
		o/$(MODE)/tool/hello/hello-pe.dbg	\
		o/$(MODE)/tool/build/elf2pe
	@$(COMPILE) -AELF2PE o/$(MODE)/tool/build/elf2pe -o $@ $<

# elf2pe can generate binaries that don't have dll imports
o/$(MODE)/tool/hello/life-pe.dbg:			\
		o/$(MODE)/tool/hello/life-pe.o
	@$(COMPILE) -ALINK.elf $(LINK) $(LINKARGS) $(OUTPUT_OPTION) -q -e WinMain -Ttext-segment=0x140000000
o/$(MODE)/tool/hello/life-pe.ape:			\
		o/$(MODE)/tool/hello/life-pe.dbg	\
		o/$(MODE)/tool/build/elf2pe
	@$(COMPILE) -AELF2PE o/$(MODE)/tool/build/elf2pe -o $@ $<

# demonstrates in process monitor the lowest resource usage a win32 app can have
o/$(MODE)/tool/hello/wait-pe.dbg:			\
		o/$(MODE)/tool/hello/wait-pe.o
	@$(COMPILE) -ALINK.elf $(LINK) $(LINKARGS) $(OUTPUT_OPTION) -q -e WinMain
o/$(MODE)/tool/hello/wait-pe.ape:			\
		o/$(MODE)/tool/hello/wait-pe.dbg	\
		o/$(MODE)/tool/build/elf2pe
	@$(COMPILE) -AELF2PE o/$(MODE)/tool/build/elf2pe -R 64kb -S 4kb -o $@ $<

o/$(MODE)/tool/hello/life-pe.ape.zip.o: private ZIPOBJ_FLAGS += -B

$(TOOL_HELLO_OBJS): tool/hello/BUILD.mk
