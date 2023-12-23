#ifndef COSMOPOLITAN_DSP_CORE_CORE_H_
#define COSMOPOLITAN_DSP_CORE_CORE_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

int alaw(int);
int unalaw(int);
int mulaw(int);
int unmulaw(int);
void *double2byte(long, const void *, double, double) vallocesque;
void *byte2double(long, const void *, double, double) vallocesque;
void *dct(float[restrict hasatleast 8][8], unsigned,
          float, float, float, float, float);
void *dctjpeg(float[restrict hasatleast 8][8], unsigned);
double det3(const double[3][3]) nosideeffect;
void *inv3(double[restrict 3][3], const double[restrict 3][3], double);
void *matmul3(double[restrict 3][3], const double[3][3], const double[3][3]);
void *vmatmul3(double[restrict 3], const double[3], const double[3][3]);
void *matvmul3(double[restrict 3], const double[3][3], const double[3]);
double rgb2stdtv(double) pureconst;
double rgb2lintv(double) pureconst;
double rgb2stdpc(double, double) pureconst;
double rgb2linpc(double, double) pureconst;
double tv2pcgamma(double, double) pureconst;

#ifndef __cplusplus
void sad16x8n(size_t n, short[n][8], const short[n][8]);
void float2short(size_t n, short[n][8], const float[n][8]);
void scalevolume(size_t n, int16_t[n][8], int);
#endif

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_DSP_CORE_CORE_H_ */
