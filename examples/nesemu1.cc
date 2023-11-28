/* NESEMU1 :: EMULATOR FOR THE NINTENDO ENTERTAINMENT SYSTEM (R) ARCHITECTURE */
/* WRITTEN BY AND COPYRIGHT 2011 JOEL YLILUOMA ── SEE: http://iki.fi/bisqwit/ */
/* PORTED TO TELETYPEWRITERS IN YEAR 2020 BY JUSTINE ALEXANDRA ROBERTS TUNNEY */
/* TRADEMARKS ARE OWNED BY THEIR RESPECTIVE OWNERS LAWYERCATS LUV TAUTOLOGIES */
/* https://bisqwit.iki.fi/jutut/kuvat/programming_examples/nesemu1/nesemu1.cc */
#include "dsp/core/core.h"
#include "dsp/core/half.h"
#include "dsp/core/illumination.h"
#include "dsp/scale/scale.h"
#include "dsp/tty/itoa8.h"
#include "dsp/tty/quant.h"
#include "dsp/tty/tty.h"
#include "libc/assert.h"
#include "libc/calls/calls.h"
#include "libc/calls/struct/itimerval.h"
#include "libc/calls/struct/sigset.h"
#include "libc/calls/struct/winsize.h"
#include "libc/calls/termios.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/fmt/conv.h"
#include "libc/intrin/safemacros.internal.h"
#include "libc/inttypes.h"
#include "libc/log/check.h"
#include "libc/log/log.h"
#include "libc/macros.internal.h"
#include "libc/math.h"
#include "libc/mem/arraylist2.internal.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/zipos.internal.h"
#include "libc/sock/sock.h"
#include "libc/sock/struct/pollfd.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/ex.h"
#include "libc/sysv/consts/exit.h"
#include "libc/sysv/consts/f.h"
#include "libc/sysv/consts/fileno.h"
#include "libc/sysv/consts/itimer.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/poll.h"
#include "libc/sysv/consts/prio.h"
#include "libc/sysv/consts/sig.h"
#include "libc/sysv/consts/w.h"
#include "libc/thread/thread.h"
#include "libc/time/time.h"
#include "libc/x/xasprintf.h"
#include "libc/x/xsigaction.h"
#include "libc/zip.internal.h"
#include "third_party/getopt/getopt.internal.h"
#include "third_party/libcxx/vector"
#include "tool/viz/lib/knobs.h"

__static_yoink("zipos");

#define USAGE \
  " [ROM] [FMV]\n\
\n\
SYNOPSIS\n\
\n\
  Emulates NES Video Games in Terminal\n\
\n\
FLAGS\n\
\n\
  -A      ansi color mode\n\
  -t      normal color mode\n\
  -x      xterm256 color mode\n\
  -4      unicode character set\n\
  -3      ibm cp437 character set\n\
  -1      ntsc crt artifact emulation\n\
  -h\n\
  -?      shows this information\n\
\n\
KEYBOARD\n\
\n\
  We support Emacs / Mac OS X control key bindings. We also support\n\
  Vim. We support arrow keys. We also support WASD QWERTY & Dvorak.\n\
  The 'A' button is mapped to SPACE. The 'B' button is mapped to b.\n\
  Lastly TAB is SELECT and ENTER is START.\n\
\n\
  Teletypewriters are naturally limited in terms of keyboard input.\n\
  They don't exactly have n-key rollover. More like 1-key rollover.\n\
\n\
  Try tapping rather than holding keys. You can tune the activation\n\
  duration by pressing '8' and '9'. You can also adjust the keyboard\n\
  repeat delay in your operating system settings to make it shorter.\n\
\n\
  Ideally we should send patches to all the terms that introduces a\n\
  new ANSI escape sequences for key down / key up events. It'd also\n\
  be great to have inband audio with terminals too.\n\
\n\
GRAPHICS\n\
\n\
  The '1' key toggles CRT monitor artifact emulation, which can make\n\
  some games like Zelda II look better. The '3' and '4' keys toggle\n\
  the selection of UNICODE block characters.\n\
\n\
ZIP\n\
\n\
  This executable is also a ZIP archive. If you change the extension\n\
  then you can modify its inner structure, to place roms inside it.\n\
\n\
AUTHORS\n\
\n\
  Joel Yliluoma <http://iki.fi/bisqwit/>\n\
  Justine Tunney <jtunney@gmail.com/>\n\
\n\
\n"

#define DYN     240
#define DXN     256
#define FPS     60.0988
#define HZ      1789773
#define GAMMA   2.2
#define CTRL(C) ((C) ^ 0100)
#define ALT(C)  ((033 << 010) | (C))

typedef int8_t s8;
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;

static const struct itimerval kNesFps = {
    {0, 1. / FPS * 1e6},
    {0, 1. / FPS * 1e6},
};

struct Frame {
  char *p, *w, *mem;
};

struct Action {
  int code;
  int wait;
};

struct Audio {
  size_t i;
  int16_t p[FRAMESIZE];
};

struct Status {
  int wait;
  char text[80];
};

struct ZipGames {
  size_t i, n;
  char** p;
};

static int frame_;
static int playfd_;
static int playpid_;
static size_t vtsize_;
static bool artifacts_;
static long tyn_, txn_;
static struct Frame vf_[2];
static struct Audio audio_;
static const char* inputfn_;
static struct Status status_;
static volatile bool exited_;
static volatile bool timeout_;
static volatile bool resized_;
static struct TtyRgb* ttyrgb_;
static unsigned char *R, *G, *B;
static struct ZipGames zipgames_;
static struct Action arrow_, button_;
static struct SamplingSolution* ssy_;
static struct SamplingSolution* ssx_;
static unsigned char pixels_[3][DYN][DXN];
static unsigned char palette_[3][64][512][3];
static int joy_current_[2], joy_next_[2], joypos_[2];

static int keyframes_ = 10;
static enum TtyBlocksSelection blocks_ = kTtyBlocksUnicode;
static enum TtyQuantizationAlgorithm quant_ = kTtyQuantTrue;

static int Clamp(int v) {
  return MAX(0, MIN(255, v));
}

static double FixGamma(double x) {
  return tv2pcgamma(x, GAMMA);
}

void InitPalette(void) {
  // The input value is a NES color index (with de-emphasis bits).
  // See http://wiki.nesdev.com/w/index.php/NTSC_video for magic numbers
  // We need RGB values. To produce a RGB value, we emulate the NTSC circuitry.
  double A[3] = {-1.109, -.275, .947};
  double B[3] = {1.709, -.636, .624};
  double rgbc[3], lightbulb[3][3], rgbd65[3], sc[2];
  int o, u, r, c, b, p, y, i, l, q, e, p0, p1, pixel;
  signed char volt[] = "\372\273\32\305\35\311I\330D\357\175\13D!}N";
  GetChromaticAdaptationMatrix(lightbulb, kIlluminantC, kIlluminantD65);
  for (o = 0; o < 3; ++o) {
    for (p0 = 0; p0 < 512; ++p0) {
      for (p1 = 0; p1 < 64; ++p1) {
        for (u = 0; u < 3; ++u) {
          // Calculate the luma and chroma by emulating the relevant circuits:
          y = i = q = 0;
          // 12 samples of NTSC signal constitute a color.
          for (p = 0; p < 12; ++p) {
            // Sample either the previous or the current pixel.
            r = (p + o * 4) % 12;
            // Decode the color index.
            if (artifacts_) {
              pixel = r < 8 - u * 2 ? p0 : p1;
            } else {
              pixel = p0;
            }
            c = pixel % 16;
            l = c < 0xE ? pixel / 4 & 12 : 4;
            e = p0 / 64;
            // NES NTSC modulator
            // square wave between up to four voltage levels
            b = 40 + volt[(c > 12 * ((c + 8 + p) % 12 < 6)) +
                          2 * !(0451326 >> p / 2 * 3 & e) + l];
            // Ideal TV NTSC demodulator?
            sincos(M_PI * p / 6, &sc[0], &sc[1]);
            y += b;
            i += b * sc[1] * 5909;
            q += b * sc[0] * 5909;
          }
          // Converts YIQ to RGB
          // Store color at subpixel precision
          rgbc[u] = FixGamma(y / 1980. + i * A[u] / 9e6 + q * B[u] / 9e6);
        }
        matvmul3(rgbd65, lightbulb, rgbc);
        for (u = 0; u < 3; ++u) {
          palette_[o][p1][p0][u] = Clamp(rgbd65[u] * 255);
        }
      }
    }
  }
}

static ssize_t Write(int fd, const void* p, size_t n) {
  int rc;
  sigset_t ss, oldss;
  sigfillset(&ss);
  sigprocmask(SIG_SETMASK, &ss, &oldss);
  rc = write(fd, p, n);
  sigprocmask(SIG_SETMASK, &oldss, 0);
  return rc;
}

static void WriteString(const char* s) {
  Write(STDOUT_FILENO, s, strlen(s));
}

void Exit(int rc) {
  WriteString("\r\n\e[0m\e[J");
  if (rc && errno) {
    fprintf(stderr, "%s%s\r\n", "error: ", strerror(errno));
  }
  exit(rc);
}

