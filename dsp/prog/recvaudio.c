#if 0
/*─────────────────────────────────────────────────────────────────╗
│ To the extent possible under law, Justine Tunney has waived      │
│ all copyright and related or neighboring rights to this file,    │
│ as it is written in the following disclaimers:                   │
│   • http://unlicense.org/                                        │
│   • http://creativecommons.org/publicdomain/zero/1.0/            │
╚─────────────────────────────────────────────────────────────────*/
#endif
#include <arpa/inet.h>
#include <assert.h>
#include <cosmoaudio.h>
#include <errno.h>
#include <math.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include "loudness.h"

/**
 * @fileoverview plays audio from remote computer on speaker
 * @see dsp/prog/sendaudio.c
 */

#define SAMPLING_RATE     44100
#define FRAMES_PER_SECOND 60
#define DEBUG_LOG         0
#define PORT              9834

#define CHUNK_FRAMES (SAMPLING_RATE / FRAMES_PER_SECOND)

static_assert(CHUNK_FRAMES * sizeof(short) < 1472,
              "audio chunks won't fit in udp ethernet packet");

sig_atomic_t g_done;

void onsig(int sig) {
  g_done = 1;
}

short toshort(float x) {
  return fmaxf(-1, fminf(1, x)) * 32767;
}

float tofloat(short x) {
  return x / 32768.f;
}

int main(int argc, char* argv[]) {

  // listen on udp port for audio
  int server;
  if ((server = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
    perror("socket");
    return 3;
  }
  struct sockaddr_in addr = {.sin_family = AF_INET, .sin_port = htons(PORT)};
  if (bind(server, (struct sockaddr*)&addr, sizeof(addr))) {
    perror("bind");
    return 4;
  }

  // setup signals
  struct sigaction sa;
  sa.sa_flags = 0;
  sa.sa_handler = onsig;
  sigemptyset(&sa.sa_mask);
  sigaction(SIGINT, &sa, 0);

  // configure cosmo audio
  struct CosmoAudioOpenOptions cao = {0};
  cao.sizeofThis = sizeof(struct CosmoAudioOpenOptions);
  cao.deviceType = kCosmoAudioDeviceTypePlayback;
  cao.sampleRate = SAMPLING_RATE;
  cao.bufferFrames = CHUNK_FRAMES * 2;
  cao.debugLog = DEBUG_LOG;
  cao.channels = 1;

  // connect to microphone and speaker
  int status;
  struct CosmoAudio* ca;
  status = cosmoaudio_open(&ca, &cao);
  if (status != COSMOAUDIO_SUCCESS) {
    fprintf(stderr, "failed to open audio: %d\n", status);
    return 5;
  }

  while (!g_done) {
    // read from network
    ssize_t got;
    short buf16[CHUNK_FRAMES];
    if ((got = read(server, buf16, CHUNK_FRAMES * sizeof(short))) == -1) {
      if (errno == EINTR)
        continue;
      perror("read");
      return 7;
    }
    if (got != CHUNK_FRAMES * sizeof(short)) {
      fprintf(stderr, "warning: got partial audio frame\n");
      continue;
    }

    // write to speaker
    float buf32[CHUNK_FRAMES];
    for (int i = 0; i < CHUNK_FRAMES; ++i)
      buf32[i] = tofloat(buf16[i]);
    cosmoaudio_poll(ca, 0, (int[]){CHUNK_FRAMES});
    cosmoaudio_write(ca, buf32, CHUNK_FRAMES);

    // print loudness in ascii
    char meter[21];
    double db = rms_to_db(rms(buf32, CHUNK_FRAMES));
    format_decibel_meter(meter, 20, db);
    printf("\r%s| %+6.2f dB", meter, db);
    fflush(stdout);
  }

  // clean up resources
  cosmoaudio_flush(ca);
  cosmoaudio_close(ca);
  close(server);
}
