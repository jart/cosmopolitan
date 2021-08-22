/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=4 sts=4 sw=4 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Python 3                                                                     │
│ https://docs.python.org/3/license.html                                       │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/bits/bits.h"
#include "libc/calls/calls.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/log/check.h"
#include "libc/log/log.h"
#include "libc/mem/mem.h"
#include "libc/runtime/gc.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/symbols.internal.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/fileno.h"
#include "libc/sysv/consts/sig.h"
#include "libc/unicode/locale.h"
#include "libc/x/x.h"
#include "third_party/linenoise/linenoise.h"
#include "third_party/python/Include/abstract.h"
#include "third_party/python/Include/ceval.h"
#include "third_party/python/Include/dictobject.h"
#include "third_party/python/Include/fileutils.h"
#include "third_party/python/Include/funcobject.h"
#include "third_party/python/Include/import.h"
#include "third_party/python/Include/listobject.h"
#include "third_party/python/Include/moduleobject.h"
#include "third_party/python/Include/object.h"
#include "third_party/python/Include/pydebug.h"
#include "third_party/python/Include/pyerrors.h"
#include "third_party/python/Include/pylifecycle.h"
#include "third_party/python/Include/pymem.h"
#include "third_party/python/Include/pyport.h"
#include "third_party/python/Include/pythonrun.h"
#include "third_party/python/Include/unicodeobject.h"
#include "third_party/python/Include/yoink.h"
/* clang-format off */

STATIC_YOINK(".python/");
PYTHON_YOINK(".python/__future__.py");
PYTHON_YOINK(".python/_bootlocale.py");
PYTHON_YOINK(".python/_collections_abc.py");
PYTHON_YOINK(".python/_compat_pickle.py");
PYTHON_YOINK(".python/_compression.py");
PYTHON_YOINK(".python/_dummy_thread.py");
PYTHON_YOINK(".python/_markupbase.py");
PYTHON_YOINK(".python/_osx_support.py");
PYTHON_YOINK(".python/_pyio.py");
PYTHON_YOINK(".python/_sitebuiltins.py");
PYTHON_YOINK(".python/_strptime.py");
PYTHON_YOINK(".python/_sysconfigdata_m_cosmo_x86_64-cosmo.py");
PYTHON_YOINK(".python/_threading_local.py");
PYTHON_YOINK(".python/_weakrefset.py");
PYTHON_YOINK(".python/abc.py");
PYTHON_YOINK(".python/argparse.py");
PYTHON_YOINK(".python/ast.py");
PYTHON_YOINK(".python/base64.py");
PYTHON_YOINK(".python/bdb.py");
PYTHON_YOINK(".python/binhex.py");
PYTHON_YOINK(".python/bisect.py");
PYTHON_YOINK(".python/calendar.py");
PYTHON_YOINK(".python/chunk.py");
PYTHON_YOINK(".python/cmd.py");
PYTHON_YOINK(".python/code.py");
PYTHON_YOINK(".python/codecs.py");
PYTHON_YOINK(".python/colorsys.py");
PYTHON_YOINK(".python/configparser.py");
PYTHON_YOINK(".python/contextlib.py");
PYTHON_YOINK(".python/copy.py");
PYTHON_YOINK(".python/copyreg.py");
PYTHON_YOINK(".python/csv.py");
PYTHON_YOINK(".python/datetime.py");
PYTHON_YOINK(".python/decimal.py");
PYTHON_YOINK(".python/difflib.py");
PYTHON_YOINK(".python/doctest.py");
PYTHON_YOINK(".python/dummy_threading.py");
PYTHON_YOINK(".python/threading.py");
PYTHON_YOINK(".python/enum.py");
PYTHON_YOINK(".python/filecmp.py");
PYTHON_YOINK(".python/fileinput.py");
PYTHON_YOINK(".python/fnmatch.py");
PYTHON_YOINK(".python/formatter.py");
PYTHON_YOINK(".python/fractions.py");
PYTHON_YOINK(".python/functools.py");
PYTHON_YOINK(".python/genericpath.py");
PYTHON_YOINK(".python/getopt.py");
PYTHON_YOINK(".python/getpass.py");
PYTHON_YOINK(".python/gettext.py");
PYTHON_YOINK(".python/glob.py");
PYTHON_YOINK(".python/hashlib.py");
PYTHON_YOINK(".python/heapq.py");
PYTHON_YOINK(".python/hmac.py");
PYTHON_YOINK(".python/imghdr.py");
PYTHON_YOINK(".python/imp.py");
PYTHON_YOINK(".python/io.py");
PYTHON_YOINK(".python/ipaddress.py");
PYTHON_YOINK(".python/keyword.py");
PYTHON_YOINK(".python/linecache.py");
PYTHON_YOINK(".python/locale.py");
PYTHON_YOINK(".python/macpath.py");
PYTHON_YOINK(".python/macurl2path.py");
PYTHON_YOINK(".python/mimetypes.py");
PYTHON_YOINK(".python/modulefinder.py");
PYTHON_YOINK(".python/netrc.py");
PYTHON_YOINK(".python/ntpath.py");
PYTHON_YOINK(".python/nturl2path.py");
PYTHON_YOINK(".python/numbers.py");
PYTHON_YOINK(".python/opcode.py");
PYTHON_YOINK(".python/operator.py");
PYTHON_YOINK(".python/optparse.py");
PYTHON_YOINK(".python/os.py");
PYTHON_YOINK(".python/pathlib.py");
PYTHON_YOINK(".python/pickle.py");
PYTHON_YOINK(".python/pickletools.py");
PYTHON_YOINK(".python/pipes.py");
PYTHON_YOINK(".python/pkgutil.py");
PYTHON_YOINK(".python/platform.py");
PYTHON_YOINK(".python/plistlib.py");
PYTHON_YOINK(".python/posixpath.py");
PYTHON_YOINK(".python/pprint.py");
PYTHON_YOINK(".python/pstats.py");
PYTHON_YOINK(".python/pty.py");
PYTHON_YOINK(".python/pyclbr.py");
PYTHON_YOINK(".python/queue.py");
PYTHON_YOINK(".python/quopri.py");
PYTHON_YOINK(".python/random.py");
PYTHON_YOINK(".python/re.py");
PYTHON_YOINK(".python/reprlib.py");
PYTHON_YOINK(".python/runpy.py");
PYTHON_YOINK(".python/sched.py");
PYTHON_YOINK(".python/secrets.py");
PYTHON_YOINK(".python/selectors.py");
PYTHON_YOINK(".python/shelve.py");
PYTHON_YOINK(".python/shlex.py");
PYTHON_YOINK(".python/shutil.py");
PYTHON_YOINK(".python/signal.py");
PYTHON_YOINK(".python/site.py");
PYTHON_YOINK(".python/sndhdr.py");
PYTHON_YOINK(".python/socket.py");
PYTHON_YOINK(".python/socketserver.py");
PYTHON_YOINK(".python/sre_compile.py");
PYTHON_YOINK(".python/sre_constants.py");
PYTHON_YOINK(".python/sre_parse.py");
PYTHON_YOINK(".python/stat.py");
PYTHON_YOINK(".python/statistics.py");
PYTHON_YOINK(".python/string.py");
PYTHON_YOINK(".python/stringprep.py");
PYTHON_YOINK(".python/struct.py");
PYTHON_YOINK(".python/subprocess.py");
PYTHON_YOINK(".python/symbol.py");
PYTHON_YOINK(".python/symtable.py");
PYTHON_YOINK(".python/sysconfig.py");
PYTHON_YOINK(".python/tabnanny.py");
PYTHON_YOINK(".python/tempfile.py");
PYTHON_YOINK(".python/textwrap.py");
PYTHON_YOINK(".python/this.py");
PYTHON_YOINK(".python/token.py");
PYTHON_YOINK(".python/tokenize.py");
PYTHON_YOINK(".python/trace.py");
PYTHON_YOINK(".python/traceback.py");
PYTHON_YOINK(".python/tty.py");
PYTHON_YOINK(".python/types.py");
PYTHON_YOINK(".python/typing.py");
PYTHON_YOINK(".python/uu.py");
PYTHON_YOINK(".python/uuid.py");
PYTHON_YOINK(".python/warnings.py");
PYTHON_YOINK(".python/weakref.py");
PYTHON_YOINK(".python/webbrowser.py");
PYTHON_YOINK(".python/xdrlib.py");

