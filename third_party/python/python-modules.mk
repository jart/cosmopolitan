#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#───vi: set et ft=make ts=8 tw=8 fenc=utf-8 :vi───────────────────────┘

# this file contains the recipes for the C extensions
# that are included with the Python source distribution
# the list of modules are also provided in Modules/config.c
# so any changes here must be reflected there as well


# these modules are always required

# _elementtree
EXTMODULE_ELEMENTTREE_SRCS_C =						\
	third_party/python/Modules/_elementtree.c

EXTMODULE_ELEMENTTREE_OBJS = 						\
	$(EXTMODULE_ELEMENTTREE_SRCS_C:%.c=o/$(MODE)/%.o)

# _io
EXTMODULE_IO_SRCS_C =							\
	$(wildcard third_party/python/Modules/_io/*.c)

EXTMODULE_IO_OBJS =							\
	$(EXTMODULE_IO_SRCS_C:%.c=o/$(MODE)/%.o)

# _tracemalloc
EXTMODULE_TRACEMALLOC_SRCS_C =						\
	third_party/python/Modules/_tracemalloc.c			\
	third_party/python/Modules/hashtable.c

# these modules don't rely on POSIX
EXTMODULE_BASE_INDEP_SRCS_C =						\
	third_party/python/Modules/gcmodule.c				\
	third_party/python/Modules/arraymodule.c			\
	third_party/python/Modules/cmathmodule.c			\
	third_party/python/Modules/mathmodule.c				\
	third_party/python/Modules/_math.c				\
	third_party/python/Modules/_struct.c				\
	third_party/python/Modules/_weakref.c				\
	third_party/python/Modules/_testcapimodule.c			\
	third_party/python/Modules/_randommodule.c			\
	third_party/python/Modules/_pickle.c				\
	third_party/python/Modules/_datetimemodule.c			\
	third_party/python/Modules/_bisectmodule.c			\
	third_party/python/Modules/_heapqmodule.c			\
	third_party/python/Modules/unicodedata.c			\
	$(EXTMODULE_TRACEMALLOC_SRCS_C)					\
	$(EXTMODULE_ELEMENTTREE_SRCS_C)


# these modules require POSIX-compliant libs
EXTMODULE_BASE_POSIX_SRCS_C =						\
	third_party/python/Modules/main.c				\
	third_party/python/Modules/posixmodule.c			\
	third_party/python/Modules/errnomodule.c			\
	third_party/python/Modules/pwdmodule.c				\
	third_party/python/Modules/_sre.c				\
	third_party/python/Modules/_codecsmodule.c			\
	third_party/python/Modules/_functoolsmodule.c			\
	third_party/python/Modules/_operator.c				\
	third_party/python/Modules/_collectionsmodule.c			\
	third_party/python/Modules/itertoolsmodule.c			\
	third_party/python/Modules/atexitmodule.c			\
	third_party/python/Modules/signalmodule.c			\
	third_party/python/Modules/_stat.c				\
	third_party/python/Modules/timemodule.c				\
	third_party/python/Modules/zipimport.c				\
	third_party/python/Modules/faulthandler.c			\
	third_party/python/Modules/symtablemodule.c			\
	third_party/python/Modules/fcntlmodule.c			\
	third_party/python/Modules/grpmodule.c				\
	third_party/python/Modules/selectmodule.c			\
	third_party/python/Modules/mmapmodule.c				\
	third_party/python/Modules/_csv.c				\
	third_party/python/Modules/_localemodule.c			\
	$(EXTMODULE_IO_SRCS_C)

# optional modules

# _blake
EXTMODULE_BLAKE_SRCS_C = 						\
	$(wildcard third_party/python/Modules/_blake2/*.c)

EXTMODULE_BLAKE_OBJS = 							\
	$(EXTMODULE_BLAKE_SRCS_C:%.c=o/$(MODE)/%.o)

# pyexpat
EXTMODULE_PYEXPAT_SRCS_C =						\
	third_party/python/Modules/expat/xmlparse.c			\
	third_party/python/Modules/expat/xmlrole.c			\
	third_party/python/Modules/expat/xmltok.c			\
	third_party/python/Modules/pyexpat.c

EXTMODULE_PYEXPAT_OBJS =						\
	$(EXTMODULE_PYEXPAT_SRCS_C:%.c=o/$(MODE)/%.o)

# lsprof
EXTMODULE_LSPROF_SRCS_C =						\
	third_party/python/Modules/_lsprof.c				\
	third_party/python/Modules/rotatingtree.c

# CJK codecs
EXTMODULE_CJKCODECS_SRCS_C =						\
	$(wildcard third_party/python/Modules/cjkcodecs/*.c)

# sqlite

# bz2

# readline

# ctypes

# _ssl

# _hashlib


# _decimal module (w/libmpdec)
EXTMODULE_DECIMAL_SRCS_C = 						\
	$(wildcard third_party/python/Modules/_decimal/libmpdec/*.c)	\
	third_party/python/Modules/_decimal/_decimal.c

EXTMODULE_DECIMAL_OBJS =						\
	$(EXTMODULE_DECIMAL_SRCS_C:%.c=o/$(MODE)/%.o)

EXTMODULE_OPT_SRCS_C =							\
	$(EXTMODULE_CJKCODECS_SRCS_C)					\
	$(EXTMODULE_LSPROF_SRCS_C)					\
	$(EXTMODULE_BLAKE_SRCS_C)					\
	$(EXTMODULE_LSPROF_SRCS_C)					\
	$(EXTMODULE_DECIMAL_SRCS_C)					\
	$(EXTMODULE_PYEXPAT_SRCS_C)					\
	third_party/python/Modules/socketmodule.c			\
	third_party/python/Modules/md5module.c				\
	third_party/python/Modules/sha1module.c				\
	third_party/python/Modules/sha256module.c			\
	third_party/python/Modules/sha512module.c			\
	third_party/python/Modules/_sha3/sha3module.c			\
	third_party/python/Modules/resource.c				\
	third_party/python/Modules/_posixsubprocess.c			\
	third_party/python/Modules/syslogmodule.c			\
	third_party/python/Modules/binascii.c				\
	third_party/python/Modules/parsermodule.c			\
	third_party/python/Modules/syslogmodule.c			\
	third_party/python/Modules/fpectlmodule.c			\
	third_party/python/Modules/zlibmodule.c				\
	third_party/python/Modules/_json.c				\
	third_party/python/Modules/_opcode.c


# add all of the module source files together to use in python.mk
THIRD_PARTY_PYTHON_MODULES_SRCS_C =			 		\
		third_party/python/Modules/config.c			\
		third_party/python/Modules/getbuildinfo.c		\
		third_party/python/Modules/getpath.c			\
		$(EXTMODULE_BASE_INDEP_SRCS_C)				\
		$(EXTMODULE_BASE_POSIX_SRCS_C)				\
		$(EXTMODULE_OPT_SRCS_C)

THIRD_PARTY_PYTHON_MODULES_OBJS =					\
		$(THIRD_PARTY_PYTHON_MODULES_SRCS_C:%.c=o/$(MODE)/%.o)


THIRD_PARTY_PYTHON_MODULES_DIRECTDEPS =					\
			THIRD_PARTY_ZLIB
# add sqlite if needed here

$(THIRD_PARTY_PYTHON_MODULES_OBJS):					\
	OVERRIDE_CFLAGS	+=						\
		-Ithird_party/python/Modules

o/$(MODE)/third_party/python/Modules/getbuildinfo.o:			\
	OVERRIDE_CFLAGS +=						\
		-DGITVERSION='"3.6"'					\
		-DGITTAG='"3.6"'					\
		-DGITBRANCH='"cosmo"'

$(EXTMODULE_ELEMENTTREE_OBJS):						\
	OVERRIDE_CFLAGS +=						\
		-DUSE_PYEXPAT_CAPI					\
		-DHAVE_EXPAT_CONFIG_H					\
		-Ithird_party/python/Modules/expat/

$(EXTMODULE_IO_OBJS):							\
	OVERRIDE_CFLAGS +=						\
	-Ithird_party/python/Modules/_io

$(EXTMODULE_BLAKE_OBJS):						\
	OVERRIDE_CFLAGS +=						\
		-DBLAKE_USE_SSE=1

$(EXTMODULE_PYEXPAT_OBJS):						\
	OVERRIDE_CFLAGS +=						\
		-DXML_POOR_ENTROPY					\
		-DHAVE_EXPAT_CONFIG_H					\
		-DUSE_PYEXPAT_CAPI					\
		-Ithird_party/python/Modules/expat

$(EXTMODULE_DECIMAL_OBJS):						\
	OVERRIDE_CFLAGS += 						\
		-Ithird_party/python/Modules/_decimal/	 		\
		-Ithird_party/python/Modules/_decimal/libmpdec	 	\
		-DASM=1							\
		-DCONFIG_64=1

o/$(MODE)/third_party/python/Modules/_decimal/libmpdec/transpose.o:	\
	OVERRIDE_CFLAGS +=						\
		-DSTACK_FRAME_UNLIMITED
