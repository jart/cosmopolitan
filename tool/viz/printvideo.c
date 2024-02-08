/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ Permission to use, copy, modify, and/or distribute this software for         │
│ any purpose with or without fee is hereby granted, provided that the         │
│ above copyright notice and this permission notice appear in all copies.      │
│                                                                              │
│ THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL                │
│ WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED                │
│ WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE             │
│ AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL         │
│ DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR        │
│ PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER               │
│ TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR             │
│ PERFORMANCE OF THIS SOFTWARE.                                                │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "dsp/core/core.h"
#include "dsp/core/half.h"
#include "dsp/core/illumination.h"
#include "dsp/mpeg/mpeg.h"
#include "dsp/scale/scale.h"
#include "dsp/tty/quant.h"
#include "dsp/tty/tty.h"
#include "libc/assert.h"
#include "libc/calls/calls.h"
#include "libc/calls/internal.h"
#include "libc/calls/struct/framebufferfixedscreeninfo.h"
#include "libc/calls/struct/framebuffervirtualscreeninfo.h"
#include "libc/calls/struct/iovec.h"
#include "libc/calls/struct/itimerval.h"
#include "libc/calls/struct/sigaction.h"
#include "libc/calls/struct/siginfo.h"
#include "libc/calls/struct/sigset.h"
#include "libc/calls/struct/timespec.h"
#include "libc/calls/struct/winsize.h"
#include "libc/calls/termios.h"
#include "libc/calls/ucontext.h"
#include "libc/cxxabi.h"
#include "libc/errno.h"
#include "libc/fmt/conv.h"
#include "libc/fmt/itoa.h"
#include "libc/intrin/kprintf.h"
#include "libc/intrin/safemacros.internal.h"
#include "libc/intrin/xchg.internal.h"
#include "libc/log/check.h"
#include "libc/log/log.h"
#include "libc/macros.internal.h"
#include "libc/math.h"
#include "libc/mem/alg.h"
#include "libc/mem/arraylist.internal.h"
#include "libc/mem/mem.h"
#include "libc/nexgen32e/bench.h"
#include "libc/nexgen32e/x86feature.h"
#include "libc/nt/console.h"
#include "libc/nt/runtime.h"
#include "libc/runtime/runtime.h"
#include "libc/sock/sock.h"
#include "libc/sock/struct/pollfd.h"
#include "libc/stdio/internal.h"
#include "libc/stdio/rand.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/str/strwidth.h"
#include "libc/str/unicode.h"
#include "libc/sysv/consts/af.h"
#include "libc/sysv/consts/auxv.h"
#include "libc/sysv/consts/clock.h"
#include "libc/sysv/consts/ex.h"
#include "libc/sysv/consts/exit.h"
#include "libc/sysv/consts/f.h"
#include "libc/sysv/consts/fd.h"
#include "libc/sysv/consts/fileno.h"
#include "libc/sysv/consts/ipproto.h"
#include "libc/sysv/consts/itimer.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/mlock.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/ok.h"
#include "libc/sysv/consts/poll.h"
#include "libc/sysv/consts/prio.h"
#include "libc/sysv/consts/prot.h"
#include "libc/sysv/consts/sa.h"
#include "libc/sysv/consts/shut.h"
#include "libc/sysv/consts/sig.h"
#include "libc/sysv/consts/splice.h"
#include "libc/sysv/consts/termios.h"
#include "libc/sysv/consts/w.h"
#include "libc/sysv/errfuns.h"
#include "libc/thread/thread.h"
#include "libc/time/time.h"
#include "libc/x/xsigaction.h"
#include "third_party/getopt/getopt.internal.h"
#include "third_party/stb/stb_image_resize.h"
#include "tool/viz/lib/graphic.h"
#include "tool/viz/lib/knobs.h"
#include "tool/viz/lib/ycbcr.h"

/**
 * @fileoverview MPEG Video Player for Terminal.
 */

#define GAMMADELTA    0.1
#define NETBUFSIZ     (2 * 1024 * 1024)
#define MAX_FRAMERATE (1 / 60.)

#define USAGE \
  " [FLAGS] MPG\n\
Renders motion picture to teletypewriters.\n\
\n\
Flags & Keyboard Shortcuts:\n\
  -s         stats\n\
  -t         true color\n\
  -d         dithering\n\
  -3         ibm cp437 rendering\n\
  -4         unicode rendering\n\
  -a         ansi quantization\n\
  -x         xterm256 quantization\n\
  -A         assume ansi ansi palette\n\
  -T         assume tango ansi palette\n\
  -v         increases verbosity        [flag]\n\
  -L PATH    redirects stderr to path   [flag]\n\
  -y         yes to interactive prompts [flag]\n\
  -h or -?   shows this information     [flag]\n\
  UP/DOWN    adjust volume              [keyboard]\n\
  CTRL+L     redraw                     [keyboard]\n\
  CTRL+Z     suspend                    [keyboard]\n\
  CTRL+C     exit                       [keyboard]\n\
  q          quit                       [keyboard]\n\
\n\
Effects Shortcuts:\n\
\n\
  S          Toggle Swing (TV, PC)\n\
  Y          Toggle Black/White Mode\n\
  p          Toggle Primaries (BT.601, BT.709)\n\
  g          +Gamma           G          -Gamma\n\
  l          +Illumination    L          -Illumination\n\
  k          +LumaKernel      K          -LumaKernel\n\
  j          +ChromaKernel    J          -ChromaKernel\n\
  CTRL-G     {Unsharp,Sharp}\n\
\n\
Environment Variables:\n\
  SOX        overrides location of SoX executable\n\
  FFPLAY     overrides location of FFmpeg ffplay executable\n\
  ROWS=𝑦     sets height [inarticulate mode]\n\
  COLUMNS=𝑥  sets width  [inarticulate mode]\n\
  TERM=dumb  inarticulate mode\n\
\n\
Notes:\n\
\n\
Your video printer natively supports .mpg files. If your videos are\n\
in a different format, then it's fast and easy to convert them:\n\
\n\
    ffmpeg -i movie.mkv movie.mpg\n\
\n\
The terminal fonts we recommend are PragmataPro, Bitstream Vera Sans\n\
Mono (known as DejaVu Sans Mono in the open source community), Menlo,\n\
and Lucida Console.\n\
\n\
On Linux, playing audio requires either `sox` or `ffplay` being on\n\
the $PATH. Kitty is the fastest terminal. Alacritty also has a fast\n\
display. GNOME Terminal and xterm both work well in 256-color or ANSI\n\
mode.\n\
\n"

#define CTRL(C)   ((C) ^ 0100)
#define ALT(C)    ((033 << 010) | (C))
#define ARGZ(...) ((char *const[]){__VA_ARGS__, NULL})
#define MOD(X, Y) ((X) - (ABS(Y)) * ((X) / ABS(Y)))

#define BALLOC(B, A, N, NAME)               \
  ({                                        \
    INFOF("balloc/%s %,zu bytes", NAME, N); \
    *(B) = pvalloc(N);                      \
  })

#define TIMEIT(OUT_NANOS, FORM)                                           \
  do {                                                                    \
    struct timespec Start = timespec_real();                              \
    FORM;                                                                 \
    (OUT_NANOS) = timespec_tonanos(timespec_sub(timespec_real(), Start)); \
  } while (0)

typedef bool (*openspeaker_f)(void);

enum Sharp {
  kSharpNone,
  kSharpUnsharp,
  kSharpSharp,
  kSharpMAX,
};

enum Blur {
  kBlurNone,
  kBlurBox,
  kBlurGaussian,
  kBlurMAX,
};

struct NamedVector {
  char name[8];
  const double (*vector)[3];
};

