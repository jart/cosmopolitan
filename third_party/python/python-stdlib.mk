#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#───vi: set et ft=make ts=8 tw=8 fenc=utf-8 :vi───────────────────────┘

THIRD_PARTY_PYTHON_STDLIB_PY =									\
	third_party/python/Lib/									\
	third_party/python/Lib/__future__.py							\
	third_party/python/Lib/_bootlocale.py							\
	third_party/python/Lib/_collections_abc.py						\
	third_party/python/Lib/_compat_pickle.py						\
	third_party/python/Lib/_compression.py							\
	third_party/python/Lib/_dummy_thread.py							\
	third_party/python/Lib/_markupbase.py							\
	third_party/python/Lib/_osx_support.py							\
	third_party/python/Lib/_pyio.py								\
	third_party/python/Lib/_sitebuiltins.py							\
	third_party/python/Lib/_strptime.py							\
	third_party/python/Lib/_sysconfigdata_m_cosmo_x86_64-cosmo.py				\
	third_party/python/Lib/_threading_local.py						\
	third_party/python/Lib/_weakrefset.py							\
	third_party/python/Lib/abc.py								\
	third_party/python/Lib/aifc.py								\
	third_party/python/Lib/antigravity.py							\
	third_party/python/Lib/argparse.py							\
	third_party/python/Lib/ast.py								\
	third_party/python/Lib/asynchat.py							\
	third_party/python/Lib/asyncio/								\
	third_party/python/Lib/asyncio/__init__.py						\
	third_party/python/Lib/asyncio/base_events.py						\
	third_party/python/Lib/asyncio/base_futures.py						\
	third_party/python/Lib/asyncio/base_subprocess.py					\
	third_party/python/Lib/asyncio/base_tasks.py						\
	third_party/python/Lib/asyncio/compat.py						\
	third_party/python/Lib/asyncio/constants.py						\
	third_party/python/Lib/asyncio/coroutines.py						\
	third_party/python/Lib/asyncio/events.py						\
	third_party/python/Lib/asyncio/futures.py						\
	third_party/python/Lib/asyncio/locks.py							\
	third_party/python/Lib/asyncio/log.py							\
	third_party/python/Lib/asyncio/proactor_events.py					\
	third_party/python/Lib/asyncio/protocols.py						\
	third_party/python/Lib/asyncio/queues.py						\
	third_party/python/Lib/asyncio/selector_events.py					\
	third_party/python/Lib/asyncio/sslproto.py						\
	third_party/python/Lib/asyncio/streams.py						\
	third_party/python/Lib/asyncio/subprocess.py						\
	third_party/python/Lib/asyncio/tasks.py							\
	third_party/python/Lib/asyncio/test_utils.py						\
	third_party/python/Lib/asyncio/transports.py						\
	third_party/python/Lib/asyncio/unix_events.py						\
	third_party/python/Lib/asyncio/windows_events.py					\
	third_party/python/Lib/asyncio/windows_utils.py						\
	third_party/python/Lib/asyncore.py							\
	third_party/python/Lib/base64.py							\
	third_party/python/Lib/bdb.py								\
	third_party/python/Lib/binhex.py							\
	third_party/python/Lib/bisect.py							\
	third_party/python/Lib/bz2.py								\
	third_party/python/Lib/cProfile.py							\
	third_party/python/Lib/calendar.py							\
	third_party/python/Lib/cgi.py								\
	third_party/python/Lib/cgitb.py								\
	third_party/python/Lib/chunk.py								\
	third_party/python/Lib/cmd.py								\
	third_party/python/Lib/code.py								\
	third_party/python/Lib/codecs.py							\
	third_party/python/Lib/codeop.py							\
	third_party/python/Lib/collections/							\
	third_party/python/Lib/collections/__init__.py						\
	third_party/python/Lib/collections/abc.py						\
	third_party/python/Lib/colorsys.py							\
	third_party/python/Lib/compileall.py							\
	third_party/python/Lib/configparser.py							\
	third_party/python/Lib/contextlib.py							\
	third_party/python/Lib/copy.py								\
	third_party/python/Lib/copyreg.py							\
	third_party/python/Lib/crypt.py								\
	third_party/python/Lib/csv.py								\
	third_party/python/Lib/datetime.py							\
	third_party/python/Lib/dbm/								\
	third_party/python/Lib/dbm/__init__.py							\
	third_party/python/Lib/dbm/dumb.py							\
	third_party/python/Lib/dbm/gnu.py							\
	third_party/python/Lib/dbm/ndbm.py							\
	third_party/python/Lib/decimal.py							\
	third_party/python/Lib/difflib.py							\
	third_party/python/Lib/dis.py								\
	third_party/python/Lib/distutils/							\
	third_party/python/Lib/distutils/__init__.py						\
	third_party/python/Lib/distutils/_msvccompiler.py					\
	third_party/python/Lib/distutils/archive_util.py					\
	third_party/python/Lib/distutils/bcppcompiler.py					\
	third_party/python/Lib/distutils/ccompiler.py						\
	third_party/python/Lib/distutils/cmd.py							\
	third_party/python/Lib/distutils/command/						\
	third_party/python/Lib/distutils/command/__init__.py					\
	third_party/python/Lib/distutils/command/bdist.py					\
	third_party/python/Lib/distutils/command/bdist_dumb.py					\
	third_party/python/Lib/distutils/command/bdist_msi.py					\
	third_party/python/Lib/distutils/command/bdist_rpm.py					\
	third_party/python/Lib/distutils/command/bdist_wininst.py				\
	third_party/python/Lib/distutils/command/build.py					\
	third_party/python/Lib/distutils/command/build_clib.py					\
	third_party/python/Lib/distutils/command/build_ext.py					\
	third_party/python/Lib/distutils/command/build_py.py					\
	third_party/python/Lib/distutils/command/build_scripts.py				\
	third_party/python/Lib/distutils/command/check.py					\
	third_party/python/Lib/distutils/command/clean.py					\
	third_party/python/Lib/distutils/command/command_template				\
	third_party/python/Lib/distutils/command/config.py					\
	third_party/python/Lib/distutils/command/install.py					\
	third_party/python/Lib/distutils/command/install_data.py				\
	third_party/python/Lib/distutils/command/install_egg_info.py				\
	third_party/python/Lib/distutils/command/install_headers.py				\
	third_party/python/Lib/distutils/command/install_lib.py					\
	third_party/python/Lib/distutils/command/install_scripts.py				\
	third_party/python/Lib/distutils/command/register.py					\
	third_party/python/Lib/distutils/command/sdist.py					\
	third_party/python/Lib/distutils/command/upload.py					\
	third_party/python/Lib/distutils/config.py						\
	third_party/python/Lib/distutils/core.py						\
	third_party/python/Lib/distutils/cygwinccompiler.py					\
	third_party/python/Lib/distutils/debug.py						\
	third_party/python/Lib/distutils/dep_util.py						\
	third_party/python/Lib/distutils/dir_util.py						\
	third_party/python/Lib/distutils/dist.py						\
	third_party/python/Lib/distutils/errors.py						\
	third_party/python/Lib/distutils/extension.py						\
	third_party/python/Lib/distutils/fancy_getopt.py					\
	third_party/python/Lib/distutils/file_util.py						\
	third_party/python/Lib/distutils/filelist.py						\
	third_party/python/Lib/distutils/log.py							\
	third_party/python/Lib/distutils/msvc9compiler.py					\
	third_party/python/Lib/distutils/msvccompiler.py					\
	third_party/python/Lib/distutils/spawn.py						\
	third_party/python/Lib/distutils/sysconfig.py						\
	third_party/python/Lib/distutils/tests/							\
	third_party/python/Lib/distutils/tests/Setup.sample					\
	third_party/python/Lib/distutils/tests/__init__.py					\
	third_party/python/Lib/distutils/tests/support.py					\
	third_party/python/Lib/distutils/tests/test_archive_util.py				\
	third_party/python/Lib/distutils/tests/test_bdist.py					\
	third_party/python/Lib/distutils/tests/test_bdist_dumb.py				\
	third_party/python/Lib/distutils/tests/test_bdist_msi.py				\
	third_party/python/Lib/distutils/tests/test_bdist_rpm.py				\
	third_party/python/Lib/distutils/tests/test_bdist_wininst.py				\
	third_party/python/Lib/distutils/tests/test_build.py					\
	third_party/python/Lib/distutils/tests/test_build_clib.py				\
	third_party/python/Lib/distutils/tests/test_build_ext.py				\
	third_party/python/Lib/distutils/tests/test_build_py.py					\
	third_party/python/Lib/distutils/tests/test_build_scripts.py				\
	third_party/python/Lib/distutils/tests/test_check.py					\
	third_party/python/Lib/distutils/tests/test_clean.py					\
	third_party/python/Lib/distutils/tests/test_cmd.py					\
	third_party/python/Lib/distutils/tests/test_config.py					\
	third_party/python/Lib/distutils/tests/test_config_cmd.py				\
	third_party/python/Lib/distutils/tests/test_core.py					\
	third_party/python/Lib/distutils/tests/test_cygwinccompiler.py				\
	third_party/python/Lib/distutils/tests/test_dep_util.py					\
	third_party/python/Lib/distutils/tests/test_dir_util.py					\
	third_party/python/Lib/distutils/tests/test_dist.py					\
	third_party/python/Lib/distutils/tests/test_extension.py				\
	third_party/python/Lib/distutils/tests/test_file_util.py				\
	third_party/python/Lib/distutils/tests/test_filelist.py					\
	third_party/python/Lib/distutils/tests/test_install.py					\
	third_party/python/Lib/distutils/tests/test_install_data.py				\
	third_party/python/Lib/distutils/tests/test_install_headers.py				\
	third_party/python/Lib/distutils/tests/test_install_lib.py				\
	third_party/python/Lib/distutils/tests/test_install_scripts.py				\
	third_party/python/Lib/distutils/tests/test_log.py					\
	third_party/python/Lib/distutils/tests/test_msvc9compiler.py				\
	third_party/python/Lib/distutils/tests/test_msvccompiler.py				\
	third_party/python/Lib/distutils/tests/test_register.py					\
	third_party/python/Lib/distutils/tests/test_sdist.py					\
	third_party/python/Lib/distutils/tests/test_spawn.py					\
	third_party/python/Lib/distutils/tests/test_sysconfig.py				\
	third_party/python/Lib/distutils/tests/test_text_file.py				\
	third_party/python/Lib/distutils/tests/test_unixccompiler.py				\
	third_party/python/Lib/distutils/tests/test_upload.py					\
	third_party/python/Lib/distutils/tests/test_util.py					\
	third_party/python/Lib/distutils/tests/test_version.py					\
	third_party/python/Lib/distutils/tests/test_versionpredicate.py				\
	third_party/python/Lib/distutils/text_file.py						\
	third_party/python/Lib/distutils/unixccompiler.py					\
	third_party/python/Lib/distutils/util.py						\
	third_party/python/Lib/distutils/version.py						\
	third_party/python/Lib/distutils/versionpredicate.py					\
	third_party/python/Lib/doctest.py							\
	third_party/python/Lib/dummy_threading.py						\
	third_party/python/Lib/email/								\
	third_party/python/Lib/email/__init__.py						\
	third_party/python/Lib/email/_encoded_words.py						\
	third_party/python/Lib/email/_header_value_parser.py					\
	third_party/python/Lib/email/_parseaddr.py						\
	third_party/python/Lib/email/_policybase.py						\
	third_party/python/Lib/email/architecture.rst						\
	third_party/python/Lib/email/base64mime.py						\
	third_party/python/Lib/email/charset.py							\
	third_party/python/Lib/email/contentmanager.py						\
	third_party/python/Lib/email/encoders.py						\
	third_party/python/Lib/email/errors.py							\
	third_party/python/Lib/email/feedparser.py						\
	third_party/python/Lib/email/generator.py						\
	third_party/python/Lib/email/header.py							\
	third_party/python/Lib/email/headerregistry.py						\
	third_party/python/Lib/email/iterators.py						\
	third_party/python/Lib/email/message.py							\
	third_party/python/Lib/email/mime/							\
	third_party/python/Lib/email/mime/__init__.py						\
	third_party/python/Lib/email/mime/application.py					\
	third_party/python/Lib/email/mime/audio.py						\
	third_party/python/Lib/email/mime/base.py						\
	third_party/python/Lib/email/mime/image.py						\
	third_party/python/Lib/email/mime/message.py						\
	third_party/python/Lib/email/mime/multipart.py						\
	third_party/python/Lib/email/mime/nonmultipart.py					\
	third_party/python/Lib/email/mime/text.py						\
	third_party/python/Lib/email/parser.py							\
	third_party/python/Lib/email/policy.py							\
	third_party/python/Lib/email/quoprimime.py						\
	third_party/python/Lib/email/utils.py							\
	third_party/python/Lib/encodings/							\
	third_party/python/Lib/encodings/__init__.py						\
	third_party/python/Lib/encodings/aliases.py						\
	third_party/python/Lib/encodings/ascii.py						\
	third_party/python/Lib/encodings/base64_codec.py					\
	third_party/python/Lib/encodings/big5.py						\
	third_party/python/Lib/encodings/big5hkscs.py						\
	third_party/python/Lib/encodings/bz2_codec.py						\
	third_party/python/Lib/encodings/charmap.py						\
	third_party/python/Lib/encodings/cp037.py						\
	third_party/python/Lib/encodings/cp1006.py						\
	third_party/python/Lib/encodings/cp1026.py						\
	third_party/python/Lib/encodings/cp1125.py						\
	third_party/python/Lib/encodings/cp1140.py						\
	third_party/python/Lib/encodings/cp1250.py						\
	third_party/python/Lib/encodings/cp1251.py						\
	third_party/python/Lib/encodings/cp1252.py						\
	third_party/python/Lib/encodings/cp1253.py						\
	third_party/python/Lib/encodings/cp1254.py						\
	third_party/python/Lib/encodings/cp1255.py						\
	third_party/python/Lib/encodings/cp1256.py						\
	third_party/python/Lib/encodings/cp1257.py						\
	third_party/python/Lib/encodings/cp1258.py						\
	third_party/python/Lib/encodings/cp273.py						\
	third_party/python/Lib/encodings/cp424.py						\
	third_party/python/Lib/encodings/cp437.py						\
	third_party/python/Lib/encodings/cp500.py						\
	third_party/python/Lib/encodings/cp65001.py						\
	third_party/python/Lib/encodings/cp720.py						\
	third_party/python/Lib/encodings/cp737.py						\
	third_party/python/Lib/encodings/cp775.py						\
	third_party/python/Lib/encodings/cp850.py						\
	third_party/python/Lib/encodings/cp852.py						\
	third_party/python/Lib/encodings/cp855.py						\
	third_party/python/Lib/encodings/cp856.py						\
	third_party/python/Lib/encodings/cp857.py						\
	third_party/python/Lib/encodings/cp858.py						\
	third_party/python/Lib/encodings/cp860.py						\
	third_party/python/Lib/encodings/cp861.py						\
	third_party/python/Lib/encodings/cp862.py						\
	third_party/python/Lib/encodings/cp863.py						\
	third_party/python/Lib/encodings/cp864.py						\
	third_party/python/Lib/encodings/cp865.py						\
	third_party/python/Lib/encodings/cp866.py						\
	third_party/python/Lib/encodings/cp869.py						\
	third_party/python/Lib/encodings/cp874.py						\
	third_party/python/Lib/encodings/cp875.py						\
	third_party/python/Lib/encodings/cp932.py						\
	third_party/python/Lib/encodings/cp949.py						\
	third_party/python/Lib/encodings/cp950.py						\
	third_party/python/Lib/encodings/euc_jis_2004.py					\
	third_party/python/Lib/encodings/euc_jisx0213.py					\
	third_party/python/Lib/encodings/euc_jp.py						\
	third_party/python/Lib/encodings/euc_kr.py						\
	third_party/python/Lib/encodings/gb18030.py						\
	third_party/python/Lib/encodings/gb2312.py						\
	third_party/python/Lib/encodings/gbk.py							\
	third_party/python/Lib/encodings/hex_codec.py						\
	third_party/python/Lib/encodings/hp_roman8.py						\
	third_party/python/Lib/encodings/hz.py							\
	third_party/python/Lib/encodings/idna.py						\
	third_party/python/Lib/encodings/iso2022_jp.py						\
	third_party/python/Lib/encodings/iso2022_jp_1.py					\
	third_party/python/Lib/encodings/iso2022_jp_2.py					\
	third_party/python/Lib/encodings/iso2022_jp_2004.py					\
	third_party/python/Lib/encodings/iso2022_jp_3.py					\
	third_party/python/Lib/encodings/iso2022_jp_ext.py					\
	third_party/python/Lib/encodings/iso2022_kr.py						\
	third_party/python/Lib/encodings/iso8859_1.py						\
	third_party/python/Lib/encodings/iso8859_10.py						\
	third_party/python/Lib/encodings/iso8859_11.py						\
	third_party/python/Lib/encodings/iso8859_13.py						\
	third_party/python/Lib/encodings/iso8859_14.py						\
	third_party/python/Lib/encodings/iso8859_15.py						\
	third_party/python/Lib/encodings/iso8859_16.py						\
	third_party/python/Lib/encodings/iso8859_2.py						\
	third_party/python/Lib/encodings/iso8859_3.py						\
	third_party/python/Lib/encodings/iso8859_4.py						\
	third_party/python/Lib/encodings/iso8859_5.py						\
	third_party/python/Lib/encodings/iso8859_6.py						\
	third_party/python/Lib/encodings/iso8859_7.py						\
	third_party/python/Lib/encodings/iso8859_8.py						\
	third_party/python/Lib/encodings/iso8859_9.py						\
	third_party/python/Lib/encodings/johab.py						\
	third_party/python/Lib/encodings/koi8_r.py						\
	third_party/python/Lib/encodings/koi8_t.py						\
	third_party/python/Lib/encodings/koi8_u.py						\
	third_party/python/Lib/encodings/kz1048.py						\
	third_party/python/Lib/encodings/latin_1.py						\
	third_party/python/Lib/encodings/mac_arabic.py						\
	third_party/python/Lib/encodings/mac_centeuro.py					\
	third_party/python/Lib/encodings/mac_croatian.py					\
	third_party/python/Lib/encodings/mac_cyrillic.py					\
	third_party/python/Lib/encodings/mac_farsi.py						\
	third_party/python/Lib/encodings/mac_greek.py						\
	third_party/python/Lib/encodings/mac_iceland.py						\
	third_party/python/Lib/encodings/mac_latin2.py						\
	third_party/python/Lib/encodings/mac_roman.py						\
	third_party/python/Lib/encodings/mac_romanian.py					\
	third_party/python/Lib/encodings/mac_turkish.py						\
	third_party/python/Lib/encodings/mbcs.py						\
	third_party/python/Lib/encodings/oem.py							\
	third_party/python/Lib/encodings/palmos.py						\
	third_party/python/Lib/encodings/ptcp154.py						\
	third_party/python/Lib/encodings/punycode.py						\
	third_party/python/Lib/encodings/quopri_codec.py					\
	third_party/python/Lib/encodings/raw_unicode_escape.py					\
	third_party/python/Lib/encodings/rot_13.py						\
	third_party/python/Lib/encodings/shift_jis.py						\
	third_party/python/Lib/encodings/shift_jis_2004.py					\
	third_party/python/Lib/encodings/shift_jisx0213.py					\
	third_party/python/Lib/encodings/tis_620.py						\
	third_party/python/Lib/encodings/undefined.py						\
	third_party/python/Lib/encodings/unicode_escape.py					\
	third_party/python/Lib/encodings/unicode_internal.py					\
	third_party/python/Lib/encodings/utf_16.py						\
	third_party/python/Lib/encodings/utf_16_be.py						\
	third_party/python/Lib/encodings/utf_16_le.py						\
	third_party/python/Lib/encodings/utf_32.py						\
	third_party/python/Lib/encodings/utf_32_be.py						\
	third_party/python/Lib/encodings/utf_32_le.py						\
	third_party/python/Lib/encodings/utf_7.py						\
	third_party/python/Lib/encodings/utf_8.py						\
	third_party/python/Lib/encodings/utf_8_sig.py						\
	third_party/python/Lib/encodings/uu_codec.py						\
	third_party/python/Lib/encodings/zlib_codec.py						\
	third_party/python/Lib/ensurepip/							\
	third_party/python/Lib/ensurepip/__init__.py						\
	third_party/python/Lib/ensurepip/__main__.py						\
	third_party/python/Lib/ensurepip/_bundled						\
	third_party/python/Lib/ensurepip/_bundled/pip-18.1-py2.py3-none-any.whl			\
	third_party/python/Lib/ensurepip/_bundled/setuptools-40.6.2-py2.py3-none-any.whl	\
	third_party/python/Lib/ensurepip/_uninstall.py						\
	third_party/python/Lib/enum.py								\
	third_party/python/Lib/filecmp.py							\
	third_party/python/Lib/fileinput.py							\
	third_party/python/Lib/fnmatch.py							\
	third_party/python/Lib/formatter.py							\
	third_party/python/Lib/fractions.py							\
	third_party/python/Lib/ftplib.py							\
	third_party/python/Lib/functools.py							\
	third_party/python/Lib/genericpath.py							\
	third_party/python/Lib/getopt.py							\
	third_party/python/Lib/getpass.py							\
	third_party/python/Lib/gettext.py							\
	third_party/python/Lib/glob.py								\
	third_party/python/Lib/gzip.py								\
	third_party/python/Lib/hashlib.py							\
	third_party/python/Lib/heapq.py								\
	third_party/python/Lib/hmac.py								\
	third_party/python/Lib/html/								\
	third_party/python/Lib/html/__init__.py							\
	third_party/python/Lib/html/entities.py							\
	third_party/python/Lib/html/parser.py							\
	third_party/python/Lib/http/								\
	third_party/python/Lib/http/__init__.py							\
	third_party/python/Lib/http/client.py							\
	third_party/python/Lib/http/cookiejar.py						\
	third_party/python/Lib/http/cookies.py							\
	third_party/python/Lib/http/server.py							\
	third_party/python/Lib/imaplib.py							\
	third_party/python/Lib/imghdr.py							\
	third_party/python/Lib/imp.py								\
	third_party/python/Lib/importlib/							\
	third_party/python/Lib/importlib/__init__.py						\
	third_party/python/Lib/importlib/_bootstrap.py						\
	third_party/python/Lib/importlib/_bootstrap_external.py					\
	third_party/python/Lib/importlib/abc.py							\
	third_party/python/Lib/importlib/machinery.py						\
	third_party/python/Lib/importlib/util.py						\
	third_party/python/Lib/inspect.py							\
	third_party/python/Lib/io.py								\
	third_party/python/Lib/ipaddress.py							\
	third_party/python/Lib/json/								\
	third_party/python/Lib/json/__init__.py							\
	third_party/python/Lib/json/decoder.py							\
	third_party/python/Lib/json/encoder.py							\
	third_party/python/Lib/json/scanner.py							\
	third_party/python/Lib/json/tool.py							\
	third_party/python/Lib/keyword.py							\
	third_party/python/Lib/lib2to3/								\
	third_party/python/Lib/lib2to3/Grammar.txt						\
	third_party/python/Lib/lib2to3/PatternGrammar.txt					\
	third_party/python/Lib/lib2to3/__init__.py						\
	third_party/python/Lib/lib2to3/__main__.py						\
	third_party/python/Lib/lib2to3/btm_matcher.py						\
	third_party/python/Lib/lib2to3/btm_utils.py						\
	third_party/python/Lib/lib2to3/fixer_base.py						\
	third_party/python/Lib/lib2to3/fixer_util.py						\
	third_party/python/Lib/lib2to3/fixes/							\
	third_party/python/Lib/lib2to3/fixes/__init__.py					\
	third_party/python/Lib/lib2to3/fixes/fix_apply.py					\
	third_party/python/Lib/lib2to3/fixes/fix_asserts.py					\
	third_party/python/Lib/lib2to3/fixes/fix_basestring.py					\
	third_party/python/Lib/lib2to3/fixes/fix_buffer.py					\
	third_party/python/Lib/lib2to3/fixes/fix_dict.py					\
	third_party/python/Lib/lib2to3/fixes/fix_except.py					\
	third_party/python/Lib/lib2to3/fixes/fix_exec.py					\
	third_party/python/Lib/lib2to3/fixes/fix_execfile.py					\
	third_party/python/Lib/lib2to3/fixes/fix_exitfunc.py					\
	third_party/python/Lib/lib2to3/fixes/fix_filter.py					\
	third_party/python/Lib/lib2to3/fixes/fix_funcattrs.py					\
	third_party/python/Lib/lib2to3/fixes/fix_future.py					\
	third_party/python/Lib/lib2to3/fixes/fix_getcwdu.py					\
	third_party/python/Lib/lib2to3/fixes/fix_has_key.py					\
	third_party/python/Lib/lib2to3/fixes/fix_idioms.py					\
	third_party/python/Lib/lib2to3/fixes/fix_import.py					\
	third_party/python/Lib/lib2to3/fixes/fix_imports.py					\
	third_party/python/Lib/lib2to3/fixes/fix_imports2.py					\
	third_party/python/Lib/lib2to3/fixes/fix_input.py					\
	third_party/python/Lib/lib2to3/fixes/fix_intern.py					\
	third_party/python/Lib/lib2to3/fixes/fix_isinstance.py					\
	third_party/python/Lib/lib2to3/fixes/fix_itertools.py					\
	third_party/python/Lib/lib2to3/fixes/fix_itertools_imports.py				\
	third_party/python/Lib/lib2to3/fixes/fix_long.py					\
	third_party/python/Lib/lib2to3/fixes/fix_map.py						\
	third_party/python/Lib/lib2to3/fixes/fix_metaclass.py					\
	third_party/python/Lib/lib2to3/fixes/fix_methodattrs.py					\
	third_party/python/Lib/lib2to3/fixes/fix_ne.py						\
	third_party/python/Lib/lib2to3/fixes/fix_next.py					\
	third_party/python/Lib/lib2to3/fixes/fix_nonzero.py					\
	third_party/python/Lib/lib2to3/fixes/fix_numliterals.py					\
	third_party/python/Lib/lib2to3/fixes/fix_operator.py					\
	third_party/python/Lib/lib2to3/fixes/fix_paren.py					\
	third_party/python/Lib/lib2to3/fixes/fix_print.py					\
	third_party/python/Lib/lib2to3/fixes/fix_raise.py					\
	third_party/python/Lib/lib2to3/fixes/fix_raw_input.py					\
	third_party/python/Lib/lib2to3/fixes/fix_reduce.py					\
	third_party/python/Lib/lib2to3/fixes/fix_reload.py					\
	third_party/python/Lib/lib2to3/fixes/fix_renames.py					\
	third_party/python/Lib/lib2to3/fixes/fix_repr.py					\
	third_party/python/Lib/lib2to3/fixes/fix_set_literal.py					\
	third_party/python/Lib/lib2to3/fixes/fix_standarderror.py				\
	third_party/python/Lib/lib2to3/fixes/fix_sys_exc.py					\
	third_party/python/Lib/lib2to3/fixes/fix_throw.py					\
	third_party/python/Lib/lib2to3/fixes/fix_tuple_params.py				\
	third_party/python/Lib/lib2to3/fixes/fix_types.py					\
	third_party/python/Lib/lib2to3/fixes/fix_unicode.py					\
	third_party/python/Lib/lib2to3/fixes/fix_urllib.py					\
	third_party/python/Lib/lib2to3/fixes/fix_ws_comma.py					\
	third_party/python/Lib/lib2to3/fixes/fix_xrange.py					\
	third_party/python/Lib/lib2to3/fixes/fix_xreadlines.py					\
	third_party/python/Lib/lib2to3/fixes/fix_zip.py						\
	third_party/python/Lib/lib2to3/main.py							\
	third_party/python/Lib/lib2to3/patcomp.py						\
	third_party/python/Lib/lib2to3/pgen2/							\
	third_party/python/Lib/lib2to3/pgen2/__init__.py					\
	third_party/python/Lib/lib2to3/pgen2/conv.py						\
	third_party/python/Lib/lib2to3/pgen2/driver.py						\
	third_party/python/Lib/lib2to3/pgen2/grammar.py						\
	third_party/python/Lib/lib2to3/pgen2/literals.py					\
	third_party/python/Lib/lib2to3/pgen2/parse.py						\
	third_party/python/Lib/lib2to3/pgen2/pgen.py						\
	third_party/python/Lib/lib2to3/pgen2/token.py						\
	third_party/python/Lib/lib2to3/pgen2/tokenize.py					\
	third_party/python/Lib/lib2to3/pygram.py						\
	third_party/python/Lib/lib2to3/pytree.py						\
	third_party/python/Lib/lib2to3/refactor.py						\
	third_party/python/Lib/lib2to3/tests							\
	third_party/python/Lib/lib2to3/tests/__init__.py					\
	third_party/python/Lib/lib2to3/tests/__main__.py					\
	third_party/python/Lib/lib2to3/tests/data						\
	third_party/python/Lib/lib2to3/tests/data/README					\
	third_party/python/Lib/lib2to3/tests/data/bom.py					\
	third_party/python/Lib/lib2to3/tests/data/crlf.py					\
	third_party/python/Lib/lib2to3/tests/data/different_encoding.py				\
	third_party/python/Lib/lib2to3/tests/data/false_encoding.py				\
	third_party/python/Lib/lib2to3/tests/data/fixers					\
	third_party/python/Lib/lib2to3/tests/data/fixers/bad_order.py				\
	third_party/python/Lib/lib2to3/tests/data/fixers/myfixes				\
	third_party/python/Lib/lib2to3/tests/data/fixers/myfixes/__init__.py			\
	third_party/python/Lib/lib2to3/tests/data/fixers/myfixes/fix_explicit.py		\
	third_party/python/Lib/lib2to3/tests/data/fixers/myfixes/fix_first.py			\
	third_party/python/Lib/lib2to3/tests/data/fixers/myfixes/fix_last.py			\
	third_party/python/Lib/lib2to3/tests/data/fixers/myfixes/fix_parrot.py			\
	third_party/python/Lib/lib2to3/tests/data/fixers/myfixes/fix_preorder.py		\
	third_party/python/Lib/lib2to3/tests/data/fixers/no_fixer_cls.py			\
	third_party/python/Lib/lib2to3/tests/data/fixers/parrot_example.py			\
	third_party/python/Lib/lib2to3/tests/data/infinite_recursion.py				\
	third_party/python/Lib/lib2to3/tests/data/py2_test_grammar.py				\
	third_party/python/Lib/lib2to3/tests/data/py3_test_grammar.py				\
	third_party/python/Lib/lib2to3/tests/pytree_idempotency.py				\
	third_party/python/Lib/lib2to3/tests/support.py						\
	third_party/python/Lib/lib2to3/tests/test_all_fixers.py					\
	third_party/python/Lib/lib2to3/tests/test_fixers.py					\
	third_party/python/Lib/lib2to3/tests/test_main.py					\
	third_party/python/Lib/lib2to3/tests/test_parser.py					\
	third_party/python/Lib/lib2to3/tests/test_pytree.py					\
	third_party/python/Lib/lib2to3/tests/test_refactor.py					\
	third_party/python/Lib/lib2to3/tests/test_util.py					\
	third_party/python/Lib/linecache.py							\
	third_party/python/Lib/locale.py							\
	third_party/python/Lib/logging/								\
	third_party/python/Lib/logging/__init__.py						\
	third_party/python/Lib/logging/config.py						\
	third_party/python/Lib/logging/handlers.py						\
	third_party/python/Lib/lzma.py								\
	third_party/python/Lib/macpath.py							\
	third_party/python/Lib/macurl2path.py							\
	third_party/python/Lib/mailbox.py							\
	third_party/python/Lib/mailcap.py							\
	third_party/python/Lib/mimetypes.py							\
	third_party/python/Lib/modulefinder.py							\
	third_party/python/Lib/msilib/								\
	third_party/python/Lib/msilib/__init__.py						\
	third_party/python/Lib/msilib/schema.py							\
	third_party/python/Lib/msilib/sequence.py						\
	third_party/python/Lib/msilib/text.py							\
	third_party/python/Lib/multiprocessing/							\
	third_party/python/Lib/multiprocessing/__init__.py					\
	third_party/python/Lib/multiprocessing/connection.py					\
	third_party/python/Lib/multiprocessing/context.py					\
	third_party/python/Lib/multiprocessing/dummy/						\
	third_party/python/Lib/multiprocessing/dummy/__init__.py				\
	third_party/python/Lib/multiprocessing/dummy/connection.py				\
	third_party/python/Lib/multiprocessing/forkserver.py					\
	third_party/python/Lib/multiprocessing/heap.py						\
	third_party/python/Lib/multiprocessing/managers.py					\
	third_party/python/Lib/multiprocessing/pool.py						\
	third_party/python/Lib/multiprocessing/popen_fork.py					\
	third_party/python/Lib/multiprocessing/popen_forkserver.py				\
	third_party/python/Lib/multiprocessing/popen_spawn_posix.py				\
	third_party/python/Lib/multiprocessing/popen_spawn_win32.py				\
	third_party/python/Lib/multiprocessing/process.py					\
	third_party/python/Lib/multiprocessing/queues.py					\
	third_party/python/Lib/multiprocessing/reduction.py					\
	third_party/python/Lib/multiprocessing/resource_sharer.py				\
	third_party/python/Lib/multiprocessing/semaphore_tracker.py				\
	third_party/python/Lib/multiprocessing/sharedctypes.py					\
	third_party/python/Lib/multiprocessing/spawn.py						\
	third_party/python/Lib/multiprocessing/synchronize.py					\
	third_party/python/Lib/multiprocessing/util.py						\
	third_party/python/Lib/netrc.py								\
	third_party/python/Lib/nntplib.py							\
	third_party/python/Lib/ntpath.py							\
	third_party/python/Lib/nturl2path.py							\
	third_party/python/Lib/numbers.py							\
	third_party/python/Lib/opcode.py							\
	third_party/python/Lib/operator.py							\
	third_party/python/Lib/optparse.py							\
	third_party/python/Lib/os.py								\
	third_party/python/Lib/pathlib.py							\
	third_party/python/Lib/pdb.py								\
	third_party/python/Lib/pickle.py							\
	third_party/python/Lib/pickletools.py							\
	third_party/python/Lib/pipes.py								\
	third_party/python/Lib/pkgutil.py							\
	third_party/python/Lib/platform.py							\
	third_party/python/Lib/plistlib.py							\
	third_party/python/Lib/poplib.py							\
	third_party/python/Lib/posixpath.py							\
	third_party/python/Lib/pprint.py							\
	third_party/python/Lib/profile.py							\
	third_party/python/Lib/pstats.py							\
	third_party/python/Lib/pty.py								\
	third_party/python/Lib/py_compile.py							\
	third_party/python/Lib/pyclbr.py							\
	third_party/python/Lib/pydoc.py								\
	third_party/python/Lib/queue.py								\
	third_party/python/Lib/quopri.py							\
	third_party/python/Lib/random.py							\
	third_party/python/Lib/re.py								\
	third_party/python/Lib/reprlib.py							\
	third_party/python/Lib/rlcompleter.py							\
	third_party/python/Lib/runpy.py								\
	third_party/python/Lib/sched.py								\
	third_party/python/Lib/secrets.py							\
	third_party/python/Lib/selectors.py							\
	third_party/python/Lib/shelve.py							\
	third_party/python/Lib/shlex.py								\
	third_party/python/Lib/shutil.py							\
	third_party/python/Lib/signal.py							\
	third_party/python/Lib/site.py								\
	third_party/python/Lib/smtpd.py								\
	third_party/python/Lib/smtplib.py							\
	third_party/python/Lib/sndhdr.py							\
	third_party/python/Lib/socket.py							\
	third_party/python/Lib/socketserver.py							\
	third_party/python/Lib/sqlite3/								\
	third_party/python/Lib/sqlite3/__init__.py						\
	third_party/python/Lib/sqlite3/dbapi2.py						\
	third_party/python/Lib/sqlite3/dump.py							\
	third_party/python/Lib/sre_compile.py							\
	third_party/python/Lib/sre_constants.py							\
	third_party/python/Lib/sre_parse.py							\
	third_party/python/Lib/ssl.py								\
	third_party/python/Lib/stat.py								\
	third_party/python/Lib/statistics.py							\
	third_party/python/Lib/string.py							\
	third_party/python/Lib/stringprep.py							\
	third_party/python/Lib/struct.py							\
	third_party/python/Lib/subprocess.py							\
	third_party/python/Lib/sunau.py								\
	third_party/python/Lib/symbol.py							\
	third_party/python/Lib/symtable.py							\
	third_party/python/Lib/sysconfig.py							\
	third_party/python/Lib/tabnanny.py							\
	third_party/python/Lib/tarfile.py							\
	third_party/python/Lib/telnetlib.py							\
	third_party/python/Lib/tempfile.py							\
	third_party/python/Lib/textwrap.py							\
	third_party/python/Lib/this.py								\
	third_party/python/Lib/threading.py							\
	third_party/python/Lib/timeit.py							\
	third_party/python/Lib/token.py								\
	third_party/python/Lib/tokenize.py							\
	third_party/python/Lib/trace.py								\
	third_party/python/Lib/traceback.py							\
	third_party/python/Lib/tracemalloc.py							\
	third_party/python/Lib/tty.py								\
	third_party/python/Lib/types.py								\
	third_party/python/Lib/typing.py							\
	third_party/python/Lib/unittest/							\
	third_party/python/Lib/unittest/__init__.py						\
	third_party/python/Lib/unittest/__main__.py						\
	third_party/python/Lib/unittest/case.py							\
	third_party/python/Lib/unittest/loader.py						\
	third_party/python/Lib/unittest/main.py							\
	third_party/python/Lib/unittest/mock.py							\
	third_party/python/Lib/unittest/result.py						\
	third_party/python/Lib/unittest/runner.py						\
	third_party/python/Lib/unittest/signals.py						\
	third_party/python/Lib/unittest/suite.py						\
	third_party/python/Lib/unittest/util.py							\
	third_party/python/Lib/urllib/								\
	third_party/python/Lib/urllib/__init__.py						\
	third_party/python/Lib/urllib/error.py							\
	third_party/python/Lib/urllib/parse.py							\
	third_party/python/Lib/urllib/request.py						\
	third_party/python/Lib/urllib/response.py						\
	third_party/python/Lib/urllib/robotparser.py						\
	third_party/python/Lib/uu.py								\
	third_party/python/Lib/uuid.py								\
	third_party/python/Lib/venv/								\
	third_party/python/Lib/venv/__init__.py							\
	third_party/python/Lib/venv/__main__.py							\
	third_party/python/Lib/venv/scripts/common/						\
	third_party/python/Lib/venv/scripts/common/activate					\
	third_party/python/Lib/venv/scripts/nt/							\
	third_party/python/Lib/venv/scripts/nt/Activate.ps1					\
	third_party/python/Lib/venv/scripts/nt/activate.bat					\
	third_party/python/Lib/venv/scripts/nt/deactivate.bat					\
	third_party/python/Lib/venv/scripts/posix/						\
	third_party/python/Lib/venv/scripts/posix/activate.csh					\
	third_party/python/Lib/venv/scripts/posix/activate.fish					\
	third_party/python/Lib/warnings.py							\
	third_party/python/Lib/wave.py								\
	third_party/python/Lib/weakref.py							\
	third_party/python/Lib/webbrowser.py							\
	third_party/python/Lib/wsgiref/								\
	third_party/python/Lib/wsgiref/__init__.py						\
	third_party/python/Lib/wsgiref/handlers.py						\
	third_party/python/Lib/wsgiref/headers.py						\
	third_party/python/Lib/wsgiref/simple_server.py						\
	third_party/python/Lib/wsgiref/util.py							\
	third_party/python/Lib/wsgiref/validate.py						\
	third_party/python/Lib/xdrlib.py							\
	third_party/python/Lib/xml/								\
	third_party/python/Lib/xml/__init__.py							\
	third_party/python/Lib/xml/dom/								\
	third_party/python/Lib/xml/dom/NodeFilter.py						\
	third_party/python/Lib/xml/dom/__init__.py						\
	third_party/python/Lib/xml/dom/domreg.py						\
	third_party/python/Lib/xml/dom/expatbuilder.py						\
	third_party/python/Lib/xml/dom/minicompat.py						\
	third_party/python/Lib/xml/dom/minidom.py						\
	third_party/python/Lib/xml/dom/pulldom.py						\
	third_party/python/Lib/xml/dom/xmlbuilder.py						\
	third_party/python/Lib/xml/etree/							\
	third_party/python/Lib/xml/etree/ElementInclude.py					\
	third_party/python/Lib/xml/etree/ElementPath.py						\
	third_party/python/Lib/xml/etree/ElementTree.py						\
	third_party/python/Lib/xml/etree/__init__.py						\
	third_party/python/Lib/xml/etree/cElementTree.py					\
	third_party/python/Lib/xml/parsers/							\
	third_party/python/Lib/xml/parsers/__init__.py						\
	third_party/python/Lib/xml/parsers/expat.py						\
	third_party/python/Lib/xml/sax/								\
	third_party/python/Lib/xml/sax/__init__.py						\
	third_party/python/Lib/xml/sax/_exceptions.py						\
	third_party/python/Lib/xml/sax/expatreader.py						\
	third_party/python/Lib/xml/sax/handler.py						\
	third_party/python/Lib/xml/sax/saxutils.py						\
	third_party/python/Lib/xml/sax/xmlreader.py						\
	third_party/python/Lib/xmlrpc/								\
	third_party/python/Lib/xmlrpc/__init__.py						\
	third_party/python/Lib/xmlrpc/client.py							\
	third_party/python/Lib/xmlrpc/server.py							\
	third_party/python/Lib/zipapp.py							\
	third_party/python/Lib/zipfile.py

