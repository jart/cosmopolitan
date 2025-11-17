#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#── vi: set noet ft=make ts=8 sw=8 fenc=utf-8 :vi ────────────────────┘

PKGS += THIRD_PARTY_HACLSTAR

THIRD_PARTY_HACLSTAR_ARTIFACTS += THIRD_PARTY_HACLSTAR_A
THIRD_PARTY_HACLSTAR = $(THIRD_PARTY_HACLSTAR_A_DEPS) $(THIRD_PARTY_HACLSTAR_A)
THIRD_PARTY_HACLSTAR_A = o/$(MODE)/third_party/haclstar/haclstar.a
THIRD_PARTY_HACLSTAR_A_FILES := $(wildcard third_party/haclstar/*)
THIRD_PARTY_HACLSTAR_A_HDRS = $(filter %.h,$(THIRD_PARTY_HACLSTAR_A_FILES))
THIRD_PARTY_HACLSTAR_A_SRCS_C = $(filter %.c,$(THIRD_PARTY_HACLSTAR_A_FILES))
THIRD_PARTY_HACLSTAR_A_SRCS_S = $(filter %.S,$(THIRD_PARTY_HACLSTAR_A_FILES))
THIRD_PARTY_HACLSTAR_A_INCS = $(filter %.inc,$(THIRD_PARTY_HACLSTAR_A_FILES))

THIRD_PARTY_HACLSTAR_A_SRCS =						\
	$(THIRD_PARTY_HACLSTAR_A_SRCS_C)				\
	$(THIRD_PARTY_HACLSTAR_A_SRCS_S)				\

THIRD_PARTY_HACLSTAR_A_OBJS =						\
	$(THIRD_PARTY_HACLSTAR_A_SRCS_C:%.c=o/$(MODE)/%.o)		\
	$(THIRD_PARTY_HACLSTAR_A_SRCS_S:%.S=o/$(MODE)/%.o)		\

THIRD_PARTY_HACLSTAR_A_CHECKS =						\
	$(THIRD_PARTY_HACLSTAR_A).pkg					\
	$(THIRD_PARTY_HACLSTAR_A_HDRS:%=o/$(MODE)/%.ok)			\

THIRD_PARTY_HACLSTAR_A_DIRECTDEPS =					\
	LIBC_CALLS							\
	LIBC_INTRIN							\
	LIBC_MEM							\
	LIBC_NEXGEN32E							\
	LIBC_STR							\
	LIBC_SYSV							\

THIRD_PARTY_HACLSTAR_A_DEPS :=						\
	$(call uniq,$(foreach x,$(THIRD_PARTY_HACLSTAR_A_DIRECTDEPS),$($(x))))

$(THIRD_PARTY_HACLSTAR_A):						\
		third_party/haclstar/					\
		$(THIRD_PARTY_HACLSTAR_A).pkg				\
		$(THIRD_PARTY_HACLSTAR_A_OBJS)

$(THIRD_PARTY_HACLSTAR_A).pkg:						\
		$(THIRD_PARTY_HACLSTAR_A_OBJS)				\
		$(foreach x,$(THIRD_PARTY_HACLSTAR_A_DIRECTDEPS),$($(x)_A).pkg)

$(THIRD_PARTY_HACLSTAR_A_OBJS): private					\
		COPTS +=						\
			-fdata-sections					\
			-ffunction-sections				\

# HACL* generates such bloated code for SHA2-512, that using -Os will
# actually make it go 33% faster on AMD Ryzen Threadripper PRO 7995WX
# compiling it in dbg modes will cause the stack frame to exceed 4096
# using ubsan on these files takes like, literally minutes to compile
o/$(MODE)/third_party/haclstar/blake2b.o				\
o/$(MODE)/third_party/haclstar/sha256.o					\
o/$(MODE)/third_party/haclstar/sha512.o: private			\
		OVERRIDE_CFLAGS +=					\
			-Os						\
			-fno-sanitize=all				\

# non-optimized blake2b transform is also bloated and can blow the stack
o/$(MODE)/third_party/haclstar/blake2b.o: private			\
		OVERRIDE_CFLAGS +=					\
			-O2						\

# size optimization is nice to have and we haven't observed slowdowns
o/$(MODE)/third_party/haclstar/sha3.o: private				\
		OVERRIDE_CFLAGS +=					\
			-Os						\

# heavy optimization helps somewhat here
o/$(MODE)/third_party/haclstar/curve25519.o: private			\
		OVERRIDE_CFLAGS +=					\
			-O3						\

ifeq ($(ARCH), x86_64)

o/$(MODE)/third_party/haclstar/poly1305simd128.o			\
o/$(MODE)/third_party/haclstar/chacha20simd128.o: private		\
		OVERRIDE_CFLAGS +=					\
			-msse4.2

o/$(MODE)/third_party/haclstar/poly1305simd256.o			\
o/$(MODE)/third_party/haclstar/chacha20simd256.o			\
o/$(MODE)/third_party/haclstar/chacha20poly1305simd256.o: private	\
		OVERRIDE_CFLAGS +=					\
			-mavx2

endif

THIRD_PARTY_HACLSTAR_LIBS = $(foreach x,$(THIRD_PARTY_HACLSTAR_ARTIFACTS),$($(x)))
THIRD_PARTY_HACLSTAR_SRCS = $(foreach x,$(THIRD_PARTY_HACLSTAR_ARTIFACTS),$($(x)_SRCS))
THIRD_PARTY_HACLSTAR_HDRS = $(foreach x,$(THIRD_PARTY_HACLSTAR_ARTIFACTS),$($(x)_HDRS))
THIRD_PARTY_HACLSTAR_INCS = $(foreach x,$(THIRD_PARTY_HACLSTAR_ARTIFACTS),$($(x)_INCS))
THIRD_PARTY_HACLSTAR_BINS = $(foreach x,$(THIRD_PARTY_HACLSTAR_ARTIFACTS),$($(x)_BINS))
THIRD_PARTY_HACLSTAR_CHECKS = $(foreach x,$(THIRD_PARTY_HACLSTAR_ARTIFACTS),$($(x)_CHECKS))
THIRD_PARTY_HACLSTAR_OBJS = $(foreach x,$(THIRD_PARTY_HACLSTAR_ARTIFACTS),$($(x)_OBJS))
$(THIRD_PARTY_HACLSTAR_OBJS): $(BUILD_FILES) third_party/haclstar/BUILD.mk

.PHONY: o/$(MODE)/third_party/haclstar
o/$(MODE)/third_party/haclstar:						\
		$(THIRD_PARTY_HACLSTAR_A)				\
		$(THIRD_PARTY_HACLSTAR_CHECKS)