struct VtFrame {
  size_t i, n;
  union {
    void *b;
    char *bytes;
  };
};

struct FrameCountRing {
  size_t i, n;
  float p[64]; /* seconds relative to starttime_ */
};

struct FrameBuffer {
  void *map;
  size_t size;
  char *path;
  int fd;
  struct FrameBufferFixedScreenInfo fscreen;
  struct FrameBufferVirtualScreenInfo vscreen;
};

static const struct itimerval kTimerDisarm = {
    {0, 0},
    {0, 0},
};

static const struct itimerval kTimerHalfSecondSingleShot = {
    {0, 0},
    {0, 500000},
};

static const struct NamedVector kPrimaries[] = {
    {"BT.601", &kBt601Primaries},
    {"BT.709", &kBt709Primaries},
};

static const struct NamedVector kLightings[] = {
    {"A", &kIlluminantA},           {"C", &kIlluminantC},
    {"D50", &kIlluminantD50},       {"D55", &kIlluminantD55},
    {"D65", &kIlluminantD65},       {"D75", &kIlluminantD75},
    {"F2", &kIlluminantF2},         {"F7", &kIlluminantF7},
    {"F11", &kIlluminantF11},       {"A-10", &kIlluminantAD10},
    {"C-10", &kIlluminantCD10},     {"D50-10", &kIlluminantD50D10},
    {"D55-10", &kIlluminantD55D10}, {"D65-10", &kIlluminantD65D10},
    {"D75-10", &kIlluminantD75D10}, {"F2-10", &kIlluminantF2D10},
    {"F7-10", &kIlluminantF7D10},   {"F11-10", &kIlluminantF11D10},
};

static plm_t *plm_;
static float gamma_;
static int volscale_;
static enum Blur blur_;
static enum Sharp sharp_;
static jmp_buf jb_, jbi_;
static double pary_, parx_;
static struct TtyIdent ti_;
static struct YCbCr *ycbcr_;
static bool emboss_, sobel_;
static volatile int playpid_;
static struct winsize wsize_;
static float hue_, sat_, lit_;
static void *xtcodes_, *audio_;
static struct FrameBuffer fb0_;
static unsigned chans_, srate_;
static volatile bool ignoresigs_;
static size_t dh_, dw_, framecount_;
static struct FrameCountRing fcring_;
static volatile bool resized_, piped_;
static int lumakernel_, chromakernel_;
static openspeaker_f tryspeakerfns_[4];
static int primaries_, lighting_, swing_;
static uint64_t t1, t2, t3, t4, t5, t6, t8;
static const char *sox_, *ffplay_, *patharg_;
static struct VtFrame vtframe_[2], *f1_, *f2_;
static struct Graphic graphic_[2], *g1_, *g2_;
static struct timespec deadline_, dura_, starttime_;
static bool yes_, stats_, dither_, ttymode_, istango_;
static struct timespec decode_start_, f1_start_, f2_start_;
static int16_t pcm_[PLM_AUDIO_SAMPLES_PER_FRAME * 2 / 8][8];
static int16_t pcmscale_[PLM_AUDIO_SAMPLES_PER_FRAME * 2 / 8][8];
static bool fullclear_, historyclear_, tuned_, yonly_, gotvideo_;
static int homerow_, lastrow_, playfd_, infd_, outfd_, speakerfails_;
static char status_[7][200], logpath_[PATH_MAX], fifopath_[PATH_MAX],
    chansstr_[32], sratestr_[32];

static void OnCtrlC(void) {
  longjmp(jb_, 1);
}

static void OnResize(void) {
  resized_ = true;
}

static void OnSigPipe(void) {
  piped_ = true;
}

static void OnSigChld(void) {
  playpid_ = 0, piped_ = true;
}

static void StrikeDownCrapware(int sig) {
  kill(playpid_, SIGKILL);
}

static struct timespec GetGraceTime(void) {
  return timespec_sub(deadline_, timespec_real());
}

static char *strntoupper(char *s, size_t n) {
  size_t i;
  for (i = 0; s[i] && i < n; ++i) {
    if ('a' <= s[i] && s[i] <= 'z') {
      s[i] -= 'a' - 'A';
    }
  }
  return s;
}

static int GetNamedVector(const struct NamedVector *choices, size_t n,
                          const char *s) {
  int i;
  char name[sizeof(choices->name)];
#pragma GCC push_options
#pragma GCC diagnostic ignored "-Wstringop-truncation"
  strncpy(name, s, sizeof(name));
#pragma GCC pop_options
  strntoupper(name, sizeof(name));
  for (i = 0; i < n; ++i) {
    if (memcmp(choices[i].name, name, sizeof(name)) == 0) {
      return i;
    }
  }
  return -1;
}

static int GetPrimaries(const char *s) {
  return GetNamedVector(kPrimaries, ARRAYLEN(kPrimaries), s);
}

static int GetLighting(const char *s) {
  return GetNamedVector(kLightings, ARRAYLEN(kLightings), s);
}

static bool CloseSpeaker(void) {
  int rc, wstatus;
  rc = 0;
  pthread_yield();
  if (playfd_) {
    rc |= close(playfd_);
    playfd_ = -1;
  }
  if (playpid_) {
    kill(playpid_, SIGTERM);
    xsigaction(SIGALRM, StrikeDownCrapware, SA_RESETHAND, 0, 0);
    setitimer(ITIMER_REAL, &kTimerHalfSecondSingleShot, NULL);
    while (playpid_) {
      if (waitpid(playpid_, &wstatus, 0) != -1) {
        rc |= WEXITSTATUS(wstatus);
      } else if (errno == EINTR) {
        continue;
      } else {
        rc = -1;
      }
      break;
    }
    playpid_ = 0;
    setitimer(ITIMER_REAL, &kTimerDisarm, NULL);
  }
  return !!rc;
}

static void ResizeVtFrame(struct VtFrame *f, size_t yn, size_t xn) {
  BALLOC(&f->b, 4096, 64 + yn * (xn * 32 + 8), __FUNCTION__);
  f->i = f->n = 0;
}

static float timespec_tofloat(struct timespec ts) {
  return ts.tv_sec + ts.tv_nsec * 1e-9;
}

static void RecordFactThatFrameWasFullyRendered(void) {
  fcring_.p[fcring_.i] =
      timespec_tofloat(timespec_sub(timespec_real(), starttime_));
  fcring_.n += 1;
  fcring_.i += 1;
  fcring_.i &= ARRAYLEN(fcring_.p) - 1;
}

static double MeasureFrameRate(void) {
  int i, j, n, m;
  if (fcring_.n) {
    m = ARRAYLEN(fcring_.p);
    n = MIN(fcring_.n, m);
    i = (fcring_.i - 1) & (m - 1);
    j = (fcring_.i - n) & (m - 1);
    return n / (fcring_.p[i] - fcring_.p[j]);
  } else {
    return 0;
  }
}

static bool ShouldUseFrameBuffer(void) {
  return fb0_.fd != -1;
}

static bool IsHighDefinition(long yn, long xn) {
  return yn * xn >= 1280 * 720;
}

static void ComputeColoringSolution(void) {
  YCbCrInit(&ycbcr_, yonly_, swing_, gamma_, *kPrimaries[primaries_].vector,
            *kLightings[lighting_].vector);
}