#if !IsTiny()
PYTHON_YOINK(".python/aifc.py");
PYTHON_YOINK(".python/wave.py");
PYTHON_YOINK(".python/sunau.py");
#endif

#if !IsTiny()
PYTHON_YOINK(".python/dis.py");
PYTHON_YOINK(".python/codeop.py");
PYTHON_YOINK(".python/compileall.py");
PYTHON_YOINK(".python/py_compile.py");
#endif

#if !IsTiny()
PYTHON_YOINK(".python/cgi.py");
PYTHON_YOINK(".python/pdb.py");
PYTHON_YOINK(".python/cgitb.py");
PYTHON_YOINK(".python/pydoc.py");
PYTHON_YOINK(".python/timeit.py");
PYTHON_YOINK(".python/profile.py");
PYTHON_YOINK(".python/inspect.py");
PYTHON_YOINK(".python/cProfile.py");
PYTHON_YOINK(".python/tracemalloc.py");
#endif

#if !IsTiny()
PYTHON_YOINK(".python/bz2.py");
PYTHON_YOINK(".python/ssl.py");
PYTHON_YOINK(".python/gzip.py");
PYTHON_YOINK(".python/lzma.py");
PYTHON_YOINK(".python/crypt.py");
PYTHON_YOINK(".python/zipapp.py");
PYTHON_YOINK(".python/ftplib.py");
PYTHON_YOINK(".python/tarfile.py");
PYTHON_YOINK(".python/zipfile.py");
PYTHON_YOINK(".python/telnetlib.py");
PYTHON_YOINK(".python/antigravity.py");
PYTHON_YOINK(".python/rlcompleter.py");
#endif

STATIC_YOINK(".python/collections/");
PYTHON_YOINK(".python/collections/__init__.py");
PYTHON_YOINK(".python/collections/abc.py");

STATIC_YOINK(".python/json/");
PYTHON_YOINK(".python/json/__init__.py");
PYTHON_YOINK(".python/json/decoder.py");
PYTHON_YOINK(".python/json/encoder.py");
PYTHON_YOINK(".python/json/scanner.py");
PYTHON_YOINK(".python/json/tool.py");

STATIC_YOINK(".python/html/");
PYTHON_YOINK(".python/html/__init__.py");
PYTHON_YOINK(".python/html/entities.py");
PYTHON_YOINK(".python/html/parser.py");

STATIC_YOINK(".python/http/");
PYTHON_YOINK(".python/http/__init__.py");
PYTHON_YOINK(".python/http/client.py");
PYTHON_YOINK(".python/http/cookiejar.py");
PYTHON_YOINK(".python/http/cookies.py");
PYTHON_YOINK(".python/http/server.py");

STATIC_YOINK(".python/importlib/");
PYTHON_YOINK(".python/importlib/__init__.py");
PYTHON_YOINK(".python/importlib/_bootstrap.py");
PYTHON_YOINK(".python/importlib/_bootstrap_external.py");
PYTHON_YOINK(".python/importlib/abc.py");
PYTHON_YOINK(".python/importlib/machinery.py");
PYTHON_YOINK(".python/importlib/util.py");

