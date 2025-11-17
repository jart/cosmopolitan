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
#include "libc/str/str.h"
#include "third_party/haclstar/haclstar.h"
#include "third_party/haclstar/macros.h"
__static_yoink("hacl_star_notice");

/**
  Expand pseudorandom key to desired length.

  @param okm Pointer to `len` bytes of memory where output keying
      material is written to.

  @param prk Pointer to at least `HashLen` bytes of memory where
      pseudorandom key is read from. Usually, this points to the output
      from the extract step.

  @param prklen Length of pseudorandom key.

  @param info Pointer to `infolen` bytes of memory where context and
      application specific information is read from. Can be a
      zero-length string.

  @param infolen Length of context and application specific information.

  @param len Length of output keying material.
*/
void
Hacl_HKDF_expand_sha2_256(uint8_t* okm,
                          uint8_t* prk,
                          uint32_t prklen,
                          uint8_t* info,
                          uint32_t infolen,
                          uint32_t len)
{
    uint32_t tlen = 32U;
    uint32_t n = len / tlen;
    uint8_t* output = okm;
    KRML_CHECK_SIZE(sizeof(uint8_t), tlen + infolen + 1U);
    uint8_t text[tlen + infolen + 1U];
    memset(text, 0U, (tlen + infolen + 1U) * sizeof(uint8_t));
    uint8_t* text0 = text + tlen;
    uint8_t* tag = text;
    uint8_t* ctr = text + tlen + infolen;
    memcpy(text + tlen, info, infolen * sizeof(uint8_t));
    for (uint32_t i = 0U; i < n; i++) {
        ctr[0U] = (uint8_t)(i + 1U);
        if (i == 0U) {
            Hacl_HMAC_compute_sha2_256(tag, prk, prklen, text0, infolen + 1U);
        } else {
            Hacl_HMAC_compute_sha2_256(
              tag, prk, prklen, text, tlen + infolen + 1U);
        }
        memcpy(output + i * tlen, tag, tlen * sizeof(uint8_t));
    }
    if (n * tlen < len) {
        ctr[0U] = (uint8_t)(n + 1U);
        if (n == 0U) {
            Hacl_HMAC_compute_sha2_256(tag, prk, prklen, text0, infolen + 1U);
        } else {
            Hacl_HMAC_compute_sha2_256(
              tag, prk, prklen, text, tlen + infolen + 1U);
        }
        uint8_t* block = okm + n * tlen;
        memcpy(block, tag, (len - n * tlen) * sizeof(uint8_t));
    }
}

/**
  Extract a fixed-length pseudorandom key from input keying material.

  @param prk Pointer to `HashLen` bytes of memory where pseudorandom key
      is written to.

  @param salt Pointer to `saltlen` bytes of memory where salt value is read
  from.

  @param saltlen Length of salt value.

  @param ikm Pointer to `ikmlen` bytes of memory where input keying
      material is read from.

  @param ikmlen Length of input keying material.
*/
void
Hacl_HKDF_extract_sha2_256(uint8_t* prk,
                           uint8_t* salt,
                           uint32_t saltlen,
                           uint8_t* ikm,
                           uint32_t ikmlen)
{
    Hacl_HMAC_compute_sha2_256(prk, salt, saltlen, ikm, ikmlen);
}

/**
  Expand pseudorandom key to desired length.

  @param okm Pointer to `len` bytes of memory where output keying
      material is written to.

  @param prk Pointer to at least `HashLen` bytes of memory where
      pseudorandom key is read from. Usually, this points to the output
      from the extract step.

  @param prklen Length of pseudorandom key.

  @param info Pointer to `infolen` bytes of memory where context and
      application specific information is read from. Can be a
      zero-length string.

  @param infolen Length of context and application specific information.

  @param len Length of output keying material.
*/
void
Hacl_HKDF_expand_sha2_384(uint8_t* okm,
                          uint8_t* prk,
                          uint32_t prklen,
                          uint8_t* info,
                          uint32_t infolen,
                          uint32_t len)
{
    uint32_t tlen = 48U;
    uint32_t n = len / tlen;
    uint8_t* output = okm;
    KRML_CHECK_SIZE(sizeof(uint8_t), tlen + infolen + 1U);
    uint8_t text[tlen + infolen + 1U];
    memset(text, 0U, (tlen + infolen + 1U) * sizeof(uint8_t));
    uint8_t* text0 = text + tlen;
    uint8_t* tag = text;
    uint8_t* ctr = text + tlen + infolen;
    memcpy(text + tlen, info, infolen * sizeof(uint8_t));
    for (uint32_t i = 0U; i < n; i++) {
        ctr[0U] = (uint8_t)(i + 1U);
        if (i == 0U) {
            Hacl_HMAC_compute_sha2_384(tag, prk, prklen, text0, infolen + 1U);
        } else {
            Hacl_HMAC_compute_sha2_384(
              tag, prk, prklen, text, tlen + infolen + 1U);
        }
        memcpy(output + i * tlen, tag, tlen * sizeof(uint8_t));
    }
    if (n * tlen < len) {
        ctr[0U] = (uint8_t)(n + 1U);
        if (n == 0U) {
            Hacl_HMAC_compute_sha2_384(tag, prk, prklen, text0, infolen + 1U);
        } else {
            Hacl_HMAC_compute_sha2_384(
              tag, prk, prklen, text, tlen + infolen + 1U);
        }
        uint8_t* block = okm + n * tlen;
        memcpy(block, tag, (len - n * tlen) * sizeof(uint8_t));
    }
}