static void DimensionDisplay(void) {
  size_t yn, xn;
  double ratio, height, width;
  do {
    resized_ = false;
    if (ShouldUseFrameBuffer()) {
      pary_ = 1;
      parx_ = 1;
      dh_ = fb0_.vscreen.yres;
      dw_ = fb0_.vscreen.xres;
      wsize_.ws_row = fb0_.vscreen.yres_virtual;
      wsize_.ws_col = fb0_.vscreen.xres_virtual;
    } else {
      pary_ = 1;
      parx_ = 1;
      wsize_.ws_row = 25;
      wsize_.ws_col = 80;
      wsize_ = (struct winsize){.ws_row = 40, .ws_col = 80};
      if (tcgetwinsize(outfd_, &wsize_) == -1) {
        tcgetwinsize(0, &wsize_);
      }
      dh_ = wsize_.ws_row * 2;
      dw_ = wsize_.ws_col * 2;
    }
    ratio = g1_->xn;
    ratio /= g1_->yn;
    height = dh_;
    width = dw_;
    height = MIN(height, height * ratio);
    width = MIN(width, width * ratio);
    yn = height;
    xn = width;
    yn = ROUNDDOWN(yn, 2);
    xn = ROUNDDOWN(xn, 2);
    g2_ = resizegraphic(&graphic_[1], yn, xn);
    INFOF("%s 𝑑(%hu×%hu)×(%d,%d): 𝑔₁(%zu×%zu,r=%f) → 𝑔₂(%zu×%zu)",
          "DimensionDisplay", wsize_.ws_row, wsize_.ws_col, g1_->yn, g1_->xn,
          ratio, yn, xn);
    BALLOC(&xtcodes_, 64, ((g2_->yn) * g2_->xn + 8) * sizeof(struct TtyRgb),
           "xtcodes_");
    ResizeVtFrame(&vtframe_[0], (g2_->yn), g2_->xn);
    ResizeVtFrame(&vtframe_[1], (g2_->yn), g2_->xn);
    f1_ = &vtframe_[0];
    f2_ = &vtframe_[1];
    if (ttymode_) {
      homerow_ = MIN(wsize_.ws_row - HALF(g2_->yn),
                     HALF(wsize_.ws_row - HALF(g2_->yn)));
    }
    lastrow_ = homerow_ + HALF(g2_->yn);
    ComputeColoringSolution();
  } while (resized_);
}

static int WriteAudio(int fd, const void *data, size_t size, int deadlinems) {
  ssize_t rc;
  const char *p;
  size_t wrote, n;
  p = data;
  n = size;
  do {
  TryAgain:
    if ((rc = write(fd, p, n)) != -1) {
      wrote = rc;
      p += wrote;
      n -= wrote;
    } else if (errno == EINTR) {
      goto TryAgain;
    } else if (errno == EAGAIN) {
      if (poll((struct pollfd[]){{fd, POLLOUT}}, 1, deadlinems) == 0) {
        return etimedout();
      }
    } else {
      return -1;
    }
  } while (n);
  return 0;
}

static bool TrySpeaker(const char *prog, char *const *args) {
  int pipefds[2];
  CHECK_NE(-1, pipe2(pipefds, O_CLOEXEC));
  if (!(playpid_ = fork())) {
    dup2(pipefds[0], 0);
    dup2(fileno(__log_file), 1);
    dup2(fileno(__log_file), 2);
    close(fileno(__log_file));
    execv(prog, args);
    abort();
  }
  playfd_ = pipefds[1];
  return true;
}

static bool TrySox(void) {
  return TrySpeaker(sox_, ARGZ("play", "-q", "-c", chansstr_, "-traw",
                               "-esigned", "-b16", "-r", sratestr_, "-"));
}

static bool TryFfplay(void) {
  return TrySpeaker(ffplay_, ARGZ("ffplay", "-nodisp", "-loglevel", "quiet",
                                  "-fflags", "nobuffer", "-ac", chansstr_,
                                  "-ar", sratestr_, "-f", "s16le", "pipe:"));
}

static bool OpenSpeaker(void) {
  size_t i;
  static bool once, count;
  if (!once) {
    once = true;
    i = 0;
    if (ffplay_) tryspeakerfns_[i++] = TryFfplay;
    if (sox_) tryspeakerfns_[i++] = TrySox;
  }
  snprintf(fifopath_, sizeof(fifopath_), "%s%s.%d.%d.wav", __get_tmpdir(),
           firstnonnull(program_invocation_short_name, "unknown"), getpid(),
           count);
  for (i = 0; i < ARRAYLEN(tryspeakerfns_); ++i) {
    if (tryspeakerfns_[i]) {
      if (++speakerfails_ <= 2 && tryspeakerfns_[i]()) {
        return true;
      } else {
        speakerfails_ = 0;
        tryspeakerfns_[i] = NULL;
      }
    }
  }
  return false;
}

static void OnAudio(plm_t *mpeg, plm_samples_t *samples, void *user) {
  if (playfd_ != -1) {
    DEBUGF("OnAudio() [grace=%,ldns]", timespec_tonanos(GetGraceTime()));
    CHECK_EQ(2, chans_);
    CHECK_EQ(ARRAYLEN(pcm_) * 8, samples->count * chans_);
    float2short(ARRAYLEN(pcm_), pcm_, (void *)samples->interleaved);
    scalevolume(ARRAYLEN(pcm_), pcm_, volscale_);
    sad16x8n(ARRAYLEN(pcm_), pcm_, pcmscale_);
    DEBUGF("transcoded audio");
  TryAgain:
    if (WriteAudio(playfd_, pcm_, sizeof(pcm_), 1000) != -1) {
      DEBUGF("WriteAudio(%d, %zu) ok [grace=%,ldns]", playfd_,
             samples->count * 2, timespec_tonanos(GetGraceTime()));
    } else {
      WARNF("WriteAudio(%d, %zu) failed: %s", playfd_, samples->count * 2,
            strerror(errno));
      CloseSpeaker();
      if (OpenSpeaker()) {
        goto TryAgain;
      }
    }
  }
}

static void DescribeAlgorithms(char *p) {
  if (dither_ && TTYQUANT()->alg != kTtyQuantTrue) {
    p = stpcpy(p, " ℍithered");
  }
  if (yonly_) {
    p = stpcpy(p, " grayscaled");
  }
  p += sprintf(p, " magikarp:%d:%d", lumakernel_, chromakernel_);
  switch (TTYQUANT()->alg) {
    case kTtyQuantTrue:
      p = stpcpy(p, " true-color");
      break;
    case kTtyQuantXterm256:
      p = stpcpy(p, " xterm256");
      break;
    case kTtyQuantAnsi:
      p = stpcpy(p, " aixterm ansi");
      if (istango_) p = stpcpy(p, " tango");
      break;
    default:
      break;
  }
  switch (TTYQUANT()->blocks) {
    case kTtyBlocksCp437:
      p = stpcpy(p, " ibm cp437");
      break;
    case kTtyBlocksUnicode:
      p = stpcpy(p, " unicode");
      break;
    default:
      break;
  }
  *p++ = ' ';
  *p = '\0';
}

static char *StartRender(char *vt) {
  if (!ttymode_) vt += sprintf(vt, "\r\n\r\n");
  if (fullclear_) {
    vt += sprintf(vt, "\e[0m\e[H\e[J");
    fullclear_ = false;
  } else if (historyclear_) {
    vt += sprintf(vt, "\e[0m\e[H\e[J\e[3J");
    historyclear_ = false;
  }
  vt += sprintf(vt, "\e[%hhuH", homerow_ + 1);
  return vt;
}

static void EndRender(char *vt) {
  vt += sprintf(vt, "\e[0m");
  f2_->n = (intptr_t)vt - (intptr_t)f2_->b;
  f2_->i = 0;
}

static bool IsNonZeroFloat(float f) {
  return fabsf(f) > 0.001f;
}

static bool HasAdjustments(void) {
  return (IsNonZeroFloat(hue_) || IsNonZeroFloat(sat_) ||
          IsNonZeroFloat(lit_)) ||
         (emboss_ || sharp_ || blur_ || sobel_ || pf1_ || pf2_ || pf3_ ||
          pf4_ || pf5_ || pf6_ || pf7_ || pf8_ || pf9_ || pf10_ || pf11_ ||
          pf12_);
}

