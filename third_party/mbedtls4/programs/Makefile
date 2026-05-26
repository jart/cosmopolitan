MBEDTLS_TEST_PATH = ../tests
FRAMEWORK = ${MBEDTLS_PATH}/framework
include ../scripts/common.make

ifeq ($(shell uname -s),Linux)
DLOPEN_LDFLAGS ?= -ldl
else
DLOPEN_LDFLAGS ?=
endif

ifdef RECORD_PSA_STATUS_COVERAGE_LOG
LOCAL_CFLAGS += -Werror -DRECORD_PSA_STATUS_COVERAGE_LOG
endif
DEP=${MBEDLIBS} ${MBEDTLS_TEST_OBJS}

# Only build the dlopen test in shared library builds, and not when building
# for Windows.
ifdef BUILD_DLOPEN
# Don't override the value
else ifdef WINDOWS_BUILD
BUILD_DLOPEN =
else ifdef SHARED
BUILD_DLOPEN = y
else
BUILD_DLOPEN =
endif

# Declare the default rule early, since it must come first, in particular
# before including crypto-programs.make.
default: all

include $(TF_PSA_CRYPTO_PATH)/programs/crypto-programs.make

LOCAL_CFLAGS += -I$(FRAMEWORK)/tests/programs

## The following assignment is the list of base names of applications that
## will be built on Windows. Extra Linux/Unix/POSIX-only applications can
## be declared by appending with `APPS += ...` afterwards.
## See the get_app_list function in scripts/generate_visualc_files.pl and
## make sure to check that it still works if you tweak the format here.
##
## Note: Variables cannot be used to define an apps path. This cannot be
## substituted by the script generate_visualc_files.pl.
APPS = \
	$(TF_PSA_CRYPTO_APPS) \
	ssl/dtls_client \
	ssl/dtls_server \
	ssl/mini_client \
	ssl/ssl_client1 \
	ssl/ssl_client2 \
	ssl/ssl_context_info \
	ssl/ssl_fork_server \
	ssl/ssl_mail_client \
	ssl/ssl_server \
	ssl/ssl_server2 \
	test/metatest \
	test/query_compile_time_config \
	test/query_included_headers \
	test/selftest \
	test/udp_proxy \
	test/zeroize \
	util/pem2der \
	util/strerror \
	x509/cert_app \
	x509/cert_req \
	x509/cert_write \
	x509/crl_app \
	x509/load_roots \
	x509/req_app \
# End of APPS

ifeq ($(THREADING),pthread)
APPS +=	ssl/ssl_pthread_server
endif

ifdef BUILD_DLOPEN
APPS +=	test/dlopen
endif

ifdef TEST_CPP
APPS += test/cpp_dummy_build
endif

EXES = $(patsubst %,%$(EXEXT),$(APPS))

.SILENT:

.PHONY: all clean list fuzz

all: $(EXES)
ifndef WINDOWS
# APPS doesn't include the fuzzing programs, which aren't "normal"
# sample or test programs, and don't build with MSVC which is
# warning about fopen
all: fuzz
endif

SSL_OPT_APPS = $(filter ssl/%,$(APPS))
SSL_OPT_APPS += test/query_compile_time_config test/udp_proxy
# Just the programs needed to run ssl-opt.sh (and compat.sh)
ssl-opt: $(patsubst %,%$(EXEXT),$(SSL_OPT_APPS))
.PHONY: ssl-opt

fuzz: ${MBEDLIBS} ${MBEDTLS_TEST_OBJS}
	$(MAKE) -C fuzz

${MBEDTLS_TEST_OBJS}:
	$(MAKE) -C ../tests mbedtls_test

.PHONY: generated_files
GENERATED_FILES := \
	test/query_config.c \
	$(TF_PSA_CRYPTO_PROGRAMS_GENERATED_FILES)
generated_files: $(GENERATED_FILES)

