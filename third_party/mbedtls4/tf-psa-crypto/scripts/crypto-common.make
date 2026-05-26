# Helper code for the make build system in Mbed TLS: define some variables
# providing information such as file paths.
# This file is only meant to exist for a short transition period.
# It may change or be removed without notice.
# Do not use it if you are not Mbed TLS!

# Assume that this makefile is located in a first-level subdirectory of the
# Mbed TLS root, and is accessed directly (not via VPATH or such).
# If this is not the case, TF_PSA_CRYPTO_PATH or MBEDTLS_PATH must be defined
# before including this file.
ifneq ($(origin TF_PSA_CRYPTO_PATH), undefined)
  # TF_PSA_CRYPTO_PATH was defined before including this file, good.
else ifneq ($(origin MBEDTLS_PATH), undefined)
  TF_PSA_CRYPTO_PATH := $(MBEDTLS_PATH)/tf-psa-crypto
else
  # $(dir $(lastword $(MAKEFILE_LIST))) is the path to this file, possibly
  # a relative path, with a trailing slash. Strip off another directory
  # from that.
  TF_PSA_CRYPTO_PATH := $(patsubst %/,%,$(dir $(patsubst %/,%,$(dir $(lastword $(MAKEFILE_LIST))))))/tf-psa-crypto
endif

ifeq (,$(wildcard $(TF_PSA_CRYPTO_PATH)/core/psa_crypto.c))
  $(error $$(TF_PSA_CRYPTO_PATH)/core/psa_crypto.c not found)
endif

TF_PSA_CRYPTO_CORE_PATH = $(TF_PSA_CRYPTO_PATH)/core
TF_PSA_CRYPTO_DRIVERS_BUILTIN_SRC_PATH = $(TF_PSA_CRYPTO_PATH)/drivers/builtin/src

# Are we building libtestdriver1? Figure it out based on the path to the
# build directory. This is fragile, but it works with the current code
# for building libtestdriver1 in tests/Makefile in Mbed TLS. If that changes,
# a better way would be to pass an explicit parameter on the make command
# line.
ifeq (,$(patsubst %/libtestdriver1/library,,$(abspath .)))
BUILDING_LIBTESTDRIVER1 = 1
$(info Note: building $(MAKECMDGOALS) for libtestdriver1 in $(abspath .))
else
BUILDING_LIBTESTDRIVER1 =
endif

# Gather information about crypto drivers that are separate from the main
# "builtin" driver (historically located in /3rdparty in Mbed TLS 2.x/3.x).
THIRDPARTY_DIR := $(TF_PSA_CRYPTO_PATH)/drivers
THIRDPARTY_INCLUDES =
THIRDPARTY_PRIVATE_INCLUDES =

# When building libtestdriver1, skip all third-party directories.
# This saves us from having to adapt the third-party build scripts to work
# in libtestdriver1.
# This means we can't use their features in the test driver, which is fine.
ifeq (,$(BUILDING_LIBTESTDRIVER1))
include $(TF_PSA_CRYPTO_PATH)/drivers/everest/Makefile.inc
include $(TF_PSA_CRYPTO_PATH)/drivers/p256-m/Makefile.inc
include $(TF_PSA_CRYPTO_PATH)/drivers/pqcp/Makefile.inc
endif

# Directories with headers of public interfaces of TF-PSA-Crypto
TF_PSA_CRYPTO_LIBRARY_PUBLIC_INCLUDE = \
	-I$(TF_PSA_CRYPTO_PATH)/include \
	-I$(TF_PSA_CRYPTO_PATH)/drivers/builtin/include \
	$(THIRDPARTY_INCLUDES)

# Directories with headers of internal interfaces of TF-PSA-Crypto
# (currently consumed by Mbed TLS, eventually not so when we've finished
# cleaning up)
TF_PSA_CRYPTO_LIBRARY_PRIVATE_INCLUDE = \
	-I$(TF_PSA_CRYPTO_CORE_PATH) \
	-I$(TF_PSA_CRYPTO_DRIVERS_BUILTIN_SRC_PATH) \
	-I$(TF_PSA_CRYPTO_PATH)/dispatch \
	-I$(TF_PSA_CRYPTO_PATH)/extras \
	-I$(TF_PSA_CRYPTO_PATH)/platform \
	-I$(TF_PSA_CRYPTO_PATH)/utilities \
	$(THIRDPARTY_PRIVATE_INCLUDES)

# Extra linker flags required by the crypto library or the platform
TF_PSA_CRYPTO_EXTRA_LDFLAGS =

## Usage: $(call remove_enabled_options_crypto,PREPROCESSOR_INPUT)
## Remove the preprocessor symbols that are set in the current configuration
## from PREPROCESSOR_INPUT. Also normalize whitespace.
## Example:
##   $(call remove_enabled_options_crypto,MBEDTLS_FOO MBEDTLS_BAR)
## This expands to an empty string "" if MBEDTLS_FOO and MBEDTLS_BAR are both
## enabled in the TF-PSA-Crypto configuration, to "MBEDTLS_FOO" if
## MBEDTLS_BAR is enabled but MBEDTLS_FOO is disabled, etc.
##
## This only works with a Unix-like shell environment (Bourne/POSIX-style shell
## and standard commands) and a Unix-like compiler (supporting -E). In
## other environments, the output is likely to be empty.
define remove_enabled_options_crypto
$(strip $(shell
  exec 2>/dev/null;
  { echo '#include <tf-psa-crypto/build_info.h>'; echo $(1); } |
  $(CC) $(TF_PSA_CRYPTO_LIBRARY_PUBLIC_INCLUDE) $(CFLAGS) -E - |
  tail -n 1
))
endef

# Ensure that `THREADING` is always defined. This lets us get a clean run
# with `make --warn-undefined-variables` without making the conditionals
# below more complex than they already are. At this stage, if `$(THREADING)`
# is empty, it means we don't know yet whether the threading implementation
# requires extra `LDFLAGS`. Once we've done the analysis, if `$(THREADING)`
# is empty, it will mean that no extra `LDFLAGS` are required, either
# because threading is disabled or because the threading implementation
# doesn't require any extra `LDFLAGS`.
THREADING ?=

ifndef WINDOWS_BUILD
  ifeq ($(THREADING),)
    # Auto-detect configurations with pthread.
    # If the call to remove_enabled_options returns "control", the symbols
    # are confirmed set and we link with pthread.
    # If the auto-detection fails, the result of the call is empty and
    # we keep THREADING undefined.
    ifeq (control,$(call remove_enabled_options_crypto,control MBEDTLS_THREADING_C MBEDTLS_THREADING_PTHREAD))
      THREADING := pthread
    endif
  endif
  #$(info THREADING = $(THREADING))

  ifeq ($(THREADING),pthread)
    TF_PSA_CRYPTO_EXTRA_LDFLAGS += -lpthread
  endif
endif