STATIC_YOINK(".python/logging/");
PYTHON_YOINK(".python/logging/__init__.py");
PYTHON_YOINK(".python/logging/config.py");
PYTHON_YOINK(".python/logging/handlers.py");

STATIC_YOINK(".python/urllib/");
PYTHON_YOINK(".python/urllib/__init__.py");
PYTHON_YOINK(".python/urllib/error.py");
PYTHON_YOINK(".python/urllib/parse.py");
PYTHON_YOINK(".python/urllib/request.py");
PYTHON_YOINK(".python/urllib/response.py");
PYTHON_YOINK(".python/urllib/robotparser.py");

STATIC_YOINK(".python/wsgiref/");
PYTHON_YOINK(".python/wsgiref/__init__.py");
PYTHON_YOINK(".python/wsgiref/handlers.py");
PYTHON_YOINK(".python/wsgiref/headers.py");
PYTHON_YOINK(".python/wsgiref/simple_server.py");
PYTHON_YOINK(".python/wsgiref/util.py");
PYTHON_YOINK(".python/wsgiref/validate.py");

#if !IsTiny()
STATIC_YOINK(".python/sqlite3/");
PYTHON_YOINK(".python/sqlite3/__init__.py");
PYTHON_YOINK(".python/sqlite3/dbapi2.py");
PYTHON_YOINK(".python/sqlite3/dump.py");
#endif

#if !IsTiny()
STATIC_YOINK(".python/dbm/");
PYTHON_YOINK(".python/dbm/__init__.py");
PYTHON_YOINK(".python/dbm/dumb.py");
PYTHON_YOINK(".python/dbm/gnu.py");
PYTHON_YOINK(".python/dbm/ndbm.py");
#endif

#if !IsTiny()
STATIC_YOINK(".python/xml/");
PYTHON_YOINK(".python/xml/__init__.py");
STATIC_YOINK(".python/xml/dom/");
PYTHON_YOINK(".python/xml/dom/NodeFilter.py");
PYTHON_YOINK(".python/xml/dom/__init__.py");
PYTHON_YOINK(".python/xml/dom/domreg.py");
PYTHON_YOINK(".python/xml/dom/expatbuilder.py");
PYTHON_YOINK(".python/xml/dom/minicompat.py");
PYTHON_YOINK(".python/xml/dom/minidom.py");
PYTHON_YOINK(".python/xml/dom/pulldom.py");
PYTHON_YOINK(".python/xml/dom/xmlbuilder.py");
STATIC_YOINK(".python/xml/etree/");
PYTHON_YOINK(".python/xml/etree/ElementInclude.py");
PYTHON_YOINK(".python/xml/etree/ElementPath.py");
PYTHON_YOINK(".python/xml/etree/ElementTree.py");
PYTHON_YOINK(".python/xml/etree/__init__.py");
PYTHON_YOINK(".python/xml/etree/cElementTree.py");
STATIC_YOINK(".python/xml/parsers/");
PYTHON_YOINK(".python/xml/parsers/__init__.py");
PYTHON_YOINK(".python/xml/parsers/expat.py");
STATIC_YOINK(".python/xml/sax/");
PYTHON_YOINK(".python/xml/sax/__init__.py");
PYTHON_YOINK(".python/xml/sax/_exceptions.py");
PYTHON_YOINK(".python/xml/sax/expatreader.py");
PYTHON_YOINK(".python/xml/sax/handler.py");
PYTHON_YOINK(".python/xml/sax/saxutils.py");
PYTHON_YOINK(".python/xml/sax/xmlreader.py");
STATIC_YOINK(".python/xmlrpc/");
PYTHON_YOINK(".python/xmlrpc/__init__.py");
PYTHON_YOINK(".python/xmlrpc/client.py");
PYTHON_YOINK(".python/xmlrpc/server.py");
#endif

#if !IsTiny()
STATIC_YOINK(".python/multiprocessing/");
PYTHON_YOINK(".python/multiprocessing/__init__.py");
PYTHON_YOINK(".python/multiprocessing/connection.py");
PYTHON_YOINK(".python/multiprocessing/context.py");
STATIC_YOINK(".python/multiprocessing/dummy/");
PYTHON_YOINK(".python/multiprocessing/dummy/__init__.py");
PYTHON_YOINK(".python/multiprocessing/dummy/connection.py");
PYTHON_YOINK(".python/multiprocessing/forkserver.py");
PYTHON_YOINK(".python/multiprocessing/heap.py");
PYTHON_YOINK(".python/multiprocessing/managers.py");
PYTHON_YOINK(".python/multiprocessing/pool.py");
PYTHON_YOINK(".python/multiprocessing/popen_fork.py");
PYTHON_YOINK(".python/multiprocessing/popen_forkserver.py");
PYTHON_YOINK(".python/multiprocessing/popen_spawn_posix.py");
PYTHON_YOINK(".python/multiprocessing/popen_spawn_win32.py");
PYTHON_YOINK(".python/multiprocessing/process.py");
PYTHON_YOINK(".python/multiprocessing/queues.py");
PYTHON_YOINK(".python/multiprocessing/reduction.py");
PYTHON_YOINK(".python/multiprocessing/resource_sharer.py");
PYTHON_YOINK(".python/multiprocessing/semaphore_tracker.py");
PYTHON_YOINK(".python/multiprocessing/sharedctypes.py");
PYTHON_YOINK(".python/multiprocessing/spawn.py");
PYTHON_YOINK(".python/multiprocessing/synchronize.py");
PYTHON_YOINK(".python/multiprocessing/util.py");
#endif

