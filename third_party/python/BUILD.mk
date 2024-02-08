#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#── vi: set noet ft=make ts=8 sw=8 fenc=utf-8 :vi ────────────────────┘

PKGS += THIRD_PARTY_PYTHON

THIRD_PARTY_PYTHON_ARTIFACTS =						\
	THIRD_PARTY_PYTHON_STAGE1_A					\
	THIRD_PARTY_PYTHON_STAGE2_A					\
	THIRD_PARTY_PYTHON_PYTEST_A					\
	THIRD_PARTY_PYTHON_PYTHON					\
	THIRD_PARTY_PYTHON_FREEZE

THIRD_PARTY_PYTHON_BINS =						\
	$(THIRD_PARTY_PYTHON_COMS)					\
	$(THIRD_PARTY_PYTHON_COMS:%=%.dbg)

THIRD_PARTY_PYTHON_COMS =						\
	o/$(MODE)/third_party/python/Parser/asdl_c.com			\
	o/$(MODE)/third_party/python/pystone.com			\
	o/$(MODE)/third_party/python/python.com				\
	o/$(MODE)/third_party/python/freeze.com				\
	o/$(MODE)/third_party/python/pycomp.com				\
	o/$(MODE)/third_party/python/pyobj.com				\
	o/$(MODE)/third_party/python/hello.com				\
	o/$(MODE)/third_party/python/repl.com

THIRD_PARTY_PYTHON_CHECKS =						\
	$(THIRD_PARTY_PYTHON_STAGE1_A).pkg 				\
	$(THIRD_PARTY_PYTHON_STAGE2_A).pkg				\
	$(THIRD_PARTY_PYTHON_PYTEST_A).pkg				\
	$(THIRD_PARTY_PYTHON_HDRS:%=o/$(MODE)/%.ok)			\
	o/$(MODE)/third_party/python/python.pkg				\
	o/$(MODE)/third_party/python/freeze.pkg

# TODO: Deal with aarch64 under qemu not making execve() easy.
ifneq ($(MODE), dbg)
ifeq ($(ARCH), x86_64)
ifneq ($(UNAME_S), Windows)
THIRD_PARTY_PYTHON_CHECKS +=						\
	$(THIRD_PARTY_PYTHON_PYTEST_PYMAINS:%=o/$(MODE)/%.runs)
endif
endif
endif

################################################################################
# STAGE ONE - BOOTSTRAPPING PYTHON

THIRD_PARTY_PYTHON_STAGE1 =						\
	$(THIRD_PARTY_PYTHON_STAGE1_A_DEPS)				\
	$(THIRD_PARTY_PYTHON_STAGE1_A)

THIRD_PARTY_PYTHON_STAGE1_A =						\
	o/$(MODE)/third_party/python/python-stage1.a

THIRD_PARTY_PYTHON_STAGE1_A_OBJS =					\
	$(THIRD_PARTY_PYTHON_STAGE1_A_SRCS:%.c=o/$(MODE)/%.o)

THIRD_PARTY_PYTHON_HDRS =						\
	third_party/python/runpythonmodule.h				\
	third_party/python/Include/ezprint.h				\
	third_party/python/Include/yoink.h				\
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
	third_party/python/Include/cosmo.h				\
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
	third_party/python/Modules/bextra.h				\
	third_party/python/Modules/unicodedata.h			\
	third_party/python/Modules/unicodedata_unidata.h		\
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
	third_party/python/Modules/cjkcodecs/xloadzd.h			\
	third_party/python/Modules/cjkcodecs/cjkcodecs.h		\
	third_party/python/Modules/cjkcodecs/multibytecodec.h		\
	third_party/python/Modules/cjkcodecs/somanyencodings.h		\
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
	third_party/python/Modules/winreparse.h				\
	third_party/python/Objects/dict-common.h			\
	third_party/python/Parser/parser.h				\
	third_party/python/Parser/tokenizer.h				\
	third_party/python/Python/condvar.h				\
	third_party/python/Python/importdl.h				\
	third_party/python/pyconfig.h

THIRD_PARTY_PYTHON_INCS =						\
	third_party/python/chibicc.inc					\
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
	third_party/python/Objects/clinic/bytearrayobject.inc		\
	third_party/python/Objects/clinic/unicodeobject.inc		\
	third_party/python/Objects/clinic/dictobject.inc		\
	third_party/python/Objects/clinic/bytesobject.inc		\
	third_party/python/Objects/typeslots.inc			\
	third_party/python/Python/thread_pthread.inc			\
	third_party/python/Python/opcode_targets.inc			\
	third_party/python/Python/wordcode_helpers.inc			\
	third_party/python/Python/ceval_gil.inc				\
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
	third_party/python/Modules/cjkcodecs/emu_jisx0213_2000.inc	\
	third_party/python/Modules/cjkcodecs/alg_jisx0201.inc		\
	third_party/python/Modules/cjkcodecs/clinic/multibytecodec.inc	\
	third_party/python/Modules/sre_lib.inc				\
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
	third_party/python/Modules/clinic/zlibmodule.inc		\
	third_party/python/Modules/clinic/binascii.inc			\
	third_party/python/Modules/clinic/_bz2module.inc		\
	third_party/python/Modules/clinic/pyexpat.inc			\
	third_party/python/Modules/clinic/_opcode.inc			\
	third_party/python/Modules/clinic/_asynciomodule.inc		\
	third_party/python/Modules/clinic/_codecsmodule.inc		\
	third_party/python/Modules/clinic/unicodedata.inc		\
	third_party/python/Modules/clinic/grpmodule.inc			\
	third_party/python/Modules/clinic/spwdmodule.inc		\
	third_party/python/Modules/clinic/signalmodule.inc		\
	third_party/python/Modules/clinic/_cryptmodule.inc		\
	third_party/python/Modules/clinic/fcntlmodule.inc		\
	third_party/python/Modules/clinic/_sre.inc			\
	third_party/python/Modules/clinic/_cursesmodule.inc		\
	third_party/python/Modules/clinic/_winapi.inc			\
	third_party/python/Modules/clinic/_pickle.inc			\
	third_party/python/Modules/clinic/cmathmodule.inc		\
	third_party/python/Modules/clinic/_elementtree.inc		\
	third_party/python/Modules/clinic/_struct.inc			\
	third_party/python/Modules/clinic/pwdmodule.inc			\
	third_party/python/Modules/clinic/audioop.inc

THIRD_PARTY_PYTHON_STAGE1_A_SRCS =					\
	third_party/python/Modules/_threadmodule.c			\
	third_party/python/Modules/_tracemalloc.c			\
	third_party/python/Modules/faulthandler.c			\
	third_party/python/Objects/abstract.c				\
	third_party/python/Modules/fspath.c				\
	third_party/python/Modules/gcmodule.c				\
	third_party/python/Modules/getbuildinfo.c			\
	third_party/python/Modules/getpath.c				\
	third_party/python/Modules/hashtable.c				\
	third_party/python/Modules/signalmodule.c			\
	third_party/python/Objects/boolobject.c				\
	third_party/python/Objects/bytearrayobject.c			\
	third_party/python/Objects/bytes_methods.c			\
	third_party/python/Objects/bytesobject.c			\
	third_party/python/Objects/capsule.c				\
	third_party/python/Objects/call.c				\
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
	third_party/python/Objects/weakrefobject.c			\
	third_party/python/Parser/acceler.c				\
	third_party/python/Parser/grammar1.c				\
	third_party/python/Parser/myreadline.c				\
	third_party/python/Parser/node.c				\
	third_party/python/Parser/parser.c				\
	third_party/python/Parser/parsetok.c				\
	third_party/python/Parser/tokenizer.c				\
	third_party/python/Python/Python-ast.c				\
	third_party/python/Python/_warnings.c				\
	third_party/python/Python/asdl.c				\
	third_party/python/Python/ast.c					\
	third_party/python/Python/bltinmodule.c				\
	third_party/python/Python/ceval.c				\
	third_party/python/Python/codecs.c				\
	third_party/python/Python/compile.c				\
	third_party/python/Python/cosmomodule.c				\
	third_party/python/Python/xtermmodule.c				\
	third_party/python/Python/xedmodule.c				\
	third_party/python/Python/dtoa.c				\
	third_party/python/Python/dynload_shlib.c			\
	third_party/python/Python/errors.c				\
	third_party/python/Python/fileutils.c				\
	third_party/python/Python/formatter_unicode.c			\
	third_party/python/Python/frozenmodules.c			\
	third_party/python/Python/future.c				\
	third_party/python/Python/getargs.c				\
	third_party/python/Python/getcompiler.c				\
	third_party/python/Python/getcopyright.c			\
	third_party/python/Python/getplatform.c				\
	third_party/python/Python/getversion.c				\
	third_party/python/Python/graminit.c				\
	third_party/python/Python/import.c				\
	third_party/python/Python/importdl.c				\
	third_party/python/Python/inittab.c				\
	third_party/python/Python/marshal.c				\
	third_party/python/Python/modsupport.c				\
	third_party/python/Python/mysnprintf.c				\
	third_party/python/Python/mystrtoul.c				\
	third_party/python/Python/peephole.c				\
	third_party/python/Python/pyarena.c				\
	third_party/python/Python/pyhash.c				\
	third_party/python/Python/pylifecycle.c				\
	third_party/python/Python/initstdio.c				\
	third_party/python/Python/initsite.c				\
	third_party/python/Python/initimport.c				\
	third_party/python/Python/initfsencoding.c			\
	third_party/python/Python/initsigs.c				\
	third_party/python/Python/initmain.c				\
	third_party/python/Python/finalize.c				\
	third_party/python/Python/initialized.c				\
	third_party/python/Python/atexit.c				\
	third_party/python/Python/flushstdfiles.c			\
	third_party/python/Python/progname.c				\
	third_party/python/Python/restoresigs.c				\
	third_party/python/Python/fdisinteractive.c			\
	third_party/python/Python/getsig.c				\
	third_party/python/Python/fatality.c				\
	third_party/python/Python/flags.c				\
	third_party/python/Python/pymath.c				\
	third_party/python/Python/pystate.c				\
	third_party/python/Python/pystrcmp.c				\
	third_party/python/Python/pystrhex.c				\
	third_party/python/Python/pystrtod.c				\
	third_party/python/Python/pythonrun.c				\
	third_party/python/Python/pytime.c				\
	third_party/python/Python/random.c				\
	third_party/python/Python/structmember.c			\
	third_party/python/Python/symtable.c				\
        third_party/python/Parser/listnode.c				\
	third_party/python/Python/sysmodule.c				\
	third_party/python/Python/thread.c				\
	third_party/python/Python/traceback.c				\
	third_party/python/Modules/bextra.c				\
	third_party/python/Modules/unicodedata_3.2.0.c			\
	third_party/python/Modules/unicodedata_bidirectionalnames.c	\
	third_party/python/Modules/unicodedata_categorynames.c		\
	third_party/python/Modules/unicodedata_comp.c			\
	third_party/python/Modules/unicodedata_decomp.c			\
	third_party/python/Modules/unicodedata_decompprefix.c		\
	third_party/python/Modules/unicodedata_eastasianwidthnames.c	\
	third_party/python/Modules/unicodedata_getrecord.c		\
	third_party/python/Modules/unicodedata_isnormalized.c		\
	third_party/python/Modules/unicodedata_nfcfirst.c		\
	third_party/python/Modules/unicodedata_nfclast.c		\
	third_party/python/Modules/unicodedata_records.c		\
	third_party/python/Modules/unicodedata_namedsequences.c		\
	third_party/python/Modules/unicodedata_aliases.c		\
	third_party/python/Modules/unicodedata_lexicon.c		\
	third_party/python/Modules/unicodedata_phrasebook.c		\
	third_party/python/Modules/unicodedata_nfdnfkd.c		\
	third_party/python/Modules/unicodedata_nfcnfkc.c		\
	third_party/python/Modules/unicodedata_findnfcindex.c		\
	third_party/python/Modules/unicodedata_getcode.c		\
	third_party/python/Modules/unicodedata_codehash.c		\
	third_party/python/Modules/unicodedata_getdecomprecord.c	\
	third_party/python/Modules/unicodedata_isunifiedideograph.c	\
	third_party/python/Modules/unicodedata_iswhitespace.c		\
	third_party/python/Modules/unicodedata_islinebreak.c		\
	third_party/python/Modules/unicodedata_tonumeric.c		\
	third_party/python/Modules/unicodedata_extendedcase.c		\
	third_party/python/Modules/unicodedata_typerecords.c		\
	third_party/python/Modules/unicodedata_ucd.c

THIRD_PARTY_PYTHON_STAGE1_A_DIRECTDEPS =				\
	DSP_SCALE							\
	LIBC_CALLS							\
	LIBC_DLOPEN							\
	LIBC_FMT							\
	LIBC_INTRIN							\
	LIBC_LOG							\
	LIBC_MEM							\
	LIBC_NEXGEN32E							\
	LIBC_NT_KERNEL32						\
	LIBC_PROC							\
	LIBC_RUNTIME							\
	LIBC_STDIO							\
	LIBC_STR							\
	LIBC_SYSV							\
	LIBC_SYSV_CALLS							\
	LIBC_THREAD							\
	LIBC_TIME							\
	LIBC_TINYMATH							\
	LIBC_X								\
	THIRD_PARTY_DLMALLOC						\
	THIRD_PARTY_GETOPT						\
	THIRD_PARTY_XED							\
	TOOL_BUILD_LIB							\
	TOOL_ARGS

THIRD_PARTY_PYTHON_STAGE1_A_DEPS =					\
	$(call uniq,$(foreach x,$(THIRD_PARTY_PYTHON_STAGE1_A_DIRECTDEPS),$($(x))))

o//third_party/python/Python/importlib.inc:				\
		o/$(MODE)/third_party/python/freeze.com			\
		third_party/python/Lib/importlib/_bootstrap.py
	@$(COMPILE) -AFREEZE -wT$@ $^ $@

o//third_party/python/Python/importlib_external.inc:			\
		o/$(MODE)/third_party/python/freeze.com			\
		third_party/python/Lib/importlib/_bootstrap_external.py
	@$(COMPILE) -AFREEZE -wT$@ $^ $@

$(THIRD_PARTY_PYTHON_STAGE1_A):						\
		$(THIRD_PARTY_PYTHON_STAGE1_A).pkg			\
		$(THIRD_PARTY_PYTHON_STAGE1_A_OBJS)

$(THIRD_PARTY_PYTHON_STAGE1_A).pkg:					\
		$(THIRD_PARTY_PYTHON_STAGE1_A_OBJS)			\
		$(foreach x,$(THIRD_PARTY_PYTHON_STAGE1_A_DIRECTDEPS),$($(x)_A).pkg)

################################################################################
# STAGE TWO - PYTHON RUNTIME

THIRD_PARTY_PYTHON_STAGE2 =						\
	$(THIRD_PARTY_PYTHON_STAGE2_A_DEPS)				\
	$(THIRD_PARTY_PYTHON_STAGE2_A)

THIRD_PARTY_PYTHON_STAGE2_A =						\
	o/$(MODE)/third_party/python/python-stage2.a

THIRD_PARTY_PYTHON_STAGE2_A_OBJS =					\
	$(THIRD_PARTY_PYTHON_STAGE2_A_SRCS:%.c=o/$(MODE)/%.o)		\
	$(THIRD_PARTY_PYTHON_STAGE2_A_PYS_OBJS)				\
	$(THIRD_PARTY_PYTHON_STAGE2_A_DATA_OBJS)

THIRD_PARTY_PYTHON_STAGE2_A_PYS_OBJS =					\
	$(THIRD_PARTY_PYTHON_STAGE2_A_PYS:%.py=o/$(MODE)/%.o)

THIRD_PARTY_PYTHON_STAGE2_A_DATA_OBJS =					\
	$(THIRD_PARTY_PYTHON_STAGE2_A_DATA:%=o/$(MODE)/%.zip.o)		\
	o/$(MODE)/third_party/python/Lib/.zip.o

THIRD_PARTY_PYTHON_STAGE2_A_SRCS =					\
	third_party/python/runpythonmodule.c				\
	third_party/python/launch.c					\
	third_party/python/Objects/fromfd.c				\
	third_party/python/Objects/unicodeobject-deadcode.c		\
	third_party/python/Modules/_bisectmodule.c			\
	third_party/python/Modules/_bz2module.c				\
	third_party/python/Modules/_codecsmodule.c			\
	third_party/python/Modules/_collectionsmodule.c			\
	third_party/python/Modules/_csv.c				\
	third_party/python/Modules/_datetimemodule.c			\
	third_party/python/Modules/_decimal/_decimal.c			\
	third_party/python/Modules/_decimal/libmpdec/basearith.c	\
	third_party/python/Modules/_decimal/libmpdec/constants.c	\
	third_party/python/Modules/_decimal/libmpdec/context.c		\
	third_party/python/Modules/_decimal/libmpdec/convolute.c	\
	third_party/python/Modules/_decimal/libmpdec/crt.c		\
	third_party/python/Modules/_decimal/libmpdec/difradix2.c	\
	third_party/python/Modules/_decimal/libmpdec/fnt.c		\
	third_party/python/Modules/_decimal/libmpdec/fourstep.c		\
	third_party/python/Modules/_decimal/libmpdec/io.c		\
	third_party/python/Modules/_decimal/libmpdec/memory.c		\
	third_party/python/Modules/_decimal/libmpdec/mpdecimal.c	\
	third_party/python/Modules/_decimal/libmpdec/numbertheory.c	\
	third_party/python/Modules/_decimal/libmpdec/sixstep.c		\
	third_party/python/Modules/_decimal/libmpdec/transpose.c	\
	third_party/python/Modules/_elementtree.c			\
	third_party/python/Modules/_functoolsmodule.c			\
	third_party/python/Modules/_heapqmodule.c			\
	third_party/python/Modules/_io/_iomodule.c			\
	third_party/python/Modules/_io/bufferedio.c			\
	third_party/python/Modules/_io/bytesio.c			\
	third_party/python/Modules/_io/fileio.c				\
	third_party/python/Modules/_io/iobase.c				\
	third_party/python/Modules/_io/stringio.c			\
	third_party/python/Modules/_io/textio.c				\
	third_party/python/Modules/_hashmbedtls.c			\
	third_party/python/Modules/_json.c				\
	third_party/python/Modules/_multiprocessing/semaphore.c 	\
	third_party/python/Modules/_multiprocessing/multiprocessing.c 	\
	third_party/python/Modules/_localemodule.c			\
	third_party/python/Modules/_lsprof.c				\
	third_party/python/Modules/_math.c				\
	third_party/python/Modules/_opcode.c				\
	third_party/python/Modules/_operator.c				\
	third_party/python/Modules/_pickle.c				\
	third_party/python/Modules/_posixsubprocess.c			\
	third_party/python/Modules/_randommodule.c			\
	third_party/python/Modules/_sqlite/cache.c			\
	third_party/python/Modules/_sqlite/connection.c			\
	third_party/python/Modules/_sqlite/cursor.c			\
	third_party/python/Modules/_sqlite/microprotocols.c		\
	third_party/python/Modules/_sqlite/module.c			\
	third_party/python/Modules/_sqlite/prepare_protocol.c		\
	third_party/python/Modules/_sqlite/row.c			\
	third_party/python/Modules/_sqlite/statement.c			\
	third_party/python/Modules/_sqlite/util.c			\
	third_party/python/Modules/_sre.c				\
        third_party/python/Modules/_lol3.c				\
	third_party/python/Modules/_stat.c				\
	third_party/python/Modules/_struct.c				\
	third_party/python/Modules/_testcapimodule.c			\
	third_party/python/Modules/_tracemalloc.c			\
	third_party/python/Modules/_weakref.c				\
	third_party/python/Modules/arraymodule.c			\
	third_party/python/Modules/atexitmodule.c			\
	third_party/python/Modules/audioop.c				\
	third_party/python/Modules/binascii.c				\
	third_party/python/Modules/cjkcodecs/xloadzd.c			\
	third_party/python/Modules/cjkcodecs/_codecs_cn.c		\
	third_party/python/Modules/cjkcodecs/_codecs_hk.c		\
	third_party/python/Modules/cjkcodecs/_codecs_iso2022.c		\
	third_party/python/Modules/cjkcodecs/_codecs_jp.c		\
	third_party/python/Modules/cjkcodecs/_codecs_kr.c		\
	third_party/python/Modules/cjkcodecs/_codecs_tw.c		\
	third_party/python/Modules/cjkcodecs/multibytecodec.c		\
	third_party/python/Modules/cjkcodecs/__big5_decmap.c		\
	third_party/python/Modules/cjkcodecs/big5_decmap.c		\
	third_party/python/Modules/cjkcodecs/__big5_encmap.c		\
	third_party/python/Modules/cjkcodecs/big5_encmap.c		\
	third_party/python/Modules/cjkcodecs/__cp950ext_decmap.c	\
	third_party/python/Modules/cjkcodecs/cp950ext_decmap.c		\
	third_party/python/Modules/cjkcodecs/__cp950ext_encmap.c	\
	third_party/python/Modules/cjkcodecs/cp950ext_encmap.c		\
	third_party/python/Modules/cjkcodecs/__big5hkscs_bmp_encmap.c	\
	third_party/python/Modules/cjkcodecs/big5hkscs_bmp_encmap.c	\
	third_party/python/Modules/cjkcodecs/__big5hkscs_nonbmp_encmap.c\
	third_party/python/Modules/cjkcodecs/big5hkscs_nonbmp_encmap.c	\
	third_party/python/Modules/cjkcodecs/__gbcommon_encmap.c	\
	third_party/python/Modules/cjkcodecs/gbcommon_encmap.c		\
	third_party/python/Modules/cjkcodecs/__gb18030ext_encmap.c	\
	third_party/python/Modules/cjkcodecs/gb18030ext_encmap.c	\
	third_party/python/Modules/cjkcodecs/jisx0213_pair_encmap.c	\
	third_party/python/Modules/cjkcodecs/__jisxcommon_encmap.c	\
	third_party/python/Modules/cjkcodecs/jisxcommon_encmap.c	\
	third_party/python/Modules/cjkcodecs/__cp932ext_encmap.c	\
	third_party/python/Modules/cjkcodecs/cp932ext_encmap.c		\
	third_party/python/Modules/cjkcodecs/__jisx0213_bmp_encmap.c	\
	third_party/python/Modules/cjkcodecs/jisx0213_bmp_encmap.c	\
	third_party/python/Modules/cjkcodecs/__jisx0213_emp_encmap.c	\
	third_party/python/Modules/cjkcodecs/jisx0213_emp_encmap.c	\
	third_party/python/Modules/cjkcodecs/__big5_encmap.c		\
	third_party/python/Modules/cjkcodecs/big5_encmap.c		\
	third_party/python/Modules/cjkcodecs/__cp950ext_encmap.c	\
	third_party/python/Modules/cjkcodecs/cp950ext_encmap.c		\
	third_party/python/Modules/cjkcodecs/cp949_encmap.c		\
	third_party/python/Modules/cjkcodecs/__cp949_encmap.c		\
	third_party/python/Modules/cjkcodecs/__ksx1001_decmap.c		\
	third_party/python/Modules/cjkcodecs/ksx1001_decmap.c		\
	third_party/python/Modules/cjkcodecs/__cp949ext_decmap.c	\
	third_party/python/Modules/cjkcodecs/cp949ext_decmap.c		\
	third_party/python/Modules/cjkcodecs/__jisx0208_decmap.c	\
	third_party/python/Modules/cjkcodecs/jisx0208_decmap.c		\
	third_party/python/Modules/cjkcodecs/__jisx0212_decmap.c	\
	third_party/python/Modules/cjkcodecs/jisx0212_decmap.c		\
	third_party/python/Modules/cjkcodecs/__jisxcommon_encmap.c	\
	third_party/python/Modules/cjkcodecs/jisxcommon_encmap.c	\
	third_party/python/Modules/cjkcodecs/__cp932ext_decmap.c	\
	third_party/python/Modules/cjkcodecs/cp932ext_decmap.c		\
	third_party/python/Modules/cjkcodecs/__cp932ext_encmap.c	\
	third_party/python/Modules/cjkcodecs/cp932ext_encmap.c		\
	third_party/python/Modules/cjkcodecs/__jisx0213_1_bmp_decmap.c	\
	third_party/python/Modules/cjkcodecs/jisx0213_1_bmp_decmap.c	\
	third_party/python/Modules/cjkcodecs/__jisx0213_2_bmp_decmap.c	\
	third_party/python/Modules/cjkcodecs/jisx0213_2_bmp_decmap.c	\
	third_party/python/Modules/cjkcodecs/__jisx0213_bmp_encmap.c	\
	third_party/python/Modules/cjkcodecs/jisx0213_bmp_encmap.c	\
	third_party/python/Modules/cjkcodecs/__jisx0213_1_emp_decmap.c	\
	third_party/python/Modules/cjkcodecs/jisx0213_1_emp_decmap.c	\
	third_party/python/Modules/cjkcodecs/__jisx0213_2_emp_decmap.c	\
	third_party/python/Modules/cjkcodecs/jisx0213_2_emp_decmap.c	\
	third_party/python/Modules/cjkcodecs/__jisx0213_emp_encmap.c	\
	third_party/python/Modules/cjkcodecs/jisx0213_emp_encmap.c	\
	third_party/python/Modules/cjkcodecs/__jisx0213_pair_decmap.c	\
	third_party/python/Modules/cjkcodecs/jisx0213_pair_decmap.c	\
	third_party/python/Modules/cjkcodecs/jisx0213_pair_encmap.c	\
	third_party/python/Modules/cjkcodecs/__gb2312_decmap.c		\
	third_party/python/Modules/cjkcodecs/gb2312_decmap.c		\
	third_party/python/Modules/cjkcodecs/__gbkext_decmap.c		\
	third_party/python/Modules/cjkcodecs/gbkext_decmap.c		\
	third_party/python/Modules/cjkcodecs/__gbcommon_encmap.c	\
	third_party/python/Modules/cjkcodecs/gbcommon_encmap.c		\
	third_party/python/Modules/cjkcodecs/__gb18030ext_decmap.c	\
	third_party/python/Modules/cjkcodecs/gb18030ext_decmap.c	\
	third_party/python/Modules/cjkcodecs/__gb18030ext_encmap.c	\
	third_party/python/Modules/cjkcodecs/gb18030ext_encmap.c	\
	third_party/python/Modules/cjkcodecs/__big5hkscs_decmap.c	\
	third_party/python/Modules/cjkcodecs/big5hkscs_decmap.c		\
	third_party/python/Modules/cmathmodule.c			\
	third_party/python/Modules/config.c				\
	third_party/python/Modules/errnomodule.c			\
	third_party/python/Modules/expat/xmlparse.c			\
	third_party/python/Modules/expat/xmlrole.c			\
	third_party/python/Modules/expat/xmltok.c			\
	third_party/python/Modules/faulthandler.c			\
	third_party/python/Modules/fcntlmodule.c			\
	third_party/python/Modules/fpectlmodule.c			\
	third_party/python/Modules/fspath.c				\
	third_party/python/Modules/gcmodule.c				\
	third_party/python/Modules/getbuildinfo.c			\
	third_party/python/Modules/getpath.c				\
	third_party/python/Modules/grpmodule.c				\
	third_party/python/Modules/hashtable.c				\
	third_party/python/Modules/itertoolsmodule.c			\
	third_party/python/Modules/main.c				\
	third_party/python/Modules/mathmodule.c				\
	third_party/python/Modules/mmapmodule.c				\
	third_party/python/Modules/parsermodule.c			\
	third_party/python/Modules/posixmodule.c			\
	third_party/python/Modules/tokenbucket.c			\
	third_party/python/Modules/pwdmodule.c				\
	third_party/python/Modules/pyexpat.c				\
	third_party/python/Modules/resource.c				\
	third_party/python/Modules/rotatingtree.c			\
	third_party/python/Modules/selectmodule.c			\
	third_party/python/Modules/unicodedata.c			\
	third_party/python/Modules/signalmodule.c			\
	third_party/python/Modules/socketmodule.c			\
	third_party/python/Modules/tlsmodule.c				\
	third_party/python/Modules/symtablemodule.c			\
	third_party/python/Modules/syslogmodule.c			\
	third_party/python/Modules/syslogmodule.c			\
	third_party/python/Modules/termios.c				\
	third_party/python/Modules/timemodule.c				\
	third_party/python/Modules/zipimport.c				\
	third_party/python/Modules/zlibmodule.c				\
        third_party/python/Objects/accu.c				\
        third_party/python/Objects/weakrefobject.c			\
        third_party/python/Parser/bitset.c				\
        third_party/python/Parser/firstsets.c				\
        third_party/python/Parser/grammar.c				\
        third_party/python/Parser/metagrammar.c				\
        third_party/python/Parser/pgen.c				\
        third_party/python/Python/dynamic_annotations.c			\
	third_party/python/Python/recursive.c				\
        third_party/python/Python/frozen.c				\
        third_party/python/Python/frozenmain.c				\
        third_party/python/Python/getopt.c				\
        third_party/python/Python/pyfpe.c				\
        third_party/python/Python/sigcheck.c

