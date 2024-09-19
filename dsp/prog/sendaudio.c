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
#include <netdb.h>
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
 * @fileoverview sends audio from microphone to remote computer
 * @see dsp/prog/recvaudio.c
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

uint32_t host2ip(const char* host) {
  uint32_t ip;
  if ((ip = inet_addr(host)) != -1u)
    return ip;
  int rc;
  struct addrinfo* ai = NULL;
  struct addrinfo hint = {AI_NUMERICSERV, AF_INET, SOCK_STREAM, IPPROTO_TCP};
  if ((rc = getaddrinfo(host, "0", &hint, &ai))) {
    fprintf(stderr, "%s: %s\n", host, gai_strerror(rc));
    exit(50 + rc);
  }
  ip = ntohl(((struct sockaddr_in*)ai->ai_addr)->sin_addr.s_addr);
  freeaddrinfo(ai);
  return ip;
}

int main(int argc, char* argv[]) {

  if (argc != 2) {
    fprintf(stderr, "%s: missing host argument\n", argv[0]);
    return 1;
  }

  // get host argument
  const char* remote_host = argv[1];
  uint32_t ip = host2ip(remote_host);

  // connect to server
  int client;
  if ((client = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
    perror(remote_host);
    return 3;
  }
  struct sockaddr_in addr = {.sin_family = AF_INET,
                             .sin_port = htons(PORT),
                             .sin_addr.s_addr = htonl(ip)};
  if (connect(client, (struct sockaddr*)&addr, sizeof(addr))) {
    perror(remote_host);
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
  cao.deviceType = kCosmoAudioDeviceTypeCapture;
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
    // read from microphone
    float buf32[CHUNK_FRAMES];
    cosmoaudio_poll(ca, (int[]){CHUNK_FRAMES}, 0);
    cosmoaudio_read(ca, buf32, CHUNK_FRAMES);
    short buf16[CHUNK_FRAMES];
    for (int i = 0; i < CHUNK_FRAMES; ++i)
      buf16[i] = toshort(buf32[i]);

    // send to server
    if (write(client, buf16, CHUNK_FRAMES * sizeof(short)) == -1) {
      if (errno == EINTR && g_done)
        break;
      perror(remote_host);
      return 7;
    }

    // print loudness in ascii
    char meter[21];
    double db = rms_to_db(rms(buf32, CHUNK_FRAMES));
    format_decibel_meter(meter, 20, db);
    printf("\r%s| %+6.2f dB", meter, db);
    fflush(stdout);
  }

  // clean up resources
  cosmoaudio_close(ca);
  close(client);
}
