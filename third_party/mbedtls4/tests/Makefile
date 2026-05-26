MBEDTLS_TEST_PATH = .
include ../scripts/common.make

# Set this to -v to see the details of failing test cases
TEST_FLAGS ?= $(if $(filter-out 0 OFF Off off NO No no FALSE False false N n,$(CTEST_OUTPUT_ON_FAILURE)),-v,)

# Declare the default rule early, since it must come first, in particular
# before including crypto-programs.make.
default: all

include $(TF_PSA_CRYPTO_PATH)/tests/crypto-tests.make

# Also include private headers, for the sake of invasive tests.
LOCAL_CFLAGS += -I$(MBEDTLS_PATH)/library
LOCAL_CFLAGS += $(TF_PSA_CRYPTO_LIBRARY_PRIVATE_INCLUDE)

# Enable definition of various functions used throughout the testsuite
# (gethostname, strdup, fileno...) even when compiling with -std=c99. Harmless
# on non-POSIX platforms.
LOCAL_CFLAGS += -D_POSIX_C_SOURCE=200809L

ifdef RECORD_PSA_STATUS_COVERAGE_LOG
LOCAL_CFLAGS += -Werror -DRECORD_PSA_STATUS_COVERAGE_LOG
endif

GENERATED_MBEDTLS_CONFIG_DATA_FILES := $(patsubst tests/%,%,$(shell \
	$(PYTHON) ../framework/scripts/generate_config_tests.py --list || \
	echo FAILED \
))
ifeq ($(GENERATED_MBEDTLS_CONFIG_DATA_FILES),FAILED)
$(error "$(PYTHON) ../framework/scripts/generate_config_tests.py --list" failed)
endif
GENERATED_DATA_FILES += $(GENERATED_MBEDTLS_CONFIG_DATA_FILES)

GENERATED_C_FILES = \
	include/test/test_keys.h include/test/test_certs.h

GENERATED_FILES = \
	$(GENERATED_DATA_FILES) \
	$(TF_PSA_CRYPTO_TESTS_GENERATED_DATA_FILES) \
	$(GENERATED_C_FILES) \
	$(TF_PSA_CRYPTO_TESTS_GENERATED_C_FILES)

# Generated files needed to (fully) run ssl-opt.sh
.PHONY: ssl-opt

opt-testcases/handshake-generated.sh: ../framework/scripts/mbedtls_framework/tls_test_case.py
opt-testcases/handshake-generated.sh: ../scripts/generate_tls_handshake_tests.py
	echo "  Gen   $@"
	$(PYTHON) ../scripts/generate_tls_handshake_tests.py -o $@
GENERATED_FILES += opt-testcases/handshake-generated.sh
ssl-opt: opt-testcases/handshake-generated.sh

opt-testcases/tls13-compat.sh: ../framework/scripts/generate_tls13_compat_tests.py
	echo "  Gen   $@"
	$(PYTHON) ../framework/scripts/generate_tls13_compat_tests.py -o $@
GENERATED_FILES += opt-testcases/tls13-compat.sh
ssl-opt: opt-testcases/tls13-compat.sh

.PHONY: generated_files
generated_files: $(GENERATED_FILES)

# We deliberately omit the configuration files (mbedtls_config.h,
# crypto_config.h) from the depenency list because during development
# and on the CI, we often edit those in a way that doesn't change the
# output, to comment out certain options, or even to remove certain
# lines which do affect the output negatively (it will miss the
# corresponding test cases).
$(GENERATED_MBEDTLS_CONFIG_DATA_FILES): $(gen_file_dep) generated_config_test_data
generated_config_test_data: ../framework/scripts/generate_config_tests.py
generated_config_test_data: ../scripts/config.py
generated_config_test_data: ../framework/scripts/mbedtls_framework/test_case.py
generated_config_test_data: ../framework/scripts/mbedtls_framework/test_data_generation.py
generated_config_test_data:
	echo "  Gen   $(GENERATED_MBEDTLS_CONFIG_DATA_FILES)"
	$(PYTHON) ../framework/scripts/generate_config_tests.py
.SECONDARY: generated_config_test_data

