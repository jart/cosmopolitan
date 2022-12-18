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
#define ShouldUseMsabiAttribute() 1
#include "libc/calls/calls.h"
#include "libc/calls/state.internal.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/fmt/divmod10.internal.h"
#include "libc/fmt/fmt.h"
#include "libc/intrin/asan.internal.h"
#include "libc/intrin/asancodes.h"
#include "libc/intrin/atomic.h"
#include "libc/intrin/bits.h"
#include "libc/intrin/cmpxchg.h"
#include "libc/intrin/kprintf.h"
#include "libc/intrin/likely.h"
#include "libc/intrin/nomultics.internal.h"
#include "libc/intrin/safemacros.internal.h"
#include "libc/intrin/weaken.h"
#include "libc/limits.h"
#include "libc/log/internal.h"
#include "libc/macros.internal.h"
#include "libc/nexgen32e/rdtsc.h"
#include "libc/nexgen32e/uart.internal.h"
#include "libc/nt/process.h"
#include "libc/nt/runtime.h"
#include "libc/nt/thunk/msabi.h"
#include "libc/nt/winsock.h"
#include "libc/runtime/brk.internal.h"
#include "libc/runtime/internal.h"
#include "libc/runtime/memtrack.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/symbols.internal.h"
#include "libc/str/str.h"
#include "libc/str/tab.internal.h"
#include "libc/str/utf16.h"
#include "libc/sysv/consts/nr.h"
#include "libc/sysv/consts/prot.h"
#include "libc/thread/tls.h"
#include "libc/thread/tls2.h"
#include "libc/vga/vga.internal.h"

extern _Hide struct SymbolTable *__symtab;

privileged static inline char *kadvance(char *p, char *e, long n) {
  intptr_t t = (intptr_t)p;
  if (__builtin_add_overflow(t, n, &t)) t = (intptr_t)e;
  return (char *)t;
}

privileged static char *kemitquote(char *p, char *e, signed char t,
                                   unsigned c) {
  if (t) {
    if (p < e) {
      *p = t < 0 ? 'u' : 'L';
    }
    ++p;
  }
  if (p < e) {
    *p = c;
  }
  ++p;
  return p;
}

privileged static unsigned long long kgetint(va_list va, signed char t,
                                             bool s) {
  int bits;
  unsigned long long x;
  x = va_arg(va, unsigned long);
  if (t <= 0) {
    bits = 64 - (32 >> MIN(5, -t));
    x <<= bits;
    if (s) {
      x = (signed long)x >> bits;
    } else {
      x >>= bits;
    }
  }
  return x;
}

privileged static inline bool kiskernelpointer(const void *p) {
  return 0x7f0000000000 <= (intptr_t)p && (intptr_t)p < 0x800000000000;
}

privileged static inline bool kistextpointer(const void *p) {
  return _base <= (const unsigned char *)p && (const unsigned char *)p < _etext;
}

privileged static inline unsigned char *kend(void) {
  unsigned char *p;
  if (_weaken(__brk) && (p = _weaken(__brk)->p)) {
    return p;
  } else {
    return _end;
  }
}

privileged static inline bool kisimagepointer(const void *p) {
  return _base <= (const unsigned char *)p && (const unsigned char *)p < kend();
}

privileged static inline bool kischarmisaligned(const char *p, signed char t) {
  if (t == -1) return (intptr_t)p & 1;
  if (t >= 1) return !!((intptr_t)p & 3);
  return false;
}

privileged static inline bool kismemtrackhosed(void) {
  return !((_weaken(_mmi)->i <= _weaken(_mmi)->n) &&
           (_weaken(_mmi)->p == _weaken(_mmi)->s ||
            _weaken(_mmi)->p == (struct MemoryInterval *)kMemtrackStart));
}

privileged static bool kismapped(int x) {
  // xxx: we can't lock because no reentrant locks yet
  size_t m, r, l = 0;
  if (!_weaken(_mmi)) return true;
  if (kismemtrackhosed()) return false;
  r = _weaken(_mmi)->i;
  while (l < r) {
    m = (l + r) >> 1;
    if (_weaken(_mmi)->p[m].y < x) {
      l = m + 1;
    } else {
      r = m;
    }
  }
  if (l < _weaken(_mmi)->i && x >= _weaken(_mmi)->p[l].x) {
    return !!(_weaken(_mmi)->p[l].prot & PROT_READ);
  } else {
    return false;
  }
}

