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
#include "libc/calls/struct/itimerval.h"
#include "libc/calls/struct/sigaction.h"
#include "libc/fmt/nf32.h"
#include "libc/intrin/bits.h"
#include "libc/log/check.h"
#include "libc/macros.internal.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/append.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/itimer.h"
#include "libc/sysv/consts/sig.h"
#include "libc/time/time.h"

/**
 * @fileoverview experimental way to play audio in terminals
 *
 * This is an stdio application that prints audio samples. The terminal
 * on the other end, needs to be able to understand the Nf sequences we
 * use here, which should be invisible to the terminal sort of like out
 * of band signalling.
 *
 * To play audio with a supporting terminal:
 *
 *     make -j8 o//examples/ttyaudio.com
 *     wget https://justine.lol/numbers.s16
 *     o//examples/ttyaudio.com numbers.s16
 *
 * To reveal the inband ansi audio transmission:
 *
 *     o//examples/ttyaudio.com numbers.s16 2>/dev/null |
 *       o//tool/viz/bing.com |
 *       o//tool/viz/fold.com
 *
 */

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
  int codec;        // 0 = s16, 2 = µ-Law
  int channels;     // 1 = mono, 2 = stereo
  struct Ring buf;  // audio playback buffer
};

const int maxar = 31;
const int ptime = 20;

struct Speaker s;

void OnAlrm(int sig) {
}

void LoadAudioFile(struct Speaker* s, const char* path) {
  int rc;
  FILE* f;
  short buf[1024];
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

int main(int argc, char* argv[]) {
  if (argc < 2) return 1;
  if (!isatty(0)) exit(1);

  s.rate = 8000;
  s.channels = 1;
  s.codec = 0;
  LoadAudioFile(&s, argv[1]);

  char nf[21];
  char* obuf = 0;
  appendw(&obuf, READ16LE("\e%"));
  appendd(&obuf, nf, EncodeNf32(nf, s.rate) - nf);
  appendw(&obuf, '/');
  appendd(&obuf, nf, EncodeNf32(nf, s.channels) - nf);
  appendw(&obuf, '/');
  appendd(&obuf, nf, EncodeNf32(nf, s.codec) - nf);
  appendw(&obuf, '0');
  write(1, obuf, appendz(obuf).i);
  free(obuf);

  struct sigaction sa = {.sa_handler = OnAlrm};
  struct itimerval it = {{0, ptime * 1000}, {0, ptime * 1000}};
  CHECK_NE(-1, sigaction(SIGALRM, &sa, 0));
  CHECK_NE(-1, setitimer(ITIMER_REAL, &it, 0));

  for (;;) {
    char* p;
    int count;
    char nf[22];
    int i, j, x;
    char* obuf = 0;
    int samps = s.rate / (1000 / ptime);
    appendw(&obuf, READ16LE("\e "));
    for (i = 0; i < samps; ++i) {
      if (s.codec == 1) {
        x = mulaw(s.buf.p[s.buf.i++]);
      } else {
        x = s.buf.p[s.buf.i++] & 0xffff;
      }
      *(p = EncodeNf32(nf, x)) = '/';
      appendd(&obuf, nf, p + 1 - nf);
      if (s.buf.i == s.buf.n) break;
    }
    appendw(&obuf, '0');
    write(1, obuf, appendz(obuf).i);
    free(obuf);
    fprintf(stderr, "\r\e[K%d / %d", s.buf.i, s.buf.n);
    fflush(stderr);
    pause();
  }

  return 1;
}
