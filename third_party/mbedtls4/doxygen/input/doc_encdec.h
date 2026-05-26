/**
 * \file doc_encdec.h
 *
 * \brief Encryption/decryption module documentation file.
 */
/*
 *
 *  Copyright The Mbed TLS Contributors
 *  SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
 */

/**
 * @addtogroup encdec_module Encryption/decryption module
 *
 * The Encryption/decryption module provides encryption/decryption functions.
 * One can differentiate between symmetric and asymmetric algorithms; the
 * symmetric ones are mostly used for message confidentiality and the asymmetric
 * ones for key exchange and message integrity.
 * Some symmetric algorithms provide different block cipher modes, mainly
 * Electronic Code Book (ECB) which is used for short (64-bit) messages and
 * Cipher Block Chaining (CBC) which provides the structure needed for longer
 * messages. In addition the Cipher Feedback Mode (CFB-128) stream cipher mode,
 * Counter mode (CTR) and Galois Counter Mode (GCM) are implemented for
 * specific algorithms.
 *
 * All symmetric encryption algorithms are accessible via the generic cipher layer
 * (see \c mbedtls_cipher_setup()).
 *
 * The asymmetric encryption algorithms are accessible via the generic public
 * key layer (see \c mbedtls_pk_init()).
 *
 * The following algorithms are provided:
 * - Symmetric:
 *   - AES (see \c mbedtls_aes_crypt_ecb(), \c mbedtls_aes_crypt_cbc(), \c mbedtls_aes_crypt_cfb128() and
 *     \c mbedtls_aes_crypt_ctr()).
 *   - Camellia (see \c mbedtls_camellia_crypt_ecb(), \c mbedtls_camellia_crypt_cbc(),
 *     \c mbedtls_camellia_crypt_cfb128() and \c mbedtls_camellia_crypt_ctr()).
 *   - DES/3DES (see \c mbedtls_des_crypt_ecb(), \c mbedtls_des_crypt_cbc(), \c mbedtls_des3_crypt_ecb()
 *     and \c mbedtls_des3_crypt_cbc()).
 *   - GCM (AES-GCM and CAMELLIA-GCM) (see \c mbedtls_gcm_init())
 * - Asymmetric:
 *   - RSA (see \c mbedtls_rsa_public() and \c mbedtls_rsa_private()).
 *   - Elliptic Curves over GF(p) (see \c mbedtls_ecp_point_init()).
 *   - Elliptic Curve Digital Signature Algorithm (ECDSA) (see \c mbedtls_ecdsa_init()).
 *   - Elliptic Curve Diffie Hellman (ECDH) (see \c mbedtls_ecdh_init()).
 *
 * This module provides encryption/decryption which can be used to provide
 * secrecy.
 *
 * It also provides asymmetric key functions which can be used for
 * confidentiality, integrity, authentication and non-repudiation.
 */
