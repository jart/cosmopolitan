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

THIRD_PARTY_PYTHON_STDLIB_PY_OBJS = $(THIRD_PARTY_PYTHON_STDLIB_PY:%=o/$(MODE)/%.zip.o)

$(THIRD_PARTY_PYTHON_STDLIB_PY_OBJS):								\
		third_party/python/python-stdlib.mk

$(THIRD_PARTY_PYTHON_STDLIB_PY_OBJS):								\
		ZIPOBJ_FLAGS +=									\
			-P.python								\
			-C3

o/$(MODE)/third_party/python/Lib/:	\
		o/$(MODE)/third_party/python/Lib/
	@mkdir -p $@

o/$(MODE)/third_party/python/Lib/.zip.o:	\
		o/$(MODE)/third_party/python/Lib/
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/__future__.pyc:	\
		third_party/python/Lib/__future__.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/__future__.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/__future__.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/_bootlocale.pyc:	\
		third_party/python/Lib/_bootlocale.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/_bootlocale.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/_bootlocale.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/_collections_abc.pyc:	\
		third_party/python/Lib/_collections_abc.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/_collections_abc.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/_collections_abc.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/_compat_pickle.pyc:	\
		third_party/python/Lib/_compat_pickle.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/_compat_pickle.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/_compat_pickle.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/_compression.pyc:	\
		third_party/python/Lib/_compression.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/_compression.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/_compression.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/_dummy_thread.pyc:	\
		third_party/python/Lib/_dummy_thread.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/_dummy_thread.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/_dummy_thread.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/_markupbase.pyc:	\
		third_party/python/Lib/_markupbase.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/_markupbase.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/_markupbase.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/_osx_support.pyc:	\
		third_party/python/Lib/_osx_support.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/_osx_support.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/_osx_support.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/_pyio.pyc:	\
		third_party/python/Lib/_pyio.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/_pyio.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/_pyio.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/_sitebuiltins.pyc:	\
		third_party/python/Lib/_sitebuiltins.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/_sitebuiltins.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/_sitebuiltins.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/_strptime.pyc:	\
		third_party/python/Lib/_strptime.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/_strptime.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/_strptime.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/_sysconfigdata_m_cosmo_x86_64-cosmo.pyc:	\
		third_party/python/Lib/_sysconfigdata_m_cosmo_x86_64-cosmo.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/_sysconfigdata_m_cosmo_x86_64-cosmo.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/_sysconfigdata_m_cosmo_x86_64-cosmo.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/_threading_local.pyc:	\
		third_party/python/Lib/_threading_local.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/_threading_local.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/_threading_local.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/_weakrefset.pyc:	\
		third_party/python/Lib/_weakrefset.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/_weakrefset.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/_weakrefset.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/abc.pyc:	\
		third_party/python/Lib/abc.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/abc.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/abc.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/aifc.pyc:	\
		third_party/python/Lib/aifc.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/aifc.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/aifc.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/antigravity.pyc:	\
		third_party/python/Lib/antigravity.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/antigravity.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/antigravity.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/argparse.pyc:	\
		third_party/python/Lib/argparse.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/argparse.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/argparse.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/ast.pyc:	\
		third_party/python/Lib/ast.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/ast.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/ast.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/asynchat.pyc:	\
		third_party/python/Lib/asynchat.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/asynchat.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/asynchat.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/asyncio/:	\
		o/$(MODE)/third_party/python/Lib/asyncio/
	@mkdir -p $@

o/$(MODE)/third_party/python/Lib/asyncio/.zip.o:	\
		o/$(MODE)/third_party/python/Lib/asyncio/
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/asyncio/__init__.pyc:	\
		third_party/python/Lib/asyncio/__init__.py	\
		o/$(MODE)/third_party/python/Lib/asyncio/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/asyncio/__init__.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/asyncio/__init__.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/asyncio/base_events.pyc:	\
		third_party/python/Lib/asyncio/base_events.py	\
		o/$(MODE)/third_party/python/Lib/asyncio/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/asyncio/base_events.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/asyncio/base_events.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/asyncio/base_futures.pyc:	\
		third_party/python/Lib/asyncio/base_futures.py	\
		o/$(MODE)/third_party/python/Lib/asyncio/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/asyncio/base_futures.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/asyncio/base_futures.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/asyncio/base_subprocess.pyc:	\
		third_party/python/Lib/asyncio/base_subprocess.py	\
		o/$(MODE)/third_party/python/Lib/asyncio/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/asyncio/base_subprocess.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/asyncio/base_subprocess.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/asyncio/base_tasks.pyc:	\
		third_party/python/Lib/asyncio/base_tasks.py	\
		o/$(MODE)/third_party/python/Lib/asyncio/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/asyncio/base_tasks.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/asyncio/base_tasks.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/asyncio/compat.pyc:	\
		third_party/python/Lib/asyncio/compat.py	\
		o/$(MODE)/third_party/python/Lib/asyncio/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/asyncio/compat.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/asyncio/compat.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/asyncio/constants.pyc:	\
		third_party/python/Lib/asyncio/constants.py	\
		o/$(MODE)/third_party/python/Lib/asyncio/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/asyncio/constants.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/asyncio/constants.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/asyncio/coroutines.pyc:	\
		third_party/python/Lib/asyncio/coroutines.py	\
		o/$(MODE)/third_party/python/Lib/asyncio/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/asyncio/coroutines.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/asyncio/coroutines.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/asyncio/events.pyc:	\
		third_party/python/Lib/asyncio/events.py	\
		o/$(MODE)/third_party/python/Lib/asyncio/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/asyncio/events.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/asyncio/events.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/asyncio/futures.pyc:	\
		third_party/python/Lib/asyncio/futures.py	\
		o/$(MODE)/third_party/python/Lib/asyncio/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/asyncio/futures.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/asyncio/futures.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/asyncio/locks.pyc:	\
		third_party/python/Lib/asyncio/locks.py	\
		o/$(MODE)/third_party/python/Lib/asyncio/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/asyncio/locks.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/asyncio/locks.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/asyncio/log.pyc:	\
		third_party/python/Lib/asyncio/log.py	\
		o/$(MODE)/third_party/python/Lib/asyncio/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/asyncio/log.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/asyncio/log.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/asyncio/proactor_events.pyc:	\
		third_party/python/Lib/asyncio/proactor_events.py	\
		o/$(MODE)/third_party/python/Lib/asyncio/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/asyncio/proactor_events.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/asyncio/proactor_events.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/asyncio/protocols.pyc:	\
		third_party/python/Lib/asyncio/protocols.py	\
		o/$(MODE)/third_party/python/Lib/asyncio/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/asyncio/protocols.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/asyncio/protocols.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/asyncio/queues.pyc:	\
		third_party/python/Lib/asyncio/queues.py	\
		o/$(MODE)/third_party/python/Lib/asyncio/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/asyncio/queues.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/asyncio/queues.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/asyncio/selector_events.pyc:	\
		third_party/python/Lib/asyncio/selector_events.py	\
		o/$(MODE)/third_party/python/Lib/asyncio/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/asyncio/selector_events.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/asyncio/selector_events.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/asyncio/sslproto.pyc:	\
		third_party/python/Lib/asyncio/sslproto.py	\
		o/$(MODE)/third_party/python/Lib/asyncio/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/asyncio/sslproto.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/asyncio/sslproto.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/asyncio/streams.pyc:	\
		third_party/python/Lib/asyncio/streams.py	\
		o/$(MODE)/third_party/python/Lib/asyncio/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/asyncio/streams.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/asyncio/streams.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/asyncio/subprocess.pyc:	\
		third_party/python/Lib/asyncio/subprocess.py	\
		o/$(MODE)/third_party/python/Lib/asyncio/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/asyncio/subprocess.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/asyncio/subprocess.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/asyncio/tasks.pyc:	\
		third_party/python/Lib/asyncio/tasks.py	\
		o/$(MODE)/third_party/python/Lib/asyncio/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/asyncio/tasks.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/asyncio/tasks.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/asyncio/test_utils.pyc:	\
		third_party/python/Lib/asyncio/test_utils.py	\
		o/$(MODE)/third_party/python/Lib/asyncio/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/asyncio/test_utils.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/asyncio/test_utils.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/asyncio/transports.pyc:	\
		third_party/python/Lib/asyncio/transports.py	\
		o/$(MODE)/third_party/python/Lib/asyncio/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/asyncio/transports.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/asyncio/transports.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/asyncio/unix_events.pyc:	\
		third_party/python/Lib/asyncio/unix_events.py	\
		o/$(MODE)/third_party/python/Lib/asyncio/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/asyncio/unix_events.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/asyncio/unix_events.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/asyncio/windows_events.pyc:	\
		third_party/python/Lib/asyncio/windows_events.py	\
		o/$(MODE)/third_party/python/Lib/asyncio/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/asyncio/windows_events.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/asyncio/windows_events.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/asyncio/windows_utils.pyc:	\
		third_party/python/Lib/asyncio/windows_utils.py	\
		o/$(MODE)/third_party/python/Lib/asyncio/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/asyncio/windows_utils.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/asyncio/windows_utils.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/asyncore.pyc:	\
		third_party/python/Lib/asyncore.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/asyncore.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/asyncore.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/base64.pyc:	\
		third_party/python/Lib/base64.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/base64.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/base64.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/bdb.pyc:	\
		third_party/python/Lib/bdb.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/bdb.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/bdb.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/binhex.pyc:	\
		third_party/python/Lib/binhex.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/binhex.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/binhex.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/bisect.pyc:	\
		third_party/python/Lib/bisect.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/bisect.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/bisect.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/bz2.pyc:	\
		third_party/python/Lib/bz2.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/bz2.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/bz2.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/cProfile.pyc:	\
		third_party/python/Lib/cProfile.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/cProfile.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/cProfile.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/calendar.pyc:	\
		third_party/python/Lib/calendar.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/calendar.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/calendar.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/cgi.pyc:	\
		third_party/python/Lib/cgi.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/cgi.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/cgi.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/cgitb.pyc:	\
		third_party/python/Lib/cgitb.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/cgitb.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/cgitb.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/chunk.pyc:	\
		third_party/python/Lib/chunk.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/chunk.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/chunk.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/cmd.pyc:	\
		third_party/python/Lib/cmd.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/cmd.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/cmd.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/code.pyc:	\
		third_party/python/Lib/code.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/code.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/code.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/codecs.pyc:	\
		third_party/python/Lib/codecs.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/codecs.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/codecs.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/codeop.pyc:	\
		third_party/python/Lib/codeop.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/codeop.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/codeop.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/collections/:	\
		o/$(MODE)/third_party/python/Lib/collections/
	@mkdir -p $@

o/$(MODE)/third_party/python/Lib/collections/.zip.o:	\
		o/$(MODE)/third_party/python/Lib/collections/
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/collections/__init__.pyc:	\
		third_party/python/Lib/collections/__init__.py	\
		o/$(MODE)/third_party/python/Lib/collections/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/collections/__init__.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/collections/__init__.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/collections/abc.pyc:	\
		third_party/python/Lib/collections/abc.py	\
		o/$(MODE)/third_party/python/Lib/collections/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/collections/abc.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/collections/abc.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/colorsys.pyc:	\
		third_party/python/Lib/colorsys.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/colorsys.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/colorsys.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/compileall.pyc:	\
		third_party/python/Lib/compileall.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/compileall.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/compileall.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/configparser.pyc:	\
		third_party/python/Lib/configparser.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/configparser.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/configparser.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/contextlib.pyc:	\
		third_party/python/Lib/contextlib.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/contextlib.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/contextlib.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/copy.pyc:	\
		third_party/python/Lib/copy.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/copy.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/copy.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/copyreg.pyc:	\
		third_party/python/Lib/copyreg.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/copyreg.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/copyreg.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/crypt.pyc:	\
		third_party/python/Lib/crypt.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/crypt.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/crypt.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/csv.pyc:	\
		third_party/python/Lib/csv.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/csv.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/csv.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/datetime.pyc:	\
		third_party/python/Lib/datetime.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/datetime.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/datetime.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/dbm/:	\
		o/$(MODE)/third_party/python/Lib/dbm/
	@mkdir -p $@

o/$(MODE)/third_party/python/Lib/dbm/.zip.o:	\
		o/$(MODE)/third_party/python/Lib/dbm/
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/dbm/__init__.pyc:	\
		third_party/python/Lib/dbm/__init__.py	\
		o/$(MODE)/third_party/python/Lib/dbm/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/dbm/__init__.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/dbm/__init__.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/dbm/dumb.pyc:	\
		third_party/python/Lib/dbm/dumb.py	\
		o/$(MODE)/third_party/python/Lib/dbm/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/dbm/dumb.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/dbm/dumb.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/dbm/gnu.pyc:	\
		third_party/python/Lib/dbm/gnu.py	\
		o/$(MODE)/third_party/python/Lib/dbm/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/dbm/gnu.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/dbm/gnu.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/dbm/ndbm.pyc:	\
		third_party/python/Lib/dbm/ndbm.py	\
		o/$(MODE)/third_party/python/Lib/dbm/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/dbm/ndbm.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/dbm/ndbm.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/decimal.pyc:	\
		third_party/python/Lib/decimal.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/decimal.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/decimal.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/difflib.pyc:	\
		third_party/python/Lib/difflib.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/difflib.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/difflib.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/dis.pyc:	\
		third_party/python/Lib/dis.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/dis.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/dis.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/:	\
		o/$(MODE)/third_party/python/Lib/distutils/
	@mkdir -p $@

o/$(MODE)/third_party/python/Lib/distutils/.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/__init__.pyc:	\
		third_party/python/Lib/distutils/__init__.py	\
		o/$(MODE)/third_party/python/Lib/distutils/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/__init__.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/__init__.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/_msvccompiler.pyc:	\
		third_party/python/Lib/distutils/_msvccompiler.py	\
		o/$(MODE)/third_party/python/Lib/distutils/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/_msvccompiler.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/_msvccompiler.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/archive_util.pyc:	\
		third_party/python/Lib/distutils/archive_util.py	\
		o/$(MODE)/third_party/python/Lib/distutils/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/archive_util.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/archive_util.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/bcppcompiler.pyc:	\
		third_party/python/Lib/distutils/bcppcompiler.py	\
		o/$(MODE)/third_party/python/Lib/distutils/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/bcppcompiler.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/bcppcompiler.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/ccompiler.pyc:	\
		third_party/python/Lib/distutils/ccompiler.py	\
		o/$(MODE)/third_party/python/Lib/distutils/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/ccompiler.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/ccompiler.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/cmd.pyc:	\
		third_party/python/Lib/distutils/cmd.py	\
		o/$(MODE)/third_party/python/Lib/distutils/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/cmd.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/cmd.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/command/:	\
		o/$(MODE)/third_party/python/Lib/distutils/command/
	@mkdir -p $@

