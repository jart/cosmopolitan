/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=4 sts=4 sw=4 fenc=utf-8                               :vi │
╚──────────────────────────────────────────────────────────────────────────────╝
│                                                                              │
│  HACL*                                                                       │
│  Copyright (c) 2016-2022 INRIA, CMU and Microsoft Corporation                │
│  Copyright (c) 2022-2023 HACL* Contributors                                  │
│                                                                              │
│  Permission is hereby granted, free of charge, to any person obtaining       │
│  a copy of this software and associated documentation files (the             │
│  "Software"), to deal in the Software without restriction, including         │
│  without limitation the rights to use, copy, modify, merge, publish,         │
│  distribute, sublicense, and/or sell copies of the Software, and to          │
│  permit persons to whom the Software is furnished to do so, subject to       │
│  the following conditions:                                                   │
│                                                                              │
│  The above copyright notice and this permission notice shall be              │
│  included in all copies or substantial portions of the Software.             │
│                                                                              │
│  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,             │
│  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF          │
│  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.      │
│  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY        │
│  CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,        │
│  TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE           │
│  SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                      │
│                                                                              │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/serialize.h"
#include "libc/stdckdint.h"
#include "libc/str/str.h"
#include "third_party/haclstar/haclstar.h"
#include "third_party/haclstar/loop.h"
#include "third_party/haclstar/macros.h"
#include "third_party/haclstar/mask.h"
__static_yoink("hacl_star_notice");

