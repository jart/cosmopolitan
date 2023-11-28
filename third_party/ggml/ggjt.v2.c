/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:4;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=4 sts=4 sw=4 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2023 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ Permission to use, copy, modify, and/or distribute this software for         │
│ any purpose with or without fee is hereby granted, provided that the         │
│ above copyright notice and this permission notice appear in all copies.      │
│                                                                              │
│ THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL                │
│ WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED                │
│ WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE             │
│ AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL         │
│ DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR        │
│ PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER               │
│ TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR             │
│ PERFORMANCE OF THIS SOFTWARE.                                                │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/assert.h"
#include "third_party/ggml/ggjt.v2.q4_0.h"
#include "third_party/ggml/ggjt.v2.q4_1.h"
#include "third_party/ggml/ggjt.v2.q5_0.h"
#include "third_party/ggml/ggjt.v2.q5_1.h"
#include "third_party/ggml/ggjt.v2.q8_0.h"
#include "third_party/ggml/ggjt.v2.q8_1.h"
#include "third_party/ggml/ggml.h"

static const int ggjt_v2_blck_size[GGML_TYPE_COUNT] = {
    [GGML_TYPE_F32]  = 1,
    [GGML_TYPE_F16]  = 1,
    [GGML_TYPE_Q4_0] = V2_QK4_0,
    [GGML_TYPE_Q4_1] = V2_QK4_1,
    [GGML_TYPE_Q5_0] = V2_QK5_0,
    [GGML_TYPE_Q5_1] = V2_QK5_1,
    [GGML_TYPE_Q8_0] = V2_QK8_0,
    [GGML_TYPE_Q8_1] = V2_QK8_1,
    [GGML_TYPE_I8]   = 1,
    [GGML_TYPE_I16]  = 1,
    [GGML_TYPE_I32]  = 1,
};

static const size_t ggjt_v2_type_size[GGML_TYPE_COUNT] = {
    [GGML_TYPE_F32]  = sizeof(float),
    [GGML_TYPE_F16]  = sizeof(ggml_fp16_t),
    [GGML_TYPE_Q4_0] = sizeof(block_v2_q4_0),
    [GGML_TYPE_Q4_1] = sizeof(block_v2_q4_1),
    [GGML_TYPE_Q5_0] = sizeof(block_v2_q5_0),
    [GGML_TYPE_Q5_1] = sizeof(block_v2_q5_1),
    [GGML_TYPE_Q8_0] = sizeof(block_v2_q8_0),
    [GGML_TYPE_Q8_1] = sizeof(block_v2_q8_1),
    [GGML_TYPE_I8]   = sizeof(int8_t),
    [GGML_TYPE_I16]  = sizeof(int16_t),
    [GGML_TYPE_I32]  = sizeof(int32_t),
};

static const char *const ggjt_v2_type_name[GGML_TYPE_COUNT] = {
    [GGML_TYPE_F32]  = "f32",
    [GGML_TYPE_F16]  = "f16",
    [GGML_TYPE_Q4_0] = "q4_0",
    [GGML_TYPE_Q4_1] = "q4_1",
    [GGML_TYPE_Q4_2] = "q4_2",
    [GGML_TYPE_Q5_0] = "q5_0",
    [GGML_TYPE_Q5_1] = "q5_1",
    [GGML_TYPE_Q8_0] = "q8_0",
    [GGML_TYPE_Q8_1] = "q8_1",
    [GGML_TYPE_I8]   = "i8",
    [GGML_TYPE_I16]  = "i16",
    [GGML_TYPE_I32]  = "i32",
};

static const bool ggjt_v2_is_quantized[GGML_TYPE_COUNT] = {
    [GGML_TYPE_F32]  = false,
    [GGML_TYPE_F16]  = false,
    [GGML_TYPE_Q4_0] = true,
    [GGML_TYPE_Q4_1] = true,
    [GGML_TYPE_Q5_0] = true,
    [GGML_TYPE_Q5_1] = true,
    [GGML_TYPE_Q8_0] = true,
    [GGML_TYPE_Q8_1] = true,
    [GGML_TYPE_I8]   = false,
    [GGML_TYPE_I16]  = false,
    [GGML_TYPE_I32]  = false,
};

