# Helper code for library/Makefile in Mbed TLS.
# This file is only meant to be included by library/Makefile in Mbed TLS and
# is unlikely to work in another context.

ifeq (,$(BUILDING_LIBTESTDRIVER1))
# List the generated files from crypto that are needed in the build,
# because we don't have the list in a consumable form.
TF_PSA_CRYPTO_LIBRARY_GENERATED_FILES := \
	$(TF_PSA_CRYPTO_CORE_PATH)/psa_crypto_driver_wrappers.h \
	$(TF_PSA_CRYPTO_CORE_PATH)/psa_crypto_driver_wrappers_no_static.c \
	$(TF_PSA_CRYPTO_CORE_PATH)/tf_psa_crypto_config_check_before.h \
	$(TF_PSA_CRYPTO_CORE_PATH)/tf_psa_crypto_config_check_final.h \
	$(TF_PSA_CRYPTO_CORE_PATH)/tf_psa_crypto_config_check_user.h

GENERATED_WRAPPER_FILES = \
                    $(TF_PSA_CRYPTO_CORE_PATH)/psa_crypto_driver_wrappers.h \
                    $(TF_PSA_CRYPTO_CORE_PATH)/psa_crypto_driver_wrappers_no_static.c
$(GENERATED_WRAPPER_FILES): ../tf-psa-crypto/scripts/generate_driver_wrappers.py
$(GENERATED_WRAPPER_FILES): ../tf-psa-crypto/scripts/data_files/driver_templates/psa_crypto_driver_wrappers.h.jinja
$(GENERATED_WRAPPER_FILES): ../tf-psa-crypto/scripts/data_files/driver_templates/psa_crypto_driver_wrappers_no_static.c.jinja
$(GENERATED_WRAPPER_FILES):
	echo "  Gen   $(GENERATED_WRAPPER_FILES)"
	$(PYTHON) ../tf-psa-crypto/scripts/generate_driver_wrappers.py $(TF_PSA_CRYPTO_CORE_PATH)

$(TF_PSA_CRYPTO_CORE_PATH)/psa_crypto.o:$(TF_PSA_CRYPTO_CORE_PATH)/psa_crypto_driver_wrappers.h

TF_PSA_CRYPTO_GENERATED_CONFIG_CHECK_FILES = $(shell $(PYTHON) \
	$(TF_PSA_CRYPTO_CORE_PATH)/../scripts/generate_config_checks.py \
	--list $(TF_PSA_CRYPTO_CORE_PATH))
$(TF_PSA_CRYPTO_GENERATED_CONFIG_CHECK_FILES): $(gen_file_dep) \
	$(TF_PSA_CRYPTO_CORE_PATH)/../scripts/generate_config_checks.py \
	../framework/scripts/mbedtls_framework/config_checks_generator.py
$(TF_PSA_CRYPTO_GENERATED_CONFIG_CHECK_FILES):
	echo "  Gen   $(TF_PSA_CRYPTO_GENERATED_CONFIG_CHECK_FILES)"
	$(PYTHON) $(TF_PSA_CRYPTO_CORE_PATH)/../scripts/generate_config_checks.py

$(TF_PSA_CRYPTO_CORE_PATH)/tf_psa_crypto_config.o: $(TF_PSA_CRYPTO_GENERATED_CONFIG_CHECK_FILES)

else # BUILDING_LIBTESTDRIVER1 empty?
# When building libtestdriver1, assume that the generated files are present
# and up-to-date. This way, we don't need the file generation scripts to
# work in the libtestdriver1 tree, where they might not find all the data
# files that they need.
TF_PSA_CRYPTO_LIBRARY_GENERATED_FILES :=
endif # BUILDING_LIBTESTDRIVER1 empty?

TF_PSA_CRYPTO_LIBRARY_OBJS := $(patsubst %.c, %.o,$(wildcard \
                    $(TF_PSA_CRYPTO_CORE_PATH)/*.c \
                    $(TF_PSA_CRYPTO_DRIVERS_BUILTIN_SRC_PATH)/*.c \
                    $(TF_PSA_CRYPTO_PATH)/dispatch/*.c \
                    $(TF_PSA_CRYPTO_PATH)/extras/*.c \
                    $(TF_PSA_CRYPTO_PATH)/platform/*.c \
                    $(TF_PSA_CRYPTO_PATH)/utilities/*.c))
TF_PSA_CRYPTO_LIBRARY_GENERATED_OBJS = $(TF_PSA_CRYPTO_CORE_PATH)/psa_crypto_driver_wrappers_no_static.o
TF_PSA_CRYPTO_LIBRARY_OBJS := $(filter-out $(TF_PSA_CRYPTO_LIBRARY_GENERATED_OBJS),$(TF_PSA_CRYPTO_LIBRARY_OBJS))
TF_PSA_CRYPTO_LIBRARY_OBJS += $(TF_PSA_CRYPTO_LIBRARY_GENERATED_OBJS)
TF_PSA_CRYPTO_LIBRARY_OBJS+=$(THIRDPARTY_CRYPTO_OBJECTS)
