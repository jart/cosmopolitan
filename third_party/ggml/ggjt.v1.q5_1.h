#ifndef COSMOPOLITAN_THIRD_PARTY_GGML_GGJT_V1_Q5_1_H_
#define COSMOPOLITAN_THIRD_PARTY_GGML_GGJT_V1_Q5_1_H_
#include "third_party/ggml/fp16.h"
COSMOPOLITAN_C_START_

#define V1_QK5_1 32
typedef struct {
  ggml_fp16_t d;             // delta
  ggml_fp16_t m;             // min
  uint8_t qh[4];             // 5-th bit of quants
  uint8_t qs[V1_QK5_1 / 2];  // nibbles / quants
} block_v1_q5_1;

void dequantize_row_v1_q5_1(const void* restrict, float* restrict, int);
void quantize_row_v1_q5_1(const float* restrict, void* restrict, int);
size_t ggml_quantize_v1_q5_1(const float*, void*, int, int, int64_t*);
void ggml_vec_dot_v1_q5_1_q8_1(const int, float* restrict, const void* restrict,
                               const void* restrict);
void quantize_row_v1_q5_1_reference(const float* restrict,
                                    block_v1_q5_1* restrict, int);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_THIRD_PARTY_GGML_GGJT_V1_Q5_1_H_ */
