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
	LIBC_LOG							\
	LIBC_MEM							\
	LIBC_NEXGEN32E							\
	LIBC_PROC							\
	LIBC_RUNTIME							\
	LIBC_SOCK							\
	LIBC_STDIO							\
	LIBC_STR							\
	LIBC_SYSV							\
	LIBC_THREAD							\
	LIBC_TINYMATH							\
	THIRD_PARTY_GDTOA						\
	THIRD_PARTY_MUSL						\
	THIRD_PARTY_TZ							\

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

o/$(MODE)/test/posix/socket_basic_test.runs				\
o/$(MODE)/test/posix/msg_nosignal_test.runs				\
o/$(MODE)/test/posix/writev_test.runs					\
o/$(MODE)/test/posix/socket_timeout_signal_test.runs			\
o/$(MODE)/test/posix/socket_fionread_test.runs				\
o/$(MODE)/test/posix/msg_waitall_test.runs				\
o/$(MODE)/test/posix/connect_nonblock_test.runs				\
o/$(MODE)/test/posix/listen_timeout_test.runs				\
o/$(MODE)/test/posix/accept_poll_test.runs				\
o/$(MODE)/test/posix/accept_inherit_nonblock_test.runs			\
o/$(MODE)/test/posix/accept4_nonblock_test.runs:			\
		private .PLEDGE = inet

o/$(MODE)/test/posix/fcntl_advisory_locks_test.runs			\
o/$(MODE)/test/posix/fcntl_lock_test.runs				\
o/$(MODE)/test/posix/fcntl_eintr_test.runs:				\
		private .PLEDGE = flock

o/$(MODE)/test/posix/lowest_fd_test.runs:				\
		private .UNVEIL = r:/dev/urandom

o/$(MODE)/test/posix/open_test.runs:					\
		private .UNVEIL = r:/

o/$(MODE)/test/posix/fread3gb_test.runs:				\
		private QUOTA += -F5gb -M5gb

.PHONY: o/$(MODE)/test/posix
o/$(MODE)/test/posix:							\
		$(TEST_POSIX_BINS)					\
		$(TEST_POSIX_CHECKS)
