/*
 * Copyright (c) The mldsa-native project authors
 * SPDX-License-Identifier: Apache-2.0 OR ISC OR MIT
 */
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Expose declaration of allocator (normally internal) */
#define MLD_BUILD_INTERNAL
#include "../../mldsa/mldsa_native.h"
#include "../../mldsa/src/common.h"
#include "../notrandombytes/notrandombytes.h"

/*
 * Level-dependent allocation limit macros.
 * These expand to the right MLD_TOTAL_ALLOC_{44,65,87}_* constant
 * based on MLD_CONFIG_API_PARAMETER_SET.
 *
 * Note: MLD_TOTAL_ALLOC_*_KEYPAIR in the header automatically adapts
 * based on MLD_CONFIG_KEYGEN_PCT.
 */
#define MLD_TOTAL_ALLOC_KEYPAIR__(LVL) MLD_TOTAL_ALLOC_##LVL##_KEYPAIR
#define MLD_TOTAL_ALLOC_KEYPAIR_(LVL) MLD_TOTAL_ALLOC_KEYPAIR__(LVL)
#define MLD_TOTAL_ALLOC_KEYPAIR \
  MLD_TOTAL_ALLOC_KEYPAIR_(MLD_CONFIG_API_PARAMETER_SET)

#define MLD_TOTAL_ALLOC_SIGN__(LVL) MLD_TOTAL_ALLOC_##LVL##_SIGN
#define MLD_TOTAL_ALLOC_SIGN_(LVL) MLD_TOTAL_ALLOC_SIGN__(LVL)
#define MLD_TOTAL_ALLOC_SIGN MLD_TOTAL_ALLOC_SIGN_(MLD_CONFIG_API_PARAMETER_SET)

#define MLD_TOTAL_ALLOC_VERIFY__(LVL) MLD_TOTAL_ALLOC_##LVL##_VERIFY
#define MLD_TOTAL_ALLOC_VERIFY_(LVL) MLD_TOTAL_ALLOC_VERIFY__(LVL)
#define MLD_TOTAL_ALLOC_VERIFY \
  MLD_TOTAL_ALLOC_VERIFY_(MLD_CONFIG_API_PARAMETER_SET)

#define MLD_TOTAL_ALLOC__(LVL) MLD_TOTAL_ALLOC_##LVL
#define MLD_TOTAL_ALLOC_(LVL) MLD_TOTAL_ALLOC__(LVL)
#define MLD_TOTAL_ALLOC MLD_TOTAL_ALLOC_(MLD_CONFIG_API_PARAMETER_SET)

/*
 * This test checks that
 * - we handle allocator failures correctly, propagating MLD_ERR_OUT_OF_MEMORY
 *   and cleaning up all memory, and
 * - we leak no memory, and
 * - we always de-allocate in the reverse order of allocation, thereby
 *   allowing the use of a bump allocator.
 *
 * This is done through a custom bump allocator and tracking of in-flight
 * allocations.
 */

/*
 * Allocation tracker
 *
 * Simple stack of in-flight allocations that's used to test that there are
 * no leaks and that we free in reverse order than we allocate. (The absence
 * of leaks is also checked through the address sanitizer)
 */

typedef struct
{
  void *addr;
  size_t size;
  const char *file;
  int line;
  const char *var;
  const char *type;
} alloc_info_t;

#define MLD_MAX_IN_FLIGHT_ALLOCS 100
#define MLD_BUMP_ALLOC_SIZE (128 * 1024) /* 128KB buffer */

struct test_ctx_t
{
  /* Bump allocator state */
  uint8_t *buffer;
  size_t offset;
  size_t high_mark;
  size_t global_high_mark;
  size_t global_high_mark_keypair;
  size_t global_high_mark_sign;
  size_t global_high_mark_verify;

  /* Allocation tracker */
  alloc_info_t alloc_stack[MLD_MAX_IN_FLIGHT_ALLOCS];
  int alloc_stack_top;

  /* Test control */
  int alloc_counter;
  int fail_on_counter;
  int print_debug_info;
};
typedef struct test_ctx_t test_ctx_t;