#if !IsTiny()
STATIC_YOINK(".python/unittest/");
PYTHON_YOINK(".python/unittest/__init__.py");
PYTHON_YOINK(".python/unittest/__main__.py");
PYTHON_YOINK(".python/unittest/case.py");
PYTHON_YOINK(".python/unittest/loader.py");
PYTHON_YOINK(".python/unittest/main.py");
PYTHON_YOINK(".python/unittest/mock.py");
PYTHON_YOINK(".python/unittest/result.py");
PYTHON_YOINK(".python/unittest/runner.py");
PYTHON_YOINK(".python/unittest/signals.py");
PYTHON_YOINK(".python/unittest/suite.py");
PYTHON_YOINK(".python/unittest/util.py");
#endif

#if !IsTiny()
STATIC_YOINK(".python/venv/");
PYTHON_YOINK(".python/venv/__init__.py");
PYTHON_YOINK(".python/venv/__main__.py");
STATIC_YOINK(".python/venv/scripts/common/");
STATIC_YOINK(".python/venv/scripts/nt/");
STATIC_YOINK(".python/venv/scripts/posix/");
STATIC_YOINK(".python/venv/scripts/common/activate");
STATIC_YOINK(".python/venv/scripts/nt/Activate.ps1");
STATIC_YOINK(".python/venv/scripts/nt/activate.bat");
STATIC_YOINK(".python/venv/scripts/nt/deactivate.bat");
STATIC_YOINK(".python/venv/scripts/posix/activate.csh");
STATIC_YOINK(".python/venv/scripts/posix/activate.fish");
STATIC_YOINK(".python/ensurepip/");
PYTHON_YOINK(".python/ensurepip/__init__.py");
PYTHON_YOINK(".python/ensurepip/__main__.py");
STATIC_YOINK(".python/ensurepip/_bundled/");
PYTHON_YOINK(".python/ensurepip/_uninstall.py");
STATIC_YOINK(".python/ensurepip/_bundled/pip-18.1-py2.py3-none-any.whl");
STATIC_YOINK(".python/ensurepip/_bundled/setuptools-40.6.2-py2.py3-none-any.whl");
STATIC_YOINK(".python/distutils/");
PYTHON_YOINK(".python/distutils/__init__.py");
PYTHON_YOINK(".python/distutils/_msvccompiler.py");
PYTHON_YOINK(".python/distutils/archive_util.py");
PYTHON_YOINK(".python/distutils/bcppcompiler.py");
PYTHON_YOINK(".python/distutils/ccompiler.py");
PYTHON_YOINK(".python/distutils/cmd.py");
STATIC_YOINK(".python/distutils/command/");
PYTHON_YOINK(".python/distutils/command/__init__.py");
PYTHON_YOINK(".python/distutils/command/bdist.py");
PYTHON_YOINK(".python/distutils/command/bdist_dumb.py");
PYTHON_YOINK(".python/distutils/command/bdist_msi.py");
PYTHON_YOINK(".python/distutils/command/bdist_rpm.py");
PYTHON_YOINK(".python/distutils/command/bdist_wininst.py");
PYTHON_YOINK(".python/distutils/command/build.py");
PYTHON_YOINK(".python/distutils/command/build_clib.py");
PYTHON_YOINK(".python/distutils/command/build_ext.py");
PYTHON_YOINK(".python/distutils/command/build_py.py");
PYTHON_YOINK(".python/distutils/command/build_scripts.py");
PYTHON_YOINK(".python/distutils/command/check.py");
PYTHON_YOINK(".python/distutils/command/clean.py");
PYTHON_YOINK(".python/distutils/command/config.py");
PYTHON_YOINK(".python/distutils/command/install.py");
PYTHON_YOINK(".python/distutils/command/install_data.py");
PYTHON_YOINK(".python/distutils/command/install_egg_info.py");
PYTHON_YOINK(".python/distutils/command/install_headers.py");
PYTHON_YOINK(".python/distutils/command/install_lib.py");
PYTHON_YOINK(".python/distutils/command/install_scripts.py");
PYTHON_YOINK(".python/distutils/command/register.py");
PYTHON_YOINK(".python/distutils/command/sdist.py");
PYTHON_YOINK(".python/distutils/command/upload.py");
PYTHON_YOINK(".python/distutils/config.py");
PYTHON_YOINK(".python/distutils/core.py");
PYTHON_YOINK(".python/distutils/cygwinccompiler.py");
PYTHON_YOINK(".python/distutils/debug.py");
PYTHON_YOINK(".python/distutils/dep_util.py");
PYTHON_YOINK(".python/distutils/dir_util.py");
PYTHON_YOINK(".python/distutils/dist.py");
PYTHON_YOINK(".python/distutils/errors.py");
PYTHON_YOINK(".python/distutils/extension.py");
PYTHON_YOINK(".python/distutils/fancy_getopt.py");
PYTHON_YOINK(".python/distutils/file_util.py");
PYTHON_YOINK(".python/distutils/filelist.py");
PYTHON_YOINK(".python/distutils/log.py");
PYTHON_YOINK(".python/distutils/msvc9compiler.py");
PYTHON_YOINK(".python/distutils/msvccompiler.py");
PYTHON_YOINK(".python/distutils/spawn.py");
PYTHON_YOINK(".python/distutils/sysconfig.py");
STATIC_YOINK(".python/distutils/tests/");
PYTHON_YOINK(".python/distutils/tests/__init__.py");
PYTHON_YOINK(".python/distutils/tests/support.py");
PYTHON_YOINK(".python/distutils/tests/test_archive_util.py");
PYTHON_YOINK(".python/distutils/tests/test_bdist.py");
PYTHON_YOINK(".python/distutils/tests/test_bdist_dumb.py");
PYTHON_YOINK(".python/distutils/tests/test_bdist_msi.py");
PYTHON_YOINK(".python/distutils/tests/test_bdist_rpm.py");
PYTHON_YOINK(".python/distutils/tests/test_bdist_wininst.py");
PYTHON_YOINK(".python/distutils/tests/test_build.py");
PYTHON_YOINK(".python/distutils/tests/test_build_clib.py");
PYTHON_YOINK(".python/distutils/tests/test_build_ext.py");
PYTHON_YOINK(".python/distutils/tests/test_build_py.py");
PYTHON_YOINK(".python/distutils/tests/test_build_scripts.py");
PYTHON_YOINK(".python/distutils/tests/test_check.py");
PYTHON_YOINK(".python/distutils/tests/test_clean.py");
PYTHON_YOINK(".python/distutils/tests/test_cmd.py");
PYTHON_YOINK(".python/distutils/tests/test_config.py");
PYTHON_YOINK(".python/distutils/tests/test_config_cmd.py");
PYTHON_YOINK(".python/distutils/tests/test_core.py");
PYTHON_YOINK(".python/distutils/tests/test_cygwinccompiler.py");
PYTHON_YOINK(".python/distutils/tests/test_dep_util.py");
PYTHON_YOINK(".python/distutils/tests/test_dir_util.py");
PYTHON_YOINK(".python/distutils/tests/test_dist.py");
PYTHON_YOINK(".python/distutils/tests/test_extension.py");
PYTHON_YOINK(".python/distutils/tests/test_file_util.py");
PYTHON_YOINK(".python/distutils/tests/test_filelist.py");
PYTHON_YOINK(".python/distutils/tests/test_install.py");
PYTHON_YOINK(".python/distutils/tests/test_install_data.py");
PYTHON_YOINK(".python/distutils/tests/test_install_headers.py");
PYTHON_YOINK(".python/distutils/tests/test_install_lib.py");
PYTHON_YOINK(".python/distutils/tests/test_install_scripts.py");
PYTHON_YOINK(".python/distutils/tests/test_log.py");
PYTHON_YOINK(".python/distutils/tests/test_msvc9compiler.py");
PYTHON_YOINK(".python/distutils/tests/test_msvccompiler.py");
PYTHON_YOINK(".python/distutils/tests/test_register.py");
PYTHON_YOINK(".python/distutils/tests/test_sdist.py");
PYTHON_YOINK(".python/distutils/tests/test_spawn.py");
PYTHON_YOINK(".python/distutils/tests/test_sysconfig.py");
PYTHON_YOINK(".python/distutils/tests/test_text_file.py");
PYTHON_YOINK(".python/distutils/tests/test_unixccompiler.py");
PYTHON_YOINK(".python/distutils/tests/test_upload.py");
PYTHON_YOINK(".python/distutils/tests/test_util.py");
PYTHON_YOINK(".python/distutils/tests/test_version.py");
PYTHON_YOINK(".python/distutils/tests/test_versionpredicate.py");
PYTHON_YOINK(".python/distutils/text_file.py");
PYTHON_YOINK(".python/distutils/unixccompiler.py");
PYTHON_YOINK(".python/distutils/util.py");
PYTHON_YOINK(".python/distutils/version.py");
PYTHON_YOINK(".python/distutils/versionpredicate.py");
STATIC_YOINK(".python/distutils/command/command_template");
STATIC_YOINK(".python/distutils/tests/Setup.sample");
STATIC_YOINK(".python/msilib/");
PYTHON_YOINK(".python/msilib/__init__.py");
PYTHON_YOINK(".python/msilib/schema.py");
PYTHON_YOINK(".python/msilib/sequence.py");
PYTHON_YOINK(".python/msilib/text.py");
#endif