o/$(MODE)/third_party/python/Lib/distutils/command/.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/command/
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/command/__init__.pyc:	\
		third_party/python/Lib/distutils/command/__init__.py	\
		o/$(MODE)/third_party/python/Lib/distutils/command/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/command/__init__.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/command/__init__.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/command/bdist.pyc:	\
		third_party/python/Lib/distutils/command/bdist.py	\
		o/$(MODE)/third_party/python/Lib/distutils/command/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/command/bdist.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/command/bdist.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/command/bdist_dumb.pyc:	\
		third_party/python/Lib/distutils/command/bdist_dumb.py	\
		o/$(MODE)/third_party/python/Lib/distutils/command/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/command/bdist_dumb.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/command/bdist_dumb.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/command/bdist_msi.pyc:	\
		third_party/python/Lib/distutils/command/bdist_msi.py	\
		o/$(MODE)/third_party/python/Lib/distutils/command/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/command/bdist_msi.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/command/bdist_msi.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/command/bdist_rpm.pyc:	\
		third_party/python/Lib/distutils/command/bdist_rpm.py	\
		o/$(MODE)/third_party/python/Lib/distutils/command/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/command/bdist_rpm.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/command/bdist_rpm.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/command/bdist_wininst.pyc:	\
		third_party/python/Lib/distutils/command/bdist_wininst.py	\
		o/$(MODE)/third_party/python/Lib/distutils/command/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/command/bdist_wininst.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/command/bdist_wininst.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/command/build.pyc:	\
		third_party/python/Lib/distutils/command/build.py	\
		o/$(MODE)/third_party/python/Lib/distutils/command/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/command/build.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/command/build.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/command/build_clib.pyc:	\
		third_party/python/Lib/distutils/command/build_clib.py	\
		o/$(MODE)/third_party/python/Lib/distutils/command/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/command/build_clib.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/command/build_clib.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/command/build_ext.pyc:	\
		third_party/python/Lib/distutils/command/build_ext.py	\
		o/$(MODE)/third_party/python/Lib/distutils/command/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/command/build_ext.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/command/build_ext.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/command/build_py.pyc:	\
		third_party/python/Lib/distutils/command/build_py.py	\
		o/$(MODE)/third_party/python/Lib/distutils/command/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/command/build_py.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/command/build_py.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/command/build_scripts.pyc:	\
		third_party/python/Lib/distutils/command/build_scripts.py	\
		o/$(MODE)/third_party/python/Lib/distutils/command/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/command/build_scripts.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/command/build_scripts.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/command/check.pyc:	\
		third_party/python/Lib/distutils/command/check.py	\
		o/$(MODE)/third_party/python/Lib/distutils/command/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/command/check.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/command/check.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/command/clean.pyc:	\
		third_party/python/Lib/distutils/command/clean.py	\
		o/$(MODE)/third_party/python/Lib/distutils/command/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/command/clean.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/command/clean.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/command/config.pyc:	\
		third_party/python/Lib/distutils/command/config.py	\
		o/$(MODE)/third_party/python/Lib/distutils/command/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/command/config.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/command/config.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/command/install.pyc:	\
		third_party/python/Lib/distutils/command/install.py	\
		o/$(MODE)/third_party/python/Lib/distutils/command/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/command/install.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/command/install.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/command/install_data.pyc:	\
		third_party/python/Lib/distutils/command/install_data.py	\
		o/$(MODE)/third_party/python/Lib/distutils/command/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/command/install_data.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/command/install_data.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/command/install_egg_info.pyc:	\
		third_party/python/Lib/distutils/command/install_egg_info.py	\
		o/$(MODE)/third_party/python/Lib/distutils/command/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/command/install_egg_info.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/command/install_egg_info.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/command/install_headers.pyc:	\
		third_party/python/Lib/distutils/command/install_headers.py	\
		o/$(MODE)/third_party/python/Lib/distutils/command/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/command/install_headers.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/command/install_headers.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/command/install_lib.pyc:	\
		third_party/python/Lib/distutils/command/install_lib.py	\
		o/$(MODE)/third_party/python/Lib/distutils/command/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/command/install_lib.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/command/install_lib.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/command/install_scripts.pyc:	\
		third_party/python/Lib/distutils/command/install_scripts.py	\
		o/$(MODE)/third_party/python/Lib/distutils/command/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/command/install_scripts.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/command/install_scripts.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/command/register.pyc:	\
		third_party/python/Lib/distutils/command/register.py	\
		o/$(MODE)/third_party/python/Lib/distutils/command/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/command/register.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/command/register.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/command/sdist.pyc:	\
		third_party/python/Lib/distutils/command/sdist.py	\
		o/$(MODE)/third_party/python/Lib/distutils/command/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/command/sdist.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/command/sdist.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/command/upload.pyc:	\
		third_party/python/Lib/distutils/command/upload.py	\
		o/$(MODE)/third_party/python/Lib/distutils/command/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/command/upload.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/command/upload.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/config.pyc:	\
		third_party/python/Lib/distutils/config.py	\
		o/$(MODE)/third_party/python/Lib/distutils/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/config.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/config.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/core.pyc:	\
		third_party/python/Lib/distutils/core.py	\
		o/$(MODE)/third_party/python/Lib/distutils/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/core.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/core.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/cygwinccompiler.pyc:	\
		third_party/python/Lib/distutils/cygwinccompiler.py	\
		o/$(MODE)/third_party/python/Lib/distutils/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/cygwinccompiler.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/cygwinccompiler.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/debug.pyc:	\
		third_party/python/Lib/distutils/debug.py	\
		o/$(MODE)/third_party/python/Lib/distutils/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/debug.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/debug.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/dep_util.pyc:	\
		third_party/python/Lib/distutils/dep_util.py	\
		o/$(MODE)/third_party/python/Lib/distutils/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/dep_util.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/dep_util.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/dir_util.pyc:	\
		third_party/python/Lib/distutils/dir_util.py	\
		o/$(MODE)/third_party/python/Lib/distutils/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/dir_util.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/dir_util.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/dist.pyc:	\
		third_party/python/Lib/distutils/dist.py	\
		o/$(MODE)/third_party/python/Lib/distutils/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/dist.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/dist.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/errors.pyc:	\
		third_party/python/Lib/distutils/errors.py	\
		o/$(MODE)/third_party/python/Lib/distutils/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/errors.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/errors.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/extension.pyc:	\
		third_party/python/Lib/distutils/extension.py	\
		o/$(MODE)/third_party/python/Lib/distutils/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/extension.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/extension.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/fancy_getopt.pyc:	\
		third_party/python/Lib/distutils/fancy_getopt.py	\
		o/$(MODE)/third_party/python/Lib/distutils/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/fancy_getopt.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/fancy_getopt.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/file_util.pyc:	\
		third_party/python/Lib/distutils/file_util.py	\
		o/$(MODE)/third_party/python/Lib/distutils/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/file_util.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/file_util.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/filelist.pyc:	\
		third_party/python/Lib/distutils/filelist.py	\
		o/$(MODE)/third_party/python/Lib/distutils/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/filelist.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/filelist.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/log.pyc:	\
		third_party/python/Lib/distutils/log.py	\
		o/$(MODE)/third_party/python/Lib/distutils/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/log.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/log.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/msvc9compiler.pyc:	\
		third_party/python/Lib/distutils/msvc9compiler.py	\
		o/$(MODE)/third_party/python/Lib/distutils/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/msvc9compiler.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/msvc9compiler.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/msvccompiler.pyc:	\
		third_party/python/Lib/distutils/msvccompiler.py	\
		o/$(MODE)/third_party/python/Lib/distutils/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/msvccompiler.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/msvccompiler.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/spawn.pyc:	\
		third_party/python/Lib/distutils/spawn.py	\
		o/$(MODE)/third_party/python/Lib/distutils/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/spawn.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/spawn.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/sysconfig.pyc:	\
		third_party/python/Lib/distutils/sysconfig.py	\
		o/$(MODE)/third_party/python/Lib/distutils/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/sysconfig.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/sysconfig.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/tests/:	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/
	@mkdir -p $@

o/$(MODE)/third_party/python/Lib/distutils/tests/.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/tests/__init__.pyc:	\
		third_party/python/Lib/distutils/tests/__init__.py	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/tests/__init__.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/__init__.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/tests/support.pyc:	\
		third_party/python/Lib/distutils/tests/support.py	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/tests/support.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/support.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/tests/test_archive_util.pyc:	\
		third_party/python/Lib/distutils/tests/test_archive_util.py	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/tests/test_archive_util.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/test_archive_util.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/tests/test_bdist.pyc:	\
		third_party/python/Lib/distutils/tests/test_bdist.py	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/tests/test_bdist.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/test_bdist.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/tests/test_bdist_dumb.pyc:	\
		third_party/python/Lib/distutils/tests/test_bdist_dumb.py	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/tests/test_bdist_dumb.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/test_bdist_dumb.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/tests/test_bdist_msi.pyc:	\
		third_party/python/Lib/distutils/tests/test_bdist_msi.py	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/tests/test_bdist_msi.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/test_bdist_msi.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/tests/test_bdist_rpm.pyc:	\
		third_party/python/Lib/distutils/tests/test_bdist_rpm.py	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/tests/test_bdist_rpm.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/test_bdist_rpm.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/tests/test_bdist_wininst.pyc:	\
		third_party/python/Lib/distutils/tests/test_bdist_wininst.py	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/tests/test_bdist_wininst.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/test_bdist_wininst.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/tests/test_build.pyc:	\
		third_party/python/Lib/distutils/tests/test_build.py	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/tests/test_build.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/test_build.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/tests/test_build_clib.pyc:	\
		third_party/python/Lib/distutils/tests/test_build_clib.py	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/tests/test_build_clib.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/test_build_clib.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/tests/test_build_ext.pyc:	\
		third_party/python/Lib/distutils/tests/test_build_ext.py	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/tests/test_build_ext.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/test_build_ext.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/tests/test_build_py.pyc:	\
		third_party/python/Lib/distutils/tests/test_build_py.py	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/tests/test_build_py.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/test_build_py.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/tests/test_build_scripts.pyc:	\
		third_party/python/Lib/distutils/tests/test_build_scripts.py	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/tests/test_build_scripts.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/test_build_scripts.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/tests/test_check.pyc:	\
		third_party/python/Lib/distutils/tests/test_check.py	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/tests/test_check.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/test_check.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/tests/test_clean.pyc:	\
		third_party/python/Lib/distutils/tests/test_clean.py	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/tests/test_clean.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/test_clean.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/tests/test_cmd.pyc:	\
		third_party/python/Lib/distutils/tests/test_cmd.py	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/tests/test_cmd.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/test_cmd.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/tests/test_config.pyc:	\
		third_party/python/Lib/distutils/tests/test_config.py	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/tests/test_config.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/test_config.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/tests/test_config_cmd.pyc:	\
		third_party/python/Lib/distutils/tests/test_config_cmd.py	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/tests/test_config_cmd.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/test_config_cmd.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/tests/test_core.pyc:	\
		third_party/python/Lib/distutils/tests/test_core.py	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/tests/test_core.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/test_core.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/tests/test_cygwinccompiler.pyc:	\
		third_party/python/Lib/distutils/tests/test_cygwinccompiler.py	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/tests/test_cygwinccompiler.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/test_cygwinccompiler.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/tests/test_dep_util.pyc:	\
		third_party/python/Lib/distutils/tests/test_dep_util.py	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/tests/test_dep_util.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/test_dep_util.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/tests/test_dir_util.pyc:	\
		third_party/python/Lib/distutils/tests/test_dir_util.py	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/tests/test_dir_util.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/test_dir_util.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/tests/test_dist.pyc:	\
		third_party/python/Lib/distutils/tests/test_dist.py	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/tests/test_dist.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/test_dist.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/tests/test_extension.pyc:	\
		third_party/python/Lib/distutils/tests/test_extension.py	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/tests/test_extension.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/test_extension.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/tests/test_file_util.pyc:	\
		third_party/python/Lib/distutils/tests/test_file_util.py	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/tests/test_file_util.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/test_file_util.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/tests/test_filelist.pyc:	\
		third_party/python/Lib/distutils/tests/test_filelist.py	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/tests/test_filelist.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/test_filelist.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/tests/test_install.pyc:	\
		third_party/python/Lib/distutils/tests/test_install.py	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/tests/test_install.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/test_install.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/tests/test_install_data.pyc:	\
		third_party/python/Lib/distutils/tests/test_install_data.py	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/tests/test_install_data.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/test_install_data.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/tests/test_install_headers.pyc:	\
		third_party/python/Lib/distutils/tests/test_install_headers.py	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/tests/test_install_headers.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/test_install_headers.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/tests/test_install_lib.pyc:	\
		third_party/python/Lib/distutils/tests/test_install_lib.py	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/tests/test_install_lib.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/test_install_lib.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/tests/test_install_scripts.pyc:	\
		third_party/python/Lib/distutils/tests/test_install_scripts.py	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/tests/test_install_scripts.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/test_install_scripts.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/tests/test_log.pyc:	\
		third_party/python/Lib/distutils/tests/test_log.py	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/tests/test_log.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/test_log.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/tests/test_msvc9compiler.pyc:	\
		third_party/python/Lib/distutils/tests/test_msvc9compiler.py	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/tests/test_msvc9compiler.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/test_msvc9compiler.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/tests/test_msvccompiler.pyc:	\
		third_party/python/Lib/distutils/tests/test_msvccompiler.py	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/tests/test_msvccompiler.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/test_msvccompiler.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/tests/test_register.pyc:	\
		third_party/python/Lib/distutils/tests/test_register.py	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/tests/test_register.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/test_register.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/tests/test_sdist.pyc:	\
		third_party/python/Lib/distutils/tests/test_sdist.py	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/tests/test_sdist.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/test_sdist.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/tests/test_spawn.pyc:	\
		third_party/python/Lib/distutils/tests/test_spawn.py	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/tests/test_spawn.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/test_spawn.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/tests/test_sysconfig.pyc:	\
		third_party/python/Lib/distutils/tests/test_sysconfig.py	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/tests/test_sysconfig.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/test_sysconfig.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/tests/test_text_file.pyc:	\
		third_party/python/Lib/distutils/tests/test_text_file.py	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/tests/test_text_file.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/test_text_file.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/tests/test_unixccompiler.pyc:	\
		third_party/python/Lib/distutils/tests/test_unixccompiler.py	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/tests/test_unixccompiler.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/test_unixccompiler.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/tests/test_upload.pyc:	\
		third_party/python/Lib/distutils/tests/test_upload.py	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/tests/test_upload.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/test_upload.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/tests/test_util.pyc:	\
		third_party/python/Lib/distutils/tests/test_util.py	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/tests/test_util.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/test_util.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/tests/test_version.pyc:	\
		third_party/python/Lib/distutils/tests/test_version.py	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/tests/test_version.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/test_version.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/tests/test_versionpredicate.pyc:	\
		third_party/python/Lib/distutils/tests/test_versionpredicate.py	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/tests/test_versionpredicate.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/tests/test_versionpredicate.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/text_file.pyc:	\
		third_party/python/Lib/distutils/text_file.py	\
		o/$(MODE)/third_party/python/Lib/distutils/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/text_file.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/text_file.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/unixccompiler.pyc:	\
		third_party/python/Lib/distutils/unixccompiler.py	\
		o/$(MODE)/third_party/python/Lib/distutils/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/unixccompiler.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/unixccompiler.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/util.pyc:	\
		third_party/python/Lib/distutils/util.py	\
		o/$(MODE)/third_party/python/Lib/distutils/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/util.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/util.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/version.pyc:	\
		third_party/python/Lib/distutils/version.py	\
		o/$(MODE)/third_party/python/Lib/distutils/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/version.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/version.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/distutils/versionpredicate.pyc:	\
		third_party/python/Lib/distutils/versionpredicate.py	\
		o/$(MODE)/third_party/python/Lib/distutils/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/distutils/versionpredicate.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/distutils/versionpredicate.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/doctest.pyc:	\
		third_party/python/Lib/doctest.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/doctest.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/doctest.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/dummy_threading.pyc:	\
		third_party/python/Lib/dummy_threading.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/dummy_threading.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/dummy_threading.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/email/:	\
		o/$(MODE)/third_party/python/Lib/email/
	@mkdir -p $@