static void alloc_tracker_push(test_ctx_t *ctx, void *addr, size_t size,
                               const char *file, int line, const char *var,
                               const char *type)
{
  if (ctx->alloc_stack_top >= MLD_MAX_IN_FLIGHT_ALLOCS)
  {
    fprintf(stderr, "ERROR: Allocation stack overflow\n");
    exit(1);
  }
  ctx->alloc_stack[ctx->alloc_stack_top].addr = addr;
  ctx->alloc_stack[ctx->alloc_stack_top].size = size;
  ctx->alloc_stack[ctx->alloc_stack_top].file = file;
  ctx->alloc_stack[ctx->alloc_stack_top].line = line;
  ctx->alloc_stack[ctx->alloc_stack_top].var = var;
  ctx->alloc_stack[ctx->alloc_stack_top].type = type;
  ctx->alloc_stack_top++;
}

static void alloc_tracker_pop(test_ctx_t *ctx, void *addr, size_t size,
                              const char *file, int line, const char *var)
{
  alloc_info_t *top;
  if (ctx->alloc_stack_top == 0)
  {
    fprintf(
        stderr,
        "ERROR: Attempting to free %s at %s:%d but allocation stack is empty\n",
        var, file, line);
    exit(1);
  }

  top = &ctx->alloc_stack[ctx->alloc_stack_top - 1];
  if (top->addr != addr || top->size != size)
  {
    fprintf(stderr,
            "ERROR: Free order violation at %s:%d\n"
            "  Attempting to free: %s (addr=%p, sz %d)\n"
            "  Expected to free:   %s (addr=%p, sz %d) allocated at %s:%d\n",
            file, line, var, addr, (int)size, top->var, top->addr,
            (int)top->size, top->file, top->line);
    exit(1);
  }

  ctx->alloc_stack_top--;
}

static void *bump_alloc(test_ctx_t *ctx, size_t sz)
{
  /* Align to 32 bytes */
  size_t aligned_sz = (sz + 31) & ~((size_t)31);
  void *p;

  if (sz > MLD_BUMP_ALLOC_SIZE ||
      aligned_sz > MLD_BUMP_ALLOC_SIZE - ctx->offset)
  {
    return NULL;
  }

  p = ctx->buffer + ctx->offset;
  ctx->offset += aligned_sz;

  if (ctx->offset > ctx->high_mark)
  {
    ctx->high_mark = ctx->offset;
  }

  return p;
}

static int bump_free(test_ctx_t *ctx, void *p)
{
  if (p == NULL)
  {
    return 0;
  }

  /* Check that p is within the bump buffer */
  if (p < (void *)ctx->buffer || p >= (void *)(ctx->buffer + ctx->offset))
  {
    return -1;
  }

  /* Reset bump offset to the freed address */
  ctx->offset = (size_t)((uint8_t *)p - ctx->buffer);
  return 0;
}

static void reset_all(test_ctx_t *ctx)
{
  randombytes_reset();
  ctx->alloc_counter = 0;
  ctx->alloc_stack_top = 0;
  ctx->offset = 0;
  ctx->fail_on_counter = -1;
}

void *custom_alloc(test_ctx_t *ctx, size_t sz, const char *file, int line,
                   const char *var, const char *type)
{
  void *p = NULL;
  if (ctx->alloc_counter++ == ctx->fail_on_counter)
  {
    return NULL;
  }

  p = bump_alloc(ctx, sz);
  if (p == NULL)
  {
    fprintf(stderr,
            "ERROR: Bump allocator (%d bytes) ran out of memory. "
            "%s *%s (%d bytes) at %s:%d\n",
            (int)MLD_BUMP_ALLOC_SIZE, type, var, (int)sz, file, line);
    exit(1);
  }

  alloc_tracker_push(ctx, p, sz, file, line, var, type);

  if (ctx->print_debug_info == 1)
  {
    fprintf(stderr, "Alloc #%d: %s %s (%d bytes) at %s:%d\n",
            ctx->alloc_counter + 1, type, var, (int)sz, file, line);
  }

  return p;
}

void custom_free(test_ctx_t *ctx, void *p, size_t sz, const char *file,
                 int line, const char *var, const char *type)
{
  (void)type;

  if (p != NULL)
  {
    alloc_tracker_pop(ctx, p, sz, file, line, var);
  }

  if (bump_free(ctx, p) != 0)
  {
    fprintf(stderr, "ERROR: Free failed: %s %s (%d bytes) at %s:%d\n", type,
            var, (int)sz, file, line);
    exit(1);
  }
}

