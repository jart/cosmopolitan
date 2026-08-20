#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#── vi: set noet ft=make ts=8 sw=8 fenc=utf-8 :vi ────────────────────┘
#
# OVERVIEW
#
#   SQLCipher (encrypted SQLite) for cherrybean.
#   Separate from THIRD_PARTY_SQLITE3 so vanilla redbean is unchanged.

PKGS += THIRD_PARTY_SQLCIPHER

THIRD_PARTY_SQLCIPHER_ARTIFACTS += THIRD_PARTY_SQLCIPHER_A
THIRD_PARTY_SQLCIPHER = $(THIRD_PARTY_SQLCIPHER_A_DEPS) $(THIRD_PARTY_SQLCIPHER_A)
THIRD_PARTY_SQLCIPHER_A = o/$(MODE)/third_party/sqlcipher/libsqlcipher.a
THIRD_PARTY_SQLCIPHER_A_HDRS =						\
	third_party/sqlcipher/sqlite3.h					\
	third_party/sqlcipher/sqlite3ext.h
THIRD_PARTY_SQLCIPHER_A_INCS =						\
	third_party/sqlcipher/sqlite3.c					\
	third_party/sqlcipher/crypto_mbedtls.c
THIRD_PARTY_SQLCIPHER_A_SRCS =						\
	third_party/sqlcipher/sqlite3_cosmo.c
THIRD_PARTY_SQLCIPHER_A_OBJS =						\
	$(THIRD_PARTY_SQLCIPHER_A_SRCS:%.c=o/$(MODE)/%.o)

THIRD_PARTY_SQLCIPHER_A_CHECKS =					\
	$(THIRD_PARTY_SQLCIPHER_A).pkg					\
	$(THIRD_PARTY_SQLCIPHER_A_HDRS:%=o/$(MODE)/%.ok)

THIRD_PARTY_SQLCIPHER_A_DIRECTDEPS =					\
	LIBC_CALLS							\
	LIBC_FMT							\
	LIBC_INTRIN							\
	LIBC_MEM							\
	LIBC_NEXGEN32E							\
	LIBC_PROC							\
	LIBC_RUNTIME							\
	LIBC_STDIO							\
	LIBC_STR							\
	LIBC_SYSTEM							\
	LIBC_SYSV							\
	LIBC_SYSV_CALLS							\
	LIBC_THREAD							\
	LIBC_TINYMATH							\
	THIRD_PARTY_COMPILER_RT						\
	THIRD_PARTY_GDTOA						\
	THIRD_PARTY_MBEDTLS						\
	THIRD_PARTY_MUSL						\
	THIRD_PARTY_TZ							\
	THIRD_PARTY_ZLIB						\

THIRD_PARTY_SQLCIPHER_A_DEPS :=						\
	$(call uniq,$(foreach x,$(THIRD_PARTY_SQLCIPHER_A_DIRECTDEPS),$($(x))))

$(THIRD_PARTY_SQLCIPHER_A):						\
		third_party/sqlcipher/					\
		$(THIRD_PARTY_SQLCIPHER_A).pkg				\
		$(THIRD_PARTY_SQLCIPHER_A_OBJS)

$(THIRD_PARTY_SQLCIPHER_A).pkg:						\
		$(THIRD_PARTY_SQLCIPHER_A_OBJS)				\
		$(foreach x,$(THIRD_PARTY_SQLCIPHER_A_DIRECTDEPS),$($(x)_A).pkg)

# https://www.zetetic.net/sqlcipher/sqlcipher-api/
# https://www.sqlite.org/compile.html
THIRD_PARTY_SQLCIPHER_FLAGS =						\
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
	-DHAVE_PREAD							\
	-DHAVE_PWRITE							\
	-DSQLITE_THREADSAFE=1						\
	-DSQLITE_TEMP_STORE=2						\
	-DSQLITE_HAS_CODEC						\
	-DSQLITE_EXTRA_INIT=sqlcipher_extra_init			\
	-DSQLITE_EXTRA_SHUTDOWN=sqlcipher_extra_shutdown		\
	-DSQLCIPHER_CRYPTO_CUSTOM=sqlcipher_mbedtls_setup		\
	-DSQLITE_MAX_EXPR_DEPTH=0					\
	-DSQLITE_DEFAULT_MEMSTATUS=0					\
	-DSQLITE_DEFAULT_WAL_SYNCHRONOUS=1				\
	-DSQLITE_LIKE_DOESNT_MATCH_BLOBS				\
	-DSQLITE_OMIT_UTF16						\
	-DSQLITE_OMIT_TCL_VARIABLE					\
	-DSQLITE_OMIT_LOAD_EXTENSION					\
	-DSQLITE_OMIT_AUTOINIT						\
	-DSQLITE_OMIT_GET_TABLE						\
	-DSQLITE_OMIT_COMPILEOPTION_DIAGS				\
	-DSQLITE_HAVE_C99_MATH_FUNCS					\
	-DSQLITE_ENABLE_MATH_FUNCTIONS					\
	-DSQLITE_ENABLE_JSON1						\
	-DSQLITE_ENABLE_DESERIALIZE
# SQLITE_ENABLE_BATCH_ATOMIC_WRITE is intentionally off. Cosmo sqlite
# enables it with IsLinux() ioctl guards. Turning it on here without
# those guards (patches/cosmo-vfs.patch) would ioctl F2FS on Windows.

ifeq ($(MODE),dbg)
THIRD_PARTY_SQLCIPHER_CPPFLAGS_DEBUG = -DSQLITE_DEBUG
endif

$(THIRD_PARTY_SQLCIPHER_A_OBJS): private				\
		CFLAGS +=						\
			-Ithird_party/sqlcipher				\
			$(THIRD_PARTY_SQLCIPHER_FLAGS)			\
			$(THIRD_PARTY_SQLCIPHER_CPPFLAGS_DEBUG)		\
			-fdata-sections					\
			-ffunction-sections

o/$(MODE)/third_party/sqlcipher/sqlite3_cosmo.o: private QUOTA = -C32 -L180

THIRD_PARTY_SQLCIPHER_LIBS = $(foreach x,$(THIRD_PARTY_SQLCIPHER_ARTIFACTS),$($(x)))
THIRD_PARTY_SQLCIPHER_SRCS = $(foreach x,$(THIRD_PARTY_SQLCIPHER_ARTIFACTS),$($(x)_SRCS))
THIRD_PARTY_SQLCIPHER_HDRS = $(foreach x,$(THIRD_PARTY_SQLCIPHER_ARTIFACTS),$($(x)_HDRS))
THIRD_PARTY_SQLCIPHER_INCS = $(foreach x,$(THIRD_PARTY_SQLCIPHER_ARTIFACTS),$($(x)_INCS))
THIRD_PARTY_SQLCIPHER_CHECKS = $(foreach x,$(THIRD_PARTY_SQLCIPHER_ARTIFACTS),$($(x)_CHECKS))
THIRD_PARTY_SQLCIPHER_OBJS = $(foreach x,$(THIRD_PARTY_SQLCIPHER_ARTIFACTS),$($(x)_OBJS))

$(THIRD_PARTY_SQLCIPHER_OBJS): third_party/sqlcipher/BUILD.mk

.PHONY: o/$(MODE)/third_party/sqlcipher
o/$(MODE)/third_party/sqlcipher:					\
	$(THIRD_PARTY_SQLCIPHER_CHECKS)