o/$(MODE)/third_party/python/Lib/email/.zip.o:	\
		o/$(MODE)/third_party/python/Lib/email/
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/email/__init__.pyc:	\
		third_party/python/Lib/email/__init__.py	\
		o/$(MODE)/third_party/python/Lib/email/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/email/__init__.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/email/__init__.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/email/_encoded_words.pyc:	\
		third_party/python/Lib/email/_encoded_words.py	\
		o/$(MODE)/third_party/python/Lib/email/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/email/_encoded_words.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/email/_encoded_words.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/email/_header_value_parser.pyc:	\
		third_party/python/Lib/email/_header_value_parser.py	\
		o/$(MODE)/third_party/python/Lib/email/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/email/_header_value_parser.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/email/_header_value_parser.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/email/_parseaddr.pyc:	\
		third_party/python/Lib/email/_parseaddr.py	\
		o/$(MODE)/third_party/python/Lib/email/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/email/_parseaddr.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/email/_parseaddr.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/email/_policybase.pyc:	\
		third_party/python/Lib/email/_policybase.py	\
		o/$(MODE)/third_party/python/Lib/email/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/email/_policybase.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/email/_policybase.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/email/base64mime.pyc:	\
		third_party/python/Lib/email/base64mime.py	\
		o/$(MODE)/third_party/python/Lib/email/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/email/base64mime.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/email/base64mime.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/email/charset.pyc:	\
		third_party/python/Lib/email/charset.py	\
		o/$(MODE)/third_party/python/Lib/email/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/email/charset.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/email/charset.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/email/contentmanager.pyc:	\
		third_party/python/Lib/email/contentmanager.py	\
		o/$(MODE)/third_party/python/Lib/email/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/email/contentmanager.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/email/contentmanager.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/email/encoders.pyc:	\
		third_party/python/Lib/email/encoders.py	\
		o/$(MODE)/third_party/python/Lib/email/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/email/encoders.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/email/encoders.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/email/errors.pyc:	\
		third_party/python/Lib/email/errors.py	\
		o/$(MODE)/third_party/python/Lib/email/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/email/errors.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/email/errors.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/email/feedparser.pyc:	\
		third_party/python/Lib/email/feedparser.py	\
		o/$(MODE)/third_party/python/Lib/email/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/email/feedparser.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/email/feedparser.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/email/generator.pyc:	\
		third_party/python/Lib/email/generator.py	\
		o/$(MODE)/third_party/python/Lib/email/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/email/generator.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/email/generator.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/email/header.pyc:	\
		third_party/python/Lib/email/header.py	\
		o/$(MODE)/third_party/python/Lib/email/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/email/header.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/email/header.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/email/headerregistry.pyc:	\
		third_party/python/Lib/email/headerregistry.py	\
		o/$(MODE)/third_party/python/Lib/email/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/email/headerregistry.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/email/headerregistry.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/email/iterators.pyc:	\
		third_party/python/Lib/email/iterators.py	\
		o/$(MODE)/third_party/python/Lib/email/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/email/iterators.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/email/iterators.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/email/message.pyc:	\
		third_party/python/Lib/email/message.py	\
		o/$(MODE)/third_party/python/Lib/email/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/email/message.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/email/message.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/email/mime/:	\
		o/$(MODE)/third_party/python/Lib/email/mime/
	@mkdir -p $@

o/$(MODE)/third_party/python/Lib/email/mime/.zip.o:	\
		o/$(MODE)/third_party/python/Lib/email/mime/
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/email/mime/__init__.pyc:	\
		third_party/python/Lib/email/mime/__init__.py	\
		o/$(MODE)/third_party/python/Lib/email/mime/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/email/mime/__init__.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/email/mime/__init__.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/email/mime/application.pyc:	\
		third_party/python/Lib/email/mime/application.py	\
		o/$(MODE)/third_party/python/Lib/email/mime/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/email/mime/application.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/email/mime/application.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/email/mime/audio.pyc:	\
		third_party/python/Lib/email/mime/audio.py	\
		o/$(MODE)/third_party/python/Lib/email/mime/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/email/mime/audio.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/email/mime/audio.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/email/mime/base.pyc:	\
		third_party/python/Lib/email/mime/base.py	\
		o/$(MODE)/third_party/python/Lib/email/mime/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/email/mime/base.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/email/mime/base.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/email/mime/image.pyc:	\
		third_party/python/Lib/email/mime/image.py	\
		o/$(MODE)/third_party/python/Lib/email/mime/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/email/mime/image.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/email/mime/image.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/email/mime/message.pyc:	\
		third_party/python/Lib/email/mime/message.py	\
		o/$(MODE)/third_party/python/Lib/email/mime/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/email/mime/message.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/email/mime/message.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/email/mime/multipart.pyc:	\
		third_party/python/Lib/email/mime/multipart.py	\
		o/$(MODE)/third_party/python/Lib/email/mime/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/email/mime/multipart.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/email/mime/multipart.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/email/mime/nonmultipart.pyc:	\
		third_party/python/Lib/email/mime/nonmultipart.py	\
		o/$(MODE)/third_party/python/Lib/email/mime/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/email/mime/nonmultipart.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/email/mime/nonmultipart.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/email/mime/text.pyc:	\
		third_party/python/Lib/email/mime/text.py	\
		o/$(MODE)/third_party/python/Lib/email/mime/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/email/mime/text.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/email/mime/text.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/email/parser.pyc:	\
		third_party/python/Lib/email/parser.py	\
		o/$(MODE)/third_party/python/Lib/email/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/email/parser.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/email/parser.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/email/policy.pyc:	\
		third_party/python/Lib/email/policy.py	\
		o/$(MODE)/third_party/python/Lib/email/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/email/policy.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/email/policy.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/email/quoprimime.pyc:	\
		third_party/python/Lib/email/quoprimime.py	\
		o/$(MODE)/third_party/python/Lib/email/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/email/quoprimime.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/email/quoprimime.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/email/utils.pyc:	\
		third_party/python/Lib/email/utils.py	\
		o/$(MODE)/third_party/python/Lib/email/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/email/utils.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/email/utils.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/:	\
		o/$(MODE)/third_party/python/Lib/encodings/
	@mkdir -p $@