THIRD_PARTY_PYTHON_STAGE2_A_PYS =						\
	third_party/python/Parser/asdl.py					\
	third_party/python/Parser/asdl_c.py					\
	third_party/python/Lib/__future__.py					\
	third_party/python/Lib/_bootlocale.py					\
	third_party/python/Lib/_collections_abc.py				\
	third_party/python/Lib/_compat_pickle.py				\
	third_party/python/Lib/_compression.py					\
	third_party/python/Lib/_dummy_thread.py					\
	third_party/python/Lib/_markupbase.py					\
	third_party/python/Lib/_pydecimal.py					\
	third_party/python/Lib/_pyio.py						\
	third_party/python/Lib/_sitebuiltins.py					\
	third_party/python/Lib/_strptime.py					\
	third_party/python/Lib/_sysconfigdata_m_cosmo_x86_64_cosmo.py		\
	third_party/python/Lib/_threading_local.py				\
	third_party/python/Lib/_weakrefset.py					\
	third_party/python/Lib/asyncio/base_events.py	\
	third_party/python/Lib/asyncio/base_futures.py	\
	third_party/python/Lib/asyncio/base_subprocess.py	\
	third_party/python/Lib/asyncio/base_tasks.py	\
	third_party/python/Lib/asyncio/compat.py	\
	third_party/python/Lib/asyncio/constants.py	\
	third_party/python/Lib/asyncio/coroutines.py	\
	third_party/python/Lib/asyncio/events.py	\
	third_party/python/Lib/asyncio/futures.py	\
	third_party/python/Lib/asyncio/__init__.py	\
	third_party/python/Lib/asyncio/locks.py	\
	third_party/python/Lib/asyncio/log.py	\
	third_party/python/Lib/asyncio/proactor_events.py	\
	third_party/python/Lib/asyncio/protocols.py	\
	third_party/python/Lib/asyncio/queues.py	\
	third_party/python/Lib/asyncio/selector_events.py	\
	third_party/python/Lib/asyncio/sslproto.py	\
	third_party/python/Lib/asyncio/streams.py	\
	third_party/python/Lib/asyncio/subprocess.py	\
	third_party/python/Lib/asyncio/tasks.py	\
	third_party/python/Lib/asyncio/transports.py	\
	third_party/python/Lib/asyncio/unix_events.py	\
	third_party/python/Lib/abc.py						\
	third_party/python/Lib/aifc.py						\
	third_party/python/Lib/antigravity.py					\
	third_party/python/Lib/argparse.py					\
	third_party/python/Lib/ast.py						\
	third_party/python/Lib/asynchat.py					\
	third_party/python/Lib/asyncore.py					\
	third_party/python/Lib/base64.py					\
	third_party/python/Lib/bdb.py						\
	third_party/python/Lib/binhex.py					\
	third_party/python/Lib/bisect.py					\
	third_party/python/Lib/bz2.py						\
	third_party/python/Lib/cProfile.py					\
	third_party/python/Lib/calendar.py					\
	third_party/python/Lib/cgi.py						\
	third_party/python/Lib/cgitb.py						\
	third_party/python/Lib/chunk.py						\
	third_party/python/Lib/cmd.py						\
	third_party/python/Lib/code.py						\
	third_party/python/Lib/codecs.py					\
	third_party/python/Lib/codeop.py					\
	third_party/python/Lib/collections/__init__.py				\
	third_party/python/Lib/collections/abc.py				\
	third_party/python/Lib/colorsys.py					\
	third_party/python/Lib/compileall.py					\
	third_party/python/Lib/configparser.py					\
	third_party/python/Lib/contextlib.py					\
	third_party/python/Lib/copy.py						\
	third_party/python/Lib/copyreg.py					\
	third_party/python/Lib/csv.py						\
	third_party/python/Lib/datetime.py					\
	third_party/python/Lib/decimal.py					\
	third_party/python/Lib/difflib.py					\
	third_party/python/Lib/dis.py						\
	third_party/python/Lib/dummy_threading.py				\
	third_party/python/Lib/concurrent/__init__.py			\
	third_party/python/Lib/concurrent/futures/__init__.py			\
	third_party/python/Lib/concurrent/futures/process.py			\
	third_party/python/Lib/concurrent/futures/thread.py			\
	third_party/python/Lib/concurrent/futures/_base.py			\
	third_party/python/Lib/email/__init__.py				\
	third_party/python/Lib/email/_encoded_words.py				\
	third_party/python/Lib/email/_header_value_parser.py			\
	third_party/python/Lib/email/_parseaddr.py				\
	third_party/python/Lib/email/_policybase.py				\
	third_party/python/Lib/email/base64mime.py				\
	third_party/python/Lib/email/charset.py					\
	third_party/python/Lib/email/contentmanager.py				\
	third_party/python/Lib/email/encoders.py				\
	third_party/python/Lib/email/errors.py					\
	third_party/python/Lib/email/feedparser.py				\
	third_party/python/Lib/email/generator.py				\
	third_party/python/Lib/email/header.py					\
	third_party/python/Lib/email/headerregistry.py				\
	third_party/python/Lib/email/iterators.py				\
	third_party/python/Lib/email/message.py					\
	third_party/python/Lib/email/mime/__init__.py				\
	third_party/python/Lib/email/mime/application.py			\
	third_party/python/Lib/email/mime/audio.py				\
	third_party/python/Lib/email/mime/base.py				\
	third_party/python/Lib/email/mime/image.py				\
	third_party/python/Lib/email/mime/message.py				\
	third_party/python/Lib/email/mime/multipart.py				\
	third_party/python/Lib/email/mime/nonmultipart.py			\
	third_party/python/Lib/email/mime/text.py				\
	third_party/python/Lib/email/parser.py					\
	third_party/python/Lib/email/policy.py					\
	third_party/python/Lib/email/quoprimime.py				\
	third_party/python/Lib/email/utils.py					\
	third_party/python/Lib/encodings/__init__.py				\
	third_party/python/Lib/encodings/bz2_codec.py				\
	third_party/python/Lib/encodings/aliases.py				\
	third_party/python/Lib/encodings/ascii.py				\
	third_party/python/Lib/encodings/base64_codec.py			\
	third_party/python/Lib/encodings/big5.py				\
	third_party/python/Lib/encodings/big5hkscs.py				\
	third_party/python/Lib/encodings/charmap.py				\
	third_party/python/Lib/encodings/cp037.py				\
	third_party/python/Lib/encodings/cp1006.py				\
	third_party/python/Lib/encodings/cp1026.py				\
	third_party/python/Lib/encodings/cp1125.py				\
	third_party/python/Lib/encodings/cp1140.py				\
	third_party/python/Lib/encodings/cp1250.py				\
	third_party/python/Lib/encodings/cp1251.py				\
	third_party/python/Lib/encodings/cp1252.py				\
	third_party/python/Lib/encodings/cp1253.py				\
	third_party/python/Lib/encodings/cp1254.py				\
	third_party/python/Lib/encodings/cp1255.py				\
	third_party/python/Lib/encodings/cp1256.py				\
	third_party/python/Lib/encodings/cp1257.py				\
	third_party/python/Lib/encodings/cp1258.py				\
	third_party/python/Lib/encodings/cp273.py				\
	third_party/python/Lib/encodings/cp424.py				\
	third_party/python/Lib/encodings/cp437.py				\
	third_party/python/Lib/encodings/cp500.py				\
	third_party/python/Lib/encodings/cp65001.py				\
	third_party/python/Lib/encodings/cp720.py				\
	third_party/python/Lib/encodings/cp737.py				\
	third_party/python/Lib/encodings/cp775.py				\
	third_party/python/Lib/encodings/cp850.py				\
	third_party/python/Lib/encodings/cp852.py				\
	third_party/python/Lib/encodings/cp855.py				\
	third_party/python/Lib/encodings/cp856.py				\
	third_party/python/Lib/encodings/cp857.py				\
	third_party/python/Lib/encodings/cp858.py				\
	third_party/python/Lib/encodings/cp860.py				\
	third_party/python/Lib/encodings/cp861.py				\
	third_party/python/Lib/encodings/cp862.py				\
	third_party/python/Lib/encodings/cp863.py				\
	third_party/python/Lib/encodings/cp864.py				\
	third_party/python/Lib/encodings/cp865.py				\
	third_party/python/Lib/encodings/cp866.py				\
	third_party/python/Lib/encodings/cp869.py				\
	third_party/python/Lib/encodings/cp874.py				\
	third_party/python/Lib/encodings/cp875.py				\
	third_party/python/Lib/encodings/cp932.py				\
	third_party/python/Lib/encodings/cp949.py				\
	third_party/python/Lib/encodings/cp950.py				\
	third_party/python/Lib/encodings/euc_jis_2004.py			\
	third_party/python/Lib/encodings/euc_jisx0213.py			\
	third_party/python/Lib/encodings/euc_jp.py				\
	third_party/python/Lib/encodings/euc_kr.py				\
	third_party/python/Lib/encodings/gb18030.py				\
	third_party/python/Lib/encodings/gb2312.py				\
	third_party/python/Lib/encodings/gbk.py					\
	third_party/python/Lib/encodings/hex_codec.py				\
	third_party/python/Lib/encodings/hp_roman8.py				\
	third_party/python/Lib/encodings/hz.py					\
	third_party/python/Lib/encodings/idna.py				\
	third_party/python/Lib/encodings/iso2022_jp.py				\
	third_party/python/Lib/encodings/iso2022_jp_1.py			\
	third_party/python/Lib/encodings/iso2022_jp_2.py			\
	third_party/python/Lib/encodings/iso2022_jp_2004.py			\
	third_party/python/Lib/encodings/iso2022_jp_3.py			\
	third_party/python/Lib/encodings/iso2022_jp_ext.py			\
	third_party/python/Lib/encodings/iso2022_kr.py				\
	third_party/python/Lib/encodings/iso8859_1.py				\
	third_party/python/Lib/encodings/iso8859_10.py				\
	third_party/python/Lib/encodings/iso8859_11.py				\
	third_party/python/Lib/encodings/iso8859_13.py				\
	third_party/python/Lib/encodings/iso8859_14.py				\
	third_party/python/Lib/encodings/iso8859_15.py				\
	third_party/python/Lib/encodings/iso8859_16.py				\
	third_party/python/Lib/encodings/iso8859_2.py				\
	third_party/python/Lib/encodings/iso8859_3.py				\
	third_party/python/Lib/encodings/iso8859_4.py				\
	third_party/python/Lib/encodings/iso8859_5.py				\
	third_party/python/Lib/encodings/iso8859_6.py				\
	third_party/python/Lib/encodings/iso8859_7.py				\
	third_party/python/Lib/encodings/iso8859_8.py				\
	third_party/python/Lib/encodings/iso8859_9.py				\
	third_party/python/Lib/encodings/johab.py				\
	third_party/python/Lib/encodings/koi8_r.py				\
	third_party/python/Lib/encodings/koi8_t.py				\
	third_party/python/Lib/encodings/koi8_u.py				\
	third_party/python/Lib/encodings/kz1048.py				\
	third_party/python/Lib/encodings/latin_1.py				\
	third_party/python/Lib/encodings/mac_arabic.py				\
	third_party/python/Lib/encodings/mac_centeuro.py			\
	third_party/python/Lib/encodings/mac_croatian.py			\
	third_party/python/Lib/encodings/mac_cyrillic.py			\
	third_party/python/Lib/encodings/mac_farsi.py				\
	third_party/python/Lib/encodings/mac_greek.py				\
	third_party/python/Lib/encodings/mac_iceland.py				\
	third_party/python/Lib/encodings/mac_latin2.py				\
	third_party/python/Lib/encodings/mac_roman.py				\
	third_party/python/Lib/encodings/mac_romanian.py			\
	third_party/python/Lib/encodings/mac_turkish.py				\
	third_party/python/Lib/encodings/palmos.py				\
	third_party/python/Lib/encodings/ptcp154.py				\
	third_party/python/Lib/encodings/punycode.py				\
	third_party/python/Lib/encodings/quopri_codec.py			\
	third_party/python/Lib/encodings/raw_unicode_escape.py			\
	third_party/python/Lib/encodings/rot_13.py				\
	third_party/python/Lib/encodings/shift_jis.py				\
	third_party/python/Lib/encodings/shift_jis_2004.py			\
	third_party/python/Lib/encodings/shift_jisx0213.py			\
	third_party/python/Lib/encodings/tis_620.py				\
	third_party/python/Lib/encodings/undefined.py				\
	third_party/python/Lib/encodings/unicode_escape.py			\
	third_party/python/Lib/encodings/unicode_internal.py			\
	third_party/python/Lib/encodings/utf_16.py				\
	third_party/python/Lib/encodings/utf_16_be.py				\
	third_party/python/Lib/encodings/utf_16_le.py				\
	third_party/python/Lib/encodings/utf_32.py				\
	third_party/python/Lib/encodings/utf_32_be.py				\
	third_party/python/Lib/encodings/utf_32_le.py				\
	third_party/python/Lib/encodings/utf_7.py				\
	third_party/python/Lib/encodings/utf_8.py				\
	third_party/python/Lib/encodings/utf_8_sig.py				\
	third_party/python/Lib/encodings/uu_codec.py				\
	third_party/python/Lib/encodings/zlib_codec.py				\
	third_party/python/Lib/enum.py						\
	third_party/python/Lib/filecmp.py					\
	third_party/python/Lib/fileinput.py					\
	third_party/python/Lib/fnmatch.py					\
	third_party/python/Lib/formatter.py					\
	third_party/python/Lib/fractions.py					\
	third_party/python/Lib/ftplib.py					\
	third_party/python/Lib/functools.py					\
	third_party/python/Lib/genericpath.py					\
	third_party/python/Lib/getopt.py					\
	third_party/python/Lib/getpass.py					\
	third_party/python/Lib/gettext.py					\
	third_party/python/Lib/glob.py						\
	third_party/python/Lib/gzip.py						\
	third_party/python/Lib/hashlib.py					\
	third_party/python/Lib/heapq.py						\
	third_party/python/Lib/hmac.py						\
	third_party/python/Lib/html/__init__.py					\
	third_party/python/Lib/html/entities.py					\
	third_party/python/Lib/html/parser.py					\
	third_party/python/Lib/http/__init__.py					\
	third_party/python/Lib/http/client.py					\
	third_party/python/Lib/http/cookiejar.py				\
	third_party/python/Lib/http/cookies.py					\
	third_party/python/Lib/http/server.py					\
	third_party/python/Lib/imaplib.py					\
	third_party/python/Lib/imghdr.py					\
	third_party/python/Lib/imp.py						\
	third_party/python/Lib/importlib/__init__.py				\
	third_party/python/Lib/importlib/abc.py					\
	third_party/python/Lib/importlib/machinery.py				\
	third_party/python/Lib/importlib/util.py				\
	third_party/python/Lib/inspect.py					\
	third_party/python/Lib/io.py						\
	third_party/python/Lib/ipaddress.py					\
	third_party/python/Lib/json/__init__.py					\
	third_party/python/Lib/json/decoder.py					\
	third_party/python/Lib/json/encoder.py					\
	third_party/python/Lib/json/scanner.py					\
	third_party/python/Lib/json/tool.py					\
	third_party/python/Lib/keyword.py					\
	third_party/python/Lib/launchpy.py					\
	third_party/python/Lib/linecache.py					\
	third_party/python/Lib/locale.py					\
	third_party/python/Lib/logging/__init__.py				\
	third_party/python/Lib/logging/config.py				\
	third_party/python/Lib/logging/handlers.py				\
	third_party/python/Lib/macpath.py					\
	third_party/python/Lib/macurl2path.py					\
	third_party/python/Lib/mailbox.py					\
	third_party/python/Lib/mailcap.py					\
	third_party/python/Lib/mimetypes.py					\
	third_party/python/Lib/modulefinder.py					\
	third_party/python/Lib/multiprocessing/__init__.py			\
	third_party/python/Lib/multiprocessing/connection.py			\
	third_party/python/Lib/multiprocessing/context.py			\
	third_party/python/Lib/multiprocessing/dummy/__init__.py		\
	third_party/python/Lib/multiprocessing/dummy/connection.py		\
	third_party/python/Lib/multiprocessing/forkserver.py			\
	third_party/python/Lib/multiprocessing/heap.py				\
	third_party/python/Lib/multiprocessing/managers.py			\
	third_party/python/Lib/multiprocessing/pool.py				\
	third_party/python/Lib/multiprocessing/popen_fork.py			\
	third_party/python/Lib/multiprocessing/popen_forkserver.py		\
	third_party/python/Lib/multiprocessing/popen_spawn_posix.py		\
	third_party/python/Lib/multiprocessing/popen_spawn_win32.py		\
	third_party/python/Lib/multiprocessing/process.py			\
	third_party/python/Lib/multiprocessing/queues.py			\
	third_party/python/Lib/multiprocessing/reduction.py			\
	third_party/python/Lib/multiprocessing/resource_sharer.py		\
	third_party/python/Lib/multiprocessing/semaphore_tracker.py		\
	third_party/python/Lib/multiprocessing/spawn.py				\
	third_party/python/Lib/multiprocessing/synchronize.py			\
	third_party/python/Lib/multiprocessing/util.py				\
	third_party/python/Lib/netrc.py						\
	third_party/python/Lib/nntplib.py					\
	third_party/python/Lib/ntpath.py					\
	third_party/python/Lib/nturl2path.py					\
	third_party/python/Lib/numbers.py					\
	third_party/python/Lib/opcode.py					\
	third_party/python/Lib/operator.py					\
	third_party/python/Lib/optparse.py					\
	third_party/python/Lib/os.py						\
	third_party/python/Lib/pathlib.py					\
	third_party/python/Lib/pdb.py						\
	third_party/python/Lib/pickle.py					\
	third_party/python/Lib/pickletools.py					\
	third_party/python/Lib/pipes.py						\
	third_party/python/Lib/pkgutil.py					\
	third_party/python/Lib/platform.py					\
	third_party/python/Lib/plistlib.py					\
	third_party/python/Lib/poplib.py					\
	third_party/python/Lib/posixpath.py					\
	third_party/python/Lib/pprint.py					\
	third_party/python/Lib/profile.py					\
	third_party/python/Lib/pstats.py					\
	third_party/python/Lib/pty.py						\
	third_party/python/Lib/py_compile.py					\
	third_party/python/Lib/pyclbr.py					\
	third_party/python/Lib/pydoc.py						\
	third_party/python/Lib/pydoc_data/__init__.py				\
	third_party/python/Lib/pydoc_data/topics.py				\
	third_party/python/Lib/pydoc.py						\
	third_party/python/Lib/queue.py						\
	third_party/python/Lib/quopri.py					\
	third_party/python/Lib/random.py					\
	third_party/python/Lib/re.py						\
	third_party/python/Lib/reprlib.py					\
	third_party/python/Lib/runpy.py						\
	third_party/python/Lib/sched.py						\
	third_party/python/Lib/secrets.py					\
	third_party/python/Lib/selectors.py					\
	third_party/python/Lib/shelve.py					\
	third_party/python/Lib/shlex.py						\
	third_party/python/Lib/shutil.py					\
	third_party/python/Lib/signal.py					\
	third_party/python/Lib/site.py						\
	third_party/python/Lib/smtpd.py						\
	third_party/python/Lib/smtplib.py					\
	third_party/python/Lib/sndhdr.py					\
	third_party/python/Lib/socket.py					\
	third_party/python/Lib/socketserver.py					\
	third_party/python/Lib/sre_compile.py					\
	third_party/python/Lib/sre_constants.py					\
	third_party/python/Lib/sre_parse.py					\
	third_party/python/Lib/stat.py						\
	third_party/python/Lib/statistics.py					\
	third_party/python/Lib/string.py					\
	third_party/python/Lib/stringprep.py					\
	third_party/python/Lib/struct.py					\
	third_party/python/Lib/subprocess.py					\
	third_party/python/Lib/sunau.py						\
	third_party/python/Lib/symbol.py					\
	third_party/python/Lib/symtable.py					\
	third_party/python/Lib/sysconfig.py					\
	third_party/python/Lib/tabnanny.py					\
	third_party/python/Lib/tarfile.py					\
	third_party/python/Lib/telnetlib.py					\
	third_party/python/Lib/tempfile.py					\
	third_party/python/Lib/textwrap.py					\
	third_party/python/Lib/this.py						\
	third_party/python/Lib/threading.py					\
	third_party/python/Lib/timeit.py					\
	third_party/python/Lib/token.py						\
	third_party/python/Lib/tokenize.py					\
	third_party/python/Lib/trace.py						\
	third_party/python/Lib/traceback.py					\
	third_party/python/Lib/tracemalloc.py					\
	third_party/python/Lib/tty.py						\
	third_party/python/Lib/types.py						\
	third_party/python/Lib/typing.py					\
	third_party/python/Lib/urllib/__init__.py				\
	third_party/python/Lib/urllib/error.py					\
	third_party/python/Lib/urllib/parse.py					\
	third_party/python/Lib/urllib/request.py				\
	third_party/python/Lib/urllib/response.py				\
	third_party/python/Lib/urllib/robotparser.py				\
	third_party/python/Lib/uu.py						\
	third_party/python/Lib/uuid.py						\
	third_party/python/Lib/venv/__init__.py					\
	third_party/python/Lib/venv/__main__.py					\
	third_party/python/Lib/warnings.py					\
	third_party/python/Lib/wave.py						\
	third_party/python/Lib/weakref.py					\
	third_party/python/Lib/webbrowser.py					\
	third_party/python/Lib/wsgiref/__init__.py				\
	third_party/python/Lib/wsgiref/handlers.py				\
	third_party/python/Lib/wsgiref/headers.py				\
	third_party/python/Lib/wsgiref/simple_server.py				\
	third_party/python/Lib/wsgiref/util.py					\
	third_party/python/Lib/wsgiref/validate.py				\
	third_party/python/Lib/xdrlib.py					\
	third_party/python/Lib/xml/__init__.py					\
	third_party/python/Lib/xml/dom/NodeFilter.py				\
	third_party/python/Lib/xml/dom/__init__.py				\
	third_party/python/Lib/xml/dom/domreg.py				\
	third_party/python/Lib/xml/dom/expatbuilder.py				\
	third_party/python/Lib/xml/dom/minicompat.py				\
	third_party/python/Lib/xml/dom/minidom.py				\
	third_party/python/Lib/xml/dom/pulldom.py				\
	third_party/python/Lib/xml/dom/xmlbuilder.py				\
	third_party/python/Lib/xml/etree/ElementInclude.py			\
	third_party/python/Lib/xml/etree/ElementPath.py				\
	third_party/python/Lib/xml/etree/ElementTree.py				\
	third_party/python/Lib/xml/etree/__init__.py				\
	third_party/python/Lib/xml/etree/cElementTree.py			\
	third_party/python/Lib/xml/parsers/__init__.py				\
	third_party/python/Lib/xml/parsers/expat.py				\
	third_party/python/Lib/xml/sax/__init__.py				\
	third_party/python/Lib/xml/sax/_exceptions.py				\
	third_party/python/Lib/xml/sax/expatreader.py				\
	third_party/python/Lib/xml/sax/handler.py				\
	third_party/python/Lib/xml/sax/saxutils.py				\
	third_party/python/Lib/xml/sax/xmlreader.py				\
	third_party/python/Lib/xmlrpc/__init__.py				\
	third_party/python/Lib/xmlrpc/client.py					\
	third_party/python/Lib/xmlrpc/server.py					\
	third_party/python/Lib/zipapp.py					\
	third_party/python/Lib/sqlite3/__init__.py				\
	third_party/python/Lib/sqlite3/dump.py					\
	third_party/python/Lib/sqlite3/dbapi2.py				\
	third_party/python/Lib/zipfile.py