THIRD_PARTY_PYTHON_STDLIB_PY_OBJS =								\
	$(THIRD_PARTY_PYTHON_STDLIB_PY:%=o/$(MODE)/%.zip.o)

$(THIRD_PARTY_PYTHON_STDLIB_PY_OBJS):								\
		third_party/python/python-stdlib.mk

$(THIRD_PARTY_PYTHON_STDLIB_PY_OBJS):								\
		ZIPOBJ_FLAGS +=									\
			-P.python								\
			-C3

o/$(MODE)/third_party/python/Lib/__pycache__/:	\
		o/$(MODE)/third_party/python/Lib/
	@mkdir -p $@

o/$(MODE)/third_party/python/Lib/__pycache__/.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/__future__.cpython-36.pyc:	\
		third_party/python/Lib/__future__.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/__future__.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/__future__.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/_bootlocale.cpython-36.pyc:	\
		third_party/python/Lib/_bootlocale.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/_bootlocale.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/_bootlocale.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/_collections_abc.cpython-36.pyc:	\
		third_party/python/Lib/_collections_abc.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/_collections_abc.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/_collections_abc.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/_compat_pickle.cpython-36.pyc:	\
		third_party/python/Lib/_compat_pickle.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/_compat_pickle.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/_compat_pickle.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/_compression.cpython-36.pyc:	\
		third_party/python/Lib/_compression.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/_compression.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/_compression.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/_dummy_thread.cpython-36.pyc:	\
		third_party/python/Lib/_dummy_thread.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/_dummy_thread.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/_dummy_thread.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/_markupbase.cpython-36.pyc:	\
		third_party/python/Lib/_markupbase.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/_markupbase.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/_markupbase.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/_osx_support.cpython-36.pyc:	\
		third_party/python/Lib/_osx_support.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/_osx_support.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/_osx_support.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/_pyio.cpython-36.pyc:	\
		third_party/python/Lib/_pyio.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/_pyio.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/_pyio.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/_sitebuiltins.cpython-36.pyc:	\
		third_party/python/Lib/_sitebuiltins.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/_sitebuiltins.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/_sitebuiltins.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/_strptime.cpython-36.pyc:	\
		third_party/python/Lib/_strptime.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/_strptime.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/_strptime.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/_sysconfigdata_m_cosmo_x86_64-cosmo.cpython-36.pyc:	\
		third_party/python/Lib/_sysconfigdata_m_cosmo_x86_64-cosmo.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/_sysconfigdata_m_cosmo_x86_64-cosmo.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/_sysconfigdata_m_cosmo_x86_64-cosmo.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/_threading_local.cpython-36.pyc:	\
		third_party/python/Lib/_threading_local.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/_threading_local.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/_threading_local.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/_weakrefset.cpython-36.pyc:	\
		third_party/python/Lib/_weakrefset.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/_weakrefset.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/_weakrefset.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/abc.cpython-36.pyc:	\
		third_party/python/Lib/abc.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/abc.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/abc.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/aifc.cpython-36.pyc:	\
		third_party/python/Lib/aifc.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/aifc.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/aifc.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/antigravity.cpython-36.pyc:	\
		third_party/python/Lib/antigravity.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/antigravity.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/antigravity.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/argparse.cpython-36.pyc:	\
		third_party/python/Lib/argparse.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/argparse.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/argparse.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/ast.cpython-36.pyc:	\
		third_party/python/Lib/ast.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/ast.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/ast.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/asynchat.cpython-36.pyc:	\
		third_party/python/Lib/asynchat.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/asynchat.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/asynchat.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/asyncio/__pycache__/:	\
		o/$(MODE)/third_party/python/Lib/asyncio/
	@mkdir -p $@