static char *DescribeAdjustments(char *p) {
  if (emboss_) p = stpcpy(p, " emboss");
  if (sobel_) p = stpcpy(p, " sobel");
  switch (sharp_) {
    case kSharpSharp:
      p = stpcpy(p, " sharp");
      break;
    case kSharpUnsharp:
      p = stpcpy(p, " unsharp");
      break;
    default:
      break;
  }
  switch (blur_) {
    case kBlurBox:
      p = stpcpy(p, " boxblur");
      break;
    case kBlurGaussian:
      p = stpcpy(p, " gaussian");
      break;
    default:
      break;
  }
  if (IsNonZeroFloat(hue_)) p += sprintf(p, " hue%+.2f", hue_);
  if (IsNonZeroFloat(sat_)) p += sprintf(p, " sat%+.2f", sat_);
  if (IsNonZeroFloat(lit_)) p += sprintf(p, " lit%+.2f", lit_);
  if (pf1_) p = stpcpy(p, " PF1");
  if (pf2_) p = stpcpy(p, " PF2");
  if (pf3_) p = stpcpy(p, " PF3");
  if (pf4_) p = stpcpy(p, " PF4");
  if (pf5_) p = stpcpy(p, " PF5");
  if (pf6_) p = stpcpy(p, " PF6");
  if (pf7_) p = stpcpy(p, " PF7");
  if (pf8_) p = stpcpy(p, " PF8");
  if (pf9_) p = stpcpy(p, " PF9");
  if (pf10_) p = stpcpy(p, " PF10");
  if (pf11_) p = stpcpy(p, " PF11");
  if (pf12_) p = stpcpy(p, " PF12");
  *p++ = ' ';
  *p++ = '\0';
  return p;
}

static const char *DescribeSwing(int swing) {
  switch (swing) {
    case 219:
      return "TV";
    case 255:
      return "PC";
    default:
      return "??";
  }
}

static void RenderIt(void) {
  long bpf;
  double bpc;
  char *vt, *p;
  unsigned yn, xn;
  struct TtyRgb bg, fg;
  yn = g2_->yn;
  xn = g2_->xn;
  vt = f2_->b;
  p = StartRender(vt);
  if (TTYQUANT()->alg == kTtyQuantTrue) {
    bg = (struct TtyRgb){0, 0, 0, 0};
    fg = (struct TtyRgb){0xee, 0xff, 0xff, 0};
    p = stpcpy(p, "\e[48;2;0;0;0;38;2;255;255;255m");
  } else if (TTYQUANT()->alg == kTtyQuantAnsi) {
    bg = g_ansi2rgb_[0];
    fg = g_ansi2rgb_[7];
    p +=
        sprintf(p, "\e[%d;%dm", 30 + g_ansi2rgb_[0].xt, 40 + g_ansi2rgb_[7].xt);
  } else {
    bg = (struct TtyRgb){0, 0, 0, 16};
    fg = (struct TtyRgb){0xff, 0xff, 0xff, 231};
    p = stpcpy(p, "\e[48;5;16;38;5;231m");
  }
  p = ttyraster(p, xtcodes_, yn, xn, bg, fg);
  if (ttymode_ && stats_) {
    bpc = bpf = p - vt;
    bpc /= wsize_.ws_row * wsize_.ws_col;
    sprintf(status_[4], " %s/%s/%s %d×%d → %u×%u pixels ",
            kPrimaries[primaries_].name, DescribeSwing(swing_),
            kLightings[lighting_].name, plm_get_width(plm_),
            plm_get_height(plm_), g2_->xn, g2_->yn);
    sprintf(status_[5], " decode:%,8luµs | magikarp:%,8luµs ",
            plmpegdecode_latency_, magikarp_latency_);
    sprintf(status_[1], " ycbcr2rgb:%,8luµs | gyarados:%,8luµs ",
            ycbcr2rgb_latency_, gyarados_latency_);
    sprintf(status_[0], " fx:%,ldµs %.6fbpc %,ldbpf %.6ffps ",
            lroundl(t6 / 1e3L), bpc, bpf, (size_t)(p - vt), MeasureFrameRate());
    sprintf(status_[2], " gamma:%.1f %hu columns × %hu lines of text ", gamma_,
            wsize_.ws_col, wsize_.ws_row);
    DescribeAlgorithms(status_[3]);
    p += sprintf(p, "\e[0m");
    if (HasAdjustments()) {
      DescribeAdjustments(status_[6]);
      p += sprintf(p, "\e[%d;%dH%s", lastrow_ - 7,
                   HALF(xn) - strwidth(status_[6], 0), status_[6]);
    }
    p += sprintf(p, "\e[%d;%dH%s", lastrow_ - 6,
                 HALF(xn) - strwidth(status_[4], 0), status_[4]);
    p += sprintf(p, "\e[%d;%dH%s", lastrow_ - 5,
                 HALF(xn) - strwidth(status_[5], 0), status_[5]);
    p += sprintf(p, "\e[%d;%dH%s", lastrow_ - 4,
                 HALF(xn) - strwidth(status_[1], 0), status_[1]);
    p += sprintf(p, "\e[%d;%dH%s", lastrow_ - 3,
                 HALF(xn) - strwidth(status_[0], 0), status_[0]);
    p += sprintf(p, "\e[%d;%dH%30s", lastrow_ - 2,
                 HALF(xn) - strwidth(status_[2], 0), status_[2]);
    p += sprintf(p, "\e[%d;%dH%s", lastrow_ - 1,
                 HALF(xn) - strwidth(status_[3], 0), status_[3]);
    p += sprintf(p, "\e[%d;%dH %s %s ", lastrow_ - 2, 2,
                 program_invocation_name, "");
    p += sprintf(p, "\e[%d;%dH %s ", lastrow_ - 1, 2,
                 "by justine tunney <jtunney@gmail.com>");
  }
  EndRender(p);
}

static void RasterIt(void) {
  static bool once;
  static void *buf;
  if (!once) {
    buf = _mapanon(ROUNDUP(fb0_.size, FRAMESIZE));
    once = true;
  }
  WriteToFrameBuffer(fb0_.vscreen.yres_virtual, fb0_.vscreen.xres_virtual, buf,
                     g2_->yn, g2_->xn, g2_->b, fb0_.vscreen.yres,
                     fb0_.vscreen.xres);
  memcpy(fb0_.map, buf, fb0_.size);
}

