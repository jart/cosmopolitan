#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#───vi: set et ft=make ts=8 tw=8 fenc=utf-8 :vi───────────────────────┘

PKGS += LIBC_STR

LIBC_STR_ARTIFACTS += LIBC_STR_A
LIBC_STR = $(LIBC_STR_A_DEPS) $(LIBC_STR_A)
LIBC_STR_A = o/$(MODE)/libc/str/str.a
LIBC_STR_A_FILES := $(wildcard libc/str/*)
LIBC_STR_A_HDRS = $(filter %.h,$(LIBC_STR_A_FILES))
LIBC_STR_A_INCS = $(filter %.inc,$(LIBC_STR_A_FILES))
LIBC_STR_A_SRCS_A = $(filter %.s,$(LIBC_STR_A_FILES))
LIBC_STR_A_SRCS_S = $(filter %.S,$(LIBC_STR_A_FILES))
LIBC_STR_A_SRCS_C = $(filter %.c,$(LIBC_STR_A_FILES))

LIBC_STR_A_SRCS =						\
	$(LIBC_STR_A_SRCS_A)					\
	$(LIBC_STR_A_SRCS_S)					\
	$(LIBC_STR_A_SRCS_C)

LIBC_STR_A_OBJS =						\
	$(LIBC_STR_A_SRCS_A:%.s=o/$(MODE)/%.o)			\
	$(LIBC_STR_A_SRCS_S:%.S=o/$(MODE)/%.o)			\
	$(LIBC_STR_A_SRCS_C:%.c=o/$(MODE)/%.o)

LIBC_STR_A_CHECKS =						\
	$(LIBC_STR_A).pkg					\
	$(LIBC_STR_A_HDRS:%=o/$(MODE)/%.ok)

LIBC_STR_A_DIRECTDEPS =						\
	LIBC_INTRIN						\
	LIBC_STUBS						\
	LIBC_SYSV						\
	LIBC_NEXGEN32E						\
	THIRD_PARTY_COMPILER_RT

LIBC_STR_A_DEPS :=						\
	$(call uniq,$(foreach x,$(LIBC_STR_A_DIRECTDEPS),$($(x))))

$(LIBC_STR_A):	libc/str/					\
		$(LIBC_STR_A).pkg				\
		$(LIBC_STR_A_OBJS)

$(LIBC_STR_A).pkg:						\
		$(LIBC_STR_A_OBJS)				\
		$(foreach x,$(LIBC_STR_A_DIRECTDEPS),$($(x)_A).pkg)

o/$(MODE)/libc/str/wmemset.o					\
o/$(MODE)/libc/str/memset16.o					\
o/$(MODE)/libc/str/dosdatetimetounix.o: private			\
		OVERRIDE_CFLAGS +=				\
			-O3

o/$(MODE)/libc/str/getzipcdir.o					\
o/$(MODE)/libc/str/getzipcdircomment.o				\
o/$(MODE)/libc/str/getzipcdircommentsize.o			\
o/$(MODE)/libc/str/getzipcdiroffset.o				\
o/$(MODE)/libc/str/getzipcdirrecords.o				\
o/$(MODE)/libc/str/getzipcfilecompressedsize.o			\
o/$(MODE)/libc/str/getzipcfilemode.o				\
o/$(MODE)/libc/str/getzipcfileoffset.o				\
o/$(MODE)/libc/str/getzipcfileuncompressedsize.o		\
o/$(MODE)/libc/str/getziplfilecompressedsize.o			\
o/$(MODE)/libc/str/getziplfileuncompressedsize.o		\
o/$(MODE)/libc/str/getzipcfiletimestamps.o: private		\
		OVERRIDE_CFLAGS +=				\
			-Os

o/$(MODE)/libc/str/iswpunct.o					\
o/$(MODE)/libc/str/iswupper.o					\
o/$(MODE)/libc/str/iswlower.o					\
o/$(MODE)/libc/str/iswseparator.o: private			\
		OVERRIDE_CFLAGS +=				\
			-fno-jump-tables

o/$(MODE)/libc/str/bcmp.o					\
o/$(MODE)/libc/str/strcmp.o					\
o/$(MODE)/libc/str/windowsdurationtotimeval.o			\
o/$(MODE)/libc/str/windowsdurationtotimespec.o			\
o/$(MODE)/libc/str/timevaltowindowstime.o			\
o/$(MODE)/libc/str/timespectowindowstime.o			\
o/$(MODE)/libc/str/windowstimetotimeval.o			\
o/$(MODE)/libc/str/windowstimetotimespec.o: private		\
		OVERRIDE_CFLAGS +=				\
			-O2

# we can't use compiler magic because:
#   kprintf() depends on these functions
o/$(MODE)/libc/fmt/strsignal.greg.o: private			\
		OVERRIDE_CFLAGS +=				\
			-fpie					\
			-ffreestanding				\
			$(NO_MAGIC)

o/$(MODE)/libc/str/eastasianwidth.bin:				\
		libc/str/eastasianwidth.txt			\
		o/$(MODE)/tool/decode/mkwides.com
	@$(COMPILE) -AMKWIDES -T$@ o/$(MODE)/tool/decode/mkwides.com -o $@ $<
o/$(MODE)/libc/str/eastasianwidth.bin.lz4:			\
		o/$(MODE)/libc/str/eastasianwidth.bin		\
		o/$(MODE)/third_party/lz4cli/lz4cli.com
	@$(COMPILE) -ALZ4 -T$@ o/$(MODE)/third_party/lz4cli/lz4cli.com -q -f -9 --content-size $< $@
o/$(MODE)/libc/str/eastasianwidth.s:				\
		o/$(MODE)/libc/str/eastasianwidth.bin.lz4	\
		o/$(MODE)/tool/build/lz4toasm.com
	@$(COMPILE) -ABIN2ASM -T$@ o/$(MODE)/tool/build/lz4toasm.com -s kEastAsianWidth -o $@ $<

o/$(MODE)/libc/str/combiningchars.bin:				\
		libc/str/strdata.txt				\
		o/$(MODE)/tool/decode/mkcombos.com
	@$(COMPILE) -AMKCOMBOS -T$@ o/$(MODE)/tool/decode/mkcombos.com -o $@ $<
o/$(MODE)/libc/str/combiningchars.bin.lz4:			\
		o/$(MODE)/libc/str/combiningchars.bin		\
		o/$(MODE)/third_party/lz4cli/lz4cli.com
	@$(COMPILE) -ALZ4 -T$@ o/$(MODE)/third_party/lz4cli/lz4cli.com -q -f -9 --content-size $< $@
o/$(MODE)/libc/str/combiningchars.s:				\
		o/$(MODE)/libc/str/combiningchars.bin.lz4	\
		o/$(MODE)/tool/build/lz4toasm.com
	@$(COMPILE) -ABIN2ASM -T$@ o/$(MODE)/tool/build/lz4toasm.com -s kCombiningChars -o $@ $<

LIBC_STR_LIBS = $(foreach x,$(LIBC_STR_ARTIFACTS),$($(x)))
LIBC_STR_SRCS = $(foreach x,$(LIBC_STR_ARTIFACTS),$($(x)_SRCS))
LIBC_STR_HDRS = $(foreach x,$(LIBC_STR_ARTIFACTS),$($(x)_HDRS))
LIBC_STR_INCS = $(foreach x,$(LIBC_STR_ARTIFACTS),$($(x)_INCS))
LIBC_STR_BINS = $(foreach x,$(LIBC_STR_ARTIFACTS),$($(x)_BINS))
LIBC_STR_CHECKS = $(foreach x,$(LIBC_STR_ARTIFACTS),$($(x)_CHECKS))
LIBC_STR_OBJS = $(foreach x,$(LIBC_STR_ARTIFACTS),$($(x)_OBJS))
LIBC_STR_TESTS = $(foreach x,$(LIBC_STR_ARTIFACTS),$($(x)_TESTS))
# $(LIBC_STR_OBJS): $(BUILD_FILES) libc/str/str.mk

.PHONY: o/$(MODE)/libc/str
o/$(MODE)/libc/str: $(LIBC_STR_CHECKS)