o/$(MODE)/third_party/python/Lib/asyncio/__pycache__/.zip.o:	\
		o/$(MODE)/third_party/python/Lib/asyncio/__pycache__/
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/asyncio/__pycache__/__init__.cpython-36.pyc:	\
		third_party/python/Lib/asyncio/__init__.py	\
		o/$(MODE)/third_party/python/Lib/asyncio/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/asyncio/__pycache__/__init__.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/asyncio/__pycache__/__init__.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/asyncio/__pycache__/base_events.cpython-36.pyc:	\
		third_party/python/Lib/asyncio/base_events.py	\
		o/$(MODE)/third_party/python/Lib/asyncio/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/asyncio/__pycache__/base_events.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/asyncio/__pycache__/base_events.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/asyncio/__pycache__/base_futures.cpython-36.pyc:	\
		third_party/python/Lib/asyncio/base_futures.py	\
		o/$(MODE)/third_party/python/Lib/asyncio/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/asyncio/__pycache__/base_futures.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/asyncio/__pycache__/base_futures.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/asyncio/__pycache__/base_subprocess.cpython-36.pyc:	\
		third_party/python/Lib/asyncio/base_subprocess.py	\
		o/$(MODE)/third_party/python/Lib/asyncio/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/asyncio/__pycache__/base_subprocess.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/asyncio/__pycache__/base_subprocess.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/asyncio/__pycache__/base_tasks.cpython-36.pyc:	\
		third_party/python/Lib/asyncio/base_tasks.py	\
		o/$(MODE)/third_party/python/Lib/asyncio/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/asyncio/__pycache__/base_tasks.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/asyncio/__pycache__/base_tasks.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/asyncio/__pycache__/compat.cpython-36.pyc:	\
		third_party/python/Lib/asyncio/compat.py	\
		o/$(MODE)/third_party/python/Lib/asyncio/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/asyncio/__pycache__/compat.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/asyncio/__pycache__/compat.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/asyncio/__pycache__/constants.cpython-36.pyc:	\
		third_party/python/Lib/asyncio/constants.py	\
		o/$(MODE)/third_party/python/Lib/asyncio/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/asyncio/__pycache__/constants.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/asyncio/__pycache__/constants.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/asyncio/__pycache__/coroutines.cpython-36.pyc:	\
		third_party/python/Lib/asyncio/coroutines.py	\
		o/$(MODE)/third_party/python/Lib/asyncio/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/asyncio/__pycache__/coroutines.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/asyncio/__pycache__/coroutines.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/asyncio/__pycache__/events.cpython-36.pyc:	\
		third_party/python/Lib/asyncio/events.py	\
		o/$(MODE)/third_party/python/Lib/asyncio/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/asyncio/__pycache__/events.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/asyncio/__pycache__/events.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/asyncio/__pycache__/futures.cpython-36.pyc:	\
		third_party/python/Lib/asyncio/futures.py	\
		o/$(MODE)/third_party/python/Lib/asyncio/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/asyncio/__pycache__/futures.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/asyncio/__pycache__/futures.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/asyncio/__pycache__/locks.cpython-36.pyc:	\
		third_party/python/Lib/asyncio/locks.py	\
		o/$(MODE)/third_party/python/Lib/asyncio/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/asyncio/__pycache__/locks.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/asyncio/__pycache__/locks.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/asyncio/__pycache__/log.cpython-36.pyc:	\
		third_party/python/Lib/asyncio/log.py	\
		o/$(MODE)/third_party/python/Lib/asyncio/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/asyncio/__pycache__/log.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/asyncio/__pycache__/log.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/asyncio/__pycache__/proactor_events.cpython-36.pyc:	\
		third_party/python/Lib/asyncio/proactor_events.py	\
		o/$(MODE)/third_party/python/Lib/asyncio/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/asyncio/__pycache__/proactor_events.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/asyncio/__pycache__/proactor_events.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/asyncio/__pycache__/protocols.cpython-36.pyc:	\
		third_party/python/Lib/asyncio/protocols.py	\
		o/$(MODE)/third_party/python/Lib/asyncio/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/asyncio/__pycache__/protocols.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/asyncio/__pycache__/protocols.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/asyncio/__pycache__/queues.cpython-36.pyc:	\
		third_party/python/Lib/asyncio/queues.py	\
		o/$(MODE)/third_party/python/Lib/asyncio/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/asyncio/__pycache__/queues.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/asyncio/__pycache__/queues.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/asyncio/__pycache__/selector_events.cpython-36.pyc:	\
		third_party/python/Lib/asyncio/selector_events.py	\
		o/$(MODE)/third_party/python/Lib/asyncio/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/asyncio/__pycache__/selector_events.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/asyncio/__pycache__/selector_events.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/asyncio/__pycache__/sslproto.cpython-36.pyc:	\
		third_party/python/Lib/asyncio/sslproto.py	\
		o/$(MODE)/third_party/python/Lib/asyncio/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/asyncio/__pycache__/sslproto.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/asyncio/__pycache__/sslproto.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/asyncio/__pycache__/streams.cpython-36.pyc:	\
		third_party/python/Lib/asyncio/streams.py	\
		o/$(MODE)/third_party/python/Lib/asyncio/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/asyncio/__pycache__/streams.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/asyncio/__pycache__/streams.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/asyncio/__pycache__/subprocess.cpython-36.pyc:	\
		third_party/python/Lib/asyncio/subprocess.py	\
		o/$(MODE)/third_party/python/Lib/asyncio/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/asyncio/__pycache__/subprocess.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/asyncio/__pycache__/subprocess.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/asyncio/__pycache__/tasks.cpython-36.pyc:	\
		third_party/python/Lib/asyncio/tasks.py	\
		o/$(MODE)/third_party/python/Lib/asyncio/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/asyncio/__pycache__/tasks.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/asyncio/__pycache__/tasks.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/asyncio/__pycache__/test_utils.cpython-36.pyc:	\
		third_party/python/Lib/asyncio/test_utils.py	\
		o/$(MODE)/third_party/python/Lib/asyncio/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/asyncio/__pycache__/test_utils.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/asyncio/__pycache__/test_utils.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/asyncio/__pycache__/transports.cpython-36.pyc:	\
		third_party/python/Lib/asyncio/transports.py	\
		o/$(MODE)/third_party/python/Lib/asyncio/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/asyncio/__pycache__/transports.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/asyncio/__pycache__/transports.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/asyncio/__pycache__/unix_events.cpython-36.pyc:	\
		third_party/python/Lib/asyncio/unix_events.py	\
		o/$(MODE)/third_party/python/Lib/asyncio/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/asyncio/__pycache__/unix_events.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/asyncio/__pycache__/unix_events.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/asyncio/__pycache__/windows_events.cpython-36.pyc:	\
		third_party/python/Lib/asyncio/windows_events.py	\
		o/$(MODE)/third_party/python/Lib/asyncio/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/asyncio/__pycache__/windows_events.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/asyncio/__pycache__/windows_events.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/asyncio/__pycache__/windows_utils.cpython-36.pyc:	\
		third_party/python/Lib/asyncio/windows_utils.py	\
		o/$(MODE)/third_party/python/Lib/asyncio/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/asyncio/__pycache__/windows_utils.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/asyncio/__pycache__/windows_utils.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/asyncore.cpython-36.pyc:	\
		third_party/python/Lib/asyncore.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/asyncore.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/asyncore.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/base64.cpython-36.pyc:	\
		third_party/python/Lib/base64.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/base64.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/base64.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/bdb.cpython-36.pyc:	\
		third_party/python/Lib/bdb.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/bdb.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/bdb.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/binhex.cpython-36.pyc:	\
		third_party/python/Lib/binhex.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/binhex.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/binhex.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/bisect.cpython-36.pyc:	\
		third_party/python/Lib/bisect.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/bisect.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/bisect.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/bz2.cpython-36.pyc:	\
		third_party/python/Lib/bz2.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/bz2.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/bz2.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/cProfile.cpython-36.pyc:	\
		third_party/python/Lib/cProfile.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/cProfile.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/cProfile.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/calendar.cpython-36.pyc:	\
		third_party/python/Lib/calendar.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/calendar.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/calendar.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/cgi.cpython-36.pyc:	\
		third_party/python/Lib/cgi.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/cgi.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/cgi.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/cgitb.cpython-36.pyc:	\
		third_party/python/Lib/cgitb.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/cgitb.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/cgitb.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/chunk.cpython-36.pyc:	\
		third_party/python/Lib/chunk.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/chunk.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/chunk.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/cmd.cpython-36.pyc:	\
		third_party/python/Lib/cmd.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/cmd.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/cmd.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/code.cpython-36.pyc:	\
		third_party/python/Lib/code.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/code.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/code.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/codecs.cpython-36.pyc:	\
		third_party/python/Lib/codecs.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/codecs.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/codecs.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/codeop.cpython-36.pyc:	\
		third_party/python/Lib/codeop.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/codeop.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/codeop.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/collections/__pycache__/:	\
		o/$(MODE)/third_party/python/Lib/collections/
	@mkdir -p $@

o/$(MODE)/third_party/python/Lib/collections/__pycache__/.zip.o:	\
		o/$(MODE)/third_party/python/Lib/collections/__pycache__/
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/collections/__pycache__/__init__.cpython-36.pyc:	\
		third_party/python/Lib/collections/__init__.py	\
		o/$(MODE)/third_party/python/Lib/collections/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/collections/__pycache__/__init__.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/collections/__pycache__/__init__.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/collections/__pycache__/abc.cpython-36.pyc:	\
		third_party/python/Lib/collections/abc.py	\
		o/$(MODE)/third_party/python/Lib/collections/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/collections/__pycache__/abc.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/collections/__pycache__/abc.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/colorsys.cpython-36.pyc:	\
		third_party/python/Lib/colorsys.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/colorsys.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/colorsys.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/compileall.cpython-36.pyc:	\
		third_party/python/Lib/compileall.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/compileall.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/compileall.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/configparser.cpython-36.pyc:	\
		third_party/python/Lib/configparser.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/configparser.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/configparser.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/contextlib.cpython-36.pyc:	\
		third_party/python/Lib/contextlib.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/contextlib.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/contextlib.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/copy.cpython-36.pyc:	\
		third_party/python/Lib/copy.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/copy.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/copy.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/copyreg.cpython-36.pyc:	\
		third_party/python/Lib/copyreg.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/copyreg.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/copyreg.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/crypt.cpython-36.pyc:	\
		third_party/python/Lib/crypt.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/crypt.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/crypt.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/csv.cpython-36.pyc:	\
		third_party/python/Lib/csv.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/csv.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/csv.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/datetime.cpython-36.pyc:	\
		third_party/python/Lib/datetime.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/datetime.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/datetime.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/dbm/__pycache__/:	\
		o/$(MODE)/third_party/python/Lib/dbm/
	@mkdir -p $@

o/$(MODE)/third_party/python/Lib/dbm/__pycache__/.zip.o:	\
		o/$(MODE)/third_party/python/Lib/dbm/__pycache__/
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/dbm/__pycache__/__init__.cpython-36.pyc:	\
		third_party/python/Lib/dbm/__init__.py	\
		o/$(MODE)/third_party/python/Lib/dbm/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/dbm/__pycache__/__init__.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/dbm/__pycache__/__init__.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/dbm/__pycache__/dumb.cpython-36.pyc:	\
		third_party/python/Lib/dbm/dumb.py	\
		o/$(MODE)/third_party/python/Lib/dbm/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/dbm/__pycache__/dumb.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/dbm/__pycache__/dumb.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/dbm/__pycache__/gnu.cpython-36.pyc:	\
		third_party/python/Lib/dbm/gnu.py	\
		o/$(MODE)/third_party/python/Lib/dbm/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/dbm/__pycache__/gnu.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/dbm/__pycache__/gnu.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/dbm/__pycache__/ndbm.cpython-36.pyc:	\
		third_party/python/Lib/dbm/ndbm.py	\
		o/$(MODE)/third_party/python/Lib/dbm/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/dbm/__pycache__/ndbm.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/dbm/__pycache__/ndbm.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/decimal.cpython-36.pyc:	\
		third_party/python/Lib/decimal.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/decimal.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/decimal.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/difflib.cpython-36.pyc:	\
		third_party/python/Lib/difflib.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/difflib.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/difflib.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/dis.cpython-36.pyc:	\
		third_party/python/Lib/dis.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/dis.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/dis.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/__pycache__/:	\
		o/$(MODE)/third_party/python/Lib/distutils/
	@mkdir -p $@

o/$(MODE)/third_party/python/Lib/distutils/__pycache__/.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/__pycache__/
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/__pycache__/__init__.cpython-36.pyc:	\
		third_party/python/Lib/distutils/__init__.py	\
		o/$(MODE)/third_party/python/Lib/distutils/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/__pycache__/__init__.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/__pycache__/__init__.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/__pycache__/_msvccompiler.cpython-36.pyc:	\
		third_party/python/Lib/distutils/_msvccompiler.py	\
		o/$(MODE)/third_party/python/Lib/distutils/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/__pycache__/_msvccompiler.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/__pycache__/_msvccompiler.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/__pycache__/archive_util.cpython-36.pyc:	\
		third_party/python/Lib/distutils/archive_util.py	\
		o/$(MODE)/third_party/python/Lib/distutils/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/__pycache__/archive_util.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/__pycache__/archive_util.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/__pycache__/bcppcompiler.cpython-36.pyc:	\
		third_party/python/Lib/distutils/bcppcompiler.py	\
		o/$(MODE)/third_party/python/Lib/distutils/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/__pycache__/bcppcompiler.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/__pycache__/bcppcompiler.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/__pycache__/ccompiler.cpython-36.pyc:	\
		third_party/python/Lib/distutils/ccompiler.py	\
		o/$(MODE)/third_party/python/Lib/distutils/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/__pycache__/ccompiler.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/__pycache__/ccompiler.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/__pycache__/cmd.cpython-36.pyc:	\
		third_party/python/Lib/distutils/cmd.py	\
		o/$(MODE)/third_party/python/Lib/distutils/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/__pycache__/cmd.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/__pycache__/cmd.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/command/__pycache__/:	\
		o/$(MODE)/third_party/python/Lib/distutils/command/
	@mkdir -p $@

