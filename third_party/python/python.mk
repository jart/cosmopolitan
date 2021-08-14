#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#───vi: set et ft=make ts=8 tw=8 fenc=utf-8 :vi───────────────────────┘

PKGS += THIRD_PARTY_PYTHON

THIRD_PARTY_PYTHON_ARTIFACTS = THIRD_PARTY_PYTHON_A
THIRD_PARTY_PYTHON = $(THIRD_PARTY_PYTHON_DEPS) $(THIRD_PARTY_PYTHON_A)
THIRD_PARTY_PYTHON_A = o/$(MODE)/third_party/python/libpython3.6m.a

include third_party/python/python-modules.mk
include third_party/python/python-stdlib.mk

THIRD_PARTY_PYTHON_A_HDRS =						\
	third_party/python/Include/object.h				\
	third_party/python/Include/Python-ast.h				\
	third_party/python/Include/Python.h				\
	third_party/python/Include/abstract.h				\
	third_party/python/Include/accu.h				\
	third_party/python/Include/asdl.h				\
	third_party/python/Include/ast.h				\
	third_party/python/Include/bitset.h				\
	third_party/python/Include/bltinmodule.h			\
	third_party/python/Include/boolobject.h				\
	third_party/python/Include/bytearrayobject.h			\
	third_party/python/Include/bytes_methods.h			\
	third_party/python/Include/bytesobject.h			\
	third_party/python/Include/cellobject.h				\
	third_party/python/Include/ceval.h				\
	third_party/python/Include/classobject.h			\
	third_party/python/Include/code.h				\
	third_party/python/Include/codecs.h				\
	third_party/python/Include/compile.h				\
	third_party/python/Include/complexobject.h			\
	third_party/python/Include/datetime.h				\
	third_party/python/Include/descrobject.h			\
	third_party/python/Include/dictobject.h				\
	third_party/python/Include/dtoa.h				\
	third_party/python/Include/dynamic_annotations.h		\
	third_party/python/Include/enumobject.h				\
	third_party/python/Include/errcode.h				\
	third_party/python/Include/eval.h				\
	third_party/python/Include/fileobject.h				\
	third_party/python/Include/fileutils.h				\
	third_party/python/Include/floatobject.h			\
	third_party/python/Include/frameobject.h			\
	third_party/python/Include/funcobject.h				\
	third_party/python/Include/genobject.h				\
	third_party/python/Include/graminit.h				\
	third_party/python/Include/grammar.h				\
	third_party/python/Include/import.h				\
	third_party/python/Include/intrcheck.h				\
	third_party/python/Include/iterobject.h				\
	third_party/python/Include/listobject.h				\
	third_party/python/Include/longintrepr.h			\
	third_party/python/Include/longobject.h				\
	third_party/python/Include/marshal.h				\
	third_party/python/Include/memoryobject.h			\
	third_party/python/Include/metagrammar.h			\
	third_party/python/Include/methodobject.h			\
	third_party/python/Include/modsupport.h				\
	third_party/python/Include/moduleobject.h			\
	third_party/python/Include/namespaceobject.h			\
	third_party/python/Include/node.h				\
	third_party/python/Include/objimpl.h				\
	third_party/python/Include/odictobject.h			\
	third_party/python/Include/op.h					\
	third_party/python/Include/opcode.h				\
	third_party/python/Include/osdefs.h				\
	third_party/python/Include/osmodule.h				\
	third_party/python/Include/parsetok.h				\
	third_party/python/Include/patchlevel.h				\
	third_party/python/Include/pgen.h				\
	third_party/python/Include/pgenheaders.h			\
	third_party/python/Include/pyarena.h				\
	third_party/python/Include/pyatomic.h				\
	third_party/python/Include/pycapsule.h				\
	third_party/python/Include/pyctype.h				\
	third_party/python/Include/pydebug.h				\
	third_party/python/Include/pydtrace.h				\
	third_party/python/Include/pyerrors.h				\
	third_party/python/Include/pyexpat.h				\
	third_party/python/Include/pyfpe.h				\
	third_party/python/Include/pygetopt.h				\
	third_party/python/Include/pyhash.h				\
	third_party/python/Include/pylifecycle.h			\
	third_party/python/Include/pymacro.h				\
	third_party/python/Include/pymath.h				\
	third_party/python/Include/pymem.h				\
	third_party/python/Include/pyport.h				\
	third_party/python/Include/pystate.h				\
	third_party/python/Include/pystrcmp.h				\
	third_party/python/Include/pystrhex.h				\
	third_party/python/Include/pystrtod.h				\
	third_party/python/Include/pythonrun.h				\
	third_party/python/Include/pythread.h				\
	third_party/python/Include/pytime.h				\
	third_party/python/Include/rangeobject.h			\
	third_party/python/Include/setobject.h				\
	third_party/python/Include/sliceobject.h			\
	third_party/python/Include/structmember.h			\
	third_party/python/Include/structseq.h				\
	third_party/python/Include/symtable.h				\
	third_party/python/Include/sysmodule.h				\
	third_party/python/Include/token.h				\
	third_party/python/Include/traceback.h				\
	third_party/python/Include/tupleobject.h			\
	third_party/python/Include/typeslots.h				\
	third_party/python/Include/ucnhash.h				\
	third_party/python/Include/unicodeobject.h			\
	third_party/python/Include/warnings.h				\
	third_party/python/Include/weakrefobject.h			\
	third_party/python/Modules/_decimal/docstrings.h		\
	third_party/python/Modules/_decimal/libmpdec/basearith.h	\
	third_party/python/Modules/_decimal/libmpdec/bits.h		\
	third_party/python/Modules/_decimal/libmpdec/constants.h	\
	third_party/python/Modules/_decimal/libmpdec/convolute.h	\
	third_party/python/Modules/_decimal/libmpdec/crt.h		\
	third_party/python/Modules/_decimal/libmpdec/difradix2.h	\
	third_party/python/Modules/_decimal/libmpdec/fnt.h		\
	third_party/python/Modules/_decimal/libmpdec/fourstep.h		\
	third_party/python/Modules/_decimal/libmpdec/io.h		\
	third_party/python/Modules/_decimal/libmpdec/mpalloc.h		\
	third_party/python/Modules/_decimal/libmpdec/mpdecimal.h	\
	third_party/python/Modules/_decimal/libmpdec/numbertheory.h	\
	third_party/python/Modules/_decimal/libmpdec/sixstep.h		\
	third_party/python/Modules/_decimal/libmpdec/transpose.h	\
	third_party/python/Modules/_decimal/libmpdec/typearith.h	\
	third_party/python/Modules/_decimal/libmpdec/umodarith.h	\
	third_party/python/Modules/_io/_iomodule.h			\
	third_party/python/Modules/_math.h				\
	third_party/python/Modules/_multiprocessing/multiprocessing.h	\
	third_party/python/Modules/_sqlite/cache.h			\
	third_party/python/Modules/_sqlite/connection.h			\
	third_party/python/Modules/_sqlite/cursor.h			\
	third_party/python/Modules/_sqlite/microprotocols.h		\
	third_party/python/Modules/_sqlite/module.h			\
	third_party/python/Modules/_sqlite/prepare_protocol.h		\
	third_party/python/Modules/_sqlite/row.h			\
	third_party/python/Modules/_sqlite/statement.h			\
	third_party/python/Modules/_sqlite/util.h			\
	third_party/python/Modules/cjkcodecs/cjkcodecs.h		\
	third_party/python/Modules/cjkcodecs/multibytecodec.h		\
	third_party/python/Modules/expat/expat.h			\
	third_party/python/Modules/expat/expat_config.h			\
	third_party/python/Modules/expat/expat_external.h		\
	third_party/python/Modules/expat/pyexpatns.h			\
	third_party/python/Modules/expat/xmlrole.h			\
	third_party/python/Modules/expat/xmltok.h			\
	third_party/python/Modules/hashlib.h				\
	third_party/python/Modules/hashtable.h				\
	third_party/python/Modules/posixmodule.h			\
	third_party/python/Modules/rotatingtree.h			\
	third_party/python/Modules/socketmodule.h			\
	third_party/python/Modules/sre.h				\
	third_party/python/Modules/sre_constants.h			\
	third_party/python/Modules/tkinter.h				\
	third_party/python/Modules/winreparse.h				\
	third_party/python/Objects/dict-common.h			\
	third_party/python/Parser/parser.h				\
	third_party/python/Parser/tokenizer.h				\
	third_party/python/Python/condvar.h				\
	third_party/python/Python/importdl.h				\
	third_party/python/pyconfig.h

