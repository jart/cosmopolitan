/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2021 Justine Alexandra Roberts Tunney                              │
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
#include "libc/calls/calls.h"
#include "libc/calls/struct/sigaction.h"
#include "libc/calls/struct/sigset.h"
#include "libc/errno.h"
#include "libc/fmt/conv.h"
#include "libc/intrin/bits.h"
#include "libc/log/check.h"
#include "libc/log/log.h"
#include "libc/mem/gc.internal.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/ex.h"
#include "libc/sysv/consts/exit.h"
#include "libc/sysv/consts/nr.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/sa.h"
#include "libc/sysv/consts/sig.h"
#include "libc/x/xasprintf.h"
#include "libc/x/xgetline.h"
#include "third_party/dlmalloc/dlmalloc.h"
#include "third_party/getopt/getopt.h"

/**
 * @fileoverview Pythonic System Call Trace
 *
 * This program invokes `strace` as a subprocess and turns its output
 * into Python data structures. It is useful because strace output is
 * this weird plaintext format that's so famously difficult to parse.
 *
 * For example, you can run this command:
 *
 *     pstrace -o trace.pylog echo hello world
 *
 * After which you may parse the output with Python:
 *
 *     for line in open('trace.pylog'):
 *       pid,time,elap,kind,x = eval(line)
 *       if kind == 1:
 *         name,ret,args = x
 *         print "%s%r -> %d" % (name, args, ret)
 *
 * This program traces the subset of system calls governing processes
 * and files. To do that we must track file descriptor lifetimes too.
 * We also track system calls that are problematic for build configs,
 * such as sockets, since compiling code shouldn't need the Internet.
 *
 * @note this tool is linux only
 * @note freebsd: truss PROC ARGS
 * @note appleos: sudo dtruss PROC ARGS
 * @note openbsd: ktrace PROC ARGS && kdump -f ktrace.out
 * @note windows: https://github.com/rogerorr/NtTrace
 */

#define DEBUG        "%ld: %s", lineno, line
#define READ128BE(S) ((uint128_t)READ64BE(S) << 64 | READ64BE((S) + 8))
#define APPEND(L)                             \
  do {                                        \
    if (++L.n > L.c) {                        \
      L.c = MAX(11, L.c);                     \
      L.c += L.c >> 1;                        \
      L.p = realloc(L.p, L.c * sizeof(*L.p)); \
    }                                         \
    bzero(L.p + L.n - 1, sizeof(*L.p));       \
  } while (0)

struct Trace {
  struct Slices {
    long n, c;
    struct Slice {
      int n, c;
      char *p;
    } * p;
  } slices;
  struct HashTable {
    long i, n;
    struct HashEntry {
      long h;
      long i;
    } * p;
  } sliceindex;
  struct Strlists {
    long n, c;
    struct Strlist {
      long n, c;
      long *p;  // slices.p[p[i]]
    } * p;
  } strlists;
  struct Events {
    long n, c;
    struct Event {
      enum EventKind {
        EK_NONE,
        EK_CALL,
        EK_EXIT,    // ret is kernel code
        EK_SIGNAL,  // ret is signal code
        EK_KILLED,  // ret is signal code
      } kind;
      unsigned char arity;
      unsigned char syscall_;
      bool is_interrupted;
      int us;
      int elap;
      int pid;
      long sec;
      long ret;
      long lineno;
      struct Arg {
        enum ArgKind {
          AK_LONG,     // x
          AK_STR,      // slices.p[x]
          AK_STRLIST,  // strlists.p[x]
          AK_INTPAIR,  // (x&0xffffffff, x>>32)
        } kind;
        long name;
        long x;
      } arg[6];
    } * p;
  } events;
};

