#if 0
/*─────────────────────────────────────────────────────────────────╗
│ To the extent possible under law, Justine Tunney has waived      │
│ all copyright and related or neighboring rights to this file,    │
│ as it is written in the following disclaimers:                   │
│   • http://unlicense.org/                                        │
│   • http://creativecommons.org/publicdomain/zero/1.0/            │
╚─────────────────────────────────────────────────────────────────*/
#endif
#include "libc/bits/bits.h"
#include "libc/calls/calls.h"
#include "libc/errno.h"
#include "libc/fmt/conv.h"
#include "libc/log/check.h"
#include "libc/log/log.h"
#include "libc/macros.internal.h"
#include "libc/nexgen32e/x86feature.h"
#include "libc/rand/rand.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/ex.h"
#include "libc/sysv/consts/exit.h"
#include "libc/sysv/consts/grnd.h"
#include "libc/sysv/consts/sig.h"
#include "libc/testlib/hyperion.h"
#include "third_party/getopt/getopt.h"

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

uint64_t vigna(void) {
  static uint64_t x;
  uint64_t z = (x += 0x9e3779b97f4a7c15);
  z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9;
  z = (z ^ (z >> 27)) * 0x94d049bb133111eb;
  return z ^ (z >> 31);
}

uint64_t libc(void) {
  uint64_t x;
  CHECK_EQ(8, getrandom(&x, 8, 0));
  return x;
}

uint64_t kernel(void) {
  uint64_t x;
  CHECK_EQ(8, getrandom(&x, 8, GRND_NORDRND));
  return x;
}

uint64_t hardware(void) {
  uint64_t x;
  CHECK_EQ(8, getrandom(&x, 8, GRND_NOSYSTEM));
  return x;
}

uint64_t rdrnd(void) {
  char cf;
  int i = 0;
  uint64_t x;
  CHECK(X86_HAVE(RDRND));
  for (;;) {
    asm volatile(CFLAG_ASM("rdrand\t%1")
                 : CFLAG_CONSTRAINT(cf), "=r"(x)
                 : /* no inputs */
                 : "cc");
    if (cf) return x;
    if (++i < 10) continue;
    asm volatile("pause");
    i = 0;
  }
}

uint64_t rdseed(void) {
  char cf;
  int i = 0;
  uint64_t x;
  CHECK(X86_HAVE(RDSEED));
  for (;;) {
    asm volatile(CFLAG_ASM("rdseed\t%1")
                 : CFLAG_CONSTRAINT(cf), "=r"(x)
                 : /* no inputs */
                 : "cc");
    if (cf) return x;
    if (++i < 10) continue;
    asm volatile("pause");
    i = 0;
  }
}

const struct Function {
  const char *s;
  uint64_t (*f)(void);
} kFunctions[] = {
    {"ape", ape},            //
    {"hardware", hardware},  //
    {"inc", inc},            //
    {"kernel", kernel},      //
    {"knuth", knuth},        //
    {"libc", libc},          //
    {"moby", moby},          //
    {"rand64", rand64},      //
    {"rdrand", rdrnd},       //
    {"rdrnd", rdrnd},        //
    {"rdseed", rdseed},      //
    {"unixv6", unixv6},      //
    {"unixv7", unixv7},      //
    {"vigna", vigna},        //
    {"zero", zero},          //
};

bool isdone;
bool isbinary;
unsigned long count = -1;

void OnInt(int sig) {
  isdone = true;
}

wontreturn void PrintUsage(FILE *f, int rc) {
  fprintf(f, "Usage: %s [-b] [-n NUM] [FUNC]\n", program_invocation_name);
  exit(rc);
}

int main(int argc, char *argv[]) {
  int i, opt;
  ssize_t rc;
  uint64_t x;
  uint64_t (*f)(void);

  while ((opt = getopt(argc, argv, "hbn:")) != -1) {
    switch (opt) {
      case 'b':
        isbinary = true;
        break;
      case 'n':
        count = strtoul(optarg, 0, 0);
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
    x = f();
    rc = write(1, &x, MIN(8, count));
    if (!rc) break;
    if (rc == -1 && errno == EPIPE) return 1;
    if (rc == -1) perror("write"), exit(1);
    count -= rc;
  }

  return 0;
}