#define TEST_ALLOC_FAILURE(test_name, call, alloc_limit, global_high_mark_ptr) \
  do                                                                           \
  {                                                                            \
    int num_allocs, i, rc;                                                     \
    /* First pass: count allocations */                                        \
    ctx->high_mark = 0;                                                        \
    reset_all(ctx);                                                            \
    rc = call;                                                                 \
    if (rc != 0)                                                               \
    {                                                                          \
      fprintf(stderr, "ERROR: %s failed with %d in counting pass\n",           \
              test_name, rc);                                                  \
      return 1;                                                                \
    }                                                                          \
    if (ctx->alloc_stack_top != 0)                                             \
    {                                                                          \
      fprintf(stderr, "ERROR: %s leaked %d allocation(s) in counting pass\n",  \
              test_name, ctx->alloc_stack_top);                                \
      return 1;                                                                \
    }                                                                          \
    num_allocs = ctx->alloc_counter;                                           \
    /* Second pass: test each allocation failure */                            \
    for (i = 0; i < num_allocs; i++)                                           \
    {                                                                          \
      reset_all(ctx);                                                          \
      ctx->fail_on_counter = i;                                                \
      rc = call;                                                               \
      if (rc != MLD_ERR_OUT_OF_MEMORY)                                         \
      {                                                                        \
        int rc2;                                                               \
        /* Re-run dry-run and print debug info */                              \
        ctx->print_debug_info = 1;                                             \
        reset_all(ctx);                                                        \
        rc2 = call;                                                            \
        (void)rc2;                                                             \
        if (rc == 0)                                                           \
        {                                                                      \
          fprintf(stderr,                                                      \
                  "ERROR: %s unexpectedly succeeded when allocation %d/%d "    \
                  "was instrumented to fail\n",                                \
                  test_name, i + 1, num_allocs);                               \
        }                                                                      \
        else                                                                   \
        {                                                                      \
          fprintf(stderr,                                                      \
                  "ERROR: %s failed with wrong error code %d "                 \
                  "(expected %d) when allocation %d/%d was instrumented "      \
                  "to fail\n",                                                 \
                  test_name, rc, MLD_ERR_OUT_OF_MEMORY, i + 1, num_allocs);    \
        }                                                                      \
        return 1;                                                              \
      }                                                                        \
      if (ctx->alloc_stack_top != 0)                                           \
      {                                                                        \
        fprintf(stderr,                                                        \
                "ERROR: %s leaked %d allocation(s) when allocation %d/%d "     \
                "was instrumented to fail\n",                                  \
                test_name, ctx->alloc_stack_top, i + 1, num_allocs);           \
        return 1;                                                              \
      }                                                                        \
      if (ctx->offset != 0)                                                    \
      {                                                                        \
        fprintf(stderr,                                                        \
                "ERROR: %s leaked %d bytes when allocation %d/%d "             \
                "was instrumented to fail\n",                                  \
                test_name, (int)ctx->offset, i + 1, num_allocs);               \
        return 1;                                                              \
      }                                                                        \
    }                                                                          \
    if (ctx->high_mark > (alloc_limit))                                        \
    {                                                                          \
      fprintf(stderr, "ERROR: max allocation %u in %s exceeded limit %d\n",    \
              (unsigned)ctx->high_mark, test_name, (int)(alloc_limit));        \
      return 1;                                                                \
    }                                                                          \
    printf(                                                                    \
        "Allocation test for %s PASSED.\n"                                     \
        "  Max dynamic allocation: %d bytes\n",                                \
        test_name, (int)ctx->high_mark);                                       \
    if (ctx->high_mark > ctx->global_high_mark)                                \
    {                                                                          \
      ctx->global_high_mark = ctx->high_mark;                                  \
    }                                                                          \
    if (ctx->high_mark > *(global_high_mark_ptr))                              \
    {                                                                          \
      *(global_high_mark_ptr) = ctx->high_mark;                                \
    }                                                                          \
  } while (0)

