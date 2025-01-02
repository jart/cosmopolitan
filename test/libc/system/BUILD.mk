#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#── vi: set noet ft=make ts=8 sw=8 fenc=utf-8 :vi ────────────────────┘

PKGS += TEST_LIBC_SYSTEM

TEST_LIBC_SYSTEM_FILES := $(wildcard test/libc/system/*)
TEST_LIBC_SYSTEM_SRCS = $(filter %.c,$(TEST_LIBC_SYSTEM_FILES))
TEST_LIBC_SYSTEM_INCS = $(filter %.inc,$(TEST_LIBC_SYSTEM_FILES))
TEST_LIBC_SYSTEM_SRCS_TEST = $(filter %_test.c,$(TEST_LIBC_SYSTEM_SRCS))

TEST_LIBC_SYSTEM_OBJS =						\
	$(TEST_LIBC_SYSTEM_SRCS:%.c=o/$(MODE)/%.o)

TEST_LIBC_SYSTEM_COMS =						\
	$(TEST_LIBC_SYSTEM_SRCS:%.c=o/$(MODE)/%)

TEST_LIBC_SYSTEM_BINS =						\
	$(TEST_LIBC_SYSTEM_COMS)				\
	$(TEST_LIBC_SYSTEM_COMS:%=%.dbg)

TEST_LIBC_SYSTEM_TESTS =					\
	$(TEST_LIBC_SYSTEM_SRCS_TEST:%.c=o/$(MODE)/%.ok)

TEST_LIBC_SYSTEM_CHECKS =					\
	$(TEST_LIBC_SYSTEM_SRCS_TEST:%.c=o/$(MODE)/%.runs)

TEST_LIBC_SYSTEM_DIRECTDEPS =					\
	LIBC_CALLS						\
	LIBC_INTRIN						\
	LIBC_LOG						\
	LIBC_MEM						\
	LIBC_NEXGEN32E						\
	LIBC_PROC						\
	LIBC_RUNTIME						\
	LIBC_STDIO						\
	LIBC_STR						\
	LIBC_SYSTEM						\
	LIBC_SYSV						\
	LIBC_TESTLIB						\
	LIBC_THREAD						\
	LIBC_X							\
	THIRD_PARTY_MUSL					\
	THIRD_PARTY_TR						\
	THIRD_PARTY_TZ						\

TEST_LIBC_SYSTEM_DEPS :=					\
	$(call uniq,$(foreach x,$(TEST_LIBC_SYSTEM_DIRECTDEPS),$($(x))))

o/$(MODE)/test/libc/system/system.pkg:				\
		$(TEST_LIBC_SYSTEM_OBJS)			\
		$(foreach x,$(TEST_LIBC_SYSTEM_DIRECTDEPS),$($(x)_A).pkg)

o/$(MODE)/test/libc/system/%.dbg:				\
		$(TEST_LIBC_SYSTEM_DEPS)			\
		o/$(MODE)/test/libc/system/%.o			\
		o/$(MODE)/test/libc/system/system.pkg		\
		o/$(MODE)/tool/build/echo.zip.o			\
		$(LIBC_TESTMAIN)				\
		$(CRT)						\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

o/$(MODE)/test/libc/system/popen_test.dbg:			\
		$(TEST_LIBC_SYSTEM_DEPS)			\
		o/$(MODE)/test/libc/system/popen_test.o		\
		o/$(MODE)/test/libc/system/system.pkg		\
		o/$(MODE)/tool/build/echo.zip.o			\
		$(LIBC_TESTMAIN)				\
		$(CRT)						\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

o/$(MODE)/test/libc/system/system_test.dbg:			\
		$(TEST_LIBC_SYSTEM_DEPS)			\
		o/$(MODE)/test/libc/system/system_test.o	\
		o/$(MODE)/test/libc/system/system.pkg		\
		o/$(MODE)/tool/build/echo.zip.o			\
		o/$(MODE)/tool/build/cocmd.zip.o		\
		o/$(MODE)/tool/build/false.zip.o		\
		$(LIBC_TESTMAIN)				\
		$(CRT)						\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

o/$(MODE)/test/libc/system/trace_test.dbg:			\
		$(TEST_LIBC_SYSTEM_DEPS)			\
		o/$(MODE)/test/libc/system/trace_test.o		\
		o/$(MODE)/test/libc/system/system.pkg		\
		o/$(MODE)/test/libc/system/popen_test.zip.o	\
		o/$(MODE)/test/libc/system/popen_test.dbg.zip.o	\
		o/$(MODE)/tool/build/echo.zip.o			\
		$(LIBC_TESTMAIN)				\
		$(CRT)						\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

o/$(MODE)/test/libc/system/systemvpe_test.dbg:			\
		$(TEST_LIBC_SYSTEM_DEPS)			\
		o/$(MODE)/test/libc/system/systemvpe_test.o	\
		o/$(MODE)/test/libc/system/system.pkg		\
		o/$(MODE)/test/libc/proc/life.zip.o		\
		$(LIBC_TESTMAIN)				\
		$(CRT)						\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)


o/$(MODE)/test/libc/system/popen_test.zip.o: private ZIPOBJ_FLAGS += -B
o/$(MODE)/test/libc/system/popen_test.dbg.zip.o: private ZIPOBJ_FLAGS += -B

$(TEST_LIBC_SYSTEM_OBJS): test/libc/system/BUILD.mk

.PHONY: o/$(MODE)/test/libc/system
o/$(MODE)/test/libc/system:					\
		$(TEST_LIBC_SYSTEM_BINS)			\
		$(TEST_LIBC_SYSTEM_CHECKS)