void Cleanup(void) {
  ttyraw((enum TtyRawFlags)(-1u));
  ttyshowcursor(STDOUT_FILENO);
  if (playpid_) {
    kill(playpid_, SIGKILL);
    close(playfd_);
    playfd_ = -1;
  }
}

void OnCtrlC(void) {
  exited_ = true;
}

void OnTimer(void) {
  timeout_ = true;
}

void OnResize(void) {
  resized_ = true;
}

void OnPiped(void) {
  exited_ = true;
}

void OnSigChld(void) {
  waitpid(-1, 0, WNOHANG);
  close(playfd_);
  playpid_ = 0;
  playfd_ = -1;
}

void InitFrame(struct Frame* f) {
  f->p = f->w = f->mem = (char*)realloc(f->mem, vtsize_);
}

long ChopAxis(long dn, long sn) {
  while (HALF(sn) > dn) {
    sn = HALF(sn);
  }
  return sn;
}

void GetTermSize(void) {
  struct winsize wsize_;
  wsize_.ws_row = 25;
  wsize_.ws_col = 80;
  tcgetwinsize(0, &wsize_);
  FreeSamplingSolution(ssy_);
  FreeSamplingSolution(ssx_);
  tyn_ = wsize_.ws_row * 2;
  txn_ = wsize_.ws_col * 2;
  ssy_ = ComputeSamplingSolution(tyn_, ChopAxis(tyn_, DYN), 0, 0, 2);
  ssx_ = ComputeSamplingSolution(txn_, ChopAxis(txn_, DXN), 0, 0, 2);
  R = (unsigned char*)realloc(R, tyn_ * txn_);
  G = (unsigned char*)realloc(G, tyn_ * txn_);
  B = (unsigned char*)realloc(B, tyn_ * txn_);
  ttyrgb_ = (struct TtyRgb*)realloc(ttyrgb_, tyn_ * txn_ * 4);
  vtsize_ = ((tyn_ * txn_ * strlen("\e[48;2;255;48;2;255m▄")) +
             (tyn_ * strlen("\e[0m\r\n")) + 128);
  frame_ = 0;
  InitFrame(&vf_[0]);
  InitFrame(&vf_[1]);
  WriteString("\e[0m\e[H\e[J");
  resized_ = false;
}

void IoInit(void) {
  GetTermSize();
  xsigaction(SIGINT, (void*)OnCtrlC, 0, 0, NULL);
  xsigaction(SIGPIPE, (void*)OnPiped, 0, 0, NULL);
  xsigaction(SIGWINCH, (void*)OnResize, 0, 0, NULL);
  xsigaction(SIGALRM, (void*)OnTimer, 0, 0, NULL);
  xsigaction(SIGCHLD, (void*)OnSigChld, 0, 0, NULL);
  setitimer(ITIMER_REAL, &kNesFps, NULL);
  ttyhidecursor(STDOUT_FILENO);
  ttyraw(kTtySigs);
  ttyquantsetup(quant_, kTtyQuantRgb, blocks_);
  atexit(Cleanup);
}

void SetStatus(const char* fmt, ...) {
  va_list va;
  va_start(va, fmt);
  vsnprintf(status_.text, sizeof(status_.text), fmt, va);
  va_end(va);
  status_.wait = FPS / 2;
}

ssize_t ReadKeyboard(void) {
  int ch;
  ssize_t i, rc;
  char b[20] = {0};
  if ((rc = read(STDIN_FILENO, b, 16)) != -1) {
    if (!rc) {
      Exit(0);
    }
    for (i = 0; i < rc; ++i) {
      ch = b[i];
      if (b[i] == '\e') {
        ++i;
        if (b[i] == '[') {
          ++i;
          switch (b[i]) {
            case 'A':
              ch = CTRL('P');  // up arrow
              break;
            case 'B':
              ch = CTRL('N');  // down arrow
              break;
            case 'C':
              ch = CTRL('F');  // right arrow
              break;
            case 'D':
              ch = CTRL('B');  // left arrow
              break;
            default:
              break;
          }
        }
      }
      switch (ch) {
        case ' ':
          button_.code = 0b00100000;  // A
          button_.wait = keyframes_;
          break;
        case 'b':
          button_.code = 0b00010000;  // B
          button_.wait = keyframes_;
          break;
        case '\r':                    // enter
          button_.code = 0b10000000;  // START
          button_.wait = keyframes_;
          break;
        case '\t':                    // tab
          button_.code = 0b01000000;  // SELECT
          button_.wait = keyframes_;
          break;
        case 'k':                    // vim
        case 'w':                    // wasd qwerty
        case ',':                    // wasd dvorak
        case CTRL('P'):              // emacs
          arrow_.code = 0b00000100;  // UP
          arrow_.wait = keyframes_;
          break;
        case 'j':                    // vim
        case 's':                    // wasd qwerty
        case 'o':                    // wasd dvorak
        case CTRL('N'):              // emacs
          arrow_.code = 0b00001000;  // DOWN
          arrow_.wait = keyframes_;
          break;
        case 'h':                    // vim
        case 'a':                    // wasd qwerty & dvorak
        case CTRL('B'):              // emacs
          arrow_.code = 0b00000010;  // LEFT
          arrow_.wait = keyframes_;
          break;
        case 'l':                    // vim
        case 'd':                    // wasd qwerty
        case 'e':                    // wasd dvorak
        case CTRL('F'):              // emacs
          arrow_.code = 0b00000001;  // RIGHT
          arrow_.wait = keyframes_;
          break;
        case 'A':  // ansi 4-bit color mode
          quant_ = kTtyQuantAnsi;
          ttyquantsetup(quant_, kTtyQuantRgb, blocks_);
          SetStatus("ansi color");
          break;
        case 'x':  // xterm256 color mode
          quant_ = kTtyQuantXterm256;
          ttyquantsetup(quant_, kTtyQuantRgb, blocks_);
          SetStatus("xterm256 color");
          break;
        case 't':  // ansi 24bit color mode
          quant_ = kTtyQuantTrue;
          ttyquantsetup(quant_, kTtyQuantRgb, blocks_);
          SetStatus("24-bit color");
          break;
        case '1':
          artifacts_ = !artifacts_;
          InitPalette();
          SetStatus("artifacts %s", artifacts_ ? "on" : "off");
          break;
        case '3':  // oldskool ibm unicode rasterization
          blocks_ = kTtyBlocksCp437;
          ttyquantsetup(quant_, kTtyQuantRgb, blocks_);
          SetStatus("IBM CP437");
          break;
        case '4':  // newskool unicode rasterization
          blocks_ = kTtyBlocksUnicode;
          ttyquantsetup(quant_, kTtyQuantRgb, blocks_);
          SetStatus("UNICODE");
          break;
        case '8':
          keyframes_ = MAX(1, keyframes_ - 1);
          SetStatus("%d key frames", keyframes_);
          break;
        case '9':
          keyframes_ = keyframes_ + 1;
          SetStatus("%d key frames", keyframes_);
          break;
        default:
          break;
      }
    }
  }
  return rc;
}

bool HasVideo(struct Frame* f) {
  return f->w < f->p;
}

bool HasPendingVideo(void) {
  return HasVideo(&vf_[0]) || HasVideo(&vf_[1]);
}

bool HasPendingAudio(void) {
  return playpid_ && audio_.i;
}

struct Frame* FlipFrameBuffer(void) {
  frame_ = !frame_;
  return &vf_[frame_];
}

void TransmitVideo(void) {
  ssize_t rc;
  struct Frame* f;
  f = &vf_[frame_];
  if (!HasVideo(f)) f = FlipFrameBuffer();
  if ((rc = Write(STDOUT_FILENO, f->w, f->p - f->w)) != -1) {
    f->w += rc;
  } else if (errno == EAGAIN) {
    // slow teletypewriter
  } else if (errno == EPIPE) {
    Exit(0);
  }
}

void TransmitAudio(void) {
  ssize_t rc;
  if (!playpid_) return;
  if (!audio_.i) return;
  if (playfd_ == -1) return;
  if ((rc = Write(playfd_, audio_.p, audio_.i * sizeof(short))) != -1) {
    rc /= sizeof(short);
    memmove(audio_.p, audio_.p + rc, (audio_.i - rc) * sizeof(short));
    audio_.i -= rc;
  } else if (errno == EPIPE) {
    kill(playpid_, SIGKILL);
    close(playfd_);
    playfd_ = -1;
    Exit(0);
  }
}