THIRD_PARTY_PYTHON_STAGE2_A_DATA =						\
	third_party/python/Lib/mime.types					\
	third_party/python/Lib/pydoc_data/					\
	third_party/python/Lib/pydoc_data/_pydoc.css

THIRD_PARTY_PYTHON_STAGE2_A_DIRECTDEPS =				\
	DSP_CORE							\
	LIBC_CALLS							\
	LIBC_FMT							\
	LIBC_INTRIN							\
	LIBC_LOG							\
	LIBC_MEM							\
	LIBC_NEXGEN32E							\
	LIBC_NT_KERNEL32						\
	LIBC_PROC							\
	LIBC_RUNTIME							\
	LIBC_THREAD							\
	LIBC_SOCK							\
	LIBC_STDIO							\
	LIBC_STR							\
	LIBC_SYSV							\
	LIBC_SYSV_CALLS							\
	LIBC_TIME							\
	LIBC_TINYMATH							\
	LIBC_X								\
	NET_HTTP							\
	NET_HTTPS							\
	THIRD_PARTY_BZIP2						\
	THIRD_PARTY_GDTOA						\
	THIRD_PARTY_LINENOISE						\
	THIRD_PARTY_MUSL						\
	THIRD_PARTY_PYTHON_STAGE1					\
	THIRD_PARTY_MBEDTLS						\
	THIRD_PARTY_SQLITE3						\
	THIRD_PARTY_ZLIB						\
	THIRD_PARTY_XED							\
	TOOL_ARGS

THIRD_PARTY_PYTHON_STAGE2_A_DEPS =					\
	$(call uniq,$(foreach x,$(THIRD_PARTY_PYTHON_STAGE2_A_DIRECTDEPS),$($(x))))

o/$(MODE)/third_party/python/Python/frozen.o:				\
		third_party/python/Python/frozen.c			\
		o//third_party/python/Python/importlib.inc		\
		o//third_party/python/Python/importlib_external.inc

################################################################################
# TESTS

THIRD_PARTY_PYTHON_PYTEST =						\
	$(THIRD_PARTY_PYTHON_PYTEST_A_DEPS)				\
	$(THIRD_PARTY_PYTHON_PYTEST_A)

THIRD_PARTY_PYTHON_PYTEST_A =						\
	o/$(MODE)/third_party/python/python-pytest.a

THIRD_PARTY_PYTHON_PYTEST_A_OBJS =					\
	$(THIRD_PARTY_PYTHON_PYTEST_A_PYS_OBJS)				\
	$(THIRD_PARTY_PYTHON_PYTEST_A_DATA_OBJS)

THIRD_PARTY_PYTHON_PYTEST_A_PYS_OBJS = $(THIRD_PARTY_PYTHON_PYTEST_A_PYS:%.py=o/$(MODE)/%.o)
THIRD_PARTY_PYTHON_PYTEST_A_DATA_OBJS = $(THIRD_PARTY_PYTHON_PYTEST_A_DATA:%=o/$(MODE)/%.zip.o)
THIRD_PARTY_PYTHON_PYTEST_PYMAINS_OBJS = $(THIRD_PARTY_PYTHON_PYTEST_PYMAINS:%.py=o/$(MODE)/%.o)

THIRD_PARTY_PYTHON_PYTEST_A_PYS =						\
	third_party/python/Lib/doctest.py					\
	third_party/python/Lib/sqlite3/test/__init__.py				\
	third_party/python/Lib/sqlite3/test/__init__.py				\
	third_party/python/Lib/sqlite3/test/dbapi.py				\
	third_party/python/Lib/sqlite3/test/dump.py				\
	third_party/python/Lib/sqlite3/test/factory.py				\
	third_party/python/Lib/sqlite3/test/hooks.py				\
	third_party/python/Lib/sqlite3/test/regression.py			\
	third_party/python/Lib/sqlite3/test/transactions.py			\
	third_party/python/Lib/sqlite3/test/types.py				\
	third_party/python/Lib/sqlite3/test/userfunctions.py			\
	third_party/python/Lib/test/__init__.py					\
	third_party/python/Lib/test/ann_module.py				\
	third_party/python/Lib/test/ann_module2.py				\
	third_party/python/Lib/test/ann_module3.py				\
	third_party/python/Lib/test/audiotests.py				\
	third_party/python/Lib/test/autotest.py					\
	third_party/python/Lib/test/bytecode_helper.py				\
	third_party/python/Lib/test/coding20731.py				\
	third_party/python/Lib/test/datetimetester.py				\
	third_party/python/Lib/test/dis_module.py				\
	third_party/python/Lib/test/doctest_aliases.py				\
	third_party/python/Lib/test/double_const.py				\
	third_party/python/Lib/test/final_a.py					\
	third_party/python/Lib/test/final_b.py					\
	third_party/python/Lib/test/fork_wait.py				\
	third_party/python/Lib/test/future_test1.py				\
	third_party/python/Lib/test/future_test2.py				\
	third_party/python/Lib/test/gdb_sample.py				\
	third_party/python/Lib/test/imp_dummy.py				\
	third_party/python/Lib/test/inspect_fodder.py				\
	third_party/python/Lib/test/inspect_fodder2.py				\
	third_party/python/Lib/test/leakers/__init__.py				\
	third_party/python/Lib/test/leakers/test_selftype.py			\
	third_party/python/Lib/test/libregrtest/__init__.py			\
	third_party/python/Lib/test/libregrtest/cmdline.py			\
	third_party/python/Lib/test/libregrtest/main.py				\
	third_party/python/Lib/test/libregrtest/refleak.py			\
	third_party/python/Lib/test/libregrtest/runtest.py			\
	third_party/python/Lib/test/libregrtest/runtest_mp.py			\
	third_party/python/Lib/test/libregrtest/save_env.py			\
	third_party/python/Lib/test/libregrtest/setup.py			\
	third_party/python/Lib/test/libregrtest/utils.py			\
	third_party/python/Lib/test/list_tests.py				\
	third_party/python/Lib/test/mapping_tests.py				\
	third_party/python/Lib/test/memory_watchdog.py				\
	third_party/python/Lib/test/mock_socket.py				\
	third_party/python/Lib/test/mod_generics_cache.py			\
	third_party/python/Lib/test/multibytecodec_support.py			\
	third_party/python/Lib/test/pickletester.py				\
	third_party/python/Lib/test/profilee.py					\
	third_party/python/Lib/test/pyclbr_input.py				\
	third_party/python/Lib/test/pydoc_mod.py				\
	third_party/python/Lib/test/pydocfodder.py				\
	third_party/python/Lib/test/re_tests.py					\
	third_party/python/Lib/test/lock_tests.py				\
	third_party/python/Lib/test/reperf.py					\
	third_party/python/Lib/test/sample_doctest.py				\
	third_party/python/Lib/test/sample_doctest_no_docstrings.py		\
	third_party/python/Lib/test/sample_doctest_no_doctests.py		\
	third_party/python/Lib/test/seq_tests.py				\
	third_party/python/Lib/test/string_tests.py				\
	third_party/python/Lib/test/support/__init__.py				\
	third_party/python/Lib/test/support/script_helper.py			\
	third_party/python/Lib/test/support/testresult.py			\
	third_party/python/Lib/test/test_dummy_thread.py			\
	third_party/python/Lib/test/test_email/__init__.py			\
	third_party/python/Lib/test/test_httpservers.py				\
	third_party/python/Lib/test/test_json/__init__.py			\
	third_party/python/Lib/test/test_math.py				\
	third_party/python/Lib/test/test_warnings/__init__.py			\
	third_party/python/Lib/test/test_warnings/data/__init__.py		\
	third_party/python/Lib/test/test_warnings/data/import_warning.py	\
	third_party/python/Lib/test/test_warnings/data/stacklevel.py		\
	third_party/python/Lib/test/test_xml_etree.py				\
	third_party/python/Lib/test/testcodec.py				\
	third_party/python/Lib/test/tf_inherit_check.py				\
	third_party/python/Lib/test/threaded_import_hangers.py			\
	third_party/python/Lib/test/tracedmodules/__init__.py			\
	third_party/python/Lib/test/tracedmodules/testmod.py			\
	third_party/python/Lib/test/xmltests.py					\
	third_party/python/Lib/unittest/__init__.py				\
	third_party/python/Lib/unittest/__main__.py				\
	third_party/python/Lib/unittest/_main.py				\
	third_party/python/Lib/unittest/case.py					\
	third_party/python/Lib/unittest/loader.py				\
	third_party/python/Lib/unittest/mock.py					\
	third_party/python/Lib/unittest/result.py				\
	third_party/python/Lib/unittest/runner.py				\
	third_party/python/Lib/unittest/signals.py				\
	third_party/python/Lib/unittest/suite.py				\
	third_party/python/Lib/unittest/test/__init__.py			\
	third_party/python/Lib/unittest/test/_test_warnings.py			\
	third_party/python/Lib/unittest/test/dummy.py				\
	third_party/python/Lib/unittest/test/support.py				\
	third_party/python/Lib/unittest/test/test_assertions.py			\
	third_party/python/Lib/unittest/test/test_break.py			\
	third_party/python/Lib/unittest/test/test_case.py			\
	third_party/python/Lib/unittest/test/test_functiontestcase.py		\
	third_party/python/Lib/unittest/test/test_loader.py			\
	third_party/python/Lib/unittest/test/test_program.py			\
	third_party/python/Lib/unittest/test/test_result.py			\
	third_party/python/Lib/unittest/test/test_runner.py			\
	third_party/python/Lib/unittest/test/test_setups.py			\
	third_party/python/Lib/unittest/test/test_skipping.py			\
	third_party/python/Lib/unittest/test/test_suite.py			\
	third_party/python/Lib/unittest/test/testmock/__init__.py		\
	third_party/python/Lib/unittest/test/testmock/support.py		\
	third_party/python/Lib/unittest/test/testmock/testcallable.py		\
	third_party/python/Lib/unittest/test/testmock/testhelpers.py		\
	third_party/python/Lib/unittest/test/testmock/testmagicmethods.py	\
	third_party/python/Lib/unittest/test/testmock/testmock.py		\
	third_party/python/Lib/unittest/test/testmock/testsentinel.py		\
	third_party/python/Lib/unittest/test/testmock/testwith.py		\
	third_party/python/Lib/unittest/util.py