static void TranscodeVideo(plm_frame_t *pf) {
  CHECK_EQ(pf->cb.width, pf->cr.width);
  CHECK_EQ(pf->cb.height, pf->cr.height);
  DEBUGF("TranscodeVideo() [grace=%,ldns]", timespec_tonanos(GetGraceTime()));
  g2_ = &graphic_[1];
  t5 = 0;

  TIMEIT(t1, {
    pary_ = 2;
    if (pf1_) pary_ = 1.;
    if (pf2_) pary_ = (266 / 64.) * (900 / 1600.);
    pary_ *= plm_get_pixel_aspect_ratio(plm_);
    YCbCr2RgbScale(g2_->yn, g2_->xn, g2_->b, pf->y.height, pf->y.width,
                   (void *)pf->y.data, pf->cr.height, pf->cr.width,
                   (void *)pf->cb.data, (void *)pf->cr.data, pf->y.height,
                   pf->y.width, pf->cr.height, pf->cr.width, pf->height,
                   pf->width, pary_, parx_, &ycbcr_);
  });
  t2 = 0;
  t8 = 0;

  TIMEIT(t6, {
    switch (blur_) {
      case kBlurBox:
        boxblur(g2_);
        break;
      case kBlurGaussian:
        gaussian(g2_->yn, g2_->xn, g2_->b);
        break;
      default:
        break;
    }
    if (sobel_) sobel(g2_);
    if (emboss_) emboss(g2_);
    switch (sharp_) {
      case kSharpSharp:
        sharpen(3, g2_->yn, g2_->xn, g2_->b, g2_->yn, g2_->xn);
        break;
      case kSharpUnsharp:
        unsharp(3, g2_->yn, g2_->xn, g2_->b, g2_->yn, g2_->xn);
        break;
      default:
        break;
    }
    if (dither_ && TTYQUANT()->alg != kTtyQuantTrue) {
      dither(g2_->yn, g2_->xn, g2_->b, g2_->yn, g2_->xn);
    }
  });

  if (ShouldUseFrameBuffer()) {
    t3 = 0;
    TIMEIT(t4, RasterIt());
  } else {
    TIMEIT(t3, getxtermcodes(xtcodes_, g2_));
    TIMEIT(t4, RenderIt());
  }

  INFOF("𝑓%zu(%u×%u) %,zub (%f BPP) "
        "ycbcr=%,zuns "
        "scale=%,zuns "
        "lace=%,zuns "
        "fx=%,zuns "
        "quantize=%,zuns "
        "render=%,zuns",
        framecount_++, g2_->yn, g2_->xn, f2_->n,
        (f1_->n / (double)(g2_->yn * g2_->xn)), t1, t2, t8, t6, t3, t4);
}

static void OnVideo(plm_t *mpeg, plm_frame_t *pf, void *user) {
  gotvideo_ = true;
  if (f2_->n) {
    WARNF("video frame dropped");
  } else {
    TranscodeVideo(pf);
    if (!f1_->n) {
      struct VtFrame *t = f1_;
      f1_ = f2_, f2_ = t;
      f1_start_ = decode_start_;
    } else {
      f2_start_ = decode_start_;
    }
  }
}

static void OpenVideo(void) {
  size_t yn, xn;
  playfd_ = -1;
  INFOF("%s(%`'s)", "OpenVideo", patharg_);
  CHECK_NOTNULL((plm_ = plm_create_with_filename(patharg_)));
  swing_ = 219;
  xn = plm_get_width(plm_);
  yn = plm_get_height(plm_);
  lighting_ = GetLighting("D65");
  primaries_ = IsHighDefinition(yn, xn) ? GetPrimaries("BT.709")
                                        : GetPrimaries("BT.601");
  plm_set_video_decode_callback(plm_, OnVideo, NULL);
  plm_set_audio_decode_callback(plm_, OnAudio, NULL);
  plm_set_loop(plm_, false);
  FormatInt64(chansstr_, (chans_ = 2));
  FormatInt64(sratestr_, (srate_ = plm_get_samplerate(plm_)));
  if (plm_get_num_audio_streams(plm_) && OpenSpeaker()) {
    plm_set_audio_enabled(plm_, true, 0);
  } else {
    plm_set_audio_enabled(plm_, false, 0);
  }
  g2_ = g1_ = resizegraphic(&graphic_[0], yn, xn);
}

static ssize_t WriteVideoCall(void) {
  size_t amt;
  ssize_t rc;
  amt = min(4096 * 4, f1_->n - f1_->i);
  if ((rc = write(outfd_, f1_->bytes + f1_->i, amt)) != -1) {
    if ((f1_->i += rc) == f1_->n) {
      if (plm_get_audio_enabled(plm_)) {
        plm_set_audio_lead_time(
            plm_,
            max(0,
                min(timespec_tofloat(timespec_sub(timespec_real(), f1_start_)),
                    plm_get_samplerate(plm_) / PLM_AUDIO_SAMPLES_PER_FRAME)));
      }
      f1_start_ = f2_start_;
      f1_->i = f1_->n = 0;
      struct VtFrame *t = f1_;
      f1_ = f2_, f2_ = t;
      RecordFactThatFrameWasFullyRendered();
    }
  }
  return rc;
}

static void DrainVideo(void) {
  if (f1_ && f1_->n) {
    ttywrite(outfd_, f1_->bytes + f1_->i, f1_->n - f1_->i);
    f1_->i = f1_->n = 0;
  }
  if (f2_ && f2_->n) {
    f2_->i = f2_->n = 0;
  }
}

static void WriteVideo(void) {
  ssize_t rc;
  DEBUGF("write(tty) grace=%,ldns", timespec_tonanos(GetGraceTime()));
  if ((rc = WriteVideoCall()) != -1) {
    DEBUGF("write(tty) → %zd [grace=%,ldns]", rc,
           timespec_tonanos(GetGraceTime()));
  } else if (errno == EAGAIN || errno == EINTR) {
    DEBUGF("write(tty) → EINTR");
    longjmp(jbi_, 1);
  } else if (errno == EPIPE) {
    DEBUGF("write(tty) → EPIPE");
    longjmp(jb_, 1);
  } else {
    FATALF("write(tty) → %s", strerror(errno));
  }
}

static void RefreshDisplay(void) {
  if (f1_ && f1_->n) f1_->i = 0;
  DimensionDisplay();
  resized_ = false;
  historyclear_ = true;
  ttysend(outfd_, "\e[0m\e[H\e[3J");
}

static void SetQuant(enum TtyQuantizationAlgorithm alg,
                     enum TtyQuantizationChannels chans,
                     enum TtyBlocksSelection blocks) {
  tuned_ = true;
  ttyquantsetup(alg, chans, blocks);
}

static void SetQuantizationAlgorithm(enum TtyQuantizationAlgorithm alg) {
  SetQuant(alg, TTYQUANT()->chans, TTYQUANT()->blocks);
  /* TODO(jart): autotune */
}

static void SetDithering(bool dither) {
  tuned_ = true;
  dither_ = dither;
}

static optimizesize bool ProcessOptKey(int opt) {
  switch (opt) {
    case 's':
      stats_ = !stats_;
      return true;
    case '3':
      TTYQUANT()->blocks = kTtyBlocksCp437;
      return true;
    case '4':
      TTYQUANT()->blocks = kTtyBlocksUnicode;
      return true;
    case 'd':
      SetDithering(!dither_);
      return true;
    case 't':
      SetQuantizationAlgorithm(kTtyQuantTrue);
      return true;
    case 'a':
      SetQuantizationAlgorithm(kTtyQuantAnsi);
      return true;
    case 'x':
      SetQuantizationAlgorithm(kTtyQuantXterm256);
      return true;
    case 'A':
      istango_ = false;
      memcpy(g_ansi2rgb_, &kCgaPalette, sizeof(kCgaPalette));
      return true;
    case 'T':
      istango_ = true;
      memcpy(g_ansi2rgb_, &kTangoPalette, sizeof(kTangoPalette));
      return true;
    default:
      return false;
  }
}

