#include <errno.h>
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <time.h>
#include "cosmoaudio.h"

#ifndef M_PIf
#define M_PIf 3.14159265358979323846f
#endif

int main() {

  int hz = 44100;
  int channels = 2;
  struct CosmoAudio *ca;
  if (cosmoaudio_open(&ca, hz, channels) != COSMOAUDIO_SUCCESS) {
    fprintf(stderr, "%s: failed to open audio\n", argv[0]);
    return 1;
  }

  int n = 1000;
  int sample = 0;
  float *buf = (float *)malloc(sizeof(float) * channels * n);
  for (;;) {
    for (int i = 0; i < 128; i++) {
      float freq = 440;
      float t = (float)sample++ / hz;
      if (sample == hz)
        sample = 0;
      buf[i * channels] = sinf(freq * 2.f * M_PIf * t);
      buf[i * channels + 1] = sinf(freq * 2.f * M_PIf * t);
    }
    cosmoaudio_write(ca, buf, 128);
  }
}