o/$(MODE)/third_party/python/Lib/distutils/command/__pycache__/.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/command/__pycache__/
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/command/__pycache__/__init__.cpython-36.pyc:	\
		third_party/python/Lib/distutils/command/__init__.py	\
		o/$(MODE)/third_party/python/Lib/distutils/command/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/command/__pycache__/__init__.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/command/__pycache__/__init__.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/command/__pycache__/bdist.cpython-36.pyc:	\
		third_party/python/Lib/distutils/command/bdist.py	\
		o/$(MODE)/third_party/python/Lib/distutils/command/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/command/__pycache__/bdist.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/command/__pycache__/bdist.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/command/__pycache__/bdist_dumb.cpython-36.pyc:	\
		third_party/python/Lib/distutils/command/bdist_dumb.py	\
		o/$(MODE)/third_party/python/Lib/distutils/command/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/command/__pycache__/bdist_dumb.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/command/__pycache__/bdist_dumb.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/command/__pycache__/bdist_msi.cpython-36.pyc:	\
		third_party/python/Lib/distutils/command/bdist_msi.py	\
		o/$(MODE)/third_party/python/Lib/distutils/command/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/command/__pycache__/bdist_msi.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/command/__pycache__/bdist_msi.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/command/__pycache__/bdist_rpm.cpython-36.pyc:	\
		third_party/python/Lib/distutils/command/bdist_rpm.py	\
		o/$(MODE)/third_party/python/Lib/distutils/command/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/command/__pycache__/bdist_rpm.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/command/__pycache__/bdist_rpm.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/command/__pycache__/bdist_wininst.cpython-36.pyc:	\
		third_party/python/Lib/distutils/command/bdist_wininst.py	\
		o/$(MODE)/third_party/python/Lib/distutils/command/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/command/__pycache__/bdist_wininst.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/command/__pycache__/bdist_wininst.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/command/__pycache__/build.cpython-36.pyc:	\
		third_party/python/Lib/distutils/command/build.py	\
		o/$(MODE)/third_party/python/Lib/distutils/command/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/command/__pycache__/build.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/command/__pycache__/build.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/command/__pycache__/build_clib.cpython-36.pyc:	\
		third_party/python/Lib/distutils/command/build_clib.py	\
		o/$(MODE)/third_party/python/Lib/distutils/command/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/command/__pycache__/build_clib.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/command/__pycache__/build_clib.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/command/__pycache__/build_ext.cpython-36.pyc:	\
		third_party/python/Lib/distutils/command/build_ext.py	\
		o/$(MODE)/third_party/python/Lib/distutils/command/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/command/__pycache__/build_ext.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/command/__pycache__/build_ext.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/command/__pycache__/build_py.cpython-36.pyc:	\
		third_party/python/Lib/distutils/command/build_py.py	\
		o/$(MODE)/third_party/python/Lib/distutils/command/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/command/__pycache__/build_py.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/command/__pycache__/build_py.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/command/__pycache__/build_scripts.cpython-36.pyc:	\
		third_party/python/Lib/distutils/command/build_scripts.py	\
		o/$(MODE)/third_party/python/Lib/distutils/command/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/command/__pycache__/build_scripts.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/command/__pycache__/build_scripts.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/command/__pycache__/check.cpython-36.pyc:	\
		third_party/python/Lib/distutils/command/check.py	\
		o/$(MODE)/third_party/python/Lib/distutils/command/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/command/__pycache__/check.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/command/__pycache__/check.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/command/__pycache__/clean.cpython-36.pyc:	\
		third_party/python/Lib/distutils/command/clean.py	\
		o/$(MODE)/third_party/python/Lib/distutils/command/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/command/__pycache__/clean.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/command/__pycache__/clean.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/command/__pycache__/config.cpython-36.pyc:	\
		third_party/python/Lib/distutils/command/config.py	\
		o/$(MODE)/third_party/python/Lib/distutils/command/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/command/__pycache__/config.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/command/__pycache__/config.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/command/__pycache__/install.cpython-36.pyc:	\
		third_party/python/Lib/distutils/command/install.py	\
		o/$(MODE)/third_party/python/Lib/distutils/command/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/command/__pycache__/install.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/command/__pycache__/install.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/command/__pycache__/install_data.cpython-36.pyc:	\
		third_party/python/Lib/distutils/command/install_data.py	\
		o/$(MODE)/third_party/python/Lib/distutils/command/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/command/__pycache__/install_data.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/command/__pycache__/install_data.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/command/__pycache__/install_egg_info.cpython-36.pyc:	\
		third_party/python/Lib/distutils/command/install_egg_info.py	\
		o/$(MODE)/third_party/python/Lib/distutils/command/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/command/__pycache__/install_egg_info.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/command/__pycache__/install_egg_info.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/command/__pycache__/install_headers.cpython-36.pyc:	\
		third_party/python/Lib/distutils/command/install_headers.py	\
		o/$(MODE)/third_party/python/Lib/distutils/command/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/command/__pycache__/install_headers.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/command/__pycache__/install_headers.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/command/__pycache__/install_lib.cpython-36.pyc:	\
		third_party/python/Lib/distutils/command/install_lib.py	\
		o/$(MODE)/third_party/python/Lib/distutils/command/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/command/__pycache__/install_lib.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/command/__pycache__/install_lib.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/command/__pycache__/install_scripts.cpython-36.pyc:	\
		third_party/python/Lib/distutils/command/install_scripts.py	\
		o/$(MODE)/third_party/python/Lib/distutils/command/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/command/__pycache__/install_scripts.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/command/__pycache__/install_scripts.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/command/__pycache__/register.cpython-36.pyc:	\
		third_party/python/Lib/distutils/command/register.py	\
		o/$(MODE)/third_party/python/Lib/distutils/command/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/command/__pycache__/register.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/command/__pycache__/register.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/command/__pycache__/sdist.cpython-36.pyc:	\
		third_party/python/Lib/distutils/command/sdist.py	\
		o/$(MODE)/third_party/python/Lib/distutils/command/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/command/__pycache__/sdist.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/command/__pycache__/sdist.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/command/__pycache__/upload.cpython-36.pyc:	\
		third_party/python/Lib/distutils/command/upload.py	\
		o/$(MODE)/third_party/python/Lib/distutils/command/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/command/__pycache__/upload.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/command/__pycache__/upload.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/__pycache__/config.cpython-36.pyc:	\
		third_party/python/Lib/distutils/config.py	\
		o/$(MODE)/third_party/python/Lib/distutils/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/__pycache__/config.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/__pycache__/config.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/__pycache__/core.cpython-36.pyc:	\
		third_party/python/Lib/distutils/core.py	\
		o/$(MODE)/third_party/python/Lib/distutils/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/__pycache__/core.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/__pycache__/core.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/__pycache__/cygwinccompiler.cpython-36.pyc:	\
		third_party/python/Lib/distutils/cygwinccompiler.py	\
		o/$(MODE)/third_party/python/Lib/distutils/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/__pycache__/cygwinccompiler.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/__pycache__/cygwinccompiler.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/__pycache__/debug.cpython-36.pyc:	\
		third_party/python/Lib/distutils/debug.py	\
		o/$(MODE)/third_party/python/Lib/distutils/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/__pycache__/debug.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/__pycache__/debug.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/__pycache__/dep_util.cpython-36.pyc:	\
		third_party/python/Lib/distutils/dep_util.py	\
		o/$(MODE)/third_party/python/Lib/distutils/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/__pycache__/dep_util.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/__pycache__/dep_util.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/__pycache__/dir_util.cpython-36.pyc:	\
		third_party/python/Lib/distutils/dir_util.py	\
		o/$(MODE)/third_party/python/Lib/distutils/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/__pycache__/dir_util.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/__pycache__/dir_util.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/__pycache__/dist.cpython-36.pyc:	\
		third_party/python/Lib/distutils/dist.py	\
		o/$(MODE)/third_party/python/Lib/distutils/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/__pycache__/dist.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/__pycache__/dist.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/__pycache__/errors.cpython-36.pyc:	\
		third_party/python/Lib/distutils/errors.py	\
		o/$(MODE)/third_party/python/Lib/distutils/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/__pycache__/errors.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/__pycache__/errors.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/__pycache__/extension.cpython-36.pyc:	\
		third_party/python/Lib/distutils/extension.py	\
		o/$(MODE)/third_party/python/Lib/distutils/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/__pycache__/extension.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/__pycache__/extension.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/__pycache__/fancy_getopt.cpython-36.pyc:	\
		third_party/python/Lib/distutils/fancy_getopt.py	\
		o/$(MODE)/third_party/python/Lib/distutils/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/__pycache__/fancy_getopt.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/__pycache__/fancy_getopt.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/__pycache__/file_util.cpython-36.pyc:	\
		third_party/python/Lib/distutils/file_util.py	\
		o/$(MODE)/third_party/python/Lib/distutils/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/__pycache__/file_util.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/__pycache__/file_util.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/__pycache__/filelist.cpython-36.pyc:	\
		third_party/python/Lib/distutils/filelist.py	\
		o/$(MODE)/third_party/python/Lib/distutils/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/__pycache__/filelist.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/__pycache__/filelist.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/__pycache__/log.cpython-36.pyc:	\
		third_party/python/Lib/distutils/log.py	\
		o/$(MODE)/third_party/python/Lib/distutils/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/__pycache__/log.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/__pycache__/log.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/__pycache__/msvc9compiler.cpython-36.pyc:	\
		third_party/python/Lib/distutils/msvc9compiler.py	\
		o/$(MODE)/third_party/python/Lib/distutils/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/__pycache__/msvc9compiler.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/__pycache__/msvc9compiler.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/__pycache__/msvccompiler.cpython-36.pyc:	\
		third_party/python/Lib/distutils/msvccompiler.py	\
		o/$(MODE)/third_party/python/Lib/distutils/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/__pycache__/msvccompiler.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/__pycache__/msvccompiler.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/__pycache__/spawn.cpython-36.pyc:	\
		third_party/python/Lib/distutils/spawn.py	\
		o/$(MODE)/third_party/python/Lib/distutils/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/__pycache__/spawn.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/__pycache__/spawn.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/__pycache__/sysconfig.cpython-36.pyc:	\
		third_party/python/Lib/distutils/sysconfig.py	\
		o/$(MODE)/third_party/python/Lib/distutils/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/__pycache__/sysconfig.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/__pycache__/sysconfig.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/:	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/
	@mkdir -p $@

o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/__init__.cpython-36.pyc:	\
		third_party/python/Lib/distutils/tests/__init__.py	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/__init__.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/__init__.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/support.cpython-36.pyc:	\
		third_party/python/Lib/distutils/tests/support.py	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/support.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/support.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_archive_util.cpython-36.pyc:	\
		third_party/python/Lib/distutils/tests/test_archive_util.py	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_archive_util.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_archive_util.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_bdist.cpython-36.pyc:	\
		third_party/python/Lib/distutils/tests/test_bdist.py	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_bdist.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_bdist.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_bdist_dumb.cpython-36.pyc:	\
		third_party/python/Lib/distutils/tests/test_bdist_dumb.py	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_bdist_dumb.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_bdist_dumb.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_bdist_msi.cpython-36.pyc:	\
		third_party/python/Lib/distutils/tests/test_bdist_msi.py	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_bdist_msi.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_bdist_msi.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_bdist_rpm.cpython-36.pyc:	\
		third_party/python/Lib/distutils/tests/test_bdist_rpm.py	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_bdist_rpm.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_bdist_rpm.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_bdist_wininst.cpython-36.pyc:	\
		third_party/python/Lib/distutils/tests/test_bdist_wininst.py	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_bdist_wininst.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_bdist_wininst.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_build.cpython-36.pyc:	\
		third_party/python/Lib/distutils/tests/test_build.py	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_build.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_build.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_build_clib.cpython-36.pyc:	\
		third_party/python/Lib/distutils/tests/test_build_clib.py	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_build_clib.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_build_clib.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_build_ext.cpython-36.pyc:	\
		third_party/python/Lib/distutils/tests/test_build_ext.py	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_build_ext.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_build_ext.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_build_py.cpython-36.pyc:	\
		third_party/python/Lib/distutils/tests/test_build_py.py	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_build_py.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_build_py.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_build_scripts.cpython-36.pyc:	\
		third_party/python/Lib/distutils/tests/test_build_scripts.py	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_build_scripts.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_build_scripts.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_check.cpython-36.pyc:	\
		third_party/python/Lib/distutils/tests/test_check.py	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_check.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_check.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_clean.cpython-36.pyc:	\
		third_party/python/Lib/distutils/tests/test_clean.py	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_clean.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_clean.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_cmd.cpython-36.pyc:	\
		third_party/python/Lib/distutils/tests/test_cmd.py	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_cmd.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_cmd.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_config.cpython-36.pyc:	\
		third_party/python/Lib/distutils/tests/test_config.py	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_config.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_config.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_config_cmd.cpython-36.pyc:	\
		third_party/python/Lib/distutils/tests/test_config_cmd.py	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_config_cmd.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_config_cmd.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_core.cpython-36.pyc:	\
		third_party/python/Lib/distutils/tests/test_core.py	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_core.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_core.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_cygwinccompiler.cpython-36.pyc:	\
		third_party/python/Lib/distutils/tests/test_cygwinccompiler.py	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_cygwinccompiler.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_cygwinccompiler.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_dep_util.cpython-36.pyc:	\
		third_party/python/Lib/distutils/tests/test_dep_util.py	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_dep_util.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_dep_util.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_dir_util.cpython-36.pyc:	\
		third_party/python/Lib/distutils/tests/test_dir_util.py	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_dir_util.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_dir_util.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_dist.cpython-36.pyc:	\
		third_party/python/Lib/distutils/tests/test_dist.py	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_dist.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_dist.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_extension.cpython-36.pyc:	\
		third_party/python/Lib/distutils/tests/test_extension.py	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_extension.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_extension.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_file_util.cpython-36.pyc:	\
		third_party/python/Lib/distutils/tests/test_file_util.py	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_file_util.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_file_util.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_filelist.cpython-36.pyc:	\
		third_party/python/Lib/distutils/tests/test_filelist.py	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_filelist.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_filelist.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_install.cpython-36.pyc:	\
		third_party/python/Lib/distutils/tests/test_install.py	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_install.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_install.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_install_data.cpython-36.pyc:	\
		third_party/python/Lib/distutils/tests/test_install_data.py	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_install_data.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_install_data.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_install_headers.cpython-36.pyc:	\
		third_party/python/Lib/distutils/tests/test_install_headers.py	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_install_headers.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_install_headers.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_install_lib.cpython-36.pyc:	\
		third_party/python/Lib/distutils/tests/test_install_lib.py	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_install_lib.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_install_lib.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_install_scripts.cpython-36.pyc:	\
		third_party/python/Lib/distutils/tests/test_install_scripts.py	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_install_scripts.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_install_scripts.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_log.cpython-36.pyc:	\
		third_party/python/Lib/distutils/tests/test_log.py	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_log.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_log.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_msvc9compiler.cpython-36.pyc:	\
		third_party/python/Lib/distutils/tests/test_msvc9compiler.py	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_msvc9compiler.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_msvc9compiler.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_msvccompiler.cpython-36.pyc:	\
		third_party/python/Lib/distutils/tests/test_msvccompiler.py	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_msvccompiler.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_msvccompiler.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_register.cpython-36.pyc:	\
		third_party/python/Lib/distutils/tests/test_register.py	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_register.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_register.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_sdist.cpython-36.pyc:	\
		third_party/python/Lib/distutils/tests/test_sdist.py	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_sdist.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_sdist.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_spawn.cpython-36.pyc:	\
		third_party/python/Lib/distutils/tests/test_spawn.py	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_spawn.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_spawn.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_sysconfig.cpython-36.pyc:	\
		third_party/python/Lib/distutils/tests/test_sysconfig.py	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_sysconfig.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_sysconfig.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_text_file.cpython-36.pyc:	\
		third_party/python/Lib/distutils/tests/test_text_file.py	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_text_file.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_text_file.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_unixccompiler.cpython-36.pyc:	\
		third_party/python/Lib/distutils/tests/test_unixccompiler.py	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_unixccompiler.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_unixccompiler.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_upload.cpython-36.pyc:	\
		third_party/python/Lib/distutils/tests/test_upload.py	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_upload.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_upload.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_util.cpython-36.pyc:	\
		third_party/python/Lib/distutils/tests/test_util.py	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_util.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_util.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_version.cpython-36.pyc:	\
		third_party/python/Lib/distutils/tests/test_version.py	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_version.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_version.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_versionpredicate.cpython-36.pyc:	\
		third_party/python/Lib/distutils/tests/test_versionpredicate.py	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_versionpredicate.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_versionpredicate.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/__pycache__/text_file.cpython-36.pyc:	\
		third_party/python/Lib/distutils/text_file.py	\
		o/$(MODE)/third_party/python/Lib/distutils/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/__pycache__/text_file.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/__pycache__/text_file.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/__pycache__/unixccompiler.cpython-36.pyc:	\
		third_party/python/Lib/distutils/unixccompiler.py	\
		o/$(MODE)/third_party/python/Lib/distutils/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/__pycache__/unixccompiler.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/__pycache__/unixccompiler.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/__pycache__/util.cpython-36.pyc:	\
		third_party/python/Lib/distutils/util.py	\
		o/$(MODE)/third_party/python/Lib/distutils/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/__pycache__/util.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/__pycache__/util.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/__pycache__/version.cpython-36.pyc:	\
		third_party/python/Lib/distutils/version.py	\
		o/$(MODE)/third_party/python/Lib/distutils/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/__pycache__/version.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/__pycache__/version.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/__pycache__/versionpredicate.cpython-36.pyc:	\
		third_party/python/Lib/distutils/versionpredicate.py	\
		o/$(MODE)/third_party/python/Lib/distutils/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/__pycache__/versionpredicate.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/__pycache__/versionpredicate.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/doctest.cpython-36.pyc:	\
		third_party/python/Lib/doctest.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/doctest.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/doctest.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/dummy_threading.cpython-36.pyc:	\
		third_party/python/Lib/dummy_threading.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/dummy_threading.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/dummy_threading.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/email/__pycache__/:	\
		o/$(MODE)/third_party/python/Lib/email/
	@mkdir -p $@

o/$(MODE)/third_party/python/Lib/email/__pycache__/.zip.o:	\
		o/$(MODE)/third_party/python/Lib/email/__pycache__/
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/email/__pycache__/__init__.cpython-36.pyc:	\
		third_party/python/Lib/email/__init__.py	\
		o/$(MODE)/third_party/python/Lib/email/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/email/__pycache__/__init__.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/email/__pycache__/__init__.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/email/__pycache__/_encoded_words.cpython-36.pyc:	\
		third_party/python/Lib/email/_encoded_words.py	\
		o/$(MODE)/third_party/python/Lib/email/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/email/__pycache__/_encoded_words.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/email/__pycache__/_encoded_words.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/email/__pycache__/_header_value_parser.cpython-36.pyc:	\
		third_party/python/Lib/email/_header_value_parser.py	\
		o/$(MODE)/third_party/python/Lib/email/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/email/__pycache__/_header_value_parser.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/email/__pycache__/_header_value_parser.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/email/__pycache__/_parseaddr.cpython-36.pyc:	\
		third_party/python/Lib/email/_parseaddr.py	\
		o/$(MODE)/third_party/python/Lib/email/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/email/__pycache__/_parseaddr.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/email/__pycache__/_parseaddr.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/email/__pycache__/_policybase.cpython-36.pyc:	\
		third_party/python/Lib/email/_policybase.py	\
		o/$(MODE)/third_party/python/Lib/email/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/email/__pycache__/_policybase.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/email/__pycache__/_policybase.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/email/__pycache__/base64mime.cpython-36.pyc:	\
		third_party/python/Lib/email/base64mime.py	\
		o/$(MODE)/third_party/python/Lib/email/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/email/__pycache__/base64mime.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/email/__pycache__/base64mime.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/email/__pycache__/charset.cpython-36.pyc:	\
		third_party/python/Lib/email/charset.py	\
		o/$(MODE)/third_party/python/Lib/email/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/email/__pycache__/charset.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/email/__pycache__/charset.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/email/__pycache__/contentmanager.cpython-36.pyc:	\
		third_party/python/Lib/email/contentmanager.py	\
		o/$(MODE)/third_party/python/Lib/email/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/email/__pycache__/contentmanager.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/email/__pycache__/contentmanager.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/email/__pycache__/encoders.cpython-36.pyc:	\
		third_party/python/Lib/email/encoders.py	\
		o/$(MODE)/third_party/python/Lib/email/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/email/__pycache__/encoders.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/email/__pycache__/encoders.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/email/__pycache__/errors.cpython-36.pyc:	\
		third_party/python/Lib/email/errors.py	\
		o/$(MODE)/third_party/python/Lib/email/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/email/__pycache__/errors.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/email/__pycache__/errors.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/email/__pycache__/feedparser.cpython-36.pyc:	\
		third_party/python/Lib/email/feedparser.py	\
		o/$(MODE)/third_party/python/Lib/email/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/email/__pycache__/feedparser.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/email/__pycache__/feedparser.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/email/__pycache__/generator.cpython-36.pyc:	\
		third_party/python/Lib/email/generator.py	\
		o/$(MODE)/third_party/python/Lib/email/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/email/__pycache__/generator.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/email/__pycache__/generator.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/email/__pycache__/header.cpython-36.pyc:	\
		third_party/python/Lib/email/header.py	\
		o/$(MODE)/third_party/python/Lib/email/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/email/__pycache__/header.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/email/__pycache__/header.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/email/__pycache__/headerregistry.cpython-36.pyc:	\
		third_party/python/Lib/email/headerregistry.py	\
		o/$(MODE)/third_party/python/Lib/email/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/email/__pycache__/headerregistry.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/email/__pycache__/headerregistry.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/email/__pycache__/iterators.cpython-36.pyc:	\
		third_party/python/Lib/email/iterators.py	\
		o/$(MODE)/third_party/python/Lib/email/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/email/__pycache__/iterators.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/email/__pycache__/iterators.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/email/__pycache__/message.cpython-36.pyc:	\
		third_party/python/Lib/email/message.py	\
		o/$(MODE)/third_party/python/Lib/email/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/email/__pycache__/message.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/email/__pycache__/message.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/email/mime/__pycache__/:	\
		o/$(MODE)/third_party/python/Lib/email/mime/
	@mkdir -p $@

