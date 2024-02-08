#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#── vi: set noet ft=make ts=8 sw=8 fenc=utf-8 :vi ────────────────────┘

PKGS += THIRD_PARTY_TIDY

THIRD_PARTY_TIDY_SRCS = $(THIRD_PARTY_TIDY_A_SRCS)
THIRD_PARTY_TIDY_HDRS = $(THIRD_PARTY_TIDY_A_HDRS)
THIRD_PARTY_TIDY_INCS = $(THIRD_PARTY_TIDY_A_INCS)
THIRD_PARTY_TIDY_BINS = $(THIRD_PARTY_TIDY_COMS) $(THIRD_PARTY_TIDY_COMS:%=%.dbg)

THIRD_PARTY_TIDY_ARTIFACTS += THIRD_PARTY_TIDY_A
THIRD_PARTY_TIDY = $(THIRD_PARTY_TIDY_A_DEPS) $(THIRD_PARTY_TIDY_A)
THIRD_PARTY_TIDY_A = o/$(MODE)/third_party/tidy/tidy.a
THIRD_PARTY_TIDY_A_FILES := $(wildcard third_party/tidy/*)
THIRD_PARTY_TIDY_A_HDRS = $(filter %.h,$(THIRD_PARTY_TIDY_A_FILES))
THIRD_PARTY_TIDY_A_INCS = $(filter %.inc,$(THIRD_PARTY_TIDY_A_FILES))
THIRD_PARTY_TIDY_A_SRCS = $(filter %.c,$(THIRD_PARTY_TIDY_A_FILES))
THIRD_PARTY_TIDY_A_OBJS = $(THIRD_PARTY_TIDY_A_SRCS:%.c=o/$(MODE)/%.o)

THIRD_PARTY_TIDY_A_DIRECTDEPS =							\
	LIBC_FMT								\
	LIBC_INTRIN								\
	LIBC_MEM								\
	LIBC_NEXGEN32E								\
	LIBC_RUNTIME								\
	LIBC_CALLS								\
	LIBC_STDIO								\
	LIBC_SYSV								\
	LIBC_STR

THIRD_PARTY_TIDY_A_DEPS :=							\
	$(call uniq,$(foreach x,$(THIRD_PARTY_TIDY_A_DIRECTDEPS),$($(x))))

THIRD_PARTY_TIDY_A_CHECKS =							\
	$(THIRD_PARTY_TIDY_A).pkg						\
	$(THIRD_PARTY_TIDY_A_HDRS:%=o/$(MODE)/%.ok)

$(THIRD_PARTY_TIDY_A):								\
		third_party/tidy/						\
		$(THIRD_PARTY_TIDY_A).pkg					\
		$(THIRD_PARTY_TIDY_A_OBJS)

$(THIRD_PARTY_TIDY_A).pkg:							\
		$(THIRD_PARTY_TIDY_A_OBJS)					\
		$(foreach x,$(THIRD_PARTY_TIDY_A_DIRECTDEPS),$($(x)_A).pkg)

o/$(MODE)/third_party/tidy/tidy.com.dbg:					\
		$(THIRD_PARTY_TIDY)						\
		o/$(MODE)/third_party/tidy/tidy.o				\
		o/$(MODE)/third_party/tidy/.tidyrc.zip.o			\
		$(CRT)								\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

o/$(MODE)/third_party/tidy/tidy.com:						\
		o/$(MODE)/third_party/tidy/tidy.com.dbg				\
		o/$(MODE)/third_party/zip/zip.com				\
		o/$(MODE)/tool/build/symtab.com
	@$(MAKE_OBJCOPY)
	@$(MAKE_SYMTAB_CREATE)
	@$(MAKE_SYMTAB_ZIP)

o/$(MODE)/third_party/tidy/.tidyrc.zip.o: private ZIPOBJ_FLAGS += -B

THIRD_PARTY_TIDY_COMS =								\
	o/$(MODE)/third_party/tidy/tidy.com

THIRD_PARTY_TIDY_LIBS = $(foreach x,$(THIRD_PARTY_TIDY_ARTIFACTS),$($(x)))
THIRD_PARTY_TIDY_SRCS = $(foreach x,$(THIRD_PARTY_TIDY_ARTIFACTS),$($(x)_SRCS))
THIRD_PARTY_TIDY_CHECKS = $(foreach x,$(THIRD_PARTY_TIDY_ARTIFACTS),$($(x)_CHECKS))
THIRD_PARTY_TIDY_OBJS = $(foreach x,$(THIRD_PARTY_TIDY_ARTIFACTS),$($(x)_OBJS))
$(THIRD_PARTY_TIDY_OBJS): $(BUILD_FILES) third_party/tidy/BUILD.mk

.PHONY: o/$(MODE)/third_party/tidy
o/$(MODE)/third_party/tidy:							\
		$(THIRD_PARTY_TIDY_BINS)					\
		$(THIRD_PARTY_TIDY_CHECKS)