void ScaleVideoFrameToTeletypewriter(void) {
  long y, x, yn, xn;
  yn = DYN, xn = DXN;
  while (HALF(yn) > tyn_ || HALF(xn) > txn_) {
    if (HALF(xn) > txn_) {
      Magikarp2xX(DYN, DXN, pixels_[0], yn, xn);
      Magikarp2xX(DYN, DXN, pixels_[1], yn, xn);
      Magikarp2xX(DYN, DXN, pixels_[2], yn, xn);
      xn = HALF(xn);
    }
    if (HALF(yn) > tyn_) {
      Magikarp2xY(DYN, DXN, pixels_[0], yn, xn);
      Magikarp2xY(DYN, DXN, pixels_[1], yn, xn);
      Magikarp2xY(DYN, DXN, pixels_[2], yn, xn);
      yn = HALF(yn);
    }
  }
  GyaradosUint8(tyn_, txn_, R, DYN, DXN, pixels_[0], tyn_, txn_, yn, xn, 0, 255,
                ssy_, ssx_, true);
  GyaradosUint8(tyn_, txn_, G, DYN, DXN, pixels_[1], tyn_, txn_, yn, xn, 0, 255,
                ssy_, ssx_, true);
  GyaradosUint8(tyn_, txn_, B, DYN, DXN, pixels_[2], tyn_, txn_, yn, xn, 0, 255,
                ssy_, ssx_, true);
  for (y = 0; y < tyn_; ++y) {
    for (x = 0; x < txn_; ++x) {
      ttyrgb_[y * txn_ + x] =
          rgb2tty(R[y * txn_ + x], G[y * txn_ + x], B[y * txn_ + x]);
    }
  }
}

void KeyCountdown(struct Action* a) {
  if (a->wait <= 1) {
    a->code = 0;
  } else {
    a->wait--;
  }
}

void PollAndSynchronize(void) {
  do {
    if (ReadKeyboard() == -1) {
      if (errno != EINTR) Exit(1);
      if (exited_) Exit(0);
      if (resized_) GetTermSize();
    }
  } while (!timeout_);
  TransmitVideo();
  TransmitAudio();
  timeout_ = false;
  KeyCountdown(&arrow_);
  KeyCountdown(&button_);
  joy_next_[0] = arrow_.code | button_.code;
  joy_next_[1] = arrow_.code | button_.code;
}

void Raster(void) {
  struct Frame* f;
  struct TtyRgb bg = {0x12, 0x34, 0x56, 0};
  struct TtyRgb fg = {0x12, 0x34, 0x56, 0};
  ScaleVideoFrameToTeletypewriter();
  f = &vf_[!frame_];
  f->p = f->w = f->mem;
  f->p = stpcpy(f->p, "\e[0m\e[H");
  f->p = ttyraster(f->p, ttyrgb_, tyn_, txn_, bg, fg);
  if (status_.wait) {
    status_.wait--;
    f->p = stpcpy(f->p, "\e[0m\e[H");
    f->p = stpcpy(f->p, status_.text);
  }
  PollAndSynchronize();
}

void FlushScanline(unsigned py) {
  if (py == DYN - 1) {
    if (!timeout_) {
      Raster();
    }
    timeout_ = false;
  }
}

static void PutPixel(unsigned px, unsigned py, unsigned pixel, int offset) {
  static unsigned prev;
  pixels_[0][py][px] = palette_[offset][prev % 64][pixel][2];
  pixels_[1][py][px] = palette_[offset][prev % 64][pixel][1];
  pixels_[2][py][px] = palette_[offset][prev % 64][pixel][0];
  prev = pixel;
}

static void JoyStrobe(unsigned v) {
  if (v) {
    joy_current_[0] = joy_next_[0];
    joypos_[0] = 0;
  }
  if (v) {
    joy_current_[1] = joy_next_[1];
    joypos_[1] = 0;
  }
}

static u8 JoyRead(unsigned idx) {
  // http://tasvideos.org/EmulatorResources/Famtasia/FMV.html
  static const u8 masks[8] = {
      0b00100000,  // A
      0b00010000,  // B
      0b01000000,  // SELECT
      0b10000000,  // START
      0b00000100,  // UP
      0b00001000,  // DOWN
      0b00000010,  // LEFT
      0b00000001,  // RIGHT
  };
  return (joy_current_[idx] & masks[joypos_[idx]++ & 7]) ? 1 : 0;
}

template <unsigned bitno, unsigned nbits = 1, typename T = u8>
struct RegBit {
  T data;
  enum { mask = (1u << nbits) - 1u };
  template <typename T2>
  RegBit& operator=(T2 v) {
    data = (data & ~(mask << bitno)) | ((nbits > 1 ? v & mask : !!v) << bitno);
    return *this;
  }
  operator unsigned() const {
    return (data >> bitno) & mask;
  }
  RegBit& operator++() {
    return *this = *this + 1;
  }
  unsigned operator++(int) {
    unsigned r = *this;
    ++*this;
    return r;
  }
};

namespace GamePak {

const unsigned VRomGranularity = 0x0400;
const unsigned VRomPages = 0x2000 / VRomGranularity;
const unsigned RomGranularity = 0x2000;
const unsigned RomPages = 0x10000 / RomGranularity;

std::vector<u8> ROM;
std::vector<u8> VRAM(0x2000);
unsigned mappernum;
unsigned char NRAM[0x1000];
unsigned char PRAM[0x2000];
unsigned char* banks[RomPages] = {};
unsigned char* Vbanks[VRomPages] = {};
unsigned char* Nta[4] = {NRAM + 0x0000, NRAM + 0x0400, NRAM + 0x0000,
                         NRAM + 0x0400};

template <unsigned npages, unsigned char* (&b)[npages], std::vector<u8>& r,
          unsigned granu>
static void SetPages(unsigned size, unsigned baseaddr, unsigned index) {
  for (unsigned v = r.size() + index * size, p = baseaddr / granu;
       p < (baseaddr + size) / granu && p < npages; ++p, v += granu) {
    b[p] = &r[v % r.size()];
  }
}

auto& SetROM = SetPages<RomPages, banks, ROM, RomGranularity>;
auto& SetVROM = SetPages<VRomPages, Vbanks, VRAM, VRomGranularity>;

u8 Access(unsigned addr, u8 value, bool write) {
  if (write && addr >= 0x8000 && mappernum == 7) {  // e.g. Rare games
    SetROM(0x8000, 0x8000, (value & 7));
    Nta[0] = Nta[1] = Nta[2] = Nta[3] = &NRAM[0x400 * ((value >> 4) & 1)];
  }
  if (write && addr >= 0x8000 && mappernum == 2) {  // e.g. Rockman, Castlevania
    SetROM(0x4000, 0x8000, value);
  }
  if (write && addr >= 0x8000 && mappernum == 3) {  // e.g. Kage, Solomon's Key
    value &= Access(addr, 0, false);                // Simulate bus conflict
    SetVROM(0x2000, 0x0000, (value & 3));
  }
  if (write && addr >= 0x8000 &&
      mappernum == 1) {  // e.g. Rockman 2, Simon's Quest
    static u8 regs[4] = {0x0C, 0, 0, 0}, counter = 0, cache = 0;
    if (value & 0x80) {
      regs[0] = 0x0C;
      goto configure;
    }
    cache |= (value & 1) << counter;
    if (++counter == 5) {
      regs[(addr >> 13) & 3] = value = cache;
    configure:
      cache = counter = 0;
      static const u8 sel[4][4] = {
          {0, 0, 0, 0}, {1, 1, 1, 1}, {0, 1, 0, 1}, {0, 0, 1, 1}};
      for (unsigned m = 0; m < 4; ++m)
        Nta[m] = &NRAM[0x400 * sel[regs[0] & 3][m]];
      SetVROM(0x1000, 0x0000,
              ((regs[0] & 16) ? regs[1] : ((regs[1] & ~1) + 0)));
      SetVROM(0x1000, 0x1000,
              ((regs[0] & 16) ? regs[2] : ((regs[1] & ~1) + 1)));
      switch ((regs[0] >> 2) & 3) {
        case 0:
        case 1:
          SetROM(0x8000, 0x8000, (regs[3] & 0xE) / 2);
          break;
        case 2:
          SetROM(0x4000, 0x8000, 0);
          SetROM(0x4000, 0xC000, (regs[3] & 0xF));
          break;
        case 3:
          SetROM(0x4000, 0x8000, (regs[3] & 0xF));
          SetROM(0x4000, 0xC000, ~0);
          break;
      }
    }
  }
  if ((addr >> 13) == 3) return PRAM[addr & 0x1FFF];
  return banks[(addr / RomGranularity) % RomPages][addr % RomGranularity];
}

void Init() {
  unsigned v;
  SetVROM(0x2000, 0x0000, 0);
  for (v = 0; v < 4; ++v) {
    SetROM(0x4000, v * 0x4000, v == 3 ? -1 : 0);
  }
}

}  // namespace GamePak

/* CPU: Ricoh RP2A03 (based on MOS6502, almost the same as in Commodore 64) */
namespace CPU {

u8 RAM[0x800];
bool reset = true;
bool nmi = false;
bool nmi_edge_detected = false;
bool intr = false;

template <bool write>
u8 MemAccess(u16 addr, u8 v = 0);
u8 RB(u16 addr) {
  return MemAccess<0>(addr);
}
u8 WB(u16 addr, u8 v) {
  return MemAccess<1>(addr, v);
}
void Tick();

}  // namespace CPU