test/query_config.c: $(gen_file_dep) ../scripts/generate_query_config.pl
## The generated file only depends on the options that are present in mbedtls_config.h,
## not on which options are set. To avoid regenerating this file all the time
## when switching between configurations, don't declare mbedtls_config.h as a
## dependency. Remove this file from your working tree if you've just added or
## removed an option in mbedtls_config.h.
#test/query_config.c: $(gen_file_dep) ../include/mbedtls/mbedtls_config.h
test/query_config.c: $(gen_file_dep) ../scripts/data_files/query_config.fmt
test/query_config.c:
	echo "  Gen   $@"
	$(PERL) ../scripts/generate_query_config.pl

ssl/dtls_client$(EXEXT): ssl/dtls_client.c $(DEP)
	echo "  CC    ssl/dtls_client.c"
	$(CC) $(LOCAL_CFLAGS) $(CFLAGS) ssl/dtls_client.c  $(LOCAL_LDFLAGS) $(LDFLAGS) -o $@

ssl/dtls_server$(EXEXT): ssl/dtls_server.c $(DEP)
	echo "  CC    ssl/dtls_server.c"
	$(CC) $(LOCAL_CFLAGS) $(CFLAGS) ssl/dtls_server.c  $(LOCAL_LDFLAGS) $(LDFLAGS) -o $@

ssl/ssl_client1$(EXEXT): ssl/ssl_client1.c $(DEP)
	echo "  CC    ssl/ssl_client1.c"
	$(CC) $(LOCAL_CFLAGS) $(CFLAGS) ssl/ssl_client1.c  $(LOCAL_LDFLAGS) $(LDFLAGS) -o $@

SSL_TEST_OBJECTS = test/query_config.o ssl/ssl_test_lib.o
SSL_TEST_DEPS = $(SSL_TEST_OBJECTS) \
		$(FRAMEWORK)/tests/programs/query_config.h \
		ssl/ssl_test_lib.h \
		ssl/ssl_test_common_source.c \
		$(DEP)

ssl/ssl_test_lib.o: ssl/ssl_test_lib.c ssl/ssl_test_lib.h $(DEP)
	echo "  CC    ssl/ssl_test_lib.c"
	$(CC) $(LOCAL_CFLAGS) $(CFLAGS) -c ssl/ssl_test_lib.c -o $@

ssl/ssl_client2$(EXEXT): ssl/ssl_client2.c $(SSL_TEST_DEPS)
	echo "  CC    ssl/ssl_client2.c"
	$(CC) $(LOCAL_CFLAGS) $(CFLAGS) ssl/ssl_client2.c $(SSL_TEST_OBJECTS) $(LOCAL_LDFLAGS) $(LDFLAGS) -o $@

ssl/ssl_server$(EXEXT): ssl/ssl_server.c $(DEP)
	echo "  CC    ssl/ssl_server.c"
	$(CC) $(LOCAL_CFLAGS) $(CFLAGS) ssl/ssl_server.c   $(LOCAL_LDFLAGS) $(LDFLAGS) -o $@

ssl/ssl_server2$(EXEXT): ssl/ssl_server2.c $(SSL_TEST_DEPS)
	echo "  CC    ssl/ssl_server2.c"
	$(CC) $(LOCAL_CFLAGS) $(CFLAGS) ssl/ssl_server2.c $(SSL_TEST_OBJECTS) $(LOCAL_LDFLAGS) $(LDFLAGS) -o $@

ssl/ssl_context_info$(EXEXT): ssl/ssl_context_info.c test/query_config.o $(FRAMEWORK)/tests/programs/query_config.h $(DEP)
	echo "  CC    ssl/ssl_context_info.c"
	$(CC) $(LOCAL_CFLAGS) $(CFLAGS) ssl/ssl_context_info.c test/query_config.o $(LOCAL_LDFLAGS) $(LDFLAGS) -o $@

ssl/ssl_fork_server$(EXEXT): ssl/ssl_fork_server.c $(DEP)
	echo "  CC    ssl/ssl_fork_server.c"
	$(CC) $(LOCAL_CFLAGS) $(CFLAGS) ssl/ssl_fork_server.c   $(LOCAL_LDFLAGS) $(LDFLAGS) -o $@

