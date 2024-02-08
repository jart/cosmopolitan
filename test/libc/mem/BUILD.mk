#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#── vi: set noet ft=make ts=8 sw=8 fenc=utf-8 :vi ────────────────────┘

PKGS += TEST_LIBC_MEM

TEST_LIBC_MEM_FILES := $(wildcard test/libc/mem/*)
TEST_LIBC_MEM_SRCS = $(TEST_LIBC_MEM_SRCS_C) $(TEST_LIBC_MEM_SRCS_CC)
TEST_LIBC_MEM_SRCS_C = $(filter %_test.c,$(TEST_LIBC_MEM_FILES))
TEST_LIBC_MEM_SRCS_CC = $(filter %_test.cc,$(TEST_LIBC_MEM_FILES))

TEST_LIBC_MEM_OBJS =						\
	$(TEST_LIBC_MEM_SRCS_C:%.c=o/$(MODE)/%.o)		\
	$(TEST_LIBC_MEM_SRCS_CC:%.cc=o/$(MODE)/%.o)

TEST_LIBC_MEM_COMS =						\
	$(TEST_LIBC_MEM_SRCS_C:%.c=o/$(MODE)/%.com)		\
	$(TEST_LIBC_MEM_SRCS_CC:%.cc=o/$(MODE)/%.com)

TEST_LIBC_MEM_BINS =						\
	$(TEST_LIBC_MEM_COMS)					\
	$(TEST_LIBC_MEM_COMS:%=%.dbg)

TEST_LIBC_MEM_TESTS =						\
	$(TEST_LIBC_MEM_SRCS_C:%.c=o/$(MODE)/%.com.ok)		\
	$(TEST_LIBC_MEM_SRCS_CC:%.cc=o/$(MODE)/%.com.ok)

TEST_LIBC_MEM_CHECKS =						\
	$(TEST_LIBC_MEM_SRCS_C:%.c=o/$(MODE)/%.com.runs)	\
	$(TEST_LIBC_MEM_SRCS_CC:%.cc=o/$(MODE)/%.com.runs)

TEST_LIBC_MEM_DIRECTDEPS =					\
	LIBC_CALLS						\
	LIBC_FMT						\
	LIBC_INTRIN						\
	LIBC_LOG						\
	LIBC_MEM						\
	LIBC_NEXGEN32E						\
	LIBC_PROC						\
	LIBC_RUNTIME						\
	LIBC_SOCK						\
	LIBC_STDIO						\
	LIBC_STR						\
	LIBC_SYSV						\
	LIBC_SYSV_CALLS						\
	LIBC_TESTLIB						\
	LIBC_THREAD						\
	LIBC_X							\
	THIRD_PARTY_DLMALLOC					\
	THIRD_PARTY_LIBCXX

TEST_LIBC_MEM_DEPS :=						\
	$(call uniq,$(foreach x,$(TEST_LIBC_MEM_DIRECTDEPS),$($(x))))

o/$(MODE)/test/libc/mem/mem.pkg:				\
		$(TEST_LIBC_MEM_OBJS)				\
		$(foreach x,$(TEST_LIBC_MEM_DIRECTDEPS),$($(x)_A).pkg)

o/$(MODE)/test/libc/mem/%.com.dbg:				\
		$(TEST_LIBC_MEM_DEPS)				\
		o/$(MODE)/test/libc/mem/%.o			\
		o/$(MODE)/test/libc/mem/mem.pkg			\
		$(LIBC_TESTMAIN)				\
		$(CRT)						\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

o/$(MODE)/test/libc/mem/prog/sock.o:				\
		test/libc/mem/prog/sock.c			\
		libc/errno.h					\
		libc/sock/sock.h				\
		libc/intrin/bswap.h				\
		libc/sysv/consts/af.h				\
		libc/sysv/consts/sock.h

################################################################################

o/$(MODE)/test/libc/mem/prog/life.com.dbg:			\
		$(LIBC_RUNTIME)					\
		o/$(MODE)/test/libc/mem/prog/life.o		\
		$(CRT)						\
		$(APE)
	@$(APELINK)

o/$(MODE)/test/libc/mem/prog/life.elf:				\
		o/$(MODE)/tool/build/assimilate.com		\
		o/$(MODE)/test/libc/mem/prog/life.com
	@$(COMPILE) -wACP -T$@					\
		build/bootstrap/cp.com				\
		o/$(MODE)/test/libc/mem/prog/life.com		\
		o/$(MODE)/test/libc/mem/prog/life.elf
	@$(COMPILE) -wAASSIMILATE -T$@				\
		o/$(MODE)/tool/build/assimilate.com -bcef	\
		o/$(MODE)/test/libc/mem/prog/life.elf

o/$(MODE)/test/libc/mem/prog/life.elf.zip.o: private		\
		ZIPOBJ_FLAGS +=					\
			-B

################################################################################

o/$(MODE)/test/libc/mem/prog/life.com.zip.o: private		\
		ZIPOBJ_FLAGS +=					\
			-B

################################################################################

o/$(MODE)/test/libc/mem/prog/sock.com.dbg:			\
		$(LIBC_RUNTIME)					\
		$(LIBC_SOCK)					\
		o/$(MODE)/test/libc/mem/prog/sock.o		\
		$(CRT)						\
		$(APE)
	@$(APELINK)

o/$(MODE)/test/libc/mem/prog/sock.elf:				\
		o/$(MODE)/tool/build/assimilate.com		\
		o/$(MODE)/test/libc/mem/prog/sock.com
	@$(COMPILE) -wACP -T$@					\
		build/bootstrap/cp.com				\
		o/$(MODE)/test/libc/mem/prog/sock.com		\
		o/$(MODE)/test/libc/mem/prog/sock.elf
	@$(COMPILE) -wAASSIMILATE -T$@				\
		o/$(MODE)/tool/build/assimilate.com -cef	\
		o/$(MODE)/test/libc/mem/prog/sock.elf

o/$(MODE)/test/libc/mem/prog/sock.elf.zip.o: private		\
		ZIPOBJ_FLAGS +=					\
			-B

################################################################################

$(TEST_LIBC_MEM_OBJS): private					\
		DEFAULT_CCFLAGS +=				\
			-fno-builtin

.PHONY: o/$(MODE)/test/libc/mem
o/$(MODE)/test/libc/mem:					\
		$(TEST_LIBC_MEM_BINS)				\
		$(TEST_LIBC_MEM_CHECKS)