# A test application is built for each suites/test_suite_*.data file.
# Application name is same as .data file's base name and can be
# constructed by stripping path 'suites/' and extension .data.
DATA_FILES = $(filter-out $(GENERATED_DATA_FILES), $(wildcard suites/test_suite_*.data))

# Make sure that generated data files are included even if they don't
# exist yet when the makefile is parsed.
DATA_FILES += $(GENERATED_DATA_FILES)

APPS = $(basename $(subst suites/,,$(DATA_FILES)))
CRYPTO_APPS = $(basename $(subst suites/,,$(TF_PSA_CRYPTO_TESTS_DATA_FILES)))

# Construct executable name by adding OS specific suffix $(EXEXT).
BINARIES := $(addsuffix $(EXEXT),$(APPS))
CRYPTO_BINARIES := $(addsuffix $(EXEXT),$(CRYPTO_APPS))

.SILENT:

.PHONY: all check test clean

all: $(BINARIES) $(CRYPTO_BINARIES)

mbedtls_test: $(MBEDTLS_TEST_OBJS)

include/test/test_certs.h: ../framework/scripts/generate_test_cert_macros.py \
				  $(shell $(PYTHON) ../framework/scripts/generate_test_cert_macros.py --list-dependencies)
	echo "  Gen   $@"
	$(PYTHON) ../framework/scripts/generate_test_cert_macros.py --output $@

include/test/test_keys.h: ../framework/scripts/generate_test_keys.py
	echo "  Gen   $@"
	$(PYTHON) ../framework/scripts/generate_test_keys.py --output $@

TEST_OBJS_DEPS = $(wildcard include/test/*.h include/test/*/*.h)
ifdef RECORD_PSA_STATUS_COVERAGE_LOG
# Explicitly depend on this header because on a clean copy of the source tree,
# it doesn't exist yet and must be generated as part of the build, and
# therefore the wildcard enumeration above doesn't include it.
TEST_OBJS_DEPS += ../framework/tests/include/test/instrument_record_status.h
endif
TEST_OBJS_DEPS += include/test/test_certs.h include/test/test_keys.h \
                  ../tf-psa-crypto/tests/include/test/test_keys.h

# Rule to compile common test C files in framework
../framework/tests/src/%.o : ../framework/tests/src/%.c $(TEST_OBJS_DEPS)
	echo "  CC    $<"
	$(CC) $(LOCAL_CFLAGS) $(CFLAGS) -o $@ -c $<

../framework/tests/src/drivers/%.o : ../framework/tests/src/drivers/%.c
	echo "  CC    $<"
	$(CC) $(LOCAL_CFLAGS) $(CFLAGS) -o $@ -c $<

# Rule to compile common test C files in src folder
src/%.o : src/%.c $(TEST_OBJS_DEPS)
	echo "  CC    $<"
	$(CC) $(LOCAL_CFLAGS) $(CFLAGS) -o $@ -c $<

src/test_helpers/%.o : src/test_helpers/%.c
	echo "  CC    $<"
	$(CC) $(LOCAL_CFLAGS) $(CFLAGS) -o $@ -c $<

C_FILES := $(addsuffix .c,$(APPS)) $(addsuffix .c,$(CRYPTO_APPS))
c: $(C_FILES)

# Wildcard target for test code generation:
# A .c file is generated for each .data file in the suites/ directory. Each .c
# file depends on a .data and .function file from suites/ directory. Following
# nameing convention is followed:
#
#     C file        |        Depends on
#-----------------------------------------------------------------------------
#  foo.c            | suites/foo.function suites/foo.data
#  foo.bar.c        | suites/foo.function suites/foo.bar.data
#
# Note above that .c and .data files have same base name.
# However, corresponding .function file's base name is the word before first
# dot in .c file's base name.
#
.SECONDEXPANSION:

