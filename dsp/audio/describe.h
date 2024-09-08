#ifndef COSMOPOLITAN_DSP_AUDIO_DESCRIBE_H_
#define COSMOPOLITAN_DSP_AUDIO_DESCRIBE_H_
#include "dsp/audio/cosmoaudio/cosmoaudio.h"
COSMOPOLITAN_C_START_

const char *cosmoaudio_describe_status(char *, int, int);
const char *cosmoaudio_describe_open_options(
    char *, int, const struct CosmoAudioOpenOptions *);
const char *cosmoaudio_describe_poll_frames(char *, int, int *);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_DSP_AUDIO_DESCRIBE_H_ */