static int test_keygen_alloc_failure(test_ctx_t *ctx)
{
  uint8_t pk[CRYPTO_PUBLICKEYBYTES];
  uint8_t sk[CRYPTO_SECRETKEYBYTES];

  TEST_ALLOC_FAILURE("mld_keypair", mld_keypair(pk, sk, ctx),
                     MLD_TOTAL_ALLOC_KEYPAIR, &ctx->global_high_mark_keypair);
  return 0;
}

static int test_sign_alloc_failure(test_ctx_t *ctx)
{
  uint8_t pk[CRYPTO_PUBLICKEYBYTES];
  uint8_t sk[CRYPTO_SECRETKEYBYTES];
  uint8_t sig[CRYPTO_BYTES];
  uint8_t msg[32] = {0};
  const uint8_t sign_ctx[] = "test context";
  size_t siglen;

  /* Generate valid keypair first */
  reset_all(ctx);
  if (mld_keypair(pk, sk, ctx) != 0)
  {
    fprintf(stderr, "ERROR: mld_keypair failed in sign test setup\n");
    return 1;
  }

  TEST_ALLOC_FAILURE("mld_signature",
                     mld_signature(sig, &siglen, msg, sizeof(msg), sign_ctx,
                                   sizeof(sign_ctx) - 1, sk, ctx),
                     MLD_TOTAL_ALLOC_SIGN, &ctx->global_high_mark_sign);
  return 0;
}

static int test_verify_alloc_failure(test_ctx_t *ctx)
{
  uint8_t pk[CRYPTO_PUBLICKEYBYTES];
  uint8_t sk[CRYPTO_SECRETKEYBYTES];
  uint8_t sig[CRYPTO_BYTES];
  uint8_t msg[32] = {0};
  const uint8_t sign_ctx[] = "test context";
  size_t siglen;

  /* Generate valid keypair and signature first */
  reset_all(ctx);
  if (mld_keypair(pk, sk, ctx) != 0)
  {
    fprintf(stderr, "ERROR: mld_keypair failed in verify test setup\n");
    return 1;
  }

  if (mld_signature(sig, &siglen, msg, sizeof(msg), sign_ctx,
                    sizeof(sign_ctx) - 1, sk, ctx) != 0)
  {
    fprintf(stderr, "ERROR: mld_signature failed in verify test setup\n");
    return 1;
  }

  TEST_ALLOC_FAILURE("mld_verify",
                     mld_verify(sig, siglen, msg, sizeof(msg), sign_ctx,
                                sizeof(sign_ctx) - 1, pk, ctx),
                     MLD_TOTAL_ALLOC_VERIFY, &ctx->global_high_mark_verify);
  return 0;
}

static int test_sign_combined_alloc_failure(test_ctx_t *ctx)
{
  uint8_t pk[CRYPTO_PUBLICKEYBYTES];
  uint8_t sk[CRYPTO_SECRETKEYBYTES];
  uint8_t sm[CRYPTO_BYTES + 32];
  uint8_t msg[32] = {0};
  const uint8_t sign_ctx[] = "test context";
  size_t smlen;

  reset_all(ctx);
  if (mld_keypair(pk, sk, ctx) != 0)
  {
    fprintf(stderr, "ERROR: mld_keypair failed in sign combined test setup\n");
    return 1;
  }

  TEST_ALLOC_FAILURE("mld_sign",
                     mld_sign(sm, &smlen, msg, sizeof(msg), sign_ctx,
                              sizeof(sign_ctx) - 1, sk, ctx),
                     MLD_TOTAL_ALLOC_SIGN, &ctx->global_high_mark_sign);
  return 0;
}

static int test_open_alloc_failure(test_ctx_t *ctx)
{
  uint8_t pk[CRYPTO_PUBLICKEYBYTES];
  uint8_t sk[CRYPTO_SECRETKEYBYTES];
  uint8_t sm[CRYPTO_BYTES + 32];
  uint8_t msg[32] = {0};
  uint8_t msg_out[CRYPTO_BYTES + 32];
  const uint8_t sign_ctx[] = "test context";
  size_t smlen, mlen;

  reset_all(ctx);
  if (mld_keypair(pk, sk, ctx) != 0)
  {
    fprintf(stderr, "ERROR: mld_keypair failed in open test setup\n");
    return 1;
  }

  if (mld_sign(sm, &smlen, msg, sizeof(msg), sign_ctx, sizeof(sign_ctx) - 1, sk,
               ctx) != 0)
  {
    fprintf(stderr, "ERROR: mld_sign failed in open test setup\n");
    return 1;
  }

  TEST_ALLOC_FAILURE("mld_open",
                     mld_open(msg_out, &mlen, sm, smlen, sign_ctx,
                              sizeof(sign_ctx) - 1, pk, ctx),
                     MLD_TOTAL_ALLOC_VERIFY, &ctx->global_high_mark_verify);
  return 0;
}

