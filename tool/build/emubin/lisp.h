/*───────────────────────────────────────────────────────────────────────────│─╗
│ The LISP Challenge § Hardware Integration w/ x86_64 Linux & 8086 PC BIOS ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

#define CompilerBarrier() asm volatile("" ::: "memory");

#define TYPE(x) /* a.k.a. x&1 */                              \
  ({                                                          \
    char IsAtom;                                              \
    asm("test%z1\t$1,%1" : "=@ccnz"(IsAtom) : "Qm"((char)x)); \
    IsAtom;                                                   \
  })

#define OBJECT(t, v) /* a.k.a. v<<1|t */ \
  ({                                     \
    __typeof(v) Val = (v);               \
    asm("shl\t%0" : "+r"(Val));          \
    Val | (t);                           \
  })

#define SUB(x, y) /* a.k.a. x-y */           \
  ({                                         \
    __typeof(x) Reg = (x);                   \
    asm("sub\t%1,%0" : "+rm"(Reg) : "g"(y)); \
    Reg;                                     \
  })

#define STOS(di, c) asm("stos%z1" : "+D"(di), "=m"(*(di)) : "a"(c))
#define LODS(si)                                     \
  ({                                                 \
    typeof(*(si)) c;                                 \
    asm("lods%z2" : "+S"(si), "=a"(c) : "m"(*(si))); \
    c;                                               \
  })

#define PEEK_(REG, BASE, INDEX, DISP)                                    \
  ({                                                                     \
    __typeof(*(BASE)) Reg;                                               \
    if (__builtin_constant_p(INDEX) && !(INDEX)) {                       \
      asm("mov\t%c2(%1),%0"                                              \
          : REG(Reg)                                                     \
          : "bDS"(BASE), "i"((DISP) * sizeof(*(BASE))),                  \
            "m"(BASE[(INDEX) + (DISP)]));                                \
    } else {                                                             \
      asm("mov\t%c3(%1,%2),%0"                                           \
          : REG(Reg)                                                     \
          : "b"(BASE), "DS"((long)(INDEX) * sizeof(*(BASE))),            \
            "i"((DISP) * sizeof(*(BASE))), "m"(BASE[(INDEX) + (DISP)])); \
    }                                                                    \
    Reg;                                                                 \
  })

#define PEEK(BASE, INDEX, DISP) /* a.k.a. b[i] */        \
  (sizeof(*(BASE)) == 1 ? PEEK_("=Q", BASE, INDEX, DISP) \
                        : PEEK_("=r", BASE, INDEX, DISP))

#define PEEK_ARRAY_(REG, OBJECT, MEMBER, INDEX, DISP)                     \
  ({                                                                      \
    __typeof(*(OBJECT->MEMBER)) Reg;                                      \
    if (!(OBJECT)) {                                                      \
      asm("mov\t%c2(%1),%0"                                               \
          : REG(Reg)                                                      \
          : "bDS"((long)(INDEX) * sizeof(*(OBJECT->MEMBER))),             \
            "i"(__builtin_offsetof(__typeof(*(OBJECT)), MEMBER) +         \
                sizeof(*(OBJECT->MEMBER)) * (DISP)),                      \
            "m"(OBJECT->MEMBER));                                         \
    } else {                                                              \
      asm("mov\t%c3(%1,%2),%0"                                            \
          : REG(Reg)                                                      \
          : "b"(OBJECT), "DS"((long)(INDEX) * sizeof(*(OBJECT->MEMBER))), \
            "i"(__builtin_offsetof(__typeof(*(OBJECT)), MEMBER) +         \
                sizeof(*(OBJECT->MEMBER)) * (DISP)),                      \
            "m"(OBJECT->MEMBER));                                         \
    }                                                                     \
    Reg;                                                                  \
  })

#define PEEK_ARRAY(OBJECT, MEMBER, INDEX, DISP) /* o->m[i] */ \
  (sizeof(*(OBJECT->MEMBER)) == 1                             \
       ? PEEK_ARRAY_("=Q", OBJECT, MEMBER, INDEX, DISP)       \
       : PEEK_ARRAY_("=r", OBJECT, MEMBER, INDEX, DISP))

