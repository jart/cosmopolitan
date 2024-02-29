#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#── vi: set noet ft=make ts=8 sw=8 fenc=utf-8 :vi ────────────────────┘

PKGS += THIRD_PARTY_ZSTD

THIRD_PARTY_ZSTD_ARTIFACTS += THIRD_PARTY_ZSTD_A
THIRD_PARTY_ZSTD = $(THIRD_PARTY_ZSTD_A_DEPS) $(THIRD_PARTY_ZSTD_A)
THIRD_PARTY_ZSTD_A = o/$(MODE)/third_party/zstd/zstd.a
THIRD_PARTY_ZSTD_A_HDRS = $(filter %.h,$(THIRD_PARTY_ZSTD_A_FILES))
THIRD_PARTY_ZSTD_A_SRCS = $(filter %.c,$(THIRD_PARTY_ZSTD_A_FILES))
THIRD_PARTY_ZSTD_A_INCS = $(filter %.inc,$(THIRD_PARTY_ZSTD_A_FILES))
THIRD_PARTY_ZSTD_A_OBJS = $(THIRD_PARTY_ZSTD_A_SRCS:%.c=o/$(MODE)/%.o)

THIRD_PARTY_ZSTD_A_FILES :=					\
third_party/zstd/zstd.h						\
third_party/zstd/zdict.h					\
third_party/zstd/zstd_errors.h					\
third_party/zstd/lib/common/allocations.h			\
third_party/zstd/lib/common/bits.h				\
third_party/zstd/lib/common/bitstream.h				\
third_party/zstd/lib/common/compiler.h				\
third_party/zstd/lib/common/cpu.h				\
third_party/zstd/lib/common/debug.c				\
third_party/zstd/lib/common/debug.h				\
third_party/zstd/lib/common/entropy_common.c			\
third_party/zstd/lib/common/error_private.c			\
third_party/zstd/lib/common/error_private.h			\
third_party/zstd/lib/common/fse.h				\
third_party/zstd/lib/common/fse_decompress.c			\
third_party/zstd/lib/common/huf.h				\
third_party/zstd/lib/common/mem.h				\
third_party/zstd/lib/common/pool.c				\
third_party/zstd/lib/common/pool.h				\
third_party/zstd/lib/common/portability_macros.h		\
third_party/zstd/lib/common/threading.c				\
third_party/zstd/lib/common/threading.h				\
third_party/zstd/lib/common/xxhash.c				\
third_party/zstd/lib/common/xxhash.h				\
third_party/zstd/lib/common/zstd_common.c			\
third_party/zstd/lib/common/zstd_deps.h				\
third_party/zstd/lib/common/zstd_internal.h			\
third_party/zstd/lib/common/zstd_trace.h			\
third_party/zstd/lib/compress/clevels.h				\
third_party/zstd/lib/compress/fse_compress.c			\
third_party/zstd/lib/compress/hist.c				\
third_party/zstd/lib/compress/hist.h				\
third_party/zstd/lib/compress/huf_compress.c			\
third_party/zstd/lib/compress/zstd_compress.c			\
third_party/zstd/lib/compress/zstd_compress_internal.h		\
third_party/zstd/lib/compress/zstd_compress_literals.c		\
third_party/zstd/lib/compress/zstd_compress_literals.h		\
third_party/zstd/lib/compress/zstd_compress_sequences.c		\
third_party/zstd/lib/compress/zstd_compress_sequences.h		\
third_party/zstd/lib/compress/zstd_compress_superblock.c	\
third_party/zstd/lib/compress/zstd_compress_superblock.h	\
third_party/zstd/lib/compress/zstd_cwksp.h			\
third_party/zstd/lib/compress/zstd_double_fast.c		\
third_party/zstd/lib/compress/zstd_double_fast.h		\
third_party/zstd/lib/compress/zstd_fast.c			\
third_party/zstd/lib/compress/zstd_fast.h			\
third_party/zstd/lib/compress/zstd_lazy.c			\
third_party/zstd/lib/compress/zstd_lazy.h			\
third_party/zstd/lib/compress/zstd_ldm.c			\
third_party/zstd/lib/compress/zstd_ldm.h			\
third_party/zstd/lib/compress/zstd_ldm_geartab.h		\
third_party/zstd/lib/compress/zstd_opt.c			\
third_party/zstd/lib/compress/zstd_opt.h			\
third_party/zstd/lib/compress/zstdmt_compress.c			\
third_party/zstd/lib/compress/zstdmt_compress.h			\
third_party/zstd/lib/decompress/huf_decompress.c		\
third_party/zstd/lib/decompress/huf_decompress_amd64.S		\
third_party/zstd/lib/decompress/zstd_ddict.c			\
third_party/zstd/lib/decompress/zstd_ddict.h			\
third_party/zstd/lib/decompress/zstd_decompress.c		\
third_party/zstd/lib/decompress/zstd_decompress_block.c		\
third_party/zstd/lib/decompress/zstd_decompress_block.h		\
third_party/zstd/lib/decompress/zstd_decompress_internal.h	\
third_party/zstd/lib/deprecated/zbuff.h				\
third_party/zstd/lib/deprecated/zbuff_common.c			\
third_party/zstd/lib/deprecated/zbuff_compress.c		\
third_party/zstd/lib/deprecated/zbuff_decompress.c		\
third_party/zstd/lib/dictBuilder/cover.c			\
third_party/zstd/lib/dictBuilder/cover.h			\
third_party/zstd/lib/dictBuilder/divsufsort.c			\
third_party/zstd/lib/dictBuilder/divsufsort.h			\
third_party/zstd/lib/dictBuilder/fastcover.c			\
third_party/zstd/lib/dictBuilder/zdict.c			\
third_party/zstd/programs/benchfn.c				\
third_party/zstd/programs/benchfn.h				\
third_party/zstd/programs/benchzstd.c				\
third_party/zstd/programs/benchzstd.h				\
third_party/zstd/programs/datagen.c				\
third_party/zstd/programs/datagen.h				\
third_party/zstd/programs/dibio.c				\
third_party/zstd/programs/dibio.h				\
third_party/zstd/programs/fileio.c				\
third_party/zstd/programs/fileio.h				\
third_party/zstd/programs/fileio_asyncio.c			\
third_party/zstd/programs/fileio_asyncio.h			\
third_party/zstd/programs/fileio_common.h			\
third_party/zstd/programs/fileio_types.h			\
third_party/zstd/programs/platform.h				\
third_party/zstd/programs/timefn.c				\
third_party/zstd/programs/timefn.h				\
third_party/zstd/programs/util.c				\
third_party/zstd/programs/util.h				\
third_party/zstd/programs/zstdcli.c				\
third_party/zstd/programs/zstdcli_trace.c			\
third_party/zstd/programs/zstdcli_trace.h

