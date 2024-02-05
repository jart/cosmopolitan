#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#── vi: set noet ft=make ts=8 sw=8 fenc=utf-8 :vi ────────────────────┘

PKGS += THIRD_PARTY_LIBUNWIND

THIRD_PARTY_LIBUNWIND_ARTIFACTS += THIRD_PARTY_LIBUNWIND_A
THIRD_PARTY_LIBUNWIND = $(THIRD_PARTY_LIBUNWIND_A_DEPS) $(THIRD_PARTY_LIBUNWIND_A)
THIRD_PARTY_LIBUNWIND_A = o/$(MODE)/third_party/libunwind/libunwind.a

THIRD_PARTY_LIBUNWIND_A_INCS =						\
	third_party/libunwind/include/unwind_arm_ehabi.h		\
	third_party/libunwind/include/unwind_itanium.h			\
	third_party/libunwind/AddressSpace.hpp				\
	third_party/libunwind/FrameHeaderCache.hpp			\
	third_party/libunwind/RWMutex.hpp				\
	third_party/libunwind/UnwindCursor.hpp

THIRD_PARTY_LIBUNWIND_A_HDRS =						\
	third_party/libunwind/include/mach-o/compact_unwind_encoding.h	\
	third_party/libunwind/include/__libunwind_config.h		\
	third_party/libunwind/include/libunwind.h			\
	third_party/libunwind/include/unwind.h				\
	third_party/libunwind/config.h					\
	third_party/libunwind/cet_unwind.h				\
	third_party/libunwind/dwarf2.h					\
	third_party/libunwind/libunwind_ext.h				\
	third_party/libunwind/CompactUnwinder.hpp			\
	third_party/libunwind/DwarfInstructions.hpp			\
	third_party/libunwind/DwarfParser.hpp				\
	third_party/libunwind/EHHeaderParser.hpp			\
	third_party/libunwind/Registers.hpp				\
	third_party/libunwind/Unwind-EHABI.h

THIRD_PARTY_LIBUNWIND_A_SRCS_CC =					\
	third_party/libunwind/libunwind.cc

THIRD_PARTY_LIBUNWIND_A_SRCS_C =					\
	third_party/libunwind/Unwind-sjlj.c				\
	third_party/libunwind/UnwindLevel1-gcc-ext.c			\
	third_party/libunwind/UnwindLevel1.c				\
	third_party/libunwind/gcc_personality_v0.c

THIRD_PARTY_LIBUNWIND_A_SRCS =						\
	$(THIRD_PARTY_LIBUNWIND_A_SRCS_C)				\
	$(THIRD_PARTY_LIBUNWIND_A_SRCS_CC)

THIRD_PARTY_LIBUNWIND_A_OBJS =						\
	$(THIRD_PARTY_LIBUNWIND_A_SRCS_C:%.c=o/$(MODE)/%.o)		\
	$(THIRD_PARTY_LIBUNWIND_A_SRCS_CC:%.cc=o/$(MODE)/%.o)

THIRD_PARTY_LIBUNWIND_A_CHECKS =					\
	$(THIRD_PARTY_LIBUNWIND_A).pkg					\
	$(THIRD_PARTY_LIBUNWIND_A_HDRS:%=o/$(MODE)/%.okk)

THIRD_PARTY_LIBUNWIND_A_DIRECTDEPS =					\
	LIBC_CALLS							\
	LIBC_INTRIN							\
	LIBC_STDIO

THIRD_PARTY_LIBUNWIND_A_DEPS :=						\
	$(call uniq,$(foreach x,$(THIRD_PARTY_LIBUNWIND_A_DIRECTDEPS),$($(x))))

$(THIRD_PARTY_LIBUNWIND_A):						\
		third_party/libunwind/					\
		$(THIRD_PARTY_LIBUNWIND_A).pkg				\
		$(THIRD_PARTY_LIBUNWIND_A_OBJS)

$(THIRD_PARTY_LIBUNWIND_A).pkg:						\
		$(THIRD_PARTY_LIBUNWIND_A_OBJS)				\
		$(foreach x,$(THIRD_PARTY_LIBUNWIND_A_DIRECTDEPS),$($(x)_A).pkg)

$(THIRD_PARTY_LIBUNWIND_A_OBJS): private				\
		CFLAGS +=						\
			-fexceptions					\
			-fno-sanitize=all				\
			-ffunction-sections				\
			-fdata-sections					\
			-D_LIBUNWIND_USE_DLADDR=0

$(THIRD_PARTY_LIBUNWIND_A_OBJS): private				\
		CXXFLAGS +=						\
			-fexceptions					\
			-fno-sanitize=all				\
			-ffunction-sections				\
			-fdata-sections					\
			-D_LIBUNWIND_USE_DLADDR=0

THIRD_PARTY_LIBUNWIND_LIBS = $(foreach x,$(THIRD_PARTY_LIBUNWIND_ARTIFACTS),$($(x)))
THIRD_PARTY_LIBUNWIND_SRCS = $(foreach x,$(THIRD_PARTY_LIBUNWIND_ARTIFACTS),$($(x)_SRCS))
THIRD_PARTY_LIBUNWIND_HDRS = $(foreach x,$(THIRD_PARTY_LIBUNWIND_ARTIFACTS),$($(x)_HDRS))
THIRD_PARTY_LIBUNWIND_INCS = $(foreach x,$(THIRD_PARTY_LIBUNWIND_ARTIFACTS),$($(x)_INCS))
THIRD_PARTY_LIBUNWIND_CHECKS = $(foreach x,$(THIRD_PARTY_LIBUNWIND_ARTIFACTS),$($(x)_CHECKS))
THIRD_PARTY_LIBUNWIND_OBJS = $(foreach x,$(THIRD_PARTY_LIBUNWIND_ARTIFACTS),$($(x)_OBJS))

.PHONY: o/$(MODE)/third_party/libunwind
o/$(MODE)/third_party/libunwind: \
	$(THIRD_PARTY_LIBUNWIND_CHECKS)	\
	$(THIRD_PARTY_LIBUNWIND_A)
