#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#───vi: set et ft=make ts=8 tw=8 fenc=utf-8 :vi───────────────────────┘
# Description:
#   bzip2 is a compression format.

PKGS += THIRD_PARTY_BZIP2

THIRD_PARTY_BZIP2_BINS =						\
	o/$(MODE)/third_party/bzip2/µbunzip2.com			\
	o/$(MODE)/third_party/bzip2/µbunzip2.com.dbg

THIRD_PARTY_BZIP2_OBJS =						\
	o/$(MODE)/third_party/bzip2/µbunzip2.o

THIRD_PARTY_BZIP2_DEPS := $(call uniq,					\
	$(LIBC_STR)							\
	$(LIBC_STDIO))

$(THIRD_PARTY_BZIP2_OBJS):						\
	DEFAULT_CPPFLAGS +=						\
		-DHAVE_CONFIG_H

o/$(MODE)/third_party/bzip2/µbunzip2.com.dbg:				\
		$(THIRD_PARTY_BZIP2_DEPS)				\
		$(THIRD_PARTY_BZIP2_OBJS)				\
		$(CRT)							\
		$(APE)
	@$(APELINK)

$(THIRD_PARTY_BZIP2_OBJS):						\
		$(BUILD_FILES)						\
		third_party/bzip2/bzip2.mk

.PHONY: o/$(MODE)/third_party/bzip2
o/$(MODE)/third_party/bzip2: $(THIRD_PARTY_BZIP2_BINS)
