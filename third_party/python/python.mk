#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#───vi: set et ft=make ts=8 tw=8 fenc=utf-8 :vi───────────────────────┘

PKGS += THIRD_PARTY_PYTHON

THIRD_PARTY_PYTHON_ARTIFACTS = THIRD_PARTY_PYTHON_A
THIRD_PARTY_PYTHON = $(THIRD_PARTY_PYTHON_DEPS) $(THIRD_PARTY_PYTHON_A)
THIRD_PARTY_PYTHON_A = o/$(MODE)/third_party/python/libpython3.6m.a

include third_party/python/python-modules.mk
include third_party/python/python-stdlib.mk

THIRD_PARTY_PYTHON_OBJECT_SRCS_C =									\
	$(wildcard third_party/python/Objects/*.c)

THIRD_PARTY_PYTHON_PARSER_SKIP_SRCS_C =									\
	third_party/python/Parser/tokenizer_pgen.c							\
	third_party/python/Parser/pgenmain.c								\
	third_party/python/Parser/printgrammar.c							\
	third_party/python/Parser/parsetok_pgen.c
THIRD_PARTY_PYTHON_PARSER_SRCS_C =									\
	$(filter-out $(THIRD_PARTY_PYTHON_PARSER_SKIP_SRCS_C),$(wildcard third_party/python/Parser/*.c))

THIRD_PARTY_PYTHON_CORE_SKIP_SRCS_C =									\
	third_party/python/Python/dynload_%.c								\
	third_party/python/Python/strdup.c								\
	third_party/python/Python/thread.c
THIRD_PARTY_PYTHON_CORE_SRCS_C = 									\
	$(filter-out $(THIRD_PARTY_PYTHON_CORE_SKIP_SRCS_C),$(wildcard third_party/python/Python/*.c)) 	\
	third_party/python/Python/dynload_shlib.c

THIRD_PARTY_PYTHON_SRCS_C =										\
	third_party/python/Programs/python.c								\
	$(THIRD_PARTY_PYTHON_OBJECT_SRCS_C)								\
	$(THIRD_PARTY_PYTHON_PARSER_SRCS_C)								\
	$(THIRD_PARTY_PYTHON_CORE_SRCS_C)								\
	$(THIRD_PARTY_PYTHON_MODULES_SRCS_C)

THIRD_PARTY_PYTHON_OBJS =										\
	$(THIRD_PARTY_PYTHON_SRCS_C:%.c=o/$(MODE)/%.o)
THIRD_PARTY_PYTHON_A_OBJS =										\
	$(filter-out o/$(MODE)/third_party/python/Programs/python.o,$(THIRD_PARTY_PYTHON_OBJS))

THIRD_PARTY_PYTHON_BINS =										\
	$(THIRD_PARTY_PYTHON_COMS) $(THIRD_PARTY_PYTHON_COMS:%=%.dbg)
THIRD_PARTY_PYTHON_COMS =										\
	o/$(MODE)/third_party/python/python.com

THIRD_PARTY_PYTHON_A_CHECKS =										\
	$(THIRD_PARTY_PYTHON_A).pkg
# add .h.ok check here

THIRD_PARTY_PYTHON_A_DIRECTDEPS0 =									\
	$(THIRD_PARTY_PYTHON_MODULES_DIRECTDEPS)							\
	LIBC_ALG											\
	LIBC_BITS											\
	LIBC_CALLS											\
	LIBC_FMT											\
	LIBC_INTRIN											\
	LIBC_LOG											\
	LIBC_MEM											\
	LIBC_NEXGEN32E											\
	LIBC_RAND											\
	LIBC_RUNTIME											\
	LIBC_SOCK											\
	LIBC_DNS											\
	LIBC_STDIO											\
	LIBC_STR											\
	LIBC_STUBS											\
	LIBC_SYSV											\
	LIBC_SYSV_CALLS											\
	LIBC_TIME											\
	LIBC_TINYMATH											\
	LIBC_UNICODE											\
	LIBC_ZIPOS											\
	THIRD_PARTY_GDTOA										\
	THIRD_PARTY_GETOPT										\
	THIRD_PARTY_MUSL										\
	THIRD_PARTY_ZLIB

THIRD_PARTY_PYTHON_A_DIRECTDEPS =									\
	$(call uniq,$(THIRD_PARTY_PYTHON_A_DIRECTDEPS0))
THIRD_PARTY_PYTHON_A_DEPS =										\
	$(call uniq,$(foreach x,$(THIRD_PARTY_PYTHON_A_DIRECTDEPS),$($(x))))

o/$(MODE)/third_party/python/python.com.dbg:								\
	$(THIRD_PARTY_PYTHON_A_DEPS)									\
	$(THIRD_PARTY_PYTHON_A)										\
	$(THIRD_PARTY_PYTHON_STDLIB_PY_OBJS)								\
	o/$(MODE)/third_party/python/Programs/python.o							\
	$(CRT)												\
	$(APE)
	-@$(APELINK)

$(THIRD_PARTY_PYTHON_A):										\
	third_party/python										\
	$(THIRD_PARTY_PYTHON_A).pkg									\
	$(THIRD_PARTY_PYTHON_A_OBJS)

$(THIRD_PARTY_PYTHON_A).pkg:										\
	$(THIRD_PARTY_PYTHON_A_OBJS)									\
	$(foreach x,$(THIRD_PARTY_PYTHON_A_DIRECTDEPS),$($(x)_A).pkg)

$(THIRD_PARTY_PYTHON_OBJS):										\
	OVERRIDE_CFLAGS +=										\
		-DPy_BUILD_CORE										\
		-DPLATFORM='"linux"'									\
		-DMULTIARCH='"x86_64-linux-gnu"'							\
		-isystem libc/isystem									\
		-Ithird_party/python									\
		-Ithird_party/python/Include

o/$(MODE)/third_party/python/Programs/python.o								\
o/$(MODE)/third_party/python/Python/marshal.o								\
o/$(MODE)/third_party/python/Python/sysmodule.o								\
o/$(MODE)/third_party/python/Modules/selectmodule.o							\
o/$(MODE)/third_party/python/Modules/getpath.o								\
o/$(MODE)/third_party/python/Objects/listobject.o:							\
	OVERRIDE_CFLAGS +=										\
		-DSTACK_FRAME_UNLIMITED

o/$(MODE)/third_party/python/Python/dynload_shlib.o:							\
	OVERRIDE_CFLAGS +=										\
		-DSOABI='"cpython36m-x86_64-linux-gnu"'

o/$(MODE)/third_party/python/Python/sysmodule.o:							\
	OVERRIDE_CFLAGS +=										\
		-DABIFLAGS='"m"'


$(THIRD_PARTY_PYTHON_OBJS):										\
	third_party/python/python.mk									\
	third_party/python/python-modules.mk

.PHONY: o/$(MODE)/third_party/python
o/$(MODE)/third_party/python:										\
	$(THIRD_PARTY_PYTHON_BINS)									\
	$(THIRD_PARTY_PYTHON_CHECKS)