privileged bool kisdangerous(const void *p) {
  int frame;
  if (kisimagepointer(p)) return false;
  if (kiskernelpointer(p)) return false;
  if (IsLegalPointer(p)) {
    frame = (intptr_t)p >> 16;
    if (IsStackFrame(frame)) return false;
    if (IsOldStackFrame(frame)) return false;
    if (kismapped(frame)) return false;
  }
  return true;
}

privileged static void klog(const char *b, size_t n) {
  int e;
  bool cf;
  size_t i;
  uint16_t dx;
  uint32_t wrote;
  unsigned char al;
  long rax, rdi, rsi, rdx;
  if (IsWindows()) {
    e = __imp_GetLastError();
    __imp_WriteFile(__imp_GetStdHandle(kNtStdErrorHandle), b, n, &wrote, 0);
    __imp_SetLastError(e);
  } else if (IsMetal()) {
    if (_weaken(_klog_vga)) _weaken(_klog_vga)(b, n);
    for (i = 0; i < n; ++i) {
      for (;;) {
        dx = 0x3F8 + UART_LSR;
        asm("inb\t%1,%0" : "=a"(al) : "dN"(dx));
        if (al & UART_TTYTXR) break;
        asm("pause");
      }
      dx = 0x3F8;
      asm volatile("outb\t%0,%1"
                   : /* no inputs */
                   : "a"(b[i]), "dN"(dx));
    }
  } else {
    asm volatile("syscall"
                 : "=a"(rax), "=D"(rdi), "=S"(rsi), "=d"(rdx)
                 : "0"(__NR_write), "1"(2), "2"(b), "3"(n)
                 : "rcx", "r8", "r9", "r10", "r11", "memory", "cc");
  }
}