# First handle the tf-psa-crypto case, which has different paths from
# the local case. In GNU Make >=3.82, the shortest match applies regardless
# of the order in the makefile. In GNU Make <=3.81, the first matching rule
# applies.
../tf-psa-crypto/tests/%.c: ../tf-psa-crypto/tests/suites/$$(firstword $$(subst ., ,$$*)).function ../tf-psa-crypto/tests/suites/%.data ../framework/scripts/generate_test_code.py ../tf-psa-crypto/tests/suites/helpers.function ../tf-psa-crypto/tests/suites/main_test.function ../tf-psa-crypto/tests/suites/host_test.function
	echo "  Gen   $@"
	cd ../tf-psa-crypto/tests && $(PYTHON) ../../framework/scripts/generate_test_code.py -f suites/$(firstword $(subst ., ,$*)).function \
		-d suites/$*.data \
		-t suites/main_test.function \
		-p suites/host_test.function \
		-s suites  \
		--helpers-file suites/helpers.function \
		-o .

%.c: suites/$$(firstword $$(subst ., ,$$*)).function suites/%.data ../framework/scripts/generate_test_code.py ../tf-psa-crypto/tests/suites/helpers.function ../tf-psa-crypto/tests/suites/main_test.function ../tf-psa-crypto/tests/suites/host_test.function
	echo "  Gen   $@"
	$(PYTHON) ../framework/scripts/generate_test_code.py -f suites/$(firstword $(subst ., ,$*)).function \
		-d suites/$*.data \
		-t ../tf-psa-crypto/tests/suites/main_test.function \
		-p ../tf-psa-crypto/tests/suites/host_test.function \
		-s suites  \
		--helpers-file ../tf-psa-crypto/tests/suites/helpers.function \
		-o .

$(BINARIES): %$(EXEXT): %.c $(MBEDLIBS) $(TEST_OBJS_DEPS) $(MBEDTLS_TEST_OBJS)
	echo "  CC   $<"
	$(CC) $(LOCAL_CFLAGS) $(CFLAGS) $< $(LOCAL_LDFLAGS) $(LDFLAGS) -o $@

LOCAL_CRYPTO_CFLAGS = $(patsubst -I./include, -I../../tests/include, $(patsubst -I../%,-I../../%, $(LOCAL_CFLAGS)))
LOCAL_CRYPTO_LDFLAGS = $(patsubst -L../library, -L../../library, \
                       $(patsubst -L../tests/%, -L../../tests/%, \
                       $(patsubst ./src/%,../../tests/src/%, \
                       $(patsubst ../framework/tests/src/%,../../framework/tests/src/%, \
					   $(LOCAL_LDFLAGS)))))
$(CRYPTO_BINARIES): %$(EXEXT): %.c $(MBEDLIBS) $(TEST_OBJS_DEPS) $(MBEDTLS_TEST_OBJS)
	echo "  CC   $<"
	cd ../tf-psa-crypto/tests && $(CC) $(LOCAL_CRYPTO_CFLAGS) $(CFLAGS) $(subst $(EXEXT),,$(@F)).c $(LOCAL_CRYPTO_LDFLAGS) $(LDFLAGS) -o $(@F)

clean:
ifndef WINDOWS
	$(MAKE) -C $(PSASIM_PATH) clean
	rm -rf $(BINARIES) *.c *.datax
	rm -rf $(CRYPTO_BINARIES) ../tf-psa-crypto/tests/*.c ../tf-psa-crypto/tests/*.datax
	rm -f src/*.o src/test_helpers/*.o src/libmbed*
	rm -f ../framework/tests/src/*.o ../framework/tests/src/drivers/*.o
	rm -f ../framework/tests/include/test/instrument_record_status.h
	rm -f ../framework/tests/include/alt-extra/*/*_alt.h
	rm -rf libtestdriver1
	rm -rf libpsaclient libpsaserver
	rm -f ../library/libtestdriver1.a