THIRD_PARTY_PYTHON_A_INCS =						\
	third_party/python/Objects/stringlib/localeutil.inc		\
	third_party/python/Objects/stringlib/unicodedefs.inc		\
	third_party/python/Objects/stringlib/replace.inc		\
	third_party/python/Objects/stringlib/unicode_format.inc		\
	third_party/python/Objects/stringlib/find_max_char.inc		\
	third_party/python/Objects/stringlib/find.inc			\
	third_party/python/Objects/stringlib/split.inc			\
	third_party/python/Objects/stringlib/transmogrify.inc		\
	third_party/python/Objects/stringlib/ucs1lib.inc		\
	third_party/python/Objects/stringlib/count.inc			\
	third_party/python/Objects/stringlib/undef.inc			\
	third_party/python/Objects/stringlib/partition.inc		\
	third_party/python/Objects/stringlib/ucs4lib.inc		\
	third_party/python/Objects/stringlib/asciilib.inc		\
	third_party/python/Objects/stringlib/eq.inc			\
	third_party/python/Objects/stringlib/ucs2lib.inc		\
	third_party/python/Objects/stringlib/join.inc			\
	third_party/python/Objects/stringlib/fastsearch.inc		\
	third_party/python/Objects/stringlib/ctype.inc			\
	third_party/python/Objects/stringlib/codecs.inc			\
	third_party/python/Objects/stringlib/stringdefs.inc		\
	third_party/python/Objects/unicodetype_db.inc			\
	third_party/python/Objects/clinic/bytearrayobject.inc		\
	third_party/python/Objects/clinic/unicodeobject.inc		\
	third_party/python/Objects/clinic/dictobject.inc		\
	third_party/python/Objects/clinic/bytesobject.inc		\
	third_party/python/Objects/typeslots.inc			\
	third_party/python/Python/thread_pthread.inc			\
	third_party/python/Python/opcode_targets.inc			\
	third_party/python/Python/wordcode_helpers.inc			\
	third_party/python/Python/ceval_gil.inc				\
	third_party/python/Python/thread_nt.inc				\
	third_party/python/Python/importlib_external.inc		\
	third_party/python/Python/importlib.inc				\
	third_party/python/Python/clinic/import.inc			\
	third_party/python/Python/clinic/bltinmodule.inc		\
	third_party/python/Modules/expat/asciitab.inc			\
	third_party/python/Modules/expat/nametab.inc			\
	third_party/python/Modules/expat/utf8tab.inc			\
	third_party/python/Modules/expat/internal.inc			\
	third_party/python/Modules/expat/ascii.inc			\
	third_party/python/Modules/expat/xmltok_ns.inc			\
	third_party/python/Modules/expat/xmltok_impl.inc		\
	third_party/python/Modules/expat/siphash.inc			\
	third_party/python/Modules/expat/latin1tab.inc			\
	third_party/python/Modules/expat/xmltok_impl.h.inc		\
	third_party/python/Modules/expat/iasciitab.inc			\
	third_party/python/Modules/_ssl_data.inc			\
	third_party/python/Modules/cjkcodecs/emu_jisx0213_2000.inc	\
	third_party/python/Modules/cjkcodecs/alg_jisx0201.inc		\
	third_party/python/Modules/cjkcodecs/mappings_hk.inc		\
	third_party/python/Modules/cjkcodecs/mappings_cn.inc		\
	third_party/python/Modules/cjkcodecs/mappings_jisx0213_pair.inc	\
	third_party/python/Modules/cjkcodecs/mappings_kr.inc		\
	third_party/python/Modules/cjkcodecs/mappings_jp.inc		\
	third_party/python/Modules/cjkcodecs/mappings_tw.inc		\
	third_party/python/Modules/cjkcodecs/clinic/multibytecodec.inc	\
	third_party/python/Modules/sre_lib.inc				\
	third_party/python/Modules/unicodename_db.inc			\
	third_party/python/Modules/testcapi_long.inc			\
	third_party/python/Modules/_io/clinic/bufferedio.inc		\
	third_party/python/Modules/_io/clinic/bytesio.inc		\
	third_party/python/Modules/_io/clinic/iobase.inc		\
	third_party/python/Modules/_io/clinic/textio.inc		\
	third_party/python/Modules/_io/clinic/fileio.inc		\
	third_party/python/Modules/_io/clinic/stringio.inc		\
	third_party/python/Modules/_io/clinic/winconsoleio.inc		\
	third_party/python/Modules/_io/clinic/_iomodule.inc		\
	third_party/python/Modules/clinic/arraymodule.inc		\
	third_party/python/Modules/clinic/_datetimemodule.inc		\
	third_party/python/Modules/clinic/_lzmamodule.inc		\
	third_party/python/Modules/clinic/_weakref.inc			\
	third_party/python/Modules/clinic/posixmodule.inc		\
	third_party/python/Modules/clinic/_hashopenssl.inc		\
	third_party/python/Modules/clinic/zlibmodule.inc		\
	third_party/python/Modules/clinic/binascii.inc			\
	third_party/python/Modules/clinic/_bz2module.inc		\
	third_party/python/Modules/clinic/pyexpat.inc			\
	third_party/python/Modules/clinic/_opcode.inc			\
	third_party/python/Modules/clinic/_asynciomodule.inc		\
	third_party/python/Modules/clinic/_ssl.inc			\
	third_party/python/Modules/clinic/_codecsmodule.inc		\
	third_party/python/Modules/clinic/unicodedata.inc		\
	third_party/python/Modules/clinic/grpmodule.inc			\
	third_party/python/Modules/clinic/sha512module.inc		\
	third_party/python/Modules/clinic/spwdmodule.inc		\
	third_party/python/Modules/clinic/signalmodule.inc		\
	third_party/python/Modules/clinic/_cryptmodule.inc		\
	third_party/python/Modules/clinic/fcntlmodule.inc		\
	third_party/python/Modules/clinic/_dbmmodule.inc		\
	third_party/python/Modules/clinic/sha1module.inc		\
	third_party/python/Modules/clinic/_sre.inc			\
	third_party/python/Modules/clinic/md5module.inc			\
	third_party/python/Modules/clinic/_cursesmodule.inc		\
	third_party/python/Modules/clinic/_winapi.inc			\
	third_party/python/Modules/clinic/_gdbmmodule.inc		\
	third_party/python/Modules/clinic/_pickle.inc			\
	third_party/python/Modules/clinic/cmathmodule.inc		\
	third_party/python/Modules/clinic/_elementtree.inc		\
	third_party/python/Modules/clinic/pwdmodule.inc			\
	third_party/python/Modules/clinic/audioop.inc			\
	third_party/python/Modules/clinic/_tkinter.inc			\
	third_party/python/Modules/clinic/sha256module.inc		\
	third_party/python/Modules/unicodedata_db.inc

