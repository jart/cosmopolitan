#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#───vi: set et ft=make ts=8 tw=8 fenc=utf-8 :vi───────────────────────┘

PKGS += THIRD_PARTY_PYTHON

THIRD_PARTY_PYTHON_ARTIFACTS = THIRD_PARTY_PYTHON_A
THIRD_PARTY_PYTHON = $(THIRD_PARTY_PYTHON_DEPS) $(THIRD_PARTY_PYTHON_A)
THIRD_PARTY_PYTHON_A = o/$(MODE)/third_party/python/libpython3.6m.a

include third_party/python/python-modules.mk
include third_party/python/python-stdlib.mk

THIRD_PARTY_PYTHON_OBJECT_SRCS =				\
	third_party/python/Objects/abstract.c			\
	third_party/python/Objects/accu.c			\
	third_party/python/Objects/boolobject.c			\
	third_party/python/Objects/bytearrayobject.c		\
	third_party/python/Objects/bytes_methods.c		\
	third_party/python/Objects/bytesobject.c		\
	third_party/python/Objects/capsule.c			\
	third_party/python/Objects/cellobject.c			\
	third_party/python/Objects/classobject.c		\
	third_party/python/Objects/codeobject.c			\
	third_party/python/Objects/complexobject.c		\
	third_party/python/Objects/descrobject.c		\
	third_party/python/Objects/dictobject.c			\
	third_party/python/Objects/enumobject.c			\
	third_party/python/Objects/exceptions.c			\
	third_party/python/Objects/fileobject.c			\
	third_party/python/Objects/floatobject.c		\
	third_party/python/Objects/frameobject.c		\
	third_party/python/Objects/funcobject.c			\
	third_party/python/Objects/genobject.c			\
	third_party/python/Objects/iterobject.c			\
	third_party/python/Objects/listobject.c			\
	third_party/python/Objects/longobject.c			\
	third_party/python/Objects/memoryobject.c		\
	third_party/python/Objects/methodobject.c		\
	third_party/python/Objects/moduleobject.c		\
	third_party/python/Objects/namespaceobject.c		\
	third_party/python/Objects/object.c			\
	third_party/python/Objects/obmalloc.c			\
	third_party/python/Objects/odictobject.c		\
	third_party/python/Objects/rangeobject.c		\
	third_party/python/Objects/setobject.c			\
	third_party/python/Objects/sliceobject.c		\
	third_party/python/Objects/structseq.c			\
	third_party/python/Objects/tupleobject.c		\
	third_party/python/Objects/typeobject.c			\
	third_party/python/Objects/unicodectype.c		\
	third_party/python/Objects/unicodeobject.c		\
	third_party/python/Objects/weakrefobject.c

THIRD_PARTY_PYTHON_PARSER_SRCS =				\
	third_party/python/Parser/acceler.c			\
	third_party/python/Parser/bitset.c			\
	third_party/python/Parser/firstsets.c			\
	third_party/python/Parser/grammar.c			\
	third_party/python/Parser/grammar1.c			\
	third_party/python/Parser/listnode.c			\
	third_party/python/Parser/metagrammar.c			\
	third_party/python/Parser/myreadline.c			\
	third_party/python/Parser/node.c			\
	third_party/python/Parser/parser.c			\
	third_party/python/Parser/parsetok.c			\
	third_party/python/Parser/pgen.c			\
	third_party/python/Parser/tokenizer.c

THIRD_PARTY_PYTHON_CORE_SRCS =					\
	third_party/python/Python/Python-ast.c			\
	third_party/python/Python/_warnings.c			\
	third_party/python/Python/asdl.c			\
	third_party/python/Python/ast.c				\
	third_party/python/Python/bltinmodule.c			\
	third_party/python/Python/ceval.c			\
	third_party/python/Python/codecs.c			\
	third_party/python/Python/compile.c			\
	third_party/python/Python/dtoa.c			\
	third_party/python/Python/dynamic_annotations.c		\
	third_party/python/Python/dynload_shlib.c		\
	third_party/python/Python/errors.c			\
	third_party/python/Python/fileutils.c			\
	third_party/python/Python/formatter_unicode.c		\
	third_party/python/Python/frozen.c			\
	third_party/python/Python/frozenmain.c			\
	third_party/python/Python/future.c			\
	third_party/python/Python/getargs.c			\
	third_party/python/Python/getcompiler.c			\
	third_party/python/Python/getcopyright.c		\
	third_party/python/Python/getopt.c			\
	third_party/python/Python/getplatform.c			\
	third_party/python/Python/getversion.c			\
	third_party/python/Python/graminit.c			\
	third_party/python/Python/import.c			\
	third_party/python/Python/importdl.c			\
	third_party/python/Python/marshal.c			\
	third_party/python/Python/modsupport.c			\
	third_party/python/Python/mysnprintf.c			\
	third_party/python/Python/mystrtoul.c			\
	third_party/python/Python/peephole.c			\
	third_party/python/Python/pyarena.c			\
	third_party/python/Python/pyctype.c			\
	third_party/python/Python/pyfpe.c			\
	third_party/python/Python/pyhash.c			\
	third_party/python/Python/pylifecycle.c			\
	third_party/python/Python/pymath.c			\
	third_party/python/Python/pystate.c			\
	third_party/python/Python/pystrcmp.c			\
	third_party/python/Python/pystrhex.c			\
	third_party/python/Python/pystrtod.c			\
	third_party/python/Python/pythonrun.c			\
	third_party/python/Python/pytime.c			\
	third_party/python/Python/random.c			\
	third_party/python/Python/sigcheck.c			\
	third_party/python/Python/structmember.c		\
	third_party/python/Python/symtable.c			\
	third_party/python/Python/sysmodule.c			\
	third_party/python/Python/traceback.c

