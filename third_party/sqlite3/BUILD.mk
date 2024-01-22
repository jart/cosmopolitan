#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#── vi: set noet ft=make ts=8 sw=8 fenc=utf-8 :vi ────────────────────┘
#
# OVERVIEW
#
#   SQLite Embedded Database
#
# NOTES
#
#   Please be warned that locks currently do nothing on Windows since
#   figuring out how to polyfill them correctly is a work in progress
#   Further note we currently don't do that thing SQLite does for Mac
#   file locks so your dbase will only be as reliable as Apple wanted
#   it to be when they wrote their POSIX file locking implementation.

PKGS += THIRD_PARTY_SQLITE3

THIRD_PARTY_SQLITE3_ARTIFACTS += THIRD_PARTY_SQLITE3_A
THIRD_PARTY_SQLITE3 = $(THIRD_PARTY_SQLITE3_A_DEPS) $(THIRD_PARTY_SQLITE3_A)
THIRD_PARTY_SQLITE3_A = o/$(MODE)/third_party/sqlite3/libsqlite3.a
THIRD_PARTY_SQLITE3_A_FILES := $(wildcard third_party/sqlite3/*)
THIRD_PARTY_SQLITE3_A_HDRS = $(filter %.h,$(THIRD_PARTY_SQLITE3_A_FILES))
THIRD_PARTY_SQLITE3_A_INCS = $(filter %.inc,$(THIRD_PARTY_SQLITE3_A_FILES))
THIRD_PARTY_SQLITE3_A_SRCS_C = $(filter %.c,$(THIRD_PARTY_SQLITE3_A_FILES))
THIRD_PARTY_SQLITE3_A_SRCS_T = $(filter %.inc,$(THIRD_PARTY_SQLITE3_A_FILES))
THIRD_PARTY_SQLITE3_BINS = $(THIRD_PARTY_SQLITE3_COMS) $(THIRD_PARTY_SQLITE3_COMS:%=%.dbg)

THIRD_PARTY_SQLITE3_A_SRCS =						\
	$(THIRD_PARTY_SQLITE3_A_SRCS_C)					\
	$(THIRD_PARTY_SQLITE3_A_SRCS_T)

THIRD_PARTY_SQLITE3_A_OBJS =						\
	$(filter-out %shell.o,$(THIRD_PARTY_SQLITE3_A_SRCS_C:%.c=o/$(MODE)/%.o))

THIRD_PARTY_SQLITE3_SHELL_OBJS =					\
	$(filter %shell.o,$(THIRD_PARTY_SQLITE3_A_SRCS_C:%.c=o/$(MODE)/%.o))

THIRD_PARTY_SQLITE3_COMS =						\
	o/$(MODE)/third_party/sqlite3/sqlite3.com

THIRD_PARTY_SQLITE3_A_CHECKS =						\
	$(THIRD_PARTY_SQLITE3_A).pkg					\
	$(THIRD_PARTY_SQLITE3_A_HDRS:%=o/$(MODE)/%.ok)

THIRD_PARTY_SQLITE3_A_DIRECTDEPS =					\
	LIBC_CALLS							\
	LIBC_FMT							\
	LIBC_INTRIN							\
	LIBC_MEM							\
	LIBC_NEXGEN32E							\
	LIBC_PROC							\
	LIBC_RUNTIME							\
	LIBC_STDIO							\
	LIBC_STR							\
	LIBC_SYSV							\
	LIBC_SYSV_CALLS							\
	LIBC_THREAD							\
	LIBC_TIME							\
	LIBC_TINYMATH							\
	THIRD_PARTY_COMPILER_RT						\
	THIRD_PARTY_GDTOA						\
	THIRD_PARTY_LINENOISE						\
	THIRD_PARTY_MUSL						\
	THIRD_PARTY_ZLIB						\
	TOOL_ARGS

THIRD_PARTY_SQLITE3_A_DEPS :=						\
	$(call uniq,$(foreach x,$(THIRD_PARTY_SQLITE3_A_DIRECTDEPS),$($(x))))

o/$(MODE)/third_party/sqlite3/sqlite3.com.dbg:				\
		$(THIRD_PARTY_SQLITE3_A_DEPS)				\
		$(THIRD_PARTY_SQLITE3_SHELL_OBJS)			\
		o/$(MODE)/third_party/sqlite3/shell.o			\
		o/$(MODE)/third_party/sqlite3/shell.pkg			\
		$(CRT)							\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

o/$(MODE)/third_party/sqlite3/sqlite3.com:				\
		o/$(MODE)/third_party/sqlite3/sqlite3.com.dbg		\
		o/$(MODE)/third_party/zip/zip.com			\
		o/$(MODE)/tool/build/symtab.com
	@$(MAKE_OBJCOPY)
	@$(MAKE_SYMTAB_CREATE)
	@$(MAKE_SYMTAB_ZIP)

$(THIRD_PARTY_SQLITE3_A):						\
		third_party/sqlite3/					\
		$(THIRD_PARTY_SQLITE3_A).pkg				\
		$(THIRD_PARTY_SQLITE3_A_OBJS)

$(THIRD_PARTY_SQLITE3_A).pkg:						\
		$(THIRD_PARTY_SQLITE3_A_OBJS)				\
		$(foreach x,$(THIRD_PARTY_SQLITE3_A_DIRECTDEPS),$($(x)_A).pkg)

o/$(MODE)/third_party/sqlite3/shell.pkg:				\
		$(THIRD_PARTY_SQLITE3_SHELL_OBJS)			\
		$(foreach x,$(THIRD_PARTY_SQLITE3_A_DIRECTDEPS),$($(x)_A).pkg)

# https://www.sqlite.org/compile.html
THIRD_PARTY_SQLITE3_FLAGS =						\
	-DNDEBUG							\
	-DSQLITE_CORE							\
	-DSQLITE_OS_UNIX						\
	-DBUILD_sqlite							\
	-DHAVE_USLEEP							\
	-DHAVE_READLINK							\
	-DHAVE_FCHOWN							\
	-DHAVE_LSTAT							\
	-DHAVE_GMTIME_R							\
	-DHAVE_FDATASYNC						\
	-DHAVE_STRCHRNUL						\
	-DHAVE_LOCALTIME_R						\
	-DHAVE_MALLOC_USABLE_SIZE					\
	-DSQLITE_THREADSAFE=1						\
	-DSQLITE_MAX_EXPR_DEPTH=0					\
	-DSQLITE_DEFAULT_MEMSTATUS=0					\
	-DSQLITE_DEFAULT_WAL_SYNCHRONOUS=1				\
	-DSQLITE_LIKE_DOESNT_MATCH_BLOBS				\
	-DSQLITE_OMIT_UTF16						\
	-DSQLITE_OMIT_TCL_VARIABLE					\
	-DSQLITE_OMIT_LOAD_EXTENSION					\
	-DSQLITE_OMIT_AUTOINIT						\
	-DSQLITE_OMIT_GET_TABLE						\
	-DSQLITE_OMIT_COMPILEOPTION_DIAGS                               \
	-DSQLITE_HAVE_C99_MATH_FUNCS					\
	-DSQLITE_ENABLE_MATH_FUNCTIONS					\
	-DSQLITE_ENABLE_JSON1						\
	-DSQLITE_ENABLE_DESERIALIZE					\
	-DSQLITE_ENABLE_PREUPDATE_HOOK					\
	-DSQLITE_ENABLE_SESSION

ifeq ($(MODE),dbg)
THIRD_PARTY_SQLITE3_CPPFLAGS_DEBUG = -DSQLITE_DEBUG
endif

$(THIRD_PARTY_SQLITE3_A_OBJS): private					\
		CFLAGS +=						\
			$(THIRD_PARTY_SQLITE3_FLAGS)			\
			$(THIRD_PARTY_SQLITE3_CPPFLAGS_DEBUG)		\

$(THIRD_PARTY_SQLITE3_SHELL_OBJS): private				\
		CFLAGS +=						\
			$(THIRD_PARTY_SQLITE3_FLAGS)			\
			$(THIRD_PARTY_SQLITE3_CPPFLAGS_DEBUG)		\
			-DHAVE_READLINE=0				\
			-DHAVE_EDITLINE=0				\
			-DSQLITE_HAVE_ZLIB				\
			-DSQLITE_ENABLE_IOTRACE				\
			-DSQLITE_ENABLE_COLUMN_METADATA			\
			-DSQLITE_ENABLE_EXPLAIN_COMMENTS		\
			-DSQLITE_ENABLE_UNKNOWN_SQL_FUNCTION		\
			-DSQLITE_ENABLE_STMTVTAB			\
			-DSQLITE_ENABLE_DBPAGE_VTAB			\
			-DSQLITE_ENABLE_DBSTAT_VTAB			\
			-DSQLITE_ENABLE_BYTECODE_VTAB			\
			-DSQLITE_ENABLE_OFFSET_SQL_FUNC			\
			-DSQLITE_ENABLE_DESERIALIZE			\
			-DSQLITE_ENABLE_FTS3				\
			-DSQLITE_ENABLE_FTS4				\
			-DSQLITE_ENABLE_FTS5				\
			-DSQLITE_ENABLE_RTREE				\
			-DSQLITE_ENABLE_GEOPOLY				\
			-DHAVE_LINENOISE

o//third_party/sqlite3/parse.o						\
o//third_party/sqlite3/select.o						\
o//third_party/sqlite3/pragma.o						\
o//third_party/sqlite3/vdbe.o: private					\
		CFLAGS +=						\
			-Os

o/$(MODE)/third_party/sqlite3/shell.o: private				\
		CFLAGS +=						\
			-DSTACK_FRAME_UNLIMITED

$(THIRD_PARTY_SQLITE3_A_OBJS)						\
$(THIRD_PARTY_SQLITE3_SHELL_OBJS): private				\
		CFLAGS +=						\
			-fdata-sections					\
			-ffunction-sections

# use smaller relocations for indirect branches
o/$(MODE)/third_party/sqlite3/expr.o					\
o/$(MODE)/third_party/sqlite3/printf.o					\
o/$(MODE)/third_party/sqlite3/parse.o: private				\
		CFLAGS +=						\
			-fpie

o/$(MODE)/third_party/sqlite3/shell.o: private QUOTA = -C32 -L180
o/$(MODE)/third_party/sqlite3/fts5.o: private QUOTA = -C32
o/$(MODE)/third_party/sqlite3/fts5.shell.o: private QUOTA = -C32 -L180

o/$(MODE)/third_party/sqlite3/rtree.o:					\
		third_party/sqlite3/rtree.c				\
		third_party/sqlite3/geopoly.inc				\
		third_party/gdtoa/gdtoa.h

o/$(MODE)/third_party/sqlite3/rtree.shell.o:				\
		third_party/sqlite3/rtree.shell.c			\
		third_party/sqlite3/geopoly.inc				\
		third_party/gdtoa/gdtoa.h

THIRD_PARTY_SQLITE3_LIBS = $(foreach x,$(THIRD_PARTY_SQLITE3_ARTIFACTS),$($(x)))
THIRD_PARTY_SQLITE3_SRCS = $(foreach x,$(THIRD_PARTY_SQLITE3_ARTIFACTS),$($(x)_SRCS))
THIRD_PARTY_SQLITE3_HDRS = $(foreach x,$(THIRD_PARTY_SQLITE3_ARTIFACTS),$($(x)_HDRS))
THIRD_PARTY_SQLITE3_INCS = $(foreach x,$(THIRD_PARTY_SQLITE3_ARTIFACTS),$($(x)_INCS))
THIRD_PARTY_SQLITE3_CHECKS = $(foreach x,$(THIRD_PARTY_SQLITE3_ARTIFACTS),$($(x)_CHECKS))
THIRD_PARTY_SQLITE3_OBJS = $(foreach x,$(THIRD_PARTY_SQLITE3_ARTIFACTS),$($(x)_OBJS))

$(THIRD_PARTY_SQLITE3_OBJS): third_party/sqlite3/BUILD.mk
$(THIRD_PARTY_SQLITE3_SHELL_OBJS): third_party/sqlite3/BUILD.mk

.PHONY: o/$(MODE)/third_party/sqlite3
o/$(MODE)/third_party/sqlite3:						\
	$(THIRD_PARTY_SQLITE3_BINS)					\
	$(THIRD_PARTY_SQLITE3_CHECKS)				