o/$(MODE)/third_party/python/Lib/encodings/.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/__init__.pyc:	\
		third_party/python/Lib/encodings/__init__.py	\
		o/$(MODE)/third_party/python/Lib/encodings/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/__init__.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/__init__.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/aliases.pyc:	\
		third_party/python/Lib/encodings/aliases.py	\
		o/$(MODE)/third_party/python/Lib/encodings/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/aliases.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/aliases.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/ascii.pyc:	\
		third_party/python/Lib/encodings/ascii.py	\
		o/$(MODE)/third_party/python/Lib/encodings/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/ascii.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/ascii.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/base64_codec.pyc:	\
		third_party/python/Lib/encodings/base64_codec.py	\
		o/$(MODE)/third_party/python/Lib/encodings/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/base64_codec.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/base64_codec.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/big5.pyc:	\
		third_party/python/Lib/encodings/big5.py	\
		o/$(MODE)/third_party/python/Lib/encodings/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/big5.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/big5.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/big5hkscs.pyc:	\
		third_party/python/Lib/encodings/big5hkscs.py	\
		o/$(MODE)/third_party/python/Lib/encodings/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/big5hkscs.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/big5hkscs.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/bz2_codec.pyc:	\
		third_party/python/Lib/encodings/bz2_codec.py	\
		o/$(MODE)/third_party/python/Lib/encodings/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/bz2_codec.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/bz2_codec.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/charmap.pyc:	\
		third_party/python/Lib/encodings/charmap.py	\
		o/$(MODE)/third_party/python/Lib/encodings/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/charmap.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/charmap.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/cp037.pyc:	\
		third_party/python/Lib/encodings/cp037.py	\
		o/$(MODE)/third_party/python/Lib/encodings/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/cp037.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/cp037.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/cp1006.pyc:	\
		third_party/python/Lib/encodings/cp1006.py	\
		o/$(MODE)/third_party/python/Lib/encodings/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/cp1006.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/cp1006.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/cp1026.pyc:	\
		third_party/python/Lib/encodings/cp1026.py	\
		o/$(MODE)/third_party/python/Lib/encodings/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/cp1026.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/cp1026.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/cp1125.pyc:	\
		third_party/python/Lib/encodings/cp1125.py	\
		o/$(MODE)/third_party/python/Lib/encodings/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/cp1125.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/cp1125.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/cp1140.pyc:	\
		third_party/python/Lib/encodings/cp1140.py	\
		o/$(MODE)/third_party/python/Lib/encodings/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/cp1140.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/cp1140.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/cp1250.pyc:	\
		third_party/python/Lib/encodings/cp1250.py	\
		o/$(MODE)/third_party/python/Lib/encodings/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/cp1250.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/cp1250.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/cp1251.pyc:	\
		third_party/python/Lib/encodings/cp1251.py	\
		o/$(MODE)/third_party/python/Lib/encodings/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/cp1251.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/cp1251.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/cp1252.pyc:	\
		third_party/python/Lib/encodings/cp1252.py	\
		o/$(MODE)/third_party/python/Lib/encodings/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/cp1252.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/cp1252.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/cp1253.pyc:	\
		third_party/python/Lib/encodings/cp1253.py	\
		o/$(MODE)/third_party/python/Lib/encodings/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/cp1253.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/cp1253.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/cp1254.pyc:	\
		third_party/python/Lib/encodings/cp1254.py	\
		o/$(MODE)/third_party/python/Lib/encodings/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/cp1254.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/cp1254.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/cp1255.pyc:	\
		third_party/python/Lib/encodings/cp1255.py	\
		o/$(MODE)/third_party/python/Lib/encodings/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/cp1255.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/cp1255.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/cp1256.pyc:	\
		third_party/python/Lib/encodings/cp1256.py	\
		o/$(MODE)/third_party/python/Lib/encodings/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/cp1256.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/cp1256.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/cp1257.pyc:	\
		third_party/python/Lib/encodings/cp1257.py	\
		o/$(MODE)/third_party/python/Lib/encodings/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/cp1257.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/cp1257.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/cp1258.pyc:	\
		third_party/python/Lib/encodings/cp1258.py	\
		o/$(MODE)/third_party/python/Lib/encodings/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/cp1258.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/cp1258.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/cp273.pyc:	\
		third_party/python/Lib/encodings/cp273.py	\
		o/$(MODE)/third_party/python/Lib/encodings/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/cp273.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/cp273.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/cp424.pyc:	\
		third_party/python/Lib/encodings/cp424.py	\
		o/$(MODE)/third_party/python/Lib/encodings/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/cp424.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/cp424.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/cp437.pyc:	\
		third_party/python/Lib/encodings/cp437.py	\
		o/$(MODE)/third_party/python/Lib/encodings/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/cp437.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/cp437.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/cp500.pyc:	\
		third_party/python/Lib/encodings/cp500.py	\
		o/$(MODE)/third_party/python/Lib/encodings/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/cp500.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/cp500.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/cp65001.pyc:	\
		third_party/python/Lib/encodings/cp65001.py	\
		o/$(MODE)/third_party/python/Lib/encodings/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/cp65001.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/cp65001.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/cp720.pyc:	\
		third_party/python/Lib/encodings/cp720.py	\
		o/$(MODE)/third_party/python/Lib/encodings/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/cp720.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/cp720.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/cp737.pyc:	\
		third_party/python/Lib/encodings/cp737.py	\
		o/$(MODE)/third_party/python/Lib/encodings/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/cp737.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/cp737.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/cp775.pyc:	\
		third_party/python/Lib/encodings/cp775.py	\
		o/$(MODE)/third_party/python/Lib/encodings/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/cp775.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/cp775.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/cp850.pyc:	\
		third_party/python/Lib/encodings/cp850.py	\
		o/$(MODE)/third_party/python/Lib/encodings/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/cp850.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/cp850.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/cp852.pyc:	\
		third_party/python/Lib/encodings/cp852.py	\
		o/$(MODE)/third_party/python/Lib/encodings/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/cp852.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/cp852.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/cp855.pyc:	\
		third_party/python/Lib/encodings/cp855.py	\
		o/$(MODE)/third_party/python/Lib/encodings/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/cp855.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/cp855.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/cp856.pyc:	\
		third_party/python/Lib/encodings/cp856.py	\
		o/$(MODE)/third_party/python/Lib/encodings/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/cp856.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/cp856.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/cp857.pyc:	\
		third_party/python/Lib/encodings/cp857.py	\
		o/$(MODE)/third_party/python/Lib/encodings/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/cp857.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/cp857.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/cp858.pyc:	\
		third_party/python/Lib/encodings/cp858.py	\
		o/$(MODE)/third_party/python/Lib/encodings/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/cp858.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/cp858.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/cp860.pyc:	\
		third_party/python/Lib/encodings/cp860.py	\
		o/$(MODE)/third_party/python/Lib/encodings/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/cp860.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/cp860.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/cp861.pyc:	\
		third_party/python/Lib/encodings/cp861.py	\
		o/$(MODE)/third_party/python/Lib/encodings/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/cp861.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/cp861.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/cp862.pyc:	\
		third_party/python/Lib/encodings/cp862.py	\
		o/$(MODE)/third_party/python/Lib/encodings/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/cp862.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/cp862.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/cp863.pyc:	\
		third_party/python/Lib/encodings/cp863.py	\
		o/$(MODE)/third_party/python/Lib/encodings/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/cp863.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/cp863.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/cp864.pyc:	\
		third_party/python/Lib/encodings/cp864.py	\
		o/$(MODE)/third_party/python/Lib/encodings/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/cp864.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/cp864.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/cp865.pyc:	\
		third_party/python/Lib/encodings/cp865.py	\
		o/$(MODE)/third_party/python/Lib/encodings/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/cp865.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/cp865.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/cp866.pyc:	\
		third_party/python/Lib/encodings/cp866.py	\
		o/$(MODE)/third_party/python/Lib/encodings/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/cp866.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/cp866.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/cp869.pyc:	\
		third_party/python/Lib/encodings/cp869.py	\
		o/$(MODE)/third_party/python/Lib/encodings/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/cp869.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/cp869.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/cp874.pyc:	\
		third_party/python/Lib/encodings/cp874.py	\
		o/$(MODE)/third_party/python/Lib/encodings/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/cp874.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/cp874.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/cp875.pyc:	\
		third_party/python/Lib/encodings/cp875.py	\
		o/$(MODE)/third_party/python/Lib/encodings/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/cp875.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/cp875.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/cp932.pyc:	\
		third_party/python/Lib/encodings/cp932.py	\
		o/$(MODE)/third_party/python/Lib/encodings/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/cp932.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/cp932.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/cp949.pyc:	\
		third_party/python/Lib/encodings/cp949.py	\
		o/$(MODE)/third_party/python/Lib/encodings/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/cp949.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/cp949.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/cp950.pyc:	\
		third_party/python/Lib/encodings/cp950.py	\
		o/$(MODE)/third_party/python/Lib/encodings/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/cp950.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/cp950.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/euc_jis_2004.pyc:	\
		third_party/python/Lib/encodings/euc_jis_2004.py	\
		o/$(MODE)/third_party/python/Lib/encodings/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/euc_jis_2004.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/euc_jis_2004.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/euc_jisx0213.pyc:	\
		third_party/python/Lib/encodings/euc_jisx0213.py	\
		o/$(MODE)/third_party/python/Lib/encodings/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/euc_jisx0213.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/euc_jisx0213.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/euc_jp.pyc:	\
		third_party/python/Lib/encodings/euc_jp.py	\
		o/$(MODE)/third_party/python/Lib/encodings/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/euc_jp.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/euc_jp.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/euc_kr.pyc:	\
		third_party/python/Lib/encodings/euc_kr.py	\
		o/$(MODE)/third_party/python/Lib/encodings/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/euc_kr.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/euc_kr.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/gb18030.pyc:	\
		third_party/python/Lib/encodings/gb18030.py	\
		o/$(MODE)/third_party/python/Lib/encodings/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/gb18030.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/gb18030.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/gb2312.pyc:	\
		third_party/python/Lib/encodings/gb2312.py	\
		o/$(MODE)/third_party/python/Lib/encodings/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/gb2312.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/gb2312.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/gbk.pyc:	\
		third_party/python/Lib/encodings/gbk.py	\
		o/$(MODE)/third_party/python/Lib/encodings/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/gbk.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/gbk.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/hex_codec.pyc:	\
		third_party/python/Lib/encodings/hex_codec.py	\
		o/$(MODE)/third_party/python/Lib/encodings/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/hex_codec.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/hex_codec.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/hp_roman8.pyc:	\
		third_party/python/Lib/encodings/hp_roman8.py	\
		o/$(MODE)/third_party/python/Lib/encodings/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/hp_roman8.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/hp_roman8.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/hz.pyc:	\
		third_party/python/Lib/encodings/hz.py	\
		o/$(MODE)/third_party/python/Lib/encodings/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/hz.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/hz.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/idna.pyc:	\
		third_party/python/Lib/encodings/idna.py	\
		o/$(MODE)/third_party/python/Lib/encodings/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/idna.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/idna.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/iso2022_jp.pyc:	\
		third_party/python/Lib/encodings/iso2022_jp.py	\
		o/$(MODE)/third_party/python/Lib/encodings/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/iso2022_jp.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/iso2022_jp.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/iso2022_jp_1.pyc:	\
		third_party/python/Lib/encodings/iso2022_jp_1.py	\
		o/$(MODE)/third_party/python/Lib/encodings/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/iso2022_jp_1.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/iso2022_jp_1.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/iso2022_jp_2.pyc:	\
		third_party/python/Lib/encodings/iso2022_jp_2.py	\
		o/$(MODE)/third_party/python/Lib/encodings/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/iso2022_jp_2.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/iso2022_jp_2.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/iso2022_jp_2004.pyc:	\
		third_party/python/Lib/encodings/iso2022_jp_2004.py	\
		o/$(MODE)/third_party/python/Lib/encodings/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/iso2022_jp_2004.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/iso2022_jp_2004.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/iso2022_jp_3.pyc:	\
		third_party/python/Lib/encodings/iso2022_jp_3.py	\
		o/$(MODE)/third_party/python/Lib/encodings/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/iso2022_jp_3.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/iso2022_jp_3.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/iso2022_jp_ext.pyc:	\
		third_party/python/Lib/encodings/iso2022_jp_ext.py	\
		o/$(MODE)/third_party/python/Lib/encodings/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/iso2022_jp_ext.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/iso2022_jp_ext.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/iso2022_kr.pyc:	\
		third_party/python/Lib/encodings/iso2022_kr.py	\
		o/$(MODE)/third_party/python/Lib/encodings/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/iso2022_kr.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/iso2022_kr.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/iso8859_1.pyc:	\
		third_party/python/Lib/encodings/iso8859_1.py	\
		o/$(MODE)/third_party/python/Lib/encodings/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/iso8859_1.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/iso8859_1.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/iso8859_10.pyc:	\
		third_party/python/Lib/encodings/iso8859_10.py	\
		o/$(MODE)/third_party/python/Lib/encodings/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/iso8859_10.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/iso8859_10.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/iso8859_11.pyc:	\
		third_party/python/Lib/encodings/iso8859_11.py	\
		o/$(MODE)/third_party/python/Lib/encodings/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/iso8859_11.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/iso8859_11.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/iso8859_13.pyc:	\
		third_party/python/Lib/encodings/iso8859_13.py	\
		o/$(MODE)/third_party/python/Lib/encodings/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/iso8859_13.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/iso8859_13.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/iso8859_14.pyc:	\
		third_party/python/Lib/encodings/iso8859_14.py	\
		o/$(MODE)/third_party/python/Lib/encodings/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/iso8859_14.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/iso8859_14.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/iso8859_15.pyc:	\
		third_party/python/Lib/encodings/iso8859_15.py	\
		o/$(MODE)/third_party/python/Lib/encodings/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/iso8859_15.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/iso8859_15.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/iso8859_16.pyc:	\
		third_party/python/Lib/encodings/iso8859_16.py	\
		o/$(MODE)/third_party/python/Lib/encodings/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/iso8859_16.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/iso8859_16.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/iso8859_2.pyc:	\
		third_party/python/Lib/encodings/iso8859_2.py	\
		o/$(MODE)/third_party/python/Lib/encodings/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/iso8859_2.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/iso8859_2.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/iso8859_3.pyc:	\
		third_party/python/Lib/encodings/iso8859_3.py	\
		o/$(MODE)/third_party/python/Lib/encodings/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/iso8859_3.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/iso8859_3.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/iso8859_4.pyc:	\
		third_party/python/Lib/encodings/iso8859_4.py	\
		o/$(MODE)/third_party/python/Lib/encodings/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/iso8859_4.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/iso8859_4.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/iso8859_5.pyc:	\
		third_party/python/Lib/encodings/iso8859_5.py	\
		o/$(MODE)/third_party/python/Lib/encodings/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/iso8859_5.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/iso8859_5.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/iso8859_6.pyc:	\
		third_party/python/Lib/encodings/iso8859_6.py	\
		o/$(MODE)/third_party/python/Lib/encodings/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/iso8859_6.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/iso8859_6.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/iso8859_7.pyc:	\
		third_party/python/Lib/encodings/iso8859_7.py	\
		o/$(MODE)/third_party/python/Lib/encodings/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/iso8859_7.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/iso8859_7.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/iso8859_8.pyc:	\
		third_party/python/Lib/encodings/iso8859_8.py	\
		o/$(MODE)/third_party/python/Lib/encodings/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/iso8859_8.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/iso8859_8.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/iso8859_9.pyc:	\
		third_party/python/Lib/encodings/iso8859_9.py	\
		o/$(MODE)/third_party/python/Lib/encodings/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/iso8859_9.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/iso8859_9.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/johab.pyc:	\
		third_party/python/Lib/encodings/johab.py	\
		o/$(MODE)/third_party/python/Lib/encodings/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/johab.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/johab.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/koi8_r.pyc:	\
		third_party/python/Lib/encodings/koi8_r.py	\
		o/$(MODE)/third_party/python/Lib/encodings/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/koi8_r.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/koi8_r.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/koi8_t.pyc:	\
		third_party/python/Lib/encodings/koi8_t.py	\
		o/$(MODE)/third_party/python/Lib/encodings/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/koi8_t.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/koi8_t.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/koi8_u.pyc:	\
		third_party/python/Lib/encodings/koi8_u.py	\
		o/$(MODE)/third_party/python/Lib/encodings/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/koi8_u.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/koi8_u.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/kz1048.pyc:	\
		third_party/python/Lib/encodings/kz1048.py	\
		o/$(MODE)/third_party/python/Lib/encodings/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/kz1048.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/kz1048.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/latin_1.pyc:	\
		third_party/python/Lib/encodings/latin_1.py	\
		o/$(MODE)/third_party/python/Lib/encodings/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/latin_1.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/latin_1.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/mac_arabic.pyc:	\
		third_party/python/Lib/encodings/mac_arabic.py	\
		o/$(MODE)/third_party/python/Lib/encodings/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/mac_arabic.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/mac_arabic.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/mac_centeuro.pyc:	\
		third_party/python/Lib/encodings/mac_centeuro.py	\
		o/$(MODE)/third_party/python/Lib/encodings/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/mac_centeuro.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/mac_centeuro.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/mac_croatian.pyc:	\
		third_party/python/Lib/encodings/mac_croatian.py	\
		o/$(MODE)/third_party/python/Lib/encodings/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/mac_croatian.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/mac_croatian.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/mac_cyrillic.pyc:	\
		third_party/python/Lib/encodings/mac_cyrillic.py	\
		o/$(MODE)/third_party/python/Lib/encodings/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/mac_cyrillic.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/mac_cyrillic.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/mac_farsi.pyc:	\
		third_party/python/Lib/encodings/mac_farsi.py	\
		o/$(MODE)/third_party/python/Lib/encodings/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/mac_farsi.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/mac_farsi.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/mac_greek.pyc:	\
		third_party/python/Lib/encodings/mac_greek.py	\
		o/$(MODE)/third_party/python/Lib/encodings/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/mac_greek.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/mac_greek.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/mac_iceland.pyc:	\
		third_party/python/Lib/encodings/mac_iceland.py	\
		o/$(MODE)/third_party/python/Lib/encodings/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/mac_iceland.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/mac_iceland.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/mac_latin2.pyc:	\
		third_party/python/Lib/encodings/mac_latin2.py	\
		o/$(MODE)/third_party/python/Lib/encodings/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/mac_latin2.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/mac_latin2.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/mac_roman.pyc:	\
		third_party/python/Lib/encodings/mac_roman.py	\
		o/$(MODE)/third_party/python/Lib/encodings/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/mac_roman.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/mac_roman.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/mac_romanian.pyc:	\
		third_party/python/Lib/encodings/mac_romanian.py	\
		o/$(MODE)/third_party/python/Lib/encodings/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/mac_romanian.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/mac_romanian.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/mac_turkish.pyc:	\
		third_party/python/Lib/encodings/mac_turkish.py	\
		o/$(MODE)/third_party/python/Lib/encodings/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/mac_turkish.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/mac_turkish.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/mbcs.pyc:	\
		third_party/python/Lib/encodings/mbcs.py	\
		o/$(MODE)/third_party/python/Lib/encodings/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/mbcs.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/mbcs.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/oem.pyc:	\
		third_party/python/Lib/encodings/oem.py	\
		o/$(MODE)/third_party/python/Lib/encodings/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/oem.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/oem.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/palmos.pyc:	\
		third_party/python/Lib/encodings/palmos.py	\
		o/$(MODE)/third_party/python/Lib/encodings/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/palmos.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/palmos.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/ptcp154.pyc:	\
		third_party/python/Lib/encodings/ptcp154.py	\
		o/$(MODE)/third_party/python/Lib/encodings/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/ptcp154.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/ptcp154.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/punycode.pyc:	\
		third_party/python/Lib/encodings/punycode.py	\
		o/$(MODE)/third_party/python/Lib/encodings/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/punycode.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/punycode.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/quopri_codec.pyc:	\
		third_party/python/Lib/encodings/quopri_codec.py	\
		o/$(MODE)/third_party/python/Lib/encodings/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/quopri_codec.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/quopri_codec.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/raw_unicode_escape.pyc:	\
		third_party/python/Lib/encodings/raw_unicode_escape.py	\
		o/$(MODE)/third_party/python/Lib/encodings/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/raw_unicode_escape.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/raw_unicode_escape.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/rot_13.pyc:	\
		third_party/python/Lib/encodings/rot_13.py	\
		o/$(MODE)/third_party/python/Lib/encodings/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/rot_13.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/rot_13.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/shift_jis.pyc:	\
		third_party/python/Lib/encodings/shift_jis.py	\
		o/$(MODE)/third_party/python/Lib/encodings/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/shift_jis.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/shift_jis.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/shift_jis_2004.pyc:	\
		third_party/python/Lib/encodings/shift_jis_2004.py	\
		o/$(MODE)/third_party/python/Lib/encodings/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/shift_jis_2004.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/shift_jis_2004.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/shift_jisx0213.pyc:	\
		third_party/python/Lib/encodings/shift_jisx0213.py	\
		o/$(MODE)/third_party/python/Lib/encodings/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/shift_jisx0213.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/shift_jisx0213.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/tis_620.pyc:	\
		third_party/python/Lib/encodings/tis_620.py	\
		o/$(MODE)/third_party/python/Lib/encodings/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/tis_620.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/tis_620.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/undefined.pyc:	\
		third_party/python/Lib/encodings/undefined.py	\
		o/$(MODE)/third_party/python/Lib/encodings/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/undefined.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/undefined.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/unicode_escape.pyc:	\
		third_party/python/Lib/encodings/unicode_escape.py	\
		o/$(MODE)/third_party/python/Lib/encodings/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/unicode_escape.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/unicode_escape.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/unicode_internal.pyc:	\
		third_party/python/Lib/encodings/unicode_internal.py	\
		o/$(MODE)/third_party/python/Lib/encodings/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/unicode_internal.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/unicode_internal.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/utf_16.pyc:	\
		third_party/python/Lib/encodings/utf_16.py	\
		o/$(MODE)/third_party/python/Lib/encodings/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/utf_16.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/utf_16.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/utf_16_be.pyc:	\
		third_party/python/Lib/encodings/utf_16_be.py	\
		o/$(MODE)/third_party/python/Lib/encodings/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/utf_16_be.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/utf_16_be.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/utf_16_le.pyc:	\
		third_party/python/Lib/encodings/utf_16_le.py	\
		o/$(MODE)/third_party/python/Lib/encodings/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/utf_16_le.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/utf_16_le.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/utf_32.pyc:	\
		third_party/python/Lib/encodings/utf_32.py	\
		o/$(MODE)/third_party/python/Lib/encodings/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/utf_32.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/utf_32.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/utf_32_be.pyc:	\
		third_party/python/Lib/encodings/utf_32_be.py	\
		o/$(MODE)/third_party/python/Lib/encodings/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/utf_32_be.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/utf_32_be.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/utf_32_le.pyc:	\
		third_party/python/Lib/encodings/utf_32_le.py	\
		o/$(MODE)/third_party/python/Lib/encodings/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/utf_32_le.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/utf_32_le.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/utf_7.pyc:	\
		third_party/python/Lib/encodings/utf_7.py	\
		o/$(MODE)/third_party/python/Lib/encodings/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/utf_7.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/utf_7.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/utf_8.pyc:	\
		third_party/python/Lib/encodings/utf_8.py	\
		o/$(MODE)/third_party/python/Lib/encodings/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/utf_8.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/utf_8.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/utf_8_sig.pyc:	\
		third_party/python/Lib/encodings/utf_8_sig.py	\
		o/$(MODE)/third_party/python/Lib/encodings/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/utf_8_sig.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/utf_8_sig.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/uu_codec.pyc:	\
		third_party/python/Lib/encodings/uu_codec.py	\
		o/$(MODE)/third_party/python/Lib/encodings/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/uu_codec.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/uu_codec.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/encodings/zlib_codec.pyc:	\
		third_party/python/Lib/encodings/zlib_codec.py	\
		o/$(MODE)/third_party/python/Lib/encodings/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/encodings/zlib_codec.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/encodings/zlib_codec.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/ensurepip/:	\
		o/$(MODE)/third_party/python/Lib/ensurepip/
	@mkdir -p $@