static const struct Syscall {
  char name[16];
} kSyscalls[] = {
    {"accept"},          //
    {"accept4"},         //
    {"access"},          //
    {"bind"},            //
    {"chdir"},           //
    {"chmod"},           //
    {"chown"},           //
    {"chroot"},          //
    {"clone"},           //
    {"close"},           //
    {"connect"},         //
    {"creat"},           //
    {"dup"},             //
    {"dup2"},            //
    {"dup3"},            //
    {"epoll_create"},    //
    {"epoll_create1"},   //
    {"eventfd"},         //
    {"eventfd2"},        //
    {"execve"},          //
    {"execveat"},        //
    {"faccessat"},       //
    {"fchmodat"},        //
    {"fchownat"},        //
    {"fdatasync"},       //
    {"fcntl"},           //
    {"flock"},           //
    {"fork"},            //
    {"fsync"},           //
    {"lchown"},          //
    {"link"},            //
    {"linkat"},          //
    {"listen"},          //
    {"memfd_create"},    //
    {"mkdir"},           //
    {"mkdirat"},         //
    {"mknod"},           //
    {"mknodat"},         //
    {"open"},            //
    {"openat"},          //
    {"pipe"},            //
    {"pipe2"},           //
    {"readlink"},        //
    {"readlinkat"},      //
    {"rename"},          //
    {"renameat"},        //
    {"renameat2"},       //
    {"rmdir"},           //
    {"signalfd"},        //
    {"signalfd4"},       //
    {"socket"},          //
    {"socketpair"},      //
    {"statfs"},          //
    {"symlink"},         //
    {"symlinkat"},       //
    {"sync"},            //
    {"syncfs"},          //
    {"timerfd_create"},  //
    {"truncate"},        //
    {"unlink"},          //
    {"unlinkat"},        //
    {"utimensat"},       //
    {"vfork"},           //
};

static const struct Signal {
  char name[8];
  unsigned char number;
} kSignals[] = {
    {"SIGABRT", 6},    //
    {"SIGALRM", 14},   //
    {"SIGBUS", 7},     //
    {"SIGCHLD", 17},   //
    {"SIGCONT", 18},   //
    {"SIGFPE", 8},     //
    {"SIGHUP", 1},     //
    {"SIGILL", 4},     //
    {"SIGINT", 2},     //
    {"SIGIO", 29},     //
    {"SIGIOT", 6},     //
    {"SIGKILL", 9},    //
    {"SIGPIPE", 13},   //
    {"SIGPOLL", 29},   //
    {"SIGPROF", 27},   //
    {"SIGPWR", 30},    //
    {"SIGQUIT", 3},    //
    {"SIGSEGV", 11},   //
    {"SIGSTOP", 19},   //
    {"SIGSYS", 31},    //
    {"SIGTERM", 15},   //
    {"SIGTRAP", 5},    //
    {"SIGTSTP", 20},   //
    {"SIGTTIN", 21},   //
    {"SIGTTOU", 22},   //
    {"SIGURG", 23},    //
    {"SIGUSR1", 10},   //
    {"SIGUSR2", 12},   //
    {"SIGWINCH", 28},  //
    {"SIGXCPU", 24},   //
    {"SIGXFSZ", 25},   //
};