static const quantize_chunk_f *const ggjt_v2_quantize_chunk[GGML_TYPE_COUNT] = {
    [GGML_TYPE_Q4_0] = (void *)ggml_quantize_v2_q4_0,
    [GGML_TYPE_Q4_1] = (void *)ggml_quantize_v2_q4_1,
    [GGML_TYPE_Q5_0] = (void *)ggml_quantize_v2_q5_0,
    [GGML_TYPE_Q5_1] = (void *)ggml_quantize_v2_q5_1,
    [GGML_TYPE_Q8_0] = (void *)ggml_quantize_v2_q8_0,
};

static const quantize_fns_t ggjt_v2_quantize_fns[GGML_TYPE_COUNT] = {
    [GGML_TYPE_Q4_0] = {
        .dequantize_row_q         = dequantize_row_v2_q4_0,
        .quantize_row_q           = quantize_row_v2_q4_0,
        .quantize_row_q_reference = (quantize_row_q_t) quantize_row_v2_q4_0_reference,
        .quantize_row_q_dot       = quantize_row_v2_q8_0,
        .vec_dot_q                = ggml_vec_dot_v2_q4_0_q8_0,
        .vec_dot_type             = GGML_TYPE_Q8_0,
    },
    [GGML_TYPE_Q4_1] = {
        .dequantize_row_q         = dequantize_row_v2_q4_1,
        .quantize_row_q           = quantize_row_v2_q4_1,
        .quantize_row_q_reference = (quantize_row_q_t) quantize_row_v2_q4_1_reference,
        .quantize_row_q_dot       = quantize_row_v2_q8_1,
        .vec_dot_q                = ggml_vec_dot_v2_q4_1_q8_1,
        .vec_dot_type             = GGML_TYPE_Q8_1,
    },
    [GGML_TYPE_Q5_0] = {
        .dequantize_row_q         = dequantize_row_v2_q5_0,
        .quantize_row_q           = quantize_row_v2_q5_0,
        .quantize_row_q_reference = (quantize_row_q_t) quantize_row_v2_q5_0_reference,
        .quantize_row_q_dot       = quantize_row_v2_q8_0,
        .vec_dot_q                = ggml_vec_dot_v2_q5_0_q8_0,
        .vec_dot_type             = GGML_TYPE_Q8_0,
    },
    [GGML_TYPE_Q5_1] = {
        .dequantize_row_q         = dequantize_row_v2_q5_1,
        .quantize_row_q           = quantize_row_v2_q5_1,
        .quantize_row_q_reference = (quantize_row_q_t) quantize_row_v2_q5_1_reference,
        .quantize_row_q_dot       = quantize_row_v2_q8_1,
        .vec_dot_q                = ggml_vec_dot_v2_q5_1_q8_1,
        .vec_dot_type             = GGML_TYPE_Q8_1,
    },
    [GGML_TYPE_Q8_0] = {
        .dequantize_row_q         = dequantize_row_v2_q8_0,
        .quantize_row_q           = quantize_row_v2_q8_0,
        .quantize_row_q_reference = (quantize_row_q_t) quantize_row_v2_q8_0_reference,
        .quantize_row_q_dot       = quantize_row_v2_q8_0,
        .vec_dot_q                = ggml_vec_dot_v2_q8_0_q8_0,
        .vec_dot_type             = GGML_TYPE_Q8_0,
    },
    [GGML_TYPE_Q8_1] = {
        .dequantize_row_q         = NULL,   // TODO
        .quantize_row_q           = quantize_row_v2_q8_1,
        .quantize_row_q_reference = (quantize_row_q_t) quantize_row_v2_q8_1_reference,
        .quantize_row_q_dot       = quantize_row_v2_q8_1,
        .vec_dot_q                = NULL,   // TODO
        .vec_dot_type             = GGML_TYPE_Q8_1,
    },
};

void ggjt_v2(void) {
    GGML_BLCK_SIZE = ggjt_v2_blck_size;
    GGML_TYPE_SIZE = ggjt_v2_type_size;
    GGML_TYPE_NAME = ggjt_v2_type_name;
    GGML_IS_QUANTIZED = ggjt_v2_is_quantized;
    quantize_fns = ggjt_v2_quantize_fns;
    GGML_QUANTIZE_CHUNK = ggjt_v2_quantize_chunk;
}
