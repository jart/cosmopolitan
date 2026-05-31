CFLAGS	?= -O2
WARNING_CFLAGS ?=  -Wall -Wextra -Wformat=2 -Wno-format-nonliteral
LDFLAGS ?=

# MicroBlaze specific options:
# CFLAGS += -mno-xl-soft-mul -mxl-barrel-shift

# To compile on Plan9:
# CFLAGS += -D_BSD_EXTENSION

PERL ?= perl

ifdef WINDOWS
PYTHON ?= python
else
PYTHON ?= $(shell if type python3 >/dev/null 2>/dev/null; then echo python3; else echo python; fi)
endif

# Set AR_DASH= (empty string) to use an ar implementation that does not accept
# the - prefix for command line options (e.g. llvm-ar)
AR_DASH ?= -

ifndef MBEDTLS_PATH
MBEDTLS_PATH := ..
endif

ifeq (,$(wildcard $(MBEDTLS_PATH)/framework/exported.make))
    # Use the define keyword to get a multi-line message.
    # GNU make appends ".  Stop.", so tweak the ending of our message accordingly.
    define error_message
$(MBEDTLS_PATH)/framework/exported.make not found.
Run `git submodule update --init` to fetch the submodule contents.
This is a fatal error
    endef
    $(error $(error_message))
endif
include $(MBEDTLS_PATH)/framework/exported.make

include $(MBEDTLS_PATH)/tf-psa-crypto/scripts/crypto-common.make

# For the time being, Mbed TLS uses non-public interfaces of TF-PSA-Crypto,
# so we include both public and internal headers.
LOCAL_CFLAGS = $(WARNING_CFLAGS) -I. \
	       $(TF_PSA_CRYPTO_LIBRARY_PRIVATE_INCLUDE) \
               -I../include \
	       $(TF_PSA_CRYPTO_LIBRARY_PUBLIC_INCLUDE) \
	       -D_FILE_OFFSET_BITS=64
LOCAL_LDFLAGS =

ifdef DEBUG
LOCAL_CFLAGS += -g3
endif

# if were running on Windows build for Windows
ifdef WINDOWS
WINDOWS_BUILD=1
else ifeq ($(shell uname -s),Darwin)
ifeq ($(AR),ar)
APPLE_BUILD ?= 1
endif
endif

ifdef WINDOWS_BUILD
LOCAL_LDFLAGS += -lbcrypt
endif

# To compile as a shared library:
ifdef SHARED
# all code is position-indep with mingw, avoid warning about useless flag
ifndef WINDOWS_BUILD
LOCAL_CFLAGS += -fPIC -fpic
endif
endif

SOEXT_TLS?=so.23
SOEXT_X509?=so.9
SOEXT_CRYPTO?=so.18

ARFLAGS = $(AR_DASH)src
ifdef APPLE_BUILD
ifneq ($(APPLE_BUILD),0)
ARFLAGS = $(AR_DASH)Src
RLFLAGS = -no_warning_for_no_symbols -c
RL ?= ranlib
endif
endif

DLEXT ?= so
ifdef WINDOWS_BUILD
# Windows shared library extension:
DLEXT = dll
else ifdef APPLE_BUILD
ifneq ($(APPLE_BUILD),0)
# Mac OS X shared library extension:
DLEXT = dylib
endif
endif

# See root Makefile
GEN_FILES ?= yes
ifdef GEN_FILES
gen_file_dep =
else
gen_file_dep = |
endif

OBJS_CRYPTO = $(TF_PSA_CRYPTO_LIBRARY_OBJS)

OBJS_X509= \
	   mbedtls_config.o \
	   x509.o \
	   x509_create.o \
	   x509_crl.o \
	   x509_crt.o \
	   x509_csr.o \
	   x509_oid.o \
	   x509write.o \
	   x509write_crt.o \
	   x509write_csr.o \
	   pkcs7.o \
	   error.o \
	   # This line is intentionally left blank

OBJS_TLS= \
	  debug.o \
	  mps_reader.o \
	  mps_trace.o \
	  net_sockets.o \
	  ssl_cache.o \
	  ssl_ciphersuites.o \
	  ssl_client.o \
	  ssl_cookie.o \
	  ssl_debug_helpers_generated.o \
	  ssl_msg.o \
	  ssl_ticket.o \
	  ssl_tls.o \
	  ssl_tls12_client.o \
	  ssl_tls12_server.o \
	  ssl_tls13_keys.o \
	  ssl_tls13_client.o \
	  ssl_tls13_server.o \
	  ssl_tls13_generic.o \
	  timing.o \
	  version.o \
	  version_features.o \
	  # This line is intentionally left blank