else
	if exist *.c del /Q /F *.c
	if exist *.exe del /Q /F *.exe
	if exist *.datax del /Q /F *.datax
	if exist ../tf-psa-crypto/tests/*.c del /Q /F ../tf-psa-crypto/tests/*.c
	if exist ../tf-psa-crypto/tests/*.exe del /Q /F ../tf-psa-crypto/tests/*.exe
	if exist ../tf-psa-crypto/tests/*.datax del /Q /F ../tf-psa-crypto/tests/*.datax
	if exist src/*.o del /Q /F src/*.o
	if exist src/test_helpers/*.o del /Q /F src/test_helpers/*.o
	if exist src/libmbed* del /Q /F src/libmbed*
	if exist ../framework/tests/src/*.o del /Q /F ../framework/tests/src/*.o
	if exist ../framework/tests/src/drivers/*.o del /Q /F ../framework/tests/src/drivers/*.o
	if exist ../framework/tests/include/test/instrument_record_status.h del /Q /F ../framework/tests/include/test/instrument_record_status.h
endif

# Test suites caught by SKIP_TEST_SUITES are built but not executed.
check: $(BINARIES) $(CRYPTO_BINARIES)
	perl scripts/run-test-suites.pl $(TEST_FLAGS) --skip=$(SKIP_TEST_SUITES)

test: check

# Generate variants of some headers for testing
../framework/tests/include/alt-extra/%_alt.h: ../include/%.h
	perl -p -e 's/^(# *(define|ifndef) +\w+_)H\b/$${1}ALT_H/' $< >$@
../framework/tests/include/alt-extra/%_alt.h: ../tf-psa-crypto/include/%.h
	perl -p -e 's/^(# *(define|ifndef) +\w+_)H\b/$${1}ALT_H/' $< >$@
../framework/tests/include/alt-extra/%_alt.h: ../tf-psa-crypto/drivers/builtin/include/%.h
	perl -p -e 's/^(# *(define|ifndef) +\w+_)H\b/$${1}ALT_H/' $< >$@

# Generate test library
libtestdriver1.a:
	rm -Rf ./libtestdriver1
	mkdir ./libtestdriver1
	mkdir ./libtestdriver1/framework
	mkdir ./libtestdriver1/tf-psa-crypto
	mkdir ./libtestdriver1/tf-psa-crypto/drivers
	mkdir ./libtestdriver1/tf-psa-crypto/drivers/everest
	mkdir ./libtestdriver1/tf-psa-crypto/drivers/p256-m
	touch ./libtestdriver1/tf-psa-crypto/drivers/everest/Makefile.inc
	touch ./libtestdriver1/tf-psa-crypto/drivers/p256-m/Makefile.inc
	cp -Rf ../framework/scripts ../framework/exported.make ./libtestdriver1/framework
	cp -Rf ../library ./libtestdriver1
	cp -Rf ../include ./libtestdriver1
	cp -Rf ../scripts ./libtestdriver1
	cp -Rf ../tf-psa-crypto/core ./libtestdriver1/tf-psa-crypto
	cp -Rf ../tf-psa-crypto/include ./libtestdriver1/tf-psa-crypto
	cp -Rf ../tf-psa-crypto/drivers/builtin ./libtestdriver1/tf-psa-crypto/drivers/builtin
	if [ -d ../tf-psa-crypto/dispatch ]; then \
	    cp -Rf ../tf-psa-crypto/dispatch ./libtestdriver1/tf-psa-crypto/dispatch; \
	fi
	if [ -d ../tf-psa-crypto/extras ]; then \
	    cp -Rf ../tf-psa-crypto/extras ./libtestdriver1/tf-psa-crypto/extras; \
	fi
	if [ -d ../tf-psa-crypto/platform ]; then \
	    cp -Rf ../tf-psa-crypto/platform ./libtestdriver1/tf-psa-crypto/platform; \
	fi
	if [ -d ../tf-psa-crypto/utilities ]; then \
	    cp -Rf ../tf-psa-crypto/utilities ./libtestdriver1/tf-psa-crypto/utilities; \
	fi
	cp -Rf ../tf-psa-crypto/scripts ./libtestdriver1/tf-psa-crypto

	# Set the test driver base (minimal) configuration.
	cp ../tf-psa-crypto/tests/configs/config_test_driver.h ./libtestdriver1/include/mbedtls/mbedtls_config.h
	cp ../tf-psa-crypto/tests/configs/crypto_config_test_driver.h ./libtestdriver1/tf-psa-crypto/include/psa/crypto_config.h

	# Set the PSA cryptography configuration for the test library.
	# The configuration is created by joining the base
	# ../tf-psa-crypto/tests/configs/crypto_config_test_driver.h,
	# with the the library's PSA_WANT_* macros extracted from
	# ./tf-psa-crypto/include/psa/crypto_config.h
	# and then extended with entries of
	# ../tf-psa-crypto/tests/configs/crypto_config_test_driver_extension.h
	# to mirror the PSA_ACCEL_* macros.

	mv ./libtestdriver1/tf-psa-crypto/include/psa/crypto_config.h ./libtestdriver1/tf-psa-crypto/include/psa/crypto_config.h.bak
	head -n -1 ./libtestdriver1/tf-psa-crypto/include/psa/crypto_config.h.bak > ./libtestdriver1/tf-psa-crypto/include/psa/crypto_config.h
	grep '^#define PSA_WANT_*' ../tf-psa-crypto/include/psa/crypto_config.h >> ./libtestdriver1/tf-psa-crypto/include/psa/crypto_config.h
	cat ../tf-psa-crypto/tests/configs/crypto_config_test_driver_extension.h >> ./libtestdriver1/tf-psa-crypto/include/psa/crypto_config.h
	echo "#endif /* PSA_CRYPTO_CONFIG_H */" >> ./libtestdriver1/tf-psa-crypto/include/psa/crypto_config.h

	# Prefix MBEDTLS_* PSA_* symbols with LIBTESTDRIVER1_ as well as
	# mbedtls_* psa_* symbols with libtestdriver1_ to avoid symbol clash
	# when this test driver library is linked with the Mbed TLS library.
	perl -i ./scripts/libtestdriver1_rewrite.pl ./libtestdriver1/library/*.[ch]
	perl -i ./scripts/libtestdriver1_rewrite.pl ./libtestdriver1/include/*/*.h
	perl -i ./scripts/libtestdriver1_rewrite.pl ./libtestdriver1/tf-psa-crypto/core/*.[ch]
	perl -i ./scripts/libtestdriver1_rewrite.pl ./libtestdriver1/tf-psa-crypto/include/*/*.h
	perl -i ./scripts/libtestdriver1_rewrite.pl ./libtestdriver1/tf-psa-crypto/include/*/*/*.h
	perl -i ./scripts/libtestdriver1_rewrite.pl ./libtestdriver1/tf-psa-crypto/drivers/builtin/include/*/*.h
	perl -i ./scripts/libtestdriver1_rewrite.pl ./libtestdriver1/tf-psa-crypto/drivers/builtin/include/*/*/*.h
	perl -i ./scripts/libtestdriver1_rewrite.pl ./libtestdriver1/tf-psa-crypto/drivers/builtin/src/*.[ch]
	if [ -d ../tf-psa-crypto/dispatch ]; then \
	    perl -i ./scripts/libtestdriver1_rewrite.pl ./libtestdriver1/tf-psa-crypto/dispatch/*.[ch]; \
	fi
	if [ -d ../tf-psa-crypto/extras ]; then \
	    perl -i ./scripts/libtestdriver1_rewrite.pl ./libtestdriver1/tf-psa-crypto/extras/*.[ch]; \
	fi
	if [ -d ../tf-psa-crypto/platform ]; then \
	    perl -i ./scripts/libtestdriver1_rewrite.pl ./libtestdriver1/tf-psa-crypto/platform/*.[ch]; \
	fi
	if [ -d ../tf-psa-crypto/utilities ]; then \
	    perl -i ./scripts/libtestdriver1_rewrite.pl ./libtestdriver1/tf-psa-crypto/utilities/*.[ch]; \
	fi
	$(MAKE) -C ./libtestdriver1/library CFLAGS="-I../../ $(CFLAGS)" LDFLAGS="$(LDFLAGS)" libmbedcrypto.a
	cp ./libtestdriver1/library/libmbedcrypto.a ../library/libtestdriver1.a

ifdef RECORD_PSA_STATUS_COVERAGE_LOG
../framework/tests/include/test/instrument_record_status.h: ../tf-psa-crypto/include/psa/crypto.h Makefile
	echo "  Gen  $@"
	sed <../tf-psa-crypto/include/psa/crypto.h >$@ -n 's/^psa_status_t \([A-Za-z0-9_]*\)(.*/#define \1(...) RECORD_STATUS("\1", \1(__VA_ARGS__))/p'
endif