namespace PPU { /* Picture Processing Unit */

union regtype {  // PPU register file
  u32 value;
  /* clang-format off */
  // Reg0 (write)             // Reg1 (write)             // Reg2 (read)
  RegBit<0,8,u32> sysctrl;    RegBit< 8,8,u32> dispctrl;  RegBit<16,8,u32> status;
  RegBit<0,2,u32> BaseNTA;    RegBit< 8,1,u32> Grayscale; RegBit<21,1,u32> SPoverflow;
  RegBit<2,1,u32> Inc;        RegBit< 9,1,u32> ShowBG8;   RegBit<22,1,u32> SP0hit;
  RegBit<3,1,u32> SPaddr;     RegBit<10,1,u32> ShowSP8;   RegBit<23,1,u32> InVBlank;
  RegBit<4,1,u32> BGaddr;     RegBit<11,1,u32> ShowBG;    // Reg3 (write)
  RegBit<5,1,u32> SPsize;     RegBit<12,1,u32> ShowSP;    RegBit<24,8,u32> OAMaddr;
  RegBit<6,1,u32> SlaveFlag;  RegBit<11,2,u32> ShowBGSP;  RegBit<24,2,u32> OAMdata;
  RegBit<7,1,u32> NMIenabled; RegBit<13,3,u32> EmpRGB;    RegBit<26,6,u32> OAMindex;
  /* clang-format on */
} reg;

// Raw memory data as read&written by the game
u8 palette[32];
u8 OAM[256];

// Decoded sprite information, used & changed during each scanline
struct {
  u8 sprindex, y, index, attr, x_;
  u16 pattern;
} OAM2[8], OAM3[8];

union scrolltype {
  RegBit<3, 16, u32> raw;        // raw VRAM address (16-bit)
  RegBit<0, 8, u32> xscroll;     // low 8 bits of first write to 2005
  RegBit<0, 3, u32> xfine;       // low 3 bits of first write to 2005
  RegBit<3, 5, u32> xcoarse;     // high 5 bits of first write to 2005
  RegBit<8, 5, u32> ycoarse;     // high 5 bits of second write to 2005
  RegBit<13, 2, u32> basenta;    // nametable index (copied from 2000)
  RegBit<13, 1, u32> basenta_h;  // horizontal nametable index
  RegBit<14, 1, u32> basenta_v;  // vertical   nametable index
  RegBit<15, 3, u32> yfine;      // low 3 bits of second write to 2005
  RegBit<11, 8, u32> vaddrhi;    // first write to 2006 w/ high 2 bits set to 0
  RegBit<3, 8, u32> vaddrlo;     // second write to 2006
} scroll, vaddr;

unsigned pat_addr, sprinpos, sproutpos, sprrenpos, sprtmp;
u16 tileattr, tilepat, ioaddr;
u32 bg_shift_pat, bg_shift_attr;
int x_ = 0;
int scanline = 241;
int scanline_end = 341;
int VBlankState = 0;
int cycle_counter = 0;
int read_buffer = 0;
int open_bus = 0;
int open_bus_decay_timer = 0;
bool even_odd_toggle = false;
bool offset_toggle = false;

/* Memory mapping: Convert PPU memory address into reference to relevant data */
u8& NesMmap(int i) {
  i &= 0x3FFF;
  if (i >= 0x3F00) {
    if (i % 4 == 0) i &= 0x0F;
    return palette[i & 0x1F];
  }
  if (i < 0x2000) {
    return GamePak::Vbanks[(i / GamePak::VRomGranularity) % GamePak::VRomPages]
                          [i % GamePak::VRomGranularity];
  }
  return GamePak::Nta[(i >> 10) & 3][i & 0x3FF];
}

// External I/O: read or write
u8 PpuAccess(u16 index, u8 v, bool write) {
  auto RefreshOpenBus = [&](u8 v) {
    return open_bus_decay_timer = 77777, open_bus = v;
  };
  u8 res = open_bus;
  if (write) RefreshOpenBus(v);
  switch (index) {  // Which port from $200x?
    case 0:
      if (write) {
        reg.sysctrl = v;
        scroll.basenta = reg.BaseNTA;
      }
      break;
    case 1:
      if (write) {
        reg.dispctrl = v;
      }
      break;
    case 2:
      if (write) break;
      res = reg.status | (open_bus & 0x1F);
      reg.InVBlank = false;   // Reading $2002 clears the vblank flag.
      offset_toggle = false;  // Also resets the toggle for address updates.
      if (VBlankState != -5) {
        VBlankState = 0;  // This also may cancel the setting of InVBlank.
      }
      break;
    case 3:
      if (write) reg.OAMaddr = v;
      break;  // Index into Object Attribute Memory
    case 4:
      if (write) {
        OAM[reg.OAMaddr++] = v;  // Write or read the OAM (sprites).
      } else {
        res =
            RefreshOpenBus(OAM[reg.OAMaddr] & (reg.OAMdata == 2 ? 0xE3 : 0xFF));
      }
      break;
    case 5:
      if (!write) break;  // Set background scrolling offset
      if (offset_toggle) {
        scroll.yfine = v & 7;
        scroll.ycoarse = v >> 3;
      } else {
        scroll.xscroll = v;
      }
      offset_toggle = !offset_toggle;
      break;
    case 6:
      if (!write) break;  // Set video memory position for reads/writes
      if (offset_toggle) {
        scroll.vaddrlo = v;
        vaddr.raw = (unsigned)scroll.raw;
      } else {
        scroll.vaddrhi = v & 0x3F;
      }
      offset_toggle = !offset_toggle;
      break;
    case 7:
      res = read_buffer;
      u8& t = NesMmap(vaddr.raw);  // Access the video memory.
      if (write) {
        res = t = v;
      } else {
        if ((vaddr.raw & 0x3F00) == 0x3F00) {  // palette?
          res = read_buffer = (open_bus & 0xC0) | (t & 0x3F);
        }
        read_buffer = t;
      }
      RefreshOpenBus(res);
      vaddr.raw = vaddr.raw +
                  (reg.Inc ? 32 : 1);  // The address is automatically updated.
      break;
  }
  return res;
}

void RenderingTick() {
  int y1, y2;
  bool tile_decode_mode =
      0x10FFFF & (1u << (x_ / 16));  // When x_ is 0..255, 320..335
  // Each action happens in two steps: 1) select memory address; 2) receive data
  // and react on it.
  switch (x_ % 8) {
    case 2:  // Point to attribute table
      ioaddr = 0x23C0 + 0x400 * vaddr.basenta + 8 * (vaddr.ycoarse / 4) +
               (vaddr.xcoarse / 4);
      if (tile_decode_mode) break;  // Or nametable, with sprites.
    case 0:                         // Point to nametable
      ioaddr = 0x2000 + (vaddr.raw & 0xFFF);
      // Reset sprite data
      if (x_ == 0) {
        sprinpos = sproutpos = 0;
        if (reg.ShowSP) reg.OAMaddr = 0;
      }
      if (!reg.ShowBG) break;
      // Reset scrolling (vertical once, horizontal each scanline)
      if (x_ == 304 && scanline == -1) vaddr.raw = (unsigned)scroll.raw;
      if (x_ == 256) {
        vaddr.xcoarse = (unsigned)scroll.xcoarse;
        vaddr.basenta_h = (unsigned)scroll.basenta_h;
        sprrenpos = 0;
      }
      break;
    case 1:
      if (x_ == 337 && scanline == -1 && even_odd_toggle && reg.ShowBG) {
        scanline_end = 340;
      }
      // Name table access
      pat_addr = 0x1000 * reg.BGaddr + 16 * NesMmap(ioaddr) + vaddr.yfine;
      if (!tile_decode_mode) break;
      // Push the current tile into shift registers.
      // The bitmap pattern is 16 bits, while the attribute is 2 bits, repeated
      // 8 times.
      bg_shift_pat = (bg_shift_pat >> 16) + 0x00010000 * tilepat;
      bg_shift_attr = (bg_shift_attr >> 16) + 0x55550000 * tileattr;
      break;
    case 3:
      // Attribute table access
      if (tile_decode_mode) {
        tileattr = (NesMmap(ioaddr) >>
                    ((vaddr.xcoarse & 2) + 2 * (vaddr.ycoarse & 2))) &
                   3;
        // Go to the next tile horizontally (and switch nametable if it wraps)
        if (!++vaddr.xcoarse) {
          vaddr.basenta_h = 1 - vaddr.basenta_h;
        }
        // At the edge of the screen, do the same but vertically
        if (x_ == 251 && !++vaddr.yfine && ++vaddr.ycoarse == 30) {
          vaddr.ycoarse = 0;
          vaddr.basenta_v = 1 - vaddr.basenta_v;
        }
      } else if (sprrenpos < sproutpos) {
        // Select sprite pattern instead of background pattern
        auto& o = OAM3[sprrenpos];  // Sprite to render on next scanline
        memcpy(&o, &OAM2[sprrenpos], sizeof(o));
        unsigned y = (scanline)-o.y;
        if (o.attr & 0x80) y ^= (reg.SPsize ? 15 : 7);
        pat_addr = 0x1000 * (reg.SPsize ? (o.index & 0x01) : reg.SPaddr);
        pat_addr += 0x10 * (reg.SPsize ? (o.index & 0xFE) : (o.index & 0xFF));
        pat_addr += (y & 7) + (y & 8) * 2;
      }
      break;
    // Pattern table bytes
    case 5:
      tilepat = NesMmap(pat_addr | 0);
      break;
    case 7:  // Interleave the bits of the two pattern bytes
      unsigned p = tilepat | (NesMmap(pat_addr | 8) << 8);
      p = (p & 0xF00F) | ((p & 0x0F00) >> 4) | ((p & 0x00F0) << 4);
      p = (p & 0xC3C3) | ((p & 0x3030) >> 2) | ((p & 0x0C0C) << 2);
      p = (p & 0x9999) | ((p & 0x4444) >> 1) | ((p & 0x2222) << 1);
      tilepat = p;
      // When decoding sprites, save the sprite graphics and move to next sprite
      if (!tile_decode_mode && sprrenpos < sproutpos) {
        OAM3[sprrenpos++].pattern = tilepat;
      }
      break;
  }
  // Find which sprites are visible on next scanline (TODO: implement crazy
  // 9-sprite malfunction)
  switch (x_ >= 64 && x_ < 256 && x_ % 2 ? (reg.OAMaddr++ & 3) : 4) {
    default:
      // Access OAM (object attribute memory)
      sprtmp = OAM[reg.OAMaddr];
      break;
    case 0:
      if (sprinpos >= 64) {
        reg.OAMaddr = 0;
        break;
      }
      ++sprinpos;  // next sprite
      if (sproutpos < 8) OAM2[sproutpos].y = sprtmp;
      if (sproutpos < 8) OAM2[sproutpos].sprindex = reg.OAMindex;
      y1 = sprtmp;
      y2 = sprtmp + (reg.SPsize ? 16 : 8);
      if (!(scanline >= y1 && scanline < y2)) {
        reg.OAMaddr = sprinpos != 2 ? reg.OAMaddr + 3 : 8;
      }
      break;
    case 1:
      if (sproutpos < 8) OAM2[sproutpos].index = sprtmp;
      break;
    case 2:
      if (sproutpos < 8) OAM2[sproutpos].attr = sprtmp;
      break;
    case 3:
      if (sproutpos < 8) OAM2[sproutpos].x_ = sprtmp;
      if (sproutpos < 8) {
        ++sproutpos;
      } else {
        reg.SPoverflow = true;
      }
      if (sprinpos == 2) reg.OAMaddr = 8;
      break;
  }
}

void RenderPixel() {
  bool edge = u8(x_ + 8) < 16;  // 0..7, 248..255
  bool showbg = reg.ShowBG && (!edge || reg.ShowBG8);
  bool showsp = reg.ShowSP && (!edge || reg.ShowSP8);

  // Render the background
  unsigned fx = scroll.xfine,
           xpos = 15 - (((x_ & 7) + fx + 8 * !!(x_ & 7)) & 15);

  unsigned pixel = 0, attr = 0;
  if (showbg) {  // Pick a pixel from the shift registers
    pixel = (bg_shift_pat >> (xpos * 2)) & 3;
    attr = (bg_shift_attr >> (xpos * 2)) & (pixel ? 3 : 0);
  } else if ((vaddr.raw & 0x3F00) == 0x3F00 && !reg.ShowBGSP) {
    pixel = vaddr.raw;
  }

  // Overlay the sprites
  if (showsp) {
    for (unsigned sno = 0; sno < sprrenpos; ++sno) {
      auto& s = OAM3[sno];
      // Check if this sprite is horizontally in range
      unsigned xdiff = x_ - s.x_;
      if (xdiff >= 8) continue;  // Also matches negative values
      // Determine which pixel to display; skip transparent pixels
      if (!(s.attr & 0x40)) xdiff = 7 - xdiff;
      u8 spritepixel = (s.pattern >> (xdiff * 2)) & 3;
      if (!spritepixel) continue;
      // Register sprite-0 hit if applicable
      if (x_ < 255 && pixel && s.sprindex == 0) reg.SP0hit = true;
      // Render the pixel unless behind-background placement wanted
      if (!(s.attr & 0x20) || !pixel) {
        attr = (s.attr & 3) + 4;
        pixel = spritepixel;
      }
      // Only process the first non-transparent sprite pixel.
      break;
    }
  }

  pixel = palette[(attr * 4 + pixel) & 0x1F] & (reg.Grayscale ? 0x30 : 0x3F);
  PutPixel(x_, scanline, pixel | (reg.EmpRGB << 6), cycle_counter);
}

void ReadToolAssistedSpeedrunRobotKeys() {
  static FILE* fp;
  if (!fp && !isempty(inputfn_)) {
    fp = fopen(inputfn_, "rb");
  }
  if (fp) {
    static unsigned ctrlmask = 0;
    if (!ftell(fp)) {
      fseek(fp, 0x05, SEEK_SET);
      ctrlmask = fgetc(fp);
      fseek(fp, 0x90, SEEK_SET);  // Famtasia Movie format.
    }
    if (ctrlmask & 0x80) {
      joy_next_[0] = fgetc(fp);
      if (feof(fp)) joy_next_[0] = 0;
    }
    if (ctrlmask & 0x40) {
      joy_next_[1] = fgetc(fp);
      if (feof(fp)) joy_next_[1] = 0;
    }
  }
}

// PPU::Tick() -- This function is called 3 times per each CPU cycle.
// Each call iterates through one pixel of the screen.
// The screen is divided into 262 scanlines, each having 341 columns, as such:
//
//           x_=0                 x_=256      x_=340
//       ___|____________________|__________|
//  y=-1    | pre-render scanline| prepare  | >
//       ___|____________________| sprites _| > Graphics
//  y=0     | visible area       | for the  | > processing
//          | - this is rendered | next     | > scanlines
//  y=239   |   on the screen.   | scanline | >
//       ___|____________________|______
//  y=240   | idle
//       ___|_______________________________
//  y=241   | vertical blanking (idle)
//          | 20 scanlines long
//  y=260___|____________________|__________|
//
// On actual PPU, the scanline begins actually before x_=0, with
// sync/colorburst/black/background color being rendered, and
// ends after x_=256 with background/black being rendered first,
// but in this emulator we only care about the visible area.
//
// When background rendering is enabled, scanline -1 is
// 340 or 341 pixels long, alternating each frame.
// In all other situations the scanline is 341 pixels long.
// Thus, it takes 89341 or 89342 PPU::Tick() calls to render 1 frame.
void Tick() {
  // Set/clear vblank where needed
  switch (VBlankState) {
    case -5:
      reg.status = 0;
      break;
    case 2:
      reg.InVBlank = true;
      break;
    case 0:
      CPU::nmi = reg.InVBlank && reg.NMIenabled;
      break;
  }
  if (VBlankState != 0) VBlankState += (VBlankState < 0 ? 1 : -1);
  if (open_bus_decay_timer && !--open_bus_decay_timer) open_bus = 0;

  // Graphics processing scanline?
  if (scanline < DYN) {
    /* Process graphics for this cycle */
    if (reg.ShowBGSP) RenderingTick();
    if (scanline >= 0 && x_ < 256) RenderPixel();
  }

  // Done with the cycle. Check for end of scanline.
  if (++cycle_counter == 3) cycle_counter = 0;  // For NTSC pixel shifting
  if (++x_ >= scanline_end) {
    // Begin new scanline
    FlushScanline(scanline);
    scanline_end = 341;
    x_ = 0;
    // Does something special happen on the new scanline?
    switch (scanline += 1) {
      case 261:         // Begin of rendering
        scanline = -1;  // pre-render line
        even_odd_toggle = !even_odd_toggle;
        // Clear vblank flag
        VBlankState = -5;
        break;
      case 241:  // Begin of vertical blanking
        ReadToolAssistedSpeedrunRobotKeys();
        // Set vblank flag
        VBlankState = 2;
    }
  }
}

}  // namespace PPU

