#if 0
/*─────────────────────────────────────────────────────────────────╗
│ To the extent possible under law, Justine Tunney has waived      │
│ all copyright and related or neighboring rights to this file,    │
│ as it is written in the following disclaimers:                   │
│   • http://unlicense.org/                                        │
│   • http://creativecommons.org/publicdomain/zero/1.0/            │
╚─────────────────────────────────────────────────────────────────*/
#endif
#include "dsp/core/core.h"
#include "libc/calls/calls.h"
#include "libc/calls/struct/sigaction.h"
#include "libc/log/check.h"
#include "libc/macros.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/sysv/consts/itimer.h"
#include "libc/sysv/consts/sig.h"
#include "libc/time/time.h"

#define CSI  "s"
#define SGR1 "?80"
#define SGR2 "?81"

struct Ring {
  int i;     // read index
  int j;     // write index
  int n;     // total samples
  short* p;  // samples
};

struct Speaker {
  int rate;         // in hertz, e.g. 8000
  int channels;     // 1 = mono, 2 = stereo
  struct Ring buf;  // audio playback buffer
};

const int maxar = 31;
const int ptime = 20;

struct Speaker s;

void LoadAudioFile(struct Speaker* s, const char* path) {
  int rc;
  FILE* f;
  short buf[256];
  if (!(f = fopen(path, "rb"))) {
    fprintf(stderr, "failed to open file\n");
    exit(1);
  }
  for (;;) {
    rc = fread(buf, sizeof(short), sizeof(buf) / sizeof(short), f);
    if (rc) {
      s->buf.p = (short*)realloc(s->buf.p, (s->buf.n + rc) * sizeof(short));
      memcpy(s->buf.p + s->buf.n, buf, rc * sizeof(short));
      s->buf.n += rc;
    } else if (ferror(f)) {
      fprintf(stderr, "read error: %s\n", strerror(ferror(f)));
      exit(2);
    } else {
      break;
    }
  }
  fclose(f);
}

void OnAlrm(int sig) {
  int i, j;
  int count;
  int samps = s.rate / (1000 / ptime);
  for (i = 0; i < samps; i += count) {
    printf("\e[" SGR2);
    count = MIN(samps - i, maxar);
    for (j = 0; j < count; ++j) {
      printf(";%d", s.buf.p[s.buf.i++] & 0xffff);
      /* printf(";%d", mulaw(s.buf.p[s.buf.i++])); */
      if (s.buf.i == s.buf.n) break;
    }
    printf(CSI);
    if (s.buf.i == s.buf.n) break;
  }
  fflush(stdout);
  if (s.buf.i == s.buf.n) exit(0);
  fprintf(stderr, "\r\e[K%d / %d", s.buf.i, s.buf.n);
  fflush(stderr);
}

int main(int argc, char* argv[]) {
  if (!isatty(1)) exit(1);

  s.rate = 8000;
  s.channels = 1;
  LoadAudioFile(&s, "/home/jart/Music/numbers.s16");

  printf("\e[" SGR1 "%d;%d;0" CSI, s.rate, s.channels);
  fflush(stdout);

  struct sigaction sa = {.sa_handler = OnAlrm};
  struct itimerval it = {{0, ptime * 1000}, {0, ptime * 1000}};
  CHECK_NE(-1, sigaction(SIGALRM, &sa, 0));
  CHECK_NE(-1, setitimer(ITIMER_REAL, &it, 0));

  for (;;) {
    pause();
  }

  return 0;
}