THIRD_PARTY_PYTHON_PYTEST_A_DATA =										\
	third_party/python/Lib/test/Python.asdl	\
	third_party/python/Lib/email/architecture.rst								\
	third_party/python/Lib/venv/scripts/common/activate							\
	third_party/python/Lib/venv/scripts/nt/Activate.ps1							\
	third_party/python/Lib/venv/scripts/nt/activate.bat							\
	third_party/python/Lib/venv/scripts/nt/deactivate.bat							\
	third_party/python/Lib/venv/scripts/posix/activate.csh							\
	third_party/python/Lib/venv/scripts/posix/activate.fish							\
	third_party/python/Lib/test/hello.com									\
	third_party/python/Lib/test/xmltestdata/								\
	third_party/python/Lib/test/xmltestdata/simple.xml							\
	third_party/python/Lib/test/xmltestdata/simple-ns.xml							\
	third_party/python/Lib/test/xmltestdata/expat224_utf8_bug.xml						\
	third_party/python/Lib/test/xmltestdata/test.xml.out							\
	third_party/python/Lib/test/xmltestdata/test.xml							\
	third_party/python/Lib/test/keycert.passwd.pem								\
	third_party/python/Lib/test/test_email/data/msg_44.txt							\
	third_party/python/Lib/test/test_email/data/PyBanner048.gif						\
	third_party/python/Lib/test/test_email/data/msg_40.txt							\
	third_party/python/Lib/test/test_email/data/msg_18.txt							\
	third_party/python/Lib/test/test_email/data/msg_33.txt							\
	third_party/python/Lib/test/test_email/data/msg_05.txt							\
	third_party/python/Lib/test/test_email/data/msg_09.txt							\
	third_party/python/Lib/test/test_email/data/msg_15.txt							\
	third_party/python/Lib/test/test_email/data/msg_14.txt							\
	third_party/python/Lib/test/test_email/data/msg_03.txt							\
	third_party/python/Lib/test/test_email/data/msg_32.txt							\
	third_party/python/Lib/test/test_email/data/msg_12.txt							\
	third_party/python/Lib/test/test_email/data/msg_12a.txt							\
	third_party/python/Lib/test/test_email/data/msg_01.txt							\
	third_party/python/Lib/test/test_email/data/msg_30.txt							\
	third_party/python/Lib/test/test_email/data/msg_36.txt							\
	third_party/python/Lib/test/test_email/data/msg_20.txt							\
	third_party/python/Lib/test/test_email/data/msg_38.txt							\
	third_party/python/Lib/test/test_email/data/msg_24.txt							\
	third_party/python/Lib/test/test_email/data/msg_31.txt							\
	third_party/python/Lib/test/test_email/data/msg_42.txt							\
	third_party/python/Lib/test/test_email/data/msg_34.txt							\
	third_party/python/Lib/test/test_email/data/msg_17.txt							\
	third_party/python/Lib/test/test_email/data/msg_07.txt							\
	third_party/python/Lib/test/test_email/data/msg_35.txt							\
	third_party/python/Lib/test/test_email/data/msg_28.txt							\
	third_party/python/Lib/test/test_email/data/msg_19.txt							\
	third_party/python/Lib/test/test_email/data/msg_02.txt							\
	third_party/python/Lib/test/test_email/data/msg_29.txt							\
	third_party/python/Lib/test/test_email/data/msg_46.txt							\
	third_party/python/Lib/test/test_email/data/msg_08.txt							\
	third_party/python/Lib/test/test_email/data/audiotest.au						\
	third_party/python/Lib/test/test_email/data/msg_21.txt							\
	third_party/python/Lib/test/test_email/data/msg_41.txt							\
	third_party/python/Lib/test/test_email/data/msg_23.txt							\
	third_party/python/Lib/test/test_email/data/msg_06.txt							\
	third_party/python/Lib/test/test_email/data/msg_27.txt							\
	third_party/python/Lib/test/test_email/data/msg_11.txt							\
	third_party/python/Lib/test/test_email/data/msg_16.txt							\
	third_party/python/Lib/test/test_email/data/msg_25.txt							\
	third_party/python/Lib/test/test_email/data/msg_45.txt							\
	third_party/python/Lib/test/test_email/data/msg_43.txt							\
	third_party/python/Lib/test/test_email/data/msg_22.txt							\
	third_party/python/Lib/test/test_email/data/msg_39.txt							\
	third_party/python/Lib/test/test_email/data/msg_04.txt							\
	third_party/python/Lib/test/test_email/data/msg_37.txt							\
	third_party/python/Lib/test/test_email/data/msg_13.txt							\
	third_party/python/Lib/test/test_email/data/msg_26.txt							\
	third_party/python/Lib/test/test_email/data/msg_10.txt							\
	third_party/python/Lib/test/sndhdrdata/							\
	third_party/python/Lib/test/sndhdrdata/sndhdr.hcom							\
	third_party/python/Lib/test/sndhdrdata/sndhdr.wav							\
	third_party/python/Lib/test/sndhdrdata/sndhdr.au							\
	third_party/python/Lib/test/sndhdrdata/sndhdr.aifc							\
	third_party/python/Lib/test/sndhdrdata/sndhdr.voc							\
	third_party/python/Lib/test/sndhdrdata/sndhdr.aiff							\
	third_party/python/Lib/test/sndhdrdata/sndhdr.8svx							\
	third_party/python/Lib/test/sndhdrdata/sndhdr.sndt							\
	third_party/python/Lib/test/sndhdrdata/README								\
	third_party/python/Lib/test/allsans.pem									\
	third_party/python/Lib/test/nullcert.pem								\
	third_party/python/Lib/test/test_doctest.txt								\
	third_party/python/Lib/test/test_doctest2.txt								\
	third_party/python/Lib/test/test_doctest3.txt								\
	third_party/python/Lib/test/test_doctest4.txt								\
	third_party/python/Lib/test/audiodata/pluck-alaw.aifc							\
	third_party/python/Lib/test/audiodata/pluck-pcm32.wav							\
	third_party/python/Lib/test/audiodata/pluck-pcm8.aiff							\
	third_party/python/Lib/test/audiodata/pluck-pcm16.au							\
	third_party/python/Lib/test/audiodata/pluck-pcm32.au							\
	third_party/python/Lib/test/audiodata/pluck-pcm8.wav							\
	third_party/python/Lib/test/audiodata/pluck-pcm32.aiff							\
	third_party/python/Lib/test/audiodata/pluck-ulaw.au							\
	third_party/python/Lib/test/audiodata/pluck-pcm16.aiff							\
	third_party/python/Lib/test/audiodata/pluck-pcm24.au							\
	third_party/python/Lib/test/audiodata/pluck-ulaw.aifc							\
	third_party/python/Lib/test/audiodata/pluck-pcm16.wav							\
	third_party/python/Lib/test/audiodata/pluck-pcm24.aiff							\
	third_party/python/Lib/test/audiodata/pluck-pcm24.wav							\
	third_party/python/Lib/test/audiodata/pluck-pcm8.au							\
	third_party/python/Lib/test/imghdrdata/										\
	third_party/python/Lib/test/imghdrdata/python.pgm							\
	third_party/python/Lib/test/imghdrdata/python.jpg							\
	third_party/python/Lib/test/imghdrdata/python.bmp							\
	third_party/python/Lib/test/imghdrdata/python.xbm							\
	third_party/python/Lib/test/imghdrdata/python.sgi							\
	third_party/python/Lib/test/imghdrdata/python.ras							\
	third_party/python/Lib/test/imghdrdata/python.webp							\
	third_party/python/Lib/test/imghdrdata/python.ppm							\
	third_party/python/Lib/test/imghdrdata/python.png							\
	third_party/python/Lib/test/imghdrdata/python.tiff							\
	third_party/python/Lib/test/imghdrdata/python.pbm							\
	third_party/python/Lib/test/imghdrdata/python.gif							\
	third_party/python/Lib/test/imghdrdata/python.exr							\
	third_party/python/Lib/test/exception_hierarchy.txt							\
	third_party/python/Lib/test/decimaltestdata/nexttoward.decTest						\
	third_party/python/Lib/test/decimaltestdata/ln.decTest							\
	third_party/python/Lib/test/decimaltestdata/dqMinMag.decTest						\
	third_party/python/Lib/test/decimaltestdata/dqCompareTotalMag.decTest					\
	third_party/python/Lib/test/decimaltestdata/dqSameQuantum.decTest					\
	third_party/python/Lib/test/decimaltestdata/dqScaleB.decTest						\
	third_party/python/Lib/test/decimaltestdata/testall.decTest						\
	third_party/python/Lib/test/decimaltestdata/dqCompareSig.decTest					\
	third_party/python/Lib/test/decimaltestdata/ddFMA.decTest						\
	third_party/python/Lib/test/decimaltestdata/decQuad.decTest						\
	third_party/python/Lib/test/decimaltestdata/dqClass.decTest						\
	third_party/python/Lib/test/decimaltestdata/nextplus.decTest						\
	third_party/python/Lib/test/decimaltestdata/dqNextMinus.decTest						\
	third_party/python/Lib/test/decimaltestdata/dqQuantize.decTest						\
	third_party/python/Lib/test/decimaltestdata/ddMaxMag.decTest						\
	third_party/python/Lib/test/decimaltestdata/ddCompareTotal.decTest					\
	third_party/python/Lib/test/decimaltestdata/ddCompareTotalMag.decTest					\
	third_party/python/Lib/test/decimaltestdata/abs.decTest							\
	third_party/python/Lib/test/decimaltestdata/ddAnd.decTest						\
	third_party/python/Lib/test/decimaltestdata/dqRemainderNear.decTest					\
	third_party/python/Lib/test/decimaltestdata/powersqrt.decTest						\
	third_party/python/Lib/test/decimaltestdata/ddReduce.decTest						\
	third_party/python/Lib/test/decimaltestdata/comparetotal.decTest					\
	third_party/python/Lib/test/decimaltestdata/ddMax.decTest						\
	third_party/python/Lib/test/decimaltestdata/dsEncode.decTest						\
	third_party/python/Lib/test/decimaltestdata/compare.decTest						\
	third_party/python/Lib/test/decimaltestdata/ddMin.decTest						\
	third_party/python/Lib/test/decimaltestdata/dqAdd.decTest						\
	third_party/python/Lib/test/decimaltestdata/ddCopySign.decTest						\
	third_party/python/Lib/test/decimaltestdata/divideint.decTest						\
	third_party/python/Lib/test/decimaltestdata/dqFMA.decTest						\
	third_party/python/Lib/test/decimaltestdata/squareroot.decTest						\
	third_party/python/Lib/test/decimaltestdata/dqMax.decTest						\
	third_party/python/Lib/test/decimaltestdata/dqRemainder.decTest						\
	third_party/python/Lib/test/decimaltestdata/dqRotate.decTest						\
	third_party/python/Lib/test/decimaltestdata/dqCopySign.decTest						\
	third_party/python/Lib/test/decimaltestdata/dqAbs.decTest						\
	third_party/python/Lib/test/decimaltestdata/max.decTest							\
	third_party/python/Lib/test/decimaltestdata/class.decTest						\
	third_party/python/Lib/test/decimaltestdata/copysign.decTest						\
	third_party/python/Lib/test/decimaltestdata/power.decTest						\
	third_party/python/Lib/test/decimaltestdata/ddMultiply.decTest						\
	third_party/python/Lib/test/decimaltestdata/ddDivide.decTest						\
	third_party/python/Lib/test/decimaltestdata/ddRemainderNear.decTest					\
	third_party/python/Lib/test/decimaltestdata/log10.decTest						\
	third_party/python/Lib/test/decimaltestdata/ddPlus.decTest						\
	third_party/python/Lib/test/decimaltestdata/dsBase.decTest						\
	third_party/python/Lib/test/decimaltestdata/remainderNear.decTest					\
	third_party/python/Lib/test/decimaltestdata/ddSubtract.decTest						\
	third_party/python/Lib/test/decimaltestdata/dqCompareTotal.decTest					\
	third_party/python/Lib/test/decimaltestdata/ddCompare.decTest						\
	third_party/python/Lib/test/decimaltestdata/ddNextToward.decTest					\
	third_party/python/Lib/test/decimaltestdata/extra.decTest						\
	third_party/python/Lib/test/decimaltestdata/dqMaxMag.decTest						\
	third_party/python/Lib/test/decimaltestdata/dqCopyAbs.decTest						\
	third_party/python/Lib/test/decimaltestdata/ddCopy.decTest						\
	third_party/python/Lib/test/decimaltestdata/add.decTest							\
	third_party/python/Lib/test/decimaltestdata/dqMinus.decTest						\
	third_party/python/Lib/test/decimaltestdata/ddToIntegral.decTest					\
	third_party/python/Lib/test/decimaltestdata/scaleb.decTest						\
	third_party/python/Lib/test/decimaltestdata/dqShift.decTest						\
	third_party/python/Lib/test/decimaltestdata/copyabs.decTest						\
	third_party/python/Lib/test/decimaltestdata/ddScaleB.decTest						\
	third_party/python/Lib/test/decimaltestdata/copynegate.decTest						\
	third_party/python/Lib/test/decimaltestdata/reduce.decTest						\
	third_party/python/Lib/test/decimaltestdata/dqEncode.decTest						\
	third_party/python/Lib/test/decimaltestdata/minus.decTest						\
	third_party/python/Lib/test/decimaltestdata/dqCopyNegate.decTest					\
	third_party/python/Lib/test/decimaltestdata/tointegral.decTest						\
	third_party/python/Lib/test/decimaltestdata/dqCopy.decTest						\
	third_party/python/Lib/test/decimaltestdata/dqMin.decTest						\
	third_party/python/Lib/test/decimaltestdata/quantize.decTest						\
	third_party/python/Lib/test/decimaltestdata/dqDivideInt.decTest						\
	third_party/python/Lib/test/decimaltestdata/shift.decTest						\
	third_party/python/Lib/test/decimaltestdata/clamp.decTest						\
	third_party/python/Lib/test/decimaltestdata/dqInvert.decTest						\
	third_party/python/Lib/test/decimaltestdata/exp.decTest							\
	third_party/python/Lib/test/decimaltestdata/dqDivide.decTest						\
	third_party/python/Lib/test/decimaltestdata/dqAnd.decTest						\
	third_party/python/Lib/test/decimaltestdata/randoms.decTest						\
	third_party/python/Lib/test/decimaltestdata/dqLogB.decTest						\
	third_party/python/Lib/test/decimaltestdata/ddOr.decTest						\
	third_party/python/Lib/test/decimaltestdata/comparetotmag.decTest					\
	third_party/python/Lib/test/decimaltestdata/or.decTest							\
	third_party/python/Lib/test/decimaltestdata/tointegralx.decTest						\
	third_party/python/Lib/test/decimaltestdata/ddLogB.decTest						\
	third_party/python/Lib/test/decimaltestdata/dqPlus.decTest						\
	third_party/python/Lib/test/decimaltestdata/rounding.decTest						\
	third_party/python/Lib/test/decimaltestdata/decDouble.decTest						\
	third_party/python/Lib/test/decimaltestdata/samequantum.decTest						\
	third_party/python/Lib/test/decimaltestdata/dqMultiply.decTest						\
	third_party/python/Lib/test/decimaltestdata/ddShift.decTest						\
	third_party/python/Lib/test/decimaltestdata/ddMinus.decTest						\
	third_party/python/Lib/test/decimaltestdata/invert.decTest						\
	third_party/python/Lib/test/decimaltestdata/ddSameQuantum.decTest					\
	third_party/python/Lib/test/decimaltestdata/dqNextToward.decTest					\
	third_party/python/Lib/test/decimaltestdata/minmag.decTest						\
	third_party/python/Lib/test/decimaltestdata/logb.decTest						\
	third_party/python/Lib/test/decimaltestdata/dqBase.decTest						\
	third_party/python/Lib/test/decimaltestdata/rotate.decTest						\
	third_party/python/Lib/test/decimaltestdata/subtract.decTest						\
	third_party/python/Lib/test/decimaltestdata/dqToIntegral.decTest					\
	third_party/python/Lib/test/decimaltestdata/ddBase.decTest						\
	third_party/python/Lib/test/decimaltestdata/ddQuantize.decTest						\
	third_party/python/Lib/test/decimaltestdata/and.decTest							\
	third_party/python/Lib/test/decimaltestdata/ddNextMinus.decTest						\
	third_party/python/Lib/test/decimaltestdata/inexact.decTest						\
	third_party/python/Lib/test/decimaltestdata/ddInvert.decTest						\
	third_party/python/Lib/test/decimaltestdata/ddCopyAbs.decTest						\
	third_party/python/Lib/test/decimaltestdata/remainder.decTest						\
	third_party/python/Lib/test/decimaltestdata/ddEncode.decTest						\
	third_party/python/Lib/test/decimaltestdata/ddXor.decTest						\
	third_party/python/Lib/test/decimaltestdata/ddCompareSig.decTest					\
	third_party/python/Lib/test/decimaltestdata/maxmag.decTest						\
	third_party/python/Lib/test/decimaltestdata/dqReduce.decTest						\
	third_party/python/Lib/test/decimaltestdata/ddCanonical.decTest						\
	third_party/python/Lib/test/decimaltestdata/dqNextPlus.decTest						\
	third_party/python/Lib/test/decimaltestdata/xor.decTest							\
	third_party/python/Lib/test/decimaltestdata/rescale.decTest						\
	third_party/python/Lib/test/decimaltestdata/ddCopyNegate.decTest					\
	third_party/python/Lib/test/decimaltestdata/ddRemainder.decTest						\
	third_party/python/Lib/test/decimaltestdata/base.decTest						\
	third_party/python/Lib/test/decimaltestdata/multiply.decTest						\
	third_party/python/Lib/test/decimaltestdata/plus.decTest						\
	third_party/python/Lib/test/decimaltestdata/dqCanonical.decTest						\
	third_party/python/Lib/test/decimaltestdata/copy.decTest						\
	third_party/python/Lib/test/decimaltestdata/ddRotate.decTest						\
	third_party/python/Lib/test/decimaltestdata/ddNextPlus.decTest						\
	third_party/python/Lib/test/decimaltestdata/dqSubtract.decTest						\
	third_party/python/Lib/test/decimaltestdata/ddAdd.decTest						\
	third_party/python/Lib/test/decimaltestdata/randomBound32.decTest					\
	third_party/python/Lib/test/decimaltestdata/dqXor.decTest						\
	third_party/python/Lib/test/decimaltestdata/nextminus.decTest						\
	third_party/python/Lib/test/decimaltestdata/dqCompare.decTest						\
	third_party/python/Lib/test/decimaltestdata/ddAbs.decTest						\
	third_party/python/Lib/test/decimaltestdata/min.decTest							\
	third_party/python/Lib/test/decimaltestdata/decSingle.decTest						\
	third_party/python/Lib/test/decimaltestdata/fma.decTest							\
	third_party/python/Lib/test/decimaltestdata/ddClass.decTest						\
	third_party/python/Lib/test/decimaltestdata/ddDivideInt.decTest						\
	third_party/python/Lib/test/decimaltestdata/dqOr.decTest						\
	third_party/python/Lib/test/decimaltestdata/ddMinMag.decTest						\
	third_party/python/Lib/test/decimaltestdata/divide.decTest						\
	third_party/python/Lib/test/decimaltestdata/								\
	third_party/python/Lib/test/floating_points.txt								\
	third_party/python/Lib/test/badkey.pem									\
	third_party/python/Lib/test/revocation.crl								\
	third_party/python/Lib/test/recursion.tar								\
	third_party/python/Lib/test/tokenize_tests-no-coding-cookie-and-utf8-bom-sig-only.txt			\
	third_party/python/Lib/test/test_doctest3.txt								\
	third_party/python/Lib/test/keycert3.pem								\
	third_party/python/Lib/test/testtar.tar									\
	third_party/python/Lib/test/zip_cp437_header.zip							\
	third_party/python/Lib/test/pycakey.pem									\
	third_party/python/Lib/test/pycacert.pem								\
	third_party/python/Lib/test/ffdh3072.pem								\
	third_party/python/Lib/test/randv2_32.pck								\
	third_party/python/Lib/test/randv2_64.pck								\
	third_party/python/Lib/test/mailcap.txt									\
	third_party/python/Lib/test/test_doctest4.txt								\
	third_party/python/Lib/test/sgml_input.html								\
	third_party/python/Lib/test/test_difflib_expect.html							\
	third_party/python/Lib/test/math_testcases.txt								\
	third_party/python/Lib/test/leakers/README.txt								\
	third_party/python/Lib/test/nullbytecert.pem								\
	third_party/python/Lib/test/cfgparser.1									\
	third_party/python/Lib/test/nokia.pem									\
	third_party/python/Lib/test/zipdir.zip									\
	third_party/python/Lib/test/keycert4.pem								\
	third_party/python/Lib/test/tokenize_tests-latin1-coding-cookie-and-utf8-bom-sig.txt			\
	third_party/python/Lib/test/test_doctest2.txt								\
	third_party/python/Lib/test/keycert2.pem								\
	third_party/python/Lib/test/capath/ceff1710.0								\
	third_party/python/Lib/test/capath/99d0fa06.0								\
	third_party/python/Lib/test/capath/5ed36f99.0								\
	third_party/python/Lib/test/capath/b1930218.0								\
	third_party/python/Lib/test/capath/6e88d7b8.0								\
	third_party/python/Lib/test/capath/4e1295a3.0								\
	third_party/python/Lib/test/audiotest.au								\
	third_party/python/Lib/test/dtracedata/									\
	third_party/python/Lib/test/dtracedata/gc.d.expected							\
	third_party/python/Lib/test/dtracedata/gc.d								\
	third_party/python/Lib/test/dtracedata/line.d								\
	third_party/python/Lib/test/dtracedata/assert_usable.d							\
	third_party/python/Lib/test/dtracedata/assert_usable.stp						\
	third_party/python/Lib/test/dtracedata/call_stack.d							\
	third_party/python/Lib/test/dtracedata/call_stack.stp.expected						\
	third_party/python/Lib/test/dtracedata/gc.stp								\
	third_party/python/Lib/test/dtracedata/call_stack.stp							\
	third_party/python/Lib/test/dtracedata/gc.stp.expected							\
	third_party/python/Lib/test/dtracedata/call_stack.d.expected						\
	third_party/python/Lib/test/dtracedata/line.d.expected							\
	third_party/python/Lib/test/tokenize_tests.txt								\
	third_party/python/Lib/test/selfsigned_pythontestdotnet.pem						\
	third_party/python/Lib/test/tokenize_tests-utf8-coding-cookie-and-utf8-bom-sig.txt			\
	third_party/python/Lib/test/keycert.pem									\
	third_party/python/Lib/test/crashers/README								\
	third_party/python/Lib/test/ieee754.txt									\
	third_party/python/Lib/test/randv3.pck									\
	third_party/python/Lib/test/empty.vbs									\
	third_party/python/Lib/test/tokenize_tests-utf8-coding-cookie-and-no-utf8-bom-sig.txt			\
	third_party/python/Lib/test/Sine-1000Hz-300ms.aif							\
	third_party/python/Lib/test/cjkencodings/								\
	third_party/python/Lib/test/cjkencodings/gbk-utf8.txt							\
	third_party/python/Lib/test/cjkencodings/gb2312-utf8.txt						\
	third_party/python/Lib/test/cjkencodings/hz-utf8.txt							\
	third_party/python/Lib/test/cjkencodings/euc_jisx0213-utf8.txt						\
	third_party/python/Lib/test/cjkencodings/euc_jisx0213.txt						\
	third_party/python/Lib/test/cjkencodings/johab-utf8.txt							\
	third_party/python/Lib/test/cjkencodings/euc_kr-utf8.txt						\
	third_party/python/Lib/test/cjkencodings/cp949-utf8.txt							\
	third_party/python/Lib/test/cjkencodings/iso2022_jp.txt							\
	third_party/python/Lib/test/cjkencodings/iso2022_kr-utf8.txt						\
	third_party/python/Lib/test/cjkencodings/johab.txt							\
	third_party/python/Lib/test/cjkencodings/euc_jp.txt							\
	third_party/python/Lib/test/cjkencodings/gb2312.txt							\
	third_party/python/Lib/test/cjkencodings/big5hkscs.txt							\
	third_party/python/Lib/test/cjkencodings/big5.txt							\
	third_party/python/Lib/test/cjkencodings/gbk.txt							\
	third_party/python/Lib/test/cjkencodings/iso2022_kr.txt							\
	third_party/python/Lib/test/cjkencodings/gb18030.txt							\
	third_party/python/Lib/test/cjkencodings/big5hkscs-utf8.txt						\
	third_party/python/Lib/test/cjkencodings/shift_jis.txt							\
	third_party/python/Lib/test/cjkencodings/cp949.txt							\
	third_party/python/Lib/test/cjkencodings/euc_jp-utf8.txt						\
	third_party/python/Lib/test/cjkencodings/hz.txt								\
	third_party/python/Lib/test/cjkencodings/big5-utf8.txt							\
	third_party/python/Lib/test/cjkencodings/gb18030-utf8.txt						\
	third_party/python/Lib/test/cjkencodings/shift_jisx0213.txt						\
	third_party/python/Lib/test/cjkencodings/shift_jis-utf8.txt						\
	third_party/python/Lib/test/cjkencodings/shift_jisx0213-utf8.txt					\
	third_party/python/Lib/test/cjkencodings/euc_kr.txt							\
	third_party/python/Lib/test/cjkencodings/iso2022_jp-utf8.txt						\
	third_party/python/Lib/test/BIG5.TXT									\
	third_party/python/Lib/test/BIG5HKSCS-2004.TXT								\
	third_party/python/Lib/test/CP932.TXT									\
	third_party/python/Lib/test/CP936.TXT									\
	third_party/python/Lib/test/CP949.TXT									\
	third_party/python/Lib/test/CP950.TXT									\
	third_party/python/Lib/test/EUC-CN.TXT									\
	third_party/python/Lib/test/EUC-JISX0213.TXT								\
	third_party/python/Lib/test/EUC-JP.TXT									\
	third_party/python/Lib/test/EUC-KR.TXT									\
	third_party/python/Lib/test/JOHAB.TXT									\
	third_party/python/Lib/test/SHIFTJIS.TXT								\
	third_party/python/Lib/test/SHIFT_JISX0213.TXT								\
	third_party/python/Lib/test/gb-18030-2000.ucm								\
	third_party/python/Lib/test/cfgparser.3									\
	third_party/python/Lib/test/floating_points.txt								\
	third_party/python/Lib/test/formatfloat_testcases.txt							\
	third_party/python/Lib/test/talos-2019-0758.pem								\
	third_party/python/Lib/test/badcert.pem									\
	third_party/python/Lib/test/bad_coding.py								\
	third_party/python/Lib/test/bad_coding2.py								\
	third_party/python/Lib/test/cmath_testcases.txt								\
	third_party/python/Lib/test/pstats.pck									\
	third_party/python/Lib/test/test_importlib/namespace_pkgs/project2/parent/child/two.py			\
	third_party/python/Lib/test/test_importlib/namespace_pkgs/portion2/foo/two.py				\
	third_party/python/Lib/test/test_importlib/namespace_pkgs/project3/parent/child/three.py		\
	third_party/python/Lib/test/test_importlib/namespace_pkgs/portion1/foo/one.py				\
	third_party/python/Lib/test/test_importlib/namespace_pkgs/both_portions/foo/two.py			\
	third_party/python/Lib/test/test_importlib/namespace_pkgs/both_portions/foo/one.py			\
	third_party/python/Lib/test/test_importlib/namespace_pkgs/project1/parent/child/one.py			\
	third_party/python/Lib/test/test_importlib/namespace_pkgs/not_a_namespace_pkg/foo/one.py		\
	third_party/python/Lib/test/test_importlib/namespace_pkgs/not_a_namespace_pkg/foo/__init__.py		\
	third_party/python/Lib/test/test_importlib/namespace_pkgs/module_and_namespace_package/a_test.py	\
	third_party/python/Lib/test/dtracedata/line.py								\
	third_party/python/Lib/test/dtracedata/gc.py								\
	third_party/python/Lib/test/dtracedata/instance.py							\
	third_party/python/Lib/test/dtracedata/call_stack.py							\
	third_party/python/Lib/test/subprocessdata/qgrep.py							\
	third_party/python/Lib/test/subprocessdata/input_reader.py						\
	third_party/python/Lib/test/subprocessdata/qcat.py							\
	third_party/python/Lib/test/subprocessdata/sigchild_ignore.py						\
	third_party/python/Lib/test/subprocessdata/fd_status.py							\
	third_party/python/Lib/test/crashers/recursive_call.py							\
	third_party/python/Lib/test/crashers/gc_inspection.py							\
	third_party/python/Lib/test/crashers/mutation_inside_cyclegc.py						\
	third_party/python/Lib/test/crashers/infinite_loop_re.py						\
	third_party/python/Lib/test/crashers/trace_at_recursion_limit.py					\
	third_party/python/Lib/test/crashers/underlying_dict.py							\
	third_party/python/Lib/test/crashers/bogus_code_obj.py							\
	third_party/python/Lib/test/cfgparser.2

THIRD_PARTY_PYTHON_PYTEST_A_DIRECTDEPS =					\
	LIBC_NEXGEN32E								\
	THIRD_PARTY_PYTHON_STAGE1						\
	THIRD_PARTY_PYTHON_STAGE2

################################################################################
# TESTS

