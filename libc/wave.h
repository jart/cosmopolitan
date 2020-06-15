#ifndef COSMOPOLITAN_LIBC_WAVE_H_
#define COSMOPOLITAN_LIBC_WAVE_H_

#define kWaveAlign 4

#define kWaveMagnumRiff 0x46464952u /* "RIFF" */
#define kWaveMagnumWave 0x46464952u /* "WAVE" */

#define WAVE_AUDIOFORMAT_PCM 1

#define WAVE_OFFSET_CHUNK_ID        0
#define WAVE_OFFSET_CHUNK_SIZE      4
#define WAVE_OFFSET_FORMAT          8
#define WAVE_OFFSET_SUBCHUNK_1_ID   12
#define WAVE_OFFSET_SUBCHUNK_1_SIZE 16
#define WAVE_OFFSET_AUDIOFORMAT     20
#define WAVE_OFFSET_CHANNELS        22
#define WAVE_OFFSET_SAMPLERATE      24
#define WAVE_OFFSET_BYTERATE        28
#define WAVE_OFFSET_BLOCKALIGN      32
#define WAVE_OFFSET_BITSPERSAMPLE   34
#define WAVE_OFFSET_SUBCHUNK_2_ID   36
/* #define WAVE_OFFSET_SUBCHUNK_2_ID   40 */

#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

/**
 * @fileoverview RIFF WAVE data structures.
 */

#define WAVE_MAGIC(P)       read64le(P)
#define WAVE_MAGIC_WRITE(P) read64le(P)

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_WAVE_H_ */