static const struct Errno {
  char name[16];
  unsigned char number;
} kErrnos[] = {
    {"E2BIG", 7},              //
    {"EACCES", 13},            //
    {"EADDRINUSE", 98},        //
    {"EADDRNOTAVAIL", 99},     //
    {"EADV", 68},              //
    {"EAFNOSUPPORT", 97},      //
    {"EAGAIN", 11},            //
    {"EALREADY", 114},         //
    {"EBADE", 52},             //
    {"EBADF", 9},              //
    {"EBADFD", 77},            //
    {"EBADMSG", 74},           //
    {"EBADR", 53},             //
    {"EBADRQC", 56},           //
    {"EBADSLT", 57},           //
    {"EBFONT", 59},            //
    {"EBUSY", 16},             //
    {"ECANCELED", 125},        //
    {"ECHILD", 10},            //
    {"ECHRNG", 44},            //
    {"ECOMM", 70},             //
    {"ECONNABORTED", 103},     //
    {"ECONNREFUSED", 111},     //
    {"ECONNRESET", 104},       //
    {"EDEADLK", 35},           //
    {"EDESTADDRREQ", 89},      //
    {"EDOM", 33},              //
    {"EDOTDOT", 73},           //
    {"EDQUOT", 122},           //
    {"EEXIST", 17},            //
    {"EFAULT", 14},            //
    {"EFBIG", 27},             //
    {"EHOSTDOWN", 112},        //
    {"EHOSTUNREACH", 113},     //
    {"EHWPOISON", 133},        //
    {"EIDRM", 43},             //
    {"EILSEQ", 84},            //
    {"EINPROGRESS", 115},      //
    {"EINTR", 4},              //
    {"EINVAL", 22},            //
    {"EIO", 5},                //
    {"EISCONN", 106},          //
    {"EISDIR", 21},            //
    {"EISNAM", 120},           //
    {"EKEYEXPIRED", 127},      //
    {"EKEYREJECTED", 129},     //
    {"EKEYREVOKED", 128},      //
    {"EL2HLT", 51},            //
    {"EL2NSYNC", 45},          //
    {"EL3HLT", 46},            //
    {"EL3RST", 47},            //
    {"ELIBACC", 79},           //
    {"ELIBBAD", 80},           //
    {"ELIBEXEC", 83},          //
    {"ELIBMAX", 82},           //
    {"ELIBSCN", 81},           //
    {"ELNRNG", 48},            //
    {"ELOOP", 40},             //
    {"EMEDIUMTYPE", 124},      //
    {"EMFILE", 24},            //
    {"EMLINK", 31},            //
    {"EMSGSIZE", 90},          //
    {"EMULTIHOP", 72},         //
    {"ENAMETOOLONG", 36},      //
    {"ENAVAIL", 119},          //
    {"ENETDOWN", 100},         //
    {"ENETRESET", 102},        //
    {"ENETUNREACH", 101},      //
    {"ENFILE", 23},            //
    {"ENOANO", 55},            //
    {"ENOBUFS", 105},          //
    {"ENOCSI", 50},            //
    {"ENODATA", 61},           //
    {"ENODEV", 19},            //
    {"ENOENT", 2},             //
    {"ENOEXEC", 8},            //
    {"ENOKEY", 126},           //
    {"ENOLCK", 37},            //
    {"ENOLINK", 67},           //
    {"ENOMEDIUM", 123},        //
    {"ENOMEM", 12},            //
    {"ENOMSG", 42},            //
    {"ENONET", 64},            //
    {"ENOPKG", 65},            //
    {"ENOPROTOOPT", 92},       //
    {"ENOSPC", 28},            //
    {"ENOSR", 63},             //
    {"ENOSTR", 60},            //
    {"ENOSYS", 38},            //
    {"ENOTBLK", 15},           //
    {"ENOTCONN", 107},         //
    {"ENOTDIR", 20},           //
    {"ENOTEMPTY", 39},         //
    {"ENOTNAM", 118},          //
    {"ENOTRECOVERABLE", 131},  //
    {"ENOTSOCK", 88},          //
    {"ENOTSUP", 95},           //
    {"ENOTTY", 25},            //
    {"ENOTUNIQ", 76},          //
    {"ENXIO", 6},              //
    {"EOPNOTSUPP", 95},        //
    {"EOVERFLOW", 75},         //
    {"EOWNERDEAD", 130},       //
    {"EPERM", 1},              //
    {"EPFNOSUPPORT", 96},      //
    {"EPIPE", 32},             //
    {"EPROTO", 71},            //
    {"EPROTONOSUPPORT", 93},   //
    {"EPROTOTYPE", 91},        //
    {"ERANGE", 34},            //
    {"EREMCHG", 78},           //
    {"EREMOTE", 66},           //
    {"EREMOTEIO", 121},        //
    {"ERESTART", 85},          //
    {"ERFKILL", 132},          //
    {"EROFS", 30},             //
    {"ESHUTDOWN", 108},        //
    {"ESOCKTNOSUPPORT", 94},   //
    {"ESPIPE", 29},            //
    {"ESRCH", 3},              //
    {"ESRMNT", 69},            //
    {"ESTALE", 116},           //
    {"ESTRPIPE", 86},          //
    {"ETIME", 62},             //
    {"ETIMEDOUT", 110},        //
    {"ETOOMANYREFS", 109},     //
    {"ETXTBSY", 26},           //
    {"EUCLEAN", 117},          //
    {"EUNATCH", 49},           //
    {"EUSERS", 87},            //
    {"EWOULDBLOCK", 11},       //
    {"EXDEV", 18},             //
    {"EXFULL", 54},            //
};

