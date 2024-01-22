#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#── vi: set noet ft=make ts=8 sw=8 fenc=utf-8 :vi ────────────────────┘

PKGS += THIRD_PARTY_MBEDTLS_TEST

THIRD_PARTY_MBEDTLS_TEST_FILES := $(wildcard third_party/mbedtls/test/*)
THIRD_PARTY_MBEDTLS_TEST_SRCS = $(filter %.c,$(THIRD_PARTY_MBEDTLS_TEST_FILES))
THIRD_PARTY_MBEDTLS_TEST_HDRS = $(filter %.h,$(THIRD_PARTY_MBEDTLS_TEST_FILES))
THIRD_PARTY_MBEDTLS_TEST_INCS = $(filter %.inc,$(THIRD_PARTY_MBEDTLS_TEST_FILES))

THIRD_PARTY_MBEDTLS_TEST_OBJS =											\
	$(THIRD_PARTY_MBEDTLS_TEST_SRCS:%.c=o/$(MODE)/%.o)

# TOOD(jart): Re-enable me once we can mock out time.
# o/$(MODE)/third_party/mbedtls/test/test_suite_x509parse.com

THIRD_PARTY_MBEDTLS_TEST_COMS =											\
	o/$(MODE)/third_party/mbedtls/test/test_suite_aes.cbc.com						\
	o/$(MODE)/third_party/mbedtls/test/test_suite_aes.cfb.com						\
	o/$(MODE)/third_party/mbedtls/test/test_suite_aes.ecb.com						\
	o/$(MODE)/third_party/mbedtls/test/test_suite_aes.ofb.com						\
	o/$(MODE)/third_party/mbedtls/test/test_suite_aes.rest.com						\
	o/$(MODE)/third_party/mbedtls/test/test_suite_aes.xts.com						\
	o/$(MODE)/third_party/mbedtls/test/test_suite_asn1parse.com						\
	o/$(MODE)/third_party/mbedtls/test/test_suite_asn1write.com						\
	o/$(MODE)/third_party/mbedtls/test/test_suite_base64.com						\
	o/$(MODE)/third_party/mbedtls/test/test_suite_blowfish.com						\
	o/$(MODE)/third_party/mbedtls/test/test_suite_chacha20.com						\
	o/$(MODE)/third_party/mbedtls/test/test_suite_chachapoly.com						\
	o/$(MODE)/third_party/mbedtls/test/test_suite_cipher.aes.com						\
	o/$(MODE)/third_party/mbedtls/test/test_suite_cipher.blowfish.com					\
	o/$(MODE)/third_party/mbedtls/test/test_suite_cipher.ccm.com						\
	o/$(MODE)/third_party/mbedtls/test/test_suite_cipher.chacha20.com					\
	o/$(MODE)/third_party/mbedtls/test/test_suite_cipher.chachapoly.com					\
	o/$(MODE)/third_party/mbedtls/test/test_suite_cipher.des.com						\
	o/$(MODE)/third_party/mbedtls/test/test_suite_cipher.gcm.com						\
	o/$(MODE)/third_party/mbedtls/test/test_suite_cipher.misc.com						\
	o/$(MODE)/third_party/mbedtls/test/test_suite_cipher.nist_kw.com					\
	o/$(MODE)/third_party/mbedtls/test/test_suite_cipher.null.com						\
	o/$(MODE)/third_party/mbedtls/test/test_suite_cipher.padding.com					\
	o/$(MODE)/third_party/mbedtls/test/test_suite_ctr_drbg.com						\
	o/$(MODE)/third_party/mbedtls/test/test_suite_des.com							\
	o/$(MODE)/third_party/mbedtls/test/test_suite_dhm.com							\
	o/$(MODE)/third_party/mbedtls/test/test_suite_ecdh.com							\
	o/$(MODE)/third_party/mbedtls/test/test_suite_ecdsa.com							\
	o/$(MODE)/third_party/mbedtls/test/test_suite_ecp.com							\
	o/$(MODE)/third_party/mbedtls/test/test_suite_entropy.com						\
	o/$(MODE)/third_party/mbedtls/test/test_suite_error.com							\
	o/$(MODE)/third_party/mbedtls/test/test_suite_gcm.aes128_de.com						\
	o/$(MODE)/third_party/mbedtls/test/test_suite_gcm.aes128_en.com						\
	o/$(MODE)/third_party/mbedtls/test/test_suite_gcm.aes192_de.com						\
	o/$(MODE)/third_party/mbedtls/test/test_suite_gcm.aes192_en.com						\
	o/$(MODE)/third_party/mbedtls/test/test_suite_gcm.aes256_de.com						\
	o/$(MODE)/third_party/mbedtls/test/test_suite_gcm.aes256_en.com						\
	o/$(MODE)/third_party/mbedtls/test/test_suite_gcm.misc.com						\
	o/$(MODE)/third_party/mbedtls/test/test_suite_hkdf.com							\
	o/$(MODE)/third_party/mbedtls/test/test_suite_hmac_drbg.misc.com					\
	o/$(MODE)/third_party/mbedtls/test/test_suite_hmac_drbg.no_reseed.com					\
	o/$(MODE)/third_party/mbedtls/test/test_suite_hmac_drbg.nopr.com					\
	o/$(MODE)/third_party/mbedtls/test/test_suite_hmac_drbg.pr.com						\
	o/$(MODE)/third_party/mbedtls/test/test_suite_md.com							\
	o/$(MODE)/third_party/mbedtls/test/test_suite_mdx.com							\
	o/$(MODE)/third_party/mbedtls/test/test_suite_memory_buffer_alloc.com					\
	o/$(MODE)/third_party/mbedtls/test/test_suite_mpi.com							\
	o/$(MODE)/third_party/mbedtls/test/test_suite_net.com							\
	o/$(MODE)/third_party/mbedtls/test/test_suite_nist_kw.com						\
	o/$(MODE)/third_party/mbedtls/test/test_suite_oid.com							\
	o/$(MODE)/third_party/mbedtls/test/test_suite_pem.com							\
	o/$(MODE)/third_party/mbedtls/test/test_suite_pk.com							\
	o/$(MODE)/third_party/mbedtls/test/test_suite_pkcs1_v15.com						\
	o/$(MODE)/third_party/mbedtls/test/test_suite_pkcs1_v21.com						\
	o/$(MODE)/third_party/mbedtls/test/test_suite_pkcs5.com							\
	o/$(MODE)/third_party/mbedtls/test/test_suite_pkparse.com						\
	o/$(MODE)/third_party/mbedtls/test/test_suite_pkwrite.com						\
	o/$(MODE)/third_party/mbedtls/test/test_suite_poly1305.com						\
	o/$(MODE)/third_party/mbedtls/test/test_suite_random.com						\
	o/$(MODE)/third_party/mbedtls/test/test_suite_rsa.com							\
	o/$(MODE)/third_party/mbedtls/test/test_suite_shax.com							\
	o/$(MODE)/third_party/mbedtls/test/test_suite_ssl.com							\
	o/$(MODE)/third_party/mbedtls/test/test_suite_timing.com						\
	o/$(MODE)/third_party/mbedtls/test/test_suite_version.com						\
	o/$(MODE)/third_party/mbedtls/test/test_suite_x509write.com						\
	o/$(MODE)/third_party/mbedtls/test/secp384r1_test.com							\
	o/$(MODE)/third_party/mbedtls/test/everest_test.com

THIRD_PARTY_MBEDTLS_TEST_TESTS =										\
	$(THIRD_PARTY_MBEDTLS_TEST_COMS:%=%.ok)

THIRD_PARTY_CHIBICC_TEST_BINS =											\
	$(THIRD_PARTY_CHIBICC_TEST_COMS)									\
	$(THIRD_PARTY_CHIBICC_TEST_COMS:%=%.dbg)

THIRD_PARTY_MBEDTLS_TEST_CHECKS =										\
	$(THIRD_PARTY_MBEDTLS_TEST_COMS:%=%.runs)								\
	$(THIRD_PARTY_MBEDTLS_TEST_HDRS:%=o/$(MODE)/%.ok)

THIRD_PARTY_MBEDTLS_TEST_DIRECTDEPS =										\
	LIBC_CALLS												\
	LIBC_FMT												\
	LIBC_INTRIN												\
	LIBC_LOG												\
	LIBC_MEM												\
	LIBC_NEXGEN32E												\
	LIBC_NT_KERNEL32											\
	LIBC_RUNTIME												\
	LIBC_STDIO												\
	LIBC_STR												\
	LIBC_SYSV												\
	LIBC_TIME												\
	LIBC_TESTLIB												\
	LIBC_X													\
	THIRD_PARTY_COMPILER_RT											\
	THIRD_PARTY_GDTOA											\
	THIRD_PARTY_MBEDTLS											\
	THIRD_PARTY_MUSL

THIRD_PARTY_MBEDTLS_TEST_DEPS :=										\
	$(call uniq,$(foreach x,$(THIRD_PARTY_MBEDTLS_TEST_DIRECTDEPS),$($(x))))

o/$(MODE)/third_party/mbedtls/test/test.pkg:									\
		$(THIRD_PARTY_MBEDTLS_TEST_OBJS)								\
		$(foreach x,$(THIRD_PARTY_MBEDTLS_TEST_DIRECTDEPS),$($(x)_A).pkg)

o/$(MODE)/third_party/mbedtls/test/lib.o: third_party/mbedtls/test/lib.c

o/$(MODE)/third_party/mbedtls/test/%.com.dbg:									\
		$(THIRD_PARTY_MBEDTLS_TEST_DEPS)								\
		o/$(MODE)/third_party/mbedtls/test/lib.o							\
		o/$(MODE)/third_party/mbedtls/test/%.o								\
		o/$(MODE)/third_party/mbedtls/test/test.pkg							\
		$(CRT)												\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

o/$(MODE)/third_party/mbedtls/test/%.com.runs: o/$(MODE)/third_party/mbedtls/test/%.com
	@$(COMPILE) -ACHECK -wtT$@ $< $(TESTARGS)

o/$(MODE)/third_party/mbedtls/test/lib.o: private								\
			CFLAGS +=										\
				-fdata-sections									\
				-ffunction-sections

.PHONY: o/$(MODE)/third_party/mbedtls/test
.PRECIOUS: $(THIRD_PARTY_CHIBICC_TEST_COMS:%=%.dbg)
o/$(MODE)/third_party/mbedtls/test:										\
		$(THIRD_PARTY_MBEDTLS_TEST_CHECKS)

.PHONY: o/$(MODE)/third_party/mbedtls/test/TESTS
o/$(MODE)/third_party/mbedtls/test/TESTS:									\
		$(THIRD_PARTY_MBEDTLS_TEST_TESTS)

o/$(MODE)/third_party/mbedtls/test/test_suite_aes.cbc.com: o/$(MODE)/third_party/mbedtls/test/test_suite_aes.cbc.com.dbg
o/$(MODE)/third_party/mbedtls/test/test_suite_aes.cbc.com.dbg:							\
		$(THIRD_PARTY_MBEDTLS_TEST_DEPS)								\
		o/$(MODE)/third_party/mbedtls/test/lib.o							\
		o/$(MODE)/third_party/mbedtls/test/test_suite_aes.cbc.o						\
		o/$(MODE)/third_party/mbedtls/test/test_suite_aes.cbc.datax.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/test.pkg							\
		$(CRT)												\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

o/$(MODE)/third_party/mbedtls/test/test_suite_aes.cfb.com: o/$(MODE)/third_party/mbedtls/test/test_suite_aes.cfb.com.dbg
o/$(MODE)/third_party/mbedtls/test/test_suite_aes.cfb.com.dbg:							\
		$(THIRD_PARTY_MBEDTLS_TEST_DEPS)								\
		o/$(MODE)/third_party/mbedtls/test/lib.o							\
		o/$(MODE)/third_party/mbedtls/test/test_suite_aes.cfb.o						\
		o/$(MODE)/third_party/mbedtls/test/test_suite_aes.cfb.datax.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/test.pkg							\
		$(CRT)												\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

o/$(MODE)/third_party/mbedtls/test/test_suite_aes.ecb.com: o/$(MODE)/third_party/mbedtls/test/test_suite_aes.ecb.com.dbg
o/$(MODE)/third_party/mbedtls/test/test_suite_aes.ecb.com.dbg:							\
		$(THIRD_PARTY_MBEDTLS_TEST_DEPS)								\
		o/$(MODE)/third_party/mbedtls/test/lib.o							\
		o/$(MODE)/third_party/mbedtls/test/test_suite_aes.ecb.o						\
		o/$(MODE)/third_party/mbedtls/test/test_suite_aes.ecb.datax.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/test.pkg							\
		$(CRT)												\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

o/$(MODE)/third_party/mbedtls/test/test_suite_aes.ofb.com: o/$(MODE)/third_party/mbedtls/test/test_suite_aes.ofb.com.dbg
o/$(MODE)/third_party/mbedtls/test/test_suite_aes.ofb.com.dbg:							\
		$(THIRD_PARTY_MBEDTLS_TEST_DEPS)								\
		o/$(MODE)/third_party/mbedtls/test/lib.o							\
		o/$(MODE)/third_party/mbedtls/test/test_suite_aes.ofb.o						\
		o/$(MODE)/third_party/mbedtls/test/test_suite_aes.ofb.datax.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/test.pkg							\
		$(CRT)												\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

o/$(MODE)/third_party/mbedtls/test/test_suite_aes.rest.com: o/$(MODE)/third_party/mbedtls/test/test_suite_aes.rest.com.dbg
o/$(MODE)/third_party/mbedtls/test/test_suite_aes.rest.com.dbg:							\
		$(THIRD_PARTY_MBEDTLS_TEST_DEPS)								\
		o/$(MODE)/third_party/mbedtls/test/lib.o							\
		o/$(MODE)/third_party/mbedtls/test/test_suite_aes.rest.o					\
		o/$(MODE)/third_party/mbedtls/test/test_suite_aes.rest.datax.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/test.pkg							\
		$(CRT)												\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

o/$(MODE)/third_party/mbedtls/test/test_suite_aes.xts.com: o/$(MODE)/third_party/mbedtls/test/test_suite_aes.xts.com.dbg
o/$(MODE)/third_party/mbedtls/test/test_suite_aes.xts.com.dbg:							\
		$(THIRD_PARTY_MBEDTLS_TEST_DEPS)								\
		o/$(MODE)/third_party/mbedtls/test/lib.o							\
		o/$(MODE)/third_party/mbedtls/test/test_suite_aes.xts.o						\
		o/$(MODE)/third_party/mbedtls/test/test_suite_aes.xts.datax.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/test.pkg							\
		$(CRT)												\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

o/$(MODE)/third_party/mbedtls/test/test_suite_asn1parse.com: o/$(MODE)/third_party/mbedtls/test/test_suite_asn1parse.com.dbg
o/$(MODE)/third_party/mbedtls/test/test_suite_asn1parse.com.dbg:						\
		$(THIRD_PARTY_MBEDTLS_TEST_DEPS)								\
		o/$(MODE)/third_party/mbedtls/test/lib.o							\
		o/$(MODE)/third_party/mbedtls/test/test_suite_asn1parse.o					\
		o/$(MODE)/third_party/mbedtls/test/test_suite_asn1parse.datax.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/test.pkg							\
		$(CRT)												\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

o/$(MODE)/third_party/mbedtls/test/test_suite_asn1write.com: o/$(MODE)/third_party/mbedtls/test/test_suite_asn1write.com.dbg
o/$(MODE)/third_party/mbedtls/test/test_suite_asn1write.com.dbg:						\
		$(THIRD_PARTY_MBEDTLS_TEST_DEPS)								\
		o/$(MODE)/third_party/mbedtls/test/lib.o							\
		o/$(MODE)/third_party/mbedtls/test/test_suite_asn1write.o					\
		o/$(MODE)/third_party/mbedtls/test/test_suite_asn1write.datax.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/test.pkg							\
		$(CRT)												\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

o/$(MODE)/third_party/mbedtls/test/test_suite_base64.com: o/$(MODE)/third_party/mbedtls/test/test_suite_base64.com.dbg
o/$(MODE)/third_party/mbedtls/test/test_suite_base64.com.dbg:							\
		$(THIRD_PARTY_MBEDTLS_TEST_DEPS)								\
		o/$(MODE)/third_party/mbedtls/test/lib.o							\
		o/$(MODE)/third_party/mbedtls/test/test_suite_base64.o						\
		o/$(MODE)/third_party/mbedtls/test/test_suite_base64.datax.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/test.pkg							\
		$(CRT)												\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

o/$(MODE)/third_party/mbedtls/test/test_suite_blowfish.com: o/$(MODE)/third_party/mbedtls/test/test_suite_blowfish.com.dbg
o/$(MODE)/third_party/mbedtls/test/test_suite_blowfish.com.dbg:							\
		$(THIRD_PARTY_MBEDTLS_TEST_DEPS)								\
		o/$(MODE)/third_party/mbedtls/test/lib.o							\
		o/$(MODE)/third_party/mbedtls/test/test_suite_blowfish.o					\
		o/$(MODE)/third_party/mbedtls/test/test_suite_blowfish.datax.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/test.pkg							\
		$(CRT)												\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

o/$(MODE)/third_party/mbedtls/test/test_suite_chacha20.com: o/$(MODE)/third_party/mbedtls/test/test_suite_chacha20.com.dbg
o/$(MODE)/third_party/mbedtls/test/test_suite_chacha20.com.dbg:							\
		$(THIRD_PARTY_MBEDTLS_TEST_DEPS)								\
		o/$(MODE)/third_party/mbedtls/test/lib.o							\
		o/$(MODE)/third_party/mbedtls/test/test_suite_chacha20.o					\
		o/$(MODE)/third_party/mbedtls/test/test_suite_chacha20.datax.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/test.pkg							\
		$(CRT)												\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

o/$(MODE)/third_party/mbedtls/test/test_suite_chachapoly.com: o/$(MODE)/third_party/mbedtls/test/test_suite_chachapoly.com.dbg
o/$(MODE)/third_party/mbedtls/test/test_suite_chachapoly.com.dbg:						\
		$(THIRD_PARTY_MBEDTLS_TEST_DEPS)								\
		o/$(MODE)/third_party/mbedtls/test/lib.o							\
		o/$(MODE)/third_party/mbedtls/test/test_suite_chachapoly.o					\
		o/$(MODE)/third_party/mbedtls/test/test_suite_chachapoly.datax.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/test.pkg							\
		$(CRT)												\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

o/$(MODE)/third_party/mbedtls/test/test_suite_cipher.aes.com: o/$(MODE)/third_party/mbedtls/test/test_suite_cipher.aes.com.dbg
o/$(MODE)/third_party/mbedtls/test/test_suite_cipher.aes.com.dbg:						\
		$(THIRD_PARTY_MBEDTLS_TEST_DEPS)								\
		o/$(MODE)/third_party/mbedtls/test/lib.o							\
		o/$(MODE)/third_party/mbedtls/test/test_suite_cipher.aes.o					\
		o/$(MODE)/third_party/mbedtls/test/test_suite_cipher.aes.datax.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/test.pkg							\
		$(CRT)												\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

o/$(MODE)/third_party/mbedtls/test/test_suite_cipher.blowfish.com: o/$(MODE)/third_party/mbedtls/test/test_suite_cipher.blowfish.com.dbg
o/$(MODE)/third_party/mbedtls/test/test_suite_cipher.blowfish.com.dbg:						\
		$(THIRD_PARTY_MBEDTLS_TEST_DEPS)								\
		o/$(MODE)/third_party/mbedtls/test/lib.o							\
		o/$(MODE)/third_party/mbedtls/test/test_suite_cipher.blowfish.o					\
		o/$(MODE)/third_party/mbedtls/test/test_suite_cipher.blowfish.datax.zip.o			\
		o/$(MODE)/third_party/mbedtls/test/test.pkg							\
		$(CRT)												\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

o/$(MODE)/third_party/mbedtls/test/test_suite_cipher.ccm.com: o/$(MODE)/third_party/mbedtls/test/test_suite_cipher.ccm.com.dbg
o/$(MODE)/third_party/mbedtls/test/test_suite_cipher.ccm.com.dbg:						\
		$(THIRD_PARTY_MBEDTLS_TEST_DEPS)								\
		o/$(MODE)/third_party/mbedtls/test/lib.o							\
		o/$(MODE)/third_party/mbedtls/test/test_suite_cipher.ccm.o					\
		o/$(MODE)/third_party/mbedtls/test/test_suite_cipher.ccm.datax.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/test.pkg							\
		$(CRT)												\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

o/$(MODE)/third_party/mbedtls/test/test_suite_cipher.chacha20.com: o/$(MODE)/third_party/mbedtls/test/test_suite_cipher.chacha20.com.dbg
o/$(MODE)/third_party/mbedtls/test/test_suite_cipher.chacha20.com.dbg:						\
		$(THIRD_PARTY_MBEDTLS_TEST_DEPS)								\
		o/$(MODE)/third_party/mbedtls/test/lib.o							\
		o/$(MODE)/third_party/mbedtls/test/test_suite_cipher.chacha20.o					\
		o/$(MODE)/third_party/mbedtls/test/test_suite_cipher.chacha20.datax.zip.o			\
		o/$(MODE)/third_party/mbedtls/test/test.pkg							\
		$(CRT)												\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

o/$(MODE)/third_party/mbedtls/test/test_suite_cipher.chachapoly.com: o/$(MODE)/third_party/mbedtls/test/test_suite_cipher.chachapoly.com.dbg
o/$(MODE)/third_party/mbedtls/test/test_suite_cipher.chachapoly.com.dbg:					\
		$(THIRD_PARTY_MBEDTLS_TEST_DEPS)								\
		o/$(MODE)/third_party/mbedtls/test/lib.o							\
		o/$(MODE)/third_party/mbedtls/test/test_suite_cipher.chachapoly.o				\
		o/$(MODE)/third_party/mbedtls/test/test_suite_cipher.chachapoly.datax.zip.o			\
		o/$(MODE)/third_party/mbedtls/test/test.pkg							\
		$(CRT)												\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

o/$(MODE)/third_party/mbedtls/test/test_suite_cipher.des.com: o/$(MODE)/third_party/mbedtls/test/test_suite_cipher.des.com.dbg
o/$(MODE)/third_party/mbedtls/test/test_suite_cipher.des.com.dbg:						\
		$(THIRD_PARTY_MBEDTLS_TEST_DEPS)								\
		o/$(MODE)/third_party/mbedtls/test/lib.o							\
		o/$(MODE)/third_party/mbedtls/test/test_suite_cipher.des.o					\
		o/$(MODE)/third_party/mbedtls/test/test_suite_cipher.des.datax.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/test.pkg							\
		$(CRT)												\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

o/$(MODE)/third_party/mbedtls/test/test_suite_cipher.gcm.com: o/$(MODE)/third_party/mbedtls/test/test_suite_cipher.gcm.com.dbg
o/$(MODE)/third_party/mbedtls/test/test_suite_cipher.gcm.com.dbg:						\
		$(THIRD_PARTY_MBEDTLS_TEST_DEPS)								\
		o/$(MODE)/third_party/mbedtls/test/lib.o							\
		o/$(MODE)/third_party/mbedtls/test/test_suite_cipher.gcm.o					\
		o/$(MODE)/third_party/mbedtls/test/test_suite_cipher.gcm.datax.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/test.pkg							\
		$(CRT)												\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

o/$(MODE)/third_party/mbedtls/test/test_suite_cipher.misc.com: o/$(MODE)/third_party/mbedtls/test/test_suite_cipher.misc.com.dbg
o/$(MODE)/third_party/mbedtls/test/test_suite_cipher.misc.com.dbg:						\
		$(THIRD_PARTY_MBEDTLS_TEST_DEPS)								\
		o/$(MODE)/third_party/mbedtls/test/lib.o							\
		o/$(MODE)/third_party/mbedtls/test/test_suite_cipher.misc.o					\
		o/$(MODE)/third_party/mbedtls/test/test_suite_cipher.misc.datax.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/test.pkg							\
		$(CRT)												\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

o/$(MODE)/third_party/mbedtls/test/test_suite_cipher.nist_kw.com: o/$(MODE)/third_party/mbedtls/test/test_suite_cipher.nist_kw.com.dbg
o/$(MODE)/third_party/mbedtls/test/test_suite_cipher.nist_kw.com.dbg:						\
		$(THIRD_PARTY_MBEDTLS_TEST_DEPS)								\
		o/$(MODE)/third_party/mbedtls/test/lib.o							\
		o/$(MODE)/third_party/mbedtls/test/test_suite_cipher.nist_kw.o					\
		o/$(MODE)/third_party/mbedtls/test/test_suite_cipher.nist_kw.datax.zip.o			\
		o/$(MODE)/third_party/mbedtls/test/test.pkg							\
		$(CRT)												\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

o/$(MODE)/third_party/mbedtls/test/test_suite_cipher.null.com: o/$(MODE)/third_party/mbedtls/test/test_suite_cipher.null.com.dbg
o/$(MODE)/third_party/mbedtls/test/test_suite_cipher.null.com.dbg:						\
		$(THIRD_PARTY_MBEDTLS_TEST_DEPS)								\
		o/$(MODE)/third_party/mbedtls/test/lib.o							\
		o/$(MODE)/third_party/mbedtls/test/test_suite_cipher.null.o					\
		o/$(MODE)/third_party/mbedtls/test/test_suite_cipher.null.datax.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/test.pkg							\
		$(CRT)												\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

o/$(MODE)/third_party/mbedtls/test/test_suite_cipher.padding.com: o/$(MODE)/third_party/mbedtls/test/test_suite_cipher.padding.com.dbg
o/$(MODE)/third_party/mbedtls/test/test_suite_cipher.padding.com.dbg:						\
		$(THIRD_PARTY_MBEDTLS_TEST_DEPS)								\
		o/$(MODE)/third_party/mbedtls/test/lib.o							\
		o/$(MODE)/third_party/mbedtls/test/test_suite_cipher.padding.o					\
		o/$(MODE)/third_party/mbedtls/test/test_suite_cipher.padding.datax.zip.o			\
		o/$(MODE)/third_party/mbedtls/test/test.pkg							\
		$(CRT)												\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

o/$(MODE)/third_party/mbedtls/test/test_suite_ctr_drbg.com.runs: private .UNVEIL += rwc:o/tmp
o/$(MODE)/third_party/mbedtls/test/test_suite_ctr_drbg.com: o/$(MODE)/third_party/mbedtls/test/test_suite_ctr_drbg.com.dbg
o/$(MODE)/third_party/mbedtls/test/test_suite_ctr_drbg.com.dbg:							\
		$(THIRD_PARTY_MBEDTLS_TEST_DEPS)								\
		o/$(MODE)/third_party/mbedtls/test/lib.o							\
		o/$(MODE)/third_party/mbedtls/test/test_suite_ctr_drbg.o					\
		o/$(MODE)/third_party/mbedtls/test/test_suite_ctr_drbg.datax.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/test.pkg							\
		$(CRT)												\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

o/$(MODE)/third_party/mbedtls/test/test_suite_des.com: o/$(MODE)/third_party/mbedtls/test/test_suite_des.com.dbg
o/$(MODE)/third_party/mbedtls/test/test_suite_des.com.dbg:							\
		$(THIRD_PARTY_MBEDTLS_TEST_DEPS)								\
		o/$(MODE)/third_party/mbedtls/test/lib.o							\
		o/$(MODE)/third_party/mbedtls/test/test_suite_des.o						\
		o/$(MODE)/third_party/mbedtls/test/test_suite_des.datax.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/test.pkg							\
		$(CRT)												\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

o/$(MODE)/third_party/mbedtls/test/test_suite_dhm.com: o/$(MODE)/third_party/mbedtls/test/test_suite_dhm.com.dbg
o/$(MODE)/third_party/mbedtls/test/test_suite_dhm.com.dbg:							\
		$(THIRD_PARTY_MBEDTLS_TEST_DEPS)								\
		o/$(MODE)/third_party/mbedtls/test/lib.o							\
		o/$(MODE)/third_party/mbedtls/test/test_suite_dhm.o						\
		o/$(MODE)/third_party/mbedtls/test/test_suite_dhm.datax.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/dh.optlen.pem.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/dhparams.pem.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/test.pkg							\
		$(CRT)												\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

o/$(MODE)/third_party/mbedtls/test/test_suite_ecdh.com: o/$(MODE)/third_party/mbedtls/test/test_suite_ecdh.com.dbg
o/$(MODE)/third_party/mbedtls/test/test_suite_ecdh.com.dbg:							\
		$(THIRD_PARTY_MBEDTLS_TEST_DEPS)								\
		o/$(MODE)/third_party/mbedtls/test/lib.o							\
		o/$(MODE)/third_party/mbedtls/test/test_suite_ecdh.o						\
		o/$(MODE)/third_party/mbedtls/test/test_suite_ecdh.datax.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/test.pkg							\
		$(CRT)												\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

o/$(MODE)/third_party/mbedtls/test/test_suite_ecdsa.com: o/$(MODE)/third_party/mbedtls/test/test_suite_ecdsa.com.dbg
o/$(MODE)/third_party/mbedtls/test/test_suite_ecdsa.com.dbg:							\
		$(THIRD_PARTY_MBEDTLS_TEST_DEPS)								\
		o/$(MODE)/third_party/mbedtls/test/lib.o							\
		o/$(MODE)/third_party/mbedtls/test/test_suite_ecdsa.o						\
		o/$(MODE)/third_party/mbedtls/test/test_suite_ecdsa.datax.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/test.pkg							\
		$(CRT)												\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

o/$(MODE)/third_party/mbedtls/test/test_suite_ecp.com: o/$(MODE)/third_party/mbedtls/test/test_suite_ecp.com.dbg
o/$(MODE)/third_party/mbedtls/test/test_suite_ecp.com.dbg:							\
		$(THIRD_PARTY_MBEDTLS_TEST_DEPS)								\
		o/$(MODE)/third_party/mbedtls/test/lib.o							\
		o/$(MODE)/third_party/mbedtls/test/test_suite_ecp.o						\
		o/$(MODE)/third_party/mbedtls/test/test_suite_ecp.datax.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/test.pkg							\
		$(CRT)												\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

o/$(MODE)/third_party/mbedtls/test/test_suite_entropy.com: o/$(MODE)/third_party/mbedtls/test/test_suite_entropy.com.dbg
o/$(MODE)/third_party/mbedtls/test/test_suite_entropy.com.dbg:							\
		$(THIRD_PARTY_MBEDTLS_TEST_DEPS)								\
		o/$(MODE)/third_party/mbedtls/test/lib.o							\
		o/$(MODE)/third_party/mbedtls/test/test_suite_entropy.o						\
		o/$(MODE)/third_party/mbedtls/test/test_suite_entropy.datax.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/test.pkg							\
		$(CRT)												\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

o/$(MODE)/third_party/mbedtls/test/test_suite_error.com: o/$(MODE)/third_party/mbedtls/test/test_suite_error.com.dbg
o/$(MODE)/third_party/mbedtls/test/test_suite_error.com.dbg:							\
		$(THIRD_PARTY_MBEDTLS_TEST_DEPS)								\
		o/$(MODE)/third_party/mbedtls/test/lib.o							\
		o/$(MODE)/third_party/mbedtls/test/test_suite_error.o						\
		o/$(MODE)/third_party/mbedtls/test/test_suite_error.datax.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/test.pkg							\
		$(CRT)												\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

o/$(MODE)/third_party/mbedtls/test/test_suite_gcm.aes128_de.com: o/$(MODE)/third_party/mbedtls/test/test_suite_gcm.aes128_de.com.dbg
o/$(MODE)/third_party/mbedtls/test/test_suite_gcm.aes128_de.com.dbg:						\
		$(THIRD_PARTY_MBEDTLS_TEST_DEPS)								\
		o/$(MODE)/third_party/mbedtls/test/lib.o							\
		o/$(MODE)/third_party/mbedtls/test/test_suite_gcm.aes128_de.o					\
		o/$(MODE)/third_party/mbedtls/test/test_suite_gcm.aes128_de.datax.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/test.pkg							\
		$(CRT)												\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

o/$(MODE)/third_party/mbedtls/test/test_suite_gcm.aes128_en.com: o/$(MODE)/third_party/mbedtls/test/test_suite_gcm.aes128_en.com.dbg
o/$(MODE)/third_party/mbedtls/test/test_suite_gcm.aes128_en.com.dbg:						\
		$(THIRD_PARTY_MBEDTLS_TEST_DEPS)								\
		o/$(MODE)/third_party/mbedtls/test/lib.o							\
		o/$(MODE)/third_party/mbedtls/test/test_suite_gcm.aes128_en.o					\
		o/$(MODE)/third_party/mbedtls/test/test_suite_gcm.aes128_en.datax.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/test.pkg							\
		$(CRT)												\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

o/$(MODE)/third_party/mbedtls/test/test_suite_gcm.aes192_de.com: o/$(MODE)/third_party/mbedtls/test/test_suite_gcm.aes192_de.com.dbg
o/$(MODE)/third_party/mbedtls/test/test_suite_gcm.aes192_de.com.dbg:						\
		$(THIRD_PARTY_MBEDTLS_TEST_DEPS)								\
		o/$(MODE)/third_party/mbedtls/test/lib.o							\
		o/$(MODE)/third_party/mbedtls/test/test_suite_gcm.aes192_de.o					\
		o/$(MODE)/third_party/mbedtls/test/test_suite_gcm.aes192_de.datax.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/test.pkg							\
		$(CRT)												\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

o/$(MODE)/third_party/mbedtls/test/test_suite_gcm.aes192_en.com: o/$(MODE)/third_party/mbedtls/test/test_suite_gcm.aes192_en.com.dbg
o/$(MODE)/third_party/mbedtls/test/test_suite_gcm.aes192_en.com.dbg:						\
		$(THIRD_PARTY_MBEDTLS_TEST_DEPS)								\
		o/$(MODE)/third_party/mbedtls/test/lib.o							\
		o/$(MODE)/third_party/mbedtls/test/test_suite_gcm.aes192_en.o					\
		o/$(MODE)/third_party/mbedtls/test/test_suite_gcm.aes192_en.datax.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/test.pkg							\
		$(CRT)												\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

o/$(MODE)/third_party/mbedtls/test/test_suite_gcm.aes256_de.com: o/$(MODE)/third_party/mbedtls/test/test_suite_gcm.aes256_de.com.dbg
o/$(MODE)/third_party/mbedtls/test/test_suite_gcm.aes256_de.com.dbg:						\
		$(THIRD_PARTY_MBEDTLS_TEST_DEPS)								\
		o/$(MODE)/third_party/mbedtls/test/lib.o							\
		o/$(MODE)/third_party/mbedtls/test/test_suite_gcm.aes256_de.o					\
		o/$(MODE)/third_party/mbedtls/test/test_suite_gcm.aes256_de.datax.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/test.pkg							\
		$(CRT)												\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

o/$(MODE)/third_party/mbedtls/test/test_suite_gcm.aes256_en.com: o/$(MODE)/third_party/mbedtls/test/test_suite_gcm.aes256_en.com.dbg
o/$(MODE)/third_party/mbedtls/test/test_suite_gcm.aes256_en.com.dbg:						\
		$(THIRD_PARTY_MBEDTLS_TEST_DEPS)								\
		o/$(MODE)/third_party/mbedtls/test/lib.o							\
		o/$(MODE)/third_party/mbedtls/test/test_suite_gcm.aes256_en.o					\
		o/$(MODE)/third_party/mbedtls/test/test_suite_gcm.aes256_en.datax.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/test.pkg							\
		$(CRT)												\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

o/$(MODE)/third_party/mbedtls/test/test_suite_gcm.misc.com: o/$(MODE)/third_party/mbedtls/test/test_suite_gcm.misc.com.dbg
o/$(MODE)/third_party/mbedtls/test/test_suite_gcm.misc.com.dbg:							\
		$(THIRD_PARTY_MBEDTLS_TEST_DEPS)								\
		o/$(MODE)/third_party/mbedtls/test/lib.o							\
		o/$(MODE)/third_party/mbedtls/test/test_suite_gcm.misc.o					\
		o/$(MODE)/third_party/mbedtls/test/test_suite_gcm.misc.datax.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/test.pkg							\
		$(CRT)												\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

o/$(MODE)/third_party/mbedtls/test/test_suite_hkdf.com: o/$(MODE)/third_party/mbedtls/test/test_suite_hkdf.com.dbg
o/$(MODE)/third_party/mbedtls/test/test_suite_hkdf.com.dbg:							\
		$(THIRD_PARTY_MBEDTLS_TEST_DEPS)								\
		o/$(MODE)/third_party/mbedtls/test/lib.o							\
		o/$(MODE)/third_party/mbedtls/test/test_suite_hkdf.o						\
		o/$(MODE)/third_party/mbedtls/test/test_suite_hkdf.datax.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/test.pkg							\
		$(CRT)												\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

o/$(MODE)/third_party/mbedtls/test/test_suite_hmac_drbg.misc.com.runs: private .UNVEIL += rwc:o/tmp
o/$(MODE)/third_party/mbedtls/test/test_suite_hmac_drbg.misc.com: o/$(MODE)/third_party/mbedtls/test/test_suite_hmac_drbg.misc.com.dbg
o/$(MODE)/third_party/mbedtls/test/test_suite_hmac_drbg.misc.com.dbg:						\
		$(THIRD_PARTY_MBEDTLS_TEST_DEPS)								\
		o/$(MODE)/third_party/mbedtls/test/lib.o							\
		o/$(MODE)/third_party/mbedtls/test/test_suite_hmac_drbg.misc.o					\
		o/$(MODE)/third_party/mbedtls/test/test_suite_hmac_drbg.misc.datax.zip.o			\
		o/$(MODE)/third_party/mbedtls/test/test.pkg							\
		$(CRT)												\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

o/$(MODE)/third_party/mbedtls/test/test_suite_hmac_drbg.no_reseed.com: o/$(MODE)/third_party/mbedtls/test/test_suite_hmac_drbg.no_reseed.com.dbg
o/$(MODE)/third_party/mbedtls/test/test_suite_hmac_drbg.no_reseed.com.dbg:					\
		$(THIRD_PARTY_MBEDTLS_TEST_DEPS)								\
		o/$(MODE)/third_party/mbedtls/test/lib.o							\
		o/$(MODE)/third_party/mbedtls/test/test_suite_hmac_drbg.no_reseed.o				\
		o/$(MODE)/third_party/mbedtls/test/test_suite_hmac_drbg.no_reseed.datax.zip.o			\
		o/$(MODE)/third_party/mbedtls/test/test.pkg							\
		$(CRT)												\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

o/$(MODE)/third_party/mbedtls/test/test_suite_hmac_drbg.nopr.com: o/$(MODE)/third_party/mbedtls/test/test_suite_hmac_drbg.nopr.com.dbg
o/$(MODE)/third_party/mbedtls/test/test_suite_hmac_drbg.nopr.com.dbg:						\
		$(THIRD_PARTY_MBEDTLS_TEST_DEPS)								\
		o/$(MODE)/third_party/mbedtls/test/lib.o							\
		o/$(MODE)/third_party/mbedtls/test/test_suite_hmac_drbg.nopr.o					\
		o/$(MODE)/third_party/mbedtls/test/test_suite_hmac_drbg.nopr.datax.zip.o			\
		o/$(MODE)/third_party/mbedtls/test/test.pkg							\
		$(CRT)												\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

o/$(MODE)/third_party/mbedtls/test/test_suite_hmac_drbg.pr.com: o/$(MODE)/third_party/mbedtls/test/test_suite_hmac_drbg.pr.com.dbg
o/$(MODE)/third_party/mbedtls/test/test_suite_hmac_drbg.pr.com.dbg:						\
		$(THIRD_PARTY_MBEDTLS_TEST_DEPS)								\
		o/$(MODE)/third_party/mbedtls/test/lib.o							\
		o/$(MODE)/third_party/mbedtls/test/test_suite_hmac_drbg.pr.o					\
		o/$(MODE)/third_party/mbedtls/test/test_suite_hmac_drbg.pr.datax.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/test.pkg							\
		$(CRT)												\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

o/$(MODE)/third_party/mbedtls/test/test_suite_md.com: o/$(MODE)/third_party/mbedtls/test/test_suite_md.com.dbg
o/$(MODE)/third_party/mbedtls/test/test_suite_md.com.dbg:							\
		$(THIRD_PARTY_MBEDTLS_TEST_DEPS)								\
		o/$(MODE)/third_party/mbedtls/test/lib.o							\
		o/$(MODE)/third_party/mbedtls/test/test_suite_md.o						\
		o/$(MODE)/third_party/mbedtls/test/test_suite_md.datax.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/hash_file_1.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/hash_file_2.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/hash_file_3.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/hash_file_4.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/hash_file_5.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/test.pkg							\
		$(CRT)												\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

o/$(MODE)/third_party/mbedtls/test/test_suite_mdx.com: o/$(MODE)/third_party/mbedtls/test/test_suite_mdx.com.dbg
o/$(MODE)/third_party/mbedtls/test/test_suite_mdx.com.dbg:							\
		$(THIRD_PARTY_MBEDTLS_TEST_DEPS)								\
		o/$(MODE)/third_party/mbedtls/test/lib.o							\
		o/$(MODE)/third_party/mbedtls/test/test_suite_mdx.o						\
		o/$(MODE)/third_party/mbedtls/test/test_suite_mdx.datax.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/test.pkg							\
		$(CRT)												\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

o/$(MODE)/third_party/mbedtls/test/test_suite_memory_buffer_alloc.com: o/$(MODE)/third_party/mbedtls/test/test_suite_memory_buffer_alloc.com.dbg
o/$(MODE)/third_party/mbedtls/test/test_suite_memory_buffer_alloc.com.dbg:					\
		$(THIRD_PARTY_MBEDTLS_TEST_DEPS)								\
		o/$(MODE)/third_party/mbedtls/test/lib.o							\
		o/$(MODE)/third_party/mbedtls/test/test_suite_memory_buffer_alloc.o				\
		o/$(MODE)/third_party/mbedtls/test/test_suite_memory_buffer_alloc.datax.zip.o			\
		o/$(MODE)/third_party/mbedtls/test/test.pkg							\
		$(CRT)												\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

o/$(MODE)/third_party/mbedtls/test/test_suite_mpi.com.runs: private .UNVEIL += rwc:o/tmp
o/$(MODE)/third_party/mbedtls/test/test_suite_mpi.com: o/$(MODE)/third_party/mbedtls/test/test_suite_mpi.com.dbg
o/$(MODE)/third_party/mbedtls/test/test_suite_mpi.com.dbg:							\
		$(THIRD_PARTY_MBEDTLS_TEST_DEPS)								\
		o/$(MODE)/third_party/mbedtls/test/lib.o							\
		o/$(MODE)/third_party/mbedtls/test/test_suite_mpi.o						\
		o/$(MODE)/third_party/mbedtls/test/test_suite_mpi.datax.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/hash_file_3.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/hash_file_4.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/mpi_10.zip.o						\
		o/$(MODE)/third_party/mbedtls/test/data/mpi_too_big.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/mpi_write.zip.o						\
		o/$(MODE)/third_party/mbedtls/test/test.pkg							\
		$(CRT)												\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

o/$(MODE)/third_party/mbedtls/test/test_suite_net.com: o/$(MODE)/third_party/mbedtls/test/test_suite_net.com.dbg
o/$(MODE)/third_party/mbedtls/test/test_suite_net.com.dbg:							\
		$(THIRD_PARTY_MBEDTLS_TEST_DEPS)								\
		o/$(MODE)/third_party/mbedtls/test/lib.o							\
		o/$(MODE)/third_party/mbedtls/test/test_suite_net.o						\
		o/$(MODE)/third_party/mbedtls/test/test_suite_net.datax.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/test.pkg							\
		$(CRT)												\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

o/$(MODE)/third_party/mbedtls/test/test_suite_nist_kw.com: o/$(MODE)/third_party/mbedtls/test/test_suite_nist_kw.com.dbg
o/$(MODE)/third_party/mbedtls/test/test_suite_nist_kw.com.dbg:							\
		$(THIRD_PARTY_MBEDTLS_TEST_DEPS)								\
		o/$(MODE)/third_party/mbedtls/test/lib.o							\
		o/$(MODE)/third_party/mbedtls/test/test_suite_nist_kw.o						\
		o/$(MODE)/third_party/mbedtls/test/test_suite_nist_kw.datax.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/test.pkg							\
		$(CRT)												\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

o/$(MODE)/third_party/mbedtls/test/test_suite_oid.com: o/$(MODE)/third_party/mbedtls/test/test_suite_oid.com.dbg
o/$(MODE)/third_party/mbedtls/test/test_suite_oid.com.dbg:							\
		$(THIRD_PARTY_MBEDTLS_TEST_DEPS)								\
		o/$(MODE)/third_party/mbedtls/test/lib.o							\
		o/$(MODE)/third_party/mbedtls/test/test_suite_oid.o						\
		o/$(MODE)/third_party/mbedtls/test/test_suite_oid.datax.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/test.pkg							\
		$(CRT)												\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

o/$(MODE)/third_party/mbedtls/test/test_suite_pem.com: o/$(MODE)/third_party/mbedtls/test/test_suite_pem.com.dbg
o/$(MODE)/third_party/mbedtls/test/test_suite_pem.com.dbg:							\
		$(THIRD_PARTY_MBEDTLS_TEST_DEPS)								\
		o/$(MODE)/third_party/mbedtls/test/lib.o							\
		o/$(MODE)/third_party/mbedtls/test/test_suite_pem.o						\
		o/$(MODE)/third_party/mbedtls/test/test_suite_pem.datax.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/test.pkg							\
		$(CRT)												\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

o/$(MODE)/third_party/mbedtls/test/test_suite_pk.com: o/$(MODE)/third_party/mbedtls/test/test_suite_pk.com.dbg
o/$(MODE)/third_party/mbedtls/test/test_suite_pk.com.dbg:							\
		$(THIRD_PARTY_MBEDTLS_TEST_DEPS)								\
		o/$(MODE)/third_party/mbedtls/test/lib.o							\
		o/$(MODE)/third_party/mbedtls/test/test_suite_pk.o						\
		o/$(MODE)/third_party/mbedtls/test/test_suite_pk.datax.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/ec_256_prv.pem.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/ec_256_pub.pem.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/server1.key.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/server1.pubkey.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/server2.key.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/server5.key.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/test.pkg							\
		$(CRT)												\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

o/$(MODE)/third_party/mbedtls/test/test_suite_pkcs1_v15.com: o/$(MODE)/third_party/mbedtls/test/test_suite_pkcs1_v15.com.dbg
o/$(MODE)/third_party/mbedtls/test/test_suite_pkcs1_v15.com.dbg:						\
		$(THIRD_PARTY_MBEDTLS_TEST_DEPS)								\
		o/$(MODE)/third_party/mbedtls/test/lib.o							\
		o/$(MODE)/third_party/mbedtls/test/test_suite_pkcs1_v15.o					\
		o/$(MODE)/third_party/mbedtls/test/test_suite_pkcs1_v15.datax.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/test.pkg							\
		$(CRT)												\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

o/$(MODE)/third_party/mbedtls/test/test_suite_pkcs1_v21.com: o/$(MODE)/third_party/mbedtls/test/test_suite_pkcs1_v21.com.dbg
o/$(MODE)/third_party/mbedtls/test/test_suite_pkcs1_v21.com.dbg:						\
		$(THIRD_PARTY_MBEDTLS_TEST_DEPS)								\
		o/$(MODE)/third_party/mbedtls/test/lib.o							\
		o/$(MODE)/third_party/mbedtls/test/test_suite_pkcs1_v21.o					\
		o/$(MODE)/third_party/mbedtls/test/test_suite_pkcs1_v21.datax.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/test.pkg							\
		$(CRT)												\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

o/$(MODE)/third_party/mbedtls/test/test_suite_pkcs5.com: o/$(MODE)/third_party/mbedtls/test/test_suite_pkcs5.com.dbg
o/$(MODE)/third_party/mbedtls/test/test_suite_pkcs5.com.dbg:							\
		$(THIRD_PARTY_MBEDTLS_TEST_DEPS)								\
		o/$(MODE)/third_party/mbedtls/test/lib.o							\
		o/$(MODE)/third_party/mbedtls/test/test_suite_pkcs5.o						\
		o/$(MODE)/third_party/mbedtls/test/test_suite_pkcs5.datax.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/test.pkg							\
		$(CRT)												\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

o/$(MODE)/third_party/mbedtls/test/test_suite_pkparse.com: o/$(MODE)/third_party/mbedtls/test/test_suite_pkparse.com.dbg
o/$(MODE)/third_party/mbedtls/test/test_suite_pkparse.com.dbg:							\
		$(THIRD_PARTY_MBEDTLS_TEST_DEPS)								\
		o/$(MODE)/third_party/mbedtls/test/lib.o							\
		o/$(MODE)/third_party/mbedtls/test/test_suite_pkparse.o						\
		o/$(MODE)/third_party/mbedtls/test/test_suite_pkparse.datax.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/ec_224_prv.pem.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/ec_224_pub.pem.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/ec_256_prv.pem.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/ec_256_pub.pem.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/ec_384_prv.pem.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/ec_384_pub.pem.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/ec_521_prv.pem.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/ec_521_pub.pem.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/ec_bp256_prv.pem.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/ec_bp256_pub.pem.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/ec_bp384_prv.pem.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/ec_bp384_pub.pem.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/ec_bp512_prv.pem.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/ec_bp512_pub.pem.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/ec_prv.pk8.der.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/ec_prv.pk8.pem.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/ec_prv.pk8.pw.der.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/ec_prv.pk8.pw.pem.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/ec_prv.pk8nopub.der.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/ec_prv.pk8nopub.pem.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/ec_prv.pk8nopubparam.der.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/ec_prv.pk8nopubparam.pem.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/ec_prv.pk8param.der.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/ec_prv.pk8param.pem.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/ec_prv.sec1.der.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/ec_prv.sec1.pem.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/ec_prv.sec1.pw.pem.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/ec_prv.specdom.der.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/ec_pub.der.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/ec_pub.pem.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/format_gen.key.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/rsa_pkcs1_1024_3des.pem.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/rsa_pkcs1_1024_aes128.pem.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/rsa_pkcs1_1024_aes192.pem.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/rsa_pkcs1_1024_aes256.pem.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/rsa_pkcs1_1024_des.pem.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/rsa_pkcs1_2048_3des.pem.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/rsa_pkcs1_2048_aes128.pem.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/rsa_pkcs1_2048_aes192.pem.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/rsa_pkcs1_2048_aes256.pem.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/rsa_pkcs1_2048_des.pem.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/rsa_pkcs1_2048_public.der.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/rsa_pkcs1_2048_public.pem.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/rsa_pkcs1_4096_3des.pem.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/rsa_pkcs1_4096_aes128.pem.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/rsa_pkcs1_4096_aes192.pem.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/rsa_pkcs1_4096_aes256.pem.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/rsa_pkcs1_4096_des.pem.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/rsa_pkcs8_2048_public.der.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/rsa_pkcs8_2048_public.pem.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/rsa_pkcs8_pbe_sha1_1024_2des.der.zip.o			\
		o/$(MODE)/third_party/mbedtls/test/data/rsa_pkcs8_pbe_sha1_1024_2des.pem.zip.o			\
		o/$(MODE)/third_party/mbedtls/test/data/rsa_pkcs8_pbe_sha1_1024_3des.der.zip.o			\
		o/$(MODE)/third_party/mbedtls/test/data/rsa_pkcs8_pbe_sha1_1024_3des.pem.zip.o			\
		o/$(MODE)/third_party/mbedtls/test/data/rsa_pkcs8_pbe_sha1_1024_rc4_128.der.zip.o		\
		o/$(MODE)/third_party/mbedtls/test/data/rsa_pkcs8_pbe_sha1_1024_rc4_128.pem.zip.o		\
		o/$(MODE)/third_party/mbedtls/test/data/rsa_pkcs8_pbe_sha1_2048_2des.der.zip.o			\
		o/$(MODE)/third_party/mbedtls/test/data/rsa_pkcs8_pbe_sha1_2048_2des.pem.zip.o			\
		o/$(MODE)/third_party/mbedtls/test/data/rsa_pkcs8_pbe_sha1_2048_3des.der.zip.o			\
		o/$(MODE)/third_party/mbedtls/test/data/rsa_pkcs8_pbe_sha1_2048_3des.pem.zip.o			\
		o/$(MODE)/third_party/mbedtls/test/data/rsa_pkcs8_pbe_sha1_2048_rc4_128.der.zip.o		\
		o/$(MODE)/third_party/mbedtls/test/data/rsa_pkcs8_pbe_sha1_2048_rc4_128.pem.zip.o		\
		o/$(MODE)/third_party/mbedtls/test/data/rsa_pkcs8_pbe_sha1_4096_2des.der.zip.o			\
		o/$(MODE)/third_party/mbedtls/test/data/rsa_pkcs8_pbe_sha1_4096_2des.pem.zip.o			\
		o/$(MODE)/third_party/mbedtls/test/data/rsa_pkcs8_pbe_sha1_4096_3des.der.zip.o			\
		o/$(MODE)/third_party/mbedtls/test/data/rsa_pkcs8_pbe_sha1_4096_3des.pem.zip.o			\
		o/$(MODE)/third_party/mbedtls/test/data/rsa_pkcs8_pbe_sha1_4096_rc4_128.der.zip.o		\
		o/$(MODE)/third_party/mbedtls/test/data/rsa_pkcs8_pbe_sha1_4096_rc4_128.pem.zip.o		\
		o/$(MODE)/third_party/mbedtls/test/data/rsa_pkcs8_pbes2_pbkdf2_1024_3des.der.zip.o		\
		o/$(MODE)/third_party/mbedtls/test/data/rsa_pkcs8_pbes2_pbkdf2_1024_3des.pem.zip.o		\
		o/$(MODE)/third_party/mbedtls/test/data/rsa_pkcs8_pbes2_pbkdf2_1024_3des_sha224.der.zip.o	\
		o/$(MODE)/third_party/mbedtls/test/data/rsa_pkcs8_pbes2_pbkdf2_1024_3des_sha224.pem.zip.o	\
		o/$(MODE)/third_party/mbedtls/test/data/rsa_pkcs8_pbes2_pbkdf2_1024_3des_sha256.der.zip.o	\
		o/$(MODE)/third_party/mbedtls/test/data/rsa_pkcs8_pbes2_pbkdf2_1024_3des_sha256.pem.zip.o	\
		o/$(MODE)/third_party/mbedtls/test/data/rsa_pkcs8_pbes2_pbkdf2_1024_3des_sha384.der.zip.o	\
		o/$(MODE)/third_party/mbedtls/test/data/rsa_pkcs8_pbes2_pbkdf2_1024_3des_sha384.pem.zip.o	\
		o/$(MODE)/third_party/mbedtls/test/data/rsa_pkcs8_pbes2_pbkdf2_1024_3des_sha512.der.zip.o	\
		o/$(MODE)/third_party/mbedtls/test/data/rsa_pkcs8_pbes2_pbkdf2_1024_3des_sha512.pem.zip.o	\
		o/$(MODE)/third_party/mbedtls/test/data/rsa_pkcs8_pbes2_pbkdf2_1024_des.der.zip.o		\
		o/$(MODE)/third_party/mbedtls/test/data/rsa_pkcs8_pbes2_pbkdf2_1024_des.pem.zip.o		\
		o/$(MODE)/third_party/mbedtls/test/data/rsa_pkcs8_pbes2_pbkdf2_1024_des_sha224.der.zip.o	\
		o/$(MODE)/third_party/mbedtls/test/data/rsa_pkcs8_pbes2_pbkdf2_1024_des_sha224.pem.zip.o	\
		o/$(MODE)/third_party/mbedtls/test/data/rsa_pkcs8_pbes2_pbkdf2_1024_des_sha256.der.zip.o	\
		o/$(MODE)/third_party/mbedtls/test/data/rsa_pkcs8_pbes2_pbkdf2_1024_des_sha256.pem.zip.o	\
		o/$(MODE)/third_party/mbedtls/test/data/rsa_pkcs8_pbes2_pbkdf2_1024_des_sha384.der.zip.o	\
		o/$(MODE)/third_party/mbedtls/test/data/rsa_pkcs8_pbes2_pbkdf2_1024_des_sha384.pem.zip.o	\
		o/$(MODE)/third_party/mbedtls/test/data/rsa_pkcs8_pbes2_pbkdf2_1024_des_sha512.der.zip.o	\
		o/$(MODE)/third_party/mbedtls/test/data/rsa_pkcs8_pbes2_pbkdf2_1024_des_sha512.pem.zip.o	\
		o/$(MODE)/third_party/mbedtls/test/data/rsa_pkcs8_pbes2_pbkdf2_2048_3des.der.zip.o		\
		o/$(MODE)/third_party/mbedtls/test/data/rsa_pkcs8_pbes2_pbkdf2_2048_3des.pem.zip.o		\
		o/$(MODE)/third_party/mbedtls/test/data/rsa_pkcs8_pbes2_pbkdf2_2048_3des_sha224.der.zip.o	\
		o/$(MODE)/third_party/mbedtls/test/data/rsa_pkcs8_pbes2_pbkdf2_2048_3des_sha224.pem.zip.o	\
		o/$(MODE)/third_party/mbedtls/test/data/rsa_pkcs8_pbes2_pbkdf2_2048_3des_sha256.der.zip.o	\
		o/$(MODE)/third_party/mbedtls/test/data/rsa_pkcs8_pbes2_pbkdf2_2048_3des_sha256.pem.zip.o	\
		o/$(MODE)/third_party/mbedtls/test/data/rsa_pkcs8_pbes2_pbkdf2_2048_3des_sha384.der.zip.o	\
		o/$(MODE)/third_party/mbedtls/test/data/rsa_pkcs8_pbes2_pbkdf2_2048_3des_sha384.pem.zip.o	\
		o/$(MODE)/third_party/mbedtls/test/data/rsa_pkcs8_pbes2_pbkdf2_2048_3des_sha512.der.zip.o	\
		o/$(MODE)/third_party/mbedtls/test/data/rsa_pkcs8_pbes2_pbkdf2_2048_3des_sha512.pem.zip.o	\
		o/$(MODE)/third_party/mbedtls/test/data/rsa_pkcs8_pbes2_pbkdf2_2048_des.der.zip.o		\
		o/$(MODE)/third_party/mbedtls/test/data/rsa_pkcs8_pbes2_pbkdf2_2048_des.pem.zip.o		\
		o/$(MODE)/third_party/mbedtls/test/data/rsa_pkcs8_pbes2_pbkdf2_2048_des_sha224.der.zip.o	\
		o/$(MODE)/third_party/mbedtls/test/data/rsa_pkcs8_pbes2_pbkdf2_2048_des_sha224.pem.zip.o	\
		o/$(MODE)/third_party/mbedtls/test/data/rsa_pkcs8_pbes2_pbkdf2_2048_des_sha256.der.zip.o	\
		o/$(MODE)/third_party/mbedtls/test/data/rsa_pkcs8_pbes2_pbkdf2_2048_des_sha256.pem.zip.o	\
		o/$(MODE)/third_party/mbedtls/test/data/rsa_pkcs8_pbes2_pbkdf2_2048_des_sha384.der.zip.o	\
		o/$(MODE)/third_party/mbedtls/test/data/rsa_pkcs8_pbes2_pbkdf2_2048_des_sha384.pem.zip.o	\
		o/$(MODE)/third_party/mbedtls/test/data/rsa_pkcs8_pbes2_pbkdf2_2048_des_sha512.der.zip.o	\
		o/$(MODE)/third_party/mbedtls/test/data/rsa_pkcs8_pbes2_pbkdf2_2048_des_sha512.pem.zip.o	\
		o/$(MODE)/third_party/mbedtls/test/data/rsa_pkcs8_pbes2_pbkdf2_4096_3des.der.zip.o		\
		o/$(MODE)/third_party/mbedtls/test/data/rsa_pkcs8_pbes2_pbkdf2_4096_3des.pem.zip.o		\
		o/$(MODE)/third_party/mbedtls/test/data/rsa_pkcs8_pbes2_pbkdf2_4096_3des_sha224.der.zip.o	\
		o/$(MODE)/third_party/mbedtls/test/data/rsa_pkcs8_pbes2_pbkdf2_4096_3des_sha224.pem.zip.o	\
		o/$(MODE)/third_party/mbedtls/test/data/rsa_pkcs8_pbes2_pbkdf2_4096_3des_sha256.der.zip.o	\
		o/$(MODE)/third_party/mbedtls/test/data/rsa_pkcs8_pbes2_pbkdf2_4096_3des_sha256.pem.zip.o	\
		o/$(MODE)/third_party/mbedtls/test/data/rsa_pkcs8_pbes2_pbkdf2_4096_3des_sha384.der.zip.o	\
		o/$(MODE)/third_party/mbedtls/test/data/rsa_pkcs8_pbes2_pbkdf2_4096_3des_sha384.pem.zip.o	\
		o/$(MODE)/third_party/mbedtls/test/data/rsa_pkcs8_pbes2_pbkdf2_4096_3des_sha512.der.zip.o	\
		o/$(MODE)/third_party/mbedtls/test/data/rsa_pkcs8_pbes2_pbkdf2_4096_3des_sha512.pem.zip.o	\
		o/$(MODE)/third_party/mbedtls/test/data/rsa_pkcs8_pbes2_pbkdf2_4096_des.der.zip.o		\
		o/$(MODE)/third_party/mbedtls/test/data/rsa_pkcs8_pbes2_pbkdf2_4096_des.pem.zip.o		\
		o/$(MODE)/third_party/mbedtls/test/data/rsa_pkcs8_pbes2_pbkdf2_4096_des_sha224.der.zip.o	\
		o/$(MODE)/third_party/mbedtls/test/data/rsa_pkcs8_pbes2_pbkdf2_4096_des_sha224.pem.zip.o	\
		o/$(MODE)/third_party/mbedtls/test/data/rsa_pkcs8_pbes2_pbkdf2_4096_des_sha256.der.zip.o	\
		o/$(MODE)/third_party/mbedtls/test/data/rsa_pkcs8_pbes2_pbkdf2_4096_des_sha256.pem.zip.o	\
		o/$(MODE)/third_party/mbedtls/test/data/rsa_pkcs8_pbes2_pbkdf2_4096_des_sha384.der.zip.o	\
		o/$(MODE)/third_party/mbedtls/test/data/rsa_pkcs8_pbes2_pbkdf2_4096_des_sha384.pem.zip.o	\
		o/$(MODE)/third_party/mbedtls/test/data/rsa_pkcs8_pbes2_pbkdf2_4096_des_sha512.der.zip.o	\
		o/$(MODE)/third_party/mbedtls/test/data/rsa_pkcs8_pbes2_pbkdf2_4096_des_sha512.pem.zip.o	\
		o/$(MODE)/third_party/mbedtls/test/data/test-ca.key.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/test.pkg							\
		$(CRT)												\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

o/$(MODE)/third_party/mbedtls/test/test_suite_pkwrite.com: o/$(MODE)/third_party/mbedtls/test/test_suite_pkwrite.com.dbg
o/$(MODE)/third_party/mbedtls/test/test_suite_pkwrite.com.dbg:							\
		$(THIRD_PARTY_MBEDTLS_TEST_DEPS)								\
		o/$(MODE)/third_party/mbedtls/test/lib.o							\
		o/$(MODE)/third_party/mbedtls/test/test_suite_pkwrite.o						\
		o/$(MODE)/third_party/mbedtls/test/test_suite_pkwrite.datax.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/ec_256_long_prv.pem.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/ec_521_prv.pem.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/ec_521_pub.pem.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/ec_521_short_prv.pem.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/ec_bp512_prv.pem.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/ec_bp512_pub.pem.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/ec_prv.sec1.pem.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/ec_pub.pem.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/rsa4096_prv.pem.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/rsa4096_pub.pem.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/server1.key.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/server1.pubkey.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/test.pkg							\
		$(CRT)												\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

o/$(MODE)/third_party/mbedtls/test/test_suite_poly1305.com: o/$(MODE)/third_party/mbedtls/test/test_suite_poly1305.com.dbg
o/$(MODE)/third_party/mbedtls/test/test_suite_poly1305.com.dbg:							\
		$(THIRD_PARTY_MBEDTLS_TEST_DEPS)								\
		o/$(MODE)/third_party/mbedtls/test/lib.o							\
		o/$(MODE)/third_party/mbedtls/test/test_suite_poly1305.o					\
		o/$(MODE)/third_party/mbedtls/test/test_suite_poly1305.datax.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/test.pkg							\
		$(CRT)												\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

o/$(MODE)/third_party/mbedtls/test/test_suite_random.com: o/$(MODE)/third_party/mbedtls/test/test_suite_random.com.dbg
o/$(MODE)/third_party/mbedtls/test/test_suite_random.com.dbg:							\
		$(THIRD_PARTY_MBEDTLS_TEST_DEPS)								\
		o/$(MODE)/third_party/mbedtls/test/lib.o							\
		o/$(MODE)/third_party/mbedtls/test/test_suite_random.o						\
		o/$(MODE)/third_party/mbedtls/test/test_suite_random.datax.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/test.pkg							\
		$(CRT)												\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

o/$(MODE)/third_party/mbedtls/test/test_suite_rsa.com: o/$(MODE)/third_party/mbedtls/test/test_suite_rsa.com.dbg
o/$(MODE)/third_party/mbedtls/test/test_suite_rsa.com.dbg:							\
		$(THIRD_PARTY_MBEDTLS_TEST_DEPS)								\
		o/$(MODE)/third_party/mbedtls/test/lib.o							\
		o/$(MODE)/third_party/mbedtls/test/test_suite_rsa.o						\
		o/$(MODE)/third_party/mbedtls/test/test_suite_rsa.datax.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/test.pkg							\
		$(CRT)												\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

o/$(MODE)/third_party/mbedtls/test/test_suite_shax.com: o/$(MODE)/third_party/mbedtls/test/test_suite_shax.com.dbg
o/$(MODE)/third_party/mbedtls/test/test_suite_shax.com.dbg:							\
		$(THIRD_PARTY_MBEDTLS_TEST_DEPS)								\
		o/$(MODE)/third_party/mbedtls/test/lib.o							\
		o/$(MODE)/third_party/mbedtls/test/test_suite_shax.o						\
		o/$(MODE)/third_party/mbedtls/test/test_suite_shax.datax.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/test.pkg							\
		$(CRT)												\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

o/$(MODE)/third_party/mbedtls/test/test_suite_ssl.com: o/$(MODE)/third_party/mbedtls/test/test_suite_ssl.com.dbg
o/$(MODE)/third_party/mbedtls/test/test_suite_ssl.com.dbg:							\
		$(THIRD_PARTY_MBEDTLS_TEST_DEPS)								\
		o/$(MODE)/third_party/mbedtls/test/lib.o							\
		o/$(MODE)/third_party/mbedtls/test/test_suite_ssl.o						\
		o/$(MODE)/third_party/mbedtls/test/test_suite_ssl.datax.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/server5.crt.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/test.pkg							\
		$(CRT)												\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

o/$(MODE)/third_party/mbedtls/test/test_suite_timing.com: o/$(MODE)/third_party/mbedtls/test/test_suite_timing.com.dbg
o/$(MODE)/third_party/mbedtls/test/test_suite_timing.com.dbg:							\
		$(THIRD_PARTY_MBEDTLS_TEST_DEPS)								\
		o/$(MODE)/third_party/mbedtls/test/lib.o							\
		o/$(MODE)/third_party/mbedtls/test/test_suite_timing.o						\
		o/$(MODE)/third_party/mbedtls/test/test_suite_timing.datax.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/test.pkg							\
		$(CRT)												\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

o/$(MODE)/third_party/mbedtls/test/test_suite_version.com: o/$(MODE)/third_party/mbedtls/test/test_suite_version.com.dbg
o/$(MODE)/third_party/mbedtls/test/test_suite_version.com.dbg:							\
		$(THIRD_PARTY_MBEDTLS_TEST_DEPS)								\
		o/$(MODE)/third_party/mbedtls/test/lib.o							\
		o/$(MODE)/third_party/mbedtls/test/test_suite_version.o						\
		o/$(MODE)/third_party/mbedtls/test/test_suite_version.datax.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/test.pkg							\
		$(CRT)												\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

o/$(MODE)/third_party/mbedtls/test/test_suite_x509parse.com: o/$(MODE)/third_party/mbedtls/test/test_suite_x509parse.com.dbg
o/$(MODE)/third_party/mbedtls/test/test_suite_x509parse.com.dbg:						\
		$(THIRD_PARTY_MBEDTLS_TEST_DEPS)								\
		o/$(MODE)/third_party/mbedtls/test/lib.o							\
		o/$(MODE)/third_party/mbedtls/test/test_suite_x509parse.o					\
		o/$(MODE)/third_party/mbedtls/test/test_suite_x509parse.datax.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/.zip.o							\
		o/$(MODE)/third_party/mbedtls/test/data/bitstring-in-dn.pem.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/cert_example_multi.crt.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/cert_example_multi_nocn.crt.zip.o			\
		o/$(MODE)/third_party/mbedtls/test/data/cert_example_wildcard.crt.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/cert_md2.crt.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/cert_md4.crt.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/cert_md5.crt.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/cert_sha1.crt.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/cert_sha224.crt.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/cert_sha256.crt.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/cert_sha384.crt.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/cert_sha512.crt.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/cert_v1_with_ext.crt.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/cli-rsa-sha256-badalg.crt.der.zip.o			\
		o/$(MODE)/third_party/mbedtls/test/data/crl-ec-sha1.pem.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/crl-ec-sha224.pem.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/crl-ec-sha256.pem.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/crl-ec-sha384.pem.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/crl-ec-sha512.pem.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/crl-future.pem.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/crl-futureRevocationDate.pem.zip.o			\
		o/$(MODE)/third_party/mbedtls/test/data/crl-idp.pem.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/crl-idpnc.pem.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/crl-malformed-trailing-spaces.pem.zip.o			\
		o/$(MODE)/third_party/mbedtls/test/data/crl-rsa-pss-sha1-badsign.pem.zip.o			\
		o/$(MODE)/third_party/mbedtls/test/data/crl-rsa-pss-sha1.pem.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/crl-rsa-pss-sha224.pem.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/crl-rsa-pss-sha256.pem.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/crl-rsa-pss-sha384.pem.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/crl-rsa-pss-sha512.pem.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/crl.pem.zip.o						\
		o/$(MODE)/third_party/mbedtls/test/data/crl_cat_ec-rsa.pem.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/crl_cat_ecfut-rsa.pem.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/crl_cat_rsa-ec.pem.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/crl_expired.pem.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/crl_md2.pem.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/crl_md4.pem.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/crl_md5.pem.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/crl_sha1.pem.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/crl_sha224.pem.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/crl_sha256.pem.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/crl_sha384.pem.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/crl_sha512.pem.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/dir-maxpath/.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/dir-maxpath/00.crt.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/dir-maxpath/00.key.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/dir-maxpath/01.crt.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/dir-maxpath/01.key.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/dir-maxpath/02.crt.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/dir-maxpath/02.key.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/dir-maxpath/03.crt.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/dir-maxpath/03.key.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/dir-maxpath/04.crt.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/dir-maxpath/04.key.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/dir-maxpath/05.crt.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/dir-maxpath/05.key.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/dir-maxpath/06.crt.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/dir-maxpath/06.key.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/dir-maxpath/07.crt.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/dir-maxpath/07.key.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/dir-maxpath/08.crt.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/dir-maxpath/08.key.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/dir-maxpath/09.crt.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/dir-maxpath/09.key.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/dir-maxpath/10.crt.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/dir-maxpath/10.key.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/dir-maxpath/11.crt.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/dir-maxpath/11.key.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/dir-maxpath/12.crt.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/dir-maxpath/12.key.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/dir-maxpath/13.crt.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/dir-maxpath/13.key.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/dir-maxpath/14.crt.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/dir-maxpath/14.key.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/dir-maxpath/15.crt.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/dir-maxpath/15.key.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/dir-maxpath/16.crt.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/dir-maxpath/16.key.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/dir-maxpath/17.crt.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/dir-maxpath/17.key.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/dir-maxpath/18.crt.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/dir-maxpath/18.key.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/dir-maxpath/19.crt.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/dir-maxpath/19.key.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/dir-maxpath/20.crt.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/dir-maxpath/20.key.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/dir-maxpath/Readme.txt.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/dir-maxpath/c00.pem.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/dir-maxpath/c01.pem.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/dir-maxpath/c02.pem.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/dir-maxpath/c03.pem.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/dir-maxpath/c04.pem.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/dir-maxpath/c05.pem.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/dir-maxpath/c06.pem.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/dir-maxpath/c07.pem.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/dir-maxpath/c08.pem.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/dir-maxpath/c09.pem.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/dir-maxpath/c10.pem.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/dir-maxpath/c11.pem.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/dir-maxpath/c12.pem.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/dir-maxpath/c13.pem.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/dir-maxpath/c14.pem.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/dir-maxpath/c15.pem.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/dir-maxpath/c16.pem.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/dir-maxpath/c17.pem.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/dir-maxpath/c18.pem.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/dir-maxpath/c19.pem.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/dir-maxpath/c20.pem.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/dir-maxpath/int.opensslconf.zip.o			\
		o/$(MODE)/third_party/mbedtls/test/data/dir-maxpath/long.sh.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/dir1/.zip.o						\
		o/$(MODE)/third_party/mbedtls/test/data/dir2/.zip.o						\
		o/$(MODE)/third_party/mbedtls/test/data/dir3/.zip.o						\
		o/$(MODE)/third_party/mbedtls/test/data/dir4/.zip.o						\
		o/$(MODE)/third_party/mbedtls/test/data/dir1/test-ca.crt.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/dir2/test-ca.crt.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/dir2/test-ca2.crt.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/dir3/Readme.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/dir3/test-ca.crt.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/dir3/test-ca2.crt.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/dir4/cert11.crt.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/dir4/cert12.crt.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/dir4/cert13.crt.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/dir4/cert14.crt.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/dir4/cert21.crt.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/dir4/cert22.crt.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/dir4/cert23.crt.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/dir4/cert31.crt.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/dir4/cert32.crt.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/dir4/cert33.crt.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/dir4/cert34.crt.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/dir4/cert41.crt.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/dir4/cert42.crt.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/dir4/cert43.crt.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/dir4/cert44.crt.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/dir4/cert45.crt.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/dir4/cert51.crt.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/dir4/cert52.crt.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/dir4/cert53.crt.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/dir4/cert54.crt.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/dir4/cert61.crt.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/dir4/cert62.crt.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/dir4/cert63.crt.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/dir4/cert71.crt.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/dir4/cert72.crt.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/dir4/cert73.crt.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/dir4/cert74.crt.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/dir4/cert81.crt.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/dir4/cert82.crt.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/dir4/cert83.crt.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/dir4/cert91.crt.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/dir4/cert92.crt.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/enco-ca-prstr.pem.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/enco-cert-utf8str.pem.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/keyUsage.decipherOnly.crt.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/multiple_san.crt.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/non-ascii-string-in-issuer.crt.zip.o			\
		o/$(MODE)/third_party/mbedtls/test/data/server1-ms.req.sha256.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/server1-v1.crt.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/server1.cert_type.crt.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/server1.crt.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/server1.crt.der.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/server1.ext_ku.crt.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/server1.key_usage.crt.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/server1.req.md4.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/server1.req.md5.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/server1.req.sha1.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/server1.req.sha224.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/server1.req.sha256.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/server1.req.sha384.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/server1.req.sha512.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/server10-bs_int3.pem.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/server10_int3-bs.pem.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/server10_int3_int-ca2.crt.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/server10_int3_int-ca2_ca.crt.zip.o			\
		o/$(MODE)/third_party/mbedtls/test/data/server10_int3_spurious_int-ca2.crt.zip.o		\
		o/$(MODE)/third_party/mbedtls/test/data/server1_ca.crt.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/server1_pathlen_int_max-1.crt.zip.o			\
		o/$(MODE)/third_party/mbedtls/test/data/server1_pathlen_int_max.crt.zip.o			\
		o/$(MODE)/third_party/mbedtls/test/data/server2-badsign.crt.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/server2-v1-chain.crt.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/server2-v1.crt.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/server2.crt.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/server2.crt.der.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/server3.crt.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/server4.crt.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/server5-badsign.crt.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/server5-expired.crt.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/server5-fan.crt.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/server5-future.crt.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/server5-othername.crt.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/server5-selfsigned.crt.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/server5-sha1.crt.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/server5-sha224.crt.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/server5-sha384.crt.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/server5-sha512.crt.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/server5-ss-expired.crt.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/server5-ss-forgeca.crt.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/server5-tricky-ip-san.crt.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/server5-unsupported_othername.crt.zip.o			\
		o/$(MODE)/third_party/mbedtls/test/data/server5.crt.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/server5.eku-cli.crt.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/server5.eku-cs_any.crt.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/server5.eku-srv.crt.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/server5.eku-srv_cli.crt.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/server5.req.sha1.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/server5.req.sha224.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/server5.req.sha256.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/server5.req.sha384.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/server5.req.sha512.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/server6-ss-child.crt.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/server6.crt.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/server7-badsign.crt.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/server7-expired.crt.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/server7-future.crt.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/server7.crt.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/server7_all_space.crt.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/server7_int-ca-exp.crt.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/server7_int-ca.crt.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/server7_int-ca_ca2.crt.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/server7_pem_space.crt.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/server7_spurious_int-ca.crt.zip.o			\
		o/$(MODE)/third_party/mbedtls/test/data/server7_trailing_space.crt.zip.o			\
		o/$(MODE)/third_party/mbedtls/test/data/server8.crt.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/server8_int-ca2.crt.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/server9-bad-mgfhash.crt.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/server9-bad-saltlen.crt.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/server9-badsign.crt.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/server9-defaults.crt.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/server9-sha224.crt.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/server9-sha256.crt.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/server9-sha384.crt.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/server9-sha512.crt.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/server9-with-ca.crt.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/server9.crt.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/server9.req.sha1.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/server9.req.sha224.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/server9.req.sha256.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/server9.req.sha384.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/server9.req.sha512.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/test-ca-alt-good.crt.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/test-ca-any_policy.crt.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/test-ca-any_policy_ec.crt.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/test-ca-any_policy_with_qualifier.crt.zip.o		\
		o/$(MODE)/third_party/mbedtls/test/data/test-ca-any_policy_with_qualifier_ec.crt.zip.o		\
		o/$(MODE)/third_party/mbedtls/test/data/test-ca-good-alt.crt.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/test-ca-multi_policy.crt.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/test-ca-multi_policy_ec.crt.zip.o			\
		o/$(MODE)/third_party/mbedtls/test/data/test-ca-unsupported_policy.crt.zip.o			\
		o/$(MODE)/third_party/mbedtls/test/data/test-ca-unsupported_policy_ec.crt.zip.o			\
		o/$(MODE)/third_party/mbedtls/test/data/test-ca-v1.crt.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/test-ca.crt.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/test-ca.crt.der.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/test-ca2-expired.crt.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/test-ca2.crt.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/test-ca2.ku-crl.crt.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/test-ca2.ku-crt.crt.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/test-ca2.ku-crt_crl.crt.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/test-ca2.ku-ds.crt.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/test-ca2_cat-future-invalid.crt.zip.o			\
		o/$(MODE)/third_party/mbedtls/test/data/test-ca2_cat-future-present.crt.zip.o			\
		o/$(MODE)/third_party/mbedtls/test/data/test-ca2_cat-past-invalid.crt.zip.o			\
		o/$(MODE)/third_party/mbedtls/test/data/test-ca2_cat-past-present.crt.zip.o			\
		o/$(MODE)/third_party/mbedtls/test/data/test-ca2_cat-present-future.crt.zip.o			\
		o/$(MODE)/third_party/mbedtls/test/data/test-ca2_cat-present-past.crt.zip.o			\
		o/$(MODE)/third_party/mbedtls/test/data/test-ca_cat12.crt.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/test-ca_cat21.crt.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/test-ca_printable.crt.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/test-ca_uppercase.crt.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/test-ca_utf8.crt.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/test-int-ca.crt.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/test-int-ca2.crt.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/test-int-ca3.crt.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/test.pkg							\
		$(CRT)												\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

o/$(MODE)/third_party/mbedtls/test/test_suite_x509write.com: o/$(MODE)/third_party/mbedtls/test/test_suite_x509write.com.dbg
o/$(MODE)/third_party/mbedtls/test/test_suite_x509write.com.dbg:						\
		$(THIRD_PARTY_MBEDTLS_TEST_DEPS)								\
		o/$(MODE)/third_party/mbedtls/test/lib.o							\
		o/$(MODE)/third_party/mbedtls/test/test_suite_x509write.o					\
		o/$(MODE)/third_party/mbedtls/test/test_suite_x509write.datax.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/server1.ca.crt.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/server1.ca_noauthid.crt.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/server1.cert_type.crt.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/server1.cert_type_noauthid.crt.zip.o			\
		o/$(MODE)/third_party/mbedtls/test/data/server1.crt.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/server1.key.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/server1.key_usage.crt.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/server1.key_usage_noauthid.crt.zip.o			\
		o/$(MODE)/third_party/mbedtls/test/data/server1.noauthid.crt.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/server1.req.cert_type.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/server1.req.cert_type_empty.zip.o			\
		o/$(MODE)/third_party/mbedtls/test/data/server1.req.key_usage.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/server1.req.key_usage_empty.zip.o			\
		o/$(MODE)/third_party/mbedtls/test/data/server1.req.ku-ct.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/server1.req.md4.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/server1.req.md5.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/server1.req.sha1.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/server1.req.sha224.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/server1.req.sha256.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/server1.req.sha384.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/server1.req.sha512.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/server1.v1.crt.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/server5.key.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/data/server5.req.ku.sha1.zip.o				\
		o/$(MODE)/third_party/mbedtls/test/data/test-ca.key.zip.o					\
		o/$(MODE)/third_party/mbedtls/test/test.pkg							\
		$(CRT)												\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

o/$(MODE)/third_party/mbedtls/test/everest_test.com: o/$(MODE)/third_party/mbedtls/test/everest_test.com.dbg
o/$(MODE)/third_party/mbedtls/test/everest_test.com.dbg:							\
		$(THIRD_PARTY_MBEDTLS_TEST_DEPS)								\
		o/$(MODE)/third_party/mbedtls/test/everest_test.o						\
		o/$(MODE)/third_party/mbedtls/test/everest_unravaged.o						\
		o/$(MODE)/third_party/mbedtls/test/test.pkg							\
		$(LIBC_TESTMAIN)										\
		$(CRT)												\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

o/$(MODE)/third_party/mbedtls/test/secp384r1_test.com: o/$(MODE)/third_party/mbedtls/test/secp384r1_test.com.dbg
o/$(MODE)/third_party/mbedtls/test/secp384r1_test.com.dbg:							\
		$(THIRD_PARTY_MBEDTLS_TEST_DEPS)								\
		o/$(MODE)/third_party/mbedtls/test/secp384r1_test.o						\
		o/$(MODE)/third_party/mbedtls/test/test.pkg							\
		$(LIBC_TESTMAIN)										\
		$(CRT)												\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

# these need to be explictly defined because landlock make won't sandbox
# prerequisites with a trailing slash.
o/$(MODE)/third_party/mbedtls/test/data/.zip.o:									\
		third_party/mbedtls/test/data
o/$(MODE)/third_party/mbedtls/test/data/dir-maxpath/.zip.o:							\
		third_party/mbedtls/test/data/dir-maxpath
o/$(MODE)/third_party/mbedtls/test/data/dir1/.zip.o:								\
		third_party/mbedtls/test/data/dir1
o/$(MODE)/third_party/mbedtls/test/data/dir2/.zip.o:								\
		third_party/mbedtls/test/data/dir2
o/$(MODE)/third_party/mbedtls/test/data/dir3/.zip.o:								\
		third_party/mbedtls/test/data/dir3
o/$(MODE)/third_party/mbedtls/test/data/dir4/.zip.o:								\
		third_party/mbedtls/test/data/dir4
