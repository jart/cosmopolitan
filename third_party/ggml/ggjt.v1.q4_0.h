#ifndef COSMOPOLITAN_THIRD_PARTY_GGML_V1_Q4_0_H_
#define COSMOPOLITAN_THIRD_PARTY_GGML_V1_Q4_0_H_
COSMOPOLITAN_C_START_

#define V1_QK4_0 32
typedef struct {
  float d;                   // delta
  uint8_t qs[V1_QK4_0 / 2];  // nibbles / quants
} block_v1_q4_0;

void dequantize_row_v1_q4_0(const void* restrict, float* restrict, int);
size_t ggml_quantize_v1_q4_0(const float*, void*, int, int, int64_t*);
void quantize_row_v1_q4_0(const float* restrict, void* restrict, int);
void quantize_row_v1_q4_0_reference(const float* restrict,
                                    block_v1_q4_0* restrict, int);
void ggml_vec_dot_v1_q4_0_q8_0(const int, float* restrict, const void* restrict,
                               const void* restrict);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_THIRD_PARTY_GGML_V1_Q4_0_H_ */