.SILENT:

.PHONY: all static shared clean

ifndef SHARED
all: static
else
all: shared static
endif

include $(TF_PSA_CRYPTO_CORE_PATH)/crypto-library.make

static: libmbedcrypto.a libmbedx509.a libmbedtls.a
	cd ../tests && echo "This is a seedfile that contains 64 bytes (65 on Windows)......" > seedfile
	cd ../tf-psa-crypto/tests && echo "This is a seedfile that contains 64 bytes (65 on Windows)......" > seedfile

shared: libmbedcrypto.$(DLEXT) libmbedx509.$(DLEXT) libmbedtls.$(DLEXT)

# Windows builds under Mingw can fail if make tries to create archives in the same
# directory at the same time - see https://bugs.launchpad.net/gcc-arm-embedded/+bug/1848002.
# This forces builds of the .a files to be serialised.
ifdef WINDOWS
libmbedtls.a: | libmbedx509.a
libmbedx509.a: | libmbedcrypto.a
endif

# tls
libmbedtls.a: $(OBJS_TLS)
	echo "  AR    $@"
	$(AR) $(ARFLAGS) $@ $(OBJS_TLS)
ifdef APPLE_BUILD
ifneq ($(APPLE_BUILD),0)
	echo "  RL    $@"
	$(RL) $(RLFLAGS) $@
endif
endif

libmbedtls.$(SOEXT_TLS): $(OBJS_TLS) libmbedx509.so
	echo "  LD    $@"
	$(CC) -shared -Wl,-soname,$@ -o $@ $(OBJS_TLS) -L. -lmbedx509 -lmbedcrypto $(LOCAL_LDFLAGS) $(LDFLAGS)

ifneq ($(SOEXT_TLS),so)
libmbedtls.so: libmbedtls.$(SOEXT_TLS)
	echo "  LN    $@ -> $<"
	ln -sf $< $@
endif

libmbedtls.dylib: $(OBJS_TLS) libmbedx509.dylib
	echo "  LD    $@"
	$(CC) -dynamiclib -o $@ $(OBJS_TLS) -L. -lmbedx509 -lmbedcrypto $(LOCAL_LDFLAGS) $(LDFLAGS)

libmbedtls.dll: $(OBJS_TLS) libmbedx509.dll
	echo "  LD    $@"
	$(CC) -shared -Wl,-soname,$@ -Wl,--out-implib,$@.a -o $@ $(OBJS_TLS) -lws2_32 -lwinmm -lgdi32 -L. -lmbedx509 -lmbedcrypto -static-libgcc $(LOCAL_LDFLAGS) $(LDFLAGS)

# x509
libmbedx509.a: $(OBJS_X509)
	echo "  AR    $@"
	$(AR) $(ARFLAGS) $@ $(OBJS_X509)
ifdef APPLE_BUILD
ifneq ($(APPLE_BUILD),0)
	echo "  RL    $@"
	$(RL) $(RLFLAGS) $@
endif
endif

libmbedx509.$(SOEXT_X509): $(OBJS_X509) libmbedcrypto.so
	echo "  LD    $@"
	$(CC) -shared -Wl,-soname,$@ -o $@ $(OBJS_X509) -L. -lmbedcrypto $(LOCAL_LDFLAGS) $(LDFLAGS)

ifneq ($(SOEXT_X509),so)
libmbedx509.so: libmbedx509.$(SOEXT_X509)
	echo "  LN    $@ -> $<"
	ln -sf $< $@
endif

libmbedx509.dylib: $(OBJS_X509) libmbedcrypto.dylib
	echo "  LD    $@"
	$(CC) -dynamiclib -o $@ $(OBJS_X509) -L. -lmbedcrypto  $(LOCAL_LDFLAGS) $(LDFLAGS)

libmbedx509.dll: $(OBJS_X509) libmbedcrypto.dll
	echo "  LD    $@"
	$(CC) -shared -Wl,-soname,$@ -Wl,--out-implib,$@.a -o $@ $(OBJS_X509) -lws2_32 -lwinmm -lgdi32 -L. -lmbedcrypto -static-libgcc $(LOCAL_LDFLAGS) $(LDFLAGS)

# crypto
libmbedcrypto.a: $(OBJS_CRYPTO)
	echo "  AR    $@"
	$(AR) $(ARFLAGS) $@ $(OBJS_CRYPTO)
ifdef APPLE_BUILD
ifneq ($(APPLE_BUILD),0)
	echo "  RL    $@"
	$(RL) $(RLFLAGS) $@
endif
endif

