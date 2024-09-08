#if 0
/*─────────────────────────────────────────────────────────────────╗
│ To the extent possible under law, Justine Tunney has waived      │
│ all copyright and related or neighboring rights to this file,    │
│ as it is written in the following disclaimers:                   │
│   • http://unlicense.org/                                        │
│   • http://creativecommons.org/publicdomain/zero/1.0/            │
╚─────────────────────────────────────────────────────────────────*/
#endif
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "cosmoaudio.h"

#define SAMPLING_RATE 44100
#define WAVE_INTERVAL 440
#define CHANNELS      2

#ifndef M_PIf
#define M_PIf 3.14159265358979323846f
#endif

int main() {

  struct CosmoAudioOpenOptions cao = {0};
  cao.sizeofThis = sizeof(struct CosmoAudioOpenOptions);
  cao.deviceType = kCosmoAudioDeviceTypePlayback;
  cao.sampleRate = SAMPLING_RATE;
  cao.channels = CHANNELS;

  int status;
  struct CosmoAudio *ca;
  status = cosmoaudio_open(&ca, &cao);
  if (status != COSMOAUDIO_SUCCESS) {
    fprintf(stderr, "failed to open audio: %d\n", status);
    return 1;
  }

  float buf[256 * CHANNELS];
  for (int g = 0; g < SAMPLING_RATE;) {
    int frames = 1;
    status = cosmoaudio_poll(ca, NULL, &frames);
    if (status != COSMOAUDIO_SUCCESS) {
      fprintf(stderr, "failed to poll output: %d\n", status);
      return 2;
    }
    if (frames > 256)
      frames = 256;
    if (frames > SAMPLING_RATE - g)
      frames = SAMPLING_RATE - g;
    for (int f = 0; f < frames; ++f) {
      float t = (float)g++ / SAMPLING_RATE;
      float s = sinf(2 * M_PIf * WAVE_INTERVAL * t);
      for (int c = 0; c < CHANNELS; c++)
        buf[f * CHANNELS + c] = s;
    }
    status = cosmoaudio_write(ca, buf, frames);
    if (status != frames) {
      fprintf(stderr, "failed to write output: %d\n", status);
      return 3;
    }
  }

  status = cosmoaudio_flush(ca);
  if (status != COSMOAUDIO_SUCCESS) {
    fprintf(stderr, "failed to flush output: %d\n", status);
    return 4;
  }

  status = cosmoaudio_close(ca);
  if (status != COSMOAUDIO_SUCCESS) {
    fprintf(stderr, "failed to close audio: %d\n", status);
    return 5;
  }
}