namespace APU { /* Audio Processing Unit */

static const u8 LengthCounters[32] = {
    10, 254, 20, 2,  40, 4,  80, 6,  160, 8,  60, 10, 14, 12, 26, 14,
    12, 16,  24, 18, 48, 20, 96, 22, 192, 24, 72, 26, 16, 28, 32, 30,
};

static const u16 NoisePeriods[16] = {
    2, 4, 8, 16, 32, 48, 64, 80, 101, 127, 190, 254, 381, 508, 1017, 2034,
};

static const u16 DMCperiods[16] = {
    428, 380, 340, 320, 286, 254, 226, 214, 190, 160, 142, 128, 106, 84, 72, 54,
};

bool IRQdisable = true;
bool FiveCycleDivider;
bool ChannelsEnabled[5];
bool PeriodicIRQ;
bool DMC_IRQ;

bool count(int& v, int reset) {
  return --v < 0 ? (v = reset), true : false;
}

struct channel {
  int length_counter, linear_counter, address, envelope;
  int sweep_delay, env_delay, wave_counter, hold, phase, level;
  union {  // Per-channel register file
    // 4000, 4004, 400C, 4012:
    RegBit<0, 8, u32> reg0;
    RegBit<6, 2, u32> DutyCycle;
    RegBit<4, 1, u32> EnvDecayDisable;
    RegBit<0, 4, u32> EnvDecayRate;
    RegBit<5, 1, u32> EnvDecayLoopEnable;
    RegBit<0, 4, u32> FixedVolume;
    RegBit<5, 1, u32> LengthCounterDisable;
    RegBit<0, 7, u32> LinearCounterInit;
    RegBit<7, 1, u32> LinearCounterDisable;
    // 4001, 4005, 4013:
    RegBit<8, 8, u32> reg1;
    RegBit<8, 3, u32> SweepShift;
    RegBit<11, 1, u32> SweepDecrease;
    RegBit<12, 3, u32> SweepRate;
    RegBit<15, 1, u32> SweepEnable;
    RegBit<8, 8, u32> PCMlength;
    // 4002, 4006, 400A, 400E:
    RegBit<16, 8, u32> reg2;
    RegBit<16, 4, u32> NoiseFreq;
    RegBit<23, 1, u32> NoiseType;
    RegBit<16, 11, u32> WaveLength;
    // 4003, 4007, 400B, 400F, 4010:
    RegBit<24, 8, u32> reg3;
    RegBit<27, 5, u32> LengthCounterInit;
    RegBit<30, 1, u32> LoopEnabled;
    RegBit<31, 1, u32> IRQenable;
  } reg;

