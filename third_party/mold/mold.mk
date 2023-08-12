#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#───vi: set et ft=make ts=8 tw=8 fenc=utf-8 :vi───────────────────────┘

PKGS += THIRD_PARTY_MOLD

THIRD_PARTY_MOLD_ARTIFACTS += THIRD_PARTY_MOLD_A
THIRD_PARTY_MOLD = $(THIRD_PARTY_MOLD_A_DEPS) $(THIRD_PARTY_MOLD_A)
THIRD_PARTY_MOLD_A = o/$(MODE)/third_party/mold/mold.a
THIRD_PARTY_MOLD_FILES := $(wildcard third_party/mold/*) $(wildcard third_party/mold/elf/*)
THIRD_PARTY_MOLD_HDRS = $(filter %.h,$(THIRD_PARTY_MOLD_FILES))
THIRD_PARTY_MOLD_SRCS = $(filter %.cc,$(THIRD_PARTY_MOLD_FILES))
THIRD_PARTY_MOLD_OBJS = $(THIRD_PARTY_MOLD_SRCS:%.cc=o/$(MODE)/%.o)

THIRD_PARTY_MOLD_A_DIRECTDEPS =				\
	THIRD_PARTY_LIBCXX					\
	LIBC_STR							\
	LIBC_INTRIN                 		\
	LIBC_STDIO							\
	LIBC_CALLS							\
	LIBC_TINYMATH						\
	LIBC_SYSV							\
	LIBC_RUNTIME              			\
	THIRD_PARTY_ZSTD					\
	THIRD_PARTY_XXHASH					\
	THIRD_PARTY_GETOPT					\
	THIRD_PARTY_ZLIB

THIRD_PARTY_MOLD_A_DEPS :=				\
	$(call uniq,$(foreach x,$(THIRD_PARTY_MOLD_A_DIRECTDEPS),$($(x))))

# https://github.com/rui314/mold/blob/d4d93d7fb72dd19c44aafa4dd5397e35787d33ad/CMakeLists.txt#L62
# TODO(fzakaria): figure out solution for -Wno-error=class-memaccess 
$(THIRD_PARTY_MOLD_OBJS): private			\
		CPPFLAGS +=							\
			-std=gnu++20					\
			-fno-exceptions					\
			-fno-unwind-tables				\
			-fno-asynchronous-unwind-tables \
			-Wno-sign-compare				\
			-Wno-unused-function			\
			-DMOLD_X86_64=1					\
			-DMOLD_TARGET=X86_64

THIRD_PARTY_MOLD_CHECKS =				\
	$(THIRD_PARTY_MOLD_A).pkg			\
	$(THIRD_PARTY_MOLD_HDRS:%=o/$(MODE)/%.ok)

$(THIRD_PARTY_MOLD_A):					\
		third_party/mold/			\
		$(THIRD_PARTY_MOLD_A).pkg		\
		$(THIRD_PARTY_MOLD_OBJS)

$(THIRD_PARTY_MOLD_A).pkg:				\
		$(THIRD_PARTY_MOLD_OBJS)		\
		$(foreach x,$(THIRD_PARTY_MOLD_A_DIRECTDEPS),$($(x)_A).pkg)

$(THIRD_PARTY_MOLD_A_OBJS): private			\
		CPPFLAGS +=				\
			-std=c++20

o/$(MODE)/third_party/mold/mold.com.dbg:				\
		$(THIRD_PARTY_MOLD)				\
		o/$(MODE)/third_party/awk/main.o		\
		$(CRT)					\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)
 
THIRD_PARTY_MOLD_COMS = o/$(MODE)/third_party/mold/mold.com
THIRD_PARTY_MOLD_BINS = $(THIRD_PARTY_MOLD_COMS) $(THIRD_PARTY_MOLD_COMS:%=%.dbg)
THIRD_PARTY_MOLD_LIBS = $(THIRD_PARTY_MOLD_A)
$(THIRD_PARTY_MOLD_OBJS): $(BUILD_FILES) third_party/mold/mold.mk

.PHONY: o/$(MODE)/third_party/mold
o/$(MODE)/third_party/mold:						\
		$(THIRD_PARTY_MOLD_BINS)				\
		$(THIRD_PARTY_AWK_CHECKS)