static char **strace_args;
static size_t strace_args_len;
static volatile bool interrupted;

static long Hash(const void *p, size_t n) {
  unsigned h, i;
  for (h = i = 0; i < n; i++) {
    h += ((unsigned char *)p)[i];
    h *= 0x9e3779b1;
  }
  return MAX(1, h);
}

static uint64_t MakeKey64(const char *p, size_t n) {
  char k[8] = {0};
  memcpy(k, p, n);
  return READ64BE(k);
}

static uint128_t MakeKey128(const char *p, size_t n) {
  char k[16] = {0};
  memcpy(k, p, n);
  return READ128BE(k);
}

static int GetSyscall(const char *name, size_t namelen) {
  int m, l, r;
  uint128_t x, y;
  char *endofname;
  if (namelen && namelen <= 16) {
    x = MakeKey128(name, namelen);
    l = 0;
    r = ARRAYLEN(kSyscalls) - 1;
    while (l <= r) {
      m = (l + r) >> 1;
      y = READ128BE(kSyscalls[m].name);
      if (x < y) {
        r = m - 1;
      } else if (x > y) {
        l = m + 1;
      } else {
        return m;
      }
    }
  }
  return -1;
}

static int GetErrno(const char *name, size_t namelen) {
  int m, l, r;
  uint128_t x, y;
  char *endofname;
  if (namelen && namelen <= 16) {
    x = MakeKey128(name, namelen);
    l = 0;
    r = ARRAYLEN(kErrnos) - 1;
    while (l <= r) {
      m = (l + r) >> 1;
      y = READ128BE(kErrnos[m].name);
      if (x < y) {
        r = m - 1;
      } else if (x > y) {
        l = m + 1;
      } else {
        return kErrnos[m].number;
      }
    }
  }
  return -1;
}

static int GetSignal(const char *name, size_t namelen) {
  int m, l, r;
  uint64_t x, y;
  char *endofname;
  if (namelen && namelen <= 8) {
    x = MakeKey64(name, namelen);
    l = 0;
    r = ARRAYLEN(kSignals) - 1;
    while (l <= r) {
      m = (l + r) >> 1;
      y = READ64BE(kSignals[m].name);
      if (x < y) {
        r = m - 1;
      } else if (x > y) {
        l = m + 1;
      } else {
        return kSignals[m].number;
      }
    }
  }
  return -1;
}

static struct Trace *NewTrace(void) {
  return calloc(1, sizeof(struct Trace));
}

static void FreeTrace(struct Trace *t) {
  long i;
  if (t) {
    for (i = 0; i < t->slices.n; ++i) {
      free(t->slices.p[i].p);
    }
    free(t->slices.p);
    free(t->sliceindex.p);
    for (i = 0; i < t->strlists.n; ++i) {
      free(t->strlists.p[i].p);
    }
    free(t->strlists.p);
    free(t->events.p);
    free(t);
  }
}

static void AppendStrlists(struct Trace *t) {
  APPEND(t->strlists);
}

static void AppendStrlist(struct Strlist *l) {
  APPEND((*l));
}

static void AppendEvent(struct Trace *t) {
  APPEND(t->events);
}

static void AppendSlices(struct Trace *t) {
  APPEND(t->slices);
}

static void AppendSlice(struct Slice *s, int c) {
  APPEND((*s));
  s->p[s->n - 1] = c;
}