o/$(MODE)/third_party/python/Lib/ensurepip/.zip.o:	\
		o/$(MODE)/third_party/python/Lib/ensurepip/
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/ensurepip/__init__.pyc:	\
		third_party/python/Lib/ensurepip/__init__.py	\
		o/$(MODE)/third_party/python/Lib/ensurepip/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/ensurepip/__init__.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/ensurepip/__init__.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/ensurepip/__main__.pyc:	\
		third_party/python/Lib/ensurepip/__main__.py	\
		o/$(MODE)/third_party/python/Lib/ensurepip/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/ensurepip/__main__.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/ensurepip/__main__.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/ensurepip/_uninstall.pyc:	\
		third_party/python/Lib/ensurepip/_uninstall.py	\
		o/$(MODE)/third_party/python/Lib/ensurepip/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/ensurepip/_uninstall.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/ensurepip/_uninstall.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/enum.pyc:	\
		third_party/python/Lib/enum.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/enum.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/enum.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/filecmp.pyc:	\
		third_party/python/Lib/filecmp.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/filecmp.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/filecmp.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/fileinput.pyc:	\
		third_party/python/Lib/fileinput.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/fileinput.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/fileinput.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/fnmatch.pyc:	\
		third_party/python/Lib/fnmatch.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/fnmatch.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/fnmatch.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/formatter.pyc:	\
		third_party/python/Lib/formatter.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/formatter.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/formatter.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/fractions.pyc:	\
		third_party/python/Lib/fractions.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/fractions.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/fractions.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/ftplib.pyc:	\
		third_party/python/Lib/ftplib.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/ftplib.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/ftplib.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/functools.pyc:	\
		third_party/python/Lib/functools.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/functools.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/functools.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/genericpath.pyc:	\
		third_party/python/Lib/genericpath.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/genericpath.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/genericpath.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/getopt.pyc:	\
		third_party/python/Lib/getopt.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/getopt.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/getopt.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/getpass.pyc:	\
		third_party/python/Lib/getpass.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/getpass.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/getpass.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/gettext.pyc:	\
		third_party/python/Lib/gettext.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/gettext.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/gettext.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/glob.pyc:	\
		third_party/python/Lib/glob.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/glob.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/glob.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/gzip.pyc:	\
		third_party/python/Lib/gzip.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/gzip.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/gzip.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/hashlib.pyc:	\
		third_party/python/Lib/hashlib.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/hashlib.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/hashlib.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/heapq.pyc:	\
		third_party/python/Lib/heapq.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/heapq.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/heapq.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/hmac.pyc:	\
		third_party/python/Lib/hmac.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/hmac.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/hmac.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/html/:	\
		o/$(MODE)/third_party/python/Lib/html/
	@mkdir -p $@

o/$(MODE)/third_party/python/Lib/html/.zip.o:	\
		o/$(MODE)/third_party/python/Lib/html/
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/html/__init__.pyc:	\
		third_party/python/Lib/html/__init__.py	\
		o/$(MODE)/third_party/python/Lib/html/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/html/__init__.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/html/__init__.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/html/entities.pyc:	\
		third_party/python/Lib/html/entities.py	\
		o/$(MODE)/third_party/python/Lib/html/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/html/entities.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/html/entities.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/html/parser.pyc:	\
		third_party/python/Lib/html/parser.py	\
		o/$(MODE)/third_party/python/Lib/html/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/html/parser.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/html/parser.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/http/:	\
		o/$(MODE)/third_party/python/Lib/http/
	@mkdir -p $@

o/$(MODE)/third_party/python/Lib/http/.zip.o:	\
		o/$(MODE)/third_party/python/Lib/http/
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/http/__init__.pyc:	\
		third_party/python/Lib/http/__init__.py	\
		o/$(MODE)/third_party/python/Lib/http/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/http/__init__.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/http/__init__.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/http/client.pyc:	\
		third_party/python/Lib/http/client.py	\
		o/$(MODE)/third_party/python/Lib/http/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/http/client.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/http/client.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/http/cookiejar.pyc:	\
		third_party/python/Lib/http/cookiejar.py	\
		o/$(MODE)/third_party/python/Lib/http/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/http/cookiejar.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/http/cookiejar.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/http/cookies.pyc:	\
		third_party/python/Lib/http/cookies.py	\
		o/$(MODE)/third_party/python/Lib/http/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/http/cookies.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/http/cookies.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/http/server.pyc:	\
		third_party/python/Lib/http/server.py	\
		o/$(MODE)/third_party/python/Lib/http/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/http/server.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/http/server.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/imaplib.pyc:	\
		third_party/python/Lib/imaplib.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/imaplib.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/imaplib.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/imghdr.pyc:	\
		third_party/python/Lib/imghdr.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/imghdr.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/imghdr.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/imp.pyc:	\
		third_party/python/Lib/imp.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/imp.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/imp.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/importlib/:	\
		o/$(MODE)/third_party/python/Lib/importlib/
	@mkdir -p $@

o/$(MODE)/third_party/python/Lib/importlib/.zip.o:	\
		o/$(MODE)/third_party/python/Lib/importlib/
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/importlib/__init__.pyc:	\
		third_party/python/Lib/importlib/__init__.py	\
		o/$(MODE)/third_party/python/Lib/importlib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/importlib/__init__.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/importlib/__init__.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/importlib/_bootstrap.pyc:	\
		third_party/python/Lib/importlib/_bootstrap.py	\
		o/$(MODE)/third_party/python/Lib/importlib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/importlib/_bootstrap.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/importlib/_bootstrap.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/importlib/_bootstrap_external.pyc:	\
		third_party/python/Lib/importlib/_bootstrap_external.py	\
		o/$(MODE)/third_party/python/Lib/importlib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/importlib/_bootstrap_external.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/importlib/_bootstrap_external.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/importlib/abc.pyc:	\
		third_party/python/Lib/importlib/abc.py	\
		o/$(MODE)/third_party/python/Lib/importlib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/importlib/abc.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/importlib/abc.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/importlib/machinery.pyc:	\
		third_party/python/Lib/importlib/machinery.py	\
		o/$(MODE)/third_party/python/Lib/importlib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/importlib/machinery.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/importlib/machinery.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/importlib/util.pyc:	\
		third_party/python/Lib/importlib/util.py	\
		o/$(MODE)/third_party/python/Lib/importlib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/importlib/util.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/importlib/util.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/inspect.pyc:	\
		third_party/python/Lib/inspect.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/inspect.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/inspect.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/io.pyc:	\
		third_party/python/Lib/io.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/io.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/io.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/ipaddress.pyc:	\
		third_party/python/Lib/ipaddress.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/ipaddress.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/ipaddress.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/json/:	\
		o/$(MODE)/third_party/python/Lib/json/
	@mkdir -p $@

o/$(MODE)/third_party/python/Lib/json/.zip.o:	\
		o/$(MODE)/third_party/python/Lib/json/
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/json/__init__.pyc:	\
		third_party/python/Lib/json/__init__.py	\
		o/$(MODE)/third_party/python/Lib/json/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/json/__init__.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/json/__init__.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/json/decoder.pyc:	\
		third_party/python/Lib/json/decoder.py	\
		o/$(MODE)/third_party/python/Lib/json/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/json/decoder.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/json/decoder.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/json/encoder.pyc:	\
		third_party/python/Lib/json/encoder.py	\
		o/$(MODE)/third_party/python/Lib/json/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/json/encoder.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/json/encoder.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/json/scanner.pyc:	\
		third_party/python/Lib/json/scanner.py	\
		o/$(MODE)/third_party/python/Lib/json/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/json/scanner.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/json/scanner.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/json/tool.pyc:	\
		third_party/python/Lib/json/tool.py	\
		o/$(MODE)/third_party/python/Lib/json/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/json/tool.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/json/tool.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/keyword.pyc:	\
		third_party/python/Lib/keyword.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/keyword.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/keyword.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/linecache.pyc:	\
		third_party/python/Lib/linecache.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/linecache.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/linecache.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/locale.pyc:	\
		third_party/python/Lib/locale.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/locale.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/locale.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/logging/:	\
		o/$(MODE)/third_party/python/Lib/logging/
	@mkdir -p $@

o/$(MODE)/third_party/python/Lib/logging/.zip.o:	\
		o/$(MODE)/third_party/python/Lib/logging/
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/logging/__init__.pyc:	\
		third_party/python/Lib/logging/__init__.py	\
		o/$(MODE)/third_party/python/Lib/logging/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/logging/__init__.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/logging/__init__.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/logging/config.pyc:	\
		third_party/python/Lib/logging/config.py	\
		o/$(MODE)/third_party/python/Lib/logging/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/logging/config.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/logging/config.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/logging/handlers.pyc:	\
		third_party/python/Lib/logging/handlers.py	\
		o/$(MODE)/third_party/python/Lib/logging/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/logging/handlers.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/logging/handlers.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/lzma.pyc:	\
		third_party/python/Lib/lzma.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/lzma.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/lzma.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/macpath.pyc:	\
		third_party/python/Lib/macpath.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/macpath.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/macpath.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/macurl2path.pyc:	\
		third_party/python/Lib/macurl2path.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/macurl2path.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/macurl2path.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/mailbox.pyc:	\
		third_party/python/Lib/mailbox.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/mailbox.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/mailbox.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/mailcap.pyc:	\
		third_party/python/Lib/mailcap.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/mailcap.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/mailcap.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/mimetypes.pyc:	\
		third_party/python/Lib/mimetypes.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/mimetypes.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/mimetypes.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/modulefinder.pyc:	\
		third_party/python/Lib/modulefinder.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/modulefinder.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/modulefinder.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/msilib/:	\
		o/$(MODE)/third_party/python/Lib/msilib/
	@mkdir -p $@

o/$(MODE)/third_party/python/Lib/msilib/.zip.o:	\
		o/$(MODE)/third_party/python/Lib/msilib/
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/msilib/__init__.pyc:	\
		third_party/python/Lib/msilib/__init__.py	\
		o/$(MODE)/third_party/python/Lib/msilib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/msilib/__init__.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/msilib/__init__.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/msilib/schema.pyc:	\
		third_party/python/Lib/msilib/schema.py	\
		o/$(MODE)/third_party/python/Lib/msilib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/msilib/schema.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/msilib/schema.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/msilib/sequence.pyc:	\
		third_party/python/Lib/msilib/sequence.py	\
		o/$(MODE)/third_party/python/Lib/msilib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/msilib/sequence.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/msilib/sequence.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/msilib/text.pyc:	\
		third_party/python/Lib/msilib/text.py	\
		o/$(MODE)/third_party/python/Lib/msilib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/msilib/text.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/msilib/text.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/multiprocessing/:	\
		o/$(MODE)/third_party/python/Lib/multiprocessing/
	@mkdir -p $@

