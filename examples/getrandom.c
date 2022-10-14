#if 0
/*─────────────────────────────────────────────────────────────────╗
│ To the extent possible under law, Justine Tunney has waived      │
│ all copyright and related or neighboring rights to this file,    │
│ as it is written in the following disclaimers:                   │
│   • http://unlicense.org/                                        │
│   • http://creativecommons.org/publicdomain/zero/1.0/            │
╚─────────────────────────────────────────────────────────────────*/
#endif
#include "ape/sections.internal.h"
#include "libc/calls/calls.h"
#include "libc/calls/struct/sigaction.h"
#include "libc/errno.h"
#include "libc/fmt/conv.h"
#include "libc/intrin/bits.h"
#include "libc/log/check.h"
#include "libc/log/log.h"
#include "libc/macros.internal.h"
#include "libc/mem/mem.h"
#include "libc/nexgen32e/x86feature.h"
#include "libc/nt/runtime.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/rand.h"
#include "libc/stdio/stdio.h"
#include "libc/stdio/xorshift.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/ex.h"
#include "libc/sysv/consts/exit.h"
#include "libc/sysv/consts/grnd.h"
#include "libc/sysv/consts/sig.h"
#include "libc/testlib/hyperion.h"
#include "third_party/getopt/getopt.h"

#define B 4096

bool isdone;
bool isbinary;
unsigned long count = -1;

uint64_t bcast(uint64_t f(void)) {
  unsigned i;
  uint64_t x;
  for (x = i = 0; i < 8; ++i) {
    x <<= 8;
    x |= f() & 255;
  }
  return x;
}

uint64_t randv6(void) {
  static int16_t gorp;
  gorp = (gorp + 625) & 077777;
  return gorp;
}

uint64_t randv7(void) {
  static uint32_t randx = 1;
  return ((randx = randx * 1103515245 + 12345) >> 16) & 077777;
}

uint64_t zero(void) {
  return 0;
}

uint64_t inc(void) {
  static uint64_t x;
  return x++;
}

uint64_t unixv6(void) {
  return bcast(randv6);
}

uint64_t unixv7(void) {
  return bcast(randv7);
}

uint64_t ape(void) {
  static int i;
  if ((i += 8) > _end - _base) i = 8;
  return READ64LE(_base + i);
}

uint64_t moby(void) {
  static int i;
  if ((i += 8) > kMobySize) i = 8;
  return READ64LE(kMoby + i);
}

uint64_t knuth(void) {
  uint64_t a, b;
  static uint64_t x = 1;
  x *= 6364136223846793005;
  x += 1442695040888963407;
  a = x >> 32;
  x *= 6364136223846793005;
  x += 1442695040888963407;
  b = x >> 32;
  return a | b << 32;
}

uint64_t rngset64(void) {
  static unsigned i;
  static uint64_t s;
  if (!i) {
    s = _rand64();
    i = (s + 1) & (511);
  }
  return MarsagliaXorshift64(&s);
}

uint64_t xorshift64(void) {
  static uint64_t s = kMarsagliaXorshift64Seed;
  return MarsagliaXorshift64(&s);
}

uint64_t xorshift32(void) {
  static uint32_t s = kMarsagliaXorshift32Seed;
  uint64_t a = MarsagliaXorshift32(&s);
  uint64_t b = MarsagliaXorshift32(&s);
  return (uint64_t)a << 32 | b;
}

uint64_t libc(void) {
  uint64_t x;
  CHECK_EQ(8, getrandom(&x, 8, 0));
  return x;
}

uint64_t GetRandom(void) {
  uint64_t x;
  CHECK_EQ(8, getrandom(&x, 8, 0));
  return x;
}