THIRD_PARTY_PYTHON_PYTEST_PYMAINS =						\
	third_party/python/Lib/test/signalinterproctester.py			\
	third_party/python/Lib/test/test___future__.py				\
	third_party/python/Lib/test/test__locale.py				\
	third_party/python/Lib/test/test__opcode.py				\
	third_party/python/Lib/test/test_abc.py					\
	third_party/python/Lib/test/test_abstract_numbers.py			\
	third_party/python/Lib/test/test_aifc.py				\
	third_party/python/Lib/test/test_array.py				\
	third_party/python/Lib/test/test_asdl_parser.py				\
	third_party/python/Lib/test/test_atexit.py				\
	third_party/python/Lib/test/test_audioop.py				\
	third_party/python/Lib/test/test_augassign.py				\
	third_party/python/Lib/test/test_base64.py				\
	third_party/python/Lib/test/test_baseexception.py			\
	third_party/python/Lib/test/test_bigaddrspace.py			\
	third_party/python/Lib/test/test_bigmem.py				\
	third_party/python/Lib/test/test_binascii.py				\
	third_party/python/Lib/test/test_binhex.py				\
	third_party/python/Lib/test/test_binop.py				\
	third_party/python/Lib/test/test_bisect.py				\
	third_party/python/Lib/test/test_bool.py				\
	third_party/python/Lib/test/test_buffer.py				\
	third_party/python/Lib/test/test_bufio.py				\
	third_party/python/Lib/test/test_builtin.py				\
	third_party/python/Lib/test/test_bytes.py				\
	third_party/python/Lib/test/test_bz2.py					\
	third_party/python/Lib/test/test_calendar.py				\
	third_party/python/Lib/test/test_call.py				\
	third_party/python/Lib/test/test_cgi.py					\
	third_party/python/Lib/test/test_cgitb.py				\
	third_party/python/Lib/test/test_charmapcodec.py			\
	third_party/python/Lib/test/test_class.py				\
	third_party/python/Lib/test/test_cmath.py				\
	third_party/python/Lib/test/test_cmd.py					\
	third_party/python/Lib/test/test_cmd_line.py				\
	third_party/python/Lib/test/test_cmd_line_script.py		\
	third_party/python/Lib/test/test_code.py				\
	third_party/python/Lib/test/test_code_module.py				\
	third_party/python/Lib/test/test_codeccallbacks.py			\
	third_party/python/Lib/test/test_codecencodings_cn.py			\
	third_party/python/Lib/test/test_codecencodings_hk.py			\
	third_party/python/Lib/test/test_codecencodings_iso2022.py		\
	third_party/python/Lib/test/test_codecencodings_jp.py			\
	third_party/python/Lib/test/test_codecencodings_kr.py			\
	third_party/python/Lib/test/test_codecencodings_tw.py			\
	third_party/python/Lib/test/test_codecmaps_cn.py			\
	third_party/python/Lib/test/test_codecmaps_hk.py			\
	third_party/python/Lib/test/test_codecmaps_jp.py			\
	third_party/python/Lib/test/test_codecmaps_kr.py			\
	third_party/python/Lib/test/test_codecmaps_tw.py			\
	third_party/python/Lib/test/test_codecs.py				\
	third_party/python/Lib/test/test_codeop.py				\
	third_party/python/Lib/test/test_collections.py				\
	third_party/python/Lib/test/test_colorsys.py				\
	third_party/python/Lib/test/test_compare.py				\
	third_party/python/Lib/test/test_complex.py				\
	third_party/python/Lib/test/test_contains.py				\
	third_party/python/Lib/test/test_contextlib.py				\
	third_party/python/Lib/test/test_copy.py				\
	third_party/python/Lib/test/test_copyreg.py				\
	third_party/python/Lib/test/test_coroutines.py				\
	third_party/python/Lib/test/test_cosmo.py				\
	third_party/python/Lib/test/test_cprofile.py				\
	third_party/python/Lib/test/test_crashers.py				\
	third_party/python/Lib/test/test_csv.py					\
	third_party/python/Lib/test/test_datetime.py				\
	third_party/python/Lib/test/test_decimal.py				\
	third_party/python/Lib/test/test_decorators.py				\
	third_party/python/Lib/test/test_defaultdict.py				\
	third_party/python/Lib/test/test_deque.py				\
	third_party/python/Lib/test/test_descrtut.py				\
	third_party/python/Lib/test/test_dict.py				\
	third_party/python/Lib/test/test_dict_version.py			\
	third_party/python/Lib/test/test_dictcomps.py				\
	third_party/python/Lib/test/test_dictviews.py				\
	third_party/python/Lib/test/test_difflib.py				\
	third_party/python/Lib/test/test_dis.py					\
	third_party/python/Lib/test/test_doctest.py				\
	third_party/python/Lib/test/test_doctest2.py				\
	third_party/python/Lib/test/test_dummy_threading.py			\
	third_party/python/Lib/test/test_dynamic.py				\
	third_party/python/Lib/test/test_dynamicclassattribute.py		\
	third_party/python/Lib/test/test_email/test__encoded_words.py		\
	third_party/python/Lib/test/test_email/test__header_value_parser.py	\
	third_party/python/Lib/test/test_email/test_asian_codecs.py		\
	third_party/python/Lib/test/test_email/test_contentmanager.py		\
	third_party/python/Lib/test/test_email/test_defect_handling.py		\
	third_party/python/Lib/test/test_email/test_email.py			\
	third_party/python/Lib/test/test_email/test_generator.py		\
	third_party/python/Lib/test/test_email/test_headerregistry.py		\
	third_party/python/Lib/test/test_email/test_inversion.py		\
	third_party/python/Lib/test/test_email/test_message.py			\
	third_party/python/Lib/test/test_email/test_parser.py			\
	third_party/python/Lib/test/test_email/test_pickleable.py		\
	third_party/python/Lib/test/test_email/test_policy.py			\
	third_party/python/Lib/test/test_email/test_utils.py			\
	third_party/python/Lib/test/test_enum.py				\
	third_party/python/Lib/test/test_enumerate.py				\
	third_party/python/Lib/test/test_eof.py					\
	third_party/python/Lib/test/test_epoll.py				\
	third_party/python/Lib/test/test_errno.py				\
	third_party/python/Lib/test/test_exception_hierarchy.py			\
	third_party/python/Lib/test/test_exception_variations.py		\
	third_party/python/Lib/test/test_exceptions.py				\
	third_party/python/Lib/test/test_extcall.py				\
	third_party/python/Lib/test/test_fcntl.py				\
	third_party/python/Lib/test/test_file.py				\
	third_party/python/Lib/test/test_file_eintr.py				\
	third_party/python/Lib/test/test_filecmp.py				\
	third_party/python/Lib/test/test_fileinput.py				\
	third_party/python/Lib/test/test_fileio.py				\
	third_party/python/Lib/test/test_finalization.py			\
	third_party/python/Lib/test/test_float.py				\
	third_party/python/Lib/test/test_fnmatch.py				\
	third_party/python/Lib/test/test_format.py				\
	third_party/python/Lib/test/test_fractions.py				\
	third_party/python/Lib/test/test_frame.py				\
	third_party/python/Lib/test/test_fstring.py				\
	third_party/python/Lib/test/test_funcattrs.py				\
	third_party/python/Lib/test/test_functools.py				\
	third_party/python/Lib/test/test_future.py				\
	third_party/python/Lib/test/test_future3.py				\
	third_party/python/Lib/test/test_future4.py				\
	third_party/python/Lib/test/test_future5.py				\
	third_party/python/Lib/test/test_gc.py					\
	third_party/python/Lib/test/test_generator_stop.py			\
	third_party/python/Lib/test/test_generators.py				\
	third_party/python/Lib/test/test_genericpath.py				\
	third_party/python/Lib/test/test_genexps.py				\
	third_party/python/Lib/test/test_getargs2.py				\
	third_party/python/Lib/test/test_getopt.py				\
	third_party/python/Lib/test/test_global.py				\
	third_party/python/Lib/test/test_grammar.py				\
	third_party/python/Lib/test/test_gzip.py				\
	third_party/python/Lib/test/test_hash.py				\
	third_party/python/Lib/test/test_hashlib.py				\
	third_party/python/Lib/test/test_heapq.py				\
	third_party/python/Lib/test/test_html.py				\
	third_party/python/Lib/test/test_htmlparser.py				\
	third_party/python/Lib/test/test_http_cookiejar.py			\
	third_party/python/Lib/test/test_http_cookies.py			\
	third_party/python/Lib/test/test_imghdr.py				\
	third_party/python/Lib/test/test_imp.py					\
	third_party/python/Lib/test/test_index.py				\
	third_party/python/Lib/test/test_int.py					\
	third_party/python/Lib/test/test_int_literal.py				\
	third_party/python/Lib/test/test_ipaddress.py				\
	third_party/python/Lib/test/test_isinstance.py				\
	third_party/python/Lib/test/test_iter.py				\
	third_party/python/Lib/test/test_iterlen.py				\
	third_party/python/Lib/test/test_itertools.py				\
	third_party/python/Lib/test/test_json/__main__.py			\
	third_party/python/Lib/test/test_kdf.py					\
	third_party/python/Lib/test/test_keywordonlyarg.py			\
	third_party/python/Lib/test/test_list.py				\
	third_party/python/Lib/test/test_listcomps.py				\
	third_party/python/Lib/test/test_long.py				\
	third_party/python/Lib/test/test_longexp.py				\
	third_party/python/Lib/test/test_marshal.py				\
	third_party/python/Lib/test/test_memoryio.py				\
	third_party/python/Lib/test/test_memoryview.py				\
	third_party/python/Lib/test/test_metaclass.py				\
	third_party/python/Lib/test/test_mimetypes.py				\
	third_party/python/Lib/test/test_minidom.py				\
	third_party/python/Lib/test/test_mmap.py				\
	third_party/python/Lib/test/test_module.py				\
	third_party/python/Lib/test/test_modulefinder.py			\
	third_party/python/Lib/test/test_multibytecodec.py			\
	third_party/python/Lib/test/test_numeric_tower.py			\
	third_party/python/Lib/test/test_opcodes.py				\
	third_party/python/Lib/test/test_operator.py				\
	third_party/python/Lib/test/test_optparse.py				\
	third_party/python/Lib/test/test_ordered_dict.py			\
	third_party/python/Lib/test/test_parser.py				\
	third_party/python/Lib/test/test_peepholer.py				\
	third_party/python/Lib/test/test_pickle.py				\
	third_party/python/Lib/test/test_pickletools.py				\
	third_party/python/Lib/test/test_pkgimport.py				\
	third_party/python/Lib/test/test_plistlib.py				\
	third_party/python/Lib/test/test_poll.py				\
	third_party/python/Lib/test/test_poll.py				\
	third_party/python/Lib/test/test_pow.py					\
	third_party/python/Lib/test/test_pprint.py				\
	third_party/python/Lib/test/test_print.py				\
	third_party/python/Lib/test/test_thread.py				\
	third_party/python/Lib/test/test_profile.py				\
	third_party/python/Lib/test/test_property.py				\
	third_party/python/Lib/test/test_pstats.py				\
	third_party/python/Lib/test/test_pulldom.py				\
	third_party/python/Lib/test/test_pyexpat.py				\
	third_party/python/Lib/test/test_quopri.py				\
	third_party/python/Lib/test/test_raise.py				\
	third_party/python/Lib/test/test_random.py				\
	third_party/python/Lib/test/test_range.py				\
	third_party/python/Lib/test/test_re.py					\
	third_party/python/Lib/test/test_repl.py				\
	third_party/python/Lib/test/test_reprlib.py				\
	third_party/python/Lib/test/test_richcmp.py				\
	third_party/python/Lib/test/test_robotparser.py				\
	third_party/python/Lib/test/test_sax.py					\
	third_party/python/Lib/test/test_sched.py				\
	third_party/python/Lib/test/test_scope.py				\
	third_party/python/Lib/test/test_scratch.py				\
	third_party/python/Lib/test/test_script_helper.py			\
	third_party/python/Lib/test/test_secrets.py				\
	third_party/python/Lib/test/test_select.py				\
	third_party/python/Lib/test/test_selectors.py				\
	third_party/python/Lib/test/test_set.py					\
	third_party/python/Lib/test/test_setcomps.py				\
	third_party/python/Lib/test/test_shlex.py				\
	third_party/python/Lib/test/test_signal.py				\
	third_party/python/Lib/test/test_slice.py				\
	third_party/python/Lib/test/test_sndhdr.py				\
	third_party/python/Lib/test/test_sort.py				\
	third_party/python/Lib/test/test_stat.py				\
	third_party/python/Lib/test/test_statistics.py				\
	third_party/python/Lib/test/test_strftime.py				\
	third_party/python/Lib/test/test_string.py				\
	third_party/python/Lib/test/test_string_literals.py			\
	third_party/python/Lib/test/test_stringprep.py				\
	third_party/python/Lib/test/test_strptime.py				\
	third_party/python/Lib/test/test_strtod.py				\
	third_party/python/Lib/test/test_struct.py				\
	third_party/python/Lib/test/test_structmembers.py			\
	third_party/python/Lib/test/test_structseq.py				\
	third_party/python/Lib/test/test_subclassinit.py			\
	third_party/python/Lib/test/test_sunau.py				\
	third_party/python/Lib/test/test_super.py				\
	third_party/python/Lib/test/test_symbol.py				\
	third_party/python/Lib/test/test_symtable.py				\
	third_party/python/Lib/test/test_syntax.py				\
	third_party/python/Lib/test/test_sys_setprofile.py			\
	third_party/python/Lib/test/test_syslog.py				\
	third_party/python/Lib/test/test_tarfile.py				\
	third_party/python/Lib/test/test_textwrap.py				\
	third_party/python/Lib/test/test_time.py				\
	third_party/python/Lib/test/test_timeit.py				\
	third_party/python/Lib/test/test_timeout.py				\
	third_party/python/Lib/test/test_traceback.py				\
	third_party/python/Lib/test/test_tracemalloc.py				\
	third_party/python/Lib/test/test_tuple.py				\
	third_party/python/Lib/test/test_typechecks.py				\
	third_party/python/Lib/test/test_types.py				\
	third_party/python/Lib/test/test_typing.py				\
	third_party/python/Lib/test/test_unary.py				\
	third_party/python/Lib/test/test_unicode.py				\
	third_party/python/Lib/test/test_unicode_file.py			\
	third_party/python/Lib/test/test_unicode_identifiers.py			\
	third_party/python/Lib/test/test_unicodedata.py				\
	third_party/python/Lib/test/test_univnewlines.py			\
	third_party/python/Lib/test/test_unpack.py				\
	third_party/python/Lib/test/test_unpack_ex.py				\
	third_party/python/Lib/test/test_urlparse.py				\
	third_party/python/Lib/test/test_userdict.py				\
	third_party/python/Lib/test/test_userlist.py				\
	third_party/python/Lib/test/test_userstring.py				\
	third_party/python/Lib/test/test_utf8source.py				\
	third_party/python/Lib/test/test_uu.py					\
	third_party/python/Lib/test/test_uuid.py				\
	third_party/python/Lib/test/test_wave.py				\
	third_party/python/Lib/test/test_weakref.py				\
	third_party/python/Lib/test/test_weakset.py				\
	third_party/python/Lib/test/test_with.py				\
	third_party/python/Lib/test/test_wsgiref.py				\
	third_party/python/Lib/test/test_xdrlib.py				\
	third_party/python/Lib/test/test_xml_dom_minicompat.py			\
	third_party/python/Lib/test/test_xml_etree_c.py				\
	third_party/python/Lib/test/test_yield_from.py				\
	third_party/python/Lib/test/test_zipapp.py				\
	third_party/python/Lib/test/test_zlib.py

# TODO: test_threading passing probably requires more mutexes in libc/calls/
# TODO: test_sys is potentially flaky now that we have threads

THIRD_PARTY_PYTHON_PYTEST_TOOSLOW =						\
	third_party/python/Lib/test/test_threadsignals.py

THIRD_PARTY_PYTHON_PYTEST_TODOS =						\
	third_party/python/Lib/test/test_sys.py					\
	third_party/python/Lib/test/test_threading.py				\
	third_party/python/Lib/test/test_threading_local.py			\
	third_party/python/Lib/test/mp_preload.py				\
	third_party/python/Lib/test/outstanding_bugs.py				\
	third_party/python/Lib/test/pythoninfo.py				\
	third_party/python/Lib/test/sortperf.py					\
	third_party/python/Lib/test/test_asyncgen.py				\
	third_party/python/Lib/test/test_asynchat.py				\
	third_party/python/Lib/test/test_asyncore.py				\
	third_party/python/Lib/test/test_capi.py				\
	third_party/python/Lib/test/test_crypt.py				\
	third_party/python/Lib/test/test_devpoll.py				\
	third_party/python/Lib/test/test_docxmlrpc.py				\
	third_party/python/Lib/test/test_dtrace.py				\
	third_party/python/Lib/test/test_eintr.py				\
	third_party/python/Lib/test/test_flufl.py				\
	third_party/python/Lib/test/test_fork1.py				\
	third_party/python/Lib/test/test_ftplib.py				\
	third_party/python/Lib/test/test_gdb.py					\
	third_party/python/Lib/test/test_httplib.py				\
	third_party/python/Lib/test/test_imaplib.py				\
	third_party/python/Lib/test/test_io.py					\
	third_party/python/Lib/test/test_kqueue.py				\
	third_party/python/Lib/test/test_largefile.py				\
	third_party/python/Lib/test/test_linecache.py				\
	third_party/python/Lib/test/test_locale.py				\
	third_party/python/Lib/test/test_macpath.py				\
	third_party/python/Lib/test/test_macurl2path.py				\
	third_party/python/Lib/test/test_mailcap.py				\
	third_party/python/Lib/test/test_multiprocessing_fork.py		\
	third_party/python/Lib/test/test_multiprocessing_forkserver.py		\
	third_party/python/Lib/test/test_multiprocessing_main_handling.py	\
	third_party/python/Lib/test/test_multiprocessing_main_handling.py	\
	third_party/python/Lib/test/test_multiprocessing_main_handling.py	\
	third_party/python/Lib/test/test_multiprocessing_spawn.py		\
	third_party/python/Lib/test/test_netrc.py				\
	third_party/python/Lib/test/test_nis.py					\
	third_party/python/Lib/test/test_nntplib.py				\
	third_party/python/Lib/test/test_normalization.py			\
	third_party/python/Lib/test/test_ntpath.py				\
	third_party/python/Lib/test/test_openpty.py				\
	third_party/python/Lib/test/test_ossaudiodev.py				\
	third_party/python/Lib/test/test_platform.py				\
	third_party/python/Lib/test/test_poplib.py				\
	third_party/python/Lib/test/test_pty.py					\
	third_party/python/Lib/test/test_pyclbr.py				\
	third_party/python/Lib/test/test_pydoc.py				\
	third_party/python/Lib/test/test_queue.py				\
	third_party/python/Lib/test/test_readline.py				\
	third_party/python/Lib/test/test_regrtest.py				\
	third_party/python/Lib/test/test_runpy.py				\
	third_party/python/Lib/test/test_smtpd.py				\
	third_party/python/Lib/test/test_smtplib.py				\
	third_party/python/Lib/test/test_smtpnet.py				\
	third_party/python/Lib/test/test_socket.py				\
	third_party/python/Lib/test/test_socketserver.py			\
	third_party/python/Lib/test/test_spwd.py				\
	third_party/python/Lib/test/test_startfile.py				\
	third_party/python/Lib/test/test_telnetlib.py				\
	third_party/python/Lib/test/test_threadedtempfile.py			\
	third_party/python/Lib/test/test_turtle.py				\
	third_party/python/Lib/test/test_unittest.py				\
	third_party/python/Lib/test/test_urllib.py				\
	third_party/python/Lib/test/test_urllib2.py				\
	third_party/python/Lib/test/test_urllib2_localnet.py			\
	third_party/python/Lib/test/test_urllib2net.py				\
	third_party/python/Lib/test/test_urllib_response.py			\
	third_party/python/Lib/test/test_urllibnet.py				\
	third_party/python/Lib/test/test_wait3.py				\
	third_party/python/Lib/test/test_wait4.py				\
	third_party/python/Lib/test/test_webbrowser.py				\
	third_party/python/Lib/test/test_xmlrpc_net.py			\
	third_party/python/Lib/test/test_zipfile.py				\
	third_party/python/Lib/test/test_zipfile64.py

THIRD_PARTY_PYTHON_PYTEST_PYMAINS_DIRECTDEPS =				\
	LIBC_NEXGEN32E							\
	LIBC_TESTLIB							\
	LIBC_LOG							\
	LIBC_MEM							\
	LIBC_INTRIN							\
	LIBC_X								\
	LIBC_STR							\
	LIBC_STDIO							\
	LIBC_CALLS							\
	LIBC_RUNTIME							\
	THIRD_PARTY_PYTHON_STAGE1					\
	THIRD_PARTY_PYTHON_STAGE2					\
	THIRD_PARTY_PYTHON_PYTEST					\
	THIRD_PARTY_LINENOISE						\
	THIRD_PARTY_XED							\
	TOOL_ARGS

THIRD_PARTY_PYTHON_PYTEST_PYMAINS_DEPS =				\
	$(call uniq,$(foreach x,$(THIRD_PARTY_PYTHON_PYTEST_PYMAINS_DIRECTDEPS),$($(x))))

o/$(MODE)/third_party/python/pythontester.pkg:				\
		$(THIRD_PARTY_PYTHON_PYTEST_PYMAINS_OBJS)		\
		o/$(MODE)/third_party/python/pythontester.o		\
		$(foreach x,$(THIRD_PARTY_PYTHON_PYTEST_PYMAINS_DIRECTDEPS),$($(x)_A).pkg)

o/$(MODE)/third_party/python/pythontester.com.dbg:			\
		o/$(MODE)/third_party/python/pythontester.pkg		\
		$(THIRD_PARTY_PYTHON_PYTEST_PYMAINS_DEPS)		\
		$(THIRD_PARTY_PYTHON_PYTEST_PYMAINS_OBJS)		\
		$(THIRD_PARTY_PYTHON_PYTEST_TODOS:%.py=o/$(MODE)/%.o)	\
		o/$(MODE)/third_party/python/pythontester.o		\
		$(CRT)							\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

# if these files exist, then python will try to open them
o/$(MODE)/third_party/python/Lib/test/test_robotparser.py.runs		\
o/$(MODE)/third_party/python/Lib/test/test_wsgiref.py.runs: private	\
		.UNVEIL +=						\
			/etc/mime.types					\
			/etc/httpd/mime.types				\
			/etc/httpd/conf/mime.types			\
			/etc/apache/mime.types				\
			/etc/apache2/mime.types				\
			/usr/local/etc/httpd/conf/mime.types		\
			/usr/local/lib/netscape/mime.types		\
			/usr/local/etc/httpd/conf/mime.types		\
			/usr/local/etc/mime.types

o/$(MODE)/third_party/python/Lib/test/test_epoll.py.runs:		\
		private .PLEDGE = stdio rpath wpath cpath fattr proc inet
o/$(MODE)/third_party/python/Lib/test/test_wsgiref.py.runs:		\
		private .PLEDGE = stdio rpath wpath cpath fattr proc inet
