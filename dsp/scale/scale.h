#ifndef COSMOPOLITAN_DSP_SCALE_SCALE_H_
#define COSMOPOLITAN_DSP_SCALE_SCALE_H_
COSMOPOLITAN_C_START_

extern long gyarados_latency_;

extern signed char g_magikarp[8];
extern signed char g_magkern[8];
extern const signed char kMagikarp[8][8];
extern const signed char kMagkern[8][8];

struct SamplingSolution;
void FreeSamplingSolution(struct SamplingSolution *);
struct SamplingSolution *ComputeSamplingSolution(long, long, double, double,
                                                 double);

void *Scale2xX(long, long, void *, long, long);
void *Scale2xY(long, long, void *, long, long);
void *Magikarp2xX(long, long, void *, long, long);
void *Magikarp2xY(long, long, void *, long, long);
void *Magkern2xX(long, long, void *, long, long);
void *Magkern2xY(long, long, void *, long, long);
void *MagikarpY(long, long, void *, long, long, const void *, long, long,
                const signed char[8]);

void *GyaradosUint8(long, long, void *, long, long, const void *, long, long,
                    long, long, long, long, struct SamplingSolution *,
                    struct SamplingSolution *, bool);
void *GyaradosInt16(long, void *, long, const void *, long, long,
                    struct SamplingSolution *, bool);
void *EzGyarados(long, long, long, void *, long, long, long, const void *, long,
                 long, long, long, long, long, double, double, double, double);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_DSP_SCALE_SCALE_H_ */