STATIC_YOINK(".python/encodings/");
PYTHON_YOINK(".python/encodings/__init__.py");
PYTHON_YOINK(".python/encodings/aliases.py");
PYTHON_YOINK(".python/encodings/mbcs.py");
PYTHON_YOINK(".python/encodings/ascii.py");
PYTHON_YOINK(".python/encodings/latin_1.py");
PYTHON_YOINK(".python/encodings/utf_8.py");
PYTHON_YOINK(".python/encodings/utf_8_sig.py");
PYTHON_YOINK(".python/encodings/utf_16.py");
PYTHON_YOINK(".python/encodings/utf_16_be.py");
PYTHON_YOINK(".python/encodings/utf_16_le.py");
PYTHON_YOINK(".python/encodings/utf_32.py");
PYTHON_YOINK(".python/encodings/utf_32_be.py");
PYTHON_YOINK(".python/encodings/utf_32_le.py");
PYTHON_YOINK(".python/encodings/uu_codec.py");
PYTHON_YOINK(".python/encodings/undefined.py");
PYTHON_YOINK(".python/encodings/hex_codec.py");
PYTHON_YOINK(".python/encodings/base64_codec.py");
PYTHON_YOINK(".python/encodings/unicode_escape.py");
PYTHON_YOINK(".python/encodings/unicode_internal.py");
PYTHON_YOINK(".python/encodings/raw_unicode_escape.py");
#if !IsTiny()
PYTHON_YOINK(".python/encodings/zlib_codec.py");
PYTHON_YOINK(".python/encodings/big5.py");
PYTHON_YOINK(".python/encodings/big5hkscs.py");
PYTHON_YOINK(".python/encodings/bz2_codec.py");
PYTHON_YOINK(".python/encodings/charmap.py");
PYTHON_YOINK(".python/encodings/cp037.py");
PYTHON_YOINK(".python/encodings/cp1006.py");
PYTHON_YOINK(".python/encodings/cp1026.py");
PYTHON_YOINK(".python/encodings/cp1125.py");
PYTHON_YOINK(".python/encodings/cp1140.py");
PYTHON_YOINK(".python/encodings/cp1250.py");
PYTHON_YOINK(".python/encodings/cp1251.py");
PYTHON_YOINK(".python/encodings/cp1252.py");
PYTHON_YOINK(".python/encodings/cp1253.py");
PYTHON_YOINK(".python/encodings/cp1254.py");
PYTHON_YOINK(".python/encodings/cp1255.py");
PYTHON_YOINK(".python/encodings/cp1256.py");
PYTHON_YOINK(".python/encodings/cp1257.py");
PYTHON_YOINK(".python/encodings/cp1258.py");
PYTHON_YOINK(".python/encodings/cp273.py");
PYTHON_YOINK(".python/encodings/cp424.py");
PYTHON_YOINK(".python/encodings/cp437.py");
PYTHON_YOINK(".python/encodings/cp500.py");
PYTHON_YOINK(".python/encodings/cp65001.py");
PYTHON_YOINK(".python/encodings/cp720.py");
PYTHON_YOINK(".python/encodings/cp737.py");
PYTHON_YOINK(".python/encodings/cp775.py");
PYTHON_YOINK(".python/encodings/cp850.py");
PYTHON_YOINK(".python/encodings/cp852.py");
PYTHON_YOINK(".python/encodings/cp855.py");
PYTHON_YOINK(".python/encodings/cp856.py");
PYTHON_YOINK(".python/encodings/cp857.py");
PYTHON_YOINK(".python/encodings/cp858.py");
PYTHON_YOINK(".python/encodings/cp860.py");
PYTHON_YOINK(".python/encodings/cp861.py");
PYTHON_YOINK(".python/encodings/cp862.py");
PYTHON_YOINK(".python/encodings/cp863.py");
PYTHON_YOINK(".python/encodings/cp864.py");
PYTHON_YOINK(".python/encodings/cp865.py");
PYTHON_YOINK(".python/encodings/cp866.py");
PYTHON_YOINK(".python/encodings/cp869.py");
PYTHON_YOINK(".python/encodings/cp874.py");
PYTHON_YOINK(".python/encodings/cp875.py");
PYTHON_YOINK(".python/encodings/cp932.py");
PYTHON_YOINK(".python/encodings/cp949.py");
PYTHON_YOINK(".python/encodings/cp950.py");
PYTHON_YOINK(".python/encodings/euc_jis_2004.py");
PYTHON_YOINK(".python/encodings/euc_jisx0213.py");
PYTHON_YOINK(".python/encodings/euc_jp.py");
PYTHON_YOINK(".python/encodings/euc_kr.py");
PYTHON_YOINK(".python/encodings/gb18030.py");
PYTHON_YOINK(".python/encodings/gb2312.py");
PYTHON_YOINK(".python/encodings/gbk.py");
PYTHON_YOINK(".python/encodings/hp_roman8.py");
PYTHON_YOINK(".python/encodings/hz.py");
PYTHON_YOINK(".python/encodings/idna.py");
PYTHON_YOINK(".python/encodings/iso2022_jp.py");
PYTHON_YOINK(".python/encodings/iso2022_jp_1.py");
PYTHON_YOINK(".python/encodings/iso2022_jp_2.py");
PYTHON_YOINK(".python/encodings/iso2022_jp_2004.py");
PYTHON_YOINK(".python/encodings/iso2022_jp_3.py");
PYTHON_YOINK(".python/encodings/iso2022_jp_ext.py");
PYTHON_YOINK(".python/encodings/iso2022_kr.py");
PYTHON_YOINK(".python/encodings/iso8859_1.py");
PYTHON_YOINK(".python/encodings/iso8859_10.py");
PYTHON_YOINK(".python/encodings/iso8859_11.py");
PYTHON_YOINK(".python/encodings/iso8859_13.py");
PYTHON_YOINK(".python/encodings/iso8859_14.py");
PYTHON_YOINK(".python/encodings/iso8859_15.py");
PYTHON_YOINK(".python/encodings/iso8859_16.py");
PYTHON_YOINK(".python/encodings/iso8859_2.py");
PYTHON_YOINK(".python/encodings/iso8859_3.py");
PYTHON_YOINK(".python/encodings/iso8859_4.py");
PYTHON_YOINK(".python/encodings/iso8859_5.py");
PYTHON_YOINK(".python/encodings/iso8859_6.py");
PYTHON_YOINK(".python/encodings/iso8859_7.py");
PYTHON_YOINK(".python/encodings/iso8859_8.py");
PYTHON_YOINK(".python/encodings/iso8859_9.py");
PYTHON_YOINK(".python/encodings/johab.py");
PYTHON_YOINK(".python/encodings/koi8_r.py");
PYTHON_YOINK(".python/encodings/koi8_t.py");
PYTHON_YOINK(".python/encodings/koi8_u.py");
PYTHON_YOINK(".python/encodings/kz1048.py");
PYTHON_YOINK(".python/encodings/mac_arabic.py");
PYTHON_YOINK(".python/encodings/mac_centeuro.py");
PYTHON_YOINK(".python/encodings/mac_croatian.py");
PYTHON_YOINK(".python/encodings/mac_cyrillic.py");
PYTHON_YOINK(".python/encodings/mac_farsi.py");
PYTHON_YOINK(".python/encodings/mac_greek.py");
PYTHON_YOINK(".python/encodings/mac_iceland.py");
PYTHON_YOINK(".python/encodings/mac_latin2.py");
PYTHON_YOINK(".python/encodings/mac_roman.py");
PYTHON_YOINK(".python/encodings/mac_romanian.py");
PYTHON_YOINK(".python/encodings/mac_turkish.py");
PYTHON_YOINK(".python/encodings/oem.py");
PYTHON_YOINK(".python/encodings/palmos.py");
PYTHON_YOINK(".python/encodings/ptcp154.py");
PYTHON_YOINK(".python/encodings/punycode.py");
PYTHON_YOINK(".python/encodings/quopri_codec.py");
PYTHON_YOINK(".python/encodings/rot_13.py");
PYTHON_YOINK(".python/encodings/shift_jis.py");
PYTHON_YOINK(".python/encodings/shift_jis_2004.py");
PYTHON_YOINK(".python/encodings/shift_jisx0213.py");
PYTHON_YOINK(".python/encodings/tis_620.py");
PYTHON_YOINK(".python/encodings/utf_7.py");
#endif

