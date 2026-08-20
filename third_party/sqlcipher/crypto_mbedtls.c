/*-*- mode:c;indent-tabs-mode:t;c-basic-offset:8;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=8 sts=8 sw=8 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ SQLCipher crypto provider using Cosmopolitan's mbedtls.                      │
│                                                                              │
│ Implements sqlcipher_provider: AES-256-CBC (no padding), HMAC-SHA1/256/512,  │
│ PBKDF2-HMAC, and getrandom(). CBC is done with AES-ECB so MODE=tiny (which   │
│ omits mbedtls's CBC helper) still links.                                     │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/stdio/rand.h"
#include "libc/str/str.h"
#include "third_party/mbedtls/aes.h"
#include "third_party/mbedtls/md.h"
#include "third_party/mbedtls/pkcs5.h"
#include "third_party/mbedtls/version.h"

#ifdef SQLITE_HAS_CODEC

#ifndef SQLCIPHER_H
#include "sqlcipher.h"
#endif

static const mbedtls_md_info_t *sqlcipher_mbedtls_md(int algorithm) {
	/* HMAC_* and PBKDF2_HMAC_* enums share values 0/1/2. */
	switch (algorithm) {
	case SQLCIPHER_HMAC_SHA1:
		return mbedtls_md_info_from_type(MBEDTLS_MD_SHA1);
	case SQLCIPHER_HMAC_SHA256:
		return mbedtls_md_info_from_type(MBEDTLS_MD_SHA256);
	case SQLCIPHER_HMAC_SHA512:
		return mbedtls_md_info_from_type(MBEDTLS_MD_SHA512);
	default:
		return 0;
	}
}

static int sqlcipher_mbedtls_add_random(void *ctx, const void *buffer, int length) {
	(void)ctx;
	(void)buffer;
	(void)length;
	return SQLITE_OK;
}

static int sqlcipher_mbedtls_random(void *ctx, void *buffer, int length) {
	unsigned char *p = (unsigned char *)buffer;
	(void)ctx;
	if (length < 0) return SQLITE_ERROR;
	while (length > 0) {
		ssize_t n = getrandom(p, (size_t)length, 0);
		if (n <= 0) return SQLITE_ERROR;
		p += n;
		length -= n;
	}
	return SQLITE_OK;
}

static int sqlcipher_mbedtls_hmac(void *ctx, int algorithm,
    const unsigned char *hmac_key, int key_sz, const unsigned char *in,
    int in_sz, const unsigned char *in2, int in2_sz, unsigned char *out) {
	mbedtls_md_context_t md;
	const mbedtls_md_info_t *info;
	int rc = SQLITE_ERROR;
	(void)ctx;
	if (!in || !out || key_sz < 0 || in_sz < 0) return SQLITE_ERROR;
	info = sqlcipher_mbedtls_md(algorithm);
	if (!info) return SQLITE_ERROR;
	mbedtls_md_init(&md);
	if (mbedtls_md_setup(&md, info, 1) != 0) goto done;
	if (mbedtls_md_hmac_starts(&md, hmac_key, (size_t)key_sz) != 0) goto done;
	if (mbedtls_md_hmac_update(&md, in, (size_t)in_sz) != 0) goto done;
	if (in2 && in2_sz > 0) {
		if (mbedtls_md_hmac_update(&md, in2, (size_t)in2_sz) != 0) goto done;
	}
	if (mbedtls_md_hmac_finish(&md, out) != 0) goto done;
	rc = SQLITE_OK;
done:
	mbedtls_md_free(&md);
	return rc;
}

static int sqlcipher_mbedtls_kdf(void *ctx, int algorithm,
    const unsigned char *pass, int pass_sz, const unsigned char *salt,
    int salt_sz, int workfactor, int key_sz, unsigned char *key) {
	mbedtls_md_context_t md;
	const mbedtls_md_info_t *info;
	int rc = SQLITE_ERROR;
	(void)ctx;
	if (!pass || !key || pass_sz < 0 || salt_sz < 0 || workfactor <= 0 ||
	    key_sz <= 0) {
		return SQLITE_ERROR;
	}
	info = sqlcipher_mbedtls_md(algorithm);
	if (!info) return SQLITE_ERROR;
	mbedtls_md_init(&md);
	if (mbedtls_md_setup(&md, info, 1) != 0) goto done;
	if (mbedtls_pkcs5_pbkdf2_hmac(&md, pass, (size_t)pass_sz, salt,
	        (size_t)salt_sz, (unsigned)workfactor, (uint32_t)key_sz,
	        key) != 0) {
		goto done;
	}
	rc = SQLITE_OK;
done:
	mbedtls_md_free(&md);
	return rc;
}