  // Function for updating the wave generators and taking the sample for each
  // channel.
  template <unsigned c>
  int Tick() {
    channel& ch = *this;
    if (!ChannelsEnabled[c]) return c == 4 ? 64 : 8;
    int wl = (ch.reg.WaveLength + 1) * (c >= 2 ? 1 : 2);
    if (c == 3) wl = NoisePeriods[ch.reg.NoiseFreq];
    int volume = ch.length_counter
                     ? ch.reg.EnvDecayDisable ? ch.reg.FixedVolume : ch.envelope
                     : 0;
    // Sample may change at wavelen intervals.
    auto& S = ch.level;
    if (!count(ch.wave_counter, wl)) return S;
    switch (c) {
      default:  // Square wave. With four different 8-step binary waveforms (32
                // bits of data total).
        if (wl < 8) return S = 8;
        return S = (0xF33C0C04u &
                    (1u << (++ch.phase % 8 + ch.reg.DutyCycle * 8)))
                       ? volume
                       : 0;

      case 2:  // Triangle wave
        if (ch.length_counter && ch.linear_counter && wl >= 3) ++ch.phase;
        return S = (ch.phase & 15) ^ ((ch.phase & 16) ? 15 : 0);

      case 3:  // Noise: Linear feedback shift register
        if (!ch.hold) ch.hold = 1;
        ch.hold =
            (ch.hold >> 1) |
            (((ch.hold ^ (ch.hold >> (ch.reg.NoiseType ? 6 : 1))) & 1) << 14);
        return S = (ch.hold & 1) ? 0 : volume;

      case 4:  // Delta modulation channel (DMC)
        // hold = 8 bit value, phase = number of bits buffered
        if (ch.phase == 0) {  // Nothing in sample buffer?
          if (!ch.length_counter && ch.reg.LoopEnabled) {  // Loop?
            ch.length_counter = ch.reg.PCMlength * 16 + 1;
            ch.address = (ch.reg.reg0 | 0x300) << 6;
          }
          if (ch.length_counter > 0) {  // Load next 8 bits if available
            // Note: Re-entrant! But not recursive, because even
            // the shortest wave length is greater than the read time.
            // TODO: proper clock
            if (ch.reg.WaveLength > 20) {
              for (unsigned t = 0; t < 3; ++t) {
                CPU::RB(u16(ch.address) | 0x8000);  // timing
              }
            }
            ch.hold = CPU::RB(u16(ch.address++) | 0x8000);  // Fetch byte
            ch.phase = 8;
            --ch.length_counter;
          } else {  // Otherwise, disable channel or issue IRQ
            ChannelsEnabled[4] =
                ch.reg.IRQenable && (CPU::intr = DMC_IRQ = true);
          }
        }
        if (ch.phase != 0) {  // Update the signal if sample buffer nonempty
          int v = ch.linear_counter;
          if (ch.hold & (0x80 >> --ch.phase)) {
            v += 2;
          } else {
            v -= 2;
          }
          if (v >= 0 && v <= 0x7F) ch.linear_counter = v;
        }
        return S = ch.linear_counter;
    }
  }
} channels[5] = {};

struct {
  short lo, hi;
} hz240counter = {0, 0};

void Write(u8 index, u8 value) {
  unsigned c;
  channel& ch = channels[(index / 4) % 5];
  switch (index < 0x10 ? index % 4 : index) {
    case 0:
      if (ch.reg.LinearCounterDisable) {
        ch.linear_counter = value & 0x7F;
      }
      ch.reg.reg0 = value;
      break;
    case 1:
      ch.reg.reg1 = value;
      ch.sweep_delay = ch.reg.SweepRate;
      break;
    case 2:
      ch.reg.reg2 = value;
      break;
    case 3:
      ch.reg.reg3 = value;
      if (ChannelsEnabled[index / 4]) {
        ch.length_counter = LengthCounters[ch.reg.LengthCounterInit];
      }
      ch.linear_counter = ch.reg.LinearCounterInit;
      ch.env_delay = ch.reg.EnvDecayRate;
      ch.envelope = 15;
      if (index < 8) ch.phase = 0;
      break;
    case 0x10:
      ch.reg.reg3 = value;
      ch.reg.WaveLength = DMCperiods[value & 0x0F];
      break;
    case 0x12:
      ch.reg.reg0 = value;
      ch.address = (ch.reg.reg0 | 0x300) << 6;
      break;
    case 0x13:
      ch.reg.reg1 = value;
      ch.length_counter = ch.reg.PCMlength * 16 + 1;
      break;  // sample length
    case 0x11:
      ch.linear_counter = value & 0x7F;
      break;  // dac value
    case 0x15:
      for (c = 0; c < 5; ++c) {
        ChannelsEnabled[c] = value & (1 << c);
      }
      for (c = 0; c < 5; ++c) {
        if (!ChannelsEnabled[c]) {
          channels[c].length_counter = 0;
        } else if (c == 4 && channels[c].length_counter == 0) {
          channels[c].length_counter = ch.reg.PCMlength * 16 + 1;
        }
      }
      break;
    case 0x17:
      IRQdisable = value & 0x40;
      FiveCycleDivider = value & 0x80;
      hz240counter = {0, 0};
      if (IRQdisable) {
        PeriodicIRQ = DMC_IRQ = false;
      }
      break;
  }
}

u8 Read() {
  unsigned c;
  u8 res = 0;
  for (c = 0; c < 5; ++c) {
    res |= channels[c].length_counter ? 1 << c : 0;
  }
  if (PeriodicIRQ) res |= 0x40;
  PeriodicIRQ = false;
  if (DMC_IRQ) res |= 0x80;
  DMC_IRQ = false;
  CPU::intr = false;
  return res;
}

void Tick() {  // Invoked at CPU's rate.
  // Divide CPU clock by 7457.5 to get a 240 Hz, which controls certain events.
  if ((hz240counter.lo += 2) >= 14915) {
    hz240counter.lo -= 14915;
    if (++hz240counter.hi >= 4 + FiveCycleDivider) hz240counter.hi = 0;

    // 60 Hz interval: IRQ. IRQ is not invoked in five-cycle mode (48 Hz).
    if (!IRQdisable && !FiveCycleDivider && hz240counter.hi == 0) {
      CPU::intr = PeriodicIRQ = true;
    }

    // Some events are invoked at 96 Hz or 120 Hz rate. Others, 192 Hz or 240
    // Hz.
    bool HalfTick = (hz240counter.hi & 5) == 1;
    bool FullTick = hz240counter.hi < 4;
    for (unsigned c = 0; c < 4; ++c) {
      channel& ch = channels[c];
      int wl = ch.reg.WaveLength;

      // Length tick (all channels except DMC, but different disable bit for
      // triangle wave)
      if (HalfTick && ch.length_counter &&
          !(c == 2 ? ch.reg.LinearCounterDisable : ch.reg.LengthCounterDisable))
        ch.length_counter -= 1;

      // Sweep tick (square waves only)
      if (HalfTick && c < 2 && count(ch.sweep_delay, ch.reg.SweepRate))
        if (wl >= 8 && ch.reg.SweepEnable && ch.reg.SweepShift) {
          int s = wl >> ch.reg.SweepShift, d[4] = {s, s, ~s, -s};
          wl += d[ch.reg.SweepDecrease * 2 + c];
          if (wl < 0x800) ch.reg.WaveLength = wl;
        }

      // Linear tick (triangle wave only)
      if (FullTick && c == 2) {
        ch.linear_counter =
            ch.reg.LinearCounterDisable
                ? ch.reg.LinearCounterInit
                : (ch.linear_counter > 0 ? ch.linear_counter - 1 : 0);
      }

      // Envelope tick (square and noise channels)
      if (FullTick && c != 2 && count(ch.env_delay, ch.reg.EnvDecayRate)) {
        if (ch.envelope > 0 || ch.reg.EnvDecayLoopEnable) {
          ch.envelope = (ch.envelope - 1) & 15;
        }
      }
    }
  }

// Mix the audio: Get the momentary sample from each channel and mix them.
#define s(c) channels[c].Tick<c == 1 ? 0 : c>()
  auto v = [](float m, float n, float d) { return n != 0.f ? m / n : d; };
  short sample =
      30000 *
      (v(95.88f, (100.f + v(8128.f, s(0) + s(1), -100.f)), 0.f) +
       v(159.79f,
         (100.f +
          v(1.0, s(2) / 8227.f + s(3) / 12241.f + s(4) / 22638.f, -100.f)),
         0.f) -
       0.5f);
#undef s

  audio_.p[audio_.i = (audio_.i + 1) & (ARRAYLEN(audio_.p) - 1)] = sample;
}

}  // namespace APU