o/$(MODE)/third_party/python/Lib/test/test_fcntl.py.runs:		\
		private .PLEDGE = stdio rpath wpath cpath fattr proc flock
o/$(MODE)/third_party/python/Lib/test/test_signal.py.runs:		\
		private .PLEDGE = stdio rpath wpath cpath fattr proc flock inet
o/$(MODE)/third_party/python/Lib/test/test_timeout.py.runs:		\
		private .PLEDGE = stdio rpath wpath cpath fattr proc inet

PYTHONTESTER = o/$(MODE)/third_party/python/pythontester.com

o/$(MODE)/third_party/python/Lib/test/test_grammar.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_grammar $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_set.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_set $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_genexps.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_genexps $(PYTESTARGS)

# [jart] incompatible with landlock because it uses current directory for temp files
# o/$(MODE)/third_party/python/Lib/test/test_sqlite.py.runs: $(PYTHONTESTER)
# 	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_sqlite $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_bz2.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_bz2 $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_zlib.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_zlib $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_opcodes.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_opcodes $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_marshal.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_marshal $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_pow.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_pow $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_binascii.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_binascii $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_binhex.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_binhex $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_capi.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_capi $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test__locale.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test__locale $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_binop.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_binop $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test___future__.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test___future__ $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test__opcode.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test__opcode $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_abc.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_abc $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_bytes.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_bytes $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_setcomps.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_setcomps $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_itertools.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_itertools $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_listcomps.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_listcomps $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_aifc.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_aifc $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_audioop.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_audioop $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_bool.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_bool $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_base64.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_base64 $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_baseexception.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_baseexception $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_array.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_array $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_builtin.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_builtin $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_charmapcodec.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_charmapcodec $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_codecs.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_codecs $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_codeop.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_codeop $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_cgi.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_cgi $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_abstract_numbers.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_abstract_numbers $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_augassign.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_augassign $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_bigaddrspace.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_bigaddrspace $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_class.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_class $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_call.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_call $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_buffer.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_buffer $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_bufio.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_bufio $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_enum.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_enum $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_code.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_code $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_cmd.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_cmd $(PYTESTARGS)

# [jart] incompatible with landlock because it uses current directory for temp files
# o/$(MODE)/third_party/python/Lib/test/test_pwd.py.runs: $(PYTHONTESTER)
# 	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_pwd $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_cmath.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_cmath $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_defaultdict.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_defaultdict $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_decorators.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_decorators $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_copy.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_copy $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_csv.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_csv $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_difflib.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_difflib $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_colorsys.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_colorsys $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_compare.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_compare $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_copyreg.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_copyreg $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_collections.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_collections $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_format.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_format $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_fractions.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_fractions $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_eof.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_eof $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_fnmatch.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_fnmatch $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_frame.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_frame $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_dummy_threading.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_dummy_threading $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_dynamic.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_dynamic $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_dict.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_dict $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_wsgiref.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_wsgiref $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_wave.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_wave $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_urlparse.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_urlparse $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_userdict.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_userdict $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_userlist.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_userlist $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_userstring.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_userstring $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_utf8source.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_utf8source $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_uu.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_uu $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_email/test__encoded_words.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_email.test__encoded_words $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_email/test__header_value_parser.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_email.test__header_value_parser $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_email/test_asian_codecs.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_email.test_asian_codecs $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_email/test_contentmanager.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_email.test_contentmanager $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_email/test_defect_handling.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_email.test_defect_handling $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_email/test_email.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_email.test_email $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_email/test_generator.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_email.test_generator $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_email/test_headerregistry.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_email.test_headerregistry $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_email/test_inversion.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_email.test_inversion $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_email/test_message.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_email.test_message $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_email/test_parser.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_email.test_parser $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_email/test_pickleable.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_email.test_pickleable $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_email/test_policy.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_email.test_policy $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_email/test_utils.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_email.test_utils $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_strtod.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_strtod $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_struct.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_struct $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_structmembers.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_structmembers $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_hash.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_hash $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_heapq.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_heapq $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_operator.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_operator $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_optparse.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_optparse $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_finalization.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_finalization $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_enumerate.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_enumerate $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_errno.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_errno $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_html.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_html $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_htmlparser.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_htmlparser $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_http_cookiejar.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_http_cookiejar $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_http_cookies.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_http_cookies $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_list.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_list $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_long.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_long $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_longexp.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_longexp $(PYTESTARGS)

# [jart] incompatible with landlock because it uses current directory for temp files
# o/$(MODE)/third_party/python/Lib/test/test_glob.py.runs: $(PYTHONTESTER)
# 	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_glob $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_global.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_global $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_ipaddress.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_ipaddress $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_isinstance.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_isinstance $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_iter.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_iter $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_json/__main__.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_json $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_tarfile.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_tarfile $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_iterlen.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_iterlen $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_stat.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_stat $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_memoryio.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_memoryio $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_memoryview.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_memoryview $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_metaclass.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_metaclass $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_mimetypes.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_mimetypes $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_kdf.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_kdf $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_cosmo.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_cosmo $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_scratch.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_scratch $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_hashlib.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_hashlib $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_complex.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_complex $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_funcattrs.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_funcattrs $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_functools.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_functools $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_int.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_int $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_int_literal.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_int_literal $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_bisect.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_bisect $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_pyexpat.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_pyexpat $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_ioctl.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_ioctl $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_getopt.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_getopt $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_sort.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_sort $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_slice.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_slice $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_decimal.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_decimal $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_deque.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_deque $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_mmap.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_mmap $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_poll.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_poll $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_robotparser.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_robotparser $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_re.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_re $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_range.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_range $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_sax.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_sax $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_scope.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_scope $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_stringprep.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_stringprep $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_syntax.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_syntax $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_unicodedata.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_unicodedata $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_unpack.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_unpack $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_unpack_ex.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_unpack_ex $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_file.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_file $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_uuid.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_uuid $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_filecmp.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_filecmp $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_fileinput.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_fileinput $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_fileio.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_fileio $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_float.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_float $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_pickle.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_pickle $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_pickletools.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_pickletools $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_tuple.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_tuple $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_reprlib.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_reprlib $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_strftime.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_strftime $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_quopri.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_quopri $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_with.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_with $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_raise.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_raise $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_yield_from.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_yield_from $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_typechecks.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_typechecks $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_types.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_types $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_random.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_random $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_typing.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_typing $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_unary.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_unary $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_print.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_print $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_thread.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_thread $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_threading.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_threading $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_threading_local.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_threading_local $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_threadsignals.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_threadsignals $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_pprint.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_pprint $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_secrets.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_secrets $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_select.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_select $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_selectors.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_selectors $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_contains.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_contains $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_super.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_super $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_unicode.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_unicode $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_unicode_file.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_unicode_file $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_unicode_identifiers.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_unicode_identifiers $(PYTESTARGS)

# [jart] unsupported with landlock right now because exdev renaming
# o/$(MODE)/third_party/python/Lib/test/test_unicode_file_functions.py.runs: $(PYTHONTESTER)
# 	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_unicode_file_functions $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_textwrap.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_textwrap $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_pulldom.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_pulldom $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_minidom.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_minidom $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_xml_dom_minicompat.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_xml_dom_minicompat $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_xml_etree_c.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_xml_etree_c $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_coroutines.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_coroutines $(PYTESTARGS)

# [jart] incompatible with landlock because it uses current directory for temp files
# o/$(MODE)/third_party/python/Lib/test/test_tempfile.py.runs: $(PYTHONTESTER)
# 	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_tempfile $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_normalization.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_normalization $(PYTESTARGS)

# [jart] unsupported with landlock right now because it needs /bin/sh
# o/$(MODE)/third_party/python/Lib/test/test_os.py.runs: $(PYTHONTESTER)
# 	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_os $(PYTESTARGS)

# [jart] incompatible with landlock because it uses current directory for temp files
# o/$(MODE)/third_party/python/Lib/test/test_logging.py.runs: $(PYTHONTESTER)
# 	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_logging $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_io.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_io $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_gzip.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_gzip $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_tracemalloc.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_tracemalloc $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_configparser.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_configparser $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_flufl.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_flufl $(PYTESTARGS)

# [jart] incompatible with landlock because it uses current directory for temp files
# o/$(MODE)/third_party/python/Lib/test/test_keyword.py.runs: $(PYTHONTESTER)
# 	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_keyword $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_keywordonlyarg.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_keywordonlyarg $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_sys.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_sys $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_cgitb.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_cgitb $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_asyncgen.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_asyncgen $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_runpy.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_runpy $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_doctest.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_doctest $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_doctest2.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_doctest2 $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_calendar.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_calendar $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_asynchat.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_asynchat $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_asdl_parser.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_asdl_parser $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_atexit.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_atexit $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_dis.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_dis $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_asyncore.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_asyncore $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_epoll.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_epoll $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_cmd_line.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_cmd_line $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_cmd_line_script.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_cmd_line_script $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_code_module.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_code_module $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_codeccallbacks.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_codeccallbacks $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_codecmaps_cn.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_codecmaps_cn $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_codecmaps_jp.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_codecmaps_jp $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_codecencodings_cn.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_codecencodings_cn $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_codecencodings_hk.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_codecencodings_hk $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_codecmaps_hk.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_codecmaps_hk $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_codecmaps_kr.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_codecmaps_kr $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_codecmaps_tw.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_codecmaps_tw $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_codecencodings_iso2022.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_codecencodings_iso2022 $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_codecencodings_jp.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_codecencodings_jp $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_codecencodings_kr.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_codecencodings_kr $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_codecencodings_tw.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_codecencodings_tw $(PYTESTARGS)

# needs >256kb stack size to run
#o/$(MODE)/third_party/python/Lib/test/test_compile.py.runs: $(PYTHONTESTER)
#	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_compile $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_contextlib.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_contextlib $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_cprofile.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_cprofile $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_crashers.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_crashers $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_crypt.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_crypt $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_datetime.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_datetime $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_descrtut.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_descrtut $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_devpoll.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_devpoll $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_dict_version.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_dict_version $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_dictcomps.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_dictcomps $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_dictviews.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_dictviews $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_dtrace.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_dtrace $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_dynamicclassattribute.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_dynamicclassattribute $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_eintr.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_eintr $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_exception_hierarchy.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_exception_hierarchy $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_xmlrpc_net.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_xmlrpc_net $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_bigmem.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_bigmem $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_exception_variations.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_exception_variations $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_exceptions.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_exceptions $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_time.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_time $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_docxmlrpc.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_docxmlrpc $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_extcall.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_extcall $(PYTESTARGS)

# too slow
#o/$(MODE)/third_party/python/Lib/test/test_faulthandler.py.runs: $(PYTHONTESTER)
#	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_faulthandler $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_fcntl.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_fcntl $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_file_eintr.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_file_eintr $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_fork1.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_fork1 $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_fstring.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_fstring $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_ftplib.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_ftplib $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_future.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_future $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_future3.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_future3 $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_future4.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_future4 $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_future5.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_future5 $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_gc.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_gc $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_gdb.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_gdb $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_generator_stop.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_generator_stop $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_generators.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_generators $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_genericpath.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_genericpath $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_getargs2.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_getargs2 $(PYTESTARGS)

# [jart] incompatible with landlock because it reads /etc/passwd
# o/$(MODE)/third_party/python/Lib/test/test_getpass.py.runs: $(PYTHONTESTER)
# 	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_getpass $(PYTESTARGS)

# [jart] incompatible with landlock because it uses current directory for temp files
# o/$(MODE)/third_party/python/Lib/test/test_gettext.py.runs: $(PYTHONTESTER)
# 	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_gettext $(PYTESTARGS)

# [jart] incompatible with landlock because it reads /etc/passwd
# o/$(MODE)/third_party/python/Lib/test/test_grp.py.runs: $(PYTHONTESTER)
# 	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_grp $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_imaplib.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_imaplib $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_imghdr.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_imghdr $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_imp.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_imp $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_index.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_index $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_kqueue.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_kqueue $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_largefile.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_largefile $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_linecache.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_linecache $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_locale.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_locale $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_macpath.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_macpath $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_macurl2path.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_macurl2path $(PYTESTARGS)

# [jart] unsupported with landlock right now because exdev renaming
# o/$(MODE)/third_party/python/Lib/test/test_mailbox.py.runs: $(PYTHONTESTER)
# 	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_mailbox $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_mailcap.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_mailcap $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_module.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_module $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_modulefinder.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_modulefinder $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_multibytecodec.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_multibytecodec $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_multiprocessing_fork.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_multiprocessing_fork $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_multiprocessing_forkserver.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_multiprocessing_forkserver $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_multiprocessing_main_handling.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_multiprocessing_main_handling $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_multiprocessing_spawn.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_multiprocessing_spawn $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_netrc.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_netrc $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_nis.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_nis $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_nntplib.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_nntplib $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_ntpath.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_ntpath $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_numeric_tower.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_numeric_tower $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_ossaudiodev.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_ossaudiodev $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_parser.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_parser $(PYTESTARGS)

# [jart] unsupported with landlock right now because exdev renaming
# o/$(MODE)/third_party/python/Lib/test/test_pathlib.py.runs: $(PYTHONTESTER)
# 	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_pathlib $(PYTESTARGS)

# [jart] unsupported with landlock right now because it uses current directory for temp files
# o/$(MODE)/third_party/python/Lib/test/test_pdb.py.runs: $(PYTHONTESTER)
# 	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_pdb $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_peepholer.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_peepholer $(PYTESTARGS)

# [jart] unsupported with landlock right now because it needs /bin/sh
# o/$(MODE)/third_party/python/Lib/test/test_pipes.py.runs: $(PYTHONTESTER)
# 	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_pipes $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_pkgimport.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_pkgimport $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_platform.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_platform $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_plistlib.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_plistlib $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_httplib.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_httplib $(PYTESTARGS)

# [jart] unsupported with landlock right now because it needs /bin/sh
# o/$(MODE)/third_party/python/Lib/test/test_popen.py.runs: $(PYTHONTESTER)
# 	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_popen $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_poplib.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_poplib $(PYTESTARGS)

# [jart] incompatible with landlock because it uses current directory for temp files
# o/$(MODE)/third_party/python/Lib/test/test_posix.py.runs: $(PYTHONTESTER)
# 	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_posix $(PYTESTARGS)

# [jart] unsupported with landlock right now because exdev renaming
# o/$(MODE)/third_party/python/Lib/test/test_posixpath.py.runs: $(PYTHONTESTER)
# 	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_posixpath $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_profile.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_profile $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_property.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_property $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_pstats.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_pstats $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_pty.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_pty $(PYTESTARGS)

# [jart] incompatible with landlock because it uses current directory for temp files
# o/$(MODE)/third_party/python/Lib/test/test_py_compile.py.runs: $(PYTHONTESTER)
# 	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_py_compile $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_pyclbr.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_pyclbr $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_pydoc.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_pydoc $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_readline.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_readline $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_regrtest.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_regrtest $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_repl.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_repl $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_richcmp.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_richcmp $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_sched.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_sched $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_script_helper.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_script_helper $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_shlex.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_shlex $(PYTESTARGS)

# [jart] incompatible with landlock because it uses current directory for temp files
# o/$(MODE)/third_party/python/Lib/test/test_shutil.py.runs: $(PYTHONTESTER)
# 	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_shutil $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_signal.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_signal $(PYTESTARGS)

# [jart] incompatible with landlock because it uses current directory for temp files
# o/$(MODE)/third_party/python/Lib/test/test_site.py.runs: $(PYTHONTESTER)
# 	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_site $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_smtpd.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_smtpd $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_smtplib.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_smtplib $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_smtpnet.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_smtpnet $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_sndhdr.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_sndhdr $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_socket.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_socket $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_socketserver.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_socketserver $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_spwd.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_spwd $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_startfile.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_startfile $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_statistics.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_statistics $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_string.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_string $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_string_literals.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_string_literals $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_strptime.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_strptime $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_structseq.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_structseq $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_subclassinit.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_subclassinit $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_subprocess.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_subprocess $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_sunau.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_sunau $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_support.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_support $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_symbol.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_symbol $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_symtable.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_symtable $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_sys_setprofile.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_sys_setprofile $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_syslog.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_syslog $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_telnetlib.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_telnetlib $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_threadedtempfile.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_threadedtempfile $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_timeit.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_timeit $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_timeout.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_timeout $(PYTESTARGS)

# TODO(jart): what's wrong with this since landlock?
# o/$(MODE)/third_party/python/Lib/test/test_tokenize.py.runs: $(PYTHONTESTER)
# 	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_tokenize $(PYTESTARGS)

# [jart] incompatible with landlock because it uses current directory for temp files
# o/$(MODE)/third_party/python/Lib/test/test_trace.py.runs: $(PYTHONTESTER)
# 	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_trace $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_traceback.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_traceback $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_turtle.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_turtle $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_unittest.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_unittest $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_univnewlines.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_univnewlines $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_urllib.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_urllib $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_urllib2.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_urllib2 $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_urllib2_localnet.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_urllib2_localnet $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_urllib2net.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_urllib2net $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_urllib_response.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_urllib_response $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_urllibnet.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_urllibnet $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_wait3.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_wait3 $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_wait4.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_wait4 $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_webbrowser.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_webbrowser $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_xdrlib.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_xdrlib $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_weakref.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_weakref $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_weakset.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_weakset $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_zipapp.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_zipapp $(PYTESTARGS)

# [jart] incompatible with landlock because it uses current directory for temp files
# o/$(MODE)/third_party/python/Lib/test/test_zipimport.py.runs: $(PYTHONTESTER)
# 	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_zipimport $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_zipfile.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_zipfile $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_zipfile64.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_zipfile64 $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/mp_preload.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.mp_preload $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/bisect.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.bisect $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/signalinterproctester.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.signalinterproctester $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/pythoninfo.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.pythoninfo $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/datetimetester.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.datetimetester $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/outstanding_bugs.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.outstanding_bugs $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/sortperf.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.sortperf $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_openpty.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_openpty $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_queue.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_queue $(PYTESTARGS)

o/$(MODE)/third_party/python/Lib/test/test_ordered_dict.py.runs: $(PYTHONTESTER)
	@$(COMPILE) -ACHECK -wtT$@ $(PYHARNESSARGS) $(PYTHONTESTER) -m test.test_ordered_dict $(PYTESTARGS)

################################################################################

o/$(MODE)/third_party/python/pyobj.com.dbg:				\
		$(THIRD_PARTY_PYTHON_STAGE1)				\
		o/$(MODE)/third_party/python/pyobj.o			\
		$(CRT)							\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

o/$(MODE)/third_party/python/pycomp.com.dbg:				\
		$(THIRD_PARTY_PYTHON_STAGE1)				\
		o/$(MODE)/third_party/python/pycomp.o			\
		$(CRT)							\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

o/$(MODE)/third_party/python/repl.com.dbg:				\
		$(THIRD_PARTY_PYTHON_STAGE2)				\
		o/$(MODE)/third_party/python/repl.o			\
		$(CRT)							\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

o/$(MODE)/third_party/python/pystone.com.dbg:				\
		$(THIRD_PARTY_PYTHON_STAGE2)				\
		o/$(MODE)/third_party/python/Lib/test/pystone.o		\
		$(CRT)							\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

o/$(MODE)/third_party/python/Parser/asdl_c.com.dbg:			\
		$(THIRD_PARTY_PYTHON_STAGE2)				\
		o/$(MODE)/third_party/python/Parser/asdl_c.o		\
		$(CRT)							\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

$(THIRD_PARTY_PYTHON_STAGE2_A):						\
		third_party/python/Modules/				\
		third_party/python/Objects/				\
		$(THIRD_PARTY_PYTHON_STAGE2_A).pkg			\
		$(THIRD_PARTY_PYTHON_STAGE2_A_OBJS)

$(THIRD_PARTY_PYTHON_PYTEST_A):						\
		third_party/python/Lib/test/				\
		$(THIRD_PARTY_PYTHON_PYTEST_A).pkg			\
		$(THIRD_PARTY_PYTHON_PYTEST_A_OBJS)

$(THIRD_PARTY_PYTHON_STAGE2_A).pkg:					\
		$(THIRD_PARTY_PYTHON_STAGE2_A_OBJS)			\
		$(foreach x,$(THIRD_PARTY_PYTHON_STAGE2_A_DIRECTDEPS),$($(x)_A).pkg)

$(THIRD_PARTY_PYTHON_PYTEST_A).pkg:					\
		$(THIRD_PARTY_PYTHON_PYTEST_A_OBJS)			\
		$(foreach x,$(THIRD_PARTY_PYTHON_PYTEST_A_DIRECTDEPS),$($(x)_A).pkg)

o/$(MODE)/third_party/python/python-pymains.pkg:			\
		$(THIRD_PARTY_PYTHON_PYTEST_PYMAINS_OBJS)		\
		$(foreach x,$(THIRD_PARTY_PYTHON_PYTEST_PYMAINS_DIRECTDEPS),$($(x)_A).pkg)

# includes need to be in the first 64kb, otherwise...
o/$(MODE)/third_party/python/Modules/posixmodule.o:			\
		third_party/python/Modules/clinic/posixmodule.inc

$(THIRD_PARTY_PYTHON_STAGE1_A_OBJS)					\
$(THIRD_PARTY_PYTHON_STAGE2_A_OBJS): private				\
		CFLAGS +=						\
			-fwrapv						\
			-ffunction-sections				\
			-fdata-sections

o/$(MODE)/third_party/python/Python/ceval.o				\
o/$(MODE)/third_party/python/Objects/object.o				\
o/$(MODE)/third_party/python/Python/graminit.o				\
o/$(MODE)/third_party/python/Objects/abstract.o: private		\
		CFLAGS +=						\
			-fno-function-sections				\
			-fno-data-sections

# use smaller relocations for indirect branches
o/$(MODE)/third_party/python/Modules/_decimal/libmpdec/basearith.o	\
o/$(MODE)/third_party/python/Modules/_decimal/libmpdec/mpdecimal.o	\
o/$(MODE)/third_party/python/Modules/_json.o				\
o/$(MODE)/third_party/python/Modules/_pickle.o				\
o/$(MODE)/third_party/python/Modules/_sre.o				\
o/$(MODE)/third_party/python/Modules/cjkcodecs/_codecs_jp.o		\
o/$(MODE)/third_party/python/Modules/expat/xmlparse.o			\
o/$(MODE)/third_party/python/Modules/expat/xmlrole.o			\
o/$(MODE)/third_party/python/Modules/expat/xmltok.o			\
o/$(MODE)/third_party/python/Objects/memoryobject.o			\
o/$(MODE)/third_party/python/Objects/unicodeobject.o			\
o/$(MODE)/third_party/python/Python/ast.o				\
o/$(MODE)/third_party/python/Python/compile.o: private			\
		CFLAGS +=						\
			-fpie