o/$(MODE)/third_party/python/Lib/multiprocessing/.zip.o:	\
		o/$(MODE)/third_party/python/Lib/multiprocessing/
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/multiprocessing/__init__.pyc:	\
		third_party/python/Lib/multiprocessing/__init__.py	\
		o/$(MODE)/third_party/python/Lib/multiprocessing/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/multiprocessing/__init__.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/multiprocessing/__init__.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/multiprocessing/connection.pyc:	\
		third_party/python/Lib/multiprocessing/connection.py	\
		o/$(MODE)/third_party/python/Lib/multiprocessing/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/multiprocessing/connection.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/multiprocessing/connection.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/multiprocessing/context.pyc:	\
		third_party/python/Lib/multiprocessing/context.py	\
		o/$(MODE)/third_party/python/Lib/multiprocessing/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/multiprocessing/context.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/multiprocessing/context.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/multiprocessing/dummy/:	\
		o/$(MODE)/third_party/python/Lib/multiprocessing/dummy/
	@mkdir -p $@

o/$(MODE)/third_party/python/Lib/multiprocessing/dummy/.zip.o:	\
		o/$(MODE)/third_party/python/Lib/multiprocessing/dummy/
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/multiprocessing/dummy/__init__.pyc:	\
		third_party/python/Lib/multiprocessing/dummy/__init__.py	\
		o/$(MODE)/third_party/python/Lib/multiprocessing/dummy/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/multiprocessing/dummy/__init__.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/multiprocessing/dummy/__init__.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/multiprocessing/dummy/connection.pyc:	\
		third_party/python/Lib/multiprocessing/dummy/connection.py	\
		o/$(MODE)/third_party/python/Lib/multiprocessing/dummy/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/multiprocessing/dummy/connection.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/multiprocessing/dummy/connection.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/multiprocessing/forkserver.pyc:	\
		third_party/python/Lib/multiprocessing/forkserver.py	\
		o/$(MODE)/third_party/python/Lib/multiprocessing/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/multiprocessing/forkserver.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/multiprocessing/forkserver.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/multiprocessing/heap.pyc:	\
		third_party/python/Lib/multiprocessing/heap.py	\
		o/$(MODE)/third_party/python/Lib/multiprocessing/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/multiprocessing/heap.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/multiprocessing/heap.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/multiprocessing/managers.pyc:	\
		third_party/python/Lib/multiprocessing/managers.py	\
		o/$(MODE)/third_party/python/Lib/multiprocessing/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/multiprocessing/managers.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/multiprocessing/managers.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/multiprocessing/pool.pyc:	\
		third_party/python/Lib/multiprocessing/pool.py	\
		o/$(MODE)/third_party/python/Lib/multiprocessing/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/multiprocessing/pool.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/multiprocessing/pool.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/multiprocessing/popen_fork.pyc:	\
		third_party/python/Lib/multiprocessing/popen_fork.py	\
		o/$(MODE)/third_party/python/Lib/multiprocessing/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/multiprocessing/popen_fork.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/multiprocessing/popen_fork.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/multiprocessing/popen_forkserver.pyc:	\
		third_party/python/Lib/multiprocessing/popen_forkserver.py	\
		o/$(MODE)/third_party/python/Lib/multiprocessing/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/multiprocessing/popen_forkserver.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/multiprocessing/popen_forkserver.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/multiprocessing/popen_spawn_posix.pyc:	\
		third_party/python/Lib/multiprocessing/popen_spawn_posix.py	\
		o/$(MODE)/third_party/python/Lib/multiprocessing/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/multiprocessing/popen_spawn_posix.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/multiprocessing/popen_spawn_posix.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/multiprocessing/popen_spawn_win32.pyc:	\
		third_party/python/Lib/multiprocessing/popen_spawn_win32.py	\
		o/$(MODE)/third_party/python/Lib/multiprocessing/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/multiprocessing/popen_spawn_win32.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/multiprocessing/popen_spawn_win32.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/multiprocessing/process.pyc:	\
		third_party/python/Lib/multiprocessing/process.py	\
		o/$(MODE)/third_party/python/Lib/multiprocessing/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/multiprocessing/process.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/multiprocessing/process.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/multiprocessing/queues.pyc:	\
		third_party/python/Lib/multiprocessing/queues.py	\
		o/$(MODE)/third_party/python/Lib/multiprocessing/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/multiprocessing/queues.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/multiprocessing/queues.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/multiprocessing/reduction.pyc:	\
		third_party/python/Lib/multiprocessing/reduction.py	\
		o/$(MODE)/third_party/python/Lib/multiprocessing/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/multiprocessing/reduction.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/multiprocessing/reduction.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/multiprocessing/resource_sharer.pyc:	\
		third_party/python/Lib/multiprocessing/resource_sharer.py	\
		o/$(MODE)/third_party/python/Lib/multiprocessing/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/multiprocessing/resource_sharer.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/multiprocessing/resource_sharer.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/multiprocessing/semaphore_tracker.pyc:	\
		third_party/python/Lib/multiprocessing/semaphore_tracker.py	\
		o/$(MODE)/third_party/python/Lib/multiprocessing/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/multiprocessing/semaphore_tracker.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/multiprocessing/semaphore_tracker.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/multiprocessing/sharedctypes.pyc:	\
		third_party/python/Lib/multiprocessing/sharedctypes.py	\
		o/$(MODE)/third_party/python/Lib/multiprocessing/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/multiprocessing/sharedctypes.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/multiprocessing/sharedctypes.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/multiprocessing/spawn.pyc:	\
		third_party/python/Lib/multiprocessing/spawn.py	\
		o/$(MODE)/third_party/python/Lib/multiprocessing/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/multiprocessing/spawn.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/multiprocessing/spawn.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/multiprocessing/synchronize.pyc:	\
		third_party/python/Lib/multiprocessing/synchronize.py	\
		o/$(MODE)/third_party/python/Lib/multiprocessing/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/multiprocessing/synchronize.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/multiprocessing/synchronize.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/multiprocessing/util.pyc:	\
		third_party/python/Lib/multiprocessing/util.py	\
		o/$(MODE)/third_party/python/Lib/multiprocessing/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/multiprocessing/util.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/multiprocessing/util.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/netrc.pyc:	\
		third_party/python/Lib/netrc.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/netrc.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/netrc.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/nntplib.pyc:	\
		third_party/python/Lib/nntplib.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/nntplib.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/nntplib.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/ntpath.pyc:	\
		third_party/python/Lib/ntpath.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/ntpath.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/ntpath.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/nturl2path.pyc:	\
		third_party/python/Lib/nturl2path.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/nturl2path.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/nturl2path.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/numbers.pyc:	\
		third_party/python/Lib/numbers.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/numbers.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/numbers.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/opcode.pyc:	\
		third_party/python/Lib/opcode.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/opcode.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/opcode.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/operator.pyc:	\
		third_party/python/Lib/operator.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/operator.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/operator.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/optparse.pyc:	\
		third_party/python/Lib/optparse.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/optparse.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/optparse.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/os.pyc:	\
		third_party/python/Lib/os.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/os.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/os.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/pathlib.pyc:	\
		third_party/python/Lib/pathlib.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/pathlib.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/pathlib.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/pdb.pyc:	\
		third_party/python/Lib/pdb.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/pdb.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/pdb.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/pickle.pyc:	\
		third_party/python/Lib/pickle.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/pickle.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/pickle.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/pickletools.pyc:	\
		third_party/python/Lib/pickletools.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/pickletools.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/pickletools.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/pipes.pyc:	\
		third_party/python/Lib/pipes.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/pipes.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/pipes.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/pkgutil.pyc:	\
		third_party/python/Lib/pkgutil.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/pkgutil.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/pkgutil.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/platform.pyc:	\
		third_party/python/Lib/platform.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/platform.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/platform.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/plistlib.pyc:	\
		third_party/python/Lib/plistlib.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/plistlib.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/plistlib.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/poplib.pyc:	\
		third_party/python/Lib/poplib.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/poplib.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/poplib.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/posixpath.pyc:	\
		third_party/python/Lib/posixpath.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/posixpath.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/posixpath.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/pprint.pyc:	\
		third_party/python/Lib/pprint.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/pprint.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/pprint.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/profile.pyc:	\
		third_party/python/Lib/profile.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/profile.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/profile.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/pstats.pyc:	\
		third_party/python/Lib/pstats.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/pstats.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/pstats.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/pty.pyc:	\
		third_party/python/Lib/pty.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/pty.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/pty.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/py_compile.pyc:	\
		third_party/python/Lib/py_compile.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/py_compile.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/py_compile.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/pyclbr.pyc:	\
		third_party/python/Lib/pyclbr.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/pyclbr.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/pyclbr.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/pydoc.pyc:	\
		third_party/python/Lib/pydoc.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/pydoc.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/pydoc.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/queue.pyc:	\
		third_party/python/Lib/queue.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/queue.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/queue.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/quopri.pyc:	\
		third_party/python/Lib/quopri.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/quopri.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/quopri.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/random.pyc:	\
		third_party/python/Lib/random.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/random.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/random.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/re.pyc:	\
		third_party/python/Lib/re.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/re.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/re.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/reprlib.pyc:	\
		third_party/python/Lib/reprlib.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/reprlib.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/reprlib.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/rlcompleter.pyc:	\
		third_party/python/Lib/rlcompleter.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/rlcompleter.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/rlcompleter.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/runpy.pyc:	\
		third_party/python/Lib/runpy.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/runpy.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/runpy.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/sched.pyc:	\
		third_party/python/Lib/sched.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/sched.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/sched.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/secrets.pyc:	\
		third_party/python/Lib/secrets.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/secrets.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/secrets.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/selectors.pyc:	\
		third_party/python/Lib/selectors.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/selectors.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/selectors.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/shelve.pyc:	\
		third_party/python/Lib/shelve.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/shelve.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/shelve.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/shlex.pyc:	\
		third_party/python/Lib/shlex.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/shlex.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/shlex.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/shutil.pyc:	\
		third_party/python/Lib/shutil.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/shutil.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/shutil.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/signal.pyc:	\
		third_party/python/Lib/signal.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/signal.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/signal.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/site.pyc:	\
		third_party/python/Lib/site.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/site.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/site.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/smtpd.pyc:	\
		third_party/python/Lib/smtpd.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/smtpd.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/smtpd.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/smtplib.pyc:	\
		third_party/python/Lib/smtplib.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/smtplib.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/smtplib.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/sndhdr.pyc:	\
		third_party/python/Lib/sndhdr.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/sndhdr.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/sndhdr.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/socket.pyc:	\
		third_party/python/Lib/socket.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/socket.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/socket.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/socketserver.pyc:	\
		third_party/python/Lib/socketserver.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/socketserver.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/socketserver.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/sqlite3/:	\
		o/$(MODE)/third_party/python/Lib/sqlite3/
	@mkdir -p $@

o/$(MODE)/third_party/python/Lib/sqlite3/.zip.o:	\
		o/$(MODE)/third_party/python/Lib/sqlite3/
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/sqlite3/__init__.pyc:	\
		third_party/python/Lib/sqlite3/__init__.py	\
		o/$(MODE)/third_party/python/Lib/sqlite3/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/sqlite3/__init__.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/sqlite3/__init__.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/sqlite3/dbapi2.pyc:	\
		third_party/python/Lib/sqlite3/dbapi2.py	\
		o/$(MODE)/third_party/python/Lib/sqlite3/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/sqlite3/dbapi2.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/sqlite3/dbapi2.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/sqlite3/dump.pyc:	\
		third_party/python/Lib/sqlite3/dump.py	\
		o/$(MODE)/third_party/python/Lib/sqlite3/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/sqlite3/dump.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/sqlite3/dump.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/sre_compile.pyc:	\
		third_party/python/Lib/sre_compile.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/sre_compile.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/sre_compile.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/sre_constants.pyc:	\
		third_party/python/Lib/sre_constants.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/sre_constants.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/sre_constants.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/sre_parse.pyc:	\
		third_party/python/Lib/sre_parse.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/sre_parse.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/sre_parse.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/ssl.pyc:	\
		third_party/python/Lib/ssl.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/ssl.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/ssl.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/stat.pyc:	\
		third_party/python/Lib/stat.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/stat.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/stat.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/statistics.pyc:	\
		third_party/python/Lib/statistics.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/statistics.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/statistics.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/string.pyc:	\
		third_party/python/Lib/string.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/string.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/string.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/stringprep.pyc:	\
		third_party/python/Lib/stringprep.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/stringprep.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/stringprep.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/struct.pyc:	\
		third_party/python/Lib/struct.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/struct.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/struct.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/subprocess.pyc:	\
		third_party/python/Lib/subprocess.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/subprocess.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/subprocess.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/sunau.pyc:	\
		third_party/python/Lib/sunau.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/sunau.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/sunau.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/symbol.pyc:	\
		third_party/python/Lib/symbol.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/symbol.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/symbol.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/symtable.pyc:	\
		third_party/python/Lib/symtable.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/symtable.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/symtable.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/sysconfig.pyc:	\
		third_party/python/Lib/sysconfig.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/sysconfig.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/sysconfig.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/tabnanny.pyc:	\
		third_party/python/Lib/tabnanny.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/tabnanny.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/tabnanny.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/tarfile.pyc:	\
		third_party/python/Lib/tarfile.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/tarfile.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/tarfile.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/telnetlib.pyc:	\
		third_party/python/Lib/telnetlib.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/telnetlib.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/telnetlib.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/tempfile.pyc:	\
		third_party/python/Lib/tempfile.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/tempfile.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/tempfile.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/textwrap.pyc:	\
		third_party/python/Lib/textwrap.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/textwrap.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/textwrap.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/this.pyc:	\
		third_party/python/Lib/this.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/this.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/this.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/threading.pyc:	\
		third_party/python/Lib/threading.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/threading.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/threading.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/timeit.pyc:	\
		third_party/python/Lib/timeit.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/timeit.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/timeit.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/token.pyc:	\
		third_party/python/Lib/token.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/token.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/token.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/tokenize.pyc:	\
		third_party/python/Lib/tokenize.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/tokenize.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/tokenize.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/trace.pyc:	\
		third_party/python/Lib/trace.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/trace.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/trace.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/traceback.pyc:	\
		third_party/python/Lib/traceback.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/traceback.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/traceback.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/tracemalloc.pyc:	\
		third_party/python/Lib/tracemalloc.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/tracemalloc.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/tracemalloc.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/tty.pyc:	\
		third_party/python/Lib/tty.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/tty.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/tty.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/types.pyc:	\
		third_party/python/Lib/types.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/types.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/types.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/typing.pyc:	\
		third_party/python/Lib/typing.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/typing.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/typing.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/unittest/:	\
		o/$(MODE)/third_party/python/Lib/unittest/
	@mkdir -p $@