o/$(MODE)/third_party/python/Lib/email/mime/__pycache__/.zip.o:	\
		o/$(MODE)/third_party/python/Lib/email/mime/__pycache__/
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/email/mime/__pycache__/__init__.cpython-36.pyc:	\
		third_party/python/Lib/email/mime/__init__.py	\
		o/$(MODE)/third_party/python/Lib/email/mime/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/email/mime/__pycache__/__init__.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/email/mime/__pycache__/__init__.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/email/mime/__pycache__/application.cpython-36.pyc:	\
		third_party/python/Lib/email/mime/application.py	\
		o/$(MODE)/third_party/python/Lib/email/mime/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/email/mime/__pycache__/application.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/email/mime/__pycache__/application.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/email/mime/__pycache__/audio.cpython-36.pyc:	\
		third_party/python/Lib/email/mime/audio.py	\
		o/$(MODE)/third_party/python/Lib/email/mime/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/email/mime/__pycache__/audio.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/email/mime/__pycache__/audio.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/email/mime/__pycache__/base.cpython-36.pyc:	\
		third_party/python/Lib/email/mime/base.py	\
		o/$(MODE)/third_party/python/Lib/email/mime/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/email/mime/__pycache__/base.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/email/mime/__pycache__/base.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/email/mime/__pycache__/image.cpython-36.pyc:	\
		third_party/python/Lib/email/mime/image.py	\
		o/$(MODE)/third_party/python/Lib/email/mime/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/email/mime/__pycache__/image.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/email/mime/__pycache__/image.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/email/mime/__pycache__/message.cpython-36.pyc:	\
		third_party/python/Lib/email/mime/message.py	\
		o/$(MODE)/third_party/python/Lib/email/mime/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/email/mime/__pycache__/message.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/email/mime/__pycache__/message.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/email/mime/__pycache__/multipart.cpython-36.pyc:	\
		third_party/python/Lib/email/mime/multipart.py	\
		o/$(MODE)/third_party/python/Lib/email/mime/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/email/mime/__pycache__/multipart.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/email/mime/__pycache__/multipart.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/email/mime/__pycache__/nonmultipart.cpython-36.pyc:	\
		third_party/python/Lib/email/mime/nonmultipart.py	\
		o/$(MODE)/third_party/python/Lib/email/mime/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/email/mime/__pycache__/nonmultipart.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/email/mime/__pycache__/nonmultipart.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/email/mime/__pycache__/text.cpython-36.pyc:	\
		third_party/python/Lib/email/mime/text.py	\
		o/$(MODE)/third_party/python/Lib/email/mime/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/email/mime/__pycache__/text.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/email/mime/__pycache__/text.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/email/__pycache__/parser.cpython-36.pyc:	\
		third_party/python/Lib/email/parser.py	\
		o/$(MODE)/third_party/python/Lib/email/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/email/__pycache__/parser.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/email/__pycache__/parser.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/email/__pycache__/policy.cpython-36.pyc:	\
		third_party/python/Lib/email/policy.py	\
		o/$(MODE)/third_party/python/Lib/email/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/email/__pycache__/policy.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/email/__pycache__/policy.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/email/__pycache__/quoprimime.cpython-36.pyc:	\
		third_party/python/Lib/email/quoprimime.py	\
		o/$(MODE)/third_party/python/Lib/email/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/email/__pycache__/quoprimime.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/email/__pycache__/quoprimime.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/email/__pycache__/utils.cpython-36.pyc:	\
		third_party/python/Lib/email/utils.py	\
		o/$(MODE)/third_party/python/Lib/email/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/email/__pycache__/utils.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/email/__pycache__/utils.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/:	\
		o/$(MODE)/third_party/python/Lib/encodings/
	@mkdir -p $@

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/__init__.cpython-36.pyc:	\
		third_party/python/Lib/encodings/__init__.py	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/__init__.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/__init__.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/aliases.cpython-36.pyc:	\
		third_party/python/Lib/encodings/aliases.py	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/aliases.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/aliases.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/ascii.cpython-36.pyc:	\
		third_party/python/Lib/encodings/ascii.py	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/ascii.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/ascii.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/base64_codec.cpython-36.pyc:	\
		third_party/python/Lib/encodings/base64_codec.py	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/base64_codec.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/base64_codec.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/big5.cpython-36.pyc:	\
		third_party/python/Lib/encodings/big5.py	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/big5.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/big5.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/big5hkscs.cpython-36.pyc:	\
		third_party/python/Lib/encodings/big5hkscs.py	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/big5hkscs.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/big5hkscs.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/bz2_codec.cpython-36.pyc:	\
		third_party/python/Lib/encodings/bz2_codec.py	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/bz2_codec.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/bz2_codec.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/charmap.cpython-36.pyc:	\
		third_party/python/Lib/encodings/charmap.py	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/charmap.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/charmap.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp037.cpython-36.pyc:	\
		third_party/python/Lib/encodings/cp037.py	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp037.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp037.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp1006.cpython-36.pyc:	\
		third_party/python/Lib/encodings/cp1006.py	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp1006.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp1006.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp1026.cpython-36.pyc:	\
		third_party/python/Lib/encodings/cp1026.py	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp1026.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp1026.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp1125.cpython-36.pyc:	\
		third_party/python/Lib/encodings/cp1125.py	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp1125.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp1125.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp1140.cpython-36.pyc:	\
		third_party/python/Lib/encodings/cp1140.py	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp1140.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp1140.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp1250.cpython-36.pyc:	\
		third_party/python/Lib/encodings/cp1250.py	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp1250.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp1250.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp1251.cpython-36.pyc:	\
		third_party/python/Lib/encodings/cp1251.py	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp1251.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp1251.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp1252.cpython-36.pyc:	\
		third_party/python/Lib/encodings/cp1252.py	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp1252.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp1252.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp1253.cpython-36.pyc:	\
		third_party/python/Lib/encodings/cp1253.py	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp1253.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp1253.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp1254.cpython-36.pyc:	\
		third_party/python/Lib/encodings/cp1254.py	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp1254.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp1254.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp1255.cpython-36.pyc:	\
		third_party/python/Lib/encodings/cp1255.py	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp1255.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp1255.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp1256.cpython-36.pyc:	\
		third_party/python/Lib/encodings/cp1256.py	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp1256.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp1256.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp1257.cpython-36.pyc:	\
		third_party/python/Lib/encodings/cp1257.py	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp1257.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp1257.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp1258.cpython-36.pyc:	\
		third_party/python/Lib/encodings/cp1258.py	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp1258.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp1258.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp273.cpython-36.pyc:	\
		third_party/python/Lib/encodings/cp273.py	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp273.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp273.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp424.cpython-36.pyc:	\
		third_party/python/Lib/encodings/cp424.py	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp424.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp424.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp437.cpython-36.pyc:	\
		third_party/python/Lib/encodings/cp437.py	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp437.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp437.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp500.cpython-36.pyc:	\
		third_party/python/Lib/encodings/cp500.py	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp500.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp500.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp65001.cpython-36.pyc:	\
		third_party/python/Lib/encodings/cp65001.py	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp65001.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp65001.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp720.cpython-36.pyc:	\
		third_party/python/Lib/encodings/cp720.py	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp720.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp720.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp737.cpython-36.pyc:	\
		third_party/python/Lib/encodings/cp737.py	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp737.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp737.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp775.cpython-36.pyc:	\
		third_party/python/Lib/encodings/cp775.py	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp775.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp775.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp850.cpython-36.pyc:	\
		third_party/python/Lib/encodings/cp850.py	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp850.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp850.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp852.cpython-36.pyc:	\
		third_party/python/Lib/encodings/cp852.py	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp852.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp852.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp855.cpython-36.pyc:	\
		third_party/python/Lib/encodings/cp855.py	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp855.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp855.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp856.cpython-36.pyc:	\
		third_party/python/Lib/encodings/cp856.py	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp856.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp856.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp857.cpython-36.pyc:	\
		third_party/python/Lib/encodings/cp857.py	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp857.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp857.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp858.cpython-36.pyc:	\
		third_party/python/Lib/encodings/cp858.py	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp858.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp858.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp860.cpython-36.pyc:	\
		third_party/python/Lib/encodings/cp860.py	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp860.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp860.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp861.cpython-36.pyc:	\
		third_party/python/Lib/encodings/cp861.py	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp861.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp861.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp862.cpython-36.pyc:	\
		third_party/python/Lib/encodings/cp862.py	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp862.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp862.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp863.cpython-36.pyc:	\
		third_party/python/Lib/encodings/cp863.py	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp863.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp863.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp864.cpython-36.pyc:	\
		third_party/python/Lib/encodings/cp864.py	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp864.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp864.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp865.cpython-36.pyc:	\
		third_party/python/Lib/encodings/cp865.py	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp865.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp865.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp866.cpython-36.pyc:	\
		third_party/python/Lib/encodings/cp866.py	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp866.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp866.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp869.cpython-36.pyc:	\
		third_party/python/Lib/encodings/cp869.py	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp869.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp869.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp874.cpython-36.pyc:	\
		third_party/python/Lib/encodings/cp874.py	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp874.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp874.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp875.cpython-36.pyc:	\
		third_party/python/Lib/encodings/cp875.py	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp875.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp875.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp932.cpython-36.pyc:	\
		third_party/python/Lib/encodings/cp932.py	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp932.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp932.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp949.cpython-36.pyc:	\
		third_party/python/Lib/encodings/cp949.py	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp949.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp949.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp950.cpython-36.pyc:	\
		third_party/python/Lib/encodings/cp950.py	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp950.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp950.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/euc_jis_2004.cpython-36.pyc:	\
		third_party/python/Lib/encodings/euc_jis_2004.py	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/euc_jis_2004.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/euc_jis_2004.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/euc_jisx0213.cpython-36.pyc:	\
		third_party/python/Lib/encodings/euc_jisx0213.py	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/euc_jisx0213.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/euc_jisx0213.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/euc_jp.cpython-36.pyc:	\
		third_party/python/Lib/encodings/euc_jp.py	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/euc_jp.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/euc_jp.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/euc_kr.cpython-36.pyc:	\
		third_party/python/Lib/encodings/euc_kr.py	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/euc_kr.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/euc_kr.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/gb18030.cpython-36.pyc:	\
		third_party/python/Lib/encodings/gb18030.py	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/gb18030.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/gb18030.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/gb2312.cpython-36.pyc:	\
		third_party/python/Lib/encodings/gb2312.py	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/gb2312.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/gb2312.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/gbk.cpython-36.pyc:	\
		third_party/python/Lib/encodings/gbk.py	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/gbk.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/gbk.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/hex_codec.cpython-36.pyc:	\
		third_party/python/Lib/encodings/hex_codec.py	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/hex_codec.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/hex_codec.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/hp_roman8.cpython-36.pyc:	\
		third_party/python/Lib/encodings/hp_roman8.py	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/hp_roman8.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/hp_roman8.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/hz.cpython-36.pyc:	\
		third_party/python/Lib/encodings/hz.py	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/hz.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/hz.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/idna.cpython-36.pyc:	\
		third_party/python/Lib/encodings/idna.py	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/idna.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/idna.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/iso2022_jp.cpython-36.pyc:	\
		third_party/python/Lib/encodings/iso2022_jp.py	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/iso2022_jp.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/iso2022_jp.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/iso2022_jp_1.cpython-36.pyc:	\
		third_party/python/Lib/encodings/iso2022_jp_1.py	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/iso2022_jp_1.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/iso2022_jp_1.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/iso2022_jp_2.cpython-36.pyc:	\
		third_party/python/Lib/encodings/iso2022_jp_2.py	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/iso2022_jp_2.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/iso2022_jp_2.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/iso2022_jp_2004.cpython-36.pyc:	\
		third_party/python/Lib/encodings/iso2022_jp_2004.py	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/iso2022_jp_2004.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/iso2022_jp_2004.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/iso2022_jp_3.cpython-36.pyc:	\
		third_party/python/Lib/encodings/iso2022_jp_3.py	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/iso2022_jp_3.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/iso2022_jp_3.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/iso2022_jp_ext.cpython-36.pyc:	\
		third_party/python/Lib/encodings/iso2022_jp_ext.py	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/iso2022_jp_ext.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/iso2022_jp_ext.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/iso2022_kr.cpython-36.pyc:	\
		third_party/python/Lib/encodings/iso2022_kr.py	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/iso2022_kr.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/iso2022_kr.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/iso8859_1.cpython-36.pyc:	\
		third_party/python/Lib/encodings/iso8859_1.py	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/iso8859_1.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/iso8859_1.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/iso8859_10.cpython-36.pyc:	\
		third_party/python/Lib/encodings/iso8859_10.py	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/iso8859_10.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/iso8859_10.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/iso8859_11.cpython-36.pyc:	\
		third_party/python/Lib/encodings/iso8859_11.py	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/iso8859_11.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/iso8859_11.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/iso8859_13.cpython-36.pyc:	\
		third_party/python/Lib/encodings/iso8859_13.py	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/iso8859_13.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/iso8859_13.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/iso8859_14.cpython-36.pyc:	\
		third_party/python/Lib/encodings/iso8859_14.py	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/iso8859_14.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/iso8859_14.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/iso8859_15.cpython-36.pyc:	\
		third_party/python/Lib/encodings/iso8859_15.py	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/iso8859_15.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/iso8859_15.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/iso8859_16.cpython-36.pyc:	\
		third_party/python/Lib/encodings/iso8859_16.py	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/iso8859_16.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/iso8859_16.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/iso8859_2.cpython-36.pyc:	\
		third_party/python/Lib/encodings/iso8859_2.py	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/iso8859_2.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/iso8859_2.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/iso8859_3.cpython-36.pyc:	\
		third_party/python/Lib/encodings/iso8859_3.py	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/iso8859_3.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/iso8859_3.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/iso8859_4.cpython-36.pyc:	\
		third_party/python/Lib/encodings/iso8859_4.py	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/iso8859_4.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/iso8859_4.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/iso8859_5.cpython-36.pyc:	\
		third_party/python/Lib/encodings/iso8859_5.py	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/iso8859_5.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/iso8859_5.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/iso8859_6.cpython-36.pyc:	\
		third_party/python/Lib/encodings/iso8859_6.py	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/iso8859_6.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/iso8859_6.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/iso8859_7.cpython-36.pyc:	\
		third_party/python/Lib/encodings/iso8859_7.py	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/iso8859_7.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/iso8859_7.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/iso8859_8.cpython-36.pyc:	\
		third_party/python/Lib/encodings/iso8859_8.py	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/iso8859_8.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/iso8859_8.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/iso8859_9.cpython-36.pyc:	\
		third_party/python/Lib/encodings/iso8859_9.py	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/iso8859_9.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/iso8859_9.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/johab.cpython-36.pyc:	\
		third_party/python/Lib/encodings/johab.py	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/johab.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/johab.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/koi8_r.cpython-36.pyc:	\
		third_party/python/Lib/encodings/koi8_r.py	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/koi8_r.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/koi8_r.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/koi8_t.cpython-36.pyc:	\
		third_party/python/Lib/encodings/koi8_t.py	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/koi8_t.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/koi8_t.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/koi8_u.cpython-36.pyc:	\
		third_party/python/Lib/encodings/koi8_u.py	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/koi8_u.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/koi8_u.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/kz1048.cpython-36.pyc:	\
		third_party/python/Lib/encodings/kz1048.py	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/kz1048.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/kz1048.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/latin_1.cpython-36.pyc:	\
		third_party/python/Lib/encodings/latin_1.py	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/latin_1.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/latin_1.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/mac_arabic.cpython-36.pyc:	\
		third_party/python/Lib/encodings/mac_arabic.py	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/mac_arabic.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/mac_arabic.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/mac_centeuro.cpython-36.pyc:	\
		third_party/python/Lib/encodings/mac_centeuro.py	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/mac_centeuro.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/mac_centeuro.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/mac_croatian.cpython-36.pyc:	\
		third_party/python/Lib/encodings/mac_croatian.py	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/mac_croatian.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/mac_croatian.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/mac_cyrillic.cpython-36.pyc:	\
		third_party/python/Lib/encodings/mac_cyrillic.py	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/mac_cyrillic.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/mac_cyrillic.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/mac_farsi.cpython-36.pyc:	\
		third_party/python/Lib/encodings/mac_farsi.py	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/mac_farsi.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/mac_farsi.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/mac_greek.cpython-36.pyc:	\
		third_party/python/Lib/encodings/mac_greek.py	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/mac_greek.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/mac_greek.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/mac_iceland.cpython-36.pyc:	\
		third_party/python/Lib/encodings/mac_iceland.py	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/mac_iceland.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/mac_iceland.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/mac_latin2.cpython-36.pyc:	\
		third_party/python/Lib/encodings/mac_latin2.py	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/mac_latin2.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/mac_latin2.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/mac_roman.cpython-36.pyc:	\
		third_party/python/Lib/encodings/mac_roman.py	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/mac_roman.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/mac_roman.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/mac_romanian.cpython-36.pyc:	\
		third_party/python/Lib/encodings/mac_romanian.py	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/mac_romanian.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/mac_romanian.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/mac_turkish.cpython-36.pyc:	\
		third_party/python/Lib/encodings/mac_turkish.py	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/mac_turkish.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/mac_turkish.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/mbcs.cpython-36.pyc:	\
		third_party/python/Lib/encodings/mbcs.py	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/mbcs.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/mbcs.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/oem.cpython-36.pyc:	\
		third_party/python/Lib/encodings/oem.py	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/oem.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/oem.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/palmos.cpython-36.pyc:	\
		third_party/python/Lib/encodings/palmos.py	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/palmos.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/palmos.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/ptcp154.cpython-36.pyc:	\
		third_party/python/Lib/encodings/ptcp154.py	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/ptcp154.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/ptcp154.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/punycode.cpython-36.pyc:	\
		third_party/python/Lib/encodings/punycode.py	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/punycode.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/punycode.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/quopri_codec.cpython-36.pyc:	\
		third_party/python/Lib/encodings/quopri_codec.py	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/quopri_codec.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/quopri_codec.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/raw_unicode_escape.cpython-36.pyc:	\
		third_party/python/Lib/encodings/raw_unicode_escape.py	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/raw_unicode_escape.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/raw_unicode_escape.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/rot_13.cpython-36.pyc:	\
		third_party/python/Lib/encodings/rot_13.py	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/rot_13.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/rot_13.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/shift_jis.cpython-36.pyc:	\
		third_party/python/Lib/encodings/shift_jis.py	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/shift_jis.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/shift_jis.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/shift_jis_2004.cpython-36.pyc:	\
		third_party/python/Lib/encodings/shift_jis_2004.py	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/shift_jis_2004.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/shift_jis_2004.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/shift_jisx0213.cpython-36.pyc:	\
		third_party/python/Lib/encodings/shift_jisx0213.py	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/shift_jisx0213.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/shift_jisx0213.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/tis_620.cpython-36.pyc:	\
		third_party/python/Lib/encodings/tis_620.py	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/tis_620.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/tis_620.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/undefined.cpython-36.pyc:	\
		third_party/python/Lib/encodings/undefined.py	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/undefined.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/undefined.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/unicode_escape.cpython-36.pyc:	\
		third_party/python/Lib/encodings/unicode_escape.py	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/unicode_escape.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/unicode_escape.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/unicode_internal.cpython-36.pyc:	\
		third_party/python/Lib/encodings/unicode_internal.py	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/unicode_internal.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/unicode_internal.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/utf_16.cpython-36.pyc:	\
		third_party/python/Lib/encodings/utf_16.py	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/utf_16.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/utf_16.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/utf_16_be.cpython-36.pyc:	\
		third_party/python/Lib/encodings/utf_16_be.py	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/utf_16_be.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/utf_16_be.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/utf_16_le.cpython-36.pyc:	\
		third_party/python/Lib/encodings/utf_16_le.py	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/utf_16_le.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/utf_16_le.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/utf_32.cpython-36.pyc:	\
		third_party/python/Lib/encodings/utf_32.py	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/utf_32.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/utf_32.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/utf_32_be.cpython-36.pyc:	\
		third_party/python/Lib/encodings/utf_32_be.py	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/utf_32_be.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/utf_32_be.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/utf_32_le.cpython-36.pyc:	\
		third_party/python/Lib/encodings/utf_32_le.py	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/utf_32_le.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/utf_32_le.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/utf_7.cpython-36.pyc:	\
		third_party/python/Lib/encodings/utf_7.py	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/utf_7.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/utf_7.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/utf_8.cpython-36.pyc:	\
		third_party/python/Lib/encodings/utf_8.py	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/utf_8.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/utf_8.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/utf_8_sig.cpython-36.pyc:	\
		third_party/python/Lib/encodings/utf_8_sig.py	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/utf_8_sig.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/utf_8_sig.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/uu_codec.cpython-36.pyc:	\
		third_party/python/Lib/encodings/uu_codec.py	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/uu_codec.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/uu_codec.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/zlib_codec.cpython-36.pyc:	\
		third_party/python/Lib/encodings/zlib_codec.py	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/__pycache__/zlib_codec.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/__pycache__/zlib_codec.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/ensurepip/__pycache__/:	\
		o/$(MODE)/third_party/python/Lib/ensurepip/
	@mkdir -p $@

o/$(MODE)/third_party/python/Lib/ensurepip/__pycache__/.zip.o:	\
		o/$(MODE)/third_party/python/Lib/ensurepip/__pycache__/
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/ensurepip/__pycache__/__init__.cpython-36.pyc:	\
		third_party/python/Lib/ensurepip/__init__.py	\
		o/$(MODE)/third_party/python/Lib/ensurepip/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/ensurepip/__pycache__/__init__.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/ensurepip/__pycache__/__init__.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/ensurepip/__pycache__/__main__.cpython-36.pyc:	\
		third_party/python/Lib/ensurepip/__main__.py	\
		o/$(MODE)/third_party/python/Lib/ensurepip/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/ensurepip/__pycache__/__main__.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/ensurepip/__pycache__/__main__.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/ensurepip/__pycache__/_uninstall.cpython-36.pyc:	\
		third_party/python/Lib/ensurepip/_uninstall.py	\
		o/$(MODE)/third_party/python/Lib/ensurepip/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/ensurepip/__pycache__/_uninstall.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/ensurepip/__pycache__/_uninstall.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/enum.cpython-36.pyc:	\
		third_party/python/Lib/enum.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/enum.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/enum.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/filecmp.cpython-36.pyc:	\
		third_party/python/Lib/filecmp.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/filecmp.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/filecmp.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/fileinput.cpython-36.pyc:	\
		third_party/python/Lib/fileinput.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/fileinput.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/fileinput.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/fnmatch.cpython-36.pyc:	\
		third_party/python/Lib/fnmatch.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/fnmatch.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/fnmatch.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/formatter.cpython-36.pyc:	\
		third_party/python/Lib/formatter.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/formatter.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/formatter.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/fractions.cpython-36.pyc:	\
		third_party/python/Lib/fractions.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/fractions.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/fractions.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/ftplib.cpython-36.pyc:	\
		third_party/python/Lib/ftplib.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/ftplib.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/ftplib.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/functools.cpython-36.pyc:	\
		third_party/python/Lib/functools.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/functools.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/functools.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/genericpath.cpython-36.pyc:	\
		third_party/python/Lib/genericpath.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/genericpath.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/genericpath.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/getopt.cpython-36.pyc:	\
		third_party/python/Lib/getopt.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/getopt.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/getopt.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/getpass.cpython-36.pyc:	\
		third_party/python/Lib/getpass.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/getpass.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/getpass.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/gettext.cpython-36.pyc:	\
		third_party/python/Lib/gettext.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/gettext.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/gettext.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/glob.cpython-36.pyc:	\
		third_party/python/Lib/glob.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/glob.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/glob.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/gzip.cpython-36.pyc:	\
		third_party/python/Lib/gzip.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/gzip.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/gzip.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/hashlib.cpython-36.pyc:	\
		third_party/python/Lib/hashlib.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/hashlib.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/hashlib.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/heapq.cpython-36.pyc:	\
		third_party/python/Lib/heapq.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/heapq.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/heapq.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/hmac.cpython-36.pyc:	\
		third_party/python/Lib/hmac.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/hmac.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/hmac.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/html/__pycache__/:	\
		o/$(MODE)/third_party/python/Lib/html/
	@mkdir -p $@

