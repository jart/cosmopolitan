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
#include "third_party/ggml/ggjt.v1.q4_0.h"
#include "libc/assert.h"
#include "libc/macros.internal.h"
#include "libc/str/str.h"
#include "third_party/aarch64/arm_neon.internal.h"
#include "third_party/ggml/ggjt.v1.internal.h"
#include "third_party/ggml/ggjt.v1.q8_0.h"
#include "third_party/intel/immintrin.internal.h"
#include "third_party/libcxx/math.h"

// quantization for the ggjt.v1.q4_0 file format

static_assert(sizeof(block_v1_q4_0) == sizeof(float) + V1_QK4_0 / 2,
              "wrong q4_0 block size/padding");
static_assert(sizeof(block_v1_q8_0) == sizeof(float) + V1_QK8_0,
              "wrong q8_0 block size/padding");

// reference implementation for deterministic creation of model files
void quantize_row_v1_q4_0_reference(const float * restrict x, block_v1_q4_0 * restrict y, int k) {
    assert(k % V1_QK4_0 == 0);
    const int nb = k / V1_QK4_0;

    uint8_t pp[V1_QK4_0/2];

    for (int i = 0; i < nb; i++) {
        float amax = 0.0f; // absolute max
        float max = 0.0f;

        for (int l = 0; l < V1_QK4_0; l++) {
            const float v = x[i*V1_QK4_0 + l];
            if (amax < fabsf(v)) {
                amax = fabsf(v);
                max = v;
            }
        }

        const float d = max / -8;
        const float id = d ? 1.0f/d : 0.0f;

        y[i].d = d;

        for (int l = 0; l < V1_QK4_0; l += 2) {
            const float v0 = x[i*V1_QK4_0 + l + 0]*id;
            const float v1 = x[i*V1_QK4_0 + l + 1]*id;

            const uint8_t vi0 = MIN(15, (int8_t)roundf(v0) + 8);
            const uint8_t vi1 = MIN(15, (int8_t)roundf(v1) + 8);

            assert(vi0 < 16);
            assert(vi1 < 16);

            pp[l/2] = vi0 | (vi1 << 4);
        }

        memcpy(y[i].qs, pp, sizeof(pp));
    }
}