uint32_t python(void) {
#define K 0  // 624 /* wut */
#define N 624
#define M 397
  static int index;
  static char once;
  static uint32_t mt[N];
  static const uint32_t mag01[2] = {0, 0x9908b0dfu};
  uint32_t y;
  int kk;
  if (!once) {
    char *sp;
    ssize_t rc;
    uint32_t i, j, k, s[K];
    mt[0] = 19650218;
    for (i = 1; i < N; i++) {
      mt[i] = (1812433253u * (mt[i - 1] ^ (mt[i - 1] >> 30)) + i);
    }
    if (K) {
      for (sp = (char *)s, i = 0; i < sizeof(s); i += rc) {
        if ((rc = getrandom(sp + i, sizeof(s) - i, 0)) == -1) {
          if (errno != EINTR) abort();
        }
      }
      for (i = 1, j = 0, k = MAX(N, K); k; k--) {
        mt[i] =
            (mt[i] ^ ((mt[i - 1] ^ (mt[i - 1] >> 30)) * 1664525u)) + s[j] + j;
        if (++i >= N) mt[0] = mt[N - 1], i = 1;
        if (++j >= K) j = 0;
      }
      for (k = N - 1; k; k--) {
        mt[i] = (mt[i] ^ ((mt[i - 1] ^ (mt[i - 1] >> 30)) * 1566083941u)) - i;
        if (++i >= N) mt[0] = mt[N - 1], i = 1;
      }
      mt[0] = 0x80000000;
      explicit_bzero(s, sizeof(s));
    }
    once = 1;
  }
  if (index >= N) {
    for (kk = 0; kk < N - M; kk++) {
      y = (mt[kk] & 0x80000000u) | (mt[kk + 1] & 0x7fffffff);
      mt[kk] = mt[kk + M] ^ (y >> 1) ^ mag01[y & 1];
    }
    for (; kk < N - 1; kk++) {
      y = (mt[kk] & 0x80000000u) | (mt[kk + 1] & 0x7fffffff);
      mt[kk] = mt[kk + (M - N)] ^ (y >> 1) ^ mag01[y & 1];
    }
    y = (mt[N - 1] & 0x80000000u) | (mt[0] & 0x7fffffffu);
    mt[N - 1] = mt[M - 1] ^ (y >> 1) ^ mag01[y & 1];
    index = 0;
  }
  y = mt[index++];
  y ^= y >> 11;
  y ^= (y << 7) & 0x9d2c5680u;
  y ^= (y << 15) & 0xefc60000u;
  y ^= y >> 18;
  return y;
#undef M
#undef N
#undef K
}

uint64_t pythonx2(void) {
  uint64_t x = python();
  x <<= 32;
  x |= python();
  return x;
}

const struct Function {
  const char *s;
  uint64_t (*f)(void);
} kFunctions[] = {
    {"ape", ape},                //
    {"getrandom", GetRandom},    //
    {"inc", inc},                //
    {"knuth", knuth},            //
    {"lemur64", lemur64},        //
    {"libc", libc},              //
    {"moby", moby},              //
    {"mt19937", _mt19937},       //
    {"python", pythonx2},        //
    {"rand64", _rand64},         //
    {"rdrand", rdrand},          //
    {"rdrnd", rdrand},           //
    {"rdseed", rdseed},          //
    {"rngset64", rngset64},      //
    {"unixv6", unixv6},          //
    {"unixv7", unixv7},          //
    {"vigna", vigna},            //
    {"xorshift32", xorshift32},  //
    {"xorshift64", xorshift64},  //
    {"zero", zero},              //
};

void OnInt(int sig) {
  isdone = true;
}

wontreturn void PrintUsage(FILE *f, int rc) {
  fprintf(f, "Usage: %s [-b] [-n NUM] [FUNC]\n", program_invocation_name);
  exit(rc);
}

int main(int argc, char *argv[]) {
  char *p;
  int i, opt;
  ssize_t rc;
  uint64_t x;
  static char buf[B];
  uint64_t (*f)(void);

  while ((opt = getopt(argc, argv, "hbc:n:")) != -1) {
    switch (opt) {
      case 'b':
        isbinary = true;
        break;
      case 'c':
      case 'n':
        count = sizetol(optarg, 1024);
        break;
      case 'h':
        PrintUsage(stdout, EXIT_SUCCESS);
      default:
        PrintUsage(stderr, EX_USAGE);
    }
  }

  if (optind == argc) {
    f = libc;
  } else {
    for (f = 0, i = 0; i < ARRAYLEN(kFunctions); ++i) {
      if (!strcasecmp(argv[optind], kFunctions[i].s)) {
        f = kFunctions[i].f;
        break;
      }
    }
    if (!f) {
      fprintf(stderr, "unknown function: %`'s\n", argv[optind]);
      fprintf(stderr, "try: ");
      for (i = 0; i < ARRAYLEN(kFunctions); ++i) {
        if (i) fprintf(stderr, ", ");
        fprintf(stderr, "%s", kFunctions[i].s);
      }
      fprintf(stderr, "\n");
      return 1;
    }
  }

  signal(SIGINT, OnInt);
  signal(SIGPIPE, SIG_IGN);

  if (!isbinary) {
    for (; count && !isdone && !feof(stdout); --count) {
      printf("0x%016lx\n", f());
    }
    fflush(stdout);
    return ferror(stdout) ? 1 : 0;
  }

  while (count && !isdone) {
    if (count >= B) {
      for (i = 0; i < B / 8; ++i) {
        x = f();
        p = buf + i * 8;
        WRITE64LE(p, x);
      }
      for (i = 0; i < B; i += rc) {
        rc = write(1, buf + i, B - i);
        if (rc == -1 && errno == EPIPE) exit(1);
        if (rc == -1) perror("write"), exit(1);
      }
    } else {
      x = f();
      rc = write(1, &x, MIN(8, count));
    }
    if (!rc) break;
    if (rc == -1 && errno == EPIPE) exit(1);
    if (rc == -1) perror("write"), exit(1);
    count -= rc;
  }

  return 0;
}
