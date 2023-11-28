#ifndef COSMOPOLITAN_THIRD_PARTY_GGML_GGJT_V1_Q8_1_H_
#define COSMOPOLITAN_THIRD_PARTY_GGML_GGJT_V1_Q8_1_H_
COSMOPOLITAN_C_START_

#define V1_QK8_1 32
typedef struct {
  float d;              // delta
  float s0;             // d * sum(qs[i]) low
  float s1;             // d * sum(qs[i]) high
  int8_t qs[V1_QK8_1];  // quants
} block_v1_q8_1;

void quantize_row_v1_q8_1(const float* restrict, void* restrict, int);
void quantize_row_v1_q8_1_reference(const float* restrict,
                                    block_v1_q8_1* restrict, int);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_THIRD_PARTY_GGML_GGJT_V1_Q8_1_H_ */