#define POKE_ARRAY_(REG, OBJECT, MEMBER, INDEX, DISP, VALUE)        \
  do {                                                              \
    if (!(OBJECT)) {                                                \
      asm("mov\t%1,%c3(%2)"                                         \
          : "=m"(OBJECT->MEMBER)                                    \
          : REG((__typeof(*(OBJECT->MEMBER)))(VALUE)),              \
            "bDS"((long)(INDEX) * sizeof(*(OBJECT->MEMBER))),       \
            "i"(__builtin_offsetof(__typeof(*(OBJECT)), MEMBER) +   \
                sizeof(*(OBJECT->MEMBER)) * (DISP)));               \
    } else {                                                        \
      asm("mov\t%1,%c4(%2,%3)"                                      \
          : "=m"(OBJECT->MEMBER)                                    \
          : REG((__typeof(*(OBJECT->MEMBER)))(VALUE)), "b"(OBJECT), \
            "DS"((long)(INDEX) * sizeof(*(OBJECT->MEMBER))),        \
            "i"(__builtin_offsetof(__typeof(*(OBJECT)), MEMBER) +   \
                sizeof(*(OBJECT->MEMBER)) * (DISP)));               \
    }                                                               \
  } while (0)

#define POKE_ARRAY(OBJECT, MEMBER, INDEX, DISP, VALUE) /* o->m[i]=v */ \
  do {                                                                 \
    __typeof(*(OBJECT->MEMBER)) Reg;                                   \
    switch (sizeof(*(OBJECT->MEMBER))) {                               \
      case 1:                                                          \
        POKE_ARRAY_("Q", OBJECT, MEMBER, INDEX, DISP, VALUE);          \
        break;                                                         \
      default:                                                         \
        POKE_ARRAY_("r", OBJECT, MEMBER, INDEX, DISP, VALUE);          \
        break;                                                         \
    }                                                                  \
  } while (0)

int setjmp(void *) __attribute__((__returns_twice__));
int longjmp(void *, int) __attribute__((__noreturn__));

static inline void *SetMemory(void *di, int al, unsigned long cx) {
  asm("rep stosb"
      : "=D"(di), "=c"(cx), "=m"(*(char(*)[cx])di)
      : "0"(di), "1"(cx), "a"(al));
  return di;
}

static inline void *CopyMemory(void *di, void *si, unsigned long cx) {
  asm("rep movsb"
      : "=D"(di), "=S"(si), "=c"(cx), "=m"(*(char(*)[cx])di)
      : "0"(di), "1"(si), "2"(cx));
  return di;
}

static void RawMode(void) {
#ifndef __REAL_MODE__
  int rc;
  int c[14];
  asm volatile("syscall"
               : "=a"(rc)
               : "0"(0x10), "D"(0), "S"(0x5401), "d"(c)
               : "rcx", "r11", "memory");
  c[0] &= ~0b0000010111111000;  // INPCK|ISTRIP|PARMRK|INLCR|IGNCR|ICRNL|IXON
  c[2] &= ~0b0000000100110000;  // CSIZE|PARENB
  c[2] |= 0b00000000000110000;  // CS8
  c[3] &= ~0b1000000001011010;  // ECHONL|ECHO|ECHOE|IEXTEN|ICANON
  asm volatile("syscall"
               : "=a"(rc)
               : "0"(0x10), "D"(0), "S"(0x5402), "d"(c)
               : "rcx", "r11", "memory");
#endif
}

__attribute__((__noinline__)) static void PrintChar(long c) {
#ifdef __REAL_MODE__
  asm volatile("mov\t$0x0E,%%ah\n\t"
               "int\t$0x10"
               : /* no outputs */
               : "a"(c), "b"(7)
               : "memory");
#else
  static short buf;
  int rc;
  buf = c;
  asm volatile("syscall"
               : "=a"(rc)
               : "0"(1), "D"(1), "S"(&buf), "d"(1)
               : "rcx", "r11", "memory");
#endif
}

static int ReadChar(void) {
  int c;
#ifdef __REAL_MODE__
  asm volatile("int\t$0x16" : "=a"(c) : "0"(0) : "memory");
#else
  static int buf;
  asm volatile("syscall"
               : "=a"(c)
               : "0"(0), "D"(0), "S"(&buf), "d"(1)
               : "rcx", "r11", "memory");
  c = buf;
#endif
  return c;
}