namespace CPU {

void Tick() {
  // PPU clock: 3 times the CPU rate
  for (unsigned n = 0; n < 3; ++n) PPU::Tick();
  // APU clock: 1 times the CPU rate
  for (unsigned n = 0; n < 1; ++n) APU::Tick();
}

template <bool write>
u8 MemAccess(u16 addr, u8 v) {
  // Memory writes are turned into reads while reset is being signalled
  if (reset && write) return MemAccess<0>(addr);
  Tick();
  // Map the memory from CPU's viewpoint.
  /**/ if (addr < 0x2000) {
    u8& r = RAM[addr & 0x7FF];
    if (!write) return r;
    r = v;
  } else if (addr < 0x4000) {
    return PPU::PpuAccess(addr & 7, v, write);
  } else if (addr < 0x4018) {
    switch (addr & 0x1F) {
      case 0x14:  // OAM DMA: Copy 256 bytes from RAM into PPU's sprite memory
        if (write)
          for (unsigned b = 0; b < 256; ++b)
            WB(0x2004, RB((v & 7) * 0x0100 + b));
        return 0;
      case 0x15:
        if (!write) return APU::Read();
        APU::Write(0x15, v);
        break;
      case 0x16:
        if (!write) return JoyRead(0);
        JoyStrobe(v);
        break;
      case 0x17:
        if (!write) return JoyRead(1);  // write:passthru
      default:
        if (!write) break;
        APU::Write(addr & 0x1F, v);
    }
  } else {
    return GamePak::Access(addr, v, write);
  }
  return 0;
}

// CPU registers:
u16 PC = 0xC000;
u8 A = 0, X = 0, Y = 0, S = 0;
union { /* Status flags: */
  u8 raw;
  RegBit<0> C;  // carry
  RegBit<1> Z;  // zero
  RegBit<2> I;  // interrupt enable/disable
  RegBit<3> D;  // decimal mode (unsupported on NES, but flag exists)
  // 4,5 (0x10,0x20) don't exist
  RegBit<6> V;  // overflow
  RegBit<7> N;  // negative
} P;

u16 wrap(u16 oldaddr, u16 newaddr) {
  return (oldaddr & 0xFF00) + u8(newaddr);
}
void Misfire(u16 old, u16 addr) {
  u16 q = wrap(old, addr);
  if (q != addr) RB(q);
}
u8 Pop() {
  return RB(0x100 | u8(++S));
}
void Push(u8 v) {
  WB(0x100 | u8(S--), v);
}

template <u16 op>  // Execute a single CPU instruction, defined by opcode "op".
void Ins() {  // With template magic, the compiler will literally synthesize
              // >256 different functions.
  // Note: op 0x100 means "NMI", 0x101 means "Reset", 0x102 means "IRQ". They
  // are implemented in terms of "BRK". User is responsible for ensuring that
  // WB() will not store into memory while Reset is being processed.
  unsigned addr = 0, d = 0, t = 0xFF, c = 0, sb = 0,
           pbits = op < 0x100 ? 0x30 : 0x20;

  // Define the opcode decoding matrix, which decides which micro-operations
  // constitute any particular opcode. (Note: The PLA of 6502 works on a
  // slightly different principle.)
  enum { o8 = op / 8, o8m = 1 << (op % 8) };

// Fetch op'th item from a bitstring encoded in a data-specific variant of
// base64, where each character transmits 8 bits of information rather than 6.
// This peculiar encoding was chosen to reduce the source code size.
// Enum temporaries are used in order to ensure compile-time evaluation.
#define t(s, code)                                                             \
  {                                                                            \
    enum {                                                                     \
      i = o8m &                                                                \
          (s[o8] > 90 ? (130 + " (),-089<>?BCFGHJLSVWZ[^hlmnxy|}"[s[o8] - 94]) \
                      : (s[o8] - " (("[s[o8] / 39]))                           \
    };                                                                         \
    if (i) {                                                                   \
      code;                                                                    \
    }                                                                          \
  }

  /* wow */
  /* clang-format off */
  /* Decode address operand */
  t("                                !", addr = 0xFFFA) // NMI vector location
  t("                                *", addr = 0xFFFC) // Reset vector location
  t("!                               ,", addr = 0xFFFE) // Interrupt vector location
  t("zy}z{y}zzy}zzy}zzy}zzy}zzy}zzy}z ", addr = RB(PC++))
  t("2 yy2 yy2 yy2 yy2 XX2 XX2 yy2 yy ", d = X) // register index
  t("  62  62  62  62  om  om  62  62 ", d = Y)
  t("2 y 2 y 2 y 2 y 2 y 2 y 2 y 2 y  ", addr=u8(addr+d); d=0; Tick())              // add zeropage-index
  t(" y z!y z y z y z y z y z y z y z ", addr=u8(addr);   addr+=256*RB(PC++))       // absolute address
  t("3 6 2 6 2 6 286 2 6 2 6 2 6 2 6 /", addr=RB(c=addr); addr+=256*RB(wrap(c,c+1)))// indirect w/ page wrap
  t("  *Z  *Z  *Z  *Z      6z  *Z  *Z ", Misfire(addr, addr+d)) // abs. load: extra misread when cross-page
  t("  4k  4k  4k  4k  6z      4k  4k ", RB(wrap(addr, addr+d)))// abs. store: always issue a misread
  /* Load source operand */
  t("aa__ff__ab__,4  ____ -  ____     ", t &= A) // Many operations take A or X as operand. Some try in
  t("                knnn     4  99   ", t &= X) // error to take both; the outcome is an AND operation.
  t("                9989    99       ", t &= Y) // sty,dey,iny,tya,cpy
  t("                       4         ", t &= S) // tsx, las
  t("!!!!  !!  !!  !!  !   !!  !!  !!/", t &= P.raw|pbits; c = t)// php, flag test/set/clear, interrupts
  t("_^__dc___^__            ed__98   ", c = t; t = 0xFF)        // save as second operand
  t("vuwvzywvvuwvvuwv    zy|zzywvzywv ", t &= RB(addr+d)) // memory operand
  t(",2  ,2  ,2  ,2  -2  -2  -2  -2   ", t &= RB(PC++))   // immediate operand
  /* Operations that mogrify memory operands directly */
  t("    88                           ", P.V = t & 0x40; P.N = t & 0x80) // bit
  t("    nink    nnnk                 ", sb = P.C)       // rol,rla, ror,rra,arr
  t("nnnknnnk     0                   ", P.C = t & 0x80) // rol,rla, asl,slo,[arr,anc]
  t("        nnnknink                 ", P.C = t & 0x01) // lsr,sre, ror,rra,asr
  t("ninknink                         ", t = (t << 1) | (sb * 0x01))
  t("        nnnknnnk                 ", t = (t >> 1) | (sb * 0x80))
  t("                 !      kink     ", t = u8(t - 1))  // dec,dex,dey,dcp
  t("                         !  khnk ", t = u8(t + 1))  // inc,inx,iny,isb
  /* Store modified value (memory) */
  t("kgnkkgnkkgnkkgnkzy|J    kgnkkgnk ", WB(addr+d, t))
  t("                   q             ", WB(wrap(addr, addr+d), t &= ((addr+d) >> 8))) // [shx,shy,shs,sha?]
  /* Some operations used up one clock cycle that we did not account for yet */
  t("rpstljstqjstrjst - - - -kjstkjst/", Tick()) // nop,flag ops,inc,dec,shifts,stack,transregister,interrupts
  /* Stack operations and unconditional jumps */
  t("     !  !    !                   ", Tick(); t = Pop())                        // pla,plp,rti
  t("        !   !                    ", RB(PC++); PC = Pop(); PC |= (Pop() << 8)) // rti,rts
  t("            !                    ", RB(PC++))  // rts
  t("!   !                           /", d=PC+(op?-1:1); Push(d>>8); Push(d))      // jsr, interrupts
  t("!   !    8   8                  /", PC = addr) // jmp, jsr, interrupts
  t("!!       !                      /", Push(t))   // pha, php, interrupts
  /* Bitmasks */
  t("! !!  !!  !!  !!  !   !!  !!  !!/", t = 1)
  t("  !   !                   !!  !! ", t <<= 1)
  t("! !   !   !!  !!       !   !   !/", t <<= 2)
  t("  !   !   !   !        !         ", t <<= 4)
  t("   !       !           !   !____ ", t = u8(~t)) // sbc, isb,      clear flag
  t("`^__   !       !               !/", t = c | t)  // ora, slo,      set flag
  t("  !!dc`_  !!  !   !   !!  !!  !  ", t = c & t)  // and, bit, rla, clear/test flag
  t("        _^__                     ", t = c ^ t)  // eor, sre
  /* Conditional branches */
  t("      !       !       !       !  ", if(t)  { Tick(); Misfire(PC, addr = s8(addr) + PC); PC=addr; })
  t("  !       !       !       !      ", if(!t) { Tick(); Misfire(PC, addr = s8(addr) + PC); PC=addr; })
  /* Addition and subtraction */
  t("            _^__            ____ ", c = t; t += A + P.C; P.V = (c^t) & (A^t) & 0x80; P.C = t & 0x100)
  t("                        ed__98   ", t = c - t; P.C = ~t & 0x100) // cmp,cpx,cpy, dcp, sbx
  /* Store modified value (register) */
  t("aa__aa__aa__ab__ 4 !____    ____ ", A = t)
  t("                    nnnn 4   !   ", X = t) // ldx, dex, tax, inx, tsx,lax,las,sbx
  t("                 !  9988 !       ", Y = t) // ldy, dey, tay, iny
  t("                   4   0         ", S = t) // txs, las, shs
  t("!  ! ! !!  !   !       !   !   !/", P.raw = t & ~0x30) // plp, rti, flag set/clear
  /* Generic status flag updates */
  t("wwwvwwwvwwwvwxwv 5 !}}||{}wv{{wv ", P.N = t & 0x80)
  t("wwwv||wvwwwvwxwv 5 !}}||{}wv{{wv ", P.Z = u8(t) == 0)
  t("             0                   ", P.V = (((t >> 5)+1)&2))         // [arr]
  /* clang-format on */
  /* All implemented opcodes are cycle-accurate and memory-access-accurate.
   * [] means that this particular separate rule exists only to provide the
   * indicated unofficial opcode(s). */
}

void Op() {
  /* Check the state of NMI flag */
  bool nmi_now = nmi;

  unsigned op = RB(PC++);

  if (reset) {
    op = 0x101;
  } else if (nmi_now && !nmi_edge_detected) {
    op = 0x100;
    nmi_edge_detected = true;
  } else if (intr && !P.I) {
    op = 0x102;
  }
  if (!nmi_now) nmi_edge_detected = false;

    // Define function pointers for each opcode (00..FF) and each interrupt
    // (100,101,102)
#define c(n) Ins<0x##n>, Ins<0x##n + 1>,
#define o(n) c(n) c(n + 2) c(n + 4) c(n + 6)
  static void (*const i[0x108])() = {
      o(00) o(08) o(10) o(18) o(20) o(28) o(30) o(38) o(40) o(48) o(50) o(58)
          o(60) o(68) o(70) o(78) o(80) o(88) o(90) o(98) o(A0) o(A8) o(B0)
              o(B8) o(C0) o(C8) o(D0) o(D8) o(E0) o(E8) o(F0) o(F8) o(100)};
#undef o
#undef c
  i[op]();

  reset = false;
}

}  // namespace CPU