/**
  Extract a fixed-length pseudorandom key from input keying material.

  @param prk Pointer to `HashLen` bytes of memory where pseudorandom key
      is written to.

  @param salt Pointer to `saltlen` bytes of memory where salt value is
      read from.

  @param saltlen Length of salt value.

  @param ikm Pointer to `ikmlen` bytes of memory where input keying
      material is read from.

  @param ikmlen Length of input keying material.
*/
void
Hacl_HKDF_extract_sha2_384(uint8_t* prk,
                           uint8_t* salt,
                           uint32_t saltlen,
                           uint8_t* ikm,
                           uint32_t ikmlen)
{
    Hacl_HMAC_compute_sha2_384(prk, salt, saltlen, ikm, ikmlen);
}

/**
  Expand pseudorandom key to desired length.

  @param okm Pointer to `len` bytes of memory where output keying
      material is written to.

  @param prk Pointer to at least `HashLen` bytes of memory where
      pseudorandom key is read from. Usually, this points to the output
      from the extract step.

  @param prklen Length of pseudorandom key.

  @param info Pointer to `infolen` bytes of memory where context and
      application specific information is read from. Can be a
      zero-length string.

  @param infolen Length of context and application specific information.

  @param len Length of output keying material.
*/
void
Hacl_HKDF_expand_sha2_512(uint8_t* okm,
                          uint8_t* prk,
                          uint32_t prklen,
                          uint8_t* info,
                          uint32_t infolen,
                          uint32_t len)
{
    uint32_t tlen = 64U;
    uint32_t n = len / tlen;
    uint8_t* output = okm;
    KRML_CHECK_SIZE(sizeof(uint8_t), tlen + infolen + 1U);
    uint8_t text[tlen + infolen + 1U];
    memset(text, 0U, (tlen + infolen + 1U) * sizeof(uint8_t));
    uint8_t* text0 = text + tlen;
    uint8_t* tag = text;
    uint8_t* ctr = text + tlen + infolen;
    memcpy(text + tlen, info, infolen * sizeof(uint8_t));
    for (uint32_t i = 0U; i < n; i++) {
        ctr[0U] = (uint8_t)(i + 1U);
        if (i == 0U) {
            Hacl_HMAC_compute_sha2_512(tag, prk, prklen, text0, infolen + 1U);
        } else {
            Hacl_HMAC_compute_sha2_512(
              tag, prk, prklen, text, tlen + infolen + 1U);
        }
        memcpy(output + i * tlen, tag, tlen * sizeof(uint8_t));
    }
    if (n * tlen < len) {
        ctr[0U] = (uint8_t)(n + 1U);
        if (n == 0U) {
            Hacl_HMAC_compute_sha2_512(tag, prk, prklen, text0, infolen + 1U);
        } else {
            Hacl_HMAC_compute_sha2_512(
              tag, prk, prklen, text, tlen + infolen + 1U);
        }
        uint8_t* block = okm + n * tlen;
        memcpy(block, tag, (len - n * tlen) * sizeof(uint8_t));
    }
}

/**
  Extract a fixed-length pseudorandom key from input keying material.

  @param prk Pointer to `HashLen` bytes of memory where pseudorandom key
      is written to.

  @param salt Pointer to `saltlen` bytes of memory where salt value is
      read from.

  @param saltlen Length of salt value.

  @param ikm Pointer to `ikmlen` bytes of memory where input keying
      material is read from.

  @param ikmlen Length of input keying material.
*/
void
Hacl_HKDF_extract_sha2_512(uint8_t* prk,
                           uint8_t* salt,
                           uint32_t saltlen,
                           uint8_t* ikm,
                           uint32_t ikmlen)
{
    Hacl_HMAC_compute_sha2_512(prk, salt, saltlen, ikm, ikmlen);
}