static long Intern(struct Trace *t, char *data, long size) {
  struct HashEntry *p;
  long i, j, k, n, m, h, n2;
  h = Hash(data, size);
  n = t->sliceindex.n;
  i = 0;
  if (n) {
    k = 0;
    do {
      i = (h + k + ((k + 1) >> 1)) & (n - 1);
      if (t->sliceindex.p[i].h == h &&
          t->slices.p[t->sliceindex.p[i].i].n == size &&
          !memcmp(t->slices.p[t->sliceindex.p[i].i].p, data, size)) {
        free(data);
        return t->sliceindex.p[i].i;
      }
      ++k;
    } while (t->sliceindex.p[i].h);
  }
  if (++t->sliceindex.i >= (n >> 1)) {
    m = n ? n << 1 : 16;
    p = calloc(m, sizeof(struct HashEntry));
    for (j = 0; j < n; ++j) {
      if (t->sliceindex.p[j].h) {
        k = 0;
        do {
          i = (t->sliceindex.p[j].h + k + ((k + 1) >> 1)) & (m - 1);
          ++k;
        } while (p[i].h);
        p[i].h = t->sliceindex.p[j].h;
        p[i].i = t->sliceindex.p[j].i;
      }
    }
    k = 0;
    do {
      i = (h + k + ((k + 1) >> 1)) & (m - 1);
      ++k;
    } while (p[i].h);
    free(t->sliceindex.p);
    t->sliceindex.p = p;
    t->sliceindex.n = m;
  }
  AppendSlices(t);
  t->slices.p[t->slices.n - 1].p = data;
  t->slices.p[t->slices.n - 1].n = size;
  t->sliceindex.p[i].i = t->slices.n - 1;
  t->sliceindex.p[i].h = h;
  return t->slices.n - 1;
}

static long ReadCharLiteral(struct Slice *buf, long c, char *p, long *i) {
  if (c != '\\') return c;
  switch ((c = p[(*i)++])) {
    case 'a':
      return '\a';
    case 'b':
      return '\b';
    case 't':
      return '\t';
    case 'n':
      return '\n';
    case 'v':
      return '\v';
    case 'f':
      return '\f';
    case 'r':
      return '\r';
    case 'e':
      return 033;
    case 'x':
      if (isxdigit(p[*i])) {
        c = hextoint(p[(*i)++]);
        if (isxdigit(p[*i])) {
          c = c * 16 + hextoint(p[(*i)++]);
        }
      }
      return c;
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
      c -= '0';
      if ('0' <= p[*i] && p[*i] <= '7') {
        c = c * 8 + (p[(*i)++] - '0');
        if ('0' <= p[*i] && p[*i] <= '7') {
          c = c * 8 + (p[(*i)++] - '0');
        }
      }
      return c;
    default:
      return c;
  }
}

static long GetDuration(long sec1, long us1, long sec2, long us2) {
  long elap;
  if ((elap = (sec2 - sec1) * 1000000)) {
    return elap + 1000000 - us1 + us2;
  } else {
    return elap + us2 - us1;
  }
}