o//third_party/python/Modules/_decimal/libmpdec/basearith.o		\
o//third_party/python/Modules/_decimal/libmpdec/constants.o		\
o//third_party/python/Modules/_decimal/libmpdec/context.o		\
o//third_party/python/Modules/_decimal/libmpdec/convolute.o		\
o//third_party/python/Modules/_decimal/libmpdec/crt.o			\
o//third_party/python/Modules/_decimal/libmpdec/difradix2.o		\
o//third_party/python/Modules/_decimal/libmpdec/fnt.o			\
o//third_party/python/Modules/_decimal/libmpdec/fourstep.o		\
o//third_party/python/Modules/_decimal/libmpdec/io.o			\
o//third_party/python/Modules/_decimal/libmpdec/memory.o		\
o//third_party/python/Modules/_decimal/libmpdec/mpdecimal.o		\
o//third_party/python/Modules/_decimal/libmpdec/numbertheory.o		\
o//third_party/python/Modules/_decimal/libmpdec/sixstep.o		\
o//third_party/python/Modules/_decimal/libmpdec/transpose.o		\
o//third_party/python/Modules/mathmodule.o				\
o//third_party/python/Modules/hashtable.o				\
o//third_party/python/Objects/abstract.o				\
o//third_party/python/Objects/typeobject.o				\
o//third_party/python/Objects/frameobject.o				\
o//third_party/python/Objects/unicodeobject.o				\
o//third_party/python/Objects/longobject.o				\
o//third_party/python/Objects/dictobject.o				\
o//third_party/python/Objects/obmalloc.o				\
o//third_party/python/Objects/funcobject.o				\
o//third_party/python/Objects/pyhash.o					\
o//third_party/python/Python/ceval.o: private				\
		CFLAGS +=						\
			-O2

$(THIRD_PARTY_PYTHON_STAGE1_A_OBJS)					\
$(THIRD_PARTY_PYTHON_STAGE2_A_OBJS): private				\
		CPPFLAGS +=						\
			-DPy_BUILD_CORE					\
			-DMULTIARCH='"x86_64-cosmo"'

ifneq ($(MODE),dbg)
$(THIRD_PARTY_PYTHON_STAGE1_A_OBJS)					\
$(THIRD_PARTY_PYTHON_STAGE2_A_OBJS): private				\
		CPPFLAGS +=						\
			-DNDEBUG
endif

o/$(MODE)/third_party/python/Python/sysmodule.o: private		\
		CFLAGS +=						\
			-DABIFLAGS='"m"'

# NOTE: Care must be taken that the compiler doesn't try to "optimize"
#       the indirect jumps by sharing them between all opcodes. Such
#       optimizations can be disabled on gcc by using -fno-gcse.
o/$(MODE)/third_party/python/Python/ceval.o: private			\
		CFLAGS +=						\
			-fno-gcse

# Issue #23654: Turn off ICC's tail call optimization for the
#               stack_overflow generator. ICC turns the recursive tail
#               call into a loop. [Let's do GCC too, just to be safe.]
o/$(MODE)/third_party/python/Modules/faulthandler.o: private		\
		CFLAGS +=						\
			-fno-optimize-sibling-calls

o/$(MODE)/third_party/python/Lib/mimetypes.o: private PYFLAGS += -Y.python/mime.types
o/$(MODE)/third_party/python/Lib/test/test_baseexception.o: private PYFLAGS += -Y.python/test/exception_hierarchy.txt
o/$(MODE)/third_party/python/Lib/test/test_cmath.o: private PYFLAGS += -Y.python/test/ieee754.txt
o/$(MODE)/third_party/python/Lib/test/test_difflib.o: private PYFLAGS += -Y.python/test/test_difflib_expect.html

o/$(MODE)/third_party/python/Lib/test/test_cosmo.o: private		\
		PYFLAGS +=						\
			-Y.python/test/hello.com

o/$(MODE)/third_party/python/Lib/test/test_asdl_parser.o: private	\
		PYFLAGS +=						\
			-Y.python/test/Python.asdl

o/$(MODE)/third_party/python/Lib/test/test_math.o: private		\
		PYFLAGS +=						\
			-Y.python/test/ieee754.txt			\
			-Y.python/test/math_testcases.txt		\
			-Y.python/test/cmath_testcases.txt

o/$(MODE)/third_party/python/Lib/test/audiotests.o: private		\
		PYFLAGS +=						\
			-Y.python/test/audiodata/pluck-alaw.aifc	\
			-Y.python/test/audiodata/pluck-alaw.aifc	\
			-Y.python/test/audiodata/pluck-pcm16.aiff	\
			-Y.python/test/audiodata/pluck-pcm24.aiff	\
			-Y.python/test/audiodata/pluck-pcm32.aiff	\
			-Y.python/test/audiodata/pluck-pcm8.aiff	\
			-Y.python/test/audiodata/pluck-pcm8.wav		\
			-Y.python/test/audiodata/pluck-ulaw.aifc	\
			-Y.python/test/Sine-1000Hz-300ms.aif

o/$(MODE)/third_party/python/Lib/test/test_wave.o: private		\
		PYFLAGS +=						\
			-Y.python/test/audiodata/pluck-pcm16.wav	\
			-Y.python/test/audiodata/pluck-pcm24.wav	\
			-Y.python/test/audiodata/pluck-pcm32.wav

o/$(MODE)/third_party/python/Lib/test/test_sax.o: private		\
		PYFLAGS +=						\
			-Y.python/test/xmltestdata/test.xml		\
			-Y.python/test/xmltestdata/test.xml.out		\
			-Y.python/test/xmltestdata/simple.xml		\
			-Y.python/test/xmltestdata/simple-ns.xml

o/$(MODE)/third_party/python/Lib/test/test_float.o: private		\
		PYFLAGS +=						\
			-Y.python/test/floating_points.txt		\
			-Y.python/test/formatfloat_testcases.txt

o/$(MODE)/third_party/python/Lib/test/test_tarfile.o: private		\
		PYFLAGS +=						\
			-Y.python/test/zipdir.zip			\
			-Y.python/test/testtar.tar			\
			-Y.python/test/recursion.tar			\
			-Y.python/test/tokenize_tests.txt		\
			-Y.python/test/tokenize_tests-latin1-coding-cookie-and-utf8-bom-sig.txt \
			-Y.python/test/tokenize_tests-no-coding-cookie-and-utf8-bom-sig-only.txt \
			-Y.python/test/tokenize_tests-utf8-coding-cookie-and-no-utf8-bom-sig.txt \
			-Y.python/test/tokenize_tests-utf8-coding-cookie-and-utf8-bom-sig.txt

o/$(MODE)/third_party/python/Lib/test/test_random.o: private		\
		PYFLAGS +=						\
			-Y.python/test/randv2_32.pck			\
			-Y.python/test/randv2_64.pck			\
			-Y.python/test/randv3.pck

o/$(MODE)/third_party/python/Lib/test/test_pstats.o: private		\
		PYFLAGS +=						\
			-Y.python/test/pstats.pck

o/$(MODE)/third_party/python/Lib/test/test_sunau.o: private		\
		PYFLAGS +=						\
			-Y.python/test/audiodata/pluck-alaw.aifc	\
			-Y.python/test/audiodata/pluck-pcm16.aiff	\
			-Y.python/test/audiodata/pluck-pcm16.au		\
			-Y.python/test/audiodata/pluck-pcm16.wav	\
			-Y.python/test/audiodata/pluck-pcm24.aiff	\
			-Y.python/test/audiodata/pluck-pcm24.au		\
			-Y.python/test/audiodata/pluck-pcm24.wav	\
			-Y.python/test/audiodata/pluck-pcm32.aiff	\
			-Y.python/test/audiodata/pluck-pcm32.au		\
			-Y.python/test/audiodata/pluck-pcm32.wav	\
			-Y.python/test/audiodata/pluck-pcm8.aiff	\
			-Y.python/test/audiodata/pluck-pcm8.au		\
			-Y.python/test/audiodata/pluck-pcm8.wav		\
			-Y.python/test/audiodata/pluck-ulaw.aifc	\
			-Y.python/test/audiodata/pluck-ulaw.au

o/$(MODE)/third_party/python/Lib/test/test_py_compile.o: private	\
		PYFLAGS +=						\
			-Y.python/test/bad_coding2.py

o/$(MODE)/third_party/python/Lib/test/test_tokenize.o: private		\
		PYFLAGS +=						\
			-Y.python/test/bad_coding.py			\
			-Y.python/test/bad_coding2.py

o/$(MODE)/third_party/python/Lib/test/test_doctest.o: private		\
		PYFLAGS +=						\
			-Y.python/test/test_doctest.txt			\
			-Y.python/test/test_doctest2.txt		\
			-Y.python/test/test_doctest3.txt		\
			-Y.python/test/test_doctest4.txt

o/$(MODE)/third_party/python/Lib/test/test_imghdr.o: private		\
		PYFLAGS +=						\
			-Y.python/test/imghdrdata/			\
			-Y.python/test/imghdrdata/python.bmp		\
			-Y.python/test/imghdrdata/python.exr		\
			-Y.python/test/imghdrdata/python.gif		\
			-Y.python/test/imghdrdata/python.jpg		\
			-Y.python/test/imghdrdata/python.pbm		\
			-Y.python/test/imghdrdata/python.pgm		\
			-Y.python/test/imghdrdata/python.png		\
			-Y.python/test/imghdrdata/python.ppm		\
			-Y.python/test/imghdrdata/python.ras		\
			-Y.python/test/imghdrdata/python.sgi		\
			-Y.python/test/imghdrdata/python.tiff		\
			-Y.python/test/imghdrdata/python.webp		\
			-Y.python/test/imghdrdata/python.xbm	

o/$(MODE)/third_party/python/Lib/test/test_sndhdr.o: private		\
		PYFLAGS +=						\
			-Y.python/test/sndhdrdata/	\
			-Y.python/test/sndhdrdata/sndhdr.8svx	\
			-Y.python/test/sndhdrdata/sndhdr.aifc	\
			-Y.python/test/sndhdrdata/sndhdr.aiff	\
			-Y.python/test/sndhdrdata/sndhdr.au	\
			-Y.python/test/sndhdrdata/sndhdr.hcom	\
			-Y.python/test/sndhdrdata/sndhdr.sndt	\
			-Y.python/test/sndhdrdata/sndhdr.voc	\
			-Y.python/test/sndhdrdata/sndhdr.wav

o/$(MODE)/third_party/python/Lib/test/test_email/test_email.o: private	\
		PYFLAGS +=						\
			-Y.python/test/test_email/data/PyBanner048.gif	\
			-Y.python/test/test_email/data/audiotest.au	\
			-Y.python/test/test_email/data/msg_01.txt	\
			-Y.python/test/test_email/data/msg_02.txt	\
			-Y.python/test/test_email/data/msg_03.txt	\
			-Y.python/test/test_email/data/msg_04.txt	\
			-Y.python/test/test_email/data/msg_05.txt	\
			-Y.python/test/test_email/data/msg_06.txt	\
			-Y.python/test/test_email/data/msg_07.txt	\
			-Y.python/test/test_email/data/msg_08.txt	\
			-Y.python/test/test_email/data/msg_09.txt	\
			-Y.python/test/test_email/data/msg_10.txt	\
			-Y.python/test/test_email/data/msg_11.txt	\
			-Y.python/test/test_email/data/msg_12.txt	\
			-Y.python/test/test_email/data/msg_12a.txt	\
			-Y.python/test/test_email/data/msg_13.txt	\
			-Y.python/test/test_email/data/msg_14.txt	\
			-Y.python/test/test_email/data/msg_15.txt	\
			-Y.python/test/test_email/data/msg_16.txt	\
			-Y.python/test/test_email/data/msg_17.txt	\
			-Y.python/test/test_email/data/msg_18.txt	\
			-Y.python/test/test_email/data/msg_19.txt	\
			-Y.python/test/test_email/data/msg_20.txt	\
			-Y.python/test/test_email/data/msg_21.txt	\
			-Y.python/test/test_email/data/msg_22.txt	\
			-Y.python/test/test_email/data/msg_23.txt	\
			-Y.python/test/test_email/data/msg_24.txt	\
			-Y.python/test/test_email/data/msg_25.txt	\
			-Y.python/test/test_email/data/msg_26.txt	\
			-Y.python/test/test_email/data/msg_27.txt	\
			-Y.python/test/test_email/data/msg_28.txt	\
			-Y.python/test/test_email/data/msg_29.txt	\
			-Y.python/test/test_email/data/msg_30.txt	\
			-Y.python/test/test_email/data/msg_31.txt	\
			-Y.python/test/test_email/data/msg_32.txt	\
			-Y.python/test/test_email/data/msg_33.txt	\
			-Y.python/test/test_email/data/msg_34.txt	\
			-Y.python/test/test_email/data/msg_35.txt	\
			-Y.python/test/test_email/data/msg_36.txt	\
			-Y.python/test/test_email/data/msg_37.txt	\
			-Y.python/test/test_email/data/msg_38.txt	\
			-Y.python/test/test_email/data/msg_39.txt	\
			-Y.python/test/test_email/data/msg_40.txt	\
			-Y.python/test/test_email/data/msg_41.txt	\
			-Y.python/test/test_email/data/msg_42.txt	\
			-Y.python/test/test_email/data/msg_43.txt	\
			-Y.python/test/test_email/data/msg_44.txt	\
			-Y.python/test/test_email/data/msg_45.txt	\
			-Y.python/test/test_email/data/msg_46.txt

o/$(MODE)/third_party/python/Lib/test/test_xml_etree_c.o			\
o/$(MODE)/third_party/python/Lib/test/test_minidom.o				\
o/$(MODE)/third_party/python/Lib/test/test_pulldom.o: private			\
		PYFLAGS +=							\
			-Y.python/test/xmltestdata/				\
			-Y.python/test/xmltestdata/simple.xml			\
			-Y.python/test/xmltestdata/simple-ns.xml		\
			-Y.python/test/xmltestdata/expat224_utf8_bug.xml	\
			-Y.python/test/xmltestdata/test.xml.out			\
			-Y.python/test/xmltestdata/test.xml

o/$(MODE)/third_party/python/Lib/test/test_decimal.o: private				\
		PYFLAGS +=								\
			-Y.python/test/decimaltestdata/					\
			-Y.python/test/decimaltestdata/nexttoward.decTest		\
			-Y.python/test/decimaltestdata/ln.decTest			\
			-Y.python/test/decimaltestdata/dqMinMag.decTest			\
			-Y.python/test/decimaltestdata/dqCompareTotalMag.decTest	\
			-Y.python/test/decimaltestdata/dqSameQuantum.decTest		\
			-Y.python/test/decimaltestdata/dqScaleB.decTest			\
			-Y.python/test/decimaltestdata/testall.decTest			\
			-Y.python/test/decimaltestdata/dqCompareSig.decTest		\
			-Y.python/test/decimaltestdata/ddFMA.decTest			\
			-Y.python/test/decimaltestdata/decQuad.decTest			\
			-Y.python/test/decimaltestdata/dqClass.decTest			\
			-Y.python/test/decimaltestdata/nextplus.decTest			\
			-Y.python/test/decimaltestdata/dqNextMinus.decTest		\
			-Y.python/test/decimaltestdata/dqQuantize.decTest		\
			-Y.python/test/decimaltestdata/ddMaxMag.decTest			\
			-Y.python/test/decimaltestdata/ddCompareTotal.decTest		\
			-Y.python/test/decimaltestdata/ddCompareTotalMag.decTest	\
			-Y.python/test/decimaltestdata/abs.decTest			\
			-Y.python/test/decimaltestdata/ddAnd.decTest			\
			-Y.python/test/decimaltestdata/dqRemainderNear.decTest		\
			-Y.python/test/decimaltestdata/powersqrt.decTest		\
			-Y.python/test/decimaltestdata/ddReduce.decTest			\
			-Y.python/test/decimaltestdata/comparetotal.decTest		\
			-Y.python/test/decimaltestdata/ddMax.decTest			\
			-Y.python/test/decimaltestdata/dsEncode.decTest			\
			-Y.python/test/decimaltestdata/compare.decTest			\
			-Y.python/test/decimaltestdata/ddMin.decTest			\
			-Y.python/test/decimaltestdata/dqAdd.decTest			\
			-Y.python/test/decimaltestdata/ddCopySign.decTest		\
			-Y.python/test/decimaltestdata/divideint.decTest		\
			-Y.python/test/decimaltestdata/dqFMA.decTest			\
			-Y.python/test/decimaltestdata/squareroot.decTest		\
			-Y.python/test/decimaltestdata/dqMax.decTest			\
			-Y.python/test/decimaltestdata/dqRemainder.decTest		\
			-Y.python/test/decimaltestdata/dqRotate.decTest			\
			-Y.python/test/decimaltestdata/dqCopySign.decTest		\
			-Y.python/test/decimaltestdata/dqAbs.decTest			\
			-Y.python/test/decimaltestdata/max.decTest			\
			-Y.python/test/decimaltestdata/class.decTest			\
			-Y.python/test/decimaltestdata/copysign.decTest			\
			-Y.python/test/decimaltestdata/power.decTest			\
			-Y.python/test/decimaltestdata/ddMultiply.decTest		\
			-Y.python/test/decimaltestdata/ddDivide.decTest			\
			-Y.python/test/decimaltestdata/ddRemainderNear.decTest		\
			-Y.python/test/decimaltestdata/log10.decTest			\
			-Y.python/test/decimaltestdata/ddPlus.decTest			\
			-Y.python/test/decimaltestdata/dsBase.decTest			\
			-Y.python/test/decimaltestdata/remainderNear.decTest		\
			-Y.python/test/decimaltestdata/ddSubtract.decTest		\
			-Y.python/test/decimaltestdata/dqCompareTotal.decTest		\
			-Y.python/test/decimaltestdata/ddCompare.decTest		\
			-Y.python/test/decimaltestdata/ddNextToward.decTest		\
			-Y.python/test/decimaltestdata/extra.decTest			\
			-Y.python/test/decimaltestdata/dqMaxMag.decTest			\
			-Y.python/test/decimaltestdata/dqCopyAbs.decTest		\
			-Y.python/test/decimaltestdata/ddCopy.decTest			\
			-Y.python/test/decimaltestdata/add.decTest			\
			-Y.python/test/decimaltestdata/dqMinus.decTest			\
			-Y.python/test/decimaltestdata/ddToIntegral.decTest		\
			-Y.python/test/decimaltestdata/scaleb.decTest			\
			-Y.python/test/decimaltestdata/dqShift.decTest			\
			-Y.python/test/decimaltestdata/copyabs.decTest			\
			-Y.python/test/decimaltestdata/ddScaleB.decTest			\
			-Y.python/test/decimaltestdata/copynegate.decTest		\
			-Y.python/test/decimaltestdata/reduce.decTest			\
			-Y.python/test/decimaltestdata/dqEncode.decTest			\
			-Y.python/test/decimaltestdata/minus.decTest			\
			-Y.python/test/decimaltestdata/dqCopyNegate.decTest		\
			-Y.python/test/decimaltestdata/tointegral.decTest		\
			-Y.python/test/decimaltestdata/dqCopy.decTest			\
			-Y.python/test/decimaltestdata/dqMin.decTest			\
			-Y.python/test/decimaltestdata/quantize.decTest			\
			-Y.python/test/decimaltestdata/dqDivideInt.decTest		\
			-Y.python/test/decimaltestdata/shift.decTest			\
			-Y.python/test/decimaltestdata/clamp.decTest			\
			-Y.python/test/decimaltestdata/dqInvert.decTest			\
			-Y.python/test/decimaltestdata/exp.decTest			\
			-Y.python/test/decimaltestdata/dqDivide.decTest			\
			-Y.python/test/decimaltestdata/dqAnd.decTest			\
			-Y.python/test/decimaltestdata/randoms.decTest			\
			-Y.python/test/decimaltestdata/dqLogB.decTest			\
			-Y.python/test/decimaltestdata/ddOr.decTest			\
			-Y.python/test/decimaltestdata/comparetotmag.decTest		\
			-Y.python/test/decimaltestdata/or.decTest			\
			-Y.python/test/decimaltestdata/tointegralx.decTest		\
			-Y.python/test/decimaltestdata/ddLogB.decTest			\
			-Y.python/test/decimaltestdata/dqPlus.decTest			\
			-Y.python/test/decimaltestdata/rounding.decTest			\
			-Y.python/test/decimaltestdata/decDouble.decTest		\
			-Y.python/test/decimaltestdata/samequantum.decTest		\
			-Y.python/test/decimaltestdata/dqMultiply.decTest		\
			-Y.python/test/decimaltestdata/ddShift.decTest			\
			-Y.python/test/decimaltestdata/ddMinus.decTest			\
			-Y.python/test/decimaltestdata/invert.decTest			\
			-Y.python/test/decimaltestdata/ddSameQuantum.decTest		\
			-Y.python/test/decimaltestdata/dqNextToward.decTest		\
			-Y.python/test/decimaltestdata/minmag.decTest			\
			-Y.python/test/decimaltestdata/logb.decTest			\
			-Y.python/test/decimaltestdata/dqBase.decTest			\
			-Y.python/test/decimaltestdata/rotate.decTest			\
			-Y.python/test/decimaltestdata/subtract.decTest			\
			-Y.python/test/decimaltestdata/dqToIntegral.decTest		\
			-Y.python/test/decimaltestdata/ddBase.decTest			\
			-Y.python/test/decimaltestdata/ddQuantize.decTest		\
			-Y.python/test/decimaltestdata/and.decTest			\
			-Y.python/test/decimaltestdata/ddNextMinus.decTest		\
			-Y.python/test/decimaltestdata/inexact.decTest			\
			-Y.python/test/decimaltestdata/ddInvert.decTest			\
			-Y.python/test/decimaltestdata/ddCopyAbs.decTest		\
			-Y.python/test/decimaltestdata/remainder.decTest		\
			-Y.python/test/decimaltestdata/ddEncode.decTest			\
			-Y.python/test/decimaltestdata/ddXor.decTest			\
			-Y.python/test/decimaltestdata/ddCompareSig.decTest		\
			-Y.python/test/decimaltestdata/maxmag.decTest			\
			-Y.python/test/decimaltestdata/dqReduce.decTest			\
			-Y.python/test/decimaltestdata/ddCanonical.decTest		\
			-Y.python/test/decimaltestdata/dqNextPlus.decTest		\
			-Y.python/test/decimaltestdata/xor.decTest			\
			-Y.python/test/decimaltestdata/rescale.decTest			\
			-Y.python/test/decimaltestdata/ddCopyNegate.decTest		\
			-Y.python/test/decimaltestdata/ddRemainder.decTest		\
			-Y.python/test/decimaltestdata/base.decTest			\
			-Y.python/test/decimaltestdata/multiply.decTest			\
			-Y.python/test/decimaltestdata/plus.decTest			\
			-Y.python/test/decimaltestdata/dqCanonical.decTest		\
			-Y.python/test/decimaltestdata/copy.decTest			\
			-Y.python/test/decimaltestdata/ddRotate.decTest			\
			-Y.python/test/decimaltestdata/ddNextPlus.decTest		\
			-Y.python/test/decimaltestdata/dqSubtract.decTest		\
			-Y.python/test/decimaltestdata/ddAdd.decTest			\
			-Y.python/test/decimaltestdata/randomBound32.decTest		\
			-Y.python/test/decimaltestdata/dqXor.decTest			\
			-Y.python/test/decimaltestdata/nextminus.decTest		\
			-Y.python/test/decimaltestdata/dqCompare.decTest		\
			-Y.python/test/decimaltestdata/ddAbs.decTest			\
			-Y.python/test/decimaltestdata/min.decTest			\
			-Y.python/test/decimaltestdata/decSingle.decTest		\
			-Y.python/test/decimaltestdata/fma.decTest			\
			-Y.python/test/decimaltestdata/ddClass.decTest			\
			-Y.python/test/decimaltestdata/ddDivideInt.decTest		\
			-Y.python/test/decimaltestdata/dqOr.decTest			\
			-Y.python/test/decimaltestdata/ddMinMag.decTest			\
			-Y.python/test/decimaltestdata/divide.decTest