/**
  Initialize HPKE sender context and generate ephemeral public key.

  This function performs the HPKE "Base" mode sender setup, which
  establishes a secure communication channel from sender to receiver
  using Curve25519 for key exchange, ChaCha20Poly1305 for encryption,
  and SHA-256 for key derivation. It generates an ephemeral keypair and
  derives shared secrets needed for subsequent encrypt and decrypt
  operations.

  @param o_pkE Pointer to 32 bytes of memory where the sender's
      ephemeral public key will be written. This key must be transmitted
      to the receiver to complete the key exchange. The receiver needs
      this to decrypt messages.

  @param o_ctx HPKE context structure is initialized with derived keys
      and state upon success. (1) `ctx_key` is your 32-byte ChaCha20
      key, which will be the same value as what the receiver gets when
      calling setupBaseR(), and this value is guaranteed to be known
      only to these two parties. (2) `ctx_nonce` is another shared that
      will be used by Hacl_HPKE_getNonce() to generate nonces for your
      ChaCha20Poly1305 send and receive operations. (3) `ctx_seq` is
      initialized to zero and is the counter Hacl_HPKE_getNonce() uses
      for making each nonce unique. (4) `ctx_exporter` holds 32 bytes of
      additional shared secret data which you can use to derive your own
      keys that are bound to this secure channel and inherit its forward
      secrecy properties.

  @param skE Pointer to 32 bytes containing the sender's ephemeral
      private key. This should be generated randomly for each HPKE
      session to ensure forward secrecy. The same private key must not
      be reused across sessions.

  @param pkR Pointer to 32 bytes containing the receiver's public key.
      This is the long-term public key of the intended recipient, used
      to perform ECDH key exchange with the ephemeral private key.

  @param infolen Length of the application info parameter in bytes.

  @param info Pointer to `infolen` bytes of application-specific context
      information. This data is bound to the derived keys but not
      encrypted. It can contain protocol identifiers, algorithm
      parameters, or other context that should be authenticated as part
      of the key derivation.

  @return 0 on success, 1 on failure. Failure can occur if:
      - The ECDH operation results in the all-zero shared secret (invalid point)
      - The receiver's public key is invalid

  @see https://datatracker.ietf.org/doc/rfc9180/
*/
uint32_t
Hacl_HPKE_Curve25519_CP128_SHA256_setupBaseS(uint8_t o_pkE[32],
                                             Hacl_Impl_HPKE_context_s* o_ctx,
                                             const uint8_t skE[32],
                                             const uint8_t pkR[32],
                                             uint32_t infolen,
                                             const void* info)
{
    uint8_t o_shared[32U] = { 0U };
    uint8_t* o_pkE1 = o_pkE;
    Hacl_Curve25519_secret_to_public(o_pkE1, skE);
    uint32_t res1 = 0U;
    uint32_t res0;
    if (res1 == 0U) {
        uint8_t o_dh[32U] = { 0U };
        uint8_t zeros[32U] = { 0U };
        Hacl_Curve25519_scalarmult(o_dh, skE, pkR);
        uint8_t res2 = 255U;
        for (uint32_t i = 0U; i < 32U; i++) {
            uint8_t uu____0 = FStar_UInt8_eq_mask(o_dh[i], zeros[i]);
            res2 = (uint32_t)uu____0 & (uint32_t)res2;
        }
        uint8_t z = res2;
        uint32_t res;
        if (z == 255U) {
            res = 1U;
        } else {
            res = 0U;
        }
        uint32_t res20 = res;
        uint8_t o_kemcontext[64U] = { 0U };
        if (res20 == 0U) {
            memcpy(o_kemcontext, o_pkE, 32U * sizeof(uint8_t));
            uint8_t* o_pkRm = o_kemcontext + 32U;
            uint8_t* o_pkR = o_pkRm;
            memcpy(o_pkR, pkR, 32U * sizeof(uint8_t));
            uint8_t* o_dhm = o_dh;
            uint8_t o_eae_prk[32U] = { 0U };
            uint8_t suite_id_kem[5U] = { 0U };
            uint8_t* uu____1 = suite_id_kem;
            uu____1[0U] = 0x4bU;
            uu____1[1U] = 0x45U;
            uu____1[2U] = 0x4dU;
            uint8_t* uu____2 = suite_id_kem + 3U;
            uu____2[0U] = 0U;
            uu____2[1U] = 32U;
            uint8_t* empty = suite_id_kem;
            uint8_t label_eae_prk[7U] = { 0x65U, 0x61U, 0x65U, 0x5fU,
                                          0x70U, 0x72U, 0x6bU };
            uint32_t len0 = 51U;
            KRML_CHECK_SIZE(sizeof(uint8_t), len0);
            uint8_t tmp0[len0];
            memset(tmp0, 0U, len0 * sizeof(uint8_t));
            uint8_t* uu____3 = tmp0;
            uu____3[0U] = 0x48U;
            uu____3[1U] = 0x50U;
            uu____3[2U] = 0x4bU;
            uu____3[3U] = 0x45U;
            uu____3[4U] = 0x2dU;
            uu____3[5U] = 0x76U;
            uu____3[6U] = 0x31U;
            memcpy(tmp0 + 7U, suite_id_kem, 5U * sizeof(uint8_t));
            memcpy(tmp0 + 12U, label_eae_prk, 7U * sizeof(uint8_t));
            memcpy(tmp0 + 19U, o_dhm, 32U * sizeof(uint8_t));
            Hacl_HKDF_extract_sha2_256(o_eae_prk, empty, 0U, tmp0, len0);
            uint8_t label_shared_secret[13U] = { 0x73U, 0x68U, 0x61U, 0x72U,
                                                 0x65U, 0x64U, 0x5fU, 0x73U,
                                                 0x65U, 0x63U, 0x72U, 0x65U,
                                                 0x74U };
            uint32_t len = 91U;
            KRML_CHECK_SIZE(sizeof(uint8_t), len);
            uint8_t tmp[len];
            memset(tmp, 0U, len * sizeof(uint8_t));
            WRITE16BE(tmp, (uint16_t)32U);
            uint8_t* uu____4 = tmp + 2U;
            uu____4[0U] = 0x48U;
            uu____4[1U] = 0x50U;
            uu____4[2U] = 0x4bU;
            uu____4[3U] = 0x45U;
            uu____4[4U] = 0x2dU;
            uu____4[5U] = 0x76U;
            uu____4[6U] = 0x31U;
            memcpy(tmp + 9U, suite_id_kem, 5U * sizeof(uint8_t));
            memcpy(tmp + 14U, label_shared_secret, 13U * sizeof(uint8_t));
            memcpy(tmp + 27U, o_kemcontext, 64U * sizeof(uint8_t));
            Hacl_HKDF_expand_sha2_256(o_shared, o_eae_prk, 32U, tmp, len, 32U);
            res0 = 0U;
        } else {
            res0 = 1U;
        }
    } else {
        res0 = 1U;
    }
    if (res0 == 0U) {
        uint8_t o_context[65U] = { 0U };
        uint8_t o_secret[32U] = { 0U };
        uint8_t suite_id[10U] = { 0U };
        uint8_t* uu____5 = suite_id;
        uu____5[0U] = 0x48U;
        uu____5[1U] = 0x50U;
        uu____5[2U] = 0x4bU;
        uu____5[3U] = 0x45U;
        uint8_t* uu____6 = suite_id + 4U;
        uu____6[0U] = 0U;
        uu____6[1U] = 32U;
        uint8_t* uu____7 = suite_id + 6U;
        uu____7[0U] = 0U;
        uu____7[1U] = 1U;
        uint8_t* uu____8 = suite_id + 8U;
        uu____8[0U] = 0U;
        uu____8[1U] = 3U;
        uint8_t label_psk_id_hash[11U] = { 0x70U, 0x73U, 0x6bU, 0x5fU,
                                           0x69U, 0x64U, 0x5fU, 0x68U,
                                           0x61U, 0x73U, 0x68U };
        uint8_t o_psk_id_hash[32U] = { 0U };
        uint8_t* empty = suite_id;
        uint32_t len0 = 28U;
        KRML_CHECK_SIZE(sizeof(uint8_t), len0);
        uint8_t tmp0[len0];
        memset(tmp0, 0U, len0 * sizeof(uint8_t));
        uint8_t* uu____9 = tmp0;
        uu____9[0U] = 0x48U;
        uu____9[1U] = 0x50U;
        uu____9[2U] = 0x4bU;
        uu____9[3U] = 0x45U;
        uu____9[4U] = 0x2dU;
        uu____9[5U] = 0x76U;
        uu____9[6U] = 0x31U;
        memcpy(tmp0 + 7U, suite_id, 10U * sizeof(uint8_t));
        memcpy(tmp0 + 17U, label_psk_id_hash, 11U * sizeof(uint8_t));
        memcpy(tmp0 + 28U, empty, 0U * sizeof(uint8_t));
        Hacl_HKDF_extract_sha2_256(o_psk_id_hash, empty, 0U, tmp0, len0);
        uint8_t label_info_hash[9U] = { 0x69U, 0x6eU, 0x66U, 0x6fU, 0x5fU,
                                        0x68U, 0x61U, 0x73U, 0x68U };
        uint8_t o_info_hash[32U] = { 0U };
        uint32_t len1 = 26U + infolen;
        KRML_CHECK_SIZE(sizeof(uint8_t), len1);
        uint8_t tmp1[len1];
        memset(tmp1, 0U, len1 * sizeof(uint8_t));
        uint8_t* uu____10 = tmp1;
        uu____10[0U] = 0x48U;
        uu____10[1U] = 0x50U;
        uu____10[2U] = 0x4bU;
        uu____10[3U] = 0x45U;
        uu____10[4U] = 0x2dU;
        uu____10[5U] = 0x76U;
        uu____10[6U] = 0x31U;
        memcpy(tmp1 + 7U, suite_id, 10U * sizeof(uint8_t));
        memcpy(tmp1 + 17U, label_info_hash, 9U * sizeof(uint8_t));
        memcpy(tmp1 + 26U, info, infolen * sizeof(uint8_t));
        Hacl_HKDF_extract_sha2_256(o_info_hash, empty, 0U, tmp1, len1);
        o_context[0U] = 0U;
        memcpy(o_context + 1U, o_psk_id_hash, 32U * sizeof(uint8_t));
        memcpy(o_context + 33U, o_info_hash, 32U * sizeof(uint8_t));
        uint8_t label_secret[6U] = { 0x73U, 0x65U, 0x63U, 0x72U, 0x65U, 0x74U };
        uint32_t len2 = 23U;
        KRML_CHECK_SIZE(sizeof(uint8_t), len2);
        uint8_t tmp2[len2];
        memset(tmp2, 0U, len2 * sizeof(uint8_t));
        uint8_t* uu____11 = tmp2;
        uu____11[0U] = 0x48U;
        uu____11[1U] = 0x50U;
        uu____11[2U] = 0x4bU;
        uu____11[3U] = 0x45U;
        uu____11[4U] = 0x2dU;
        uu____11[5U] = 0x76U;
        uu____11[6U] = 0x31U;
        memcpy(tmp2 + 7U, suite_id, 10U * sizeof(uint8_t));
        memcpy(tmp2 + 17U, label_secret, 6U * sizeof(uint8_t));
        memcpy(tmp2 + 23U, empty, 0U * sizeof(uint8_t));
        Hacl_HKDF_extract_sha2_256(o_secret, o_shared, 32U, tmp2, len2);
        uint8_t label_exp[3U] = { 0x65U, 0x78U, 0x70U };
        uint32_t len3 = 87U;
        KRML_CHECK_SIZE(sizeof(uint8_t), len3);
        uint8_t tmp3[len3];
        memset(tmp3, 0U, len3 * sizeof(uint8_t));
        WRITE16BE(tmp3, (uint16_t)32U);
        uint8_t* uu____12 = tmp3 + 2U;
        uu____12[0U] = 0x48U;
        uu____12[1U] = 0x50U;
        uu____12[2U] = 0x4bU;
        uu____12[3U] = 0x45U;
        uu____12[4U] = 0x2dU;
        uu____12[5U] = 0x76U;
        uu____12[6U] = 0x31U;
        memcpy(tmp3 + 9U, suite_id, 10U * sizeof(uint8_t));
        memcpy(tmp3 + 19U, label_exp, 3U * sizeof(uint8_t));
        memcpy(tmp3 + 22U, o_context, 65U * sizeof(uint8_t));
        Hacl_HKDF_expand_sha2_256(
          o_ctx->ctx_exporter, o_secret, 32U, tmp3, len3, 32U);
        uint8_t label_key[3U] = { 0x6bU, 0x65U, 0x79U };
        uint32_t len4 = 87U;
        KRML_CHECK_SIZE(sizeof(uint8_t), len4);
        uint8_t tmp4[len4];
        memset(tmp4, 0U, len4 * sizeof(uint8_t));
        WRITE16BE(tmp4, (uint16_t)32U);
        uint8_t* uu____13 = tmp4 + 2U;
        uu____13[0U] = 0x48U;
        uu____13[1U] = 0x50U;
        uu____13[2U] = 0x4bU;
        uu____13[3U] = 0x45U;
        uu____13[4U] = 0x2dU;
        uu____13[5U] = 0x76U;
        uu____13[6U] = 0x31U;
        memcpy(tmp4 + 9U, suite_id, 10U * sizeof(uint8_t));
        memcpy(tmp4 + 19U, label_key, 3U * sizeof(uint8_t));
        memcpy(tmp4 + 22U, o_context, 65U * sizeof(uint8_t));
        Hacl_HKDF_expand_sha2_256(
          o_ctx->ctx_key, o_secret, 32U, tmp4, len4, 32U);
        uint8_t label_base_nonce[10U] = { 0x62U, 0x61U, 0x73U, 0x65U, 0x5fU,
                                          0x6eU, 0x6fU, 0x6eU, 0x63U, 0x65U };
        uint32_t len = 94U;
        KRML_CHECK_SIZE(sizeof(uint8_t), len);
        uint8_t tmp[len];
        memset(tmp, 0U, len * sizeof(uint8_t));
        WRITE16BE(tmp, (uint16_t)12U);
        uint8_t* uu____14 = tmp + 2U;
        uu____14[0U] = 0x48U;
        uu____14[1U] = 0x50U;
        uu____14[2U] = 0x4bU;
        uu____14[3U] = 0x45U;
        uu____14[4U] = 0x2dU;
        uu____14[5U] = 0x76U;
        uu____14[6U] = 0x31U;
        memcpy(tmp + 9U, suite_id, 10U * sizeof(uint8_t));
        memcpy(tmp + 19U, label_base_nonce, 10U * sizeof(uint8_t));
        memcpy(tmp + 29U, o_context, 65U * sizeof(uint8_t));
        Hacl_HKDF_expand_sha2_256(
          o_ctx->ctx_nonce, o_secret, 32U, tmp, len, 12U);
        o_ctx->ctx_seq = 0ULL;
        return res0;
    }
    return res0;
}

