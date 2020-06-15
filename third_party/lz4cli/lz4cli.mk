#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#───vi: set et ft=make ts=8 tw=8 fenc=utf-8 :vi───────────────────────┘
#
# WARNING
#
#   This is GPL program. It's intended for compressing files.
#   Cosmopolitan provides a 116 byte LZ4 runtime for extracting them.
#   Consider using that before linking this.
#
# SEE ALSO
#
#   libc/nexgen32e/lz4cpy.c

PKGS += THIRD_PARTY_LZ4CLI

THIRD_PARTY_LZ4CLI =					\
	o/$(MODE)/third_party/lz4cli/lz4cli.com

THIRD_PARTY_LZ4CLI_OBJS =				\
	o/$(MODE)/third_party/lz4cli/bench.o		\
	o/$(MODE)/third_party/lz4cli/lz4.o		\
	o/$(MODE)/third_party/lz4cli/lz4cli.o		\
	o/$(MODE)/third_party/lz4cli/lz4io.o		\
	o/$(MODE)/third_party/lz4cli/lz4hc.o		\
	o/$(MODE)/third_party/lz4cli/lz4frame.o		\
	o/$(MODE)/third_party/lz4cli/datagen.o		\
	o/$(MODE)/third_party/lz4cli/xxhash.o		\
	o/$(MODE)/third_party/lz4cli/bench.c.zip.o	\
	o/$(MODE)/third_party/lz4cli/lz4.c.zip.o	\
	o/$(MODE)/third_party/lz4cli/lz4cli.c.zip.o	\
	o/$(MODE)/third_party/lz4cli/lz4io.c.zip.o	\
	o/$(MODE)/third_party/lz4cli/lz4hc.c.zip.o	\
	o/$(MODE)/third_party/lz4cli/lz4frame.c.zip.o	\
	o/$(MODE)/third_party/lz4cli/datagen.c.zip.o	\
	o/$(MODE)/third_party/lz4cli/xxhash.c.zip.o

o/$(MODE)/third_party/lz4cli/lz4.o			\
o/$(MODE)/third_party/lz4cli/lz4io.o			\
o/$(MODE)/third_party/lz4cli/lz4hc.o			\
o/$(MODE)/third_party/lz4cli/lz4frame.o			\
o/$(MODE)/third_party/lz4cli/datagen.o:			\
	DEFAULT_CPPFLAGS +=				\
		-DSTACK_FRAME_UNLIMITED

THIRD_PARTY_LZ4CLI_DIRECTDEPS =				\
	LIBC_STDIO					\
	LIBC_TIME					\
	LIBC_CALLS_HEFTY				\
	LIBC_UNICODE

THIRD_PARTY_LZ4CLI_DEPS :=				\
	$(call uniq,$(foreach x,$(THIRD_PARTY_LZ4CLI_DIRECTDEPS),$($(x))))

$(THIRD_PARTY_LZ4CLI_OBJS):				\
	DEFAULT_CPPFLAGS +=				\
		-isystem third_party/lz4cli

o/$(MODE)/third_party/lz4cli/lz4cli.com.dbg:		\
		$(THIRD_PARTY_LZ4CLI_DEPS)		\
		$(THIRD_PARTY_LZ4CLI_OBJS)		\
		$(CRT)					\
		$(APE)
	@$(APELINK)

$(THIRD_PARTY_LZ4CLI_OBJS):				\
		$(BUILD_FILES)				\
		third_party/lz4cli/lz4cli.mk

.PHONY: o/$(MODE)/third_party/lz4cli
o/$(MODE)/third_party/lz4cli: $(THIRD_PARTY_LZ4CLI)