o/$(MODE)/third_party/python/Lib/html/__pycache__/.zip.o:	\
		o/$(MODE)/third_party/python/Lib/html/__pycache__/
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/html/__pycache__/__init__.cpython-36.pyc:	\
		third_party/python/Lib/html/__init__.py	\
		o/$(MODE)/third_party/python/Lib/html/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/html/__pycache__/__init__.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/html/__pycache__/__init__.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/html/__pycache__/entities.cpython-36.pyc:	\
		third_party/python/Lib/html/entities.py	\
		o/$(MODE)/third_party/python/Lib/html/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/html/__pycache__/entities.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/html/__pycache__/entities.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/html/__pycache__/parser.cpython-36.pyc:	\
		third_party/python/Lib/html/parser.py	\
		o/$(MODE)/third_party/python/Lib/html/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/html/__pycache__/parser.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/html/__pycache__/parser.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/http/__pycache__/:	\
		o/$(MODE)/third_party/python/Lib/http/
	@mkdir -p $@

o/$(MODE)/third_party/python/Lib/http/__pycache__/.zip.o:	\
		o/$(MODE)/third_party/python/Lib/http/__pycache__/
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/http/__pycache__/__init__.cpython-36.pyc:	\
		third_party/python/Lib/http/__init__.py	\
		o/$(MODE)/third_party/python/Lib/http/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/http/__pycache__/__init__.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/http/__pycache__/__init__.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/http/__pycache__/client.cpython-36.pyc:	\
		third_party/python/Lib/http/client.py	\
		o/$(MODE)/third_party/python/Lib/http/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/http/__pycache__/client.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/http/__pycache__/client.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/http/__pycache__/cookiejar.cpython-36.pyc:	\
		third_party/python/Lib/http/cookiejar.py	\
		o/$(MODE)/third_party/python/Lib/http/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/http/__pycache__/cookiejar.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/http/__pycache__/cookiejar.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/http/__pycache__/cookies.cpython-36.pyc:	\
		third_party/python/Lib/http/cookies.py	\
		o/$(MODE)/third_party/python/Lib/http/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/http/__pycache__/cookies.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/http/__pycache__/cookies.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/http/__pycache__/server.cpython-36.pyc:	\
		third_party/python/Lib/http/server.py	\
		o/$(MODE)/third_party/python/Lib/http/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/http/__pycache__/server.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/http/__pycache__/server.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/imaplib.cpython-36.pyc:	\
		third_party/python/Lib/imaplib.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/imaplib.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/imaplib.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/imghdr.cpython-36.pyc:	\
		third_party/python/Lib/imghdr.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/imghdr.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/imghdr.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/imp.cpython-36.pyc:	\
		third_party/python/Lib/imp.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/imp.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/imp.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/importlib/__pycache__/:	\
		o/$(MODE)/third_party/python/Lib/importlib/
	@mkdir -p $@

o/$(MODE)/third_party/python/Lib/importlib/__pycache__/.zip.o:	\
		o/$(MODE)/third_party/python/Lib/importlib/__pycache__/
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/importlib/__pycache__/__init__.cpython-36.pyc:	\
		third_party/python/Lib/importlib/__init__.py	\
		o/$(MODE)/third_party/python/Lib/importlib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/importlib/__pycache__/__init__.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/importlib/__pycache__/__init__.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/importlib/__pycache__/_bootstrap.cpython-36.pyc:	\
		third_party/python/Lib/importlib/_bootstrap.py	\
		o/$(MODE)/third_party/python/Lib/importlib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/importlib/__pycache__/_bootstrap.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/importlib/__pycache__/_bootstrap.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/importlib/__pycache__/_bootstrap_external.cpython-36.pyc:	\
		third_party/python/Lib/importlib/_bootstrap_external.py	\
		o/$(MODE)/third_party/python/Lib/importlib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/importlib/__pycache__/_bootstrap_external.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/importlib/__pycache__/_bootstrap_external.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/importlib/__pycache__/abc.cpython-36.pyc:	\
		third_party/python/Lib/importlib/abc.py	\
		o/$(MODE)/third_party/python/Lib/importlib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/importlib/__pycache__/abc.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/importlib/__pycache__/abc.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/importlib/__pycache__/machinery.cpython-36.pyc:	\
		third_party/python/Lib/importlib/machinery.py	\
		o/$(MODE)/third_party/python/Lib/importlib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/importlib/__pycache__/machinery.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/importlib/__pycache__/machinery.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/importlib/__pycache__/util.cpython-36.pyc:	\
		third_party/python/Lib/importlib/util.py	\
		o/$(MODE)/third_party/python/Lib/importlib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/importlib/__pycache__/util.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/importlib/__pycache__/util.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/inspect.cpython-36.pyc:	\
		third_party/python/Lib/inspect.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/inspect.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/inspect.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/io.cpython-36.pyc:	\
		third_party/python/Lib/io.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/io.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/io.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/ipaddress.cpython-36.pyc:	\
		third_party/python/Lib/ipaddress.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/ipaddress.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/ipaddress.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/json/__pycache__/:	\
		o/$(MODE)/third_party/python/Lib/json/
	@mkdir -p $@

o/$(MODE)/third_party/python/Lib/json/__pycache__/.zip.o:	\
		o/$(MODE)/third_party/python/Lib/json/__pycache__/
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/json/__pycache__/__init__.cpython-36.pyc:	\
		third_party/python/Lib/json/__init__.py	\
		o/$(MODE)/third_party/python/Lib/json/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/json/__pycache__/__init__.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/json/__pycache__/__init__.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/json/__pycache__/decoder.cpython-36.pyc:	\
		third_party/python/Lib/json/decoder.py	\
		o/$(MODE)/third_party/python/Lib/json/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/json/__pycache__/decoder.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/json/__pycache__/decoder.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/json/__pycache__/encoder.cpython-36.pyc:	\
		third_party/python/Lib/json/encoder.py	\
		o/$(MODE)/third_party/python/Lib/json/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/json/__pycache__/encoder.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/json/__pycache__/encoder.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/json/__pycache__/scanner.cpython-36.pyc:	\
		third_party/python/Lib/json/scanner.py	\
		o/$(MODE)/third_party/python/Lib/json/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/json/__pycache__/scanner.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/json/__pycache__/scanner.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/json/__pycache__/tool.cpython-36.pyc:	\
		third_party/python/Lib/json/tool.py	\
		o/$(MODE)/third_party/python/Lib/json/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/json/__pycache__/tool.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/json/__pycache__/tool.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/keyword.cpython-36.pyc:	\
		third_party/python/Lib/keyword.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/keyword.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/keyword.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/linecache.cpython-36.pyc:	\
		third_party/python/Lib/linecache.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/linecache.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/linecache.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/locale.cpython-36.pyc:	\
		third_party/python/Lib/locale.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/locale.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/locale.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/logging/__pycache__/:	\
		o/$(MODE)/third_party/python/Lib/logging/
	@mkdir -p $@

o/$(MODE)/third_party/python/Lib/logging/__pycache__/.zip.o:	\
		o/$(MODE)/third_party/python/Lib/logging/__pycache__/
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/logging/__pycache__/__init__.cpython-36.pyc:	\
		third_party/python/Lib/logging/__init__.py	\
		o/$(MODE)/third_party/python/Lib/logging/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/logging/__pycache__/__init__.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/logging/__pycache__/__init__.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/logging/__pycache__/config.cpython-36.pyc:	\
		third_party/python/Lib/logging/config.py	\
		o/$(MODE)/third_party/python/Lib/logging/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/logging/__pycache__/config.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/logging/__pycache__/config.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/logging/__pycache__/handlers.cpython-36.pyc:	\
		third_party/python/Lib/logging/handlers.py	\
		o/$(MODE)/third_party/python/Lib/logging/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/logging/__pycache__/handlers.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/logging/__pycache__/handlers.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/lzma.cpython-36.pyc:	\
		third_party/python/Lib/lzma.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/lzma.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/lzma.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/macpath.cpython-36.pyc:	\
		third_party/python/Lib/macpath.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/macpath.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/macpath.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/macurl2path.cpython-36.pyc:	\
		third_party/python/Lib/macurl2path.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/macurl2path.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/macurl2path.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/mailbox.cpython-36.pyc:	\
		third_party/python/Lib/mailbox.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/mailbox.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/mailbox.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/mailcap.cpython-36.pyc:	\
		third_party/python/Lib/mailcap.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/mailcap.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/mailcap.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/mimetypes.cpython-36.pyc:	\
		third_party/python/Lib/mimetypes.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/mimetypes.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/mimetypes.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/modulefinder.cpython-36.pyc:	\
		third_party/python/Lib/modulefinder.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/modulefinder.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/modulefinder.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/msilib/__pycache__/:	\
		o/$(MODE)/third_party/python/Lib/msilib/
	@mkdir -p $@

o/$(MODE)/third_party/python/Lib/msilib/__pycache__/.zip.o:	\
		o/$(MODE)/third_party/python/Lib/msilib/__pycache__/
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/msilib/__pycache__/__init__.cpython-36.pyc:	\
		third_party/python/Lib/msilib/__init__.py	\
		o/$(MODE)/third_party/python/Lib/msilib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/msilib/__pycache__/__init__.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/msilib/__pycache__/__init__.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/msilib/__pycache__/schema.cpython-36.pyc:	\
		third_party/python/Lib/msilib/schema.py	\
		o/$(MODE)/third_party/python/Lib/msilib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/msilib/__pycache__/schema.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/msilib/__pycache__/schema.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/msilib/__pycache__/sequence.cpython-36.pyc:	\
		third_party/python/Lib/msilib/sequence.py	\
		o/$(MODE)/third_party/python/Lib/msilib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/msilib/__pycache__/sequence.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/msilib/__pycache__/sequence.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/msilib/__pycache__/text.cpython-36.pyc:	\
		third_party/python/Lib/msilib/text.py	\
		o/$(MODE)/third_party/python/Lib/msilib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/msilib/__pycache__/text.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/msilib/__pycache__/text.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/multiprocessing/__pycache__/:	\
		o/$(MODE)/third_party/python/Lib/multiprocessing/
	@mkdir -p $@

o/$(MODE)/third_party/python/Lib/multiprocessing/__pycache__/.zip.o:	\
		o/$(MODE)/third_party/python/Lib/multiprocessing/__pycache__/
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/multiprocessing/__pycache__/__init__.cpython-36.pyc:	\
		third_party/python/Lib/multiprocessing/__init__.py	\
		o/$(MODE)/third_party/python/Lib/multiprocessing/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/multiprocessing/__pycache__/__init__.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/multiprocessing/__pycache__/__init__.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/multiprocessing/__pycache__/connection.cpython-36.pyc:	\
		third_party/python/Lib/multiprocessing/connection.py	\
		o/$(MODE)/third_party/python/Lib/multiprocessing/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/multiprocessing/__pycache__/connection.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/multiprocessing/__pycache__/connection.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/multiprocessing/__pycache__/context.cpython-36.pyc:	\
		third_party/python/Lib/multiprocessing/context.py	\
		o/$(MODE)/third_party/python/Lib/multiprocessing/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/multiprocessing/__pycache__/context.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/multiprocessing/__pycache__/context.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/multiprocessing/dummy/__pycache__/:	\
		o/$(MODE)/third_party/python/Lib/multiprocessing/dummy/
	@mkdir -p $@

o/$(MODE)/third_party/python/Lib/multiprocessing/dummy/__pycache__/.zip.o:	\
		o/$(MODE)/third_party/python/Lib/multiprocessing/dummy/__pycache__/
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/multiprocessing/dummy/__pycache__/__init__.cpython-36.pyc:	\
		third_party/python/Lib/multiprocessing/dummy/__init__.py	\
		o/$(MODE)/third_party/python/Lib/multiprocessing/dummy/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/multiprocessing/dummy/__pycache__/__init__.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/multiprocessing/dummy/__pycache__/__init__.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/multiprocessing/dummy/__pycache__/connection.cpython-36.pyc:	\
		third_party/python/Lib/multiprocessing/dummy/connection.py	\
		o/$(MODE)/third_party/python/Lib/multiprocessing/dummy/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/multiprocessing/dummy/__pycache__/connection.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/multiprocessing/dummy/__pycache__/connection.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/multiprocessing/__pycache__/forkserver.cpython-36.pyc:	\
		third_party/python/Lib/multiprocessing/forkserver.py	\
		o/$(MODE)/third_party/python/Lib/multiprocessing/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/multiprocessing/__pycache__/forkserver.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/multiprocessing/__pycache__/forkserver.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/multiprocessing/__pycache__/heap.cpython-36.pyc:	\
		third_party/python/Lib/multiprocessing/heap.py	\
		o/$(MODE)/third_party/python/Lib/multiprocessing/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/multiprocessing/__pycache__/heap.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/multiprocessing/__pycache__/heap.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/multiprocessing/__pycache__/managers.cpython-36.pyc:	\
		third_party/python/Lib/multiprocessing/managers.py	\
		o/$(MODE)/third_party/python/Lib/multiprocessing/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/multiprocessing/__pycache__/managers.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/multiprocessing/__pycache__/managers.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/multiprocessing/__pycache__/pool.cpython-36.pyc:	\
		third_party/python/Lib/multiprocessing/pool.py	\
		o/$(MODE)/third_party/python/Lib/multiprocessing/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/multiprocessing/__pycache__/pool.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/multiprocessing/__pycache__/pool.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/multiprocessing/__pycache__/popen_fork.cpython-36.pyc:	\
		third_party/python/Lib/multiprocessing/popen_fork.py	\
		o/$(MODE)/third_party/python/Lib/multiprocessing/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/multiprocessing/__pycache__/popen_fork.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/multiprocessing/__pycache__/popen_fork.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/multiprocessing/__pycache__/popen_forkserver.cpython-36.pyc:	\
		third_party/python/Lib/multiprocessing/popen_forkserver.py	\
		o/$(MODE)/third_party/python/Lib/multiprocessing/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/multiprocessing/__pycache__/popen_forkserver.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/multiprocessing/__pycache__/popen_forkserver.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/multiprocessing/__pycache__/popen_spawn_posix.cpython-36.pyc:	\
		third_party/python/Lib/multiprocessing/popen_spawn_posix.py	\
		o/$(MODE)/third_party/python/Lib/multiprocessing/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/multiprocessing/__pycache__/popen_spawn_posix.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/multiprocessing/__pycache__/popen_spawn_posix.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/multiprocessing/__pycache__/popen_spawn_win32.cpython-36.pyc:	\
		third_party/python/Lib/multiprocessing/popen_spawn_win32.py	\
		o/$(MODE)/third_party/python/Lib/multiprocessing/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/multiprocessing/__pycache__/popen_spawn_win32.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/multiprocessing/__pycache__/popen_spawn_win32.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/multiprocessing/__pycache__/process.cpython-36.pyc:	\
		third_party/python/Lib/multiprocessing/process.py	\
		o/$(MODE)/third_party/python/Lib/multiprocessing/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/multiprocessing/__pycache__/process.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/multiprocessing/__pycache__/process.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/multiprocessing/__pycache__/queues.cpython-36.pyc:	\
		third_party/python/Lib/multiprocessing/queues.py	\
		o/$(MODE)/third_party/python/Lib/multiprocessing/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/multiprocessing/__pycache__/queues.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/multiprocessing/__pycache__/queues.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/multiprocessing/__pycache__/reduction.cpython-36.pyc:	\
		third_party/python/Lib/multiprocessing/reduction.py	\
		o/$(MODE)/third_party/python/Lib/multiprocessing/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/multiprocessing/__pycache__/reduction.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/multiprocessing/__pycache__/reduction.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/multiprocessing/__pycache__/resource_sharer.cpython-36.pyc:	\
		third_party/python/Lib/multiprocessing/resource_sharer.py	\
		o/$(MODE)/third_party/python/Lib/multiprocessing/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/multiprocessing/__pycache__/resource_sharer.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/multiprocessing/__pycache__/resource_sharer.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/multiprocessing/__pycache__/semaphore_tracker.cpython-36.pyc:	\
		third_party/python/Lib/multiprocessing/semaphore_tracker.py	\
		o/$(MODE)/third_party/python/Lib/multiprocessing/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/multiprocessing/__pycache__/semaphore_tracker.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/multiprocessing/__pycache__/semaphore_tracker.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/multiprocessing/__pycache__/sharedctypes.cpython-36.pyc:	\
		third_party/python/Lib/multiprocessing/sharedctypes.py	\
		o/$(MODE)/third_party/python/Lib/multiprocessing/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/multiprocessing/__pycache__/sharedctypes.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/multiprocessing/__pycache__/sharedctypes.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/multiprocessing/__pycache__/spawn.cpython-36.pyc:	\
		third_party/python/Lib/multiprocessing/spawn.py	\
		o/$(MODE)/third_party/python/Lib/multiprocessing/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/multiprocessing/__pycache__/spawn.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/multiprocessing/__pycache__/spawn.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/multiprocessing/__pycache__/synchronize.cpython-36.pyc:	\
		third_party/python/Lib/multiprocessing/synchronize.py	\
		o/$(MODE)/third_party/python/Lib/multiprocessing/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/multiprocessing/__pycache__/synchronize.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/multiprocessing/__pycache__/synchronize.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/multiprocessing/__pycache__/util.cpython-36.pyc:	\
		third_party/python/Lib/multiprocessing/util.py	\
		o/$(MODE)/third_party/python/Lib/multiprocessing/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/multiprocessing/__pycache__/util.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/multiprocessing/__pycache__/util.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/netrc.cpython-36.pyc:	\
		third_party/python/Lib/netrc.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/netrc.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/netrc.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/nntplib.cpython-36.pyc:	\
		third_party/python/Lib/nntplib.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/nntplib.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/nntplib.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/ntpath.cpython-36.pyc:	\
		third_party/python/Lib/ntpath.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/ntpath.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/ntpath.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/nturl2path.cpython-36.pyc:	\
		third_party/python/Lib/nturl2path.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/nturl2path.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/nturl2path.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/numbers.cpython-36.pyc:	\
		third_party/python/Lib/numbers.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/numbers.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/numbers.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/opcode.cpython-36.pyc:	\
		third_party/python/Lib/opcode.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/opcode.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/opcode.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/operator.cpython-36.pyc:	\
		third_party/python/Lib/operator.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/operator.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/operator.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/optparse.cpython-36.pyc:	\
		third_party/python/Lib/optparse.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/optparse.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/optparse.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/os.cpython-36.pyc:	\
		third_party/python/Lib/os.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/os.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/os.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/pathlib.cpython-36.pyc:	\
		third_party/python/Lib/pathlib.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/pathlib.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/pathlib.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/pdb.cpython-36.pyc:	\
		third_party/python/Lib/pdb.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/pdb.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/pdb.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/pickle.cpython-36.pyc:	\
		third_party/python/Lib/pickle.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/pickle.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/pickle.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/pickletools.cpython-36.pyc:	\
		third_party/python/Lib/pickletools.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/pickletools.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/pickletools.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/pipes.cpython-36.pyc:	\
		third_party/python/Lib/pipes.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/pipes.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/pipes.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/pkgutil.cpython-36.pyc:	\
		third_party/python/Lib/pkgutil.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/pkgutil.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/pkgutil.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/platform.cpython-36.pyc:	\
		third_party/python/Lib/platform.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/platform.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/platform.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/plistlib.cpython-36.pyc:	\
		third_party/python/Lib/plistlib.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/plistlib.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/plistlib.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/poplib.cpython-36.pyc:	\
		third_party/python/Lib/poplib.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/poplib.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/poplib.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/posixpath.cpython-36.pyc:	\
		third_party/python/Lib/posixpath.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/posixpath.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/posixpath.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/pprint.cpython-36.pyc:	\
		third_party/python/Lib/pprint.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/pprint.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/pprint.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/profile.cpython-36.pyc:	\
		third_party/python/Lib/profile.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/profile.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/profile.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/pstats.cpython-36.pyc:	\
		third_party/python/Lib/pstats.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/pstats.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/pstats.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/pty.cpython-36.pyc:	\
		third_party/python/Lib/pty.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/pty.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/pty.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/py_compile.cpython-36.pyc:	\
		third_party/python/Lib/py_compile.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/py_compile.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/py_compile.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/pyclbr.cpython-36.pyc:	\
		third_party/python/Lib/pyclbr.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/pyclbr.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/pyclbr.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/pydoc.cpython-36.pyc:	\
		third_party/python/Lib/pydoc.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/pydoc.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/pydoc.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/queue.cpython-36.pyc:	\
		third_party/python/Lib/queue.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/queue.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/queue.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/quopri.cpython-36.pyc:	\
		third_party/python/Lib/quopri.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/quopri.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/quopri.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/random.cpython-36.pyc:	\
		third_party/python/Lib/random.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/random.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/random.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/re.cpython-36.pyc:	\
		third_party/python/Lib/re.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/re.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/re.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/reprlib.cpython-36.pyc:	\
		third_party/python/Lib/reprlib.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/reprlib.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/reprlib.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/rlcompleter.cpython-36.pyc:	\
		third_party/python/Lib/rlcompleter.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/rlcompleter.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/rlcompleter.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/runpy.cpython-36.pyc:	\
		third_party/python/Lib/runpy.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/runpy.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/runpy.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/sched.cpython-36.pyc:	\
		third_party/python/Lib/sched.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/sched.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/sched.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/secrets.cpython-36.pyc:	\
		third_party/python/Lib/secrets.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/secrets.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/secrets.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/selectors.cpython-36.pyc:	\
		third_party/python/Lib/selectors.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/selectors.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/selectors.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/shelve.cpython-36.pyc:	\
		third_party/python/Lib/shelve.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/shelve.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/shelve.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/shlex.cpython-36.pyc:	\
		third_party/python/Lib/shlex.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/shlex.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/shlex.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/shutil.cpython-36.pyc:	\
		third_party/python/Lib/shutil.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/shutil.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/shutil.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/signal.cpython-36.pyc:	\
		third_party/python/Lib/signal.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/signal.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/signal.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/site.cpython-36.pyc:	\
		third_party/python/Lib/site.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/site.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/site.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/smtpd.cpython-36.pyc:	\
		third_party/python/Lib/smtpd.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/smtpd.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/smtpd.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/smtplib.cpython-36.pyc:	\
		third_party/python/Lib/smtplib.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/smtplib.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/smtplib.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/sndhdr.cpython-36.pyc:	\
		third_party/python/Lib/sndhdr.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/sndhdr.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/sndhdr.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/socket.cpython-36.pyc:	\
		third_party/python/Lib/socket.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/socket.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/socket.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/socketserver.cpython-36.pyc:	\
		third_party/python/Lib/socketserver.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/socketserver.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/socketserver.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/sqlite3/__pycache__/:	\
		o/$(MODE)/third_party/python/Lib/sqlite3/
	@mkdir -p $@

