#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#───vi: set et ft=make ts=8 tw=8 fenc=utf-8 :vi───────────────────────┘

LEMON = $(MKDIR) $(@D) && ACTION="LEMON $@" build/do $(THIRD_PARTY_LEMON)

THIRD_PARTY_LEMON = o/$(MODE)/third_party/lemon/lemon.com.dbg

THIRD_PARTY_LEMON_OBJS =					\
	o/$(MODE)/third_party/lemon/lemon.o			\
	o/$(MODE)/third_party/lemon/lemon.c.zip.o		\
	o/$(MODE)/third_party/lemon/lempar.c.txt.zip.o

THIRD_PARTY_LEMON_COMS =					\
	o/$(MODE)/third_party/lemon/lemon.com

THIRD_PARTY_LEMON_LINK =					\
	$(THIRD_PARTY_LEMON_DEPS)				\
	o/$(MODE)/third_party/lemon/%.o				\
	$(CRT)							\
	$(APE)

THIRD_PARTY_LEMON_DIRECTDEPS =					\
	LIBC_ALG						\
	LIBC_CALLS						\
	LIBC_FMT						\
	LIBC_MEM						\
	LIBC_NEXGEN32E						\
	LIBC_RUNTIME						\
	LIBC_STDIO						\
	LIBC_STR						\
	LIBC_STUBS						\
	LIBC_UNICODE						\
	LIBC_X							\
	LIBC_ZIPOS						\
	THIRD_PARTY_GDTOA

THIRD_PARTY_LEMON_DEPS :=					\
	$(call uniq,$(foreach x,$(THIRD_PARTY_LEMON_DIRECTDEPS),$($(x))))

o/$(MODE)/third_party/lemon/lemon.pkg:				\
		$(THIRD_PARTY_LEMON_OBJS)			\
		$(foreach x,$(THIRD_PARTY_LEMON_DIRECTDEPS),$($(x)_A).pkg)

o/$(MODE)/third_party/lemon/lemon.com.dbg:			\
		$(THIRD_PARTY_LEMON_DEPS)			\
		o/$(MODE)/third_party/lemon/lemon.pkg		\
		o/$(MODE)/third_party/lemon/lemon.o		\
		o/$(MODE)/third_party/lemon/lempar.c.txt.zip.o	\
		$(CRT)						\
		$(APE)
	-@$(APELINK)

.PHONY: o/$(MODE)/third_party/lemon
o/$(MODE)/third_party/lemon:					\
		o/$(MODE)/third_party/lemon/lemon.com