o/$(MODE)/third_party/python/Lib/test/test_codecmaps_cn.o: private			\
		PYFLAGS +=								\
			-Y.python/test/EUC-CN.TXT					\
			-Y.python/test/CP936.TXT					\
			-Y.python/test/gb-18030-2000.ucm

o/$(MODE)/third_party/python/Lib/test/test_codecmaps_jp.o: private			\
		PYFLAGS +=								\
			-Y.python/test/CP932.TXT					\
			-Y.python/test/EUC-JP.TXT					\
			-Y.python/test/SHIFTJIS.TXT					\
			-Y.python/test/EUC-JISX0213.TXT					\
			-Y.python/test/SHIFT_JISX0213.TXT

o/$(MODE)/third_party/python/Lib/test/test_codecmaps_hk.o: private			\
		PYFLAGS +=								\
			-Y.python/test/BIG5HKSCS-2004.TXT

o/$(MODE)/third_party/python/Lib/test/test_codecmaps_kr.o: private			\
		PYFLAGS +=								\
			-Y.python/test/CP949.TXT					\
			-Y.python/test/EUC-KR.TXT					\
			-Y.python/test/JOHAB.TXT

o/$(MODE)/third_party/python/Lib/test/test_codecmaps_tw.o: private			\
		PYFLAGS +=								\
			-Y.python/test/BIG5.TXT						\
			-Y.python/test/CP950.TXT

o/$(MODE)/third_party/python/Lib/test/test_codecencodings_tw.o: private			\
		PYFLAGS +=								\
			-Y.python/test/cjkencodings/					\
			-Y.python/test/cjkencodings/big5-utf8.txt			\
			-Y.python/test/cjkencodings/big5.txt

o/$(MODE)/third_party/python/Lib/test/test_codecencodings_kr.o: private			\
		PYFLAGS +=								\
			-Y.python/test/cjkencodings/					\
			-Y.python/test/cjkencodings/cp949-utf8.txt			\
			-Y.python/test/cjkencodings/cp949.txt				\
			-Y.python/test/cjkencodings/euc_kr-utf8.txt			\
			-Y.python/test/cjkencodings/euc_kr.txt				\
			-Y.python/test/cjkencodings/johab-utf8.txt			\
			-Y.python/test/cjkencodings/johab.txt

o/$(MODE)/third_party/python/Lib/test/test_codecencodings_jp.o: private			\
		PYFLAGS +=								\
			-Y.python/test/cjkencodings/					\
			-Y.python/test/cjkencodings/euc_jisx0213-utf8.txt		\
			-Y.python/test/cjkencodings/euc_jisx0213.txt			\
			-Y.python/test/cjkencodings/euc_jp-utf8.txt			\
			-Y.python/test/cjkencodings/euc_jp.txt				\
			-Y.python/test/cjkencodings/euc_kr-utf8.txt			\
			-Y.python/test/cjkencodings/euc_kr.txt				\
			-Y.python/test/cjkencodings/shift_jis-utf8.txt			\
			-Y.python/test/cjkencodings/shift_jis.txt			\
			-Y.python/test/cjkencodings/shift_jisx0213-utf8.txt		\
			-Y.python/test/cjkencodings/shift_jisx0213.txt

o/$(MODE)/third_party/python/Lib/test/test_codecencodings_iso2022.o: private		\
		PYFLAGS +=								\
			-Y.python/test/cjkencodings/					\
			-Y.python/test/cjkencodings/iso2022_jp-utf8.txt			\
			-Y.python/test/cjkencodings/iso2022_jp.txt			\
			-Y.python/test/cjkencodings/iso2022_kr-utf8.txt			\
			-Y.python/test/cjkencodings/iso2022_kr.txt

o/$(MODE)/third_party/python/Lib/test/test_codecencodings_hk.o: private			\
		PYFLAGS +=								\
			-Y.python/test/cjkencodings/					\
			-Y.python/test/cjkencodings/big5hkscs-utf8.txt			\
			-Y.python/test/cjkencodings/big5hkscs.txt

o/$(MODE)/third_party/python/Lib/test/test_codecencodings_cn.o: private			\
		PYFLAGS +=								\
			-Y.python/test/cjkencodings/					\
			-Y.python/test/cjkencodings/gb18030-utf8.txt			\
			-Y.python/test/cjkencodings/gb18030.txt				\
			-Y.python/test/cjkencodings/gb2312-utf8.txt			\
			-Y.python/test/cjkencodings/gb2312.txt				\
			-Y.python/test/cjkencodings/gbk-utf8.txt			\
			-Y.python/test/cjkencodings/gbk.txt				\
			-Y.python/test/cjkencodings/hz-utf8.txt				\
			-Y.python/test/cjkencodings/hz.txt

$(THIRD_PARTY_PYTHON_STAGE2_A_PYS_OBJS): private PYFLAGS += -P.python -C3
$(THIRD_PARTY_PYTHON_STAGE2_A_DATA_OBJS): private ZIPOBJ_FLAGS += -P.python -C3
$(THIRD_PARTY_PYTHON_PYTEST_A_PYS_OBJS): private PYFLAGS += -P.python -C3
$(THIRD_PARTY_PYTHON_PYTEST_A_DATA_OBJS): private ZIPOBJ_FLAGS += -P.python -C3

o/$(MODE)/third_party/python/Python/ceval.o: private QUOTA = -C64 -L300
o/$(MODE)/third_party/python/Objects/unicodeobject.o: private QUOTA += -C64 -L300

o/$(MODE)/third_party/python/Objects/unicodeobject.o:			\
		third_party/python/Objects/unicodeobject.c		\
		third_party/python/Objects/stringlib/localeutil.inc	\
		third_party/python/Objects/stringlib/unicode_format.inc	\
		third_party/python/Objects/stringlib/asciilib.inc	\
		third_party/python/Objects/stringlib/codecs.inc		\
		third_party/python/Objects/stringlib/undef.inc		\
		third_party/python/Objects/stringlib/ucs1lib.inc	\
		third_party/python/Objects/stringlib/codecs.inc		\
		third_party/python/Objects/stringlib/undef.inc		\
		third_party/python/Objects/stringlib/ucs2lib.inc	\
		third_party/python/Objects/stringlib/codecs.inc		\
		third_party/python/Objects/stringlib/undef.inc		\
		third_party/python/Objects/stringlib/ucs4lib.inc	\
		third_party/python/Objects/stringlib/codecs.inc		\
		third_party/python/Objects/stringlib/undef.inc

o/$(MODE)/third_party/python/Modules/_elementtree.o:			\
		third_party/python/Modules/_elementtree.c		\
		third_party/python/Modules/clinic/_elementtree.inc

o/$(MODE)/third_party/python/Modules/_io/bufferedio.o:			\
		third_party/python/Modules/_io/bufferedio.c		\
		third_party/python/Modules/_io/clinic/bufferedio.inc

o/$(MODE)/third_party/python/Modules/_io/textio.o:			\
		third_party/python/Modules/_io/textio.c			\
		third_party/python/Modules/_io/clinic/textio.inc

o/$(MODE)/third_party/python/Modules/_sre.o:				\
		third_party/python/Modules/_sre.c			\
		third_party/python/Modules/clinic/_sre.inc

o/$(MODE)/third_party/python/Parser/asdl_c.o: private PYFLAGS += -m
$(THIRD_PARTY_PYTHON_PYTEST_PYMAINS_OBJS): private PYFLAGS += -t -P.python -C3
$(THIRD_PARTY_PYTHON_PYTEST_TODOS:%.py=o/$(MODE)/%.o): private PYFLAGS += -t -P.python -C3
o/$(MODE)/third_party/python/Lib/test/pystone.o: private PYFLAGS += -m -O2 -P.python -C4

o/$(MODE)/third_party/python/Lib/test/test_long.py.runs: private QUOTA = -C64 -L180
o/$(MODE)/third_party/python/Lib/test/test_hash.py.runs: private QUOTA = -C64
o/$(MODE)/third_party/python/Lib/test/test_exceptions.py.runs: private QUOTA = -C64
o/$(MODE)/third_party/python/Lib/test/test_decimal.py.runs: private QUOTA = -C64 -L300
o/$(MODE)/third_party/python/Lib/test/test_unicode.py.runs: private QUOTA = -L300
o/$(MODE)/third_party/python/Lib/test/test_unicodedata.py.runs: private QUOTA = -C64 -L300
o/$(MODE)/third_party/python/Lib/test/test_tarfile.py.runs: private QUOTA = -L300 -C64
o/$(MODE)/third_party/python/Lib/test/test_sqlite.py.runs: private QUOTA = -L120
o/$(MODE)/third_party/python/Lib/test/test_gzip.py.runs: private QUOTA = -L120
o/$(MODE)/third_party/python/Lib/test/test_email/test_email.py.runs: private QUOTA = -C32
o/$(MODE)/third_party/python/Lib/test/test_selectors.py.runs: private QUOTA = -L180
o/$(MODE)/third_party/python/Lib/test/test_trace.py.runs: private QUOTA = -L300
o/$(MODE)/third_party/python/Lib/test/test_multibytecodec.py.runs: private QUOTA = -C128 -L600 -L300
o/$(MODE)/third_party/python/Lib/test/test_bz2.py.runs: private QUOTA = -C128 -L600 -L300
o/$(MODE)/third_party/python/Lib/test/test_bytes.py.runs: private QUOTA = -L300
o/$(MODE)/third_party/python/Lib/test/test_urlparse.py.runs: private QUOTA = -L300
o/$(MODE)/third_party/python/Modules/_decimal/libmpdec/mpdecimal.o: private QUOTA = -L180
o/$(MODE)/third_party/python/Modules/_sre.o: private QUOTA = -L180
o/dbg/third_party/python/Lib/test/test_set.py.runs: private QUOTA = -L300

THIRD_PARTY_PYTHON_LIBS =						\
	$(foreach x,$(THIRD_PARTY_PYTHON_ARTIFACTS),$($(x)))

THIRD_PARTY_PYTHON_OBJS =						\
	$(foreach x,$(THIRD_PARTY_PYTHON_ARTIFACTS),$($(x)_OBJS))	\
	o/$(MODE)/third_party/python/pyobj.o				\
	o/$(MODE)/third_party/python/pycomp.o

THIRD_PARTY_PYTHON_SRCS =						\
	$(foreach x,$(THIRD_PARTY_PYTHON_ARTIFACTS),$($(x)_SRCS))	\
	third_party/python/pyobj.c					\
	third_party/python/pycomp.c					\
	third_party/python/repl.c					\
	third_party/python/pythontester.c

################################################################################
# PYTHON.COM

THIRD_PARTY_PYTHON_PYTHON_SRCS = third_party/python/python.c
THIRD_PARTY_PYTHON_PYTHON_OBJS = o/$(MODE)/third_party/python/python.o
THIRD_PARTY_PYTHON_PYTHON_COMS = o/$(MODE)/third_party/python/python.com
THIRD_PARTY_PYTHON_PYTHON_BINS = $(THIRD_PARTY_PYTHON_PYTHON_COMS) $(THIRD_PARTY_PYTHON_PYTHON_COMS:%=%.dbg)
THIRD_PARTY_PYTHON_PYTHON_DEPS = $(call uniq,$(foreach x,$(THIRD_PARTY_PYTHON_PYTHON_DIRECTDEPS),$($(x))))
THIRD_PARTY_PYTHON_PYTHON_DIRECTDEPS =					\
	LIBC_CALLS							\
	LIBC_FMT							\
	LIBC_INTRIN							\
	LIBC_NEXGEN32E							\
	LIBC_RUNTIME							\
	LIBC_STDIO							\
	LIBC_MEM							\
	LIBC_STR							\
	LIBC_LOG							\
	LIBC_SYSV							\
	LIBC_X								\
	THIRD_PARTY_GETOPT						\
	THIRD_PARTY_LINENOISE						\
	THIRD_PARTY_PYTHON_STAGE1					\
	THIRD_PARTY_PYTHON_STAGE2					\
	THIRD_PARTY_PYTHON_PYTEST					\
	THIRD_PARTY_XED							\
	TOOL_ARGS

o/$(MODE)/third_party/python/python.pkg:				\
		$(THIRD_PARTY_PYTHON_PYTHON_OBJS)			\
		$(foreach x,$(THIRD_PARTY_PYTHON_PYTHON_DIRECTDEPS),$($(x)_A).pkg)

o/$(MODE)/third_party/python/python.com.dbg:				\
		o/$(MODE)/third_party/python/python.pkg			\
		$(THIRD_PARTY_PYTHON_PYTHON_DEPS)			\
		$(THIRD_PARTY_PYTHON_PYTHON_OBJS)			\
		$(CRT)							\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

o/$(MODE)/third_party/python/python.com:				\
		o/$(MODE)/third_party/python/python.com.dbg		\
		o/$(MODE)/third_party/zip/zip.com			\
		o/$(MODE)/tool/build/symtab.com
	@$(MAKE_OBJCOPY)
	@$(MAKE_SYMTAB_CREATE)
	@$(MAKE_SYMTAB_ZIP)

################################################################################
# FREEZE.COM

THIRD_PARTY_PYTHON_FREEZE_SRCS = third_party/python/freeze.c
THIRD_PARTY_PYTHON_FREEZE_OBJS = o/$(MODE)/third_party/python/freeze.o
THIRD_PARTY_PYTHON_FREEZE_COMS = o/$(MODE)/third_party/python/freeze.com
THIRD_PARTY_PYTHON_FREEZE_BINS = $(THIRD_PARTY_PYTHON_FREEZE_COMS) $(THIRD_PARTY_PYTHON_FREEZE_COMS:%=%.dbg)
THIRD_PARTY_PYTHON_FREEZE_DEPS = $(call uniq,$(foreach x,$(THIRD_PARTY_PYTHON_FREEZE_DIRECTDEPS),$($(x))))
THIRD_PARTY_PYTHON_FREEZE_DIRECTDEPS =					\
	LIBC_CALLS							\
	LIBC_FMT							\
	LIBC_INTRIN							\
	LIBC_NEXGEN32E							\
	LIBC_RUNTIME							\
	LIBC_STDIO							\
	LIBC_MEM							\
	LIBC_STR							\
	LIBC_LOG							\
	LIBC_SYSV							\
	LIBC_X								\
	THIRD_PARTY_GETOPT						\
	THIRD_PARTY_XED							\
	THIRD_PARTY_PYTHON_STAGE1

o/$(MODE)/third_party/python/freeze.pkg:				\
		$(THIRD_PARTY_PYTHON_FREEZE_OBJS)			\
		$(foreach x,$(THIRD_PARTY_PYTHON_FREEZE_DIRECTDEPS),$($(x)_A).pkg)

o/$(MODE)/third_party/python/freeze.com.dbg:				\
		o/$(MODE)/third_party/python/freeze.pkg			\
		$(THIRD_PARTY_PYTHON_FREEZE_DEPS)			\
		$(THIRD_PARTY_PYTHON_FREEZE_OBJS)			\
		$(CRT)							\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

.PRECIOUS: o/$(MODE)/third_party/python/chibicc.inc
o/$(MODE)/third_party/python/chibicc.inc:				\
		third_party/python/chibicc.inc				\
		libc/assert.h						\
		libc/intrin/likely.h					\
		libc/calls/struct/stat.h				\
		libc/calls/struct/timespec.h				\
		libc/dce.h						\
		libc/errno.h						\
		libc/fmt/fmts.h						\
		libc/fmt/pflink.h					\
		libc/integral/c.inc					\
		libc/integral/lp64arg.inc				\
		libc/integral/normalize.inc				\
		libc/limits.h						\
		libc/math.h						\
		libc/mem/mem.h						\
		libc/nexgen32e/kcpuids.h				\
		libc/runtime/runtime.h					\
		libc/stdio/stdio.h					\
		libc/str/str.h						\
		libc/str/unicode.h					\
		third_party/python/Include/Python.h			\
		third_party/python/Include/abstract.h			\
		third_party/python/Include/bltinmodule.h		\
		third_party/python/Include/boolobject.h			\
		third_party/python/Include/bytearrayobject.h		\
		third_party/python/Include/bytesobject.h		\
		third_party/python/Include/cellobject.h			\
		third_party/python/Include/ceval.h			\
		third_party/python/Include/classobject.h		\
		third_party/python/Include/code.h			\
		third_party/python/Include/codecs.h			\
		third_party/python/Include/compile.h			\
		third_party/python/Include/complexobject.h		\
		third_party/python/Include/descrobject.h		\
		third_party/python/Include/dictobject.h			\
		third_party/python/Include/dtoa.h			\
		third_party/python/Include/dynamic_annotations.h	\
		third_party/python/Include/enumobject.h			\
		third_party/python/Include/eval.h			\
		third_party/python/Include/fileobject.h			\
		third_party/python/Include/fileutils.h			\
		third_party/python/Include/floatobject.h		\
		third_party/python/Include/funcobject.h			\
		third_party/python/Include/genobject.h			\
		third_party/python/Include/import.h			\
		third_party/python/Include/intrcheck.h			\
		third_party/python/Include/iterobject.h			\
		third_party/python/Include/listobject.h			\
		third_party/python/Include/longintrepr.h		\
		third_party/python/Include/longobject.h			\
		third_party/python/Include/memoryobject.h		\
		third_party/python/Include/methodobject.h		\
		third_party/python/Include/modsupport.h			\
		third_party/python/Include/moduleobject.h		\
		third_party/python/Include/namespaceobject.h		\
		third_party/python/Include/object.h			\
		third_party/python/Include/objimpl.h			\
		third_party/python/Include/odictobject.h		\
		third_party/python/Include/op.h				\
		third_party/python/Include/osmodule.h			\
		third_party/python/Include/patchlevel.h			\
		third_party/python/Include/pyarena.h			\
		third_party/python/Include/pyatomic.h			\
		third_party/python/Include/pycapsule.h			\
		third_party/python/Include/pyctype.h			\
		third_party/python/Include/pydebug.h			\
		third_party/python/Include/pyerrors.h			\
		third_party/python/Include/pyfpe.h			\
		third_party/python/Include/pyhash.h			\
		third_party/python/Include/pylifecycle.h		\
		third_party/python/Include/pymacro.h			\
		third_party/python/Include/pymath.h			\
		third_party/python/Include/pymem.h			\
		third_party/python/Include/pyport.h			\
		third_party/python/Include/pystate.h			\
		third_party/python/Include/pystrcmp.h			\
		third_party/python/Include/pystrtod.h			\
		third_party/python/Include/pythonrun.h			\
		third_party/python/Include/pytime.h			\
		third_party/python/Include/rangeobject.h		\
		third_party/python/Include/setobject.h			\
		third_party/python/Include/sliceobject.h		\
		third_party/python/Include/structseq.h			\
		third_party/python/Include/sysmodule.h			\
		third_party/python/Include/traceback.h			\
		third_party/python/Include/tupleobject.h		\
		third_party/python/Include/typeslots.h			\
		third_party/python/Include/unicodeobject.h		\
		third_party/python/Include/warnings.h			\
		third_party/python/Include/weakrefobject.h		\
		third_party/python/pyconfig.h
	@$(COMPILE) -wACHECK.h $(COMPILE.c) -xc -E -P -fdirectives-only -dD -D__chibicc__ -o $@ $<

################################################################################
# HELLO.COM

THIRD_PARTY_PYTHON_HELLO_SRCS = third_party/python/hello.c
THIRD_PARTY_PYTHON_HELLO_OBJS = o/$(MODE)/third_party/python/hello.o
THIRD_PARTY_PYTHON_HELLO_COMS = o/$(MODE)/third_party/python/hello.com
THIRD_PARTY_PYTHON_HELLO_BINS = $(THIRD_PARTY_PYTHON_HELLO_COMS) $(THIRD_PARTY_PYTHON_HELLO_COMS:%=%.dbg)
THIRD_PARTY_PYTHON_HELLO_DEPS = $(call uniq,$(foreach x,$(THIRD_PARTY_PYTHON_HELLO_DIRECTDEPS),$($(x))))
THIRD_PARTY_PYTHON_HELLO_DIRECTDEPS =					\
	THIRD_PARTY_PYTHON_STAGE1					\
	THIRD_PARTY_PYTHON_STAGE2

o/$(MODE)/third_party/python/hello.pkg:					\
		$(THIRD_PARTY_PYTHON_HELLO_OBJS)			\
		$(foreach x,$(THIRD_PARTY_PYTHON_HELLO_DIRECTDEPS),$($(x)_A).pkg)

o/$(MODE)/third_party/python/hello.com.dbg:				\
		o/$(MODE)/third_party/python/hello.pkg			\
		$(THIRD_PARTY_PYTHON_HELLO_DEPS)			\
		$(THIRD_PARTY_PYTHON_HELLO_OBJS)			\
		$(CRT)							\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

$(THIRD_PARTY_PYTHON_HELLO_OBJS): private PYFLAGS += -C2 -m

# this directory entry is at the tip of the tree
# therefore building it requires special care
o/$(MODE)/third_party/python/Lib/.zip.o: third_party/python/.python
	@$(COMPILE) -wAZIPOBJ $(ZIPOBJ) -a$(ARCH) -C2 $(OUTPUT_OPTION) third_party/python/.python

# these need to be explictly defined because landlock make won't sandbox
# prerequisites with a trailing slash.
o/$(MODE)/third_party/python/Lib/pydoc_data/.zip.o:			\
		third_party/python/Lib/pydoc_data
o/$(MODE)/third_party/python/Lib/test/xmltestdata/.zip.o:		\
		third_party/python/Lib/test/xmltestdata
o/$(MODE)/third_party/python/Lib/test/sndhdrdata/.zip.o:		\
		third_party/python/Lib/test/sndhdrdata
o/$(MODE)/third_party/python/Lib/test/imghdrdata/.zip.o:		\
		third_party/python/Lib/test/imghdrdata
o/$(MODE)/third_party/python/Lib/test/decimaltestdata/.zip.o:		\
		third_party/python/Lib/test/decimaltestdata
o/$(MODE)/third_party/python/Lib/test/dtracedata/.zip.o:		\
		third_party/python/Lib/test/dtracedata
o/$(MODE)/third_party/python/Lib/test/cjkencodings/.zip.o:		\
		third_party/python/Lib/test/cjkencodings
o/$(MODE)/third_party/python/Modules/.zip.o:				\
		third_party/python/Modules
o/$(MODE)/third_party/python/Objects/.zip.o:				\
		third_party/python/Objects
o/$(MODE)/third_party/python/Lib/test/.zip.o:				\
		third_party/python/Lib/test

################################################################################

.PHONY: o/$(MODE)/third_party/python
o/$(MODE)/third_party/python:						\
		$(THIRD_PARTY_PYTHON_BINS)				\
		$(THIRD_PARTY_PYTHON_CHECKS)