o/$(MODE)/third_party/python/Lib/unittest/.zip.o:	\
		o/$(MODE)/third_party/python/Lib/unittest/
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/unittest/__init__.pyc:	\
		third_party/python/Lib/unittest/__init__.py	\
		o/$(MODE)/third_party/python/Lib/unittest/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/unittest/__init__.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/unittest/__init__.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/unittest/__main__.pyc:	\
		third_party/python/Lib/unittest/__main__.py	\
		o/$(MODE)/third_party/python/Lib/unittest/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/unittest/__main__.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/unittest/__main__.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/unittest/case.pyc:	\
		third_party/python/Lib/unittest/case.py	\
		o/$(MODE)/third_party/python/Lib/unittest/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/unittest/case.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/unittest/case.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/unittest/loader.pyc:	\
		third_party/python/Lib/unittest/loader.py	\
		o/$(MODE)/third_party/python/Lib/unittest/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/unittest/loader.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/unittest/loader.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/unittest/main.pyc:	\
		third_party/python/Lib/unittest/main.py	\
		o/$(MODE)/third_party/python/Lib/unittest/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/unittest/main.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/unittest/main.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/unittest/mock.pyc:	\
		third_party/python/Lib/unittest/mock.py	\
		o/$(MODE)/third_party/python/Lib/unittest/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/unittest/mock.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/unittest/mock.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/unittest/result.pyc:	\
		third_party/python/Lib/unittest/result.py	\
		o/$(MODE)/third_party/python/Lib/unittest/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/unittest/result.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/unittest/result.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/unittest/runner.pyc:	\
		third_party/python/Lib/unittest/runner.py	\
		o/$(MODE)/third_party/python/Lib/unittest/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/unittest/runner.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/unittest/runner.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/unittest/signals.pyc:	\
		third_party/python/Lib/unittest/signals.py	\
		o/$(MODE)/third_party/python/Lib/unittest/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/unittest/signals.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/unittest/signals.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/unittest/suite.pyc:	\
		third_party/python/Lib/unittest/suite.py	\
		o/$(MODE)/third_party/python/Lib/unittest/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/unittest/suite.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/unittest/suite.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/unittest/util.pyc:	\
		third_party/python/Lib/unittest/util.py	\
		o/$(MODE)/third_party/python/Lib/unittest/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/unittest/util.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/unittest/util.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/urllib/:	\
		o/$(MODE)/third_party/python/Lib/urllib/
	@mkdir -p $@

o/$(MODE)/third_party/python/Lib/urllib/.zip.o:	\
		o/$(MODE)/third_party/python/Lib/urllib/
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/urllib/__init__.pyc:	\
		third_party/python/Lib/urllib/__init__.py	\
		o/$(MODE)/third_party/python/Lib/urllib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/urllib/__init__.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/urllib/__init__.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/urllib/error.pyc:	\
		third_party/python/Lib/urllib/error.py	\
		o/$(MODE)/third_party/python/Lib/urllib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/urllib/error.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/urllib/error.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/urllib/parse.pyc:	\
		third_party/python/Lib/urllib/parse.py	\
		o/$(MODE)/third_party/python/Lib/urllib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/urllib/parse.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/urllib/parse.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/urllib/request.pyc:	\
		third_party/python/Lib/urllib/request.py	\
		o/$(MODE)/third_party/python/Lib/urllib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/urllib/request.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/urllib/request.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/urllib/response.pyc:	\
		third_party/python/Lib/urllib/response.py	\
		o/$(MODE)/third_party/python/Lib/urllib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/urllib/response.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/urllib/response.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/urllib/robotparser.pyc:	\
		third_party/python/Lib/urllib/robotparser.py	\
		o/$(MODE)/third_party/python/Lib/urllib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/urllib/robotparser.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/urllib/robotparser.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/uu.pyc:	\
		third_party/python/Lib/uu.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/uu.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/uu.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/uuid.pyc:	\
		third_party/python/Lib/uuid.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/uuid.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/uuid.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/venv/:	\
		o/$(MODE)/third_party/python/Lib/venv/
	@mkdir -p $@

o/$(MODE)/third_party/python/Lib/venv/.zip.o:	\
		o/$(MODE)/third_party/python/Lib/venv/
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/venv/__init__.pyc:	\
		third_party/python/Lib/venv/__init__.py	\
		o/$(MODE)/third_party/python/Lib/venv/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/venv/__init__.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/venv/__init__.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/venv/__main__.pyc:	\
		third_party/python/Lib/venv/__main__.py	\
		o/$(MODE)/third_party/python/Lib/venv/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/venv/__main__.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/venv/__main__.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/warnings.pyc:	\
		third_party/python/Lib/warnings.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/warnings.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/warnings.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/wave.pyc:	\
		third_party/python/Lib/wave.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/wave.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/wave.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/weakref.pyc:	\
		third_party/python/Lib/weakref.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/weakref.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/weakref.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/webbrowser.pyc:	\
		third_party/python/Lib/webbrowser.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/webbrowser.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/webbrowser.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/wsgiref/:	\
		o/$(MODE)/third_party/python/Lib/wsgiref/
	@mkdir -p $@

o/$(MODE)/third_party/python/Lib/wsgiref/.zip.o:	\
		o/$(MODE)/third_party/python/Lib/wsgiref/
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/wsgiref/__init__.pyc:	\
		third_party/python/Lib/wsgiref/__init__.py	\
		o/$(MODE)/third_party/python/Lib/wsgiref/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/wsgiref/__init__.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/wsgiref/__init__.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/wsgiref/handlers.pyc:	\
		third_party/python/Lib/wsgiref/handlers.py	\
		o/$(MODE)/third_party/python/Lib/wsgiref/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/wsgiref/handlers.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/wsgiref/handlers.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/wsgiref/headers.pyc:	\
		third_party/python/Lib/wsgiref/headers.py	\
		o/$(MODE)/third_party/python/Lib/wsgiref/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/wsgiref/headers.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/wsgiref/headers.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/wsgiref/simple_server.pyc:	\
		third_party/python/Lib/wsgiref/simple_server.py	\
		o/$(MODE)/third_party/python/Lib/wsgiref/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/wsgiref/simple_server.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/wsgiref/simple_server.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/wsgiref/util.pyc:	\
		third_party/python/Lib/wsgiref/util.py	\
		o/$(MODE)/third_party/python/Lib/wsgiref/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/wsgiref/util.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/wsgiref/util.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/wsgiref/validate.pyc:	\
		third_party/python/Lib/wsgiref/validate.py	\
		o/$(MODE)/third_party/python/Lib/wsgiref/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/wsgiref/validate.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/wsgiref/validate.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/xdrlib.pyc:	\
		third_party/python/Lib/xdrlib.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/xdrlib.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/xdrlib.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/xml/:	\
		o/$(MODE)/third_party/python/Lib/xml/
	@mkdir -p $@

o/$(MODE)/third_party/python/Lib/xml/.zip.o:	\
		o/$(MODE)/third_party/python/Lib/xml/
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/xml/__init__.pyc:	\
		third_party/python/Lib/xml/__init__.py	\
		o/$(MODE)/third_party/python/Lib/xml/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/xml/__init__.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/xml/__init__.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/xml/dom/:	\
		o/$(MODE)/third_party/python/Lib/xml/dom/
	@mkdir -p $@

o/$(MODE)/third_party/python/Lib/xml/dom/.zip.o:	\
		o/$(MODE)/third_party/python/Lib/xml/dom/
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/xml/dom/NodeFilter.pyc:	\
		third_party/python/Lib/xml/dom/NodeFilter.py	\
		o/$(MODE)/third_party/python/Lib/xml/dom/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/xml/dom/NodeFilter.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/xml/dom/NodeFilter.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/xml/dom/__init__.pyc:	\
		third_party/python/Lib/xml/dom/__init__.py	\
		o/$(MODE)/third_party/python/Lib/xml/dom/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/xml/dom/__init__.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/xml/dom/__init__.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/xml/dom/domreg.pyc:	\
		third_party/python/Lib/xml/dom/domreg.py	\
		o/$(MODE)/third_party/python/Lib/xml/dom/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/xml/dom/domreg.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/xml/dom/domreg.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/xml/dom/expatbuilder.pyc:	\
		third_party/python/Lib/xml/dom/expatbuilder.py	\
		o/$(MODE)/third_party/python/Lib/xml/dom/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/xml/dom/expatbuilder.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/xml/dom/expatbuilder.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/xml/dom/minicompat.pyc:	\
		third_party/python/Lib/xml/dom/minicompat.py	\
		o/$(MODE)/third_party/python/Lib/xml/dom/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/xml/dom/minicompat.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/xml/dom/minicompat.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/xml/dom/minidom.pyc:	\
		third_party/python/Lib/xml/dom/minidom.py	\
		o/$(MODE)/third_party/python/Lib/xml/dom/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/xml/dom/minidom.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/xml/dom/minidom.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/xml/dom/pulldom.pyc:	\
		third_party/python/Lib/xml/dom/pulldom.py	\
		o/$(MODE)/third_party/python/Lib/xml/dom/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/xml/dom/pulldom.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/xml/dom/pulldom.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/xml/dom/xmlbuilder.pyc:	\
		third_party/python/Lib/xml/dom/xmlbuilder.py	\
		o/$(MODE)/third_party/python/Lib/xml/dom/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/xml/dom/xmlbuilder.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/xml/dom/xmlbuilder.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/xml/etree/:	\
		o/$(MODE)/third_party/python/Lib/xml/etree/
	@mkdir -p $@

o/$(MODE)/third_party/python/Lib/xml/etree/.zip.o:	\
		o/$(MODE)/third_party/python/Lib/xml/etree/
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/xml/etree/ElementInclude.pyc:	\
		third_party/python/Lib/xml/etree/ElementInclude.py	\
		o/$(MODE)/third_party/python/Lib/xml/etree/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/xml/etree/ElementInclude.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/xml/etree/ElementInclude.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/xml/etree/ElementPath.pyc:	\
		third_party/python/Lib/xml/etree/ElementPath.py	\
		o/$(MODE)/third_party/python/Lib/xml/etree/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/xml/etree/ElementPath.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/xml/etree/ElementPath.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/xml/etree/ElementTree.pyc:	\
		third_party/python/Lib/xml/etree/ElementTree.py	\
		o/$(MODE)/third_party/python/Lib/xml/etree/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/xml/etree/ElementTree.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/xml/etree/ElementTree.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/xml/etree/__init__.pyc:	\
		third_party/python/Lib/xml/etree/__init__.py	\
		o/$(MODE)/third_party/python/Lib/xml/etree/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/xml/etree/__init__.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/xml/etree/__init__.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/xml/etree/cElementTree.pyc:	\
		third_party/python/Lib/xml/etree/cElementTree.py	\
		o/$(MODE)/third_party/python/Lib/xml/etree/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/xml/etree/cElementTree.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/xml/etree/cElementTree.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/xml/parsers/:	\
		o/$(MODE)/third_party/python/Lib/xml/parsers/
	@mkdir -p $@

o/$(MODE)/third_party/python/Lib/xml/parsers/.zip.o:	\
		o/$(MODE)/third_party/python/Lib/xml/parsers/
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/xml/parsers/__init__.pyc:	\
		third_party/python/Lib/xml/parsers/__init__.py	\
		o/$(MODE)/third_party/python/Lib/xml/parsers/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/xml/parsers/__init__.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/xml/parsers/__init__.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/xml/parsers/expat.pyc:	\
		third_party/python/Lib/xml/parsers/expat.py	\
		o/$(MODE)/third_party/python/Lib/xml/parsers/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/xml/parsers/expat.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/xml/parsers/expat.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/xml/sax/:	\
		o/$(MODE)/third_party/python/Lib/xml/sax/
	@mkdir -p $@

o/$(MODE)/third_party/python/Lib/xml/sax/.zip.o:	\
		o/$(MODE)/third_party/python/Lib/xml/sax/
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/xml/sax/__init__.pyc:	\
		third_party/python/Lib/xml/sax/__init__.py	\
		o/$(MODE)/third_party/python/Lib/xml/sax/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/xml/sax/__init__.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/xml/sax/__init__.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/xml/sax/_exceptions.pyc:	\
		third_party/python/Lib/xml/sax/_exceptions.py	\
		o/$(MODE)/third_party/python/Lib/xml/sax/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/xml/sax/_exceptions.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/xml/sax/_exceptions.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/xml/sax/expatreader.pyc:	\
		third_party/python/Lib/xml/sax/expatreader.py	\
		o/$(MODE)/third_party/python/Lib/xml/sax/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/xml/sax/expatreader.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/xml/sax/expatreader.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/xml/sax/handler.pyc:	\
		third_party/python/Lib/xml/sax/handler.py	\
		o/$(MODE)/third_party/python/Lib/xml/sax/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/xml/sax/handler.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/xml/sax/handler.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/xml/sax/saxutils.pyc:	\
		third_party/python/Lib/xml/sax/saxutils.py	\
		o/$(MODE)/third_party/python/Lib/xml/sax/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/xml/sax/saxutils.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/xml/sax/saxutils.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/xml/sax/xmlreader.pyc:	\
		third_party/python/Lib/xml/sax/xmlreader.py	\
		o/$(MODE)/third_party/python/Lib/xml/sax/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/xml/sax/xmlreader.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/xml/sax/xmlreader.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/xmlrpc/:	\
		o/$(MODE)/third_party/python/Lib/xmlrpc/
	@mkdir -p $@

o/$(MODE)/third_party/python/Lib/xmlrpc/.zip.o:	\
		o/$(MODE)/third_party/python/Lib/xmlrpc/
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/xmlrpc/__init__.pyc:	\
		third_party/python/Lib/xmlrpc/__init__.py	\
		o/$(MODE)/third_party/python/Lib/xmlrpc/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/xmlrpc/__init__.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/xmlrpc/__init__.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/xmlrpc/client.pyc:	\
		third_party/python/Lib/xmlrpc/client.py	\
		o/$(MODE)/third_party/python/Lib/xmlrpc/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/xmlrpc/client.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/xmlrpc/client.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/xmlrpc/server.pyc:	\
		third_party/python/Lib/xmlrpc/server.py	\
		o/$(MODE)/third_party/python/Lib/xmlrpc/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/xmlrpc/server.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/xmlrpc/server.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/zipapp.pyc:	\
		third_party/python/Lib/zipapp.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/zipapp.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/zipapp.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

o/$(MODE)/third_party/python/Lib/zipfile.pyc:	\
		third_party/python/Lib/zipfile.py	\
		o/$(MODE)/third_party/python/Lib/	\
		o/$(MODE)/third_party/python/pycomp
	@$(COMPILE) -APYCOMP o/$(MODE)/third_party/python/pycomp -o $@ $<

