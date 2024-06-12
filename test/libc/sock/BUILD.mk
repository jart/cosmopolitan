#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#── vi: set noet ft=make ts=8 sw=8 fenc=utf-8 :vi ────────────────────┘

PKGS += TEST_LIBC_SOCK

TEST_LIBC_SOCK_SRCS := $(wildcard test/libc/sock/*.c)
TEST_LIBC_SOCK_SRCS_TEST = $(filter %_test.c,$(TEST_LIBC_SOCK_SRCS))

TEST_LIBC_SOCK_OBJS =					\
	$(TEST_LIBC_SOCK_SRCS:%.c=o/$(MODE)/%.o)

TEST_LIBC_SOCK_COMS =					\
	$(TEST_LIBC_SOCK_SRCS:%.c=o/$(MODE)/%)

TEST_LIBC_SOCK_BINS =					\
	$(TEST_LIBC_SOCK_COMS)				\
	$(TEST_LIBC_SOCK_COMS:%=%.dbg)

TEST_LIBC_SOCK_TESTS =					\
	$(TEST_LIBC_SOCK_SRCS_TEST:%.c=o/$(MODE)/%.ok)

TEST_LIBC_SOCK_CHECKS =					\
	$(TEST_LIBC_SOCK_SRCS_TEST:%.c=o/$(MODE)/%.runs)

TEST_LIBC_SOCK_DIRECTDEPS =				\
	LIBC_CALLS					\
	LIBC_FMT					\
	LIBC_INTRIN					\
	LIBC_MEM					\
	LIBC_NEXGEN32E					\
	LIBC_PROC					\
	LIBC_RUNTIME					\
	LIBC_SOCK					\
	LIBC_STDIO					\
	LIBC_STR					\
	LIBC_SYSV					\
	LIBC_THREAD					\
	LIBC_LOG					\
	LIBC_SYSV_CALLS					\
	LIBC_TESTLIB					\
	LIBC_X						\
	TOOL_DECODE_LIB

TEST_LIBC_SOCK_DEPS :=					\
	$(call uniq,$(foreach x,$(TEST_LIBC_SOCK_DIRECTDEPS),$($(x))))

o/$(MODE)/test/libc/sock/sock.pkg:			\
		$(TEST_LIBC_SOCK_OBJS)			\
		$(foreach x,$(TEST_LIBC_SOCK_DIRECTDEPS),$($(x)_A).pkg)

o/$(MODE)/test/libc/sock/%.dbg:				\
		$(TEST_LIBC_SOCK_DEPS)			\
		o/$(MODE)/test/libc/sock/%.o		\
		o/$(MODE)/test/libc/sock/sock.pkg	\
		$(LIBC_TESTMAIN)			\
		$(CRT)					\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

o/$(MODE)/test/libc/sock/unix_test.runs:		\
		private .PLEDGE = stdio rpath wpath cpath fattr proc unix

o/$(MODE)/test/libc/sock/connect_test.runs		\
o/$(MODE)/test/libc/sock/recvfrom_test.runs		\
o/$(MODE)/test/libc/sock/nonblock_test.runs		\
o/$(MODE)/test/libc/sock/socket_test.runs		\
o/$(MODE)/test/libc/sock/shutdown_test.runs		\
o/$(MODE)/test/libc/sock/setsockopt_test.runs		\
o/$(MODE)/test/libc/sock/sendfile_test.runs		\
o/$(MODE)/test/libc/sock/poll_test.runs			\
o/$(MODE)/test/libc/sock/pollfd_test.runs		\
o/$(MODE)/test/libc/sock/getpeername_test.runs:		\
		private .PLEDGE = stdio rpath wpath cpath fattr proc inet

o/$(MODE)/test/libc/sock/sendrecvmsg_test.runs:		\
		private .PLEDGE = stdio rpath wpath cpath fattr proc inet recvfd sendfd

o/$(MODE)/test/libc/sock/socket_test.runs:		\
		private .INTERNET = 1  # todo: ipv6 filtering

o/$(MODE)/test/libc/sock/recvmsg_test.runs:		\
		private .INTERNET = 1  # need to bind to 0.0.0.0
o/$(MODE)/test/libc/sock/recvmsg_test.runs:		\
		private .PLEDGE = stdio rpath wpath cpath fattr proc inet recvfd sendfd

$(TEST_LIBC_SOCK_OBJS): test/libc/sock/BUILD.mk

.PHONY: o/$(MODE)/test/libc/sock
o/$(MODE)/test/libc/sock:				\
		$(TEST_LIBC_SOCK_BINS)			\
		$(TEST_LIBC_SOCK_CHECKS)
