#!/bin/sh
make -j8 o//third_party/mbedtls || exit

run() {
  echo $1
  $1
}

(
  run o//third_party/mbedtls/test/test_suite_aes.cbc.com
  run o//third_party/mbedtls/test/test_suite_aes.cfb.com
  run o//third_party/mbedtls/test/test_suite_aes.ecb.com
  run o//third_party/mbedtls/test/test_suite_aes.ofb.com
  run o//third_party/mbedtls/test/test_suite_aes.rest.com
  run o//third_party/mbedtls/test/test_suite_aes.xts.com
  run o//third_party/mbedtls/test/test_suite_asn1parse.com
  run o//third_party/mbedtls/test/test_suite_asn1write.com
  run o//third_party/mbedtls/test/test_suite_base64.com
  run o//third_party/mbedtls/test/test_suite_blowfish.com
  run o//third_party/mbedtls/test/test_suite_chacha20.com
  run o//third_party/mbedtls/test/test_suite_chachapoly.com
  run o//third_party/mbedtls/test/test_suite_cipher.aes.com
  run o//third_party/mbedtls/test/test_suite_cipher.blowfish.com
  run o//third_party/mbedtls/test/test_suite_cipher.ccm.com
  run o//third_party/mbedtls/test/test_suite_cipher.chacha20.com
  run o//third_party/mbedtls/test/test_suite_cipher.chachapoly.com
  run o//third_party/mbedtls/test/test_suite_cipher.des.com
  run o//third_party/mbedtls/test/test_suite_cipher.gcm.com
  run o//third_party/mbedtls/test/test_suite_cipher.misc.com
  run o//third_party/mbedtls/test/test_suite_cipher.nist_kw.com
  run o//third_party/mbedtls/test/test_suite_cipher.null.com
  run o//third_party/mbedtls/test/test_suite_cipher.padding.com
  run o//third_party/mbedtls/test/test_suite_ctr_drbg.com
  run o//third_party/mbedtls/test/test_suite_des.com
  run o//third_party/mbedtls/test/test_suite_dhm.com
  run o//third_party/mbedtls/test/test_suite_ecdh.com
  run o//third_party/mbedtls/test/test_suite_ecdsa.com
  run o//third_party/mbedtls/test/test_suite_ecjpake.com
  run o//third_party/mbedtls/test/test_suite_ecp.com
  run o//third_party/mbedtls/test/test_suite_entropy.com
  run o//third_party/mbedtls/test/test_suite_error.com
  run o//third_party/mbedtls/test/test_suite_gcm.aes128_de.com
  run o//third_party/mbedtls/test/test_suite_gcm.aes128_en.com
  run o//third_party/mbedtls/test/test_suite_gcm.aes192_de.com
  run o//third_party/mbedtls/test/test_suite_gcm.aes192_en.com
  run o//third_party/mbedtls/test/test_suite_gcm.aes256_de.com
  run o//third_party/mbedtls/test/test_suite_gcm.aes256_en.com
  run o//third_party/mbedtls/test/test_suite_gcm.misc.com
  run o//third_party/mbedtls/test/test_suite_hkdf.com
  run o//third_party/mbedtls/test/test_suite_hmac_drbg.misc.com
  run o//third_party/mbedtls/test/test_suite_hmac_drbg.no_reseed.com
  run o//third_party/mbedtls/test/test_suite_hmac_drbg.nopr.com
  run o//third_party/mbedtls/test/test_suite_hmac_drbg.pr.com
  run o//third_party/mbedtls/test/test_suite_md.com
  run o//third_party/mbedtls/test/test_suite_mdx.com
  run o//third_party/mbedtls/test/test_suite_memory_buffer_alloc.com
  run o//third_party/mbedtls/test/test_suite_mpi.com
  run o//third_party/mbedtls/test/test_suite_net.com
  run o//third_party/mbedtls/test/test_suite_nist_kw.com
  run o//third_party/mbedtls/test/test_suite_oid.com
  run o//third_party/mbedtls/test/test_suite_pem.com
  run o//third_party/mbedtls/test/test_suite_pk.com
  run o//third_party/mbedtls/test/test_suite_pkcs1_v15.com
  run o//third_party/mbedtls/test/test_suite_pkcs1_v21.com
  run o//third_party/mbedtls/test/test_suite_pkcs5.com
  run o//third_party/mbedtls/test/test_suite_pkparse.com
  run o//third_party/mbedtls/test/test_suite_pkwrite.com
  run o//third_party/mbedtls/test/test_suite_poly1305.com
  run o//third_party/mbedtls/test/test_suite_random.com
  run o//third_party/mbedtls/test/test_suite_rsa.com
  run o//third_party/mbedtls/test/test_suite_shax.com
  run o//third_party/mbedtls/test/test_suite_ssl.com
  run o//third_party/mbedtls/test/test_suite_timing.com
  run o//third_party/mbedtls/test/test_suite_version.com
  run o//third_party/mbedtls/test/test_suite_x509parse.com
  run o//third_party/mbedtls/test/test_suite_x509write.com
) | o//tool/build/deltaify2.com | sort -n | tee speed.txt

mkdir -p ~/speed/mbedtls
cp speed.txt ~/speed/mbedtls/$(date +%Y-%m-%d-%H-%H).txt