ssl/ssl_pthread_server$(EXEXT): ssl/ssl_pthread_server.c $(DEP)
	echo "  CC    ssl/ssl_pthread_server.c"
	$(CC) $(LOCAL_CFLAGS) $(CFLAGS) ssl/ssl_pthread_server.c   $(LOCAL_LDFLAGS) -lpthread  $(LDFLAGS) -o $@

ssl/ssl_mail_client$(EXEXT): ssl/ssl_mail_client.c $(DEP)
	echo "  CC    ssl/ssl_mail_client.c"
	$(CC) $(LOCAL_CFLAGS) $(CFLAGS) ssl/ssl_mail_client.c   $(LOCAL_LDFLAGS) $(LDFLAGS) -o $@

ssl/mini_client$(EXEXT): ssl/mini_client.c $(DEP)
	echo "  CC    ssl/mini_client.c"
	$(CC) $(LOCAL_CFLAGS) $(CFLAGS) ssl/mini_client.c   $(LOCAL_LDFLAGS) $(LDFLAGS) -o $@

test/cpp_dummy_build.cpp: test/generate_cpp_dummy_build.sh
	echo "  Gen   test/cpp_dummy_build.cpp"
	test/generate_cpp_dummy_build.sh

test/cpp_dummy_build$(EXEXT): test/cpp_dummy_build.cpp $(DEP)
	echo "  CXX   test/cpp_dummy_build.cpp"
	$(CXX) $(LOCAL_CXXFLAGS) $(CXXFLAGS) test/cpp_dummy_build.cpp   $(LOCAL_LDFLAGS) $(LDFLAGS) -o $@

ifdef BUILD_DLOPEN
test/dlopen$(EXEXT): test/dlopen.c $(DEP)
	echo "  CC    test/dlopen.c"
# Do not link any test objects (that would bring in a static dependency on
# libmbedcrypto at least). Do not link with libmbed* (that would defeat the
# purpose of testing dynamic loading).
	$(CC) $(LOCAL_CFLAGS) $(CFLAGS) test/dlopen.c $(LDFLAGS) $(DLOPEN_LDFLAGS) -o $@
endif

test/metatest$(EXEXT): $(FRAMEWORK)/tests/programs/metatest.c $(DEP)
	echo "  CC    $(FRAMEWORK)/tests/programs/metatest.c"
	$(CC) $(LOCAL_CFLAGS) $(CFLAGS) -I../library -I../tf-psa-crypto/core -I../tf-psa-crypto/drivers/builtin/src -I../tf-psa-crypto/platform $(FRAMEWORK)/tests/programs/metatest.c    $(LOCAL_LDFLAGS) $(LDFLAGS) -o $@

test/query_config.o: test/query_config.c $(FRAMEWORK)/tests/programs/query_config.h $(DEP)
	echo "  CC    test/query_config.c"
	$(CC) $(LOCAL_CFLAGS) $(CFLAGS) -c test/query_config.c -o $@

test/query_included_headers$(EXEXT): $(FRAMEWORK)/tests/programs/query_included_headers.c $(DEP)
	echo "  CC    $(FRAMEWORK)/tests/programs/query_included_headers.c"
	$(CC) $(LOCAL_CFLAGS) $(CFLAGS) $(FRAMEWORK)/tests/programs/query_included_headers.c    $(LOCAL_LDFLAGS) $(LDFLAGS) -o $@

test/selftest$(EXEXT): test/selftest.c $(DEP)
	echo "  CC    test/selftest.c"
	$(CC) $(LOCAL_CFLAGS) $(CFLAGS) test/selftest.c    $(LOCAL_LDFLAGS) $(LDFLAGS) -o $@

test/udp_proxy$(EXEXT): test/udp_proxy.c $(DEP)
	echo "  CC    test/udp_proxy.c"
	$(CC) $(LOCAL_CFLAGS) $(CFLAGS) test/udp_proxy.c    $(LOCAL_LDFLAGS) $(LDFLAGS) -o $@