#if !IsTiny()
PYTHON_YOINK(".python/smtpd.py");
PYTHON_YOINK(".python/poplib.py");
PYTHON_YOINK(".python/imaplib.py");
PYTHON_YOINK(".python/mailbox.py");
PYTHON_YOINK(".python/mailcap.py");
PYTHON_YOINK(".python/smtplib.py");
PYTHON_YOINK(".python/nntplib.py");
STATIC_YOINK(".python/email/");
PYTHON_YOINK(".python/email/__init__.py");
PYTHON_YOINK(".python/email/_encoded_words.py");
PYTHON_YOINK(".python/email/_header_value_parser.py");
PYTHON_YOINK(".python/email/_parseaddr.py");
PYTHON_YOINK(".python/email/_policybase.py");
PYTHON_YOINK(".python/email/base64mime.py");
PYTHON_YOINK(".python/email/charset.py");
PYTHON_YOINK(".python/email/contentmanager.py");
PYTHON_YOINK(".python/email/encoders.py");
PYTHON_YOINK(".python/email/errors.py");
PYTHON_YOINK(".python/email/feedparser.py");
PYTHON_YOINK(".python/email/generator.py");
PYTHON_YOINK(".python/email/header.py");
PYTHON_YOINK(".python/email/headerregistry.py");
PYTHON_YOINK(".python/email/iterators.py");
PYTHON_YOINK(".python/email/message.py");
STATIC_YOINK(".python/email/mime/");
PYTHON_YOINK(".python/email/mime/__init__.py");
PYTHON_YOINK(".python/email/mime/application.py");
PYTHON_YOINK(".python/email/mime/audio.py");
PYTHON_YOINK(".python/email/mime/base.py");
PYTHON_YOINK(".python/email/mime/image.py");
PYTHON_YOINK(".python/email/mime/message.py");
PYTHON_YOINK(".python/email/mime/multipart.py");
PYTHON_YOINK(".python/email/mime/nonmultipart.py");
PYTHON_YOINK(".python/email/mime/text.py");
PYTHON_YOINK(".python/email/parser.py");
PYTHON_YOINK(".python/email/policy.py");
PYTHON_YOINK(".python/email/quoprimime.py");
PYTHON_YOINK(".python/email/utils.py");
STATIC_YOINK(".python/email/architecture.rst");
#endif