o/$(MODE)/third_party/python/Lib/sqlite3/__pycache__/.zip.o:	\
		o/$(MODE)/third_party/python/Lib/sqlite3/__pycache__/
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/sqlite3/__pycache__/__init__.cpython-36.pyc:	\
		third_party/python/Lib/sqlite3/__init__.py	\
		o/$(MODE)/third_party/python/Lib/sqlite3/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/sqlite3/__pycache__/__init__.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/sqlite3/__pycache__/__init__.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/sqlite3/__pycache__/dbapi2.cpython-36.pyc:	\
		third_party/python/Lib/sqlite3/dbapi2.py	\
		o/$(MODE)/third_party/python/Lib/sqlite3/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/sqlite3/__pycache__/dbapi2.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/sqlite3/__pycache__/dbapi2.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/sqlite3/__pycache__/dump.cpython-36.pyc:	\
		third_party/python/Lib/sqlite3/dump.py	\
		o/$(MODE)/third_party/python/Lib/sqlite3/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/sqlite3/__pycache__/dump.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/sqlite3/__pycache__/dump.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/sre_compile.cpython-36.pyc:	\
		third_party/python/Lib/sre_compile.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/sre_compile.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/sre_compile.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/sre_constants.cpython-36.pyc:	\
		third_party/python/Lib/sre_constants.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/sre_constants.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/sre_constants.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/sre_parse.cpython-36.pyc:	\
		third_party/python/Lib/sre_parse.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/sre_parse.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/sre_parse.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/ssl.cpython-36.pyc:	\
		third_party/python/Lib/ssl.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/ssl.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/ssl.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/stat.cpython-36.pyc:	\
		third_party/python/Lib/stat.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/stat.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/stat.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/statistics.cpython-36.pyc:	\
		third_party/python/Lib/statistics.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/statistics.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/statistics.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/string.cpython-36.pyc:	\
		third_party/python/Lib/string.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/string.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/string.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/stringprep.cpython-36.pyc:	\
		third_party/python/Lib/stringprep.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/stringprep.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/stringprep.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/struct.cpython-36.pyc:	\
		third_party/python/Lib/struct.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/struct.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/struct.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/subprocess.cpython-36.pyc:	\
		third_party/python/Lib/subprocess.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/subprocess.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/subprocess.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/sunau.cpython-36.pyc:	\
		third_party/python/Lib/sunau.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/sunau.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/sunau.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/symbol.cpython-36.pyc:	\
		third_party/python/Lib/symbol.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/symbol.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/symbol.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/symtable.cpython-36.pyc:	\
		third_party/python/Lib/symtable.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/symtable.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/symtable.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/sysconfig.cpython-36.pyc:	\
		third_party/python/Lib/sysconfig.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/sysconfig.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/sysconfig.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/tabnanny.cpython-36.pyc:	\
		third_party/python/Lib/tabnanny.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/tabnanny.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/tabnanny.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/tarfile.cpython-36.pyc:	\
		third_party/python/Lib/tarfile.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/tarfile.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/tarfile.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/telnetlib.cpython-36.pyc:	\
		third_party/python/Lib/telnetlib.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/telnetlib.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/telnetlib.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/tempfile.cpython-36.pyc:	\
		third_party/python/Lib/tempfile.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/tempfile.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/tempfile.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/textwrap.cpython-36.pyc:	\
		third_party/python/Lib/textwrap.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/textwrap.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/textwrap.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/this.cpython-36.pyc:	\
		third_party/python/Lib/this.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/this.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/this.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/threading.cpython-36.pyc:	\
		third_party/python/Lib/threading.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/threading.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/threading.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/timeit.cpython-36.pyc:	\
		third_party/python/Lib/timeit.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/timeit.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/timeit.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/token.cpython-36.pyc:	\
		third_party/python/Lib/token.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/token.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/token.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/tokenize.cpython-36.pyc:	\
		third_party/python/Lib/tokenize.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/tokenize.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/tokenize.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/trace.cpython-36.pyc:	\
		third_party/python/Lib/trace.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/trace.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/trace.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/traceback.cpython-36.pyc:	\
		third_party/python/Lib/traceback.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/traceback.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/traceback.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/tracemalloc.cpython-36.pyc:	\
		third_party/python/Lib/tracemalloc.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/tracemalloc.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/tracemalloc.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/tty.cpython-36.pyc:	\
		third_party/python/Lib/tty.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/tty.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/tty.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/types.cpython-36.pyc:	\
		third_party/python/Lib/types.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/types.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/types.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/typing.cpython-36.pyc:	\
		third_party/python/Lib/typing.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/typing.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/typing.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/unittest/__pycache__/:	\
		o/$(MODE)/third_party/python/Lib/unittest/
	@mkdir -p $@

o/$(MODE)/third_party/python/Lib/unittest/__pycache__/.zip.o:	\
		o/$(MODE)/third_party/python/Lib/unittest/__pycache__/
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/unittest/__pycache__/__init__.cpython-36.pyc:	\
		third_party/python/Lib/unittest/__init__.py	\
		o/$(MODE)/third_party/python/Lib/unittest/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/unittest/__pycache__/__init__.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/unittest/__pycache__/__init__.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/unittest/__pycache__/__main__.cpython-36.pyc:	\
		third_party/python/Lib/unittest/__main__.py	\
		o/$(MODE)/third_party/python/Lib/unittest/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/unittest/__pycache__/__main__.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/unittest/__pycache__/__main__.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/unittest/__pycache__/case.cpython-36.pyc:	\
		third_party/python/Lib/unittest/case.py	\
		o/$(MODE)/third_party/python/Lib/unittest/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/unittest/__pycache__/case.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/unittest/__pycache__/case.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/unittest/__pycache__/loader.cpython-36.pyc:	\
		third_party/python/Lib/unittest/loader.py	\
		o/$(MODE)/third_party/python/Lib/unittest/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/unittest/__pycache__/loader.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/unittest/__pycache__/loader.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/unittest/__pycache__/main.cpython-36.pyc:	\
		third_party/python/Lib/unittest/main.py	\
		o/$(MODE)/third_party/python/Lib/unittest/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/unittest/__pycache__/main.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/unittest/__pycache__/main.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/unittest/__pycache__/mock.cpython-36.pyc:	\
		third_party/python/Lib/unittest/mock.py	\
		o/$(MODE)/third_party/python/Lib/unittest/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/unittest/__pycache__/mock.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/unittest/__pycache__/mock.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/unittest/__pycache__/result.cpython-36.pyc:	\
		third_party/python/Lib/unittest/result.py	\
		o/$(MODE)/third_party/python/Lib/unittest/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/unittest/__pycache__/result.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/unittest/__pycache__/result.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/unittest/__pycache__/runner.cpython-36.pyc:	\
		third_party/python/Lib/unittest/runner.py	\
		o/$(MODE)/third_party/python/Lib/unittest/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/unittest/__pycache__/runner.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/unittest/__pycache__/runner.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/unittest/__pycache__/signals.cpython-36.pyc:	\
		third_party/python/Lib/unittest/signals.py	\
		o/$(MODE)/third_party/python/Lib/unittest/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/unittest/__pycache__/signals.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/unittest/__pycache__/signals.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/unittest/__pycache__/suite.cpython-36.pyc:	\
		third_party/python/Lib/unittest/suite.py	\
		o/$(MODE)/third_party/python/Lib/unittest/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/unittest/__pycache__/suite.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/unittest/__pycache__/suite.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/unittest/__pycache__/util.cpython-36.pyc:	\
		third_party/python/Lib/unittest/util.py	\
		o/$(MODE)/third_party/python/Lib/unittest/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/unittest/__pycache__/util.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/unittest/__pycache__/util.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/urllib/__pycache__/:	\
		o/$(MODE)/third_party/python/Lib/urllib/
	@mkdir -p $@

o/$(MODE)/third_party/python/Lib/urllib/__pycache__/.zip.o:	\
		o/$(MODE)/third_party/python/Lib/urllib/__pycache__/
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/urllib/__pycache__/__init__.cpython-36.pyc:	\
		third_party/python/Lib/urllib/__init__.py	\
		o/$(MODE)/third_party/python/Lib/urllib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/urllib/__pycache__/__init__.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/urllib/__pycache__/__init__.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/urllib/__pycache__/error.cpython-36.pyc:	\
		third_party/python/Lib/urllib/error.py	\
		o/$(MODE)/third_party/python/Lib/urllib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/urllib/__pycache__/error.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/urllib/__pycache__/error.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/urllib/__pycache__/parse.cpython-36.pyc:	\
		third_party/python/Lib/urllib/parse.py	\
		o/$(MODE)/third_party/python/Lib/urllib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/urllib/__pycache__/parse.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/urllib/__pycache__/parse.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/urllib/__pycache__/request.cpython-36.pyc:	\
		third_party/python/Lib/urllib/request.py	\
		o/$(MODE)/third_party/python/Lib/urllib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/urllib/__pycache__/request.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/urllib/__pycache__/request.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/urllib/__pycache__/response.cpython-36.pyc:	\
		third_party/python/Lib/urllib/response.py	\
		o/$(MODE)/third_party/python/Lib/urllib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/urllib/__pycache__/response.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/urllib/__pycache__/response.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/urllib/__pycache__/robotparser.cpython-36.pyc:	\
		third_party/python/Lib/urllib/robotparser.py	\
		o/$(MODE)/third_party/python/Lib/urllib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/urllib/__pycache__/robotparser.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/urllib/__pycache__/robotparser.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/uu.cpython-36.pyc:	\
		third_party/python/Lib/uu.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/uu.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/uu.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/uuid.cpython-36.pyc:	\
		third_party/python/Lib/uuid.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/uuid.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/uuid.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/venv/__pycache__/:	\
		o/$(MODE)/third_party/python/Lib/venv/
	@mkdir -p $@

o/$(MODE)/third_party/python/Lib/venv/__pycache__/.zip.o:	\
		o/$(MODE)/third_party/python/Lib/venv/__pycache__/
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/venv/__pycache__/__init__.cpython-36.pyc:	\
		third_party/python/Lib/venv/__init__.py	\
		o/$(MODE)/third_party/python/Lib/venv/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/venv/__pycache__/__init__.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/venv/__pycache__/__init__.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/venv/__pycache__/__main__.cpython-36.pyc:	\
		third_party/python/Lib/venv/__main__.py	\
		o/$(MODE)/third_party/python/Lib/venv/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/venv/__pycache__/__main__.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/venv/__pycache__/__main__.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/warnings.cpython-36.pyc:	\
		third_party/python/Lib/warnings.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/warnings.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/warnings.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/wave.cpython-36.pyc:	\
		third_party/python/Lib/wave.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/wave.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/wave.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/weakref.cpython-36.pyc:	\
		third_party/python/Lib/weakref.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/weakref.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/weakref.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/webbrowser.cpython-36.pyc:	\
		third_party/python/Lib/webbrowser.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/webbrowser.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/webbrowser.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/wsgiref/__pycache__/:	\
		o/$(MODE)/third_party/python/Lib/wsgiref/
	@mkdir -p $@

o/$(MODE)/third_party/python/Lib/wsgiref/__pycache__/.zip.o:	\
		o/$(MODE)/third_party/python/Lib/wsgiref/__pycache__/
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/wsgiref/__pycache__/__init__.cpython-36.pyc:	\
		third_party/python/Lib/wsgiref/__init__.py	\
		o/$(MODE)/third_party/python/Lib/wsgiref/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/wsgiref/__pycache__/__init__.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/wsgiref/__pycache__/__init__.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/wsgiref/__pycache__/handlers.cpython-36.pyc:	\
		third_party/python/Lib/wsgiref/handlers.py	\
		o/$(MODE)/third_party/python/Lib/wsgiref/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/wsgiref/__pycache__/handlers.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/wsgiref/__pycache__/handlers.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/wsgiref/__pycache__/headers.cpython-36.pyc:	\
		third_party/python/Lib/wsgiref/headers.py	\
		o/$(MODE)/third_party/python/Lib/wsgiref/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/wsgiref/__pycache__/headers.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/wsgiref/__pycache__/headers.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/wsgiref/__pycache__/simple_server.cpython-36.pyc:	\
		third_party/python/Lib/wsgiref/simple_server.py	\
		o/$(MODE)/third_party/python/Lib/wsgiref/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/wsgiref/__pycache__/simple_server.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/wsgiref/__pycache__/simple_server.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/wsgiref/__pycache__/util.cpython-36.pyc:	\
		third_party/python/Lib/wsgiref/util.py	\
		o/$(MODE)/third_party/python/Lib/wsgiref/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/wsgiref/__pycache__/util.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/wsgiref/__pycache__/util.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/wsgiref/__pycache__/validate.cpython-36.pyc:	\
		third_party/python/Lib/wsgiref/validate.py	\
		o/$(MODE)/third_party/python/Lib/wsgiref/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/wsgiref/__pycache__/validate.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/wsgiref/__pycache__/validate.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/xdrlib.cpython-36.pyc:	\
		third_party/python/Lib/xdrlib.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/xdrlib.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/xdrlib.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/xml/__pycache__/:	\
		o/$(MODE)/third_party/python/Lib/xml/
	@mkdir -p $@

o/$(MODE)/third_party/python/Lib/xml/__pycache__/.zip.o:	\
		o/$(MODE)/third_party/python/Lib/xml/__pycache__/
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/xml/__pycache__/__init__.cpython-36.pyc:	\
		third_party/python/Lib/xml/__init__.py	\
		o/$(MODE)/third_party/python/Lib/xml/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/xml/__pycache__/__init__.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/xml/__pycache__/__init__.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/xml/dom/__pycache__/:	\
		o/$(MODE)/third_party/python/Lib/xml/dom/
	@mkdir -p $@

o/$(MODE)/third_party/python/Lib/xml/dom/__pycache__/.zip.o:	\
		o/$(MODE)/third_party/python/Lib/xml/dom/__pycache__/
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/xml/dom/__pycache__/NodeFilter.cpython-36.pyc:	\
		third_party/python/Lib/xml/dom/NodeFilter.py	\
		o/$(MODE)/third_party/python/Lib/xml/dom/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/xml/dom/__pycache__/NodeFilter.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/xml/dom/__pycache__/NodeFilter.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/xml/dom/__pycache__/__init__.cpython-36.pyc:	\
		third_party/python/Lib/xml/dom/__init__.py	\
		o/$(MODE)/third_party/python/Lib/xml/dom/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/xml/dom/__pycache__/__init__.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/xml/dom/__pycache__/__init__.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/xml/dom/__pycache__/domreg.cpython-36.pyc:	\
		third_party/python/Lib/xml/dom/domreg.py	\
		o/$(MODE)/third_party/python/Lib/xml/dom/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/xml/dom/__pycache__/domreg.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/xml/dom/__pycache__/domreg.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/xml/dom/__pycache__/expatbuilder.cpython-36.pyc:	\
		third_party/python/Lib/xml/dom/expatbuilder.py	\
		o/$(MODE)/third_party/python/Lib/xml/dom/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/xml/dom/__pycache__/expatbuilder.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/xml/dom/__pycache__/expatbuilder.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/xml/dom/__pycache__/minicompat.cpython-36.pyc:	\
		third_party/python/Lib/xml/dom/minicompat.py	\
		o/$(MODE)/third_party/python/Lib/xml/dom/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/xml/dom/__pycache__/minicompat.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/xml/dom/__pycache__/minicompat.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/xml/dom/__pycache__/minidom.cpython-36.pyc:	\
		third_party/python/Lib/xml/dom/minidom.py	\
		o/$(MODE)/third_party/python/Lib/xml/dom/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/xml/dom/__pycache__/minidom.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/xml/dom/__pycache__/minidom.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/xml/dom/__pycache__/pulldom.cpython-36.pyc:	\
		third_party/python/Lib/xml/dom/pulldom.py	\
		o/$(MODE)/third_party/python/Lib/xml/dom/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/xml/dom/__pycache__/pulldom.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/xml/dom/__pycache__/pulldom.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/xml/dom/__pycache__/xmlbuilder.cpython-36.pyc:	\
		third_party/python/Lib/xml/dom/xmlbuilder.py	\
		o/$(MODE)/third_party/python/Lib/xml/dom/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/xml/dom/__pycache__/xmlbuilder.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/xml/dom/__pycache__/xmlbuilder.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/xml/etree/__pycache__/:	\
		o/$(MODE)/third_party/python/Lib/xml/etree/
	@mkdir -p $@

o/$(MODE)/third_party/python/Lib/xml/etree/__pycache__/.zip.o:	\
		o/$(MODE)/third_party/python/Lib/xml/etree/__pycache__/
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/xml/etree/__pycache__/ElementInclude.cpython-36.pyc:	\
		third_party/python/Lib/xml/etree/ElementInclude.py	\
		o/$(MODE)/third_party/python/Lib/xml/etree/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/xml/etree/__pycache__/ElementInclude.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/xml/etree/__pycache__/ElementInclude.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/xml/etree/__pycache__/ElementPath.cpython-36.pyc:	\
		third_party/python/Lib/xml/etree/ElementPath.py	\
		o/$(MODE)/third_party/python/Lib/xml/etree/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/xml/etree/__pycache__/ElementPath.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/xml/etree/__pycache__/ElementPath.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/xml/etree/__pycache__/ElementTree.cpython-36.pyc:	\
		third_party/python/Lib/xml/etree/ElementTree.py	\
		o/$(MODE)/third_party/python/Lib/xml/etree/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/xml/etree/__pycache__/ElementTree.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/xml/etree/__pycache__/ElementTree.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/xml/etree/__pycache__/__init__.cpython-36.pyc:	\
		third_party/python/Lib/xml/etree/__init__.py	\
		o/$(MODE)/third_party/python/Lib/xml/etree/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/xml/etree/__pycache__/__init__.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/xml/etree/__pycache__/__init__.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/xml/etree/__pycache__/cElementTree.cpython-36.pyc:	\
		third_party/python/Lib/xml/etree/cElementTree.py	\
		o/$(MODE)/third_party/python/Lib/xml/etree/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/xml/etree/__pycache__/cElementTree.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/xml/etree/__pycache__/cElementTree.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/xml/parsers/__pycache__/:	\
		o/$(MODE)/third_party/python/Lib/xml/parsers/
	@mkdir -p $@

o/$(MODE)/third_party/python/Lib/xml/parsers/__pycache__/.zip.o:	\
		o/$(MODE)/third_party/python/Lib/xml/parsers/__pycache__/
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/xml/parsers/__pycache__/__init__.cpython-36.pyc:	\
		third_party/python/Lib/xml/parsers/__init__.py	\
		o/$(MODE)/third_party/python/Lib/xml/parsers/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/xml/parsers/__pycache__/__init__.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/xml/parsers/__pycache__/__init__.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/xml/parsers/__pycache__/expat.cpython-36.pyc:	\
		third_party/python/Lib/xml/parsers/expat.py	\
		o/$(MODE)/third_party/python/Lib/xml/parsers/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/xml/parsers/__pycache__/expat.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/xml/parsers/__pycache__/expat.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/xml/sax/__pycache__/:	\
		o/$(MODE)/third_party/python/Lib/xml/sax/
	@mkdir -p $@