THIRD_PARTY_PYTHON_OBJECTS_SRCS =					\
	third_party/python/Objects/abstract.c				\
	third_party/python/Objects/accu.c				\
	third_party/python/Objects/boolobject.c				\
	third_party/python/Objects/bytearrayobject.c			\
	third_party/python/Objects/bytes_methods.c			\
	third_party/python/Objects/bytesobject.c			\
	third_party/python/Objects/capsule.c				\
	third_party/python/Objects/cellobject.c				\
	third_party/python/Objects/classobject.c			\
	third_party/python/Objects/codeobject.c				\
	third_party/python/Objects/complexobject.c			\
	third_party/python/Objects/descrobject.c			\
	third_party/python/Objects/dictobject.c				\
	third_party/python/Objects/enumobject.c				\
	third_party/python/Objects/exceptions.c				\
	third_party/python/Objects/fileobject.c				\
	third_party/python/Objects/floatobject.c			\
	third_party/python/Objects/frameobject.c			\
	third_party/python/Objects/funcobject.c				\
	third_party/python/Objects/genobject.c				\
	third_party/python/Objects/iterobject.c				\
	third_party/python/Objects/listobject.c				\
	third_party/python/Objects/longobject.c				\
	third_party/python/Objects/memoryobject.c			\
	third_party/python/Objects/methodobject.c			\
	third_party/python/Objects/moduleobject.c			\
	third_party/python/Objects/namespaceobject.c			\
	third_party/python/Objects/object.c				\
	third_party/python/Objects/obmalloc.c				\
	third_party/python/Objects/odictobject.c			\
	third_party/python/Objects/rangeobject.c			\
	third_party/python/Objects/setobject.c				\
	third_party/python/Objects/sliceobject.c			\
	third_party/python/Objects/structseq.c				\
	third_party/python/Objects/tupleobject.c			\
	third_party/python/Objects/typeobject.c				\
	third_party/python/Objects/unicodectype.c			\
	third_party/python/Objects/unicodeobject.c			\
	third_party/python/Objects/weakrefobject.c