o/$(MODE)/third_party/python/Lib/zipfile.pyc.zip.o:	\
		o/$(MODE)/third_party/python/Lib/zipfile.pyc
	@$(COMPILE) -AZIPOBJ $(ZIPOBJ) $(ZIPOBJ_FLAGS) $(OUTPUT_OPTION) $<

THIRD_PARTY_PYTHON_STDLIB_PYC_OBJS =	\
	o/$(MODE)/third_party/python/Lib/.zip.o	\
	o/$(MODE)/third_party/python/Lib/__future__.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/_bootlocale.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/_collections_abc.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/_compat_pickle.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/_compression.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/_dummy_thread.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/_markupbase.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/_osx_support.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/_pyio.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/_sitebuiltins.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/_strptime.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/_sysconfigdata_m_cosmo_x86_64-cosmo.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/_threading_local.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/_weakrefset.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/abc.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/aifc.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/antigravity.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/argparse.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/ast.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/asynchat.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/asyncore.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/base64.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/bdb.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/binhex.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/bisect.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/bz2.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/cProfile.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/calendar.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/cgi.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/cgitb.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/chunk.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/cmd.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/code.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/codecs.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/codeop.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/colorsys.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/compileall.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/configparser.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/contextlib.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/copy.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/copyreg.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/crypt.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/csv.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/datetime.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/decimal.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/difflib.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/dis.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/doctest.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/dummy_threading.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/enum.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/filecmp.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/fileinput.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/fnmatch.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/formatter.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/fractions.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/ftplib.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/functools.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/genericpath.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/getopt.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/getpass.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/gettext.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/glob.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/gzip.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/hashlib.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/heapq.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/hmac.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/imaplib.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/imghdr.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/imp.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/inspect.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/io.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/ipaddress.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/keyword.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/linecache.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/locale.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/lzma.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/macpath.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/macurl2path.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/mailbox.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/mailcap.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/mimetypes.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/modulefinder.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/netrc.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/nntplib.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/ntpath.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/nturl2path.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/numbers.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/opcode.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/operator.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/optparse.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/os.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/pathlib.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/pdb.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/pickle.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/pickletools.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/pipes.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/pkgutil.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/platform.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/plistlib.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/poplib.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/posixpath.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/pprint.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/profile.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/pstats.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/pty.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/py_compile.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/pyclbr.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/pydoc.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/queue.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/quopri.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/random.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/re.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/reprlib.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/rlcompleter.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/runpy.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/sched.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/secrets.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/selectors.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/shelve.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/shlex.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/shutil.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/signal.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/site.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/smtpd.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/smtplib.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/sndhdr.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/socket.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/socketserver.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/sre_compile.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/sre_constants.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/sre_parse.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/ssl.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/stat.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/statistics.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/string.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/stringprep.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/struct.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/subprocess.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/sunau.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/symbol.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/symtable.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/sysconfig.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/tabnanny.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/tarfile.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/telnetlib.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/tempfile.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/textwrap.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/this.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/threading.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/timeit.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/token.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/tokenize.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/trace.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/traceback.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/tracemalloc.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/tty.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/types.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/typing.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/uu.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/uuid.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/warnings.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/wave.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/weakref.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/webbrowser.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/xdrlib.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/zipapp.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/zipfile.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/asyncio/.zip.o	\
	o/$(MODE)/third_party/python/Lib/asyncio/__init__.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/asyncio/base_events.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/asyncio/base_futures.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/asyncio/base_subprocess.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/asyncio/base_tasks.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/asyncio/compat.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/asyncio/constants.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/asyncio/coroutines.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/asyncio/events.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/asyncio/futures.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/asyncio/locks.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/asyncio/log.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/asyncio/proactor_events.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/asyncio/protocols.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/asyncio/queues.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/asyncio/selector_events.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/asyncio/sslproto.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/asyncio/streams.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/asyncio/subprocess.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/asyncio/tasks.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/asyncio/test_utils.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/asyncio/transports.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/asyncio/unix_events.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/asyncio/windows_events.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/asyncio/windows_utils.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/collections/.zip.o	\
	o/$(MODE)/third_party/python/Lib/collections/__init__.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/collections/abc.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/dbm/.zip.o	\
	o/$(MODE)/third_party/python/Lib/dbm/__init__.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/dbm/dumb.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/dbm/gnu.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/dbm/ndbm.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/__init__.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/_msvccompiler.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/archive_util.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/bcppcompiler.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/ccompiler.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/cmd.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/config.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/core.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/cygwinccompiler.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/debug.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/dep_util.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/dir_util.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/dist.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/errors.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/extension.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/fancy_getopt.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/file_util.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/filelist.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/log.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/msvc9compiler.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/msvccompiler.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/spawn.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/sysconfig.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/text_file.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/unixccompiler.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/util.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/version.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/versionpredicate.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/command/.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/command/__init__.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/command/bdist.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/command/bdist_dumb.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/command/bdist_msi.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/command/bdist_rpm.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/command/bdist_wininst.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/command/build.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/command/build_clib.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/command/build_ext.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/command/build_py.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/command/build_scripts.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/command/check.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/command/clean.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/command/config.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/command/install.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/command/install_data.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/command/install_egg_info.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/command/install_headers.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/command/install_lib.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/command/install_scripts.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/command/register.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/command/sdist.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/command/upload.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/tests/.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/tests/__init__.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/tests/support.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/tests/test_archive_util.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/tests/test_bdist.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/tests/test_bdist_dumb.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/tests/test_bdist_msi.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/tests/test_bdist_rpm.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/tests/test_bdist_wininst.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/tests/test_build.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/tests/test_build_clib.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/tests/test_build_ext.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/tests/test_build_py.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/tests/test_build_scripts.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/tests/test_check.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/tests/test_clean.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/tests/test_cmd.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/tests/test_config.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/tests/test_config_cmd.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/tests/test_core.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/tests/test_cygwinccompiler.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/tests/test_dep_util.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/tests/test_dir_util.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/tests/test_dist.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/tests/test_extension.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/tests/test_file_util.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/tests/test_filelist.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/tests/test_install.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/tests/test_install_data.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/tests/test_install_headers.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/tests/test_install_lib.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/tests/test_install_scripts.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/tests/test_log.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/tests/test_msvc9compiler.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/tests/test_msvccompiler.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/tests/test_register.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/tests/test_sdist.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/tests/test_spawn.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/tests/test_sysconfig.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/tests/test_text_file.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/tests/test_unixccompiler.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/tests/test_upload.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/tests/test_util.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/tests/test_version.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/distutils/tests/test_versionpredicate.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/email/.zip.o	\
	o/$(MODE)/third_party/python/Lib/email/__init__.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/email/_encoded_words.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/email/_header_value_parser.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/email/_parseaddr.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/email/_policybase.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/email/base64mime.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/email/charset.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/email/contentmanager.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/email/encoders.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/email/errors.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/email/feedparser.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/email/generator.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/email/header.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/email/headerregistry.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/email/iterators.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/email/message.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/email/parser.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/email/policy.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/email/quoprimime.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/email/utils.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/email/mime/.zip.o	\
	o/$(MODE)/third_party/python/Lib/email/mime/__init__.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/email/mime/application.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/email/mime/audio.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/email/mime/base.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/email/mime/image.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/email/mime/message.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/email/mime/multipart.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/email/mime/nonmultipart.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/email/mime/text.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/__init__.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/aliases.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/ascii.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/base64_codec.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/big5.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/big5hkscs.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/bz2_codec.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/charmap.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/cp037.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/cp1006.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/cp1026.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/cp1125.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/cp1140.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/cp1250.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/cp1251.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/cp1252.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/cp1253.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/cp1254.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/cp1255.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/cp1256.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/cp1257.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/cp1258.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/cp273.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/cp424.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/cp437.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/cp500.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/cp65001.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/cp720.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/cp737.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/cp775.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/cp850.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/cp852.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/cp855.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/cp856.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/cp857.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/cp858.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/cp860.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/cp861.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/cp862.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/cp863.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/cp864.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/cp865.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/cp866.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/cp869.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/cp874.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/cp875.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/cp932.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/cp949.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/cp950.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/euc_jis_2004.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/euc_jisx0213.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/euc_jp.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/euc_kr.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/gb18030.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/gb2312.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/gbk.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/hex_codec.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/hp_roman8.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/hz.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/idna.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/iso2022_jp.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/iso2022_jp_1.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/iso2022_jp_2.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/iso2022_jp_2004.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/iso2022_jp_3.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/iso2022_jp_ext.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/iso2022_kr.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/iso8859_1.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/iso8859_10.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/iso8859_11.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/iso8859_13.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/iso8859_14.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/iso8859_15.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/iso8859_16.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/iso8859_2.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/iso8859_3.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/iso8859_4.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/iso8859_5.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/iso8859_6.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/iso8859_7.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/iso8859_8.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/iso8859_9.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/johab.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/koi8_r.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/koi8_t.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/koi8_u.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/kz1048.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/latin_1.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/mac_arabic.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/mac_centeuro.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/mac_croatian.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/mac_cyrillic.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/mac_farsi.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/mac_greek.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/mac_iceland.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/mac_latin2.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/mac_roman.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/mac_romanian.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/mac_turkish.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/mbcs.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/oem.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/palmos.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/ptcp154.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/punycode.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/quopri_codec.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/raw_unicode_escape.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/rot_13.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/shift_jis.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/shift_jis_2004.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/shift_jisx0213.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/tis_620.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/undefined.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/unicode_escape.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/unicode_internal.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/utf_16.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/utf_16_be.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/utf_16_le.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/utf_32.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/utf_32_be.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/utf_32_le.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/utf_7.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/utf_8.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/utf_8_sig.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/uu_codec.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/encodings/zlib_codec.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/ensurepip/.zip.o	\
	o/$(MODE)/third_party/python/Lib/ensurepip/__init__.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/ensurepip/__main__.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/ensurepip/_uninstall.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/html/.zip.o	\
	o/$(MODE)/third_party/python/Lib/html/__init__.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/html/entities.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/html/parser.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/http/.zip.o	\
	o/$(MODE)/third_party/python/Lib/http/__init__.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/http/client.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/http/cookiejar.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/http/cookies.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/http/server.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/importlib/.zip.o	\
	o/$(MODE)/third_party/python/Lib/importlib/__init__.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/importlib/_bootstrap.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/importlib/_bootstrap_external.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/importlib/abc.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/importlib/machinery.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/importlib/util.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/json/.zip.o	\
	o/$(MODE)/third_party/python/Lib/json/__init__.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/json/decoder.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/json/encoder.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/json/scanner.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/json/tool.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/logging/.zip.o	\
	o/$(MODE)/third_party/python/Lib/logging/__init__.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/logging/config.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/logging/handlers.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/msilib/.zip.o	\
	o/$(MODE)/third_party/python/Lib/msilib/__init__.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/msilib/schema.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/msilib/sequence.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/msilib/text.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/multiprocessing/.zip.o	\
	o/$(MODE)/third_party/python/Lib/multiprocessing/__init__.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/multiprocessing/connection.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/multiprocessing/context.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/multiprocessing/forkserver.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/multiprocessing/heap.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/multiprocessing/managers.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/multiprocessing/pool.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/multiprocessing/popen_fork.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/multiprocessing/popen_forkserver.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/multiprocessing/popen_spawn_posix.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/multiprocessing/popen_spawn_win32.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/multiprocessing/process.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/multiprocessing/queues.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/multiprocessing/reduction.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/multiprocessing/resource_sharer.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/multiprocessing/semaphore_tracker.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/multiprocessing/sharedctypes.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/multiprocessing/spawn.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/multiprocessing/synchronize.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/multiprocessing/util.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/multiprocessing/dummy/.zip.o	\
	o/$(MODE)/third_party/python/Lib/multiprocessing/dummy/__init__.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/multiprocessing/dummy/connection.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/sqlite3/.zip.o	\
	o/$(MODE)/third_party/python/Lib/sqlite3/__init__.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/sqlite3/dbapi2.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/sqlite3/dump.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/unittest/.zip.o	\
	o/$(MODE)/third_party/python/Lib/unittest/__init__.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/unittest/__main__.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/unittest/case.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/unittest/loader.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/unittest/main.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/unittest/mock.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/unittest/result.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/unittest/runner.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/unittest/signals.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/unittest/suite.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/unittest/util.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/urllib/.zip.o	\
	o/$(MODE)/third_party/python/Lib/urllib/__init__.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/urllib/error.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/urllib/parse.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/urllib/request.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/urllib/response.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/urllib/robotparser.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/venv/.zip.o	\
	o/$(MODE)/third_party/python/Lib/venv/__init__.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/venv/__main__.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/wsgiref/.zip.o	\
	o/$(MODE)/third_party/python/Lib/wsgiref/__init__.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/wsgiref/handlers.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/wsgiref/headers.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/wsgiref/simple_server.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/wsgiref/util.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/wsgiref/validate.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/xml/.zip.o	\
	o/$(MODE)/third_party/python/Lib/xml/__init__.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/xml/dom/.zip.o	\
	o/$(MODE)/third_party/python/Lib/xml/dom/NodeFilter.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/xml/dom/__init__.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/xml/dom/domreg.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/xml/dom/expatbuilder.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/xml/dom/minicompat.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/xml/dom/minidom.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/xml/dom/pulldom.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/xml/dom/xmlbuilder.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/xml/etree/.zip.o	\
	o/$(MODE)/third_party/python/Lib/xml/etree/ElementInclude.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/xml/etree/ElementPath.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/xml/etree/ElementTree.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/xml/etree/__init__.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/xml/etree/cElementTree.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/xml/parsers/.zip.o	\
	o/$(MODE)/third_party/python/Lib/xml/parsers/__init__.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/xml/parsers/expat.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/xml/sax/.zip.o	\
	o/$(MODE)/third_party/python/Lib/xml/sax/__init__.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/xml/sax/_exceptions.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/xml/sax/expatreader.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/xml/sax/handler.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/xml/sax/saxutils.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/xml/sax/xmlreader.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/xmlrpc/.zip.o	\
	o/$(MODE)/third_party/python/Lib/xmlrpc/__init__.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/xmlrpc/client.pyc.zip.o	\
	o/$(MODE)/third_party/python/Lib/xmlrpc/server.pyc.zip.o

$(THIRD_PARTY_PYTHON_STDLIB_PYC_OBJS):								\
		ZIPOBJ_FLAGS +=									\
			-P.python								\
			-C5