privileged static size_t kformat(char *b, size_t n, const char *fmt,
                                 va_list va) {
  int si, y;
  wint_t t, u;
  const char *abet;
  signed char type;
  const char *s, *f;
  struct CosmoTib *tib;
  unsigned long long x;
  unsigned i, j, m, rem, sign, hash, cols, prec;
  char c, *p, *e, pdot, zero, flip, dang, base, quot, uppr, ansi, z[128];
  if (kistextpointer(b) || kisdangerous(b)) n = 0;
  if (!kistextpointer(fmt)) fmt = "!!WONTFMT";
  p = b;
  f = fmt;
  e = p + n;  // assume if n was negative e < p will be the case
  for (;;) {
    for (;;) {
      if (!(c = *f++) || c == '%') break;
    EmitFormatByte:
      if (p < e) *p = c;
      ++p;
    }
    if (!c) break;
    pdot = 0;
    flip = 0;
    dang = 0;
    hash = 0;
    sign = 0;
    prec = 0;
    quot = 0;
    type = 0;
    cols = 0;
    zero = 0;
    uppr = 0;
    abet = "0123456789abcdef";
    for (;;) {
      switch ((c = *f++)) {
        default:
          goto EmitFormatByte;

        case '\0':
          break;

        case '.':
          pdot = 1;
          continue;

        case '-':
          flip = 1;
          continue;

        case '#':
        case '`':
          hash = '0';
          continue;

        case '_':
        case ',':
        case '\'':
          quot = c;
          continue;

        case ' ':
        case '+':
          sign = c;
          continue;

        case '^':
          uppr = c;
          continue;

        case 'h':
          --type;
          continue;

        case 'j':
        case 'l':
        case 'z':
          ++type;
          continue;

        case '!':
          dang = 1;
          continue;

        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
          si = pdot ? prec : cols;
          si *= 10;
          si += c - '0';
          goto UpdateCols;

        case '*':
          si = va_arg(va, int);
        UpdateCols:
          if (pdot) {
            if (si >= 0) {
              prec = si;
            }
          } else {
            if (si < 0) {
              flip = 1;
              si = -si;
            }
            cols = si;
            if (!cols) {
              zero = 1;
            }
          }
          continue;

        case 'T':
          x = (rdtsc() - kStartTsc) / 3 % 86400000000000;
          goto FormatUnsigned;

        case 'P':
          tib = __tls_enabled ? __get_tls_privileged() : 0;
          if (!(tib && (tib->tib_flags & TIB_FLAG_VFORKED))) {
            if (tib) {
              x = atomic_load_explicit(&tib->tib_tid, memory_order_relaxed);
            } else {
              x = sys_gettid();
            }
            if (!__nocolor && p + 7 <= e) {
              *p++ = '\e';
              *p++ = '[';
              *p++ = '1';
              *p++ = ';';
              *p++ = '3';
              *p++ = '0' + x % 8;
              *p++ = 'm';
              ansi = true;
            }
          } else {
            x = 666;
          }
          goto FormatDecimal;

        case 'u':
        case 'd':
          if (UNLIKELY(type <= -3)) {
            s = va_arg(va, int) ? "true" : "false";
            goto FormatString;
          }
          x = kgetint(va, type, c == 'd');
        FormatDecimal:
          if ((long long)x < 0 && c != 'u') {
            x = -x;
            sign = '-';
          }
        FormatUnsigned:
          if (x && hash) sign = hash;
          for (i = j = 0;;) {
            x = DivMod10(x, &rem);
            z[i++ & 127] = '0' + rem;
            if (pdot ? i >= prec : !x) break;
            if (quot && ++j == 3) {
              z[i++ & 127] = quot;
              j = 0;
            }
          }
        EmitNumber:
          if (flip || pdot) zero = 0;
          while (zero && sign) {
            if (p < e) *p = sign;
            if (cols) --cols;
            sign >>= 8;
            ++p;
          }
          t = !!sign + !!(sign >> 8);
          if (!flip && cols >= t) {
            for (j = i; j < cols - t; ++j) {
              if (p < e) {
                *p++ = zero ? '0' : ' ';
              } else {
                p = kadvance(p, e, cols - t - j);
                break;
              }
            }
          }
          while (sign) {
            if (p < e) *p = sign;
            sign >>= 8;
            ++p;
          }
          for (j = i; j; ++p) {
            --j;
            if (p < e) {
              *p = z[j & 127];
            }
          }
          if (flip && cols >= t) {
            for (j = i; j < cols - t; ++j) {
              if (p < e) {
                *p++ = ' ';
              } else {
                p = kadvance(p, e, cols - t - j);
                break;
              }
            }
          }
          break;

        case 'b':
          base = 1;
          if (hash) hash = '0' | 'b' << 8;
        BinaryNumber:
          x = kgetint(va, type, false);
        FormatNumber:
          i = 0;
          m = (1 << base) - 1;
          if (hash && x) sign = hash;
          do z[i++ & 127] = abet[x & m];
          while ((x >>= base) || (pdot && i < prec));
          goto EmitNumber;

        case 'X':
          abet = "0123456789ABCDEF";
          /* fallthrough */
        case 'x':
          base = 4;
          if (hash) hash = '0' | 'x' << 8;
          goto BinaryNumber;

        case 'o':
          base = 3;
          goto BinaryNumber;

        case 'p':
          x = va_arg(va, intptr_t);
          if (!x && pdot) pdot = 0;
          if ((long)x == -1) {
            pdot = 0;
            goto FormatDecimal;
          }
          hash = '0' | 'x' << 8;
          base = 4;
          goto FormatNumber;

        case 'C':
          c = 'c';
          type = 1;
          // fallthrough
        case 'c':
          i = 1;
          j = 0;
          x = 0;
          s = (const char *)&x;
          t = va_arg(va, int);
          if (!type) t &= 255;
          if (hash) {
            quot = 1;
            hash = '\'';
            p = kemitquote(p, e, type, hash);
            if (cols && type) --cols;  // u/L
            if (cols) --cols;          // start quote
            if (cols) --cols;          // end quote
          }
          goto EmitChar;

        case 'm': {
          int unixerr;
          uint32_t winerr;
          unixerr = errno;
          winerr = 0;
          if (IsWindows()) {
            if (type == 1 && _weaken(WSAGetLastError)) {
              winerr = _weaken(WSAGetLastError)();
            } else if (_weaken(GetLastError)) {
              winerr = _weaken(GetLastError)();
            }
          }
          if (!unixerr && sign == ' ') {
            break;
          } else if (_weaken(strerror_wr) &&
                     !_weaken(strerror_wr)(unixerr, winerr, z, sizeof(z))) {
            s = z;
            type = 0;
            goto FormatString;
          } else {
            if (p + 7 <= e) {
              *p++ = ' ';
              *p++ = 'e';
              *p++ = 'r';
              *p++ = 'r';
              *p++ = 'n';
              *p++ = 'o';
              *p++ = '=';
            }
            type = 0;
            x = unixerr;
            goto FormatDecimal;
          }
        }

        case 'G':
          x = va_arg(va, int);
          if (_weaken(strsignal_r) && (s = _weaken(strsignal_r)(x, z))) {
            goto FormatString;
          } else {
            goto FormatDecimal;
          }

        case 't': {
          // %t will print the &symbol associated with an address. this
          // requires that some other code linked GetSymbolTable() and
          // called it beforehand to ensure the symbol table is loaded.
          // if the symbol table isn't linked or available, then this
          // routine will display &hexaddr so objdump -dS foo.com.dbg
          // can be manually consulted to look up the faulting code.
          int idx;
          x = va_arg(va, intptr_t);
          if (_weaken(__symtab) && *_weaken(__symtab) &&
              (idx = _weaken(__get_symbol)(0, x)) != -1) {
            if (p + 1 <= e) *p++ = '&';
            s = (*_weaken(__symtab))->name_base +
                (*_weaken(__symtab))->names[idx];
            goto FormatString;
          }
          base = 4;
          hash = '&';
          goto FormatNumber;
        }

        case 'n':
          // nonstandard %n specifier
          if (p < e) *p = '\n';
          ++p;
          break;

        case 'r':
          // undocumented %r specifier
          // used for good carriage return
          // helps integrate loggers with repls
          if (!__replstderr || __nocolor) {
            break;
          } else {
            s = "\r\e[K";
            goto FormatString;
          }

        case 's':
          if (!(s = va_arg(va, const void *))) {
            s = sign != ' ' ? "NULL" : "";
          FormatString:
            hash = 0;
            type = 0;
          } else if (!dang && (kisdangerous(s) || kischarmisaligned(s, type))) {
            if (sign == ' ') {
              if (p < e) *p = ' ';
              ++p;
            }
            x = (intptr_t)s;
            base = 4;
            hash = '!' | '!' << 8;
            goto FormatNumber;
          } else if (hash) {
            quot = 1;
            hash = '"';
            if (cols && type) --cols;  // u/L
            if (cols) --cols;          // start quote
            if (cols) --cols;          // end quote
            p = kemitquote(p, e, type, hash);
          }
          if (sign == ' ' && (!pdot || prec) && *s) {
            if (p < e) *p = ' ';
            ++p;
          }
          for (i = j = 0; !pdot || j < prec; ++j) {
            if (UNLIKELY(!((intptr_t)s & (PAGESIZE - 1)))) {
              if (!dang && kisdangerous(s)) break;
            }
            if (!type) {
              if (!(t = *s++ & 255)) break;
              if ((t & 0300) == 0200) goto ActuallyEmitByte;
              ++i;
            EmitByte:
              if (uppr && 'a' <= t && t <= 'z') {
                t -= 'a' - 'A';
              }
              if (UNLIKELY(quot) && (t == '\\' || ((t == '"' && c == 's') ||
                                                   (t == '\'' && c == 'c')))) {
                if (p + 2 <= e) {
                  p[0] = '\\';
                  p[1] = t;
                }
                p += 2;
                i += 1;
                continue;
              }
              if (pdot ||
                  (t != 0x7F && (t >= 0x20 || (t == '\n' || t == '\t' ||
                                               t == '\r' || t == '\e')))) {
              ActuallyEmitByte:
                if (p < e) *p = t;
                p += 1;
                continue;
              } else if (quot) {
                if (p + 4 <= e) {
                  p[0] = '\\';
                  p[1] = '0' + ((t & 0300) >> 6);
                  p[2] = '0' + ((t & 0070) >> 3);
                  p[3] = '0' + ((t & 0007) >> 0);
                }
                p += 4;
                i += 3;
                continue;
              } else {
                /* Control Pictures
                ═══════════════════════════════════════════════════════
                2400 │ 0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f
                ───────────────────────────────────────────────────────
                2400 │ ␀  ␁  ␂  ␃  ␄  ␅  ␆  ␇  ␈  ␉  ␊  ␋  ␌  ␍  ␎  ␏
                2410 │ ␐  ␑  ␒  ␓  ␔  ␕  ␖  ␗  ␘  ␙  ␚  ␛  ␜  ␝  ␞  ␟
                2420 │ ␠  ␡  ␢  ␣  ␤  ␥  ␦  */
                if (t != 0x7F) {
                  t += 0x2400;
                } else {
                  t = 0x2421;
                }
                goto EmitChar;
              }
            } else if (type < -1) {
              if ((t = *s++ & 255) || prec) {
                t = kCp437[t];
              }
            } else if (type < 0) {
              t = *(const char16_t *)s;
              s += sizeof(char16_t);
              if (IsHighSurrogate(t)) {
                if (!pdot || j + 1 < prec) {
                  if (UNLIKELY(!((intptr_t)s & (PAGESIZE - 1)))) {
                    if (!dang && kisdangerous(s)) break;
                  }
                  u = *(const char16_t *)s;
                  if (IsLowSurrogate(u)) {
                    t = MergeUtf16(t, u);
                    s += sizeof(char16_t);
                    j += 1;
                  }
                } else {
                  break;
                }
              } else if (!t) {
                break;
              }
            } else {
              t = *(const wchar_t *)s;
              s += sizeof(wchar_t);
            }
            if (!t) break;
            ++i;
          EmitChar:
            if (t <= 0x7f) goto EmitByte;
            if (uppr) {
              if (_weaken(towupper)) {
                t = _weaken(towupper)(t);
              } else if (uppr && 'a' <= t && t <= 'z') {
                t -= 'a' - 'A';
              }
            }
            if (t <= 0x7ff) {
              if (p + 2 <= e) {
                p[0] = 0300 | (t >> 6);
                p[1] = 0200 | (t & 077);
              }
              p += 2;
            } else if (t <= 0xffff) {
              if (UNLIKELY(IsSurrogate(t))) {
              EncodeReplacementCharacter:
                t = 0xfffd;
              }
              if (p + 3 <= e) {
                p[0] = 0340 | (t >> 12);
                p[1] = 0200 | ((t >> 6) & 077);
                p[2] = 0200 | (t & 077);
              }
              p += 3;
            } else if (~(t >> 18) & 007) {
              if (p + 4 <= e) {
                p[0] = 0360 | (t >> 18);
                p[1] = 0200 | ((t >> 12) & 077);
                p[2] = 0200 | ((t >> 6) & 077);
                p[3] = 0200 | (t & 077);
              }
              p += 4;
            } else {
              goto EncodeReplacementCharacter;
            }
          }
          if (hash) {
            if (p < e) *p = hash;
            ++p;
          }
          while (cols > i) {
            if (p + 8 < e && cols - i > 8) {
              WRITE64LE(p, 0x2020202020202020);
              cols -= 8;
              p += 8;
            } else if (p < e) {
              *p++ = ' ';
              --cols;
            } else {
              p = kadvance(p, e, cols - i);
              break;
            }
          }
          break;
      }
      if (ansi) {
        if (p + 4 <= e) {
          *p++ = '\e';
          *p++ = '[';
          *p++ = '0';
          *p++ = 'm';
        }
        ansi = false;
      }
      break;
    }
  }
  if (p < e) {
    *p = 0;
  } else if (e > b) {
    u = 0;
    *--e = 0;
    s = "\n...";
    if (!(((f - fmt) >= 2 && f[-2] == '\n') ||
          ((f - fmt) >= 3 && f[-3] == '%' && f[-2] == 'n'))) {
      ++s;
    }
    while ((t = *s++) && e > b) {
      u = *--e;
      *e = t;
    }
    if ((u & 0300) == 0200) {
      while (e > b) {
        u = *--e;
        *e = '.';
        if ((u & 0300) != 0200) {
          break;
        }
      }
    }
  }
  return p - b;
}