THIRD_PARTY_PYTHON_PARSER_SRCS =					\
	third_party/python/Parser/acceler.c				\
	third_party/python/Parser/bitset.c				\
	third_party/python/Parser/firstsets.c				\
	third_party/python/Parser/grammar.c				\
	third_party/python/Parser/grammar1.c				\
	third_party/python/Parser/listnode.c				\
	third_party/python/Parser/metagrammar.c				\
	third_party/python/Parser/myreadline.c				\
	third_party/python/Parser/node.c				\
	third_party/python/Parser/parser.c				\
	third_party/python/Parser/parsetok.c				\
	third_party/python/Parser/pgen.c				\
	third_party/python/Parser/tokenizer.c

THIRD_PARTY_PYTHON_CORE_SRCS =						\
	third_party/python/Python/Python-ast.c				\
	third_party/python/Python/_warnings.c				\
	third_party/python/Python/asdl.c				\
	third_party/python/Python/ast.c					\
	third_party/python/Python/bltinmodule.c				\
	third_party/python/Python/ceval.c				\
	third_party/python/Python/codecs.c				\
	third_party/python/Python/compile.c				\
	third_party/python/Python/dtoa.c				\
	third_party/python/Python/dynamic_annotations.c			\
	third_party/python/Python/dynload_shlib.c			\
	third_party/python/Python/errors.c				\
	third_party/python/Python/fileutils.c				\
	third_party/python/Python/formatter_unicode.c			\
	third_party/python/Python/frozen.c				\
	third_party/python/Python/frozenmain.c				\
	third_party/python/Python/future.c				\
	third_party/python/Python/getargs.c				\
	third_party/python/Python/getcompiler.c				\
	third_party/python/Python/getcopyright.c			\
	third_party/python/Python/getopt.c				\
	third_party/python/Python/getplatform.c				\
	third_party/python/Python/getversion.c				\
	third_party/python/Python/graminit.c				\
	third_party/python/Python/import.c				\
	third_party/python/Python/importdl.c				\
	third_party/python/Python/marshal.c				\
	third_party/python/Python/modsupport.c				\
	third_party/python/Python/mysnprintf.c				\
	third_party/python/Python/mystrtoul.c				\
	third_party/python/Python/peephole.c				\
	third_party/python/Python/pyarena.c				\
	third_party/python/Python/pyfpe.c				\
	third_party/python/Python/pyhash.c				\
	third_party/python/Python/pylifecycle.c				\
	third_party/python/Python/pymath.c				\
	third_party/python/Python/pystate.c				\
	third_party/python/Python/pystrcmp.c				\
	third_party/python/Python/pystrhex.c				\
	third_party/python/Python/pystrtod.c				\
	third_party/python/Python/pythonrun.c				\
	third_party/python/Python/pytime.c				\
	third_party/python/Python/random.c				\
	third_party/python/Python/sigcheck.c				\
	third_party/python/Python/structmember.c			\
	third_party/python/Python/symtable.c				\
	third_party/python/Python/sysmodule.c				\
	third_party/python/Python/traceback.c