static optimizesize void ReadKeyboard(void) {
  char b[64];
  int c, i, n, sgn;
  memset(b, -1, sizeof(b));
  b[0] = CTRL('B'); /* for eof case */
  if ((n = read(infd_, &b, sizeof(b))) != -1) {
    for (;;) {
      i = 0;
      c = b[i++];
      if (!ProcessOptKey(c)) {
        sgn = isupper(c) ? -1 : 1;
        switch (c) {
          case 'Y':
            yonly_ = !yonly_;
            ComputeColoringSolution();
            break;
          case 'S':
            swing_ = swing_ == 219 ? 255 : 219;
            ComputeColoringSolution();
            break;
          case 'p':
          case 'P':
            primaries_ = MOD(sgn + primaries_, ARRAYLEN(kPrimaries));
            ComputeColoringSolution();
            break;
          case 'l':
          case 'L':
            lighting_ = MOD(sgn + lighting_, ARRAYLEN(kLightings));
            ComputeColoringSolution();
            break;
          case 'g':
          case 'G':
            gamma_ += sgn * GAMMADELTA;
            g_xterm256_gamma += sgn * GAMMADELTA;
            ComputeColoringSolution();
            break;
          case 'k':
          case 'K':
            lumakernel_ = MOD(sgn + lumakernel_, ARRAYLEN(kMagikarp));
            memcpy(g_magikarp, kMagikarp[lumakernel_], sizeof(kMagikarp[0]));
            break;
          case 'j':
          case 'J':
            chromakernel_ = MOD(sgn + chromakernel_, ARRAYLEN(kMagkern));
            memcpy(g_magkern, kMagkern[chromakernel_], sizeof(kMagkern[0]));
            break;
          case 'q':
          case CTRL('C'):
            longjmp(jb_, 1);
            break;
          case CTRL('Z'):
            ttyshowcursor(outfd_);
            raise(SIGSTOP);
            break;
          case CTRL('G'):
            sharp_ = (sharp_ + 1) % kSharpMAX;
            break;
          case CTRL('\\'):
            raise(SIGQUIT);
            break;
          case CTRL('L'):
            RefreshDisplay();
            break;
          case '\e':
            if (n == 1) {
              longjmp(jb_, 1); /* \e <𝟷𝟶𝟶ms*VTIME> is ESC */
            }
            switch (b[i++]) {
              case '[':
                switch (b[i++]) {
                  case 'A': /* "\e[A" is up arrow */
                    ++volscale_;
                    break;
                  case 'B': /* "\e[B" is down arrow */
                    --volscale_;
                    break;
                  case 'C': /* "\e[C" is right arrow */
                    break;
                  case 'D': /* "\e[D" is left arrow */
                    break;
                  case '1':
                    switch (b[i++]) {
                      case '1':
                        switch (b[i++]) {
                          case '~': /* \e[11~ is F1 */
                            pf1_ = !pf1_;
                            break;
                          default:
                            break;
                        }
                        break;
                      case '2':
                        switch (b[i++]) {
                          case '~': /* \e[12~ is F2 */
                            pf2_ = !pf2_;
                            break;
                          default:
                            break;
                        }
                        break;
                      case '3':
                        switch (b[i++]) {
                          case '~': /* \e[13~ is F3 */
                            pf3_ = !pf3_;
                            break;
                          default:
                            break;
                        }
                        break;
                      case '4':
                        switch (b[i++]) {
                          case '~': /* \e[14~ is F4 */
                            pf4_ = !pf4_;
                            break;
                          default:
                            break;
                        }
                        break;
                      case '5':
                        switch (b[i++]) {
                          case '~': /* \e[15~ is F5 */
                            pf5_ = !pf5_;
                            break;
                          default:
                            break;
                        }
                        break;
                      case '7':
                        switch (b[i++]) {
                          case '~': /* \e[17~ is F6 */
                            pf6_ = !pf6_;
                            break;
                          default:
                            break;
                        }
                        break;
                      case '8':
                        switch (b[i++]) {
                          case '~': /* \e[18~ is F7 */
                            pf7_ = !pf7_;
                            break;
                          default:
                            break;
                        }
                        break;
                      case '9':
                        switch (b[i++]) {
                          case '~': /* \e[19~ is F8 */
                            pf8_ = !pf8_;
                            break;
                          default:
                            break;
                        }
                        break;
                      default:
                        break;
                    }
                    break;
                  case '2':
                    switch (b[i++]) {
                      case '0':
                        switch (b[i++]) {
                          case '~': /* \e[20~ is F9 */
                            pf9_ = !pf9_;
                            break;
                          default:
                            break;
                        }
                        break;
                      case '1':
                        switch (b[i++]) {
                          case '~': /* \e[21~ is F10 */
                            pf10_ = !pf10_;
                            break;
                          default:
                            break;
                        }
                        break;
                      case '3':
                        switch (b[i++]) {
                          case '~': /* \e[23~ is F11 */
                            pf11_ = !pf11_;
                            break;
                          default:
                            break;
                        }
                        break;
                      case '4':
                        switch (b[i++]) {
                          case '~': /* \e[24~ is F12 */
                            pf12_ = !pf12_;
                            break;
                          default:
                            break;
                        }
                        break;
                      default:
                        break;
                    }
                    break;
                  case '[':
                    switch (b[i++]) {
                      case 'A': /* \e[[A is F1 */
                        pf1_ = !pf1_;
                        break;
                      case 'B': /* \e[[B is F2 */
                        pf2_ = !pf2_;
                        break;
                      case 'C': /* \e[[C is F3 */
                        pf3_ = !pf3_;
                        break;
                      case 'D': /* \e[[D is F4 */
                        pf4_ = !pf4_;
                        break;
                      case 'E': /* \e[[E is F5 */
                        pf5_ = !pf5_;
                        break;
                      default:
                        break;
                    }
                    break;
                  default:
                    break;
                }
                break;
              case 'O':
                switch (b[i++]) {
                  case 'P': /* \eOP is F1 */
                    pf1_ = !pf1_;
                    break;
                  case 'Q': /* \eOQ is F2 */
                    pf2_ = !pf2_;
                    break;
                  case 'R': /* \eOR is F3 */
                    pf3_ = !pf3_;
                    break;
                  case 'S': /* \eOS is F4 */
                    pf4_ = !pf4_;
                    break;
                  case 'T': /* \eOT is F5 */
                    pf5_ = !pf5_;
                    break;
                  case 'U': /* \eOU is F6 */
                    pf6_ = !pf6_;
                    break;
                  case 'V': /* \eOV is F7 */
                    pf7_ = !pf7_;
                    break;
                  case 'W': /* \eOW is F8 */
                    pf8_ = !pf8_;
                    break;
                  case 'Y': /* \eOY is F10 */
                    pf10_ = !pf10_;
                    break;
                  case 'Z': /* \eOZ is F11 */
                    pf11_ = !pf11_;
                    break;
                  case '[': /* \eO[ is F12 */
                    pf12_ = !pf12_;
                    break;
                  default:
                    break;
                }
                break;
              default:
                break;
            }
            break;
          default:
            break;
        }
      }
      if ((n -= i) <= 0) {
        break;
      } else {
        memmove(b, b + i, sizeof(b) - i);
      }
    }
  } else if (errno == EINTR) {
    longjmp(jbi_, 1);
  }
}

static void PerformBestEffortIo(void) {
  int toto, pollms;
  struct pollfd fds[] = {
      {infd_, POLLIN},
      {outfd_, f1_ && f1_->n ? POLLOUT : 0},
  };
  pollms = MAX(0, timespec_tomillis(GetGraceTime()));
  DEBUGF("poll() ms=%,d", pollms);
  if ((toto = poll(fds, ARRAYLEN(fds), pollms)) != -1) {
    DEBUGF("poll() toto=%d [grace=%,ldns]", toto,
           timespec_tonanos(GetGraceTime()));
    if (toto) {
      if (fds[0].revents & (POLLIN | POLLERR)) ReadKeyboard();
      if (fds[1].revents & (POLLOUT | POLLERR)) WriteVideo();
    }
  } else if (errno == EINTR) {
    DEBUGF("poll() → EINTR");
    return;
  } else {
    FATALF("poll() → %s", strerror(errno));
  }
}

static void RestoreTty(void) {
  DrainVideo();
  if (ttymode_) ttysend(outfd_, "\r\n\e[J");
  ttymode_ = false;
  ttyraw(-1);
}

