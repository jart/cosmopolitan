#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#───vi: set et ft=make ts=8 tw=8 fenc=utf-8 :vi───────────────────────┘

PKGS += LIBC_UNICODE

LIBC_UNICODE_ARTIFACTS += LIBC_UNICODE_A
LIBC_UNICODE = $(LIBC_UNICODE_A_DEPS) $(LIBC_UNICODE_A)
LIBC_UNICODE_A = o/$(MODE)/libc/unicode/unicode.a
LIBC_UNICODE_A_FILES := $(wildcard libc/unicode/*)
LIBC_UNICODE_A_HDRS = $(filter %.h,$(LIBC_UNICODE_A_FILES))
LIBC_UNICODE_A_SRCS_A = $(filter %.s,$(LIBC_UNICODE_A_FILES))
LIBC_UNICODE_A_SRCS_S = $(filter %.S,$(LIBC_UNICODE_A_FILES))
LIBC_UNICODE_A_SRCS_C = $(filter %.c,$(LIBC_UNICODE_A_FILES))

LIBC_UNICODE_A_SRCS =						\
	$(LIBC_UNICODE_A_SRCS_A)				\
	$(LIBC_UNICODE_A_SRCS_S)				\
	$(LIBC_UNICODE_A_SRCS_C)

LIBC_UNICODE_A_OBJS =						\
	$(LIBC_UNICODE_A_SRCS:%=o/$(MODE)/%.zip.o)		\
	$(LIBC_UNICODE_A_SRCS_A:%.s=o/$(MODE)/%.o)		\
	$(LIBC_UNICODE_A_SRCS_S:%.S=o/$(MODE)/%.o)		\
	$(LIBC_UNICODE_A_SRCS_C:%.c=o/$(MODE)/%.o)

LIBC_UNICODE_A_CHECKS =						\
	$(LIBC_UNICODE_A).pkg					\
	$(LIBC_UNICODE_A_HDRS:%=o/$(MODE)/%.ok)

LIBC_UNICODE_A_DIRECTDEPS =					\
	LIBC_STUBS						\
	LIBC_NEXGEN32E						\
	LIBC_RUNTIME						\
	LIBC_STR						\
	LIBC_SYSV

LIBC_UNICODE_A_DEPS :=						\
	$(call uniq,$(foreach x,$(LIBC_UNICODE_A_DIRECTDEPS),$($(x))))

$(LIBC_UNICODE_A):						\
		libc/unicode/					\
		$(LIBC_UNICODE_A).pkg				\
		$(LIBC_UNICODE_A_OBJS)

$(LIBC_UNICODE_A).pkg:						\
		$(LIBC_UNICODE_A_OBJS)				\
		$(foreach x,$(LIBC_UNICODE_A_DIRECTDEPS),$($(x)_A).pkg)

LIBC_UNICODE_LIBS = $(foreach x,$(LIBC_UNICODE_ARTIFACTS),$($(x)))
LIBC_UNICODE_SRCS = $(foreach x,$(LIBC_UNICODE_ARTIFACTS),$($(x)_SRCS))
LIBC_UNICODE_HDRS = $(foreach x,$(LIBC_UNICODE_ARTIFACTS),$($(x)_HDRS))
LIBC_UNICODE_BINS = $(foreach x,$(LIBC_UNICODE_ARTIFACTS),$($(x)_BINS))
LIBC_UNICODE_CHECKS = $(foreach x,$(LIBC_UNICODE_ARTIFACTS),$($(x)_CHECKS))
LIBC_UNICODE_OBJS = $(foreach x,$(LIBC_UNICODE_ARTIFACTS),$($(x)_OBJS))
LIBC_UNICODE_TESTS = $(foreach x,$(LIBC_UNICODE_ARTIFACTS),$($(x)_TESTS))
$(LIBC_UNICODE_OBJS): $(BUILD_FILES) libc/unicode/unicode.mk

.PHONY: o/$(MODE)/libc/unicode
o/$(MODE)/libc/unicode: $(LIBC_UNICODE) $(LIBC_UNICODE_CHECKS)

o/$(MODE)/libc/unicode/eastasianwidth.bin:			\
		libc/unicode/eastasianwidth.txt			\
		o/$(MODE)/tool/decode/mkwides.com
	@TARGET=$@ ACTION=MKWIDES build/do			\
		o/$(MODE)/tool/decode/mkwides.com -o $@ $<
o/$(MODE)/libc/unicode/eastasianwidth.bin.lz4:			\
		o/$(MODE)/libc/unicode/eastasianwidth.bin	\
		o/$(MODE)/third_party/lz4cli/lz4cli.com
	@TARGET=$@ ACTION=LZ4 build/do				\
		o/$(MODE)/third_party/lz4cli/lz4cli.com -q -f -9 --content-size $< $@
o/$(MODE)/libc/unicode/eastasianwidth.s:			\
		o/$(MODE)/libc/unicode/eastasianwidth.bin.lz4	\
		o/$(MODE)/tool/build/lz4toasm.com
	@TARGET=$@ ACTION=BIN2ASM build/do			\
		o/$(MODE)/tool/build/lz4toasm.com -s kEastAsianWidth -o $@ $<

o/$(MODE)/libc/unicode/combiningchars.bin:			\
		libc/unicode/unicodedata.txt			\
		o/$(MODE)/tool/decode/mkcombos.com
	@TARGET=$@ ACTION=MKCOMBOS build/do			\
		o/$(MODE)/tool/decode/mkcombos.com -o $@ $<
o/$(MODE)/libc/unicode/combiningchars.bin.lz4:			\
		o/$(MODE)/libc/unicode/combiningchars.bin	\
		o/$(MODE)/third_party/lz4cli/lz4cli.com
	@TARGET=$@ ACTION=LZ4 build/do				\
		o/$(MODE)/third_party/lz4cli/lz4cli.com -q -f -9 --content-size $< $@
o/$(MODE)/libc/unicode/combiningchars.s:			\
		o/$(MODE)/libc/unicode/combiningchars.bin.lz4	\
		o/$(MODE)/tool/build/lz4toasm.com
	@TARGET=$@ ACTION=BIN2ASM build/do			\
		o/$(MODE)/tool/build/lz4toasm.com -s kCombiningChars -o $@ $<