static int test_signature_extmu_alloc_failure(test_ctx_t *ctx)
{
  uint8_t pk[CRYPTO_PUBLICKEYBYTES];
  uint8_t sk[CRYPTO_SECRETKEYBYTES];
  uint8_t sig[CRYPTO_BYTES];
  uint8_t mu[64] = {0};
  size_t siglen;

  reset_all(ctx);
  if (mld_keypair(pk, sk, ctx) != 0)
  {
    fprintf(stderr,
            "ERROR: mld_keypair failed in signature_extmu test setup\n");
    return 1;
  }

  TEST_ALLOC_FAILURE("mld_signature_extmu",
                     mld_signature_extmu(sig, &siglen, mu, sk, ctx),
                     MLD_TOTAL_ALLOC_SIGN, &ctx->global_high_mark_sign);
  return 0;
}

static int test_verify_extmu_alloc_failure(test_ctx_t *ctx)
{
  uint8_t pk[CRYPTO_PUBLICKEYBYTES];
  uint8_t sk[CRYPTO_SECRETKEYBYTES];
  uint8_t sig[CRYPTO_BYTES];
  uint8_t mu[64] = {0};
  size_t siglen;

  reset_all(ctx);
  if (mld_keypair(pk, sk, ctx) != 0)
  {
    fprintf(stderr, "ERROR: mld_keypair failed in verify_extmu test setup\n");
    return 1;
  }

  if (mld_signature_extmu(sig, &siglen, mu, sk, ctx) != 0)
  {
    fprintf(stderr,
            "ERROR: mld_signature_extmu failed in verify_extmu test setup\n");
    return 1;
  }

  TEST_ALLOC_FAILURE("mld_verify_extmu",
                     mld_verify_extmu(sig, siglen, mu, pk, ctx),
                     MLD_TOTAL_ALLOC_VERIFY, &ctx->global_high_mark_verify);
  return 0;
}

static int test_signature_pre_hash_shake256_alloc_failure(test_ctx_t *ctx)
{
  uint8_t pk[CRYPTO_PUBLICKEYBYTES];
  uint8_t sk[CRYPTO_SECRETKEYBYTES];
  uint8_t sig[CRYPTO_BYTES];
  uint8_t msg[32] = {0};
  uint8_t rnd[32] = {0};
  const uint8_t sign_ctx[] = "test context";
  size_t siglen;

  reset_all(ctx);
  if (mld_keypair(pk, sk, ctx) != 0)
  {
    fprintf(stderr,
            "ERROR: mld_keypair failed in signature_pre_hash_shake256 test "
            "setup\n");
    return 1;
  }

  TEST_ALLOC_FAILURE(
      "mld_signature_pre_hash_shake256",
      mld_signature_pre_hash_shake256(sig, &siglen, msg, sizeof(msg), sign_ctx,
                                      sizeof(sign_ctx) - 1, rnd, sk, ctx),
      MLD_TOTAL_ALLOC_SIGN, &ctx->global_high_mark_sign);
  return 0;
}

static int test_verify_pre_hash_shake256_alloc_failure(test_ctx_t *ctx)
{
  uint8_t pk[CRYPTO_PUBLICKEYBYTES];
  uint8_t sk[CRYPTO_SECRETKEYBYTES];
  uint8_t sig[CRYPTO_BYTES];
  uint8_t msg[32] = {0};
  uint8_t rnd[32] = {0};
  const uint8_t sign_ctx[] = "test context";
  size_t siglen;

  reset_all(ctx);
  if (mld_keypair(pk, sk, ctx) != 0)
  {
    fprintf(stderr,
            "ERROR: mld_keypair failed in verify_pre_hash_shake256 test "
            "setup\n");
    return 1;
  }

  if (mld_signature_pre_hash_shake256(sig, &siglen, msg, sizeof(msg), sign_ctx,
                                      sizeof(sign_ctx) - 1, rnd, sk, ctx) != 0)
  {
    fprintf(stderr,
            "ERROR: mld_signature_pre_hash_shake256 failed in "
            "verify_pre_hash_shake256 test setup\n");
    return 1;
  }

  TEST_ALLOC_FAILURE(
      "mld_verify_pre_hash_shake256",
      mld_verify_pre_hash_shake256(sig, siglen, msg, sizeof(msg), sign_ctx,
                                   sizeof(sign_ctx) - 1, pk, ctx),
      MLD_TOTAL_ALLOC_VERIFY, &ctx->global_high_mark_verify);
  return 0;
}