/**
 * Privileged snprintf().
 *
 * @param b is buffer, and guaranteed a NUL-terminator if `n>0`
 * @param n is number of bytes available in buffer
 * @return length of output excluding NUL, which may exceed `n`
 * @see kprintf() for documentation
 * @asyncsignalsafe
 * @vforksafe
 */
privileged size_t ksnprintf(char *b, size_t n, const char *fmt, ...) {
  size_t m;
  va_list v;
  va_start(v, fmt);
  m = kformat(b, n, fmt, v);
  va_end(v);
  return m;
}

/**
 * Privileged vsnprintf().
 *
 * @param b is buffer, and guaranteed a NUL-terminator if `n>0`
 * @param n is number of bytes available in buffer
 * @return length of output excluding NUL, which may exceed `n`
 * @see kprintf() for documentation
 * @asyncsignalsafe
 * @vforksafe
 */
privileged size_t kvsnprintf(char *b, size_t n, const char *fmt, va_list v) {
  return kformat(b, n, fmt, v);
}

/**
 * Privileged vprintf.
 *
 * @see kprintf() for documentation
 * @asyncsignalsafe
 * @vforksafe
 */
privileged void kvprintf(const char *fmt, va_list v) {
  size_t n;
  char b[4000];
  if (!v) return;
  n = kformat(b, sizeof(b), fmt, v);
  klog(b, MIN(n, sizeof(b) - 1));
}