void quantize_row_v1_q4_0(const float * restrict x, void * restrict vy, int k) {
    assert(k % V1_QK4_0 == 0);
    const int nb = k / V1_QK4_0;

    block_v1_q4_0 * restrict y = vy;

#if defined(__POWER9_VECTOR__)
    const vector float v85 = vec_splats(8.5f);
    const vector signed int v15 = vec_splats(15);
    for (int i = 0; i < nb; i++) {
        float max = 0.0f;
        float min = 0.0f;

        vector float asrcv [8];
        vector float srcv [8];
        vector float maxv[8];
        vector float minv[8];

        for (int l = 0; l < 8; l++) srcv[l]  = *(vector float *)(x + i*32 + 4*l);
        //for (int l = 0; l < 8; l++) asrcv[l] = vec_abs(srcv[l]);

        for (int l = 0; l < 4; l++) maxv[2*l] = vec_max(asrcv[2*l], asrcv[2*l+1]);
        //for (int l = 0; l < 2; l++) maxv[4*l] = vec_max(maxv[4*l], maxv[4*l+2]);
        maxv[0] = vec_max(maxv[0], maxv[2]);
        maxv[4] = vec_max(maxv[4], maxv[6]);
        //for (int l = 0; l < 1; l++) maxv[8*l] = vec_max(maxv[8*l], maxv[8*l+4]);
        maxv[0] = vec_max(maxv[0], maxv[4]);

        for (int l = 0; l < 4; l++) minv[2*l] = vec_min(asrcv[2*l], asrcv[2*l+1]);
        //for (int l = 0; l < 2; l++) minv[4*l] = vec_min(minv[4*l], minv[4*l+2]);
        minv[0] = vec_min(minv[0], minv[2]);
        minv[4] = vec_min(minv[4], minv[6]);
        //for (int l = 0; l < 1; l++) minv[8*l] = vec_min(minv[8*l], minv[8*l+4]);
        minv[0] = vec_min(minv[0], minv[4]);


        max = MAX(
                MAX(vec_extract(maxv[0], 0), vec_extract(maxv[0], 1)),
                MAX(vec_extract(maxv[0], 2), vec_extract(maxv[0], 3)));
        min = MIN(
                MIN(vec_extract(minv[0], 0), vec_extract(minv[0], 1)),
                MIN(vec_extract(minv[0], 2), vec_extract(minv[0], 3)));

        const float magnitude = max >= fabsf(min) ? max : min;
        const float d = magnitude / -8;
        const float id = d ? 1.0/d : 0.0;

        y[i].d = d;

        const vector float vid = vec_splats(id);
        uint8_t * restrict pb = y[i].qs;
        for (int l = 0; l < 8; l++) {
            const vector float vf  = vec_madd(srcv[l], vid, v85);
            const vector signed int vi = vec_signed(vf);
            const vector signed int vc = vec_min(vi, v15);

            pb[2*l + 0] = vec_extract(vc, 0) | (vec_extract(vc, 1) << 4);
            pb[2*l + 1] = vec_extract(vc, 2) | (vec_extract(vc, 3) << 4);
        }
    }
#elif __ARM_NEON
    for (int i = 0; i < nb; i++) {
        float32x4_t srcv [8];
        float32x4_t maxv[8];
        float32x4_t minv[8];

        for (int l = 0; l < 8; l++) srcv[l]  = vld1q_f32(x + i*32 + 4*l);

        for (int l = 0; l < 4; l++) maxv[2*l] = vmaxq_f32(srcv[2*l], srcv[2*l+1]);
        for (int l = 0; l < 2; l++) maxv[4*l] = vmaxq_f32(maxv[4*l], maxv[4*l+2]);
        for (int l = 0; l < 1; l++) maxv[8*l] = vmaxq_f32(maxv[8*l], maxv[8*l+4]);

        for (int l = 0; l < 4; l++) minv[2*l] = vminq_f32(srcv[2*l], srcv[2*l+1]);
        for (int l = 0; l < 2; l++) minv[4*l] = vminq_f32(minv[4*l], minv[4*l+2]);
        for (int l = 0; l < 1; l++) minv[8*l] = vminq_f32(minv[8*l], minv[8*l+4]);

        const float max = vmaxvq_f32(maxv[0]);
        const float min = vminvq_f32(minv[0]);

        const float magnitude = max >= fabsf(min) ? max : min;
        const float d = magnitude / -8;
        const float id = d ? 1.0f/d : 0.0f;

        y[i].d = d;

        for (int l = 0; l < 8; l++) {
            const float32x4_t v  = vmulq_n_f32(srcv[l], id);
            const float32x4_t vf = vaddq_f32(v, vdupq_n_f32(8.5f));
            const int32x4_t   vi = vcvtq_s32_f32(vf);
            const int32x4_t   vc = vminq_s32(vi, vdupq_n_s32(15));

            y[i].qs[2*l + 0] = vgetq_lane_s32(vc, 0) | (vgetq_lane_s32(vc, 1) << 4);
            y[i].qs[2*l + 1] = vgetq_lane_s32(vc, 2) | (vgetq_lane_s32(vc, 3) << 4);
        }
    }
#elif defined(__AVX2__)
    for (int i = 0; i < nb; i++) {
        // Load elements into 4 AVX vectors
        __m256 v0 = _mm256_loadu_ps( x );
        __m256 v1 = _mm256_loadu_ps( x + 8 );
        __m256 v2 = _mm256_loadu_ps( x + 16 );
        __m256 v3 = _mm256_loadu_ps( x + 24 );
        x += 32;

        // Compute max for the block
        __m256 max  = _mm256_max_ps( v0, v1 );
        __m256 maxTmp = _mm256_max_ps( v2, v3 );
        max = _mm256_max_ps( max, maxTmp );

        __m128 max4 = _mm_max_ps( _mm256_extractf128_ps( max, 1 ), _mm256_castps256_ps128( max ) );
        max4 = _mm_max_ps( max4, _mm_movehl_ps( max4, max4 ) );
        max4 = _mm_max_ss( max4, _mm_movehdup_ps( max4 ) );
        const float maxScalar = _mm_cvtss_f32( max4 );

        // Compute min for the block
        __m256 min  = _mm256_min_ps( v0, v1 );
        __m256 minTmp = _mm256_min_ps( v2, v3 );
        min = _mm256_min_ps( min, minTmp );

        __m128 min4 = _mm_min_ps( _mm256_extractf128_ps( min, 1 ), _mm256_castps256_ps128( min ) );
        min4 = _mm_min_ps( min4, _mm_movehl_ps( min4, min4 ) );
        min4 = _mm_min_ss( min4, _mm_movehdup_ps( min4 ) );
        const float minScalar = _mm_cvtss_f32( min4 );

        // Quantize these floats
        const float magnitude = maxScalar >= fabsf(minScalar) ? maxScalar : minScalar;
        const float d = magnitude / -8.0f;
        y[i].d = d;
        const float id = ( magnitude != 0.0f ) ? -8.0f / magnitude : 0.0f;
        const __m256 mul = _mm256_set1_ps( id );

        // Apply the multiplier
        v0 = _mm256_mul_ps( v0, mul );
        v1 = _mm256_mul_ps( v1, mul );
        v2 = _mm256_mul_ps( v2, mul );
        v3 = _mm256_mul_ps( v3, mul );

        // Round to nearest integer
        v0 = _mm256_round_ps( v0, _MM_ROUND_NEAREST );
        v1 = _mm256_round_ps( v1, _MM_ROUND_NEAREST );
        v2 = _mm256_round_ps( v2, _MM_ROUND_NEAREST );
        v3 = _mm256_round_ps( v3, _MM_ROUND_NEAREST );

        // Convert floats to integers
        __m256i i0 = _mm256_cvtps_epi32( v0 );
        __m256i i1 = _mm256_cvtps_epi32( v1 );
        __m256i i2 = _mm256_cvtps_epi32( v2 );
        __m256i i3 = _mm256_cvtps_epi32( v3 );

        // Convert int32 to int16
        i0 = _mm256_packs_epi32( i0, i1 );	// 0, 1, 2, 3,  8, 9, 10, 11,  4, 5, 6, 7, 12, 13, 14, 15
        i2 = _mm256_packs_epi32( i2, i3 );	// 16, 17, 18, 19,  24, 25, 26, 27,  20, 21, 22, 23, 28, 29, 30, 31
                                            // Convert int16 to int8
        i0 = _mm256_packs_epi16( i0, i2 );	// 0, 1, 2, 3,  8, 9, 10, 11,  16, 17, 18, 19,  24, 25, 26, 27,  4, 5, 6, 7, 12, 13, 14, 15, 20, 21, 22, 23, 28, 29, 30, 31

        // We got our precious signed bytes, but the order is now wrong
        // These AVX2 pack instructions process 16-byte pieces independently
        // The following instruction is fixing the order
        const __m256i perm = _mm256_setr_epi32( 0, 4, 1, 5, 2, 6, 3, 7 );
        i0 = _mm256_permutevar8x32_epi32( i0, perm );

        // Apply offset and clamp to translate the range from [ -8 .. +8 ] into [ +0 .. +15 ]
        const __m256i off = _mm256_set1_epi8( 8 );
        i0 = _mm256_add_epi8( i0, off );
        const __m256i maxNibble = _mm256_set1_epi8( 15 );
        i0 = _mm256_min_epi8( i0, maxNibble );

        // Compress the vector into 4 bit/value, and store
        __m128i res = packNibbles( i0 );
        _mm_storeu_si128( ( __m128i* )y[i].qs, res );
    }
#elif defined(__AVX__)
    for (int i = 0; i < nb; i++) {
        // Load elements into 4 AVX vectors
        __m256 v0 = _mm256_loadu_ps( x );
        __m256 v1 = _mm256_loadu_ps( x + 8 );
        __m256 v2 = _mm256_loadu_ps( x + 16 );
        __m256 v3 = _mm256_loadu_ps( x + 24 );
        x += 32;

        // Compute max for the block
        __m256 max  = _mm256_max_ps( v0, v1 );
        __m256 maxTmp = _mm256_max_ps( v2, v3 );
        max = _mm256_max_ps( max, maxTmp );

        __m128 max4 = _mm_max_ps( _mm256_extractf128_ps( max, 1 ), _mm256_castps256_ps128( max ) );
        max4 = _mm_max_ps( max4, _mm_movehl_ps( max4, max4 ) );
        max4 = _mm_max_ss( max4, _mm_movehdup_ps( max4 ) );
        const float maxScalar = _mm_cvtss_f32( max4 );

        // Compute min for the block
        __m256 min  = _mm256_min_ps( v0, v1 );
        __m256 minTmp = _mm256_min_ps( v2, v3 );
        min = _mm256_min_ps( min, minTmp );

        __m128 min4 = _mm_min_ps( _mm256_extractf128_ps( min, 1 ), _mm256_castps256_ps128( min ) );
        min4 = _mm_min_ps( min4, _mm_movehl_ps( min4, min4 ) );
        min4 = _mm_min_ss( min4, _mm_movehdup_ps( min4 ) );
        const float minScalar = _mm_cvtss_f32( min4 );

        // Quantize these floats
        const float magnitude = maxScalar >= fabsf(minScalar) ? maxScalar : minScalar;
        const float d = magnitude / -8.0f;
        y[i].d = d;
        const float id = ( magnitude != 0.0f ) ? -8.0f / magnitude : 0.0f;
        const __m256 mul = _mm256_set1_ps( id );

        // Apply the multiplier
        v0 = _mm256_mul_ps( v0, mul );
        v1 = _mm256_mul_ps( v1, mul );
        v2 = _mm256_mul_ps( v2, mul );
        v3 = _mm256_mul_ps( v3, mul );

        // Round to nearest integer
        v0 = _mm256_round_ps( v0, _MM_ROUND_NEAREST );
        v1 = _mm256_round_ps( v1, _MM_ROUND_NEAREST );
        v2 = _mm256_round_ps( v2, _MM_ROUND_NEAREST );
        v3 = _mm256_round_ps( v3, _MM_ROUND_NEAREST );

        // Convert floats to integers
        __m256i i0 = _mm256_cvtps_epi32( v0 );
        __m256i i1 = _mm256_cvtps_epi32( v1 );
        __m256i i2 = _mm256_cvtps_epi32( v2 );
        __m256i i3 = _mm256_cvtps_epi32( v3 );

        // Since we don't have in AVX some necessary functions,
        // we split the registers in half and call AVX2 analogs from SSE
        __m128i ni0 = _mm256_castsi256_si128( i0 );
        __m128i ni1 = _mm256_extractf128_si256( i0, 1);
        __m128i ni2 = _mm256_castsi256_si128( i1 );
        __m128i ni3 = _mm256_extractf128_si256( i1, 1);
        __m128i ni4 = _mm256_castsi256_si128( i2 );
        __m128i ni5 = _mm256_extractf128_si256( i2, 1);
        __m128i ni6 = _mm256_castsi256_si128( i3 );
        __m128i ni7 = _mm256_extractf128_si256( i3, 1);

        // Convert int32 to int16
        ni0 = _mm_packs_epi32( ni0, ni1 );
        ni2 = _mm_packs_epi32( ni2, ni3 );
        ni4 = _mm_packs_epi32( ni4, ni5 );
        ni6 = _mm_packs_epi32( ni6, ni7 );
        // Convert int16 to int8
        ni0 = _mm_packs_epi16( ni0, ni2 );
        ni4 = _mm_packs_epi16( ni4, ni6 );

        // Apply offset and clamp to translate the range from [ -8 .. +8 ] into [ +0 .. +15 ]
        const __m128i off = _mm_set1_epi8( 8 );
        ni0 = _mm_add_epi8( ni0, off );
        ni4 = _mm_add_epi8( ni4, off );
        const __m128i maxNibble = _mm_set1_epi8( 15 );
        ni0 = _mm_min_epi8( ni0, maxNibble );
        ni4 = _mm_min_epi8( ni4, maxNibble );

        // Compress the vector into 4 bit/value, and store
        __m128i res = packNibbles( ni0, ni4 );
        _mm_storeu_si128( ( __m128i* )y[i].qs, res );
    }
#elif defined(__wasm_simd128__)
    for (int i = 0; i < nb; i++) {
        float max = 0.0f;
        float min = 0.0f;

        v128_t srcv [8];
        v128_t maxv[8];
        v128_t minv[8];

        for (int l = 0; l < 8; l++) srcv[l]  = wasm_v128_load(x + i*32 + 4*l);

        for (int l = 0; l < 4; l++) maxv[2*l] = wasm_f32x4_max(srcv[2*l], srcv[2*l+1]);
        for (int l = 0; l < 2; l++) maxv[4*l] = wasm_f32x4_max(maxv[4*l], maxv[4*l+2]);
        for (int l = 0; l < 1; l++) maxv[8*l] = wasm_f32x4_max(maxv[8*l], maxv[8*l+4]);

        for (int l = 0; l < 4; l++) minv[2*l] = wasm_f32x4_min(srcv[2*l], srcv[2*l+1]);
        for (int l = 0; l < 2; l++) minv[4*l] = wasm_f32x4_min(minv[4*l], minv[4*l+2]);
        for (int l = 0; l < 1; l++) minv[8*l] = wasm_f32x4_min(minv[8*l], minv[8*l+4]);

        max = MAX(
                MAX(wasm_f32x4_extract_lane(maxv[0], 0), wasm_f32x4_extract_lane(maxv[0], 1)),
                MAX(wasm_f32x4_extract_lane(maxv[0], 2), wasm_f32x4_extract_lane(maxv[0], 3)));
        min = MIN(
                MIN(wasm_f32x4_extract_lane(minv[0], 0), wasm_f32x4_extract_lane(minv[0], 1)),
                MIN(wasm_f32x4_extract_lane(minv[0], 2), wasm_f32x4_extract_lane(minv[0], 3)));

        const float magnitude = max >= fabsf(min) ? max : min;
        const float d = magnitude / -8;
        const float id = d ? 1.0/d : 0.0;

        y[i].d = d;

        for (int l = 0; l < 8; l++) {
            const v128_t v  = wasm_f32x4_mul(srcv[l], wasm_f32x4_splat(id));
            const v128_t vf = wasm_f32x4_add(v, wasm_f32x4_splat(8.5f));
            const v128_t vi = wasm_i32x4_trunc_sat_f32x4(vf);
            const v128_t vc = wasm_i32x4_min(vi, wasm_i32x4_splat(15));

            y[i].qs[2*l + 0] = wasm_i32x4_extract_lane(vc, 0) | (wasm_i32x4_extract_lane(vc, 1) << 4);
            y[i].qs[2*l + 1] = wasm_i32x4_extract_lane(vc, 2) | (wasm_i32x4_extract_lane(vc, 3) << 4);
        }
    }
#else
    // scalar
    quantize_row_v1_q4_0_reference(x, y, k);
#endif
}

