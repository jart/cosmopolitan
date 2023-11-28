#ifndef COSMOPOLITAN_THIRD_PARTY_GGML_GGJT_V2_Q8_0_H_
#define COSMOPOLITAN_THIRD_PARTY_GGML_GGJT_V2_Q8_0_H_
COSMOPOLITAN_C_START_

#define V2_QK8_0 32
typedef struct {
  float d;              // delta
  int8_t qs[V2_QK8_0];  // quants
} block_v2_q8_0;

void dequantize_row_v2_q8_0(const void* restrict, float* restrict, int);
void quantize_row_v2_q8_0(const float* restrict, void* restrict, int);
size_t ggml_quantize_v2_q8_0(const float*, void*, int, int, int64_t*);
void ggml_vec_dot_v2_q8_0_q8_0(const int, float* restrict, const void* restrict,
                               const void* restrict);
void quantize_row_v2_q8_0_reference(const float* restrict,
                                    block_v2_q8_0* restrict, int);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_THIRD_PARTY_GGML_GGJT_V2_Q8_0_H_ */