static int test_pk_from_sk_alloc_failure(test_ctx_t *ctx)
{
  uint8_t pk[CRYPTO_PUBLICKEYBYTES];
  uint8_t sk[CRYPTO_SECRETKEYBYTES];

  reset_all(ctx);
  if (mld_keypair(pk, sk, ctx) != 0)
  {
    fprintf(stderr, "ERROR: mld_keypair failed in pk_from_sk test setup\n");
    return 1;
  }

  TEST_ALLOC_FAILURE("mld_pk_from_sk", mld_pk_from_sk(pk, sk, ctx),
                     MLD_TOTAL_ALLOC_KEYPAIR, &ctx->global_high_mark_keypair);
  return 0;
}

/*
 * Helper macro to check allocation high watermark matches expected limit.
 */
#define CHECK_ALLOC_MATCH(high_mark, expected)                               \
  do                                                                         \
  {                                                                          \
    if ((expected) != (high_mark))                                           \
    {                                                                        \
      fprintf(stderr, "ERROR: %s = %u does not match %s = %d\n", #high_mark, \
              (unsigned)(high_mark), #expected, (int)(expected));            \
      return 1;                                                              \
    }                                                                        \
  } while (0)

int main(void)
{
  MLD_ALIGN uint8_t bump_buffer[MLD_BUMP_ALLOC_SIZE];
  /* Initialize test context with default settings */
  test_ctx_t ctx = {
      NULL,  /* buffer (set below) */
      0,     /* offset */
      0,     /* high_mark */
      0,     /* global_high_mark */
      0,     /* global_high_mark_keypair */
      0,     /* global_high_mark_sign */
      0,     /* global_high_mark_verify */
      {{0}}, /* alloc_stack */
      0,     /* alloc_stack_top */
      0,     /* alloc_counter */
      -1,    /* fail_on_counter */
      0      /* print_debug_info */
  };
  ctx.buffer = bump_buffer;

  if (test_keygen_alloc_failure(&ctx) != 0)
  {
    return 1;
  }

  if (test_sign_alloc_failure(&ctx) != 0)
  {
    return 1;
  }

  if (test_verify_alloc_failure(&ctx) != 0)
  {
    return 1;
  }

  if (test_sign_combined_alloc_failure(&ctx) != 0)
  {
    return 1;
  }

  if (test_open_alloc_failure(&ctx) != 0)
  {
    return 1;
  }

  if (test_signature_extmu_alloc_failure(&ctx) != 0)
  {
    return 1;
  }

  if (test_verify_extmu_alloc_failure(&ctx) != 0)
  {
    return 1;
  }

  if (test_signature_pre_hash_shake256_alloc_failure(&ctx) != 0)
  {
    return 1;
  }

  if (test_verify_pre_hash_shake256_alloc_failure(&ctx) != 0)
  {
    return 1;
  }

  if (test_pk_from_sk_alloc_failure(&ctx) != 0)
  {
    return 1;
  }

  /* Check per-operation high watermarks match the declared limits */
  CHECK_ALLOC_MATCH(ctx.global_high_mark_keypair, MLD_TOTAL_ALLOC_KEYPAIR);
  CHECK_ALLOC_MATCH(ctx.global_high_mark_sign, MLD_TOTAL_ALLOC_SIGN);
  CHECK_ALLOC_MATCH(ctx.global_high_mark_verify, MLD_TOTAL_ALLOC_VERIFY);
  CHECK_ALLOC_MATCH(ctx.global_high_mark, MLD_TOTAL_ALLOC);

  return 0;
}