THIRD_PARTY_ZSTD_A_CHECKS =					\
	$(THIRD_PARTY_ZSTD_A).pkg				\
	$(THIRD_PARTY_ZSTD_A_HDRS:%=o/$(MODE)/%.ok)

THIRD_PARTY_ZSTD_A_DIRECTDEPS =					\
	LIBC_CALLS						\
	LIBC_FMT						\
	LIBC_INTRIN						\
	LIBC_LOG						\
	LIBC_MEM						\
	LIBC_NEXGEN32E						\
	LIBC_RUNTIME						\
        LIBC_PROC						\
	LIBC_STDIO						\
	LIBC_STR						\
	LIBC_THREAD						\
	LIBC_TIME						\
	LIBC_SYSV

THIRD_PARTY_ZSTD_A_DEPS :=					\
	$(call uniq,$(foreach x,$(THIRD_PARTY_ZSTD_A_DIRECTDEPS),$($(x))))

$(THIRD_PARTY_ZSTD_A):						\
		third_party/zstd/				\
		$(THIRD_PARTY_ZSTD_A).pkg			\
		$(THIRD_PARTY_ZSTD_A_OBJS)

$(THIRD_PARTY_ZSTD_A).pkg:					\
		$(THIRD_PARTY_ZSTD_A_OBJS)			\
		$(foreach x,$(THIRD_PARTY_ZSTD_A_DIRECTDEPS),$($(x)_A).pkg)

$(THIRD_PARTY_ZSTD_A_CHECKS): private				\
		CFLAGS +=					\
			-Wframe-larger-than=262144		\
			-Wno-comment

$(THIRD_PARTY_ZSTD_A_OBJS): private				\
		CFLAGS +=					\
			-Wframe-larger-than=262144		\
			-Wno-array-bounds			\
			-Wno-comment

$(THIRD_PARTY_ZSTD_A_OBJS): private				\
		CPPFLAGS +=					\
			-DZSTD_MULTITHREAD

o/$(MODE)/third_party/zstd/zstd.com.dbg:			\
		$(THIRD_PARTY_ZSTD)				\
		o/$(MODE)/third_party/zstd/programs/zstdcli.o	\
		$(CRT)						\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

o/$(MODE)/third_party/zstd/lib/compress/zstd_lazy.o: private	\
		CFLAGS +=					\
			-O3

o/$(MODE)/third_party/zstd/lib/compress/zstd_lazy.o:		\
		QUOTA += -C64

THIRD_PARTY_ZSTD_BINS = $(THIRD_PARTY_ZSTD_COMS) $(THIRD_PARTY_ZSTD_COMS:%=%.dbg)
THIRD_PARTY_ZSTD_COMS = o/$(MODE)/third_party/zstd/zstd.com
THIRD_PARTY_ZSTD_LIBS = $(THIRD_PARTY_ZSTD_A)
THIRD_PARTY_ZSTD_SRCS = $(foreach x,$(THIRD_PARTY_ZSTD_ARTIFACTS),$($(x)_SRCS))
THIRD_PARTY_ZSTD_INCS = $(foreach x,$(THIRD_PARTY_ZSTD_ARTIFACTS),$($(x)_INCS))
THIRD_PARTY_ZSTD_HDRS = $(foreach x,$(THIRD_PARTY_ZSTD_ARTIFACTS),$($(x)_HDRS))
THIRD_PARTY_ZSTD_OBJS = $(foreach x,$(THIRD_PARTY_ZSTD_ARTIFACTS),$($(x)_OBJS))
THIRD_PARTY_ZSTD_CHECKS = $(foreach x,$(THIRD_PARTY_ZSTD_ARTIFACTS),$($(x)_CHECKS))
$(THIRD_PARTY_ZSTD_OBJS): $(BUILD_FILES) third_party/zstd/BUILD.mk

.PHONY: o/$(MODE)/third_party/zstd
o/$(MODE)/third_party/zstd:					\
		$(THIRD_PARTY_ZSTD_BINS)			\
		$(THIRD_PARTY_ZSTD_CHECKS)