static int sqlcipher_mbedtls_cipher(void *ctx, int mode,
    const unsigned char *key, int key_sz, const unsigned char *iv,
    const unsigned char *in, int in_sz, unsigned char *out) {
	mbedtls_aes_context aes;
	unsigned char ivb[16];
	unsigned char tmp[16];
	int i, j;
	int rc = SQLITE_ERROR;
	(void)ctx;
	if (!key || !iv || !in || !out || key_sz != 32 || in_sz < 0 ||
	    (in_sz & 15)) {
		return SQLITE_ERROR;
	}
	memcpy(ivb, iv, 16);
	mbedtls_aes_init(&aes);
	if (mode == SQLCIPHER_ENCRYPT) {
		if (mbedtls_aes_setkey_enc(&aes, key, 256) != 0) goto done;
		for (i = 0; i < in_sz; i += 16) {
			for (j = 0; j < 16; ++j) tmp[j] = in[i + j] ^ ivb[j];
			if (mbedtls_aes_crypt_ecb(&aes, MBEDTLS_AES_ENCRYPT, tmp,
			        out + i) != 0) {
				goto done;
			}
			memcpy(ivb, out + i, 16);
		}
	} else if (mode == SQLCIPHER_DECRYPT) {
		if (mbedtls_aes_setkey_dec(&aes, key, 256) != 0) goto done;
		for (i = 0; i < in_sz; i += 16) {
			memcpy(tmp, in + i, 16);
			if (mbedtls_aes_crypt_ecb(&aes, MBEDTLS_AES_DECRYPT, in + i,
			        out + i) != 0) {
				goto done;
			}
			for (j = 0; j < 16; ++j) out[i + j] ^= ivb[j];
			memcpy(ivb, tmp, 16);
		}
	} else {
		goto done;
	}
	rc = SQLITE_OK;
done:
	mbedtls_aes_free(&aes);
	return rc;
}

static const char *sqlcipher_mbedtls_get_provider_name(void *ctx) {
	(void)ctx;
	return "mbedtls";
}

static const char *sqlcipher_mbedtls_get_provider_version(void *ctx) {
	(void)ctx;
	return MBEDTLS_VERSION_STRING_FULL;
}

static const char *sqlcipher_mbedtls_get_cipher(void *ctx) {
	(void)ctx;
	return "aes-256-cbc";
}

static int sqlcipher_mbedtls_get_key_sz(void *ctx) {
	(void)ctx;
	return 32;
}

static int sqlcipher_mbedtls_get_iv_sz(void *ctx) {
	(void)ctx;
	return 16;
}

static int sqlcipher_mbedtls_get_block_sz(void *ctx) {
	(void)ctx;
	return 16;
}

static int sqlcipher_mbedtls_get_hmac_sz(void *ctx, int algorithm) {
	const mbedtls_md_info_t *info = sqlcipher_mbedtls_md(algorithm);
	(void)ctx;
	return info ? (int)mbedtls_md_get_size(info) : 0;
}

static int sqlcipher_mbedtls_ctx_init(void **ctx) {
	if (ctx) *ctx = 0;
	return SQLITE_OK;
}

static int sqlcipher_mbedtls_ctx_free(void **ctx) {
	if (ctx) *ctx = 0;
	return SQLITE_OK;
}

static int sqlcipher_mbedtls_fips_status(void *ctx) {
	(void)ctx;
	return 0;
}

int sqlcipher_mbedtls_setup(sqlcipher_provider *p) {
	if (!p) return SQLITE_ERROR;
	p->init = 0;
	p->shutdown = 0;
	p->get_provider_name = sqlcipher_mbedtls_get_provider_name;
	p->random = sqlcipher_mbedtls_random;
	p->hmac = sqlcipher_mbedtls_hmac;
	p->kdf = sqlcipher_mbedtls_kdf;
	p->cipher = sqlcipher_mbedtls_cipher;
	p->get_cipher = sqlcipher_mbedtls_get_cipher;
	p->get_key_sz = sqlcipher_mbedtls_get_key_sz;
	p->get_iv_sz = sqlcipher_mbedtls_get_iv_sz;
	p->get_block_sz = sqlcipher_mbedtls_get_block_sz;
	p->get_hmac_sz = sqlcipher_mbedtls_get_hmac_sz;
	p->ctx_init = sqlcipher_mbedtls_ctx_init;
	p->ctx_free = sqlcipher_mbedtls_ctx_free;
	p->add_random = sqlcipher_mbedtls_add_random;
	p->fips_status = sqlcipher_mbedtls_fips_status;
	p->get_provider_version = sqlcipher_mbedtls_get_provider_version;
	return SQLITE_OK;
}

#endif /* SQLITE_HAS_CODEC */