char* GetLine(void) {
  static char* line;
  static size_t linesize;
  if (getline(&line, &linesize, stdin) > 0) {
    return chomp(line);
  } else {
    return NULL;
  }
}

int PlayGame(const char* romfile, const char* opt_tasfile) {
  FILE* fp;
  int devnull;
  int pipefds[2];
  const char* ffplay;
  inputfn_ = opt_tasfile;

  if (!(fp = fopen(romfile, "rb"))) {
    fprintf(stderr, "%s%s\n", "failed to open: ", romfile);
    return 2;
  }
  if (!(fgetc(fp) == 'N' && fgetc(fp) == 'E' && fgetc(fp) == 'S' &&
        fgetc(fp) == CTRL('Z'))) {
    fprintf(stderr, "%s%s\n", "not a nes rom file: ", romfile);
    return 3;
  }

  InitPalette();

  // open speaker
  // todo: this needs plenty of work
  if (!IsWindows()) {
    if ((ffplay = commandvenv("FFPLAY", "ffplay"))) {
      devnull = open("/dev/null", O_WRONLY | O_CLOEXEC);
      pipe2(pipefds, O_CLOEXEC);
      if (!(playpid_ = fork())) {
        const char* const args[] = {
            ffplay,                  //
            "-nodisp",               //
            "-loglevel", "quiet",    //
            "-ac",       "1",        //
            "-ar",       "1789773",  //
            "-f",        "s16le",    //
            "pipe:",                 //
            NULL,
        };
        dup2(pipefds[0], 0);
        dup2(devnull, 1);
        dup2(devnull, 2);
        execv(ffplay, (char* const*)args);
        abort();
      }
      close(pipefds[0]);
      playfd_ = pipefds[1];
    } else {
      fputs("\nWARNING\n\
\n\
  Need `ffplay` command to play audio\n\
  Try `sudo apt install ffmpeg` on Linux\n\
  You can specify it on `PATH` or in `FFPLAY`\n\
\n\
Press enter to continue without sound: ",
            stdout);
      fflush(stdout);
      GetLine();
    }
  }

  // Read the ROM file header
  u8 rom16count = fgetc(fp);
  u8 vrom8count = fgetc(fp);
  u8 ctrlbyte = fgetc(fp);
  u8 mappernum = fgetc(fp) | ctrlbyte >> 4;

  fgetc(fp);
  fgetc(fp);
  fgetc(fp);
  fgetc(fp);
  fgetc(fp);
  fgetc(fp);
  fgetc(fp);
  fgetc(fp);

  if (mappernum >= 0x40) mappernum &= 15;
  GamePak::mappernum = mappernum;

  // Read the ROM data
  if (rom16count) GamePak::ROM.resize(rom16count * 0x4000);
  if (vrom8count) GamePak::VRAM.resize(vrom8count * 0x2000);
  fread(&GamePak::ROM[0], rom16count, 0x4000, fp);
  fread(&GamePak::VRAM[0], vrom8count, 0x2000, fp);

  fclose(fp);
  printf("%u * 16kB ROM, %u * 8kB VROM, mapper %u, ctrlbyte %02X\n", rom16count,
         vrom8count, mappernum, ctrlbyte);

  // Start emulation
  GamePak::Init();
  IoInit();
  PPU::reg.value = 0;

  // Pre-initialize RAM the same way as FCEUX does, to improve TAS sync.
  for (unsigned a = 0; a < 0x800; ++a) CPU::RAM[a] = (a & 4) ? 0xFF : 0x00;

  // Run the CPU until the program is killed.
  for (;;) CPU::Op();
}

wontreturn void PrintUsage(int rc, FILE* f) {
  fprintf(f, "%s%s%s", "Usage: ", program_invocation_name, USAGE);
  exit(rc);
}

void GetOpts(int argc, char* argv[]) {
  int opt;
  while ((opt = getopt(argc, argv, "?hAxt134")) != -1) {
    switch (opt) {
      case 'A':
        quant_ = kTtyQuantAnsi;
        break;
      case 'x':
        quant_ = kTtyQuantXterm256;
        break;
      case 't':
        quant_ = kTtyQuantTrue;
        break;
      case '1':
        artifacts_ = !artifacts_;
        break;
      case '3':
        blocks_ = kTtyBlocksCp437;
        break;
      case '4':
        blocks_ = kTtyBlocksUnicode;
        break;
      case 'h':
      case '?':
        PrintUsage(EXIT_SUCCESS, stdout);
      default:
        PrintUsage(EX_USAGE, stderr);
    }
  }
}

size_t FindZipGames(void) {
  char* name;
  size_t i, cf;
  struct Zipos* zipos;
  if ((zipos = __zipos_get())) {
    for (i = 0, cf = ZIP_CDIR_OFFSET(zipos->cdir);
         i < ZIP_CDIR_RECORDS(zipos->cdir);
         ++i, cf += ZIP_CFILE_HDRSIZE(zipos->map + cf)) {
      if (ZIP_CFILE_NAMESIZE(zipos->map + cf) > 4 &&
          !memcmp((ZIP_CFILE_NAME(zipos->map + cf) +
                   ZIP_CFILE_NAMESIZE(zipos->map + cf) - 4),
                  ".nes", 4) &&
          (name = xasprintf("/zip/%.*s", ZIP_CFILE_NAMESIZE(zipos->map + cf),
                            ZIP_CFILE_NAME(zipos->map + cf)))) {
        APPEND(&zipgames_.p, &zipgames_.i, &zipgames_.n, &name);
      }
    }
  }
  return zipgames_.i;
}

int SelectGameFromZip(void) {
  int i, rc;
  char *line, *uri;
  fputs("\nCOSMOPOLITAN NESEMU1\n\n", stdout);
  for (i = 0; i < (int)zipgames_.i; ++i) {
    printf("  [%d] %s\n", i, zipgames_.p[i]);
  }
  fputs("\nPlease choose a game (or CTRL-C to quit) [default 0]: ", stdout);
  fflush(stdout);
  rc = 0;
  if ((line = GetLine())) {
    i = MAX(0, MIN((int)zipgames_.i - 1, atoi(line)));
    uri = zipgames_.p[i];
    rc = PlayGame(uri, NULL);
    free(uri);
  } else {
    fputs("\n", stdout);
  }
  return rc;
}

int main(int argc, char** argv) {
  int rc;
  GetOpts(argc, argv);
  if (optind + 1 < argc) {
    rc = PlayGame(argv[optind], argv[optind + 1]);
  } else if (optind < argc) {
    rc = PlayGame(argv[optind], NULL);
  } else {
    if (!FindZipGames()) {
      PrintUsage(0, stderr);
    }
    rc = SelectGameFromZip();
  }
  return rc;
}
