#if 0
/*─────────────────────────────────────────────────────────────────╗
│ To the extent possible under law, Justine Tunney has waived      │
│ all copyright and related or neighboring rights to this file,    │
│ as it is written in the following disclaimers:                   │
│   • http://unlicense.org/                                        │
│   • http://creativecommons.org/publicdomain/zero/1.0/            │
╚─────────────────────────────────────────────────────────────────*/
#endif
#include <cosmoaudio.h>
#include <math.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/**
 * @fileoverview prints ascii meter of microphone loudness
 *
 * 0. -60 dB is nearly silent, barely audible, even in a quiet room
 * 1. -50 dB is very quiet background sounds
 * 2. -40 dB is quiet ambient noise
 * 3. -30 dB is clear but soft sounds
 * 4. -20 dB is moderate volume, comfortable for extended listening
 * 5. -10 dB is fairly loud, but not uncomfortable
 * 6.  -6 dB is loud, but not at full volume
 * 7.  -3 dB is very loud, approaching system limits
 * 8.  -1 dB is extremely loud, just below maximum
 * 9.  -0 dB is maximum volume without distortion
 */

#define SAMPLING_RATE     44100
#define ASCII_METER_WIDTH 20
#define FRAMES_PER_SECOND 30
#define MIN_DECIBEL       -60
#define MAX_DECIBEL       0
#define DEBUG_LOG         1

sig_atomic_t g_done;

void on_signal(int sig) {
  g_done = 1;
}

// computes root of mean squares
double rms(float* p, int n) {
  double s = 0;
  for (int i = 0; i < n; ++i)
    s += p[i] * p[i];
  return sqrt(s / n);
}

// converts rms to decibel
double rms_to_db(double rms) {
  double db = 20 * log10(rms);
  db = fmin(db, MAX_DECIBEL);
  db = fmax(db, MIN_DECIBEL);
  return db;
}

int main() {
  signal(SIGINT, on_signal);

  // how many samples should we process at once
  int chunkFrames = SAMPLING_RATE / FRAMES_PER_SECOND;

  // configure cosmo audio
  struct CosmoAudioOpenOptions cao = {0};
  cao.sizeofThis = sizeof(struct CosmoAudioOpenOptions);
  cao.deviceType = kCosmoAudioDeviceTypeCapture;
  cao.sampleRate = SAMPLING_RATE;
  cao.bufferFrames = chunkFrames * 2;
  cao.debugLog = DEBUG_LOG;
  cao.channels = 1;

  // connect to microphone
  int status;
  struct CosmoAudio* ca;
  status = cosmoaudio_open(&ca, &cao);
  if (status != COSMOAUDIO_SUCCESS) {
    fprintf(stderr, "failed to open microphone: %d\n", status);
    return 1;
  }

  // allocate memory for audio work area
  float* chunk = malloc(chunkFrames * sizeof(float));
  if (!chunk) {
    fprintf(stderr, "out of memory\n");
    return 1;
  }

  while (!g_done) {

    // wait for full chunk of audio to become available
    int need_in_frames = chunkFrames;
    status = cosmoaudio_poll(ca, &need_in_frames, NULL);
    if (status != COSMOAUDIO_SUCCESS) {
      fprintf(stderr, "failed to poll microphone: %d\n", status);
      return 2;
    }

    // read audio frames from microphone ring buffer
    status = cosmoaudio_read(ca, chunk, chunkFrames);
    if (status != chunkFrames) {
      fprintf(stderr, "failed to read microphone: %d\n", status);
      return 3;
    }

    // convert audio chunk to to ascii meter
    char s[ASCII_METER_WIDTH + 1] = {0};
    double db = rms_to_db(rms(chunk, chunkFrames));
    double db_range = MAX_DECIBEL - MIN_DECIBEL;
    int filled_length = (db - MIN_DECIBEL) / db_range * ASCII_METER_WIDTH;
    for (int i = 0; i < ASCII_METER_WIDTH; ++i) {
      if (i < filled_length) {
        s[i] = '=';
      } else {
        s[i] = ' ';
      }
    }
    printf("\r%s| %+6.2f dB", s, db);
    fflush(stdout);
  }
  printf("\n");

  // clean up resources
  status = cosmoaudio_close(ca);
  if (status != COSMOAUDIO_SUCCESS) {
    fprintf(stderr, "failed to close microphone: %d\n", status);
    return 5;
  }
  free(chunk);
}