o/$(MODE)/third_party/python/Lib/xml/sax/__pycache__/.zip.o:	\
		o/$(MODE)/third_party/python/Lib/xml/sax/__pycache__/
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/xml/sax/__pycache__/__init__.cpython-36.pyc:	\
		third_party/python/Lib/xml/sax/__init__.py	\
		o/$(MODE)/third_party/python/Lib/xml/sax/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/xml/sax/__pycache__/__init__.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/xml/sax/__pycache__/__init__.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/xml/sax/__pycache__/_exceptions.cpython-36.pyc:	\
		third_party/python/Lib/xml/sax/_exceptions.py	\
		o/$(MODE)/third_party/python/Lib/xml/sax/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/xml/sax/__pycache__/_exceptions.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/xml/sax/__pycache__/_exceptions.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/xml/sax/__pycache__/expatreader.cpython-36.pyc:	\
		third_party/python/Lib/xml/sax/expatreader.py	\
		o/$(MODE)/third_party/python/Lib/xml/sax/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/xml/sax/__pycache__/expatreader.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/xml/sax/__pycache__/expatreader.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/xml/sax/__pycache__/handler.cpython-36.pyc:	\
		third_party/python/Lib/xml/sax/handler.py	\
		o/$(MODE)/third_party/python/Lib/xml/sax/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/xml/sax/__pycache__/handler.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/xml/sax/__pycache__/handler.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/xml/sax/__pycache__/saxutils.cpython-36.pyc:	\
		third_party/python/Lib/xml/sax/saxutils.py	\
		o/$(MODE)/third_party/python/Lib/xml/sax/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/xml/sax/__pycache__/saxutils.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/xml/sax/__pycache__/saxutils.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/xml/sax/__pycache__/xmlreader.cpython-36.pyc:	\
		third_party/python/Lib/xml/sax/xmlreader.py	\
		o/$(MODE)/third_party/python/Lib/xml/sax/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/xml/sax/__pycache__/xmlreader.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/xml/sax/__pycache__/xmlreader.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/xmlrpc/__pycache__/:	\
		o/$(MODE)/third_party/python/Lib/xmlrpc/
	@mkdir -p $@

o/$(MODE)/third_party/python/Lib/xmlrpc/__pycache__/.zip.o:	\
		o/$(MODE)/third_party/python/Lib/xmlrpc/__pycache__/
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/xmlrpc/__pycache__/__init__.cpython-36.pyc:	\
		third_party/python/Lib/xmlrpc/__init__.py	\
		o/$(MODE)/third_party/python/Lib/xmlrpc/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/xmlrpc/__pycache__/__init__.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/xmlrpc/__pycache__/__init__.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/xmlrpc/__pycache__/client.cpython-36.pyc:	\
		third_party/python/Lib/xmlrpc/client.py	\
		o/$(MODE)/third_party/python/Lib/xmlrpc/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/xmlrpc/__pycache__/client.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/xmlrpc/__pycache__/client.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/xmlrpc/__pycache__/server.cpython-36.pyc:	\
		third_party/python/Lib/xmlrpc/server.py	\
		o/$(MODE)/third_party/python/Lib/xmlrpc/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/xmlrpc/__pycache__/server.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/xmlrpc/__pycache__/server.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/zipapp.cpython-36.pyc:	\
		third_party/python/Lib/zipapp.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/zipapp.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/zipapp.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__pycache__/zipfile.cpython-36.pyc:	\
		third_party/python/Lib/zipfile.py	\
		o/$(MODE)/third_party/python/Lib/__pycache__/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__pycache__/zipfile.cpython-36.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__pycache__/zipfile.cpython-36.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

THIRD_PARTY_PYTHON_STDLIB_PYC_OBJS =	\
	o/$(MODE)/third_party/python/Lib/__pycache__/.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/__future__.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/_bootlocale.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/_collections_abc.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/_compat_pickle.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/_compression.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/_dummy_thread.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/_markupbase.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/_osx_support.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/_pyio.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/_sitebuiltins.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/_strptime.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/_sysconfigdata_m_cosmo_x86_64-cosmo.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/_threading_local.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/_weakrefset.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/abc.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/aifc.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/antigravity.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/argparse.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/ast.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/asynchat.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/asyncore.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/base64.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/bdb.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/binhex.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/bisect.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/bz2.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/cProfile.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/calendar.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/cgi.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/cgitb.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/chunk.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/cmd.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/code.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/codecs.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/codeop.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/colorsys.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/compileall.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/configparser.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/contextlib.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/copy.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/copyreg.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/crypt.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/csv.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/datetime.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/decimal.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/difflib.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/dis.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/doctest.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/dummy_threading.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/enum.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/filecmp.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/fileinput.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/fnmatch.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/formatter.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/fractions.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/ftplib.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/functools.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/genericpath.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/getopt.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/getpass.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/gettext.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/glob.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/gzip.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/hashlib.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/heapq.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/hmac.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/imaplib.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/imghdr.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/imp.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/inspect.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/io.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/ipaddress.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/keyword.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/linecache.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/locale.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/lzma.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/macpath.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/macurl2path.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/mailbox.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/mailcap.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/mimetypes.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/modulefinder.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/netrc.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/nntplib.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/ntpath.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/nturl2path.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/numbers.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/opcode.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/operator.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/optparse.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/os.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/pathlib.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/pdb.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/pickle.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/pickletools.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/pipes.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/pkgutil.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/platform.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/plistlib.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/poplib.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/posixpath.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/pprint.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/profile.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/pstats.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/pty.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/py_compile.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/pyclbr.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/pydoc.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/queue.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/quopri.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/random.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/re.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/reprlib.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/rlcompleter.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/runpy.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/sched.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/secrets.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/selectors.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/shelve.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/shlex.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/shutil.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/signal.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/site.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/smtpd.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/smtplib.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/sndhdr.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/socket.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/socketserver.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/sre_compile.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/sre_constants.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/sre_parse.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/ssl.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/stat.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/statistics.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/string.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/stringprep.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/struct.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/subprocess.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/sunau.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/symbol.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/symtable.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/sysconfig.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/tabnanny.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/tarfile.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/telnetlib.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/tempfile.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/textwrap.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/this.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/threading.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/timeit.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/token.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/tokenize.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/trace.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/traceback.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/tracemalloc.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/tty.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/types.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/typing.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/uu.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/uuid.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/warnings.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/wave.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/weakref.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/webbrowser.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/xdrlib.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/zipapp.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/__pycache__/zipfile.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/asyncio/__pycache__/.zip.o	\
	o/$(MODE)/third_party/python/Lib/asyncio/__pycache__/__init__.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/asyncio/__pycache__/base_events.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/asyncio/__pycache__/base_futures.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/asyncio/__pycache__/base_subprocess.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/asyncio/__pycache__/base_tasks.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/asyncio/__pycache__/compat.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/asyncio/__pycache__/constants.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/asyncio/__pycache__/coroutines.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/asyncio/__pycache__/events.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/asyncio/__pycache__/futures.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/asyncio/__pycache__/locks.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/asyncio/__pycache__/log.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/asyncio/__pycache__/proactor_events.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/asyncio/__pycache__/protocols.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/asyncio/__pycache__/queues.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/asyncio/__pycache__/selector_events.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/asyncio/__pycache__/sslproto.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/asyncio/__pycache__/streams.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/asyncio/__pycache__/subprocess.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/asyncio/__pycache__/tasks.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/asyncio/__pycache__/test_utils.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/asyncio/__pycache__/transports.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/asyncio/__pycache__/unix_events.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/asyncio/__pycache__/windows_events.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/asyncio/__pycache__/windows_utils.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/collections/__pycache__/.zip.o	\
	o/$(MODE)/third_party/python/Lib/collections/__pycache__/__init__.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/collections/__pycache__/abc.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/dbm/__pycache__/.zip.o	\
	o/$(MODE)/third_party/python/Lib/dbm/__pycache__/__init__.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/dbm/__pycache__/dumb.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/dbm/__pycache__/gnu.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/dbm/__pycache__/ndbm.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/__pycache__/.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/__pycache__/__init__.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/__pycache__/_msvccompiler.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/__pycache__/archive_util.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/__pycache__/bcppcompiler.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/__pycache__/ccompiler.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/__pycache__/cmd.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/__pycache__/config.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/__pycache__/core.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/__pycache__/cygwinccompiler.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/__pycache__/debug.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/__pycache__/dep_util.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/__pycache__/dir_util.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/__pycache__/dist.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/__pycache__/errors.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/__pycache__/extension.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/__pycache__/fancy_getopt.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/__pycache__/file_util.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/__pycache__/filelist.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/__pycache__/log.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/__pycache__/msvc9compiler.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/__pycache__/msvccompiler.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/__pycache__/spawn.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/__pycache__/sysconfig.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/__pycache__/text_file.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/__pycache__/unixccompiler.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/__pycache__/util.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/__pycache__/version.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/__pycache__/versionpredicate.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/command/__pycache__/.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/command/__pycache__/__init__.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/command/__pycache__/bdist.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/command/__pycache__/bdist_dumb.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/command/__pycache__/bdist_msi.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/command/__pycache__/bdist_rpm.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/command/__pycache__/bdist_wininst.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/command/__pycache__/build.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/command/__pycache__/build_clib.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/command/__pycache__/build_ext.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/command/__pycache__/build_py.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/command/__pycache__/build_scripts.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/command/__pycache__/check.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/command/__pycache__/clean.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/command/__pycache__/config.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/command/__pycache__/install.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/command/__pycache__/install_data.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/command/__pycache__/install_egg_info.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/command/__pycache__/install_headers.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/command/__pycache__/install_lib.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/command/__pycache__/install_scripts.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/command/__pycache__/register.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/command/__pycache__/sdist.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/command/__pycache__/upload.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/__init__.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/support.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_archive_util.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_bdist.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_bdist_dumb.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_bdist_msi.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_bdist_rpm.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_bdist_wininst.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_build.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_build_clib.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_build_ext.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_build_py.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_build_scripts.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_check.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_clean.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_cmd.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_config.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_config_cmd.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_core.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_cygwinccompiler.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_dep_util.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_dir_util.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_dist.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_extension.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_file_util.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_filelist.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_install.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_install_data.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_install_headers.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_install_lib.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_install_scripts.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_log.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_msvc9compiler.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_msvccompiler.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_register.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_sdist.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_spawn.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_sysconfig.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_text_file.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_unixccompiler.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_upload.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_util.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_version.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/tests/__pycache__/test_versionpredicate.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/email/__pycache__/.zip.o	\
	o/$(MODE)/third_party/python/Lib/email/__pycache__/__init__.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/email/__pycache__/_encoded_words.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/email/__pycache__/_header_value_parser.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/email/__pycache__/_parseaddr.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/email/__pycache__/_policybase.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/email/__pycache__/base64mime.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/email/__pycache__/charset.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/email/__pycache__/contentmanager.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/email/__pycache__/encoders.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/email/__pycache__/errors.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/email/__pycache__/feedparser.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/email/__pycache__/generator.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/email/__pycache__/header.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/email/__pycache__/headerregistry.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/email/__pycache__/iterators.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/email/__pycache__/message.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/email/__pycache__/parser.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/email/__pycache__/policy.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/email/__pycache__/quoprimime.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/email/__pycache__/utils.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/email/mime/__pycache__/.zip.o	\
	o/$(MODE)/third_party/python/Lib/email/mime/__pycache__/__init__.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/email/mime/__pycache__/application.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/email/mime/__pycache__/audio.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/email/mime/__pycache__/base.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/email/mime/__pycache__/image.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/email/mime/__pycache__/message.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/email/mime/__pycache__/multipart.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/email/mime/__pycache__/nonmultipart.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/email/mime/__pycache__/text.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/__pycache__/.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/__pycache__/__init__.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/__pycache__/aliases.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/__pycache__/ascii.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/__pycache__/base64_codec.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/__pycache__/big5.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/__pycache__/big5hkscs.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/__pycache__/bz2_codec.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/__pycache__/charmap.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp037.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp1006.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp1026.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp1125.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp1140.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp1250.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp1251.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp1252.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp1253.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp1254.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp1255.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp1256.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp1257.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp1258.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp273.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp424.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp437.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp500.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp65001.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp720.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp737.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp775.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp850.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp852.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp855.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp856.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp857.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp858.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp860.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp861.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp862.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp863.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp864.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp865.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp866.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp869.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp874.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp875.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp932.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp949.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/__pycache__/cp950.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/__pycache__/euc_jis_2004.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/__pycache__/euc_jisx0213.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/__pycache__/euc_jp.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/__pycache__/euc_kr.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/__pycache__/gb18030.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/__pycache__/gb2312.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/__pycache__/gbk.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/__pycache__/hex_codec.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/__pycache__/hp_roman8.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/__pycache__/hz.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/__pycache__/idna.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/__pycache__/iso2022_jp.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/__pycache__/iso2022_jp_1.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/__pycache__/iso2022_jp_2.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/__pycache__/iso2022_jp_2004.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/__pycache__/iso2022_jp_3.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/__pycache__/iso2022_jp_ext.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/__pycache__/iso2022_kr.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/__pycache__/iso8859_1.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/__pycache__/iso8859_10.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/__pycache__/iso8859_11.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/__pycache__/iso8859_13.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/__pycache__/iso8859_14.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/__pycache__/iso8859_15.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/__pycache__/iso8859_16.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/__pycache__/iso8859_2.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/__pycache__/iso8859_3.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/__pycache__/iso8859_4.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/__pycache__/iso8859_5.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/__pycache__/iso8859_6.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/__pycache__/iso8859_7.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/__pycache__/iso8859_8.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/__pycache__/iso8859_9.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/__pycache__/johab.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/__pycache__/koi8_r.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/__pycache__/koi8_t.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/__pycache__/koi8_u.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/__pycache__/kz1048.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/__pycache__/latin_1.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/__pycache__/mac_arabic.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/__pycache__/mac_centeuro.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/__pycache__/mac_croatian.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/__pycache__/mac_cyrillic.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/__pycache__/mac_farsi.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/__pycache__/mac_greek.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/__pycache__/mac_iceland.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/__pycache__/mac_latin2.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/__pycache__/mac_roman.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/__pycache__/mac_romanian.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/__pycache__/mac_turkish.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/__pycache__/mbcs.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/__pycache__/oem.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/__pycache__/palmos.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/__pycache__/ptcp154.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/__pycache__/punycode.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/__pycache__/quopri_codec.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/__pycache__/raw_unicode_escape.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/__pycache__/rot_13.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/__pycache__/shift_jis.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/__pycache__/shift_jis_2004.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/__pycache__/shift_jisx0213.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/__pycache__/tis_620.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/__pycache__/undefined.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/__pycache__/unicode_escape.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/__pycache__/unicode_internal.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/__pycache__/utf_16.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/__pycache__/utf_16_be.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/__pycache__/utf_16_le.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/__pycache__/utf_32.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/__pycache__/utf_32_be.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/__pycache__/utf_32_le.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/__pycache__/utf_7.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/__pycache__/utf_8.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/__pycache__/utf_8_sig.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/__pycache__/uu_codec.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/__pycache__/zlib_codec.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/ensurepip/__pycache__/.zip.o	\
	o/$(MODE)/third_party/python/Lib/ensurepip/__pycache__/__init__.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/ensurepip/__pycache__/__main__.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/ensurepip/__pycache__/_uninstall.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/html/__pycache__/.zip.o	\
	o/$(MODE)/third_party/python/Lib/html/__pycache__/__init__.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/html/__pycache__/entities.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/html/__pycache__/parser.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/http/__pycache__/.zip.o	\
	o/$(MODE)/third_party/python/Lib/http/__pycache__/__init__.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/http/__pycache__/client.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/http/__pycache__/cookiejar.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/http/__pycache__/cookies.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/http/__pycache__/server.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/importlib/__pycache__/.zip.o	\
	o/$(MODE)/third_party/python/Lib/importlib/__pycache__/__init__.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/importlib/__pycache__/_bootstrap.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/importlib/__pycache__/_bootstrap_external.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/importlib/__pycache__/abc.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/importlib/__pycache__/machinery.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/importlib/__pycache__/util.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/json/__pycache__/.zip.o	\
	o/$(MODE)/third_party/python/Lib/json/__pycache__/__init__.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/json/__pycache__/decoder.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/json/__pycache__/encoder.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/json/__pycache__/scanner.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/json/__pycache__/tool.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/logging/__pycache__/.zip.o	\
	o/$(MODE)/third_party/python/Lib/logging/__pycache__/__init__.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/logging/__pycache__/config.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/logging/__pycache__/handlers.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/msilib/__pycache__/.zip.o	\
	o/$(MODE)/third_party/python/Lib/msilib/__pycache__/__init__.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/msilib/__pycache__/schema.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/msilib/__pycache__/sequence.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/msilib/__pycache__/text.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/multiprocessing/__pycache__/.zip.o	\
	o/$(MODE)/third_party/python/Lib/multiprocessing/__pycache__/__init__.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/multiprocessing/__pycache__/connection.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/multiprocessing/__pycache__/context.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/multiprocessing/__pycache__/forkserver.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/multiprocessing/__pycache__/heap.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/multiprocessing/__pycache__/managers.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/multiprocessing/__pycache__/pool.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/multiprocessing/__pycache__/popen_fork.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/multiprocessing/__pycache__/popen_forkserver.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/multiprocessing/__pycache__/popen_spawn_posix.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/multiprocessing/__pycache__/popen_spawn_win32.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/multiprocessing/__pycache__/process.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/multiprocessing/__pycache__/queues.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/multiprocessing/__pycache__/reduction.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/multiprocessing/__pycache__/resource_sharer.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/multiprocessing/__pycache__/semaphore_tracker.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/multiprocessing/__pycache__/sharedctypes.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/multiprocessing/__pycache__/spawn.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/multiprocessing/__pycache__/synchronize.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/multiprocessing/__pycache__/util.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/multiprocessing/dummy/__pycache__/.zip.o	\
	o/$(MODE)/third_party/python/Lib/multiprocessing/dummy/__pycache__/__init__.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/multiprocessing/dummy/__pycache__/connection.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/sqlite3/__pycache__/.zip.o	\
	o/$(MODE)/third_party/python/Lib/sqlite3/__pycache__/__init__.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/sqlite3/__pycache__/dbapi2.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/sqlite3/__pycache__/dump.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/unittest/__pycache__/.zip.o	\
	o/$(MODE)/third_party/python/Lib/unittest/__pycache__/__init__.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/unittest/__pycache__/__main__.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/unittest/__pycache__/case.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/unittest/__pycache__/loader.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/unittest/__pycache__/main.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/unittest/__pycache__/mock.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/unittest/__pycache__/result.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/unittest/__pycache__/runner.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/unittest/__pycache__/signals.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/unittest/__pycache__/suite.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/unittest/__pycache__/util.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/urllib/__pycache__/.zip.o	\
	o/$(MODE)/third_party/python/Lib/urllib/__pycache__/__init__.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/urllib/__pycache__/error.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/urllib/__pycache__/parse.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/urllib/__pycache__/request.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/urllib/__pycache__/response.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/urllib/__pycache__/robotparser.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/venv/__pycache__/.zip.o	\
	o/$(MODE)/third_party/python/Lib/venv/__pycache__/__init__.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/venv/__pycache__/__main__.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/wsgiref/__pycache__/.zip.o	\
	o/$(MODE)/third_party/python/Lib/wsgiref/__pycache__/__init__.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/wsgiref/__pycache__/handlers.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/wsgiref/__pycache__/headers.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/wsgiref/__pycache__/simple_server.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/wsgiref/__pycache__/util.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/wsgiref/__pycache__/validate.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/xml/__pycache__/.zip.o	\
	o/$(MODE)/third_party/python/Lib/xml/__pycache__/__init__.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/xml/dom/__pycache__/.zip.o	\
	o/$(MODE)/third_party/python/Lib/xml/dom/__pycache__/NodeFilter.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/xml/dom/__pycache__/__init__.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/xml/dom/__pycache__/domreg.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/xml/dom/__pycache__/expatbuilder.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/xml/dom/__pycache__/minicompat.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/xml/dom/__pycache__/minidom.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/xml/dom/__pycache__/pulldom.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/xml/dom/__pycache__/xmlbuilder.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/xml/etree/__pycache__/.zip.o	\
	o/$(MODE)/third_party/python/Lib/xml/etree/__pycache__/ElementInclude.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/xml/etree/__pycache__/ElementPath.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/xml/etree/__pycache__/ElementTree.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/xml/etree/__pycache__/__init__.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/xml/etree/__pycache__/cElementTree.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/xml/parsers/__pycache__/.zip.o	\
	o/$(MODE)/third_party/python/Lib/xml/parsers/__pycache__/__init__.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/xml/parsers/__pycache__/expat.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/xml/sax/__pycache__/.zip.o	\
	o/$(MODE)/third_party/python/Lib/xml/sax/__pycache__/__init__.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/xml/sax/__pycache__/_exceptions.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/xml/sax/__pycache__/expatreader.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/xml/sax/__pycache__/handler.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/xml/sax/__pycache__/saxutils.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/xml/sax/__pycache__/xmlreader.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/xmlrpc/__pycache__/.zip.o	\
	o/$(MODE)/third_party/python/Lib/xmlrpc/__pycache__/__init__.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/xmlrpc/__pycache__/client.cpython-36.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/xmlrpc/__pycache__/server.cpython-36.pyc.zip.o

$(THIRD_PARTY_PYTHON_STDLIB_PYC_OBJS):								\
		ZIPOBJ_FLAGS +=									\
			-P.python								\
			-C5
