#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#── vi: set noet ft=make ts=8 sw=8 fenc=utf-8 :vi ────────────────────┘

PKGS += TEST_LIBC_PROC

TEST_LIBC_PROC_SRCS := $(wildcard test/libc/proc/*.c)
TEST_LIBC_PROC_SRCS_TEST = $(filter %_test.c,$(TEST_LIBC_PROC_SRCS))

TEST_LIBC_PROC_OBJS =						\
	$(TEST_LIBC_PROC_SRCS:%.c=o/$(MODE)/%.o)

TEST_LIBC_PROC_COMS =						\
	$(TEST_LIBC_PROC_SRCS:%.c=o/$(MODE)/%.com)

TEST_LIBC_PROC_BINS =						\
	$(TEST_LIBC_PROC_COMS)					\
	$(TEST_LIBC_PROC_COMS:%=%.dbg)

TEST_LIBC_PROC_TESTS =						\
	$(TEST_LIBC_PROC_SRCS_TEST:%.c=o/$(MODE)/%.com.ok)

TEST_LIBC_PROC_CHECKS =					\
	$(TEST_LIBC_PROC_SRCS_TEST:%.c=o/$(MODE)/%.com.runs)

TEST_LIBC_PROC_DIRECTDEPS =					\
	LIBC_CALLS						\
	LIBC_FMT						\
	LIBC_INTRIN						\
	LIBC_MEM						\
	LIBC_NEXGEN32E						\
	LIBC_NT_KERNEL32					\
	LIBC_RUNTIME						\
	LIBC_PROC						\
	LIBC_STR						\
	LIBC_SYSV						\
	LIBC_TESTLIB						\
	LIBC_THREAD						\
	LIBC_X							\
	THIRD_PARTY_MUSL					\
	THIRD_PARTY_TR

TEST_LIBC_PROC_DEPS :=						\
	$(call uniq,$(foreach x,$(TEST_LIBC_PROC_DIRECTDEPS),$($(x))))

o/$(MODE)/test/libc/proc/proc.pkg:				\
		$(TEST_LIBC_PROC_OBJS)				\
		$(foreach x,$(TEST_LIBC_PROC_DIRECTDEPS),$($(x)_A).pkg)

o/$(MODE)/test/libc/proc/%.com.dbg:				\
		$(TEST_LIBC_PROC_DEPS)				\
		o/$(MODE)/test/libc/proc/%.o			\
		o/$(MODE)/test/libc/proc/proc.pkg		\
		o/$(MODE)/tool/build/echo.com.zip.o		\
		$(LIBC_TESTMAIN)				\
		$(CRT)						\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

o/$(MODE)/test/libc/proc/posix_spawn_test.com.runs:		\
		private QUOTA += -M8192m

o/$(MODE)/test/libc/proc/posix_spawn_test.com.dbg:		\
		$(TEST_LIBC_PROC_DEPS)				\
		o/$(MODE)/test/libc/proc/posix_spawn_test.o	\
		o/$(MODE)/test/libc/proc/proc.pkg		\
		o/$(MODE)/tool/build/echo.com.zip.o		\
		o/$(MODE)/tool/build/cocmd.com.zip.o		\
		o/$(MODE)/test/libc/mem/prog/life.com.zip.o	\
		o/$(MODE)/test/libc/mem/prog/life.elf.zip.o	\
		o/$(MODE)/test/libc/proc/life-pe.com.zip.o	\
		$(LIBC_TESTMAIN)				\
		$(CRT)						\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

o/$(MODE)/test/libc/proc/system_test.com.dbg:			\
		$(TEST_LIBC_PROC_DEPS)				\
		o/$(MODE)/test/libc/proc/system_test.o		\
		o/$(MODE)/test/libc/proc/proc.pkg		\
		o/$(MODE)/tool/build/echo.com.zip.o		\
		o/$(MODE)/tool/build/cocmd.com.zip.o		\
		o/$(MODE)/tool/build/false.com.zip.o		\
		$(LIBC_TESTMAIN)				\
		$(CRT)						\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

o/$(MODE)/test/libc/proc/execve_test.com.dbg:				\
		$(TEST_LIBC_PROC_DEPS)					\
		o/$(MODE)/test/libc/proc/execve_test.o			\
		o/$(MODE)/test/libc/calls/life-nomod.com.zip.o		\
		o/$(MODE)/test/libc/proc/execve_test_prog1.com.zip.o	\
		o/$(MODE)/test/libc/mem/prog/life.elf.zip.o		\
		o/$(MODE)/test/libc/mem/prog/sock.elf.zip.o		\
		o/$(MODE)/test/libc/proc/proc.pkg			\
		$(LIBC_TESTMAIN)					\
		$(CRT)							\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

o/$(MODE)/test/libc/proc/fexecve_test.com.dbg:				\
		$(TEST_LIBC_PROC_DEPS)					\
		o/$(MODE)/test/libc/proc/fexecve_test.o		\
		o/$(MODE)/test/libc/proc/proc.pkg			\
		o/$(MODE)/test/libc/mem/prog/life.elf.zip.o		\
		o/$(MODE)/test/libc/calls/life-nomod.com.zip.o		\
		o/$(MODE)/test/libc/calls/zipread.com.zip.o		\
		$(LIBC_TESTMAIN)					\
		$(CRT)							\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

o/$(MODE)/test/libc/proc/execve_test_prog1.com.zip.o		\
o/$(MODE)/test/libc/proc/life-pe.com.zip.o: private		\
		ZIPOBJ_FLAGS +=					\
			-B

$(TEST_LIBC_PROC_OBJS): test/libc/proc/BUILD.mk

.PHONY: o/$(MODE)/test/libc/proc
o/$(MODE)/test/libc/proc:					\
		$(TEST_LIBC_PROC_BINS)				\
		$(TEST_LIBC_PROC_CHECKS)