static void HandleSignals(void) {
  if (piped_) {
    WARNF("SIGPIPE");
    CloseSpeaker();
    piped_ = false;
  }
  if (resized_) {
    RefreshDisplay();
  }
}

static void PrintVideo(void) {
  struct timespec decode_last, decode_end, next_tick, lag;
  dura_ = timespec_frommicros(min(MAX_FRAMERATE, 1 / plm_get_framerate(plm_)) *
                              1e6);
  INFOF("framerate=%f dura=%f", plm_get_framerate(plm_), dura_);
  next_tick = deadline_ = decode_last = timespec_real();
  next_tick = timespec_add(next_tick, dura_);
  deadline_ = timespec_add(deadline_, dura_);
  do {
    DEBUGF("plm_decode [grace=%,ldns]", timespec_tonanos(GetGraceTime()));
    decode_start_ = timespec_real();
    plm_decode(plm_,
               timespec_tofloat(timespec_sub(decode_start_, decode_last)));
    decode_last = decode_start_;
    decode_end = timespec_real();
    lag = timespec_sub(decode_end, decode_start_);
    while (timespec_cmp(timespec_add(decode_end, lag), next_tick) > 0) {
      next_tick = timespec_add(next_tick, dura_);
    }
    deadline_ = timespec_sub(next_tick, lag);
    if (gotvideo_ || !plm_get_video_enabled(plm_)) {
      gotvideo_ = false;
      INFOF("entering printvideo event loop (lag=%,ldns, grace=%,ldns)",
            timespec_tonanos(lag), timespec_tonanos(GetGraceTime()));
    }
    do {
      if (!setjmp(jbi_)) {
        PerformBestEffortIo();
      }
      HandleSignals();
    } while (timespec_tomillis(GetGraceTime()) > 0);
  } while (plm_ && !plm_has_ended(plm_));
}

static bool AskUserYesOrNoQuestion(const char *prompt) {
  char c;
  if (yes_ || !ttymode_) return true;
  ttysend(outfd_, "\r\e[K");
  ttysend(outfd_, prompt);
  ttysend(outfd_, " [yn] ");
  poll((struct pollfd[]){{infd_, POLLIN}}, 1, -1);
  c = 0, read(infd_, &c, 1);
  ttysend(infd_, "\r\e[K");
  return c == 'y' || c == 'Y';
}

static bool CanPlayAudio(void) {
  if (ffplay_ || sox_) {
    return true;
  } else if (AskUserYesOrNoQuestion(
                 "ffplay not found; continue without audio?")) {
    return false;
  } else {
    longjmp(jb_, 1);
  }
}

static void PrintUsage(int rc, int fd) {
  tinyprint(fd, "Usage: ", program_invocation_name, USAGE, NULL);
  exit(rc);
}

static void GetOpts(int argc, char *argv[]) {
  int opt;
  snprintf(logpath_, sizeof(logpath_), "%s%s.log", __get_tmpdir(),
           firstnonnull(program_invocation_short_name, "unknown"));
  while ((opt = getopt(argc, argv, "?34AGSTVYabdfhnpstxyzvL:")) != -1) {
    switch (opt) {
      case 'y':
        yes_ = true;
        break;
      case 'v':
        ++__log_level;
        break;
      case 'L':
        snprintf(logpath_, sizeof(logpath_), "%s", optarg);
        break;
      case 'Y':
        yonly_ = true;
        break;
      case 'h':
      case '?':
      default:
        if (!ProcessOptKey(opt)) {
          if (opt == optopt) {
            PrintUsage(EXIT_SUCCESS, STDOUT_FILENO);
          } else {
            PrintUsage(EX_USAGE, STDERR_FILENO);
          }
        }
    }
  }
}

static void OnExit(void) {
  if (playpid_) kill(playpid_, SIGTERM), sched_yield();
  if (plm_) plm_destroy(plm_), plm_ = NULL;
  YCbCrFree(&ycbcr_);
  RestoreTty();
  ttyidentclear(&ti_);
  close(infd_), infd_ = -1;
  close(outfd_), outfd_ = -1;
  free(graphic_[0].b);
  free(graphic_[1].b);
  free(vtframe_[0].b);
  free(vtframe_[1].b);
  free(xtcodes_);
  free(audio_);
  CloseSpeaker();
}

static void MakeLatencyLittleLessBad(void) {
  LOGIFNEG1(sys_mlockall(MCL_CURRENT));
  LOGIFNEG1(nice(-5));
}

static void PickDefaults(void) {
  /*
   * Direct color ain't true color -- it just means xterm does the
   * xterm256 rgb quantization for you. we're better at xterm256
   * than xterm is, so we don't need the training wheels.
   *
   * strcmp(nulltoempty(getenv("TERM")), "xterm-direct") == 0
   */
  if (strcmp(nulltoempty(getenv("TERM")), "xterm-kitty") == 0) {
    ttyquantsetup(kTtyQuantTrue, TTYQUANT()->chans, kTtyBlocksUnicode);
  }
}

static void RenounceSpecialPrivileges(void) {
  if (issetugid()) {
    setegid(getgid());
    seteuid(getuid());
  }
}

#define FBIOGET_VSCREENINFO 0x4600
#define FBIOGET_FSCREENINFO 0x4602

