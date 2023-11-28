#ifndef COSMOPOLITAN_THIRD_PARTY_GGML_GGJT_V2_Q5_0_H_
#define COSMOPOLITAN_THIRD_PARTY_GGML_GGJT_V2_Q5_0_H_
#include "third_party/ggml/fp16.h"
COSMOPOLITAN_C_START_

#define V2_QK5_0 32
typedef struct {
  ggml_fp16_t d;             // delta
  uint8_t qh[4];             // 5-th bit of quants
  uint8_t qs[V2_QK5_0 / 2];  // nibbles / quants
} block_v2_q5_0;

void dequantize_row_v2_q5_0(const void* restrict, float* restrict, int);
void quantize_row_v2_q5_0(const float* restrict, void* restrict, int);
size_t ggml_quantize_v2_q5_0(const float*, void*, int, int, int64_t*);
void ggml_vec_dot_v2_q5_0_q8_0(const int, float* restrict, const void* restrict,
                               const void* restrict);
void quantize_row_v2_q5_0_reference(const float* restrict,
                                    block_v2_q5_0* restrict, int);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_THIRD_PARTY_GGML_GGJT_V2_Q5_0_H_ */
