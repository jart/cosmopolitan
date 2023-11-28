#ifndef COSMOPOLITAN_THIRD_PARTY_GGML_V1_Q4_1_H_
#define COSMOPOLITAN_THIRD_PARTY_GGML_V1_Q4_1_H_
COSMOPOLITAN_C_START_

#define V1_QK4_1 32
typedef struct {
  float d;                   // delta
  float m;                   // min
  uint8_t qs[V1_QK4_1 / 2];  // nibbles / quants
} block_v1_q4_1;

void dequantize_row_v1_q4_1(const void* restrict, float* restrict, int);
size_t ggml_quantize_v1_q4_1(const float*, void*, int, int, int64_t*);
void quantize_row_v1_q4_1(const float* restrict, void* restrict, int);
void ggml_vec_dot_v1_q4_1_q8_1(const int, float* restrict, const void* restrict,
                               const void* restrict);
void quantize_row_v1_q4_1_reference(const float* restrict, void* restrict, int);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_THIRD_PARTY_GGML_V1_Q4_1_H_ */