libmbedcrypto.$(SOEXT_CRYPTO): $(OBJS_CRYPTO)
	echo "  LD    $@"
	$(CC) -shared -Wl,-soname,$@ -o $@ $(OBJS_CRYPTO) $(LOCAL_LDFLAGS) $(LDFLAGS)

ifneq ($(SOEXT_CRYPTO),so)
libmbedcrypto.so: libmbedcrypto.$(SOEXT_CRYPTO)
	echo "  LN    $@ -> $<"
	ln -sf $< $@
endif

libmbedcrypto.dylib: $(OBJS_CRYPTO)
	echo "  LD    $@"
	$(CC) -dynamiclib -o $@ $(OBJS_CRYPTO) $(LOCAL_LDFLAGS) $(LDFLAGS)

libmbedcrypto.dll: $(OBJS_CRYPTO)
	echo "  LD    $@"
	$(CC) -shared -Wl,-soname,$@ -Wl,--out-implib,$@.a -o $@ $(OBJS_CRYPTO) -lws2_32 -lwinmm -lgdi32 -static-libgcc $(LOCAL_LDFLAGS) $(LDFLAGS)

.c.o:
	echo "  CC    $<"
	$(CC) $(LOCAL_CFLAGS) $(CFLAGS) -o $@ -c $<

.c.s:
	echo "  CC    $<"
	$(CC) $(LOCAL_CFLAGS) $(CFLAGS) -S -o $@ -c $<

# List the generated files without running a script, so that this
# works with no tooling dependencies when GEN_FILES is disabled.
GENERATED_FILES := \
	mbedtls_config_check_before.h \
	mbedtls_config_check_final.h \
	mbedtls_config_check_user.h \
	error.c \
	version_features.c \
	ssl_debug_helpers_generated.c \
	$(TF_PSA_CRYPTO_LIBRARY_GENERATED_FILES)

.PHONY: generated_files
generated_files: $(GENERATED_FILES)

error.c: $(gen_file_dep) ../scripts/generate_errors.pl
error.c: $(gen_file_dep) ../scripts/data_files/error.fmt
error.c: $(gen_file_dep) $(filter-out %config%,$(wildcard ../include/mbedtls/*.h))
error.c:
	echo "  Gen   $@"
	$(PERL) ../scripts/generate_errors.pl

ssl_debug_helpers_generated.c: $(gen_file_dep) ../framework/scripts/generate_ssl_debug_helpers.py
ssl_debug_helpers_generated.c: $(gen_file_dep) $(filter-out %config%,$(wildcard ../include/mbedtls/*.h))
ssl_debug_helpers_generated.c:
	echo "  Gen   $@"
	$(PYTHON) ../framework/scripts/generate_ssl_debug_helpers.py --mbedtls-root .. .

version_features.c: $(gen_file_dep) ../scripts/generate_features.pl
version_features.c: $(gen_file_dep) ../scripts/data_files/version_features.fmt
## The generated file only depends on the options that are present in mbedtls_config.h,
## not on which options are set. To avoid regenerating this file all the time
## when switching between configurations, don't declare mbedtls_config.h as a
## dependency. Remove this file from your working tree if you've just added or
## removed an option in mbedtls_config.h.
#version_features.c: ../include/mbedtls/mbedtls_config.h
version_features.c:
	echo "  Gen   $@"
	$(PERL) ../scripts/generate_features.pl

GENERATED_CONFIG_CHECK_FILES = $(shell $(PYTHON) ../scripts/generate_config_checks.py --list .)
$(GENERATED_CONFIG_CHECK_FILES): $(gen_file_dep) \
	../scripts/generate_config_checks.py \
	../framework/scripts/mbedtls_framework/config_checks_generator.py
$(GENERATED_CONFIG_CHECK_FILES):
	echo "  Gen   $(GENERATED_CONFIG_CHECK_FILES)"
	$(PYTHON) ../scripts/generate_config_checks.py

mbedtls_config.o: $(GENERATED_CONFIG_CHECK_FILES)

clean:
ifndef WINDOWS
	rm -f *.o *.s libmbed*
	rm -f $(OBJS_CRYPTO) $(OBJS_CRYPTO:.o=.s)
else
	if exist *.o del /Q /F *.o
	if exist *.s del /Q /F *.s
	if exist libmbed* del /Q /F libmbed*
	del /Q /F del_errors_out_if_the_file_list_is_empty_but_not_if_a_file_does_not_exist $(subst /,\,$(OBJS_CRYPTO))
endif

neat: clean
ifndef WINDOWS
	rm -f $(GENERATED_FILES)
else
	for %f in ($(subst /,\,$(GENERATED_FILES))) if exist %f del /Q /F %f
endif
