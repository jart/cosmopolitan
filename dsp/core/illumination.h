#ifndef COSMOPOLITAN_DSP_CORE_ILLUMINANT_H_
#define COSMOPOLITAN_DSP_CORE_ILLUMINANT_H_
COSMOPOLITAN_C_START_

extern const double kBradford[3][3];
extern const double kIlluminantA[3];
extern const double kIlluminantAD10[3];
extern const double kIlluminantC[3];
extern const double kIlluminantCD10[3];
extern const double kIlluminantD50[3];
extern const double kIlluminantD50D10[3];
extern const double kIlluminantD55[3];
extern const double kIlluminantD55D10[3];
extern const double kIlluminantD65[3];
extern const double kIlluminantD65D10[3];
extern const double kIlluminantD75[3];
extern const double kIlluminantD75D10[3];
extern const double kIlluminantF2[3];
extern const double kIlluminantF2D10[3];
extern const double kIlluminantF7[3];
extern const double kIlluminantF7D10[3];
extern const double kIlluminantF11[3];
extern const double kIlluminantF11D10[3];

void *GetChromaticAdaptationMatrix(double[3][3], const double[3],
                                   const double[3]);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_DSP_CORE_ILLUMINANT_H_ */