size_t ggml_quantize_v1_q4_0(const float * src, void * dst, int n, int k, int64_t * hist) {
    assert(k % V1_QK4_0 == 0);
    const int nb = k / V1_QK4_0;

    for (int j = 0; j < n; j += k) {
        block_v1_q4_0 * restrict y = (block_v1_q4_0 *)dst + j/V1_QK4_0;

        quantize_row_v1_q4_0_reference(src + j, y, k);

        for (int i = 0; i < nb; i++) {
            for (int l = 0; l < V1_QK4_0; l += 2) {
                const uint8_t vi0 = y[i].qs[l/2] & 0x0F;
                const uint8_t vi1 = y[i].qs[l/2] >> 4;

                hist[vi0]++;
                hist[vi1]++;
            }
        }
    }

    return (n/V1_QK4_0*sizeof(block_v1_q4_0));
}

void dequantize_row_v1_q4_0(const void * restrict vx, float * restrict y, int k) {
    assert(k % V1_QK4_0 == 0);
    const int nb = k / V1_QK4_0;

    const block_v1_q4_0 * restrict x = vx;

#if defined(__AVX2__)
    for (int i = 0; i < nb; i++) {
        // scale factor
        const __m256 d_v = _mm256_broadcast_ss(&x[i].d);

        const uint8_t * restrict pp = x[i].qs;

        for (int l = 0; l < V1_QK4_0; l += 32) {
            // Load 32x4-bit integers into 32x8-bit integers
            __m256i vx8 = bytes_from_nibbles_32(pp+l/2);

            // Subtract 8 from the integers
            vx8 = _mm256_sub_epi8(vx8, _mm256_set1_epi8(8));

            // Convert to 16-bit int
            const __m256i vx16_lo = _mm256_cvtepi8_epi16(_mm256_extracti128_si256(vx8, 0));
            const __m256i vx16_hi = _mm256_cvtepi8_epi16(_mm256_extracti128_si256(vx8, 1));

            // Convert to 32-bit int -> float 32
            const __m256 vf[4] = {
                _mm256_cvtepi32_ps(_mm256_cvtepi16_epi32(_mm256_extracti128_si256(vx16_lo, 0))),
                _mm256_cvtepi32_ps(_mm256_cvtepi16_epi32(_mm256_extracti128_si256(vx16_lo, 1))),
                _mm256_cvtepi32_ps(_mm256_cvtepi16_epi32(_mm256_extracti128_si256(vx16_hi, 0))),
                _mm256_cvtepi32_ps(_mm256_cvtepi16_epi32(_mm256_extracti128_si256(vx16_hi, 1)))
            };

            // Scale and store
            for (int j = 0; j < 4; j++) {
                const __m256 result = _mm256_mul_ps(vf[j], d_v);
                _mm256_storeu_ps(y + i * V1_QK4_0 + l + j*8, result);
            }
        }
    }
#elif defined(__ARM_NEON)
    for (int i = 0; i < nb; i++) {
        const float32x4_t vd = vdupq_n_f32(x[i].d);

        const uint8_t * restrict pp = x[i].qs;

        for (int l = 0; l < V1_QK4_0; l += 16) {
            // Load 16x4-bit integers into 8x8-bit integers
            const uint8x8_t v8 = vld1_u8(pp + l/2);

            // Expand 4-bit qs to 8-bit bytes
            const uint8x8_t v0 = vand_u8(v8, vdup_n_u8(0x0F));
            const uint8x8_t v1 = vshr_n_u8(v8, 4);

            // Convert to signed 8-bit integers
            const int8x8_t vs_0 = vreinterpret_s8_u8(v0);
            const int8x8_t vs_1 = vreinterpret_s8_u8(v1);

            // Subtract 8 from each byte
            const int8x8_t vb_0 = vsub_s8(vs_0, vdup_n_s8(8));
            const int8x8_t vb_1 = vsub_s8(vs_1, vdup_n_s8(8));

            // Interleave and combine
            const int8x8_t vx_0 = vzip1_s8(vb_0, vb_1);
            const int8x8_t vx_1 = vzip2_s8(vb_0, vb_1);

            const int8x16_t vq = vcombine_s8(vx_0, vx_1);

            // convert to 2x int16x8_t
            const int16x8_t vi_0 = vmovl_s8(vget_low_s8 (vq));
            const int16x8_t vi_1 = vmovl_s8(vget_high_s8(vq));

            // convert to 4x float32x4_t
            const float32x4_t vf_0 = vcvtq_f32_s32(vmovl_s16(vget_low_s16 (vi_0)));
            const float32x4_t vf_1 = vcvtq_f32_s32(vmovl_s16(vget_high_s16(vi_0)));
            const float32x4_t vf_2 = vcvtq_f32_s32(vmovl_s16(vget_low_s16 (vi_1)));
            const float32x4_t vf_3 = vcvtq_f32_s32(vmovl_s16(vget_high_s16(vi_1)));

            // Multiply by d
            const float32x4_t r0 = vmulq_f32(vf_0, vd);
            const float32x4_t r1 = vmulq_f32(vf_1, vd);
            const float32x4_t r2 = vmulq_f32(vf_2, vd);
            const float32x4_t r3 = vmulq_f32(vf_3, vd);

            // Store
            vst1q_f32(y + i*V1_QK4_0 + l +  0, r0);
            vst1q_f32(y + i*V1_QK4_0 + l +  4, r1);
            vst1q_f32(y + i*V1_QK4_0 + l +  8, r2);
            vst1q_f32(y + i*V1_QK4_0 + l + 12, r3);
        }
    }
#else
    // scalar
    for (int i = 0; i < nb; i++) {
        const float d = x[i].d;

        const uint8_t * restrict pp = x[i].qs;

        for (int l = 0; l < V1_QK4_0; l += 2) {
            const uint8_t vi = pp[l/2];

            const int8_t vi0 = vi & 0x0F;
            const int8_t vi1 = vi >> 4;

            const float v0 = (vi0 - 8)*d;
            const float v1 = (vi1 - 8)*d;

            //printf("d = %f, vi = %d, vi0 = %d, vi1 = %d, v0 = %f, v1 = %f\n", d, vi, vi0, vi1, v0, v1);

            y[i*V1_QK4_0 + l + 0] = v0;
            y[i*V1_QK4_0 + l + 1] = v1;

            assert(!isnan(y[i*V1_QK4_0 + l + 0]));
            assert(!isnan(y[i*V1_QK4_0 + l + 1]));
        }
    }
#endif
}