/**
  Initialize HPKE receiver context from sender's ephemeral public key.

  This function performs the HPKE "Base" mode receiver setup, completing the
  key exchange initiated by the sender. It uses the received ephemeral public
  key and the receiver's private key to derive the same shared secrets that
  the sender computed, enabling decryption of subsequent messages.

  @param o_ctx HPKE context structure is initialized with derived keys
      and state upon success. (1) `ctx_key` is your 32-byte ChaCha20
      key, which will be the same value as what the receiver gets when
      calling setupBaseR(), and this value is guaranteed to be known
      only to these two parties. (2) `ctx_nonce` is another shared that
      will be used by Hacl_HPKE_getNonce() to generate nonces for your
      ChaCha20Poly1305 send and receive operations. (3) `ctx_seq` is
      initialized to zero and is the counter Hacl_HPKE_getNonce() uses
      for making each nonce unique. (4) `ctx_exporter` holds 32 bytes of
      additional shared secret data which you can use to derive your own
      keys that are bound to this secure channel and inherit its forward
      secrecy properties.

  @param enc Pointer to 32 bytes containing the sender's ephemeral public key.
      This is the public key that was generated and transmitted by the sender
      during their setupBaseS operation. It's used to complete the ECDH key
      exchange.

  @param skR Pointer to 32 bytes containing the receiver's private key. This
      is the long-term private key corresponding to the public key that the
      sender used (pkR in setupBaseS). This key must be kept secret.

  @param infolen Length of the application info parameter in bytes. This must
      match the infolen used by the sender.

  @param info Pointer to `infolen` bytes of application-specific context
      information. This must be identical to the info used by the sender,
      otherwise key derivation will produce different keys and decryption
      will fail.

  @return 0 on success, 1 on failure. Failure can occur if:
      - The ECDH operation results in the all-zero shared secret (invalid point)
      - The sender's ephemeral public key is invalid

  @note This function must be called with the same parameters (except keys)
      that were used in the corresponding setupBaseS call. The info parameter
      especially must match exactly, or the derived keys will differ.

  @see https://datatracker.ietf.org/doc/rfc9180/
*/
uint32_t
Hacl_HPKE_Curve25519_CP128_SHA256_setupBaseR(Hacl_Impl_HPKE_context_s* o_ctx,
                                             const uint8_t enc[32],
                                             const uint8_t skR[32],
                                             uint32_t infolen,
                                             const void* info)
{
    uint8_t pkR[32U] = { 0U };
    Hacl_Curve25519_secret_to_public(pkR, skR);
    uint32_t res1 = 0U;
    uint8_t shared[32U] = { 0U };
    if (res1 == 0U) {
        const uint8_t* pkE = enc;
        uint8_t dh[32U] = { 0U };
        uint8_t zeros[32U] = { 0U };
        Hacl_Curve25519_scalarmult(dh, skR, pkE);
        uint8_t res0 = 255U;
        for (uint32_t i = 0U; i < 32U; i++) {
            uint8_t uu____0 = FStar_UInt8_eq_mask(dh[i], zeros[i]);
            res0 = (uint32_t)uu____0 & (uint32_t)res0;
        }
        uint8_t z = res0;
        uint32_t res;
        if (z == 255U) {
            res = 1U;
        } else {
            res = 0U;
        }
        uint32_t res11 = res;
        uint32_t res2;
        uint8_t kemcontext[64U] = { 0U };
        if (res11 == 0U) {
            uint8_t* pkRm = kemcontext + 32U;
            uint8_t* pkR1 = pkRm;
            Hacl_Curve25519_secret_to_public(pkR1, skR);
            uint32_t res20 = 0U;
            if (res20 == 0U) {
                memcpy(kemcontext, enc, 32U * sizeof(uint8_t));
                uint8_t* dhm = dh;
                uint8_t o_eae_prk[32U] = { 0U };
                uint8_t suite_id_kem[5U] = { 0U };
                uint8_t* uu____1 = suite_id_kem;
                uu____1[0U] = 0x4bU;
                uu____1[1U] = 0x45U;
                uu____1[2U] = 0x4dU;
                uint8_t* uu____2 = suite_id_kem + 3U;
                uu____2[0U] = 0U;
                uu____2[1U] = 32U;
                uint8_t* empty = suite_id_kem;
                uint8_t label_eae_prk[7U] = { 0x65U, 0x61U, 0x65U, 0x5fU,
                                              0x70U, 0x72U, 0x6bU };
                uint32_t len0 = 51U;
                KRML_CHECK_SIZE(sizeof(uint8_t), len0);
                uint8_t tmp0[len0];
                memset(tmp0, 0U, len0 * sizeof(uint8_t));
                uint8_t* uu____3 = tmp0;
                uu____3[0U] = 0x48U;
                uu____3[1U] = 0x50U;
                uu____3[2U] = 0x4bU;
                uu____3[3U] = 0x45U;
                uu____3[4U] = 0x2dU;
                uu____3[5U] = 0x76U;
                uu____3[6U] = 0x31U;
                memcpy(tmp0 + 7U, suite_id_kem, 5U * sizeof(uint8_t));
                memcpy(tmp0 + 12U, label_eae_prk, 7U * sizeof(uint8_t));
                memcpy(tmp0 + 19U, dhm, 32U * sizeof(uint8_t));
                Hacl_HKDF_extract_sha2_256(o_eae_prk, empty, 0U, tmp0, len0);
                uint8_t label_shared_secret[13U] = { 0x73U, 0x68U, 0x61U, 0x72U,
                                                     0x65U, 0x64U, 0x5fU, 0x73U,
                                                     0x65U, 0x63U, 0x72U, 0x65U,
                                                     0x74U };
                uint32_t len = 91U;
                KRML_CHECK_SIZE(sizeof(uint8_t), len);
                uint8_t tmp[len];
                memset(tmp, 0U, len * sizeof(uint8_t));
                WRITE16BE(tmp, (uint16_t)32U);
                uint8_t* uu____4 = tmp + 2U;
                uu____4[0U] = 0x48U;
                uu____4[1U] = 0x50U;
                uu____4[2U] = 0x4bU;
                uu____4[3U] = 0x45U;
                uu____4[4U] = 0x2dU;
                uu____4[5U] = 0x76U;
                uu____4[6U] = 0x31U;
                memcpy(tmp + 9U, suite_id_kem, 5U * sizeof(uint8_t));
                memcpy(tmp + 14U, label_shared_secret, 13U * sizeof(uint8_t));
                memcpy(tmp + 27U, kemcontext, 64U * sizeof(uint8_t));
                Hacl_HKDF_expand_sha2_256(
                  shared, o_eae_prk, 32U, tmp, len, 32U);
                res2 = 0U;
            } else {
                res2 = 1U;
            }
        } else {
            res2 = 1U;
        }
        if (res2 == 0U) {
            uint8_t o_context[65U] = { 0U };
            uint8_t o_secret[32U] = { 0U };
            uint8_t suite_id[10U] = { 0U };
            uint8_t* uu____5 = suite_id;
            uu____5[0U] = 0x48U;
            uu____5[1U] = 0x50U;
            uu____5[2U] = 0x4bU;
            uu____5[3U] = 0x45U;
            uint8_t* uu____6 = suite_id + 4U;
            uu____6[0U] = 0U;
            uu____6[1U] = 32U;
            uint8_t* uu____7 = suite_id + 6U;
            uu____7[0U] = 0U;
            uu____7[1U] = 1U;
            uint8_t* uu____8 = suite_id + 8U;
            uu____8[0U] = 0U;
            uu____8[1U] = 3U;
            uint8_t label_psk_id_hash[11U] = { 0x70U, 0x73U, 0x6bU, 0x5fU,
                                               0x69U, 0x64U, 0x5fU, 0x68U,
                                               0x61U, 0x73U, 0x68U };
            uint8_t o_psk_id_hash[32U] = { 0U };
            uint8_t* empty = suite_id;
            uint32_t len0 = 28U;
            KRML_CHECK_SIZE(sizeof(uint8_t), len0);
            uint8_t tmp0[len0];
            memset(tmp0, 0U, len0 * sizeof(uint8_t));
            uint8_t* uu____9 = tmp0;
            uu____9[0U] = 0x48U;
            uu____9[1U] = 0x50U;
            uu____9[2U] = 0x4bU;
            uu____9[3U] = 0x45U;
            uu____9[4U] = 0x2dU;
            uu____9[5U] = 0x76U;
            uu____9[6U] = 0x31U;
            memcpy(tmp0 + 7U, suite_id, 10U * sizeof(uint8_t));
            memcpy(tmp0 + 17U, label_psk_id_hash, 11U * sizeof(uint8_t));
            memcpy(tmp0 + 28U, empty, 0U * sizeof(uint8_t));
            Hacl_HKDF_extract_sha2_256(o_psk_id_hash, empty, 0U, tmp0, len0);
            uint8_t label_info_hash[9U] = { 0x69U, 0x6eU, 0x66U, 0x6fU, 0x5fU,
                                            0x68U, 0x61U, 0x73U, 0x68U };
            uint8_t o_info_hash[32U] = { 0U };
            uint32_t len1 = 26U + infolen;
            KRML_CHECK_SIZE(sizeof(uint8_t), len1);
            uint8_t tmp1[len1];
            memset(tmp1, 0U, len1 * sizeof(uint8_t));
            uint8_t* uu____10 = tmp1;
            uu____10[0U] = 0x48U;
            uu____10[1U] = 0x50U;
            uu____10[2U] = 0x4bU;
            uu____10[3U] = 0x45U;
            uu____10[4U] = 0x2dU;
            uu____10[5U] = 0x76U;
            uu____10[6U] = 0x31U;
            memcpy(tmp1 + 7U, suite_id, 10U * sizeof(uint8_t));
            memcpy(tmp1 + 17U, label_info_hash, 9U * sizeof(uint8_t));
            memcpy(tmp1 + 26U, info, infolen * sizeof(uint8_t));
            Hacl_HKDF_extract_sha2_256(o_info_hash, empty, 0U, tmp1, len1);
            o_context[0U] = 0U;
            memcpy(o_context + 1U, o_psk_id_hash, 32U * sizeof(uint8_t));
            memcpy(o_context + 33U, o_info_hash, 32U * sizeof(uint8_t));
            uint8_t label_secret[6U] = { 0x73U, 0x65U, 0x63U,
                                         0x72U, 0x65U, 0x74U };
            uint32_t len2 = 23U;
            KRML_CHECK_SIZE(sizeof(uint8_t), len2);
            uint8_t tmp2[len2];
            memset(tmp2, 0U, len2 * sizeof(uint8_t));
            uint8_t* uu____11 = tmp2;
            uu____11[0U] = 0x48U;
            uu____11[1U] = 0x50U;
            uu____11[2U] = 0x4bU;
            uu____11[3U] = 0x45U;
            uu____11[4U] = 0x2dU;
            uu____11[5U] = 0x76U;
            uu____11[6U] = 0x31U;
            memcpy(tmp2 + 7U, suite_id, 10U * sizeof(uint8_t));
            memcpy(tmp2 + 17U, label_secret, 6U * sizeof(uint8_t));
            memcpy(tmp2 + 23U, empty, 0U * sizeof(uint8_t));
            Hacl_HKDF_extract_sha2_256(o_secret, shared, 32U, tmp2, len2);
            uint8_t label_exp[3U] = { 0x65U, 0x78U, 0x70U };
            uint32_t len3 = 87U;
            KRML_CHECK_SIZE(sizeof(uint8_t), len3);
            uint8_t tmp3[len3];
            memset(tmp3, 0U, len3 * sizeof(uint8_t));
            WRITE16BE(tmp3, (uint16_t)32U);
            uint8_t* uu____12 = tmp3 + 2U;
            uu____12[0U] = 0x48U;
            uu____12[1U] = 0x50U;
            uu____12[2U] = 0x4bU;
            uu____12[3U] = 0x45U;
            uu____12[4U] = 0x2dU;
            uu____12[5U] = 0x76U;
            uu____12[6U] = 0x31U;
            memcpy(tmp3 + 9U, suite_id, 10U * sizeof(uint8_t));
            memcpy(tmp3 + 19U, label_exp, 3U * sizeof(uint8_t));
            memcpy(tmp3 + 22U, o_context, 65U * sizeof(uint8_t));
            Hacl_HKDF_expand_sha2_256(
              o_ctx->ctx_exporter, o_secret, 32U, tmp3, len3, 32U);
            uint8_t label_key[3U] = { 0x6bU, 0x65U, 0x79U };
            uint32_t len4 = 87U;
            KRML_CHECK_SIZE(sizeof(uint8_t), len4);
            uint8_t tmp4[len4];
            memset(tmp4, 0U, len4 * sizeof(uint8_t));
            WRITE16BE(tmp4, (uint16_t)32U);
            uint8_t* uu____13 = tmp4 + 2U;
            uu____13[0U] = 0x48U;
            uu____13[1U] = 0x50U;
            uu____13[2U] = 0x4bU;
            uu____13[3U] = 0x45U;
            uu____13[4U] = 0x2dU;
            uu____13[5U] = 0x76U;
            uu____13[6U] = 0x31U;
            memcpy(tmp4 + 9U, suite_id, 10U * sizeof(uint8_t));
            memcpy(tmp4 + 19U, label_key, 3U * sizeof(uint8_t));
            memcpy(tmp4 + 22U, o_context, 65U * sizeof(uint8_t));
            Hacl_HKDF_expand_sha2_256(
              o_ctx->ctx_key, o_secret, 32U, tmp4, len4, 32U);
            uint8_t label_base_nonce[10U] = {
                0x62U, 0x61U, 0x73U, 0x65U, 0x5fU,
                0x6eU, 0x6fU, 0x6eU, 0x63U, 0x65U
            };
            uint32_t len = 94U;
            KRML_CHECK_SIZE(sizeof(uint8_t), len);
            uint8_t tmp[len];
            memset(tmp, 0U, len * sizeof(uint8_t));
            WRITE16BE(tmp, (uint16_t)12U);
            uint8_t* uu____14 = tmp + 2U;
            uu____14[0U] = 0x48U;
            uu____14[1U] = 0x50U;
            uu____14[2U] = 0x4bU;
            uu____14[3U] = 0x45U;
            uu____14[4U] = 0x2dU;
            uu____14[5U] = 0x76U;
            uu____14[6U] = 0x31U;
            memcpy(tmp + 9U, suite_id, 10U * sizeof(uint8_t));
            memcpy(tmp + 19U, label_base_nonce, 10U * sizeof(uint8_t));
            memcpy(tmp + 29U, o_context, 65U * sizeof(uint8_t));
            Hacl_HKDF_expand_sha2_256(
              o_ctx->ctx_nonce, o_secret, 32U, tmp, len, 12U);
            o_ctx->ctx_seq = 0ULL;
            return 0U;
        }
        return 1U;
    }
    return 1U;
}

