#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#───vi: set et ft=make ts=8 tw=8 fenc=utf-8 :vi───────────────────────┘
# Description:
#   m4 is an old skool macro processing language.

PKGS += THIRD_PARTY_M4

THIRD_PARTY_M4 =					\
	o/$(MODE)/third_party/m4/m4.com

THIRD_PARTY_M4_OBJS =					\
	o/$(MODE)/third_party/m4/eval.o			\
	o/$(MODE)/third_party/m4/expr.o			\
	o/$(MODE)/third_party/m4/look.o			\
	o/$(MODE)/third_party/m4/main.o			\
	o/$(MODE)/third_party/m4/misc.o			\
	o/$(MODE)/third_party/m4/ohash.o		\
	o/$(MODE)/third_party/m4/gnum4.o		\
	o/$(MODE)/third_party/m4/trace.o		\
	o/$(MODE)/third_party/m4/tokenizer.o		\
	o/$(MODE)/third_party/m4/parser.o

THIRD_PARTY_M4_DEPS := $(call uniq,			\
	$(LIBC_LOG)					\
	$(LIBC_MATH)					\
	$(LIBC_COMPAT)					\
	$(LIBC_UNICODE)					\
	$(THIRD_PARTY_REGEX)				\
	$(THIRD_PARTY_GETOPT))

$(THIRD_PARTY_M4_OBJS):					\
	DEFAULT_CPPFLAGS +=				\
		$(LIBC_COMPAT_CPPFLAGS)			\
		-isystem third_party/m4

$(THIRD_PARTY_M4_OBJS):					\
	DEFAULT_CFLAGS +=				\
		-Wno-unused				\
		-Wno-char-subscripts			\
		-Wno-sign-compare

o/$(MODE)/third_party/m4/m4.com.dbg:			\
		$(THIRD_PARTY_M4_DEPS)			\
		$(THIRD_PARTY_M4_OBJS)			\
		$(CRT)					\
		$(APE)
	@$(APELINK)

THIRD_PARTY_M4_CHECKS = o/$(MODE)/third_party/m4/m4.com.ok
o/$(MODE)/third_party/m4/test.m4.out:			\
		third_party/m4/TEST/test.m4		\
		third_party/m4/TEST/ack.m4		\
		third_party/m4/TEST/hanoi.m4		\
		third_party/m4/TEST/hash.m4		\
		third_party/m4/TEST/sqroot.m4		\
		third_party/m4/TEST/string.m4		\
		$(THIRD_PARTY_M4)
	@ACTION=M4 TARGET=$@ build/do			\
	  $(THIRD_PARTY_M4) third_party/m4/TEST/test.m4 >$@
o/$(MODE)/third_party/m4/m4.com.ok:			\
		o/$(MODE)/third_party/m4/test.m4.out	\
		third_party/m4/TEST/test.m4.golden
	@ACTION=CMP TARGET=$@ build/do cmp -s $^
	@ACTION=TOUCH TARGET=$@ build/do touch $@

$(THIRD_PARTY_M4_OBJS):					\
		$(BUILD_FILES)				\
		third_party/m4/m4.mk

.PHONY: o/$(MODE)/third_party/m4
o/$(MODE)/third_party/m4: $(THIRD_PARTY_M4_CHECKS)
