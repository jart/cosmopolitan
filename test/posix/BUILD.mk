#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#── vi: set noet ft=make ts=8 sw=8 fenc=utf-8 :vi ────────────────────┘

PKGS += TEST_POSIX

TEST_POSIX_SRCS :=							\
	$(wildcard test/posix/*.c)

TEST_POSIX_SRCS_TEST =							\
	$(filter %_test.c,$(TEST_POSIX_SRCS))

TEST_POSIX_OBJS =							\
	$(TEST_POSIX_SRCS:%.c=o/$(MODE)/%.o)

TEST_POSIX_COMS =							\
	$(TEST_POSIX_SRCS_TEST:%.c=o/$(MODE)/%)				\
	o/$(MODE)/test/posix/file_offset_exec_prog

TEST_POSIX_BINS =							\
	$(TEST_POSIX_COMS)						\
	$(TEST_POSIX_COMS:%=%.dbg)

TEST_POSIX_TESTS =							\
	$(TEST_POSIX_SRCS_TEST:%.c=o/$(MODE)/%.ok)

TEST_POSIX_CHECKS =							\
	$(TEST_POSIX_SRCS_TEST:%.c=o/$(MODE)/%.runs)

TEST_POSIX_DIRECTDEPS =							\
	LIBC_CALLS							\
	LIBC_FMT							\
	LIBC_INTRIN							\
	LIBC_MEM							\
	LIBC_PROC							\
	LIBC_LOG							\
	LIBC_RUNTIME							\
	LIBC_SOCK							\
	LIBC_STDIO							\
	LIBC_STR							\
	LIBC_SYSV							\
	LIBC_THREAD							\
	THIRD_PARTY_MUSL						\

TEST_POSIX_DEPS :=							\
	$(call uniq,$(foreach x,$(TEST_POSIX_DIRECTDEPS),$($(x))))

o/$(MODE)/test/posix/posix.pkg:						\
		$(TEST_POSIX_OBJS)					\
		$(foreach x,$(TEST_POSIX_DIRECTDEPS),$($(x)_A).pkg)

o/$(MODE)/test/posix/%.dbg:						\
		$(TEST_POSIX_DEPS)					\
		o/$(MODE)/test/posix/%.o				\
		o/$(MODE)/test/posix/posix.pkg				\
		$(CRT)							\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

o/$(MODE)/test/posix/file_offset_exec_test.dbg:				\
		$(TEST_POSIX_DEPS)					\
		o/$(MODE)/test/posix/file_offset_exec_test.o		\
		o/$(MODE)/test/posix/file_offset_exec_prog.zip.o	\
		o/$(MODE)/test/posix/posix.pkg				\
		$(CRT)							\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

o/$(MODE)/test/posix/file_offset_exec_prog.zip.o: private		\
		ZIPOBJ_FLAGS +=						\
			-B

o/$(MODE)/test/posix/fread3gb_test.runs:				\
		private QUOTA += -F5gb -M5gb

.PHONY: o/$(MODE)/test/posix
o/$(MODE)/test/posix:							\
		$(TEST_POSIX_BINS)					\
		$(TEST_POSIX_CHECKS)