static void TryToOpenFrameBuffer(void) {
  /*
   * Linux (i.e. without some X or Wayland thing running on top of it)
   * is barely able to display any non-ascii characters, so things look
   * much better if we can access the framebuffer.
   */
  int rc;
  fb0_.fd = -1;
  fb0_.path = NULL;
  if (!isempty(getenv("FRAMEBUFFER"))) {
    fb0_.path = strdup(getenv("FRAMEBUFFER"));
  } else if (strcmp(nulltoempty(getenv("TERM")), "linux") == 0) {
    fb0_.path = strdup("/dev/fb0");
  }
  if ((fb0_.fd = open(fb0_.path, O_RDWR)) != -1) {
    CHECK_NE(-1, (rc = ioctl(fb0_.fd, FBIOGET_FSCREENINFO, &fb0_.fscreen)));
    INFOF("ioctl(%s) → %d", "FBIOGET_FSCREENINFO", rc);
    INFOF("%s.%s=%.*s", "fb0_.fscreen", "id", sizeof(fb0_.fscreen.id),
          fb0_.fscreen.id);
    INFOF("%s.%s=%p", "fb0_.fscreen", "smem_start", fb0_.fscreen.smem_start);
    INFOF("%s.%s=%u", "fb0_.fscreen", "smem_len", fb0_.fscreen.smem_len);
    INFOF("%s.%s=%u", "fb0_.fscreen", "type", fb0_.fscreen.type);
    INFOF("%s.%s=%u", "fb0_.fscreen", "type_aux", fb0_.fscreen.type_aux);
    INFOF("%s.%s=%u", "fb0_.fscreen", "visual", fb0_.fscreen.visual);
    INFOF("%s.%s=%hu", "fb0_.fscreen", "xpanstep", fb0_.fscreen.xpanstep);
    INFOF("%s.%s=%hu", "fb0_.fscreen", "ypanstep", fb0_.fscreen.ypanstep);
    INFOF("%s.%s=%hu", "fb0_.fscreen", "ywrapstep", fb0_.fscreen.ywrapstep);
    INFOF("%s.%s=%u", "fb0_.fscreen", "line_length", fb0_.fscreen.line_length);
    INFOF("%s.%s=%p", "fb0_.fscreen", "mmio_start", fb0_.fscreen.mmio_start);
    INFOF("%s.%s=%u", "fb0_.fscreen", "mmio_len", fb0_.fscreen.mmio_len);
    INFOF("%s.%s=%u", "fb0_.fscreen", "accel", fb0_.fscreen.accel);
    INFOF("%s.%s=%#b", "fb0_.fscreen", "capabilities",
          fb0_.fscreen.capabilities);
    CHECK_NE(-1, (rc = ioctl(fb0_.fd, FBIOGET_VSCREENINFO, &fb0_.vscreen)));
    INFOF("ioctl(%s) → %d", "FBIOGET_VSCREENINFO", rc);
    INFOF("%s.%s=%u", "fb0_.vscreen", "xres", fb0_.vscreen.xres);
    INFOF("%s.%s=%u", "fb0_.vscreen", "yres", fb0_.vscreen.yres);
    INFOF("%s.%s=%u", "fb0_.vscreen", "xres_virtual",
          fb0_.vscreen.xres_virtual);
    INFOF("%s.%s=%u", "fb0_.vscreen", "yres_virtual",
          fb0_.vscreen.yres_virtual);
    INFOF("%s.%s=%u", "fb0_.vscreen", "xoffset", fb0_.vscreen.xoffset);
    INFOF("%s.%s=%u", "fb0_.vscreen", "yoffset", fb0_.vscreen.yoffset);
    INFOF("%s.%s=%u", "fb0_.vscreen", "bits_per_pixel",
          fb0_.vscreen.bits_per_pixel);
    INFOF("%s.%s=%u", "fb0_.vscreen", "grayscale", fb0_.vscreen.grayscale);
    INFOF("%s.%s=%u", "fb0_.vscreen.red", "offset", fb0_.vscreen.red.offset);
    INFOF("%s.%s=%u", "fb0_.vscreen.red", "length", fb0_.vscreen.red.length);
    INFOF("%s.%s=%u", "fb0_.vscreen.red", "msb_right",
          fb0_.vscreen.red.msb_right);
    INFOF("%s.%s=%u", "fb0_.vscreen.green", "offset",
          fb0_.vscreen.green.offset);
    INFOF("%s.%s=%u", "fb0_.vscreen.green", "length",
          fb0_.vscreen.green.length);
    INFOF("%s.%s=%u", "fb0_.vscreen.green", "msb_right",
          fb0_.vscreen.green.msb_right);
    INFOF("%s.%s=%u", "fb0_.vscreen.blue", "offset", fb0_.vscreen.blue.offset);
    INFOF("%s.%s=%u", "fb0_.vscreen.blue", "length", fb0_.vscreen.blue.length);
    INFOF("%s.%s=%u", "fb0_.vscreen.blue", "msb_right",
          fb0_.vscreen.blue.msb_right);
    INFOF("%s.%s=%u", "fb0_.vscreen.transp", "offset",
          fb0_.vscreen.transp.offset);
    INFOF("%s.%s=%u", "fb0_.vscreen.transp", "length",
          fb0_.vscreen.transp.length);
    INFOF("%s.%s=%u", "fb0_.vscreen.transp", "msb_right",
          fb0_.vscreen.transp.msb_right);
    INFOF("%s.%s=%u", "fb0_.vscreen", "nonstd", fb0_.vscreen.nonstd);
    INFOF("%s.%s=%u", "fb0_.vscreen", "activate", fb0_.vscreen.activate);
    INFOF("%s.%s=%u", "fb0_.vscreen", "height", fb0_.vscreen.height);
    INFOF("%s.%s=%u", "fb0_.vscreen", "width", fb0_.vscreen.width);
    INFOF("%s.%s=%u", "fb0_.vscreen", "accel_flags", fb0_.vscreen.accel_flags);
    INFOF("%s.%s=%u", "fb0_.vscreen", "pixclock", fb0_.vscreen.pixclock);
    INFOF("%s.%s=%u", "fb0_.vscreen", "left_margin", fb0_.vscreen.left_margin);
    INFOF("%s.%s=%u", "fb0_.vscreen", "right_margin",
          fb0_.vscreen.right_margin);
    INFOF("%s.%s=%u", "fb0_.vscreen", "upper_margin",
          fb0_.vscreen.upper_margin);
    INFOF("%s.%s=%u", "fb0_.vscreen", "lower_margin",
          fb0_.vscreen.lower_margin);
    INFOF("%s.%s=%u", "fb0_.vscreen", "hsync_len", fb0_.vscreen.hsync_len);
    INFOF("%s.%s=%u", "fb0_.vscreen", "vsync_len", fb0_.vscreen.vsync_len);
    INFOF("%s.%s=%u", "fb0_.vscreen", "sync", fb0_.vscreen.sync);
    INFOF("%s.%s=%u", "fb0_.vscreen", "vmode", fb0_.vscreen.vmode);
    INFOF("%s.%s=%u", "fb0_.vscreen", "rotate", fb0_.vscreen.rotate);
    INFOF("%s.%s=%u", "fb0_.vscreen", "colorspace", fb0_.vscreen.colorspace);
    fb0_.size = fb0_.fscreen.smem_len;
    CHECK_NE(MAP_FAILED,
             (fb0_.map = mmap(NULL, fb0_.size, PROT_READ | PROT_WRITE,
                              MAP_SHARED, fb0_.fd, 0)));
  }
}

int main(int argc, char *argv[]) {
  sigset_t wut;
  const char *s;
  ShowCrashReports();
  gamma_ = 2.4;
  volscale_ -= 2;
  dither_ = true;
  sigemptyset(&wut);
  sigaddset(&wut, SIGCHLD);
  sigaddset(&wut, SIGPIPE);
  sigprocmask(SIG_SETMASK, &wut, NULL);
  ShowCrashReports();
  fullclear_ = true;
  GetOpts(argc, argv);
  if (!tuned_) PickDefaults();
  if (optind == argc) PrintUsage(EX_USAGE, STDERR_FILENO);
  patharg_ = argv[optind];
  s = commandvenv("SOX", "sox");
  sox_ = s ? strdup(s) : 0;
  s = commandvenv("FFPLAY", "ffplay");
  ffplay_ = s ? strdup(s) : 0;
  if (!sox_ && !ffplay_) {
    fprintf(stderr, "please install either the "
                    "`play` (sox) or "
                    "`ffplay` (ffmpeg) "
                    "commands, so printvideo.com can play audio\n");
    usleep(10000);
  }
  infd_ = STDIN_FILENO;
  outfd_ = STDOUT_FILENO;
  if (!setjmp(jb_)) {
    xsigaction(SIGINT, OnCtrlC, 0, 0, NULL);
    xsigaction(SIGHUP, OnCtrlC, 0, 0, NULL);
    xsigaction(SIGTERM, OnCtrlC, 0, 0, NULL);
    xsigaction(SIGWINCH, OnResize, 0, 0, NULL);
    xsigaction(SIGCHLD, OnSigChld, 0, 0, NULL);
    xsigaction(SIGPIPE, OnSigPipe, 0, 0, NULL);
    if (ttyraw(kTtyLfToCrLf) != -1) ttymode_ = true;
    __cxa_atexit((void *)OnExit, NULL, NULL);
    __log_file = fopen(logpath_, "a");
    if (ischardev(infd_) && ischardev(outfd_)) {
      /* CHECK_NE(-1, fcntl(infd_, F_SETFL, O_NONBLOCK)); */
    } else if (infd_ != outfd_) {
      infd_ = -1;
    }
    /* CHECK_NE(-1, fcntl(outfd_, F_SETFL, O_NONBLOCK)); */
    if (CanPlayAudio()) MakeLatencyLittleLessBad();
    TryToOpenFrameBuffer();
    RenounceSpecialPrivileges();
    if (t2 > t1) longjmp(jb_, 1);
    OpenVideo();
    DimensionDisplay();
    starttime_ = timespec_real();
    PrintVideo();
  }
  INFOF("jb_ triggered");
  return 0;
}
