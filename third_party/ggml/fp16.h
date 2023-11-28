#ifndef COSMOPOLITAN_THIRD_PARTY_GGML_F16_H_
#define COSMOPOLITAN_THIRD_PARTY_GGML_F16_H_
COSMOPOLITAN_C_START_

#ifdef __ARM_NEON
// we use the built-in 16-bit float type
typedef __fp16 ggml_fp16_t;
#else
typedef uint16_t ggml_fp16_t;
#endif

void ggml_fp16_init(void);

// convert FP16 <-> FP32
float ggml_fp16_to_fp32(ggml_fp16_t x);
ggml_fp16_t ggml_fp32_to_fp16(float x);

void ggml_fp16_to_fp32_row(const ggml_fp16_t* x, float* y, size_t n);
void ggml_fp32_to_fp16_row(const float* x, ggml_fp16_t* y, size_t n);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_THIRD_PARTY_GGML_F16_H_ */