static void Parse(struct Trace *t, const char *line, long lineno) {
  char *p, *q;
  struct Slice b;
  long c, i, j, k, arg, pid, event, sec, us;
  p = line;
  pid = strtol(p, &p, 10);
  while (*p == ' ') ++p;
  sec = strtol(p, &p, 10);
  CHECK_EQ('.', *p++, DEBUG);
  us = strtol(p, &p, 10);
  CHECK_EQ(' ', *p++, DEBUG);
  if (_startswith(p, "<... ")) {
    CHECK_NOTNULL((p = strchr(p, '>')));
    ++p;
    for (event = t->events.n; event--;) {
      if (t->events.p[event].pid == pid) {
        CHECK(t->events.p[event].is_interrupted, DEBUG);
        t->events.p[event].is_interrupted = false;
        t->events.p[event].elap =
            GetDuration(t->events.p[event].sec, t->events.p[event].us, sec, us);
        break;
      }
    }
    CHECK_GE(event, 0);
  } else {
    AppendEvent(t);
    event = t->events.n - 1;
    t->events.p[event].pid = pid;
    t->events.p[event].sec = sec;
    t->events.p[event].us = us;
    t->events.p[event].lineno = lineno;
    if (_startswith(p, "+++ exited with ")) {
      p += strlen("+++ exited with ");
      t->events.p[event].kind = EK_EXIT;
      t->events.p[event].ret = atoi(p);
      return;
    } else if (_startswith(p, "+++ killed by ")) {
      p += strlen("+++ killed by ");
      CHECK((q = strchr(p, ' ')), DEBUG);
      t->events.p[event].kind = EK_KILLED;
      t->events.p[event].ret = GetSignal(p, q - p);
      return;
    } else if (_startswith(p, "--- ")) {
      p += 4;
      CHECK(isalpha(*p), DEBUG);
      CHECK((q = strchr(p, ' ')), DEBUG);
      t->events.p[event].kind = EK_SIGNAL;
      t->events.p[event].ret = GetSignal(p, q - p);
      return;
    } else if (isalpha(*p) && (q = strchr(p, '('))) {
      t->events.p[event].kind = EK_CALL;
      CHECK_NE(-1, (t->events.p[event].syscall_ = GetSyscall(p, q - p)), DEBUG);
      p = q + 1;
    }
  }
  for (;;) {
    if (*p == ',') ++p;
    while (*p == ' ') ++p;
    CHECK(*p, DEBUG);
    if (_startswith(p, "<unfinished ...>")) {
      t->events.p[event].is_interrupted = true;
      break;
    } else if (*p == ')') {
      ++p;
      while (isspace(*p)) ++p;
      CHECK_EQ('=', *p++, DEBUG);
      while (isspace(*p)) ++p;
      CHECK(isdigit(*p) || *p == '-', DEBUG);
      t->events.p[event].ret = strtol(p, &p, 0);
      if (t->events.p[event].ret == -1) {
        while (isspace(*p)) ++p;
        CHECK((q = strchr(p, ' ')), DEBUG);
        if ((t->events.p[event].ret = GetErrno(p, q - p)) != -1) {
          t->events.p[event].ret = -t->events.p[event].ret;
        }
      }
      break;
    }
    CHECK_LT((arg = t->events.p[event].arity++), 6);
    if (isalpha(*p) && !_startswith(p, "NULL")) {
      bzero(&b, sizeof(b));
      for (; isalpha(*p) || *p == '_'; ++p) {
        AppendSlice(&b, *p);
      }
      t->events.p[event].arg[arg].name = Intern(t, b.p, b.n);
      CHECK_EQ('=', *p++, DEBUG);
    } else {
      t->events.p[event].arg[arg].name = -1;
    }
    if (_startswith(p, "NULL")) {
      p += 4;
      t->events.p[event].arg[arg].kind = AK_LONG;
      t->events.p[event].arg[arg].x = 0;
    } else if (*p == '-' || isdigit(*p)) {
      t->events.p[event].arg[arg].kind = AK_LONG;
      for (;;) {
        t->events.p[event].arg[arg].x |= strtol(p, &p, 0);
        if (*p == '|') {
          ++p;
        } else {
          break;
        }
      }
    } else if (*p == '{') {
      CHECK_NOTNULL((p = strchr(p, '}')), DEBUG);
      ++p;
    } else if (*p == '"') {
      bzero(&b, sizeof(b));
      for (j = 0; (c = p[++j]);) {
        if (c == '"') {
          p += j + 1;
          break;
        }
        c = ReadCharLiteral(&b, c, p, &j);
        AppendSlice(&b, c);
      }
      t->events.p[event].arg[arg].kind = AK_STR;
      t->events.p[event].arg[arg].x = Intern(t, b.p, b.n);
    } else if (*p == '[') {
      ++p;
      if (isdigit(*p)) {
        t->events.p[event].arg[arg].kind = AK_INTPAIR;
        t->events.p[event].arg[arg].x = strtol(p, &p, 0) & 0xffffffff;
        CHECK_EQ(',', *p++, DEBUG);
        CHECK_EQ(' ', *p++, DEBUG);
        t->events.p[event].arg[arg].x |= strtol(p, &p, 0) << 32;
        CHECK_EQ(']', *p++, DEBUG);
      } else {
        AppendStrlists(t);
        for (j = 0;; ++j) {
          if (*p == ']') {
            ++p;
            break;
          }
          if (*p == ',') ++p;
          if (*p == ' ') ++p;
          CHECK_EQ('"', *p, DEBUG);
          bzero(&b, sizeof(b));
          for (k = 0; (c = p[++k]);) {
            if (c == '"') {
              p += k + 1;
              break;
            }
            c = ReadCharLiteral(&b, c, p, &k);
            AppendSlice(&b, c);
          }
          AppendStrlist(&t->strlists.p[t->strlists.n - 1]);
          t->strlists.p[t->strlists.n - 1]
              .p[t->strlists.p[t->strlists.n - 1].n - 1] = Intern(t, b.p, b.n);
        }
        t->events.p[event].arg[arg].kind = AK_STRLIST;
        t->events.p[event].arg[arg].x = t->strlists.n - 1;
      }
    } else {
      CHECK(false, DEBUG);
    }
  }
}

