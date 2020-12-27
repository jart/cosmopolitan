#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#───vi: set et ft=make ts=8 tw=8 fenc=utf-8 :vi───────────────────────┘

PKGS += TOOL_VIZ_LIB

TOOL_VIZ_LIB_ARTIFACTS += TOOL_VIZ_LIB_A
TOOL_VIZ_LIB = $(TOOL_VIZ_LIB_A_DEPS) $(TOOL_VIZ_LIB_A)
TOOL_VIZ_LIB_A = o/$(MODE)/tool/viz/lib/vizlib.a
TOOL_VIZ_LIB_A_HDRS = $(filter %.h,$(TOOL_VIZ_LIB_A_FILES))
TOOL_VIZ_LIB_A_SRCS_S = $(filter %.S,$(TOOL_VIZ_LIB_A_FILES))
TOOL_VIZ_LIB_A_SRCS_C = $(filter %.c,$(TOOL_VIZ_LIB_A_FILES))
TOOL_VIZ_LIB_A_CHECKS = $(TOOL_VIZ_LIB_A).pkg

TOOL_VIZ_LIB_A_FILES :=					\
	$(wildcard tool/viz/lib/*)			\
	$(wildcard tool/viz/lib/thunks/*)

TOOL_VIZ_LIB_A_SRCS =					\
	$(TOOL_VIZ_LIB_A_SRCS_S)			\
	$(TOOL_VIZ_LIB_A_SRCS_C)

TOOL_VIZ_LIB_A_OBJS =					\
	$(TOOL_VIZ_LIB_A_SRCS:%=o/$(MODE)/%.zip.o)	\
	$(TOOL_VIZ_LIB_A_SRCS_S:%.S=o/$(MODE)/%.o)	\
	$(TOOL_VIZ_LIB_A_SRCS_C:%.c=o/$(MODE)/%.o)

TOOL_VIZ_LIB_A_DIRECTDEPS =				\
	DSP_CORE					\
	DSP_SCALE					\
	DSP_TTY						\
	LIBC_ALG					\
	LIBC_BITS					\
	LIBC_CALLS					\
	LIBC_FMT					\
	LIBC_INTRIN					\
	LIBC_LOG					\
	LIBC_MEM					\
	LIBC_NEXGEN32E					\
	LIBC_RUNTIME					\
	LIBC_STDIO					\
	LIBC_STR					\
	LIBC_STUBS					\
	LIBC_SYSV					\
	LIBC_TESTLIB					\
	LIBC_TIME					\
	LIBC_TINYMATH					\
	LIBC_UNICODE					\
	LIBC_X						\
	THIRD_PARTY_GDTOA				\
	THIRD_PARTY_DLMALLOC

TOOL_VIZ_LIB_A_DEPS :=					\
	$(call uniq,$(foreach x,$(TOOL_VIZ_LIB_A_DIRECTDEPS),$($(x))))

o/$(MODE)/tool/viz/lib/pmaddubsw.o:			\
		OVERRIDE_CFLAGS +=			\
			-fvect-cost-model=unlimited

o/$(MODE)/tool/viz/lib/scale.o				\
o/$(MODE)/tool/viz/lib/writetoframebuffer.o		\
o/$(MODE)/tool/viz/lib/lab.o				\
o/$(MODE)/tool/viz/lib/xyz.o				\
o/$(MODE)/tool/viz/lib/doublechrominance.o		\
o/$(MODE)/tool/viz/lib/doublechrominance.o		\
o/$(MODE)/tool/viz/lib/doublechrominance.o		\
o/$(MODE)/tool/viz/lib/interlace.o			\
o/$(MODE)/tool/viz/lib/bilinearscale.o			\
o/$(MODE)/tool/viz/lib/oldbilinearscale.o		\
o/$(MODE)/tool/viz/lib/boxblur.o			\
o/$(MODE)/tool/viz/lib/dither.o				\
o/$(MODE)/tool/viz/lib/emboss.o				\
o/$(MODE)/tool/viz/lib/getxtermcodes.o			\
o/$(MODE)/tool/viz/lib/lingamma.o			\
o/$(MODE)/tool/viz/lib/perlin3.o			\
o/$(MODE)/tool/viz/lib/resizegraphic.o:			\
		OVERRIDE_CFLAGS +=			\
			-DSTACK_FRAME_UNLIMITED		\
			$(MATHEMATICAL)

o/$(MODE)/tool/viz/lib/printmatrix.o:			\
		OVERRIDE_CFLAGS +=			\
			$(IEEE_MATH)

$(TOOL_VIZ_LIB_A):					\
		tool/viz/lib/				\
		$(TOOL_VIZ_LIB_A).pkg			\
		$(TOOL_VIZ_LIB_A_OBJS)

$(TOOL_VIZ_LIB_A).pkg:					\
		$(TOOL_VIZ_LIB_A_OBJS)			\
		$(foreach x,$(TOOL_VIZ_LIB_A_DIRECTDEPS),$($(x)_A).pkg)

# ifeq (,$(MODE))
# $(TOOL_VIZ_LIB_A_OBJS):					\
# 		OVERRIDE_CFLAGS +=			\
# 			-fsanitize=address
# endif

$(TOOL_VIZ_LIB_A_OBJS): tool/viz/lib/vizlib.mk

TOOL_VIZ_LIB_LIBS = $(foreach x,$(TOOL_VIZ_LIB_ARTIFACTS),$($(x)))
TOOL_VIZ_LIB_SRCS = $(foreach x,$(TOOL_VIZ_LIB_ARTIFACTS),$($(x)_SRCS))
TOOL_VIZ_LIB_HDRS = $(foreach x,$(TOOL_VIZ_LIB_ARTIFACTS),$($(x)_HDRS))
TOOL_VIZ_LIB_BINS = $(foreach x,$(TOOL_VIZ_LIB_ARTIFACTS),$($(x)_BINS))
TOOL_VIZ_LIB_CHECKS = $(foreach x,$(TOOL_VIZ_LIB_ARTIFACTS),$($(x)_CHECKS))
TOOL_VIZ_LIB_OBJS = $(foreach x,$(TOOL_VIZ_LIB_ARTIFACTS),$($(x)_OBJS))
TOOL_VIZ_LIB_TESTS = $(foreach x,$(TOOL_VIZ_LIB_ARTIFACTS),$($(x)_TESTS))

.PHONY: o/$(MODE)/tool/viz/lib
o/$(MODE)/tool/viz/lib: $(TOOL_VIZ_LIB_CHECKS)
