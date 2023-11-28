#ifndef COSMOPOLITAN_THIRD_PARTY_GGML_GGJT_V1_Q4_2_H_
#define COSMOPOLITAN_THIRD_PARTY_GGML_GGJT_V1_Q4_2_H_
#include "third_party/ggml/fp16.h"
COSMOPOLITAN_C_START_

#define V1_QK4_2 16
typedef struct {
  ggml_fp16_t d;             // delta
  uint8_t qs[V1_QK4_2 / 2];  // nibbles / quants
} block_v1_q4_2;

void dequantize_row_v1_q4_2(const void* restrict, float* restrict, int);
void quantize_row_v1_q4_2(const float* restrict, void* restrict, int);
size_t ggml_quantize_v1_q4_2(const float*, void*, int, int, int64_t*);
void ggml_vec_dot_v1_q4_2_q8_0(const int, float* restrict, const void* restrict,
                               const void* restrict);
void quantize_row_v1_q4_2_reference(const float* restrict,
                                    block_v1_q4_2* restrict, int);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_THIRD_PARTY_GGML_GGJT_V1_Q4_2_H_ */