static void PrintArg(FILE *f, struct Trace *t, long ev, long arg) {
  long i, x;
  x = t->events.p[ev].arg[arg].name;
  if (x != -1) {
    fprintf(f, "b%`'.*s:", t->slices.p[x].n, t->slices.p[x].p);
  }
  x = t->events.p[ev].arg[arg].x;
  switch (t->events.p[ev].arg[arg].kind) {
    case AK_LONG:
      fprintf(f, "%ld", x);
      break;
    case AK_STR:
      fprintf(f, "b%`'.*s", t->slices.p[x].n, t->slices.p[x].p);
      break;
    case AK_INTPAIR:
      fprintf(f, "(%d,%d)", x >> 32, x);
      break;
    case AK_STRLIST:
      fprintf(f, "(");
      for (i = 0; i < t->strlists.p[x].n; ++i) {
        fprintf(f, "b%`'.*s,", t->slices.p[t->strlists.p[x].p[i]].n,
                t->slices.p[t->strlists.p[x].p[i]].p);
      }
      fprintf(f, ")");
      break;
    default:
      abort();
  }
}

static void PrintEvent(FILE *f, struct Trace *t, long ev) {
  long arg;
  fprintf(f, "(%d,%ld,%d,%d,", t->events.p[ev].pid,
          t->events.p[ev].sec * 1000000 + t->events.p[ev].us,
          t->events.p[ev].elap, t->events.p[ev].kind);
  switch (t->events.p[ev].kind) {
    case EK_EXIT:
    case EK_SIGNAL:
    case EK_KILLED:
      fprintf(f, "%d", t->events.p[ev].ret);
      break;
    case EK_CALL:
      CHECK_LT(t->events.p[ev].syscall_, ARRAYLEN(kSyscalls));
      fprintf(f, "(b%`'s,%ld,", kSyscalls[t->events.p[ev].syscall_].name,
              t->events.p[ev].ret);
      fprintf(f, "%c",
              t->events.p[ev].arity && t->events.p[ev].arg[0].name != -1 ? '{'
                                                                         : '(');
      for (arg = 0; arg < t->events.p[ev].arity; ++arg) {
        PrintArg(f, t, ev, arg);
        fprintf(f, ",");
      }
      fprintf(f, "%c)",
              t->events.p[ev].arity && t->events.p[ev].arg[0].name != -1 ? '}'
                                                                         : ')');
      break;
    default:
      break;
  }
  fprintf(f, ")");
}

static void AppendArg(char *arg) {
  strace_args = realloc(strace_args, ++strace_args_len * sizeof(*strace_args));
  strace_args[strace_args_len - 1] = arg;
}

static wontreturn void PrintUsage(FILE *f, int rc) {
  fprintf(f, "Usage: %s [-o OUT] PROG [ARGS...]\n", program_invocation_name);
  exit(rc);
}

