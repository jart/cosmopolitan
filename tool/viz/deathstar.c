#include "libc/calls/calls.h"
#include "libc/calls/struct/sigaction.h"
#include "libc/calls/struct/termios.h"
#include "libc/intrin/safemacros.internal.h"
#include "libc/math.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/sig.h"

/**
 * @fileoverview demo code borrowed from Rosetta Code.
 */

#define FRAMERATE 23.976
#define WRITE(s)  write(1, s, strlen(s))

#ifdef __x86_64__
__static_yoink("vga_console");
__static_yoink("EfiMain");
#endif

struct Sphere {
  double cx, cy, cz, r;
};

static const char *kShades[] = {
    "\e[48;5;232m ", "\e[48;5;233m ", "\e[48;5;234m ", "\e[48;5;235m ",
    "\e[48;5;236m ", "\e[48;5;237m ", "\e[48;5;238m ", "\e[48;5;239m ",
    "\e[48;5;240m ", "\e[48;5;241m ", "\e[48;5;242m ", "\e[48;5;243m ",
    "\e[48;5;244m ", "\e[48;5;245m ", "\e[48;5;246m ", "\e[48;5;247m ",
    "\e[48;5;248m ", "\e[48;5;249m ", "\e[48;5;250m ", "\e[48;5;251m ",
    "\e[48;5;252m ", "\e[48;5;253m ", "\e[48;5;254m ", "\e[48;5;255m ",
};

static jmp_buf jb_;
static double light_[3] = {-50, 0, 50};
static struct Sphere pos_ = {11, 11, 11, 11};
static struct Sphere neg_ = {1, 1, -4, 11};

static void OnCtrlC(int sig) {
  longjmp(jb_, 1);
}

static void Normalize(double v[3]) {
  double len;
  len = 1 / sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
  v[0] *= len;
  v[1] *= len;
  v[2] *= len;
}

static double Dot(const double x[3], const double y[3]) {
  return fabs(x[0] * y[0] + x[1] * y[1] + x[2] * y[2]);
}

/* check if a ray (x,y, -inf)->(x, y, inf) hits a sphere; if so, return
   the intersecting z values.  z1 is closer to the eye */
static int HitSphere(struct Sphere *s, double x, double y, double z[2]) {
  double zsq;
  x -= s->cx;
  y -= s->cy;
  zsq = s->r * s->r - (x * x + y * y);
  if (zsq < 0) {
    return 0;
  } else {
    zsq = sqrt(zsq);
    z[0] = s->cz - zsq;
    z[1] = s->cz + zsq;
    return 1;
  }
}

static void DrawSphere(double k, double ambient) {
  int i, j, hit_result;
  double x, y, vec[3], zb[2], zs[2];
  for (i = floor(pos_.cy - pos_.r); i <= ceil(pos_.cy + pos_.r); i++) {
    y = i + .5L;
    for (j = floor(pos_.cx - 2 * pos_.r); j <= ceil(pos_.cx + 2 * pos_.r);
         j++) {
      x = .5L * (j - pos_.cx) + .5L + pos_.cx;
      if (!HitSphere(&pos_, x, y, zb)) {
        /* ray lands in blank space, draw bg */
        hit_result = 0;
      } else if (!HitSphere(&neg_, x, y, zs)) {
        /* ray hits pos_ sphere but not neg_, draw pos_ sphere surface */
        hit_result = 1;
      } else if (zs[0] > zb[0]) {
        /* ray hits both, but pos_ front surface is closer */
        hit_result = 1;
      } else if (zs[1] > zb[1]) {
        /* pos_ sphere surface is inside neg_ sphere, show bg */
        hit_result = 0;
      } else if (zs[1] > zb[0]) {
        /* back surface on neg_ sphere is inside pos_ sphere,
           the only place where neg_ sphere surface will be shown */
        hit_result = 2;
      } else {
        hit_result = 1;
      }
      switch (hit_result) {
        case 0:
          WRITE("\e[0m ");
          continue;
        case 1:
          vec[0] = x - pos_.cx;
          vec[1] = y - pos_.cy;
          vec[2] = zb[0] - pos_.cz;
          break;
        default:
          vec[0] = neg_.cx - x;
          vec[1] = neg_.cy - y;
          vec[2] = neg_.cz - zs[1];
          break;
      }
      Normalize(vec);
      WRITE(
          kShades[min(ARRAYLEN(kShades) - 1,
                      max(0, lround((1 - (pow(Dot(light_, vec), k) + ambient)) *
                                    (ARRAYLEN(kShades) - 1))))]);
    }
    WRITE("\e[0m\n");
  }
  fflush(stdout);
}

int main() {
  double ang;
  WRITE("\e[?25l");
  if (!setjmp(jb_)) {
    signal(SIGINT, OnCtrlC);
    ang = 0;
    for (;;) {
      WRITE("\e[H");
      light_[1] = cosl(ang * 2);
      sincos(ang, &light_[0], &light_[2]);
      Normalize(light_);
      ang += .05L;
      DrawSphere(1.5L, .01L);
      usleep(1.L / FRAMERATE * 1e6);
    }
  }
  WRITE("\e[0m\e[H\e[J\e[?25h");
  return 0;
}