THIRD_PARTY_PYTHON_A_SRCS =						\
	third_party/python/Programs/python.c				\
	$(THIRD_PARTY_PYTHON_OBJECTS_SRCS)				\
	$(THIRD_PARTY_PYTHON_PARSER_SRCS)				\
	$(THIRD_PARTY_PYTHON_CORE_SRCS)					\
	$(THIRD_PARTY_PYTHON_MODULES_SRCS)

THIRD_PARTY_PYTHON_A_OBJS =						\
	$(THIRD_PARTY_PYTHON_A_SRCS:%.c=o/$(MODE)/%.o)

THIRD_PARTY_PYTHON_BINS =						\
	$(THIRD_PARTY_PYTHON_COMS) $(THIRD_PARTY_PYTHON_COMS:%=%.dbg)
THIRD_PARTY_PYTHON_COMS =						\
	o/$(MODE)/third_party/python/python.com

THIRD_PARTY_PYTHON_A_CHECKS =						\
	$(THIRD_PARTY_PYTHON_A).pkg					\
	$(THIRD_PARTY_PYTHON_A_HDRS:%=o/$(MODE)/%.ok)

THIRD_PARTY_PYTHON_A_DIRECTDEPS0 =					\
	$(THIRD_PARTY_PYTHON_MODULES_DIRECTDEPS)			\
	LIBC_ALG							\
	LIBC_BITS							\
	LIBC_CALLS							\
	LIBC_FMT							\
	LIBC_INTRIN							\
	LIBC_LOG							\
	LIBC_MEM							\
	LIBC_NEXGEN32E							\
	LIBC_RAND							\
	LIBC_RUNTIME							\
	LIBC_SOCK							\
	LIBC_DNS							\
	LIBC_STDIO							\
	LIBC_STR							\
	LIBC_STUBS							\
	LIBC_SYSV							\
	LIBC_SYSV_CALLS							\
	LIBC_TIME							\
	LIBC_TINYMATH							\
	LIBC_UNICODE							\
	LIBC_ZIPOS							\
	THIRD_PARTY_GDTOA						\
	THIRD_PARTY_GETOPT						\
	THIRD_PARTY_LINENOISE						\
	THIRD_PARTY_MUSL						\
	THIRD_PARTY_ZLIB