/**
  Creates fresh nonce for ChaCha20Poly1305 message.

  This generates a nonce and then increments the counter. If that causes
  an overflow, then an error is returned and the counter goes back to 0.

  @param ctx points to HPKE state object initialized earlier by either
      setupBaseS() or setupBaseR()

  @param o_nonce receives a unique 12-byte nonce value, which is always
      written in full, even on failure.

  @return false on success, or true if sequence overflowed
*/
bool
Hacl_HPKE_getNonce(Hacl_Impl_HPKE_context_s* ctx, uint8_t o_nonce[12])
{
    /* The sequence number provides nonce uniqueness: The nonce used for
       each encryption or decryption operation is the result of XORing
       base_nonce with the current sequence number, encoded as a
       big-endian integer of the same length as base_nonce.
       Implementations MAY use a sequence number that is shorter than
       the nonce length (padding on the left with zero), but MUST raise
       an error if the sequence number overflows. -Quoth RFC9180 */
    memcpy(o_nonce, ctx->ctx_nonce, 4);
    uint64_t word = READ64BE(ctx->ctx_nonce + 4);
    word ^= ctx->ctx_seq;
    WRITE64BE(o_nonce + 4, word);
    return ckd_add(&ctx->ctx_seq, ctx->ctx_seq, 1);
}