void ggml_vec_dot_v1_q4_0_q8_0(const int n, float * restrict s, const void * restrict vx, const void * restrict vy) {
    const int nb = n / V1_QK8_0;

    assert(n % V1_QK8_0 == 0);
    assert(nb % 2 == 0);

    const block_v1_q4_0 * restrict x = vx;
    const block_v1_q8_0 * restrict y = vy;

#if defined(__ARM_NEON)
    float32x4_t sumv0 = vdupq_n_f32(0.0f);
    float32x4_t sumv1 = vdupq_n_f32(0.0f);

    for (int i = 0; i < nb; i += 2) {
        const block_v1_q4_0 * restrict x0 = &x[i + 0];
        const block_v1_q4_0 * restrict x1 = &x[i + 1];
        const block_v1_q8_0 * restrict y0 = &y[i + 0];
        const block_v1_q8_0 * restrict y1 = &y[i + 1];

        const uint8x16_t m4b   = vdupq_n_u8(0x0F);
        const int8x16_t  s8b   = vdupq_n_s8(0x8);

        const uint8x16_t v0_0 = vld1q_u8(x0->qs);
        const uint8x16_t v0_1 = vld1q_u8(x1->qs);

        // 4-bit -> 8-bit
        const int8x16_t v0_0l = vreinterpretq_s8_u8(vandq_u8  (v0_0, m4b));
        const int8x16_t v0_0h = vreinterpretq_s8_u8(vshrq_n_u8(v0_0, 4));
        const int8x16_t v0_1l = vreinterpretq_s8_u8(vandq_u8  (v0_1, m4b));
        const int8x16_t v0_1h = vreinterpretq_s8_u8(vshrq_n_u8(v0_1, 4));

        // sub 8
        const int8x16_t v0_0ls = vsubq_s8(v0_0l, s8b);
        const int8x16_t v0_0hs = vsubq_s8(v0_0h, s8b);
        const int8x16_t v0_1ls = vsubq_s8(v0_1l, s8b);
        const int8x16_t v0_1hs = vsubq_s8(v0_1h, s8b);

        // interleave
        const int8x16_t v0_0lz = vzip1q_s8(v0_0ls, v0_0hs);
        const int8x16_t v0_0hz = vzip2q_s8(v0_0ls, v0_0hs);
        const int8x16_t v0_1lz = vzip1q_s8(v0_1ls, v0_1hs);
        const int8x16_t v0_1hz = vzip2q_s8(v0_1ls, v0_1hs);

        // load y
        const int8x16_t v1_0l = vld1q_s8(y0->qs);
        const int8x16_t v1_0h = vld1q_s8(y0->qs + 16);
        const int8x16_t v1_1l = vld1q_s8(y1->qs);
        const int8x16_t v1_1h = vld1q_s8(y1->qs + 16);

#if defined(__ARM_FEATURE_DOTPROD)
        // dot product into int32x4_t
        const int32x4_t p_0 = vdotq_s32(vdotq_s32(vdupq_n_s32(0), v0_0lz, v1_0l), v0_0hz, v1_0h);
        const int32x4_t p_1 = vdotq_s32(vdotq_s32(vdupq_n_s32(0), v0_1lz, v1_1l), v0_1hz, v1_1h);

        sumv0 = vmlaq_n_f32(sumv0, vcvtq_f32_s32(p_0), x0->d*y0->d);
        sumv1 = vmlaq_n_f32(sumv1, vcvtq_f32_s32(p_1), x1->d*y1->d);
#else
        const int16x8_t pl0l = vmull_s8(vget_low_s8 (v0_0lz), vget_low_s8 (v1_0l));
        const int16x8_t pl0h = vmull_s8(vget_high_s8(v0_0lz), vget_high_s8(v1_0l));
        const int16x8_t ph0l = vmull_s8(vget_low_s8 (v0_0hz), vget_low_s8 (v1_0h));
        const int16x8_t ph0h = vmull_s8(vget_high_s8(v0_0hz), vget_high_s8(v1_0h));

        const int16x8_t pl1l = vmull_s8(vget_low_s8 (v0_1lz), vget_low_s8 (v1_1l));
        const int16x8_t pl1h = vmull_s8(vget_high_s8(v0_1lz), vget_high_s8(v1_1l));
        const int16x8_t ph1l = vmull_s8(vget_low_s8 (v0_1hz), vget_low_s8 (v1_1h));
        const int16x8_t ph1h = vmull_s8(vget_high_s8(v0_1hz), vget_high_s8(v1_1h));

        const int32x4_t pl0 = vaddq_s32(vpaddlq_s16(pl0l), vpaddlq_s16(pl0h));
        const int32x4_t ph0 = vaddq_s32(vpaddlq_s16(ph0l), vpaddlq_s16(ph0h));
        const int32x4_t pl1 = vaddq_s32(vpaddlq_s16(pl1l), vpaddlq_s16(pl1h));
        const int32x4_t ph1 = vaddq_s32(vpaddlq_s16(ph1l), vpaddlq_s16(ph1h));

        sumv0 = vmlaq_n_f32(sumv0, vcvtq_f32_s32(vaddq_s32(pl0, ph0)), x0->d*y0->d);
        sumv1 = vmlaq_n_f32(sumv1, vcvtq_f32_s32(vaddq_s32(pl1, ph1)), x1->d*y1->d);
#endif
    }

    *s = vaddvq_f32(sumv0) + vaddvq_f32(sumv1);
#elif defined(__AVX2__)
    // Initialize accumulator with zeros
    __m256 acc = _mm256_setzero_ps();

    // Main loop
#define WORK(I) \
    /* Compute combined scale for the block */ \
    const __m256 d = _mm256_mul_ps( _mm256_broadcast_ss( &x[I].d ), _mm256_broadcast_ss( &y[I].d ) ); \
    __m256i bx = bytes_from_nibbles_32(x[I].qs); \
    /* Now we have a vector with bytes in [ 0 .. 15 ] interval. Offset them into [ -8 .. +7 ] interval. */ \
    const __m256i off = _mm256_set1_epi8( 8 ); \
    bx = _mm256_sub_epi8( bx, off ); \
    __m256i by = _mm256_loadu_si256((const __m256i *)y[I].qs); \
    const __m256 q = mul_sum_i8_pairs_float(bx, by); \
    /* Multiply q with scale and accumulate */ \
    acc = _mm256_fmadd_ps( d, q, acc )
    int i = 0;
    for (; i + 12 < nb; i += 12) {
        _mm_prefetch(x+i+12, 3);
        _mm_prefetch(x+i+15, 3);
        _mm_prefetch(x+i+18, 3);
        _mm_prefetch(x+i+21, 3);
        _mm_prefetch(y+i+12, 3);
        _mm_prefetch(y+i+14, 3);
        _mm_prefetch(y+i+16, 3);
        _mm_prefetch(y+i+18, 3);
        _mm_prefetch(y+i+20, 3);
        _mm_prefetch(y+i+22, 3);
        for (int j = 0; j < 12; ++j) {
            WORK(i+j);
        }
    }
    for (; i < nb; ++i) {
        WORK(i);
    }
#undef WORK

    *s = hsum_float_8(acc);
#elif defined(__AVX__)
    // Initialize accumulator with zeros
    __m256 acc = _mm256_setzero_ps();

    // Main loop
    for (int i = 0; i < nb; ++i) {
        // Compute combined scale for the block
        const __m256 d = _mm256_mul_ps( _mm256_broadcast_ss( &x[i].d ), _mm256_broadcast_ss( &y[i].d ) );

        __m128i i32[2];
        for (int j = 0; j < 2; ++j) {
            // Load 8 bytes, and unpack 4 bit fields into bytes, making 16 bytes
            __m128i bx = bytes_from_nibbles_16(x[i].qs + 8*j);
            __m128i by = _mm_loadu_si128((const __m128i *)(y[i].qs + 16*j));

            // Now we have a vector with bytes in [ 0 .. 15 ] interval. Offset them into [ -8 .. +7 ] interval.
            const __m128i off = _mm_set1_epi8( 8 );
            bx = _mm_sub_epi8( bx, off );

            // Get absolute values of x vectors
            const __m128i ax = _mm_sign_epi8(bx, bx);

            // Sign the values of the y vectors
            const __m128i sy = _mm_sign_epi8(by, bx);

            // Perform multiplication and create 16-bit values
            const __m128i dot = _mm_maddubs_epi16(ax, sy);

            const __m128i ones = _mm_set1_epi16(1);
            i32[j] = _mm_madd_epi16(ones, dot);
        }

        // Convert int32_t to float
        __m256 p = _mm256_cvtepi32_ps( _mm256_set_m128i( i32[0], i32[1] ));
        // Apply the scale, and accumulate
        acc = _mm256_add_ps(_mm256_mul_ps( d, p ), acc);
    }

    *s = hsum_float_8(acc);
#else
    // scalar
    float sumf = 0.0;
    for (int i = 0; i < nb; i++) {
        const float d0 = x[i].d;
        const float d1 = y[i].d;

        const uint8_t * restrict p0 = x[i].qs;
        const  int8_t * restrict p1 = y[i].qs;

        int sumi = 0;
        for (int j = 0; j < V1_QK8_0/2; j++) {
            const uint8_t v0 = p0[j];

            const int i0 = (int8_t) (v0 & 0x0F) - 8;
            const int i1 = (int8_t) (v0 >>   4) - 8;

            const int i2 = p1[2*j + 0];
            const int i3 = p1[2*j + 1];

            sumi += i0*i2 + i1*i3;
        }
        sumf += d0*d1*sumi;
    }
    *s = sumf;
#endif
}
