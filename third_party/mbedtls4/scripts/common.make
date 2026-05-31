CFLAGS	?= -O2
WARNING_CFLAGS ?= -Wall -Wextra -Wformat=2 -Wno-format-nonliteral
WARNING_CXXFLAGS ?= -Wall -Wextra -Wformat=2 -Wno-format-nonliteral -std=c++11 -pedantic
LDFLAGS ?=

PERL ?= perl

ifdef WINDOWS
PYTHON ?= python
else
PYTHON ?= $(shell if type python3 >/dev/null 2>/dev/null; then echo python3; else echo python; fi)
endif

ifndef MBEDTLS_PATH
MBEDTLS_PATH := ..
endif

PSASIM_PATH?=$(abspath $(MBEDTLS_PATH)/framework/psasim)


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

# To compile on SunOS: add "-lsocket -lnsl" to LDFLAGS

LOCAL_CFLAGS = $(WARNING_CFLAGS) -I$(MBEDTLS_TEST_PATH)/include \
               -I$(MBEDTLS_PATH)/framework/tests/include \
               -I$(MBEDTLS_PATH)/include \
               $(TF_PSA_CRYPTO_LIBRARY_PUBLIC_INCLUDE) \
               -D_FILE_OFFSET_BITS=64
LOCAL_CXXFLAGS = $(WARNING_CXXFLAGS) $(LOCAL_CFLAGS)

ifdef PSASIM
LOCAL_LDFLAGS = ${MBEDTLS_TEST_OBJS} 		\
		-L$(PSASIM_PATH)/client_libs			\
		-lpsaclient \
		-lmbedtls$(SHARED_SUFFIX)	\
		-lmbedx509$(SHARED_SUFFIX)	\
		-lmbedcrypto$(SHARED_SUFFIX)    \
		$(TF_PSA_CRYPTO_EXTRA_LDFLAGS)
else
LOCAL_LDFLAGS = ${MBEDTLS_TEST_OBJS} 		\
		-L$(MBEDTLS_PATH)/library			\
		-lmbedtls$(SHARED_SUFFIX)	\
		-lmbedx509$(SHARED_SUFFIX)	\
		-lmbedcrypto$(SHARED_SUFFIX)    \
		$(TF_PSA_CRYPTO_EXTRA_LDFLAGS)
endif

ifdef PSASIM
MBEDLIBS=$(PSASIM_PATH)/client_libs/libmbedcrypto.a \
				$(PSASIM_PATH)/client_libs/libmbedx509.a \
				$(PSASIM_PATH)/client_libs/libmbedtls.a \
				$(PSASIM_PATH)/client_libs/libpsaclient.a
else ifndef SHARED
MBEDLIBS=$(MBEDTLS_PATH)/library/libmbedcrypto.a \
				$(MBEDTLS_PATH)/library/libmbedx509.a \
				$(MBEDTLS_PATH)/library/libmbedtls.a
else
MBEDLIBS=$(MBEDTLS_PATH)/library/libmbedcrypto.$(DLEXT) \
				$(MBEDTLS_PATH)/library/libmbedx509.$(DLEXT) \
				$(MBEDTLS_PATH)/library/libmbedtls.$(DLEXT)
endif

ifdef DEBUG
LOCAL_CFLAGS += -g3
endif

# if we're running on Windows, build for Windows
ifdef WINDOWS
WINDOWS_BUILD=1
endif

ifdef WINDOWS_BUILD
  DLEXT=dll
  EXEXT=.exe
  LOCAL_LDFLAGS += -lws2_32 -lbcrypt
  ifdef SHARED
    SHARED_SUFFIX=.$(DLEXT)
  endif
else # Not building for Windows
  DLEXT ?= so
  EXEXT=
  SHARED_SUFFIX=
endif

# See root Makefile
GEN_FILES ?= yes
ifdef GEN_FILES
gen_file_dep =
else
gen_file_dep = |
endif

default: all

$(MBEDLIBS):
	$(MAKE) -C $(MBEDTLS_PATH)/library $(@F)

neat: clean
ifndef WINDOWS
	rm -f $(GENERATED_FILES)
else
	for %f in ($(subst /,\,$(GENERATED_FILES))) if exist %f del /Q /F %f
endif

# Auxiliary modules used by tests and some sample programs
MBEDTLS_CORE_TEST_OBJS := $(patsubst %.c,%.o,$(wildcard \
    ${MBEDTLS_PATH}/framework/tests/src/*.c \
    ${MBEDTLS_PATH}/framework/tests/src/drivers/*.c \
  ))
# Ignore PSA stubs when building for the client side of PSASIM (i.e.
# CRYPTO_CLIENT && !CRYPTO_C) otherwise there will be functions duplicates.
ifdef PSASIM
MBEDTLS_CORE_TEST_OBJS := $(filter-out \
    ${MBEDTLS_PATH}/framework/tests/src/psa_crypto_stubs.o, $(MBEDTLS_CORE_TEST_OBJS)\
  )
endif
# Additional auxiliary modules for TLS testing
MBEDTLS_TLS_TEST_OBJS = $(patsubst %.c,%.o,$(wildcard \
    ${MBEDTLS_TEST_PATH}/src/*.c \
    ${MBEDTLS_TEST_PATH}/src/test_helpers/*.c \
  ))

MBEDTLS_TEST_OBJS = $(MBEDTLS_CORE_TEST_OBJS) $(MBEDTLS_TLS_TEST_OBJS)