#ifdef WITH_THREAD
PYTHON_YOINK(".python/asynchat.py");
PYTHON_YOINK(".python/asyncore.py");
STATIC_YOINK(".python/asyncio/");
PYTHON_YOINK(".python/asyncio/__init__.py");
PYTHON_YOINK(".python/asyncio/base_events.py");
PYTHON_YOINK(".python/asyncio/base_futures.py");
PYTHON_YOINK(".python/asyncio/base_subprocess.py");
PYTHON_YOINK(".python/asyncio/base_tasks.py");
PYTHON_YOINK(".python/asyncio/compat.py");
PYTHON_YOINK(".python/asyncio/constants.py");
PYTHON_YOINK(".python/asyncio/coroutines.py");
PYTHON_YOINK(".python/asyncio/events.py");
PYTHON_YOINK(".python/asyncio/futures.py");
PYTHON_YOINK(".python/asyncio/locks.py");
PYTHON_YOINK(".python/asyncio/log.py");
PYTHON_YOINK(".python/asyncio/proactor_events.py");
PYTHON_YOINK(".python/asyncio/protocols.py");
PYTHON_YOINK(".python/asyncio/queues.py");
PYTHON_YOINK(".python/asyncio/selector_events.py");
PYTHON_YOINK(".python/asyncio/sslproto.py");
PYTHON_YOINK(".python/asyncio/streams.py");
PYTHON_YOINK(".python/asyncio/subprocess.py");
PYTHON_YOINK(".python/asyncio/tasks.py");
PYTHON_YOINK(".python/asyncio/test_utils.py");
PYTHON_YOINK(".python/asyncio/transports.py");
PYTHON_YOINK(".python/asyncio/unix_events.py");
PYTHON_YOINK(".python/asyncio/windows_events.py");
PYTHON_YOINK(".python/asyncio/windows_utils.py");
#endif

const struct _frozen *PyImport_FrozenModules = _PyImport_FrozenModules;
struct _inittab *PyImport_Inittab = _PyImport_Inittab;
static jmp_buf jbuf;

static void
OnKeyboardInterrupt(int sig)
{
    gclongjmp(jbuf, 1);
}

static void
AddCompletion(linenoiseCompletions *c, char *s)
{
    c->cvec = realloc(c->cvec, ++c->len * sizeof(*c->cvec));
    c->cvec[c->len - 1] = s;
}

static void
CompleteDict(const char *b, const char *q, const char *p,
             linenoiseCompletions *c, PyObject *o)
{
    const char *s;
    PyObject *k, *v;
    Py_ssize_t i, m;
    for (i = 0; PyDict_Next(o, &i, &k, &v);) {
        if ((v != Py_None && PyUnicode_Check(k) &&
             (s = PyUnicode_AsUTF8AndSize(k, &m)) &&
             m >= q - p && !memcmp(s, p, q - p))) {
            AddCompletion(c, xasprintf("%.*s%.*s", p - b, b, m, s));
        }
    }
}