THIRD_PARTY_PYTHON_SRCS_C =					\
	third_party/python/Programs/python.c			\
	$(THIRD_PARTY_PYTHON_OBJECT_SRCS)			\
	$(THIRD_PARTY_PYTHON_PARSER_SRCS)			\
	$(THIRD_PARTY_PYTHON_CORE_SRCS)				\
	$(THIRD_PARTY_PYTHON_MODULES_SRCS)

THIRD_PARTY_PYTHON_OBJS =					\
	$(THIRD_PARTY_PYTHON_SRCS_C:%.c=o/$(MODE)/%.o)
THIRD_PARTY_PYTHON_A_OBJS =					\
	$(filter-out o/$(MODE)/third_party/python/Programs/python.o,$(THIRD_PARTY_PYTHON_OBJS))

THIRD_PARTY_PYTHON_BINS =					\
	$(THIRD_PARTY_PYTHON_COMS) $(THIRD_PARTY_PYTHON_COMS:%=%.dbg)
THIRD_PARTY_PYTHON_COMS =					\
	o/$(MODE)/third_party/python/python.com

THIRD_PARTY_PYTHON_A_CHECKS =					\
	$(THIRD_PARTY_PYTHON_A).pkg
# add .h.ok check here

THIRD_PARTY_PYTHON_A_DIRECTDEPS0 =				\
	$(THIRD_PARTY_PYTHON_MODULES_DIRECTDEPS)		\
	LIBC_ALG						\
	LIBC_BITS						\
	LIBC_CALLS						\
	LIBC_FMT						\
	LIBC_INTRIN						\
	LIBC_LOG						\
	LIBC_MEM						\
	LIBC_NEXGEN32E						\
	LIBC_RAND						\
	LIBC_RUNTIME						\
	LIBC_SOCK						\
	LIBC_DNS						\
	LIBC_STDIO						\
	LIBC_STR						\
	LIBC_STUBS						\
	LIBC_SYSV						\
	LIBC_SYSV_CALLS						\
	LIBC_TIME						\
	LIBC_TINYMATH						\
	LIBC_UNICODE						\
	LIBC_ZIPOS						\
	THIRD_PARTY_GDTOA					\
	THIRD_PARTY_GETOPT					\
	THIRD_PARTY_MUSL					\
	THIRD_PARTY_ZLIB

THIRD_PARTY_PYTHON_A_DIRECTDEPS =				\
	$(call uniq,$(THIRD_PARTY_PYTHON_A_DIRECTDEPS0))
THIRD_PARTY_PYTHON_A_DEPS =					\
	$(call uniq,$(foreach x,$(THIRD_PARTY_PYTHON_A_DIRECTDEPS),$($(x))))

o/$(MODE)/third_party/python/python.com.dbg:			\
		$(THIRD_PARTY_PYTHON_A_DEPS)			\
		$(THIRD_PARTY_PYTHON_A)				\
		$(THIRD_PARTY_PYTHON_STDLIB_PY_OBJS)		\
		o/$(MODE)/third_party/python/Programs/python.o	\
		$(CRT)						\
		$(APE)
	-@$(APELINK)

$(THIRD_PARTY_PYTHON_A):					\
	third_party/python					\
	$(THIRD_PARTY_PYTHON_A).pkg				\
	$(THIRD_PARTY_PYTHON_A_OBJS)

$(THIRD_PARTY_PYTHON_A).pkg:					\
	$(THIRD_PARTY_PYTHON_A_OBJS)				\
	$(foreach x,$(THIRD_PARTY_PYTHON_A_DIRECTDEPS),$($(x)_A).pkg)

$(THIRD_PARTY_PYTHON_OBJS):					\
	OVERRIDE_CFLAGS +=					\
		-DNDEBUG					\
		-DPy_BUILD_CORE					\
		-DPLATFORM='"cosmo"'				\
		-DMULTIARCH='"x86_64-cosmo"'

o/$(MODE)/third_party/python/Programs/python.o			\
o/$(MODE)/third_party/python/Python/marshal.o			\
o/$(MODE)/third_party/python/Python/sysmodule.o			\
o/$(MODE)/third_party/python/Modules/selectmodule.o		\
o/$(MODE)/third_party/python/Modules/getpath.o			\
o/$(MODE)/third_party/python/Objects/listobject.o:		\
	OVERRIDE_CFLAGS +=					\
		-DSTACK_FRAME_UNLIMITED

o/$(MODE)/third_party/python/Python/dynload_shlib.o:		\
	OVERRIDE_CFLAGS +=					\
		-DSOABI='"cpython36m-x86_64-cosmopolitan"'

o/$(MODE)/third_party/python/Python/sysmodule.o:		\
	OVERRIDE_CFLAGS +=					\
		-DABIFLAGS='"m"'

$(THIRD_PARTY_PYTHON_OBJS):					\
	third_party/python/python.mk				\
	third_party/python/python-modules.mk

.PHONY: o/$(MODE)/third_party/python
o/$(MODE)/third_party/python:					\
	$(THIRD_PARTY_PYTHON_BINS)				\
	$(THIRD_PARTY_PYTHON_CHECKS)
