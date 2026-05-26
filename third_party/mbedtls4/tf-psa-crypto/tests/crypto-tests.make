# Helper code for tests/Makefile in Mbed TLS.
# This file is only meant to be included by tests/Makefile in Mbed TLS and
# is unlikely to work in another context.

GENERATED_BIGNUM_DATA_FILES := $(addprefix ../tf-psa-crypto/,$(shell \
	$(PYTHON) ../framework/scripts/generate_bignum_tests.py --list || \
	echo FAILED \
))
ifeq ($(GENERATED_BIGNUM_DATA_FILES),FAILED)
$(error "$(PYTHON) ../framework/scripts/generate_bignum_tests.py --list" failed)
endif
TF_PSA_CRYPTO_TESTS_GENERATED_DATA_FILES += $(GENERATED_BIGNUM_DATA_FILES)

# generate_bignum_tests.py and generate_psa_tests.py spend more time analyzing
# inputs than generating outputs. Its inputs are the same no matter which files
# are being generated.
# It's rare not to want all the outputs. So always generate all of its outputs.
# Use an intermediate phony dependency so that parallel builds don't run
# a separate instance of the recipe for each output file.
$(GENERATED_BIGNUM_DATA_FILES): $(gen_file_dep) generated_bignum_test_data
generated_bignum_test_data: ../framework/scripts/generate_bignum_tests.py
generated_bignum_test_data: ../framework/scripts/mbedtls_framework/bignum_common.py
generated_bignum_test_data: ../framework/scripts/mbedtls_framework/bignum_core.py
generated_bignum_test_data: ../framework/scripts/mbedtls_framework/bignum_mod_raw.py
generated_bignum_test_data: ../framework/scripts/mbedtls_framework/bignum_mod.py
generated_bignum_test_data: ../framework/scripts/mbedtls_framework/test_case.py
generated_bignum_test_data: ../framework/scripts/mbedtls_framework/test_data_generation.py
generated_bignum_test_data:
	echo "  Gen   $(GENERATED_BIGNUM_DATA_FILES)"
	$(PYTHON) ../framework/scripts/generate_bignum_tests.py --directory ../tf-psa-crypto/tests/suites
.SECONDARY: generated_bignum_test_data

GENERATED_CRYPTO_CONFIG_DATA_FILES := $(addprefix ../tf-psa-crypto/,$(shell \
	$(PYTHON) ../tf-psa-crypto/framework/scripts/generate_config_tests.py --list || \
	echo FAILED \
))
ifeq ($(GENERATED_CRYPTO_CONFIG_DATA_FILES),FAILED)
$(error "$(PYTHON) ../tf-psa-crypto/framework/scripts/generate_config_tests.py --list" failed)
endif
TF_PSA_CRYPTO_TESTS_GENERATED_DATA_FILES += $(GENERATED_CRYPTO_CONFIG_DATA_FILES)

# We deliberately omit the configuration files (mbedtls_config.h,
# crypto_config.h) from the depenency list because during development
# and on the CI, we often edit those in a way that doesn't change the
# output, to comment out certain options, or even to remove certain
# lines which do affect the output negatively (it will miss the
# corresponding test cases).
$(GENERATED_CRYPTO_CONFIG_DATA_FILES): $(gen_file_dep) generated_crypto_config_test_data
generated_crypto_config_test_data: ../framework/scripts/generate_config_tests.py
generated_crypto_config_test_data: ../scripts/config.py
generated_crypto_config_test_data: ../framework/scripts/mbedtls_framework/test_case.py
generated_crypto_config_test_data: ../framework/scripts/mbedtls_framework/test_data_generation.py
generated_crypto_config_test_data:
	echo "  Gen   $(GENERATED_CRYPTO_CONFIG_DATA_FILES)"
	cd ../tf-psa-crypto && $(PYTHON) ./framework/scripts/generate_config_tests.py
.SECONDARY: generated_crypto_config_test_data

GENERATED_ECP_DATA_FILES := $(addprefix ../tf-psa-crypto/,$(shell \
	$(PYTHON) ../framework/scripts/generate_ecp_tests.py --list || \
	echo FAILED \
))
ifeq ($(GENERATED_ECP_DATA_FILES),FAILED)
$(error "$(PYTHON) ../framework/scripts/generate_ecp_tests.py --list" failed)
endif
TF_PSA_CRYPTO_TESTS_GENERATED_DATA_FILES += $(GENERATED_ECP_DATA_FILES)

