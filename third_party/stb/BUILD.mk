#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#── vi: set noet ft=make ts=8 sw=8 fenc=utf-8 :vi ────────────────────┘

PKGS += THIRD_PARTY_STB

THIRD_PARTY_STB_SRCS = $(THIRD_PARTY_STB_A_SRCS)
THIRD_PARTY_STB_HDRS = $(THIRD_PARTY_STB_A_HDRS)

THIRD_PARTY_STB_ARTIFACTS += THIRD_PARTY_STB_A
THIRD_PARTY_STB = $(THIRD_PARTY_STB_A_DEPS) $(THIRD_PARTY_STB_A)
THIRD_PARTY_STB_A = o/$(MODE)/third_party/stb/stb.a
THIRD_PARTY_STB_A_FILES := $(wildcard third_party/stb/*)
THIRD_PARTY_STB_A_HDRS = $(filter %.h,$(THIRD_PARTY_STB_A_FILES))
THIRD_PARTY_STB_A_SRCS_S = $(filter %.S,$(THIRD_PARTY_STB_A_FILES))
THIRD_PARTY_STB_A_SRCS_C = $(filter %.c,$(THIRD_PARTY_STB_A_FILES))
THIRD_PARTY_STB_A_OBJS_S = $(THIRD_PARTY_STB_A_SRCS_S:%.S=o/$(MODE)/%.o)
THIRD_PARTY_STB_A_OBJS_C = $(THIRD_PARTY_STB_A_SRCS_C:%.c=o/$(MODE)/%.o)

THIRD_PARTY_STB_A_SRCS =				\
	$(THIRD_PARTY_STB_A_SRCS_S)			\
	$(THIRD_PARTY_STB_A_SRCS_C)

THIRD_PARTY_STB_A_OBJS =				\
	$(THIRD_PARTY_STB_A_OBJS_S)			\
	$(THIRD_PARTY_STB_A_OBJS_C)

THIRD_PARTY_STB_A_DIRECTDEPS =				\
	DSP_CORE					\
	LIBC_FMT					\
	LIBC_INTRIN					\
	LIBC_LOG					\
	LIBC_MEM					\
	LIBC_NEXGEN32E					\
	LIBC_RUNTIME					\
	LIBC_STDIO					\
	LIBC_STR					\
	LIBC_TINYMATH					\
	LIBC_X						\
	THIRD_PARTY_ZLIB

THIRD_PARTY_STB_A_DEPS :=				\
	$(call uniq,$(foreach x,$(THIRD_PARTY_STB_A_DIRECTDEPS),$($(x))))

THIRD_PARTY_STB_A_CHECKS =				\
	$(THIRD_PARTY_STB_A).pkg			\
	$(THIRD_PARTY_STB_A_HDRS:%=o/$(MODE)/%.ok)

$(THIRD_PARTY_STB_A):					\
		third_party/stb/			\
		$(THIRD_PARTY_STB_A).pkg		\
		$(THIRD_PARTY_STB_A_OBJS)

$(THIRD_PARTY_STB_A).pkg:				\
		$(THIRD_PARTY_STB_A_OBJS)		\
		$(foreach x,$(THIRD_PARTY_STB_A_DIRECTDEPS),$($(x)_A).pkg)

$(THIRD_PARTY_STB_A_OBJS): private			\
		CFLAGS +=				\
			-ffunction-sections		\
			-fdata-sections

o/$(MODE)/third_party/stb/stb_vorbis.o: private		\
		CPPFLAGS +=				\
			-DSTACK_FRAME_UNLIMITED

o/$(MODE)/third_party/stb/stb_truetype.o: private	\
		CFLAGS +=				\
			-Os

THIRD_PARTY_STB_LIBS = $(foreach x,$(THIRD_PARTY_STB_ARTIFACTS),$($(x)))
THIRD_PARTY_STB_SRCS = $(foreach x,$(THIRD_PARTY_STB_ARTIFACTS),$($(x)_SRCS))
THIRD_PARTY_STB_CHECKS = $(foreach x,$(THIRD_PARTY_STB_ARTIFACTS),$($(x)_CHECKS))
THIRD_PARTY_STB_OBJS = $(foreach x,$(THIRD_PARTY_STB_ARTIFACTS),$($(x)_OBJS))
$(THIRD_PARTY_STB_OBJS): $(BUILD_FILES) third_party/stb/BUILD.mk

.PHONY: o/$(MODE)/third_party/stb
o/$(MODE)/third_party/stb: $(THIRD_PARTY_STB_CHECKS)