THIRD_PARTY_PYTHON_A_DIRECTDEPS =					\
	$(call uniq,$(THIRD_PARTY_PYTHON_A_DIRECTDEPS0))
THIRD_PARTY_PYTHON_A_DEPS =						\
	$(call uniq,$(foreach x,$(THIRD_PARTY_PYTHON_A_DIRECTDEPS),$($(x))))

o/$(MODE)/third_party/python/python.com.dbg:				\
		$(THIRD_PARTY_PYTHON_A_DEPS)				\
		$(THIRD_PARTY_PYTHON_A)					\
		$(THIRD_PARTY_PYTHON_STDLIB_PY_OBJS)			\
		o/$(MODE)/third_party/python/Programs/python.o		\
		$(CRT)							\
		$(APE)
	-@$(APELINK)

$(THIRD_PARTY_PYTHON_A):						\
	third_party/python						\
	$(THIRD_PARTY_PYTHON_A).pkg					\
	$(THIRD_PARTY_PYTHON_A_OBJS)

$(THIRD_PARTY_PYTHON_A).pkg:						\
	$(THIRD_PARTY_PYTHON_A_OBJS)					\
	$(foreach x,$(THIRD_PARTY_PYTHON_A_DIRECTDEPS),$($(x)_A).pkg)

$(THIRD_PARTY_PYTHON_A_OBJS):						\
		OVERRIDE_CFLAGS +=					\
			-DNDEBUG					\
			-DPy_BUILD_CORE					\
			-DPLATFORM='"cosmo"'				\
			-DMULTIARCH='"x86_64-cosmo"'

o/$(MODE)/third_party/python/Programs/python.o				\
o/$(MODE)/third_party/python/Python/marshal.o				\
o/$(MODE)/third_party/python/Python/sysmodule.o				\
o/$(MODE)/third_party/python/Modules/selectmodule.o			\
o/$(MODE)/third_party/python/Modules/getpath.o				\
o/$(MODE)/third_party/python/Objects/listobject.o:			\
		OVERRIDE_CFLAGS +=					\
			-DSTACK_FRAME_UNLIMITED

o/$(MODE)/third_party/python/Python/dynload_shlib.o:			\
	OVERRIDE_CFLAGS +=						\
		-DSOABI='"cpython36m-x86_64-cosmo"'

o/$(MODE)/third_party/python/Python/sysmodule.o:			\
	OVERRIDE_CFLAGS +=						\
		-DABIFLAGS='"m"'

o/$(MODE)/third_party/python/Objects/unicodeobject.o: QUOTA += -C16

$(THIRD_PARTY_PYTHON_A_OBJS):						\
	OVERRIDE_CFLAGS +=						\
		-ffunction-sections					\
		-fdata-sections

$(THIRD_PARTY_PYTHON_A_OBJS):						\
	third_party/python/python.mk					\
	third_party/python/python-modules.mk

THIRD_PARTY_PYTHON_LIBS = $(foreach x,$(THIRD_PARTY_PYTHON_ARTIFACTS),$($(x)))
THIRD_PARTY_PYTHON_SRCS = $(foreach x,$(THIRD_PARTY_PYTHON_ARTIFACTS),$($(x)_SRCS))
THIRD_PARTY_PYTHON_INCS = $(foreach x,$(THIRD_PARTY_PYTHON_ARTIFACTS),$($(x)_INCS))
THIRD_PARTY_PYTHON_HDRS = $(foreach x,$(THIRD_PARTY_PYTHON_ARTIFACTS),$($(x)_HDRS))
THIRD_PARTY_PYTHON_CHECKS = $(foreach x,$(THIRD_PARTY_PYTHON_ARTIFACTS),$($(x)_CHECKS))
THIRD_PARTY_PYTHON_OBJS = $(foreach x,$(THIRD_PARTY_PYTHON_ARTIFACTS),$($(x)_OBJS))

.PHONY: o/$(MODE)/third_party/python
o/$(MODE)/third_party/python:						\
	$(THIRD_PARTY_PYTHON_BINS)					\
	$(THIRD_PARTY_PYTHON_CHECKS)
