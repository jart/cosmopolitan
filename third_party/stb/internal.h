#ifndef COSMOPOLITAN_THIRD_PARTY_STB_INTERNAL_H_
#define COSMOPOLITAN_THIRD_PARTY_STB_INTERNAL_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

void stbi__YCbCr_to_RGB_row(unsigned char *, const unsigned char *,
                            const unsigned char *, const unsigned char *,
                            unsigned, unsigned) _Hide;
int stbi__YCbCr_to_RGB_row$sse2(unsigned char *, const unsigned char *,
                                const unsigned char *, const unsigned char *,
                                unsigned) _Hide;
void stbi__idct_simd$sse(unsigned char *out, int out_stride,
                         short data[64]) _Hide;
void stbi__idct_simd$avx(unsigned char *out, int out_stride,
                         short data[64]) _Hide;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_THIRD_PARTY_STB_INTERNAL_H_ */