$(GENERATED_ECP_DATA_FILES): $(gen_file_dep) generated_ecp_test_data
generated_ecp_test_data: ../framework/scripts/generate_ecp_tests.py
generated_ecp_test_data: ../framework/scripts/mbedtls_framework/bignum_common.py
generated_ecp_test_data: ../framework/scripts/mbedtls_framework/ecp.py
generated_ecp_test_data: ../framework/scripts/mbedtls_framework/test_case.py
generated_ecp_test_data: ../framework/scripts/mbedtls_framework/test_data_generation.py
generated_ecp_test_data:
	echo "  Gen   $(GENERATED_ECP_DATA_FILES)"
	$(PYTHON) ../framework/scripts/generate_ecp_tests.py --directory ../tf-psa-crypto/tests/suites
.SECONDARY: generated_ecp_test_data

GENERATED_PSA_DATA_FILES := $(addprefix ../tf-psa-crypto/,$(shell \
	$(PYTHON) ../framework/scripts/generate_psa_tests.py --list || \
	echo FAILED \
))
ifeq ($(GENERATED_PSA_DATA_FILES),FAILED)
$(error "$(PYTHON) ../framework/scripts/generate_psa_tests.py --list" failed)
endif
TF_PSA_CRYPTO_TESTS_GENERATED_DATA_FILES += $(GENERATED_PSA_DATA_FILES)

$(GENERATED_PSA_DATA_FILES): $(gen_file_dep) generated_psa_test_data
generated_psa_test_data: ../framework/scripts/generate_psa_tests.py
generated_psa_test_data: ../framework/scripts/mbedtls_framework/crypto_data_tests.py
generated_psa_test_data: ../framework/scripts/mbedtls_framework/crypto_knowledge.py
generated_psa_test_data: ../framework/scripts/mbedtls_framework/macro_collector.py
generated_psa_test_data: ../framework/scripts/mbedtls_framework/psa_information.py
generated_psa_test_data: ../framework/scripts/mbedtls_framework/psa_storage.py
generated_psa_test_data: ../framework/scripts/mbedtls_framework/psa_test_case.py
generated_psa_test_data: ../framework/scripts/mbedtls_framework/test_case.py
generated_psa_test_data: ../framework/scripts/mbedtls_framework/test_data_generation.py
## The generated file only depends on the options that are present in
## crypto_config.h, not on which options are set. To avoid regenerating this
## file all the time when switching between configurations, don't declare
## crypto_config.h as a dependency. Remove this file from your working tree
## if you've just added or removed an option in crypto_config.h.
#generated_psa_test_data: ../tf-psa-crypto/include/psa/crypto_config.h
generated_psa_test_data: ../tf-psa-crypto/include/psa/crypto_values.h
generated_psa_test_data: ../tf-psa-crypto/include/psa/crypto_extra.h
generated_psa_test_data: ../tf-psa-crypto/tests/suites/test_suite_psa_crypto_metadata.data
generated_psa_test_data:
	echo "  Gen   $(GENERATED_PSA_DATA_FILES) ..."
	$(PYTHON) ../framework/scripts/generate_psa_tests.py --directory ../tf-psa-crypto/tests/suites
.SECONDARY: generated_psa_test_data

TF_PSA_CRYPTO_TESTS_DATA_FILES = $(filter-out $(TF_PSA_CRYPTO_TESTS_GENERATED_DATA_FILES), $(wildcard $(TF_PSA_CRYPTO_PATH)/tests/suites/test_suite_*.data))
# Make sure that generated data files are included even if they don't
# exist yet when the makefile is parsed.
TF_PSA_CRYPTO_TESTS_DATA_FILES += $(TF_PSA_CRYPTO_TESTS_GENERATED_DATA_FILES)

../tf-psa-crypto/tests/include/test/test_keys.h: ../tf-psa-crypto/framework/scripts/generate_test_keys.py
	echo "  Gen   $@"
	$(PYTHON) ../tf-psa-crypto/framework/scripts/generate_test_keys.py --output $@

TF_PSA_CRYPTO_TESTS_GENERATED_C_FILES = \
	../tf-psa-crypto/tests/include/test/test_keys.h
