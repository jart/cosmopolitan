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
#include <time.h>
#include "cosmoaudio.h"

#ifndef M_PIf
#define M_PIf 3.14159265358979323846f
#endif

int g_hz = 44100;
int g_channels = 2;
int g_generation = 0;
int g_freq = 440;

void data_callback(struct CosmoAudio *ca, float *outputSamples,
                   const float *inputSamples, int frameCount, int channels,
                   void *argument) {
  for (int i = 0; i < frameCount; i++) {
    float t = (float)g_generation++ / g_hz;
    if (g_generation == g_hz)
      g_generation = 0;
    float s = sinf(2 * M_PIf * g_freq * t);
    for (int j = 0; j < channels; j++)
      outputSamples[i * channels + j] = s;
  }
  (void)inputSamples;
  (void)argument;
  (void)ca;
}

int main() {

  struct CosmoAudioOpenOptions cao = {};
  cao.sizeofThis = sizeof(struct CosmoAudioOpenOptions);
  cao.deviceType = kCosmoAudioDeviceTypePlayback;
  cao.sampleRate = g_hz;
  cao.channels = g_channels;
  cao.dataCallback = data_callback;

  struct CosmoAudio *ca;
  if (cosmoaudio_open(&ca, &cao) != COSMOAUDIO_SUCCESS) {
    fprintf(stderr, "failed to open audio\n");
    return 1;
  }

  fgetc(stdin);

  cosmoaudio_close(ca);
}
