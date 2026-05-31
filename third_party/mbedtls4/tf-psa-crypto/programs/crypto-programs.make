# Helper code for programs/Makefile in Mbed TLS.
# This file is only meant to be included by programs/Makefile in Mbed TLS and
# is unlikely to work in another context.

TF_PSA_CRYPTO_PROGRAMS_GENERATED_FILES := \
	$(TF_PSA_CRYPTO_PATH)/programs/psa/psa_constant_names_generated.c

../tf-psa-crypto/programs/psa/psa_constant_names_generated.c: $(gen_file_dep) ../tf-psa-crypto/scripts/generate_psa_constants.py
../tf-psa-crypto/programs/psa/psa_constant_names_generated.c: $(gen_file_dep) ../tf-psa-crypto/include/psa/crypto_values.h
../tf-psa-crypto/programs/psa/psa_constant_names_generated.c: $(gen_file_dep) ../tf-psa-crypto/include/psa/crypto_extra.h
../tf-psa-crypto/programs/psa/psa_constant_names_generated.c: $(gen_file_dep) ../tf-psa-crypto/tests/suites/test_suite_psa_crypto_metadata.data
../tf-psa-crypto/programs/psa/psa_constant_names_generated.c:
	echo "  Gen   $@"
	cd ../tf-psa-crypto; $(PYTHON) ./scripts/generate_psa_constants.py

TF_PSA_CRYPTO_APPS := \
	$(TF_PSA_CRYPTO_PATH)/programs/psa/aead_demo \
	$(TF_PSA_CRYPTO_PATH)/programs/psa/crypto_examples \
	$(TF_PSA_CRYPTO_PATH)/programs/psa/hmac_demo \
	$(TF_PSA_CRYPTO_PATH)/programs/psa/key_ladder_demo \
	$(TF_PSA_CRYPTO_PATH)/programs/psa/psa_constant_names \
	$(TF_PSA_CRYPTO_PATH)/programs/psa/psa_hash \
	$(TF_PSA_CRYPTO_PATH)/programs/test/which_aes \
# End of APPS

../tf-psa-crypto/programs/psa/aead_demo$(EXEXT): ../tf-psa-crypto/programs/psa/aead_demo.c $(DEP)
	echo "  CC    psa/aead_demo.c"
	$(CC) $(LOCAL_CFLAGS) $(CFLAGS) ../tf-psa-crypto/programs/psa/aead_demo.c    $(LOCAL_LDFLAGS) $(LDFLAGS) -o $@

../tf-psa-crypto/programs/psa/crypto_examples$(EXEXT): ../tf-psa-crypto/programs/psa/crypto_examples.c $(DEP)
	echo "  CC    psa/crypto_examples.c"
	$(CC) $(LOCAL_CFLAGS) $(CFLAGS) ../tf-psa-crypto/programs/psa/crypto_examples.c    $(LOCAL_LDFLAGS) $(LDFLAGS) -o $@

../tf-psa-crypto/programs/psa/hmac_demo$(EXEXT): ../tf-psa-crypto/programs/psa/hmac_demo.c $(DEP)
	echo "  CC    psa/hmac_demo.c"
	$(CC) $(LOCAL_CFLAGS) $(CFLAGS) ../tf-psa-crypto/programs/psa/hmac_demo.c    $(LOCAL_LDFLAGS) $(LDFLAGS) -o $@

../tf-psa-crypto/programs/psa/key_ladder_demo$(EXEXT): ../tf-psa-crypto/programs/psa/key_ladder_demo.c $(DEP)
	echo "  CC    psa/key_ladder_demo.c"
	$(CC) $(LOCAL_CFLAGS) $(CFLAGS) ../tf-psa-crypto/programs/psa/key_ladder_demo.c    $(LOCAL_LDFLAGS) $(LDFLAGS) -o $@

../tf-psa-crypto/programs/psa/psa_constant_names$(EXEXT): ../tf-psa-crypto/programs/psa/psa_constant_names.c ../tf-psa-crypto/programs/psa/psa_constant_names_generated.c $(DEP)
	echo "  CC    psa/psa_constant_names.c"
	$(CC) $(LOCAL_CFLAGS) $(CFLAGS) ../tf-psa-crypto/programs/psa/psa_constant_names.c    $(LOCAL_LDFLAGS) $(LDFLAGS) -o $@

../tf-psa-crypto/programs/psa/psa_hash$(EXEXT): ../tf-psa-crypto/programs/psa/psa_hash.c $(DEP)
	echo "  CC    psa/psa_hash.c"
	$(CC) $(LOCAL_CFLAGS) $(CFLAGS) ../tf-psa-crypto/programs/psa/psa_hash.c    $(LOCAL_LDFLAGS) $(LDFLAGS) -o $@

../tf-psa-crypto/programs/test/which_aes$(EXEXT): ../tf-psa-crypto/programs/test/which_aes.c $(DEP)
	echo "  CC    test/which_aes.c"
	$(CC) $(LOCAL_CFLAGS) $(CFLAGS) ../tf-psa-crypto/programs/test/which_aes.c    $(LOCAL_LDFLAGS) $(LDFLAGS) -o $@