int main(int argc, char *argv[]) {
  int i;
  int ws;
  int opt;
  int pid;
  long ev;
  FILE *fin;
  FILE *fout;
  char *line;
  long lineno;
  char *strace;
  int pipefds[2];
  struct Trace *t;
  sigset_t block, mask;
  struct sigaction ignore, saveint, savequit;

  /*
   * parse prefix arguments
   */
  fout = stderr;
  while ((opt = getopt(argc, argv, "?ho:")) != -1) {
    switch (opt) {
      case 'o':
        fout = fopen(optarg, "w");
        break;
      case 'h':
      case '?':
        PrintUsage(stdout, EXIT_SUCCESS);
      default:
        PrintUsage(stderr, EX_USAGE);
    }
  }
  if (optind == argc) {
    PrintUsage(stderr, EX_USAGE);
  }

  /*
   * resolve full paths of dependencies
   */
  if ((strace = commandvenv("STRACE", "strace"))) {
    strace = strdup(strace);
  } else {
    fprintf(stderr, "error: please install strace\n");
    exit(1);
  }

  /*
   * create strace argument list
   */
  AppendArg("strace");
  AppendArg("-q");         // don't log attach/detach noise
  AppendArg("-v");         // don't abbreviate arrays
  AppendArg("-f");         // follow subprocesses
  AppendArg("-ttt");       // print unixseconds.micros
  AppendArg("-X");         // print numbers instead of symbols
  AppendArg("raw");        //   e.g. 2 vs. O_RDWR
  AppendArg("-s");         // don't abbreviate data
  AppendArg("805306368");  //   strace won't let us go higher
  AppendArg("-e");         // system calls that matter
  AppendArg(
      "open,close,access,pipe,dup,dup2,socket,connect,accept,bind,listen,"
      "socketpair,fork,vfork,execve,clone,flock,fsync,fdatasync,truncate,chdir,"
      "rename,mkdir,rmdir,creat,link,unlink,symlink,readlink,chmod,chown,fcntl,"
      "lchown,mknod,mknodat,statfs,chroot,sync,epoll_create,openat,mkdirat,"
      "fchownat,unlinkat,renameat,linkat,symlinkat,readlinkat,fchmodat,fchdir,"
      "faccessat,utimensat,accept4,dup3,pipe2,epoll_create1,signalfd,signalfd4,"
      "eventfd,eventfd2,timerfd_create,syncfs,renameat2,memfd_create,execveat");
  CHECK_NE(-1, pipe(pipefds));
  AppendArg("-o");
  AppendArg(xasprintf("/dev/fd/%d", pipefds[1]));
  for (i = optind; i < argc; ++i) {
    AppendArg(argv[i]);
  }
  AppendArg(NULL);

  /*
   * spawn strace
   */
  ignore.sa_flags = 0;
  ignore.sa_handler = SIG_IGN;
  sigemptyset(&ignore.sa_mask);
  sigaction(SIGINT, &ignore, &saveint);
  sigaction(SIGQUIT, &ignore, &savequit);
  sigfillset(&block);
  sigprocmask(SIG_BLOCK, &block, &mask);
  CHECK_NE(-1, (pid = vfork()));
  if (!pid) {
    close(pipefds[0]);
    sigaction(SIGINT, &saveint, NULL);
    sigaction(SIGQUIT, &savequit, NULL);
    sigprocmask(SIG_SETMASK, &mask, NULL);
    execv(strace, strace_args);
    _exit(127);
  }
  close(pipefds[1]);
  sigaddset(&mask, SIGCHLD);
  sigprocmask(SIG_SETMASK, &mask, NULL);

  /*
   * read output of strace until eof
   */
  fin = fdopen(pipefds[0], "r");
  t = NewTrace();
  for (ev = 0, lineno = 1; !interrupted && (line = xgetline(fin)); ++lineno) {
    _chomp(line);
    Parse(t, line, lineno);
    free(line);
    for (; ev < t->events.n && !t->events.p[ev].is_interrupted; ++ev) {
      PrintEvent(fout, t, ev);
      fprintf(fout, "\n");
    }
  }
  FreeTrace(t);
  CHECK_NE(-1, fclose(fout));

  /*
   * wait for strace to exit
   */
  while (waitpid(pid, &ws, 0) == -1) {
    CHECK_EQ(EINTR, errno);
  }
  CHECK_NE(-1, fclose(fin));

  /*
   * propagate exit
   */
  if (WIFEXITED(ws)) {
    return WEXITSTATUS(ws);
  } else {
    return 128 + WTERMSIG(ws);
  }
}
