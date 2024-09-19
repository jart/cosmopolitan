#ifndef COSMOPOLITAN_DSP_PROG_LOUDNESS_H_
#define COSMOPOLITAN_DSP_PROG_LOUDNESS_H_
#include <math.h>
#include <stdio.h>

#define MIN_DECIBEL -60
#define MAX_DECIBEL 0

// computes root of mean squares
static double rms(float *p, int n) {
  double s = 0;
  for (int i = 0; i < n; ++i)
    s += p[i] * p[i];
  return sqrt(s / n);
}

// converts rms to decibel
static double rms_to_db(double rms) {
  double db = 20 * log10(rms);
  db = fmin(db, MAX_DECIBEL);
  db = fmax(db, MIN_DECIBEL);
  return db;
}

// char meter[21];
// format_decibel_meter(meter, 20, rms_to_db(rms(samps, count)))
static char *format_decibel_meter(char *meter, int width, double db) {
  double range = MAX_DECIBEL - MIN_DECIBEL;
  int filled = (db - MIN_DECIBEL) / range * width;
  for (int i = 0; i < width; ++i) {
    if (i < filled) {
      meter[i] = '=';
    } else {
      meter[i] = ' ';
    }
  }
  meter[width] = 0;
  return meter;
}

#endif /* COSMOPOLITAN_DSP_PROG_LOUDNESS_H_ */
