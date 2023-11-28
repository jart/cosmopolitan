#ifndef COSMOPOLITAN_THIRD_PARTY_VQSORT_H_
#define COSMOPOLITAN_THIRD_PARTY_VQSORT_H_
COSMOPOLITAN_C_START_

void vqsort_int64(int64_t *, size_t);
void vqsort_int64_avx2(int64_t *, size_t);
void vqsort_int64_sse4(int64_t *, size_t);
void vqsort_int64_ssse3(int64_t *, size_t);
void vqsort_int64_sse2(int64_t *, size_t);

void vqsort_int32(int32_t *, size_t);
void vqsort_int32_avx2(int32_t *, size_t);
void vqsort_int32_sse4(int32_t *, size_t);
void vqsort_int32_ssse3(int32_t *, size_t);
void vqsort_int32_sse2(int32_t *, size_t);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_THIRD_PARTY_VQSORT_H_ */