test/zeroize$(EXEXT): $(FRAMEWORK)/tests/programs/zeroize.c $(DEP)
	echo "  CC    $(FRAMEWORK)/tests/programs/zeroize.c"
	$(CC) $(LOCAL_CFLAGS) $(CFLAGS) $(FRAMEWORK)/tests/programs/zeroize.c    $(LOCAL_LDFLAGS) $(LDFLAGS) -o $@

test/query_compile_time_config$(EXEXT): $(FRAMEWORK)/tests/programs/query_compile_time_config.c test/query_config.o $(FRAMEWORK)/tests/programs/query_config.h $(DEP)
	echo "  CC    $(FRAMEWORK)/tests/programs/query_compile_time_config.c"
	$(CC) $(LOCAL_CFLAGS) $(CFLAGS) $(FRAMEWORK)/tests/programs/query_compile_time_config.c test/query_config.o $(LOCAL_LDFLAGS) $(LDFLAGS) -o $@

util/pem2der$(EXEXT): util/pem2der.c $(DEP)
	echo "  CC    util/pem2der.c"
	$(CC) $(LOCAL_CFLAGS) $(CFLAGS) util/pem2der.c    $(LOCAL_LDFLAGS) $(LDFLAGS) -o $@

util/strerror$(EXEXT): util/strerror.c $(DEP)
	echo "  CC    util/strerror.c"
	$(CC) $(LOCAL_CFLAGS) $(CFLAGS) util/strerror.c    $(LOCAL_LDFLAGS) $(LDFLAGS) -o $@

x509/cert_app$(EXEXT): x509/cert_app.c $(DEP)
	echo "  CC    x509/cert_app.c"
	$(CC) $(LOCAL_CFLAGS) $(CFLAGS) x509/cert_app.c    $(LOCAL_LDFLAGS) $(LDFLAGS) -o $@

x509/cert_write$(EXEXT): x509/cert_write.c $(DEP)
	echo "  CC    x509/cert_write.c"
	$(CC) $(LOCAL_CFLAGS) $(CFLAGS) x509/cert_write.c    $(LOCAL_LDFLAGS) $(LDFLAGS) -o $@

x509/crl_app$(EXEXT): x509/crl_app.c $(DEP)
	echo "  CC    x509/crl_app.c"
	$(CC) $(LOCAL_CFLAGS) $(CFLAGS) x509/crl_app.c    $(LOCAL_LDFLAGS) $(LDFLAGS) -o $@

x509/cert_req$(EXEXT): x509/cert_req.c $(DEP)
	echo "  CC    x509/cert_req.c"
	$(CC) $(LOCAL_CFLAGS) $(CFLAGS) x509/cert_req.c    $(LOCAL_LDFLAGS) $(LDFLAGS) -o $@

x509/load_roots$(EXEXT): x509/load_roots.c $(DEP)
	echo "  CC    x509/load_roots.c"
	$(CC) $(LOCAL_CFLAGS) $(CFLAGS) x509/load_roots.c    $(LOCAL_LDFLAGS) $(LDFLAGS) -o $@

x509/req_app$(EXEXT): x509/req_app.c $(DEP)
	echo "  CC    x509/req_app.c"
	$(CC) $(LOCAL_CFLAGS) $(CFLAGS) x509/req_app.c    $(LOCAL_LDFLAGS) $(LDFLAGS) -o $@

clean:
ifndef WINDOWS
	rm -f $(EXES)
	rm -f */*.o
	-rm -f ssl/ssl_pthread_server$(EXEXT)
	-rm -f test/cpp_dummy_build.cpp test/cpp_dummy_build$(EXEXT)
	-rm -f test/dlopen$(EXEXT)
else
	if exist *.o del /Q /F *.o
	if exist *.exe del /Q /F *.exe
	if exist test\cpp_dummy_build.cpp del /Q /F test\cpp_dummy_build.cpp
endif
	$(MAKE) -C fuzz clean

list:
	echo $(EXES)