/**
 * Privileged printf().
 *
 * This function is intended for crash reporting. It's designed to be as
 * unbreakable as possible, so that error messages can always be printed
 * even when the rest of the runtime is broken. As such, it has continue
 * on error semantics, doesn't support buffering between invocations and
 * floating point is not supported. Output is also truncated if the line
 * gets too long, but care is taken to preserve your newline characters.
 * Your errno and GetLastError() state will not be clobbered, and ftrace
 * and other runtime magic won't be invoked, since all the runtime magic
 * depends on this function.
 *
 * Directives:
 *
 *     %[FLAGS][WIDTH|*][.[PRECISION|*]][TYPE]SPECIFIER
 *
 * Specifiers:
 *
 * - `c` char
 * - `o` octal
 * - `b` binary
 * - `s` string
 * - `t` symbol
 * - `p` pointer
 * - `d` decimal
 * - `n` newline
 * - `u` unsigned
 * - `r` carriage
 * - `m` strerror
 * - `G` strsignal
 * - `X` uppercase
 * - `T` timestamp
 * - `x` hexadecimal
 * - `P` PID (or TID if TLS is enabled)
 *
 * Types:
 *
 * - `hhh` bool
 * - `hh`  char or cp437
 * - `h`   short or char16_t
 * - `l`   long or wchar_t
 * - `ll`  long long
 *
 * Flags:
 *
 * - `0` zero padding
 * - `-` flip alignment
 * - `!` bypass memory safety
 * - `,` thousands grouping w/ comma
 * - `'` thousands grouping w/ apostrophe
 * - `_` thousands grouping w/ underscore
 * - `+` plus leftpad if positive (aligns w/ negatives)
 * - ` ` space leftpad if positive (aligns w/ negatives)
 * - `#` represent value with literal syntax, e.g. 0x, 0b, quotes
 * - `^` uppercasing w/ towupper() if linked, otherwise toupper()
 *
 * Error numbers:
 *
 * - `%m` formats error (if strerror_wr if is linked)
 * - `%m` formats errno number (if strerror_wr isn't linked)
 * - `% m` formats error with leading space if errno isn't zero
 * - `%lm` means favor WSAGetLastError() over GetLastError() if linked
 *
 * You need to link and load the symbol table before `%t` will work. You
 * can do that by calling `GetSymbolTable()`. If that hasn't happened it
 * will print `&hexnumber` instead.
 *
 * @asyncsignalsafe
 * @vforksafe
 */
privileged void kprintf(const char *fmt, ...) {
  /* system call support runtime depends on this function */
  /* function tracing runtime depends on this function */
  /* asan runtime depends on this function */
  va_list v;
  va_start(v, fmt);
  kvprintf(fmt, v);
  va_end(v);
}
