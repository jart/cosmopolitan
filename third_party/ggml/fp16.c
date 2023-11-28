/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=4 sts=4 sw=4 fenc=utf-8                                :vi│
╚──────────────────────────────────────────────────────────────────────────────╝
│                                                                              │
│  GGML                                                                        │
│  Copyright (c) 2023 Georgi Gerganov                                          │
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
#include "third_party/ggml/fp16.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "third_party/ggml/fp16.internal.h"
#include "third_party/libcxx/math.h"

asm(".ident\t\"\\n\\n\
GGML (MIT License)\\n\
Copyright (c) 2023 Georgi Gerganov\"");
asm(".include \"libc/disclaimer.inc\"");

#if defined(__ARM_NEON) || defined(__wasm_simd128__)
#define B1(c,s,n)  0x ## n ## c ,  0x ## n ## s
#define B2(c,s,n) B1(c,s,n ## c), B1(c,s,n ## s)
#define B3(c,s,n) B2(c,s,n ## c), B2(c,s,n ## s)
#define B4(c,s,n) B3(c,s,n ## c), B3(c,s,n ## s)
#define B5(c,s,n) B4(c,s,n ## c), B4(c,s,n ## s)
#define B6(c,s,n) B5(c,s,n ## c), B5(c,s,n ## s)
#define B7(c,s,n) B6(c,s,n ## c), B6(c,s,n ## s)
#define B8(c,s  ) B7(c,s,     c), B7(c,s,     s)

// precomputed tables for expanding 8bits to 8 bytes:
const uint64_t table_b2b_0[1 << 8] = { B8(00, 10) }; // ( b) << 4
const uint64_t table_b2b_1[1 << 8] = { B8(10, 00) }; // (!b) << 4
#endif

//
// global data
//

// precomputed gelu table for f16 (128 KB)
ggml_fp16_t table_gelu_f16[1 << 16];

// precomputed silu table for f16 (128 KB)
ggml_fp16_t table_silu_f16[1 << 16];

// precomputed exp table for f16 (128 KB)
ggml_fp16_t table_exp_f16[1 << 16];

// precomputed f32 table for f16 (256 KB)
float table_f32_f16[1 << 16];

// note: do not use these inside ggml.c
// these are meant to be used via the ggml.h API
float ggml_fp16_to_fp32(ggml_fp16_t x) {
    return (float) GGML_FP16_TO_FP32(x);
}

ggml_fp16_t ggml_fp32_to_fp16(float x) {
    return GGML_FP32_TO_FP16(x);
}

void ggml_fp16_to_fp32_row(const ggml_fp16_t * x, float * y, size_t n) {
    size_t i = 0;
#ifdef __F16C__
    for (; i + 7 < n; i += 8) {
        __m128i x_vec = _mm_loadu_si128((const __m128i *)(x + i));
        __m256 y_vec = _mm256_cvtph_ps(x_vec);
        _mm256_storeu_ps(y + i, y_vec);
    }
#endif
    for (; i < n; i++) {
        y[i] = GGML_FP16_TO_FP32(x[i]);
    }
}

void ggml_fp32_to_fp16_row(const float * x, ggml_fp16_t * y, size_t n) {
    size_t i = 0;
#ifdef __F16C__
    for (; i + 7 < n; i += 8) {
        __m256 x_vec = _mm256_loadu_ps(x + i);
        __m128i y_vec = _mm256_cvtps_ph(x_vec, _MM_FROUND_TO_NEAREST_INT);
        _mm_storeu_si128((__m128i *)(y + i), y_vec);
    }
    for(; i + 3 < n; i += 4) {
        __m128 x_vec = _mm_loadu_ps(x + i);
        __m128i y_vec = _mm_cvtps_ph(x_vec, _MM_FROUND_TO_NEAREST_INT);
        _mm_storel_epi64((__m128i *)(y + i), y_vec);
    }
#endif
    for (; i < n; i++) {
        y[i] = GGML_FP32_TO_FP16(x[i]);
    }
}

static const float GELU_COEF_A    = 0.044715f;
static const float SQRT_2_OVER_PI = 0.79788456080286535587989211986876f;

inline static float ggml_gelu_f32(float x) {
    return 0.5f*x*(1.0f + tanhf(SQRT_2_OVER_PI*x*(1.0f + GELU_COEF_A*x*x)));
}

// Sigmoid Linear Unit (SiLU) function
inline static float ggml_silu_f32(float x) {
    return x/(1.0f + expf(-x));
}

void ggml_fp16_init(void) {
    ggml_fp16_t ii;
    ftrace_enabled(-1);
    for (int i = 0; i < (1 << 16); ++i) {
        uint16_t ui = i;
        memcpy(&ii, &ui, sizeof(ii));
        const float f = table_f32_f16[i] = GGML_COMPUTE_FP16_TO_FP32(ii);
        table_gelu_f16[i] = GGML_FP32_TO_FP16(ggml_gelu_f32(f));
        table_silu_f16[i] = GGML_FP32_TO_FP16(ggml_silu_f32(f));
        table_exp_f16[i]  = GGML_FP32_TO_FP16(expf(f));
    }
    ftrace_enabled(+1);
}