static void
CompleteDir(const char *b, const char *q, const char *p,
            linenoiseCompletions *c, PyObject *o)
{
    Py_ssize_t m;
    const char *s;
    PyObject *d, *i, *k;
    if (!(d = PyObject_Dir(o))) return;
    if ((i = PyObject_GetIter(d))) {
        while ((k = PyIter_Next(i))) {
            if (((s = PyUnicode_AsUTF8AndSize(k, &m)) &&
                 m >= q - p && !memcmp(s, p, q - p))) {
                AddCompletion(c, xasprintf("%.*s%.*s", p - b, b, m, s));
            }
            Py_DECREF(k);
        }
        Py_DECREF(i);
    }
    Py_DECREF(d);
}

static void
TerminalCompletion(const char *p, linenoiseCompletions *c)
{
    PyObject *o, *t, *i;
    const char *q, *s, *b;
    for (b = p, p += strlen(p); p > b; --p) {
        if (!isalnum(p[-1]) && p[-1] != '.' && p[-1] != '_') {
            break;
        }
    }
    o = PyModule_GetDict(PyImport_AddModule("__main__"));
    if (!*(q = strchrnul(p, '.'))) {
        CompleteDict(b, q, p, c, o);
        CompleteDir(b, q, p, c, PyDict_GetItemString(o, "__builtins__"));
    } else {
        s = strndup(p, q - p);
        if ((t = PyDict_GetItemString(o, s))) {
            Py_INCREF(t);
        } else {
            o = PyDict_GetItemString(o, "__builtins__");
            if (PyObject_HasAttrString(o, s)) {
                t = PyObject_GetAttrString(o, s);
            }
        }
        while ((p = q + 1), (o = t)) {
            if (*(q = strchrnul(p, '.'))) {
                t = PyObject_GetAttrString(o, gc(strndup(p, q - p)));
                Py_DECREF(o);
            } else {
                CompleteDir(b, q, p, c, o);
                Py_DECREF(o);
                break;
            }
        }
        free(s);
    }
}

static char *
TerminalHint(const char *p, int *color, int *bold)
{
    char *h = 0;
    linenoiseCompletions c = {0};
    TerminalCompletion(p, &c);
    if (c.len == 1) {
        h = strdup(c.cvec[0] + strlen(p));
        *bold = 2;
    }
    linenoiseFreeCompletions(&c);
    return h;
}

static char *
TerminalReadline(FILE *sys_stdin, FILE *sys_stdout, const char *prompt)
{
    size_t n;
    char *p, *q;
    PyOS_sighandler_t saint;
    saint = PyOS_setsig(SIGINT, OnKeyboardInterrupt);
    if (setjmp(jbuf)) {
        linenoiseDisableRawMode(STDIN_FILENO);
        PyOS_setsig(SIGINT, saint);
        return NULL;
    }
    p = ezlinenoise(prompt, "python");
    PyOS_setsig(SIGINT, saint);
    if (p) {
        n = strlen(p);
        q = PyMem_RawMalloc(n + 2);
        strcpy(mempcpy(q, p, n), "\n");
        free(p);
        clearerr(sys_stdin);
    } else {
        q = PyMem_RawMalloc(1);
        if (q) *q = 0;
    }
    return q;
}

int
main(int argc, char **argv)
{
    wchar_t **argv_copy;
    /* We need a second copy, as Python might modify the first one. */
    wchar_t **argv_copy2;
    int i, res;
    char *oldloc;

    /* if (FindDebugBinary()) { */
    /*     ShowCrashReports(); */
    /* } */

    PyOS_ReadlineFunctionPointer = TerminalReadline;
    linenoiseSetCompletionCallback(TerminalCompletion);
    linenoiseSetHintsCallback(TerminalHint);
    linenoiseSetFreeHintsCallback(free);

    /* Force malloc() allocator to bootstrap Python */
    _PyMem_SetupAllocators("malloc");

    argv_copy = (wchar_t **)PyMem_RawMalloc(sizeof(wchar_t*) * (argc+1));
    argv_copy2 = (wchar_t **)PyMem_RawMalloc(sizeof(wchar_t*) * (argc+1));
    if (!argv_copy || !argv_copy2) {
        fprintf(stderr, "out of memory\n");
        return 1;
    }

    /* 754 requires that FP exceptions run in "no stop" mode by default,
     * and until C vendors implement C99's ways to control FP exceptions,
     * Python requires non-stop mode.  Alas, some platforms enable FP
     * exceptions by default.  Here we disable them.
     */
#ifdef __FreeBSD__
    fedisableexcept(FE_OVERFLOW);
#endif

    oldloc = _PyMem_RawStrdup(setlocale(LC_ALL, NULL));
    if (!oldloc) {
        fprintf(stderr, "out of memory\n");
        return 1;
    }

    setlocale(LC_ALL, "");
    for (i = 0; i < argc; i++) {
        argv_copy[i] = Py_DecodeLocale(argv[i], NULL);
        if (!argv_copy[i]) {
            PyMem_RawFree(oldloc);
            fprintf(stderr, "Fatal Python error: "
                            "unable to decode the command line argument #%i\n",
                            i + 1);
            return 1;
        }
        argv_copy2[i] = argv_copy[i];
    }
    argv_copy2[argc] = argv_copy[argc] = NULL;

    setlocale(LC_ALL, oldloc);
    PyMem_RawFree(oldloc);

    res = Py_Main(argc, argv_copy);

    /* Force again malloc() allocator to release memory blocks allocated
       before Py_Main() */
    _PyMem_SetupAllocators("malloc");

    for (i = 0; i < argc; i++) {
        PyMem_RawFree(argv_copy2[i]);
    }
    PyMem_RawFree(argv_copy);
    PyMem_RawFree(argv_copy2);
    return res;
}
