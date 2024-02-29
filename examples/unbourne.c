/*bin/echo ' #-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;coding:utf-8 -*-┤
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
├──────────────────────────────────────────────────────────────────────────────┤
│███▒ ▓░░░▒ █▓█▓ ▒▒███████▓█████ ██▓▓▓███▒██▒▓█▓████████ ▓██▓█████▓██ ░ ░▒  ░  │
│█░ ░ █░▒▒▒ █▓▓▓ ▒▓████▓░███▓█▓▓▓▓█▓▒▓▓███▓▒▒██▓▓█▓█████ ▒██▓█████▓██ ░ ▒░░░  ░│
│███▓     ▒ ▓██▓ █████▓░█▓▓█▓██░░█░████▓█▓██▒██▒▓▓▓█▓▓█▓█▒██▓█████▓██░▒ ▓░     │
│████ █▓     ▒███▓██████████████████████████████▓▓█▓█████▓████████▓██ ▒▓▓▓▒░▒ ░│
│███▓ ▓▓▓▓        ▒███████████▓████████████▓▓██▓▓▓█▓████▓███▓█████▓██ ▒░▓▒░░  ░│
│███▓ █▒▓▒▒░▓        ▒██▓████████████████████████████▓██▓▓████████▓██ ░░▒▒▓▓▒ ░│
│███▓ ▓▒▓▓▓ █▓▓          ▒███████████████████████████▓██▓▓░███████▓██ ░ ▒▒░░▓▓░│
│█▓█▓ █▒░░░ █▓██▓             ██████████████████████████▓█████████▓▓░░░ ▒░ ░░ ▒│
│█▓█▓░█▓▒▓▒ ██▓█▓ ▓               ▓██████████████████▒██▓████████ ▒░▒▒▒▒▒▒▒   ░│
│█▓▓▒░▓▓▒▓▓ █▓▓██ ██  ░ ░ ░░░░  ░░░ ░ ███████████████▓██▓██████ ░▒▓▓▒▓▓░▒▓░▒░  │
│██▓▓░▓▒▒▓▒░▓▓▓▓▓ █▓▓░   ░  ░░ ░ ░     ▓█████████████▒██▓████▒▒▒░░▓▓▒▓▒░▒░░▒░ ░│
│█▓▓▓░█▒▒▒▒ █▓▓▓▓ ██▓▓    ░      ░     ▒▒  ██████████▒██▓███░░░▒▒▓▓▓▓▒░░   ░   │
│██▓▓░█░▒▒▒░█▓▓▓▓ █▓▓█        ░ ░  ░░░░▓░    ████████▒██ ░░░▓▒▒▓▓▓▓ ▓▒░▒▒▒▒░░  │
│▒██▓▒▓░▒▓░░▓▓▓▓▓ ▓▓▓▓     ░░       ▒░░▒░░   ▓███████▒██░   ▒ ▒░░▒▓▒▒░▒▒▓▒ ░   │
│▒▓▓▓▒█▒▒▒▒░█▒▒▓▓ ▓▓█▓▓▓▓▓  ░▒ ░  ▒ ░▓▓▓░░░   █▒  ▓   ██▓███▒▓░▓▓▒░▓░░░░▒▒░░░ ░│
│▒█▒▓░▓▓░▓▓░▓▒▓▒▒ █▓████▓███  ░ ░▒░ ▓▓▒▒▒▒░░▓  ░ ░█ ▓ ▒█████░░  ▓▓▒▒░░▒▒▓▒░  ░░│
│░▓█▓▒▓▓▒▓▒░█▓▒▓▓ █▓████▓███▓▓ ░▒▒▒▓▓▒▒▓▓▒░▒▓░ ▒ ░██  ░ ███▒░▓░ ▓▒▓▒▒░ ░       │
│░█▓▓▓▓▓▓▒▒▓█▓▒▓▓ █▓▓███▓█████▓ ░▒▒▒▒▒▓▒▒░░▒░▓ ░░▓▒ ▒░▒░ ░▒ ▓░▓▓░░ ▓▓░ ░░░░ ░░ │
│ ██░  ░   ▒█▓▓▓▓ ██████▓██████▓░▒▒▒▓▓▓▓▓▓▒▒░▓▒░▓▒▒▒▒▓▓▓░▓▒▒░▒▒▓▓▒▒░▒  ░   ░ ░░│
│   ░░░▒     ▓▓▓▓ █▓▓█▓█▓███████▓░▒▓▒▓▓█▒░▒▓▓▓▒▒▓▓▒▓▓▒█░▒▓▒▒ ░▓▒▓ ▒▒░▒░ ▒ ░░░  │
│  ▒░░ ░ ▓  ░▓▓▓▓ ████▓█▓████████▒▓▒█░░▓▒▓ ▒▓▓░▒▒▓▒▒▓▒▓▒▒▓▒▓▓█▓▓ ▓░▒  ░░░░░ ░  │
│  ░▓ ▒░░░░ ▓█▓▓▓ █▓▒███▓█████████▓░░▒▒▓▒▓▒▒▓▓▒▓▓ ▓▒▓▓▒▓▒▓▒▓▓▒▓▓ ▓▒▒░▒ ▓▒ ░    │
│  ▒░ ░  ░░ ▓▒░░░ █▓▓██▓▓████▓▓▓█▓░░▒▓▓▓▒▒▓░▓▓▓▒▓▒▒▒▓▒▓▓▓▓▒▒▒▒▒░▒▒▒░██         │
│▒ ▒▒ ▒░░░  ▓▓▓▓▒ ▓▓▓███▒███▓███▓▓▒▒░░▒▒▒▒░▒▒▒▒▒▓▓▒▒░▓▓▓▒▓░▓▓▓▒▒▒▓░▓▓▓    ░ ░  │
│▒  ░    ▓  ▓▓▓▓▒ ▓▒▓▓▓█▒███▓▓▓█▒▒▓▓███████▓▒▓░▒▓░▓▒▓▒▓▓▒▒▒▓░▒▒░ ░▒▓▒░         │
│▓  ░░ ░ █  ▓▓▓▓▓ ▓▓▓█▓▓▓███▓█▓█▓████████████░▓▓▓▓▒▓▓▓▓▒▒▓▒▒▒ ▓▓ ▓░ ░▓     ░   │
│▓  ░  ░   ▒▓▓▓▒▒ ▓▓▓█▓█▒██▓▓▓█▓█▓█▓██████████░▓▒▒▓▓▓▒▓▓▓▒ ▒▓ ▒    ░▒▓▓▓█  ░   │
│▓▒░░▒▒░▒ ░░▓▓▓▓▓ ▓▓▓███▓██████████████████████░░▓▒░▓▒▒▒▒▒▒▓▓░       ░▒▒░░     │
│▓   ░▓▒▒   ░░▒▓▒ ░▓▓█▓█▓█████████████████████▒░░ ░ ░ ░▒░   ░ ░   ░▓▒          │
└──────────────────────────────────────────────────────────────────────────────┘
 unbourne is a gnu/systemd init process                          »cosmopolitan»


╔────────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § the unbourne shell                                        ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│─╝

  The UNBOURNE SHELL is BASED off the Almquist Shell──colloquially
  known as the Debian Almquist Shell, a.k.a. dash──which perfected
  the work of the late Stephen Bourne whose shell set the standard
  for decades thanks to a spark of brilliance from Ken Thompson.

    git://git.kernel.org/pub/scm/utils/dash/dash.git
    057cd650a4edd5856213d431a974ff35c6594489
    Fri Sep 03 15:00:58 2021 +0800

  The UNBOURNE SHELL pays HOMAGE to the Stewards of House California:

    Almquist Shell
    Derived from software contributed to Berkeley by Kenneth Almquist.

    Copyright 1991,1993 The Regents of the University of California
    Copyright 1997-2021 Herbert Xu
    Copyright 1997 Christos Zoulas

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions
    are met:

    1. Redistributions of source code must retain the above copyright
       notice, this list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright
       notice, this list of conditions and the following disclaimer in
       the documentation and/or other materials provided with the
       distribution.
    3. Neither the name of the University nor the names of its
       contributors may be used to endorse or promote products derived
       from this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS "AS IS"
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
    TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
    PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE REGENTS OR
    CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
    SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
    LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
    USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
    ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
    OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
    OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
    SUCH DAMAGE.


╔──────────────────────────────────────────────────────────────────────┬───┬───╗
│ cosmopolitan § the unbourne shell » build                             /   /  │
╚────────────────────────────────────────────────────────────────────'>/dev/null

  cc -Os -o unbourne unbourne.c
  exit

╔────────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § the unbourne shell » macros                               ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

#include "libc/assert.h"
#include "libc/calls/calls.h"
#include "libc/calls/struct/dirent.h"
#include "libc/calls/struct/rlimit.h"
#include "libc/calls/struct/rusage.h"
#include "libc/calls/struct/sigaction.h"
#include "libc/calls/struct/stat.h"
#include "libc/calls/struct/tms.h"
#include "libc/calls/termios.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/fmt/conv.h"
#include "libc/intrin/safemacros.internal.h"
#include "libc/limits.h"
#include "libc/log/log.h"
#include "libc/macros.internal.h"
#include "libc/mem/alg.h"
#include "libc/mem/alloca.h"
#include "libc/mem/gc.h"
#include "libc/mem/mem.h"
#include "libc/paths.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/sysconf.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/at.h"
#include "libc/sysv/consts/dt.h"
#include "libc/sysv/consts/f.h"
#include "libc/sysv/consts/fd.h"
#include "libc/sysv/consts/fileno.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/ok.h"
#include "libc/sysv/consts/rlim.h"
#include "libc/sysv/consts/s.h"
#include "libc/sysv/consts/sig.h"
#include "libc/sysv/consts/w.h"
#include "third_party/gdtoa/gdtoa.h"
#include "third_party/linenoise/linenoise.h"
#include "third_party/musl/passwd.h"

#define likely(expr)   __builtin_expect(!!(expr), 1)
#define unlikely(expr) __builtin_expect(!!(expr), 0)

#undef CEOF
#undef rflag

/*
 * The follow should be set to reflect the type of system you have:
 *  JOBS -> 1 if you have Berkeley job control, 0 otherwise.
 *  SHORTNAMES -> 1 if your linker cannot handle long names.
 *  define BSD if you are running 4.2 BSD or later.
 *  define SYSV if you are running under System V.
 *  define DEBUG=1 to compile in debugging ('set -o debug' to turn on)
 *  define DEBUG=2 to compile in and turn on debugging.
 *  define DO_SHAREDVFORK to indicate that vfork(2) shares its address
 *         with its parent.
 *
 * When debugging is on, debugging info will be written to ./trace and
 * a quit signal will generate a core dump.
 */

#define ALIASDEAD      2
#define ALIASINUSE     1
#define ARITH_MAX_PREC 8
#define ATABSIZE       39
#define CMDTABLESIZE   31
#define JOBS           1
#define NOPTS          17
#define OUTPUT_ERR     01
#define VTABSIZE       39

/* exceptions */
#define EXINT   0
#define EXERROR 1
#define EXEND   3
#define EXEXIT  4

/*
 * The input line number.  Input.c just defines this variable, and saves
 * and restores it when files are pushed and popped.  The user of this
 * package must set its value.
 */
#define plinno (parsefile->linno)

/* Syntax classes */
#define CWORD     0
#define CNL       1
#define CBACK     2
#define CSQUOTE   3
#define CDQUOTE   4
#define CENDQUOTE 5
#define CBQUOTE   6
#define CVAR      7
#define CENDVAR   8
#define CLP       9
#define CRP       10
#define CEOF      11
#define CCTL      12
#define CSPCL     13

/* Syntax classes for is_ functions */
#define ISDIGIT 01
#define ISUPPER 02
#define ISLOWER 04
#define ISUNDER 010
#define ISSPECL 020

#define SYNBASE 129
#define PEOF    -129

#define EOF_NLEFT -99

#define BASESYNTAX (basesyntax + SYNBASE)
#define DQSYNTAX   (dqsyntax + SYNBASE)
#define SQSYNTAX   (sqsyntax + SYNBASE)
#define ARISYNTAX  (arisyntax + SYNBASE)

#define ARITH_ASS       1
#define ARITH_OR        2
#define ARITH_AND       3
#define ARITH_BAD       4
#define ARITH_NUM       5
#define ARITH_VAR       6
#define ARITH_NOT       7
#define ARITH_BINOP_MIN 8
#define ARITH_LE        8
#define ARITH_GE        9
#define ARITH_LT        10
#define ARITH_GT        11
#define ARITH_EQ        12
#define ARITH_REM       13
#define ARITH_BAND      14
#define ARITH_LSHIFT    15
#define ARITH_RSHIFT    16
#define ARITH_MUL       17
#define ARITH_ADD       18
#define ARITH_BOR       19
#define ARITH_SUB       20
#define ARITH_BXOR      21
#define ARITH_DIV       22
#define ARITH_NE        23
#define ARITH_BINOP_MAX 24
#define ARITH_ASS_MIN   24
#define ARITH_REMASS    24
#define ARITH_BANDASS   25
#define ARITH_LSHIFTASS 26
#define ARITH_RSHIFTASS 27
#define ARITH_MULASS    28
#define ARITH_ADDASS    29
#define ARITH_BORASS    30
#define ARITH_SUBASS    31
#define ARITH_BXORASS   32
#define ARITH_DIVASS    33
#define ARITH_ASS_MAX   34
#define ARITH_LPAREN    34
#define ARITH_RPAREN    35
#define ARITH_BNOT      36
#define ARITH_QMARK     37
#define ARITH_COLON     38

/* expandarg() flags */
#define EXP_FULL      0x1
#define EXP_TILDE     0x2
#define EXP_VARTILDE  0x4
#define EXP_REDIR     0x8
#define EXP_CASE      0x10
#define EXP_VARTILDE2 0x40
#define EXP_WORD      0x80
#define EXP_QUOTED    0x100
#define EXP_KEEPNUL   0x200
#define EXP_DISCARD   0x400

/* reasons for skipping commands (see comment on breakcmd routine) */
#define SKIPBREAK   (1 << 0)
#define SKIPCONT    (1 << 1)
#define SKIPFUNC    (1 << 2)
#define SKIPFUNCDEF (1 << 3)

#define TEOF       0
#define TNL        1
#define TSEMI      2
#define TBACKGND   3
#define TAND       4
#define TOR        5
#define TPIPE      6
#define TLP        7
#define TRP        8
#define TENDCASE   9
#define TENDBQUOTE 10
#define TREDIR     11
#define TWORD      12
#define TNOT       13
#define TCASE      14
#define TDO        15
#define TDONE      16
#define TELIF      17
#define TELSE      18
#define TESAC      19
#define TFI        20
#define TFOR       21
#define TIF        22
#define TIN        23
#define TTHEN      24
#define TUNTIL     25
#define TWHILE     26
#define TBEGIN     27
#define TEND       28

/* control characters in argument strings */
#define CTL_FIRST    -127
#define CTLESC       -127
#define CTLVAR       -126
#define CTLENDVAR    -125
#define CTLBACKQ     -124
#define CTLARI       -122
#define CTLENDARI    -121
#define CTLQUOTEMARK -120
#define CTL_LAST     -120

/* variable substitution byte (follows CTLVAR) */
#define VSTYPE 0x0f
#define VSNUL  0x10

/* values of VSTYPE field */
#define VSNORMAL       0x1
#define VSMINUS        0x2
#define VSPLUS         0x3
#define VSQUESTION     0x4
#define VSASSIGN       0x5
#define VSTRIMRIGHT    0x6
#define VSTRIMRIGHTMAX 0x7
#define VSTRIMLEFT     0x8
#define VSTRIMLEFTMAX  0x9
#define VSLENGTH       0xa
/* VSLENGTH must come last. */

/* values of checkkwd variable */
#define CHKALIAS   0x1
#define CHKKWD     0x2
#define CHKNL      0x4
#define CHKEOFMARK 0x8

/* flags in argument to evaltree */
#define EV_EXIT   01
#define EV_TESTED 02

#define INT_CHARS (sizeof(int) * CHAR_BIT / 3)

/*
 * These macros allow the user to suspend the handling of interrupt
 * signals over a period of time. This is similar to SIGHOLD to or
 * sigblock, but much more efficient and portable. (But hacking the
 * kernel is so much more fun than worrying about efficiency and
 * portability. :-))
 */
#define barrier() ({ asm volatile("" : : : "memory"); })
#define INTOFF     \
  ({               \
    suppressint++; \
    barrier();     \
    0;             \
  })
#define INTON                                      \
  ({                                               \
    barrier();                                     \
    if (--suppressint == 0 && intpending) onint(); \
    0;                                             \
  })
#define FORCEINTON           \
  ({                         \
    barrier();               \
    suppressint = 0;         \
    if (intpending) onint(); \
    0;                       \
  })
#define SAVEINT(v) ((v) = suppressint)
#define RESTOREINT(v)                                    \
  ({                                                     \
    barrier();                                           \
    if ((suppressint = (v)) == 0 && intpending) onint(); \
    0;                                                   \
  })
#define CLEAR_PENDING_INT intpending = 0
#define int_pending()     intpending

/*
 * Most machines require the value returned from malloc to be aligned
 * in some way.  The following macro will get this right on many machines.
 */
#define SHELL_SIZE \
  (sizeof(union {  \
     int i;        \
     char *cp;     \
     double d;     \
   }) -            \
   1)

/*
 * It appears that grabstackstr() will barf with such alignments
 * because stalloc() will return a string allocated in a new stackblock.
 */
#define SHELL_ALIGN(nbytes) (((nbytes) + SHELL_SIZE) & ~SHELL_SIZE)

/*
 * Minimum size of a block
 *
 * Parse trees for commands are allocated in lifo order, so we use a stack
 * to make this more efficient, and also to avoid all sorts of exception
 * handling code to handle interrupts in the middle of a parse.
 *
 * The size 504 was chosen because the Ultrix malloc handles that size
 * well.
 */
#define MINSIZE SHELL_ALIGN(504)

/* flags */
#define VEXPORT    0x001
#define VREADONLY  0x002
#define VSTRFIXED  0x004
#define VTEXTFIXED 0x008
#define VSTACK     0x010
#define VUNSET     0x020
#define VNOFUNC    0x040
#define VNOSET     0x080
#define VNOSAVE    0x100

/*
 * Evaluate a command.
 */
#define ALIASCMD   (kBuiltinCmds + 3)
#define BGCMD      (kBuiltinCmds + 4)
#define BREAKCMD   (kBuiltinCmds + 5)
#define CDCMD      (kBuiltinCmds + 6)
#define COMMANDCMD (kBuiltinCmds + 8)
#define DOTCMD     (kBuiltinCmds + 0)
#define ECHOCMD    (kBuiltinCmds + 10)
#define EVALCMD    (kBuiltinCmds + 11)
#define EXECCMD    (kBuiltinCmds + 12)
#define EXITCMD    (kBuiltinCmds + 13)
#define EXPORTCMD  (kBuiltinCmds + 14)
#define FALSECMD   (kBuiltinCmds + 15)
#define FGCMD      (kBuiltinCmds + 16)
#define GETOPTSCMD (kBuiltinCmds + 17)
#define HASHCMD    (kBuiltinCmds + 18)
#define JOBSCMD    (kBuiltinCmds + 19)
#define KILLCMD    (kBuiltinCmds + 20)
#define LOCALCMD   (kBuiltinCmds + 21)
#define PRINTFCMD  (kBuiltinCmds + 22)
#define PWDCMD     (kBuiltinCmds + 23)
#define READCMD    (kBuiltinCmds + 24)
#define RETURNCMD  (kBuiltinCmds + 26)
#define SETCMD     (kBuiltinCmds + 27)
#define SHIFTCMD   (kBuiltinCmds + 28)
#define TESTCMD    (kBuiltinCmds + 2)
#define TIMESCMD   (kBuiltinCmds + 30)
#define TRAPCMD    (kBuiltinCmds + 31)
#define TRUECMD    (kBuiltinCmds + 1)
#define TYPECMD    (kBuiltinCmds + 33)
#define ULIMITCMD  (kBuiltinCmds + 34)
#define UMASKCMD   (kBuiltinCmds + 35)
#define UNALIASCMD (kBuiltinCmds + 36)
#define UNSETCMD   (kBuiltinCmds + 37)
#define WAITCMD    (kBuiltinCmds + 38)

#define BUILTIN_SPECIAL 0x1
#define BUILTIN_REGULAR 0x2
#define BUILTIN_ASSIGN  0x4

/* mode flags for set_curjob */
#define CUR_DELETE  2
#define CUR_RUNNING 1
#define CUR_STOPPED 0

/* mode flags for dowait */
#define DOWAIT_NONBLOCK    0
#define DOWAIT_BLOCK       1
#define DOWAIT_WAITCMD     2
#define DOWAIT_WAITCMD_ALL 4

/* _rmescape() flags */
#define RMESCAPE_ALLOC 0x01
#define RMESCAPE_GLOB  0x02
#define RMESCAPE_GROW  0x08
#define RMESCAPE_HEAP  0x10

/* Add CTLESC when necessary. */
#define QUOTES_ESC (EXP_FULL | EXP_CASE)

#define IBUFSIZ   (BUFSIZ + 1)
#define OUTBUFSIZ BUFSIZ
#define MEM_OUT   -3

/*
 * Sigmode records the current value of the signal handlers for the
 * various modes. A value of zero means that the current handler is not
 * known. S_HARD_IGN indicates that the signal was ignored on entry to
 * the shell,
 */
#define S_DFL      1
#define S_CATCH    2
#define S_IGN      3
#define S_HARD_IGN 4
#define S_RESET    5

#define NCMD      0
#define NPIPE     1
#define NREDIR    2
#define NBACKGND  3
#define NSUBSHELL 4
#define NAND      5
#define NOR       6
#define NSEMI     7
#define NIF       8
#define NWHILE    9
#define NUNTIL    10
#define NFOR      11
#define NCASE     12
#define NCLIST    13
#define NDEFUN    14
#define NARG      15
#define NTO       16
#define NCLOBBER  17
#define NFROM     18
#define NFROMTO   19
#define NAPPEND   20
#define NTOFD     21
#define NFROMFD   22
#define NHERE     23
#define NXHERE    24
#define NNOT      25

/* Mode argument to forkshell.  Don't change FORK_FG or FORK_BG. */
#define FORK_FG    0
#define FORK_BG    1
#define FORK_NOJOB 2

/* mode flags for showjob(s) */
#define SHOW_PGID    0x01
#define SHOW_PID     0x04
#define SHOW_CHANGED 0x08

/* values of cmdtype */
#define CMDUNKNOWN  -1
#define CMDNORMAL   0
#define CMDFUNCTION 1
#define CMDBUILTIN  2

/* action to find_command() */
#define DO_ERR      0x01
#define DO_ABS      0x02
#define DO_NOFUNC   0x04
#define DO_ALTPATH  0x08
#define DO_REGBLTIN 0x10

/* flags passed to redirect */
#define REDIR_PUSH    01
#define REDIR_SAVEFD2 03

#define CD_PHYSICAL 1
#define CD_PRINT    2

#define EMPTY    -2
#define CLOSED   -1
#define PIPESIZE 4096

#define rootshell (!shlvl)

#define eflag optlist[0]
#define fflag optlist[1]
#define Iflag optlist[2]
#define iflag optlist[3]
#define mflag optlist[4]
#define nflag optlist[5]
#define sflag optlist[6]
#define xflag optlist[7]
#define vflag optlist[8]
#define Vflag optlist[9]
#define Eflag optlist[10]
#define Cflag optlist[11]
#define aflag optlist[12]
#define bflag optlist[13]
#define uflag optlist[14]
#define nolog optlist[15]
#define debug optlist[16]

/* Used by expandstr to get here-doc like behaviour. */
#define FAKEEOFMARK (char *)1

/*
 * This file is included by programs which are optionally built into the
 * shell.  If SHELL is defined, we try to map the standard UNIX library
 * routines to ash routines using defines.
 */
#define Printf out1fmt
#define INITARGS(argv)
#define setprogname(s)
#define getprogname() commandname

#define setlocate(l, s) 0
#define equal(s1, s2)   (!strcmp(s1, s2))
#define isodigit(c)     ((c) >= '0' && (c) <= '7')
#define octtobin(c)     ((c) - '0')
#define scopy(s1, s2)   ((void)strcpy(s2, s1))

#define TRACE(param)
/* #define TRACE(param)   \ */
/*   do {                 \ */
/*     printf("TRACE: "); \ */
/*     printf param;      \ */
/*   } while (0) */

#define TRACEV(param)
#define digit_val(c)  ((c) - '0')
#define is_alpha(c)   isalpha((unsigned char)(c))
#define is_digit(c)   ((unsigned)((c) - '0') <= 9)
#define is_in_name(c) ((c) == '_' || isalnum((unsigned char)(c)))
#define is_name(c)    ((c) == '_' || isalpha((unsigned char)(c)))
#define is_special(c) \
  ((is_type + SYNBASE)[(signed char)(c)] & (ISSPECL | ISDIGIT))

#define uninitialized_var(x)                       \
  x = x /* suppress uninitialized warning w/o code \
         */

/*
 * Shell variables.
 */
#define vifs    varinit[0]
#define vpath   (&vifs)[1]
#define vps1    (&vpath)[1]
#define vps2    (&vps1)[1]
#define vps4    (&vps2)[1]
#define voptind (&vps4)[1]
#define vlineno (&voptind)[1]
#define defifs  (defifsvar + 4)
#define defpath (defpathvar + 36)

/*
 * The following macros access the values of the above variables. They
 * have to skip over the name. They return the null string for unset
 * variables.
 */
#define ifsval()      (vifs.text + 4)
#define ifsset()      ((vifs.flags & VUNSET) == 0)
#define mailval()     (vmail.text + 5)
#define mpathval()    (vmpath.text + 9)
#define pathval()     (vpath.text + 5)
#define ps1val()      (vps1.text + 4)
#define ps2val()      (vps2.text + 4)
#define ps4val()      (vps4.text + 4)
#define optindval()   (voptind.text + 7)
#define linenoval()   (vlineno.text + 7)
#define mpathset()    ((vmpath.flags & VUNSET) == 0)
#define environment() listvars(VEXPORT, VUNSET, 0)

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § the unbourne shell » data structures                      ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

typedef void *pointer;

struct redirtab {
  struct redirtab *next;
  int renamed[10];
};

/*
 * We enclose jmp_buf in a structure so that we can declare pointers to
 * jump locations. The global variable handler contains the location to
 * jump to when an exception occurs, and the global variable exception
 * contains a code identifying the exception. To implement nested
 * exception handlers, the user should save the value of handler on
 * entry to an inner scope, set handler to point to a jmploc structure
 * for the inner scope, and restore handler on exit from the scope.
 */
struct jmploc {
  jmp_buf loc;
};

/* PEOF (the end of file marker) is defined in syntax.h */
enum {
  INPUT_PUSH_FILE = 1,
  INPUT_NOFILE_OK = 2,
};

struct alias {
  struct alias *next;
  char *name;
  char *val;
  int flag;
};

struct shparam {
  int nparam;           /* # of positional parameters (without $0) */
  unsigned char malloc; /* if parameter list dynamically allocated */
  char **p;             /* parameter list */
  int optind;           /* next parameter to be processed by getopts */
  int optoff;           /* used by getopts */
};

struct strpush {
  struct strpush *prev; /* preceding string on stack */
  char *prevstring;
  int prevnleft;
  struct alias *ap;       /* if push was associated with an alias */
  char *string;           /* remember the string since it may change */
  struct strpush *spfree; /* Delay freeing so we can stop nested aliases. */
  int lastc[2];           /* Remember last two characters for pungetc. */
  int unget;              /* Number of outstanding calls to pungetc. */
};

/*
 * The parsefile structure pointed to by the global variable parsefile
 * contains information about the current file being read.
 */
struct parsefile {
  struct parsefile *prev;     /* preceding file on stack */
  int linno;                  /* current line */
  int fd;                     /* file descriptor (or -1 if string) */
  int nleft;                  /* number of chars left in this line */
  int lleft;                  /* number of chars left in this buffer */
  char *nextc;                /* next char in buffer */
  char *buf;                  /* input buffer */
  struct strpush *strpush;    /* for pushing strings at this level */
  struct strpush basestrpush; /* so pushing one is fast */
  struct strpush *spfree;     /* Delay freeing so we can stop nested aliases. */
  int lastc[2];               /* Remember last two characters for pungetc. */
  int unget;                  /* Number of outstanding calls to pungetc. */
};

struct output {
  char *nextc;
  char *end;
  char *buf;
  unsigned bufsize;
  int fd;
  int flags;
};

struct heredoc {
  struct heredoc *next; /* next here document in list */
  union node *here;     /* redirection node */
  char *eofmark;        /* string indicating end of input */
  int striptabs;        /* if set, strip leading tabs */
};

struct synstack {
  const char *syntax;
  struct synstack *prev;
  struct synstack *next;
  int innerdq;
  int varpushed;
  int dblquote;
  int varnest;    /* levels of variables expansion */
  int parenlevel; /* levels of parens in arithmetic */
  int dqvarnest;  /* levels of variables expansion within double quotes */
};

struct procstat {
  int pid;    /* process id */
  int status; /* last process status from wait() */
  char *cmd;  /* text of command being run */
};

/*
 * A job structure contains information about a job.  A job is either a
 * single process or a set of processes contained in a pipeline.  In the
 * latter case, pidlist will be non-NULL, and will point to a -1 terminated
 * array of pids.
 */
struct job {
  struct procstat ps0;  /* status of process */
  struct procstat *ps;  /* status or processes when more than one */
  int stopstatus;       /* status of a stopped job */
  unsigned nprocs : 16, /* number of processes */
      state : 8,
#define JOBRUNNING 0
#define JOBSTOPPED 1
#define JOBDONE    2
      sigint : 1,       /* job was killed by SIGINT */
      jobctl : 1,       /* job running under job control */
      waited : 1,       /* true if this entry has been waited for */
      used : 1,         /* true if this entry is in used */
      changed : 1;      /* true if status has changed */
  struct job *prev_job; /* previous job */
};

struct ncmd {
  int type;
  int linno;
  union node *assign;
  union node *args;
  union node *redirect;
};

struct npipe {
  int type;
  int backgnd;
  struct nodelist *cmdlist;
};

struct nredir {
  int type;
  int linno;
  union node *n;
  union node *redirect;
};

struct nbinary {
  int type;
  union node *ch1;
  union node *ch2;
};

struct nif {
  int type;
  union node *test;
  union node *ifpart;
  union node *elsepart;
};

struct nfor {
  int type;
  int linno;
  union node *args;
  union node *body;
  char *var_;
};

struct ncase {
  int type;
  int linno;
  union node *expr;
  union node *cases;
};

struct nclist {
  int type;
  union node *next;
  union node *pattern;
  union node *body;
};

struct ndefun {
  int type;
  int linno;
  char *text;
  union node *body;
};

struct narg {
  int type;
  union node *next;
  char *text;
  struct nodelist *backquote;
};

struct nfile {
  int type;
  union node *next;
  int fd;
  union node *fname;
  char *expfname;
};

struct ndup {
  int type;
  union node *next;
  int fd;
  int dupfd;
  union node *vname;
};

struct nhere {
  int type;
  union node *next;
  int fd;
  union node *doc;
};

struct nnot {
  int type;
  union node *com;
};

union node {
  int type;
  struct ncmd ncmd;
  struct npipe npipe;
  struct nredir nredir;
  struct nbinary nbinary;
  struct nif nif;
  struct nfor nfor;
  struct ncase ncase;
  struct nclist nclist;
  struct ndefun ndefun;
  struct narg narg;
  struct nfile nfile;
  struct ndup ndup;
  struct nhere nhere;
  struct nnot nnot;
};

struct nodelist {
  struct nodelist *next;
  union node *n;
};

struct funcnode {
  int count;
  union node n;
};

struct localvar_list {
  struct localvar_list *next;
  struct localvar *lv;
};

struct Var {
  struct Var *next; /* next entry in hash list */
  int flags;        /* flags are defined above */
  const char *text; /* name=value */
  void (*func)(const char *);
  /* function to be called when  */
  /* the variable gets set/unset */
};

struct localvar {
  struct localvar *next; /* next local variable in list */
  struct Var *vp;        /* the variable that was made local */
  int flags;             /* saved flags */
  const char *text;      /* saved text */
};

union yystype {
  int64_t val;
  char *name;
};

struct strlist {
  struct strlist *next;
  char *text;
};

struct arglist {
  struct strlist *list;
  struct strlist **lastp;
};

/*
 * Structure specifying which parts of the string should be searched
 * for IFS characters.
 */
struct ifsregion {
  struct ifsregion *next; /* next region in list */
  int begoff;             /* offset of start of region */
  int endoff;             /* offset of end of region */
  int nulonly;            /* search for nul bytes only */
};

struct builtincmd {
  const char *name;
  int (*builtin)(int, char **);
  unsigned flags;
};

struct cmdentry {
  int cmdtype;
  union param {
    int index;
    const struct builtincmd *cmd;
    struct funcnode *func;
  } u;
};

struct tblentry {
  struct tblentry *next; /* next entry in hash chain */
  union param param;     /* definition of builtin function */
  short cmdtype;         /* index identifying command */
  char rehash;           /* if set, cd done since entry created */
  char cmdname[];        /* name of command */
};

struct backcmd {  /* result of evalbackcmd */
  int fd;         /* file descriptor to read from */
  char *buf;      /* buffer */
  int nleft;      /* number of chars in buffer */
  struct job *jp; /* job structure for command */
};

struct stack_block {
  struct stack_block *prev;
  char space[MINSIZE];
};

struct stackmark {
  struct stack_block *stackp;
  char *stacknxt;
  unsigned stacknleft;
};

struct limits {
  const char *name;
  int cmd;
  int factor; /* multiply by to get rlim_{cur,max} values */
  char option;
};

struct t_op {
  const char *op_text;
  short op_num, op_type;
};

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § the unbourne shell » bss                                  ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

static char **argptr; /* argument list for builtin commands */
static char **gargv;
static char **t_wp;
static char *arg0; /* value of $0 */
static char *cmdnextc;
static char *commandname;
static char *expdest; /* output of current string */
static char *expdir;
static char *funcstring;      /* block to allocate strings from */
static char *minusc;          /* argument to -c option */
static char *optionarg;       /* set by nextopt (like getopt) */
static char *optptr;          /* used by nextopt */
static char *trap[NSIG];      /* trap handler commands */
static char *wordtext;        /* text of last word returned by readtoken */
static char basebuf[IBUFSIZ]; /* buffer for top level input file */
static char gotsig[NSIG - 1]; /* indicates specified signal received */
static char nullstr[1];       /* zero length string */
static char optlist[NOPTS];
static char sigmode[NSIG - 1]; /* current value of signal */
static const char *arith_buf;
static const char *arith_startbuf;
static const char *pathopt;
static int back_exitstatus; /* exit status of backquoted command */
static int checkkwd;
static int doprompt; /* if set, prompt the user */
static int errlinno;
static int evalskip; /* set if we are skipping commands */
static int exception;
static int exitstatus;     /* exit status of last command */
static int funcblocksize;  /* size of structures in function */
static int funcline;       /* start line of function, or 0 if not in one */
static int funcstringsize; /* size of strings in node */
static int initialpgrp;    /* pgrp of shell on invocation */
static int inps4;          /* Prevent PS4 nesting. */
static int job_warning;
static int jobctl;
static int last_token;
static int lasttoken; /* last token read */
static int lineno;
static int loopnest;   /* current loop nesting level */
static int needprompt; /* true if interactive and at start of line */
static int quoteflag;  /* set if (part of) last token was quoted */
static int rootpid;
static int rval;
static int shlvl;
static int skipcount; /* number of levels to skip */
static int suppressint;
static int tokpushback;       /* last token pushed back */
static int trapcnt;           /* number of non-null traps */
static int ttyfd = -1;        /* control terminal */
static int vforked;           /* Set if we are in the vforked child */
static int whichprompt;       /* 1 == PS1, 2 == PS2 */
static int backgndpid;        /* pid of last background process */
static pointer funcblock;     /* block to allocate function from */
static struct arglist exparg; /* holds expanded arg list */
static struct heredoc *heredoc;
static struct heredoc *heredoclist; /* list of here documents to read */
static struct ifsregion *ifslastp;  /* last struct in list */
static struct ifsregion ifsfirst;   /* first struct in list of ifs regions */
static struct jmploc *handler;
static struct jmploc main_handler;
static struct job *curjob; /* current job */
static struct job *jobtab; /* array of jobs */
static struct localvar_list *localvar_stack;
static struct nodelist *argbackq; /* list of back quote expressions */
static struct nodelist *backquotelist;
static struct output preverrout;
static struct parsefile basepf; /* top level input file */
static struct redirtab *redirlist;
static struct shparam shellparam; /* current positional parameters */
static struct stack_block stackbase;
static struct t_op const *t_wp_op;
static struct tblentry **lastcmdentry;
static struct tblentry *cmdtable[CMDTABLESIZE];
static struct Var *vartab[VTABSIZE];
static union node *redirnode;
static union yystype yylval;
static unsigned
    closed_redirs; /* Bit map of currently closed file descriptors. */
static unsigned expdir_max;
static unsigned njobs;                   /* size of array */
static volatile sig_atomic_t gotsigchld; /* received SIGCHLD */
static volatile sig_atomic_t intpending;
static volatile sig_atomic_t pending_sig; /* last pending signal */
static struct alias *atab[ATABSIZE];

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § the unbourne shell » data                                 ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

static char *curdir = nullstr;  /* current working directory */
static char *physdir = nullstr; /* physical working directory */
static char *sstrend = stackbase.space + MINSIZE;
static char *stacknxt = stackbase.space;
static char defifsvar[] = "IFS= \t\n";
static char defoptindvar[] = "OPTIND=1";
static char linenovar[sizeof("LINENO=") + INT_CHARS + 1] = "LINENO=";
static int builtinloc = -1; /* index in path of %builtin, or -1 */
static int savestatus = -1; /* exit status of last command outside traps */
static struct output errout = {0, 0, 0, 0, 2, 0};
static struct output output = {0, 0, 0, OUTBUFSIZ, 1, 0};
static struct parsefile *parsefile = &basepf; /* current input file */
static struct stack_block *stackp = &stackbase;
static unsigned stacknleft = MINSIZE;

static struct output *out1 = &output;
static struct output *out2 = &errout;

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § the unbourne shell » rodata                               ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

/* Array indicating which tokens mark the end of a list */
static const char tokendlist[] = {
    1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0,
    1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 1,
};

static const char *const tokname[] = {
    "end of file", "newline",     "\";\"",    "\"&\"",    "\"&&\"",
    "\"||\"",      "\"|\"",       "\"(\"",    "\")\"",    "\";;\"",
    "\"`\"",       "redirection", "word",     "\"!\"",    "\"case\"",
    "\"do\"",      "\"done\"",    "\"elif\"", "\"else\"", "\"esac\"",
    "\"fi\"",      "\"for\"",     "\"if\"",   "\"in\"",   "\"then\"",
    "\"until\"",   "\"while\"",   "\"{\"",    "\"}\"",
};

static const char *const parsekwd[] = {
    "!",   "case", "do", "done", "elif",  "else",  "esac", "fi",
    "for", "if",   "in", "then", "until", "while", "{",    "}"};

static const char defpathvar[] =
    "PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin";

static const char *const optnames[NOPTS] = {
    "errexit",   "noglob", "ignoreeof", "interactive", "monitor", "noexec",
    "stdin",     "xtrace", "verbose",   "vi",          "emacs",   "noclobber",
    "allexport", "notify", "nounset",   "nolog",       "debug",
};

static const char optletters[NOPTS] = {
    'e', 'f', 'I', 'i', 'm', 'n', 's', 'x', 'v',
    'V', 'E', 'C', 'a', 'b', 'u', 0,   0,
};

static const char spcstr[] = " ";
static const char snlfmt[] = "%s\n";
static const char qchars[] = {CTLESC, CTLQUOTEMARK, 0};
static const char illnum[] = "Illegal number: %s";
static const char homestr[] = "HOME";
static const char dolatstr[] = {CTLQUOTEMARK, CTLVAR,       VSNORMAL, '@',
                                '=',          CTLQUOTEMARK, '\0'};

/* TODO(jart): What's wrong with varinit? */
#if defined(__GNUC__) || defined(__llvm__)
#pragma GCC diagnostic ignored "-Warray-bounds"
#endif

/* Some macros depend on the order, add new variables to the end. */
static void changepath(const char *);
static void getoptsreset(const char *);

static struct Var varinit[] = {
    {0, VSTRFIXED | VTEXTFIXED, defifsvar, 0},
    {0, VSTRFIXED | VTEXTFIXED, defpathvar, changepath},
    {0, VSTRFIXED | VTEXTFIXED, "PS1=$ ", 0},
    {0, VSTRFIXED | VTEXTFIXED, "PS2=> ", 0},
    {0, VSTRFIXED | VTEXTFIXED, "PS4=+ ", 0},
    {0, VSTRFIXED | VTEXTFIXED, defoptindvar, getoptsreset},
    {0, VSTRFIXED | VTEXTFIXED, linenovar, 0},
};

static const char kPrec[ARITH_BINOP_MAX - ARITH_BINOP_MIN] = {
#define ARITH_PRECEDENCE(OP, PREC) [OP - ARITH_BINOP_MIN] = PREC
    ARITH_PRECEDENCE(ARITH_MUL, 0),    ARITH_PRECEDENCE(ARITH_DIV, 0),
    ARITH_PRECEDENCE(ARITH_REM, 0),    ARITH_PRECEDENCE(ARITH_ADD, 1),
    ARITH_PRECEDENCE(ARITH_SUB, 1),    ARITH_PRECEDENCE(ARITH_LSHIFT, 2),
    ARITH_PRECEDENCE(ARITH_RSHIFT, 2), ARITH_PRECEDENCE(ARITH_LT, 3),
    ARITH_PRECEDENCE(ARITH_LE, 3),     ARITH_PRECEDENCE(ARITH_GT, 3),
    ARITH_PRECEDENCE(ARITH_GE, 3),     ARITH_PRECEDENCE(ARITH_EQ, 4),
    ARITH_PRECEDENCE(ARITH_NE, 4),     ARITH_PRECEDENCE(ARITH_BAND, 5),
    ARITH_PRECEDENCE(ARITH_BXOR, 6),   ARITH_PRECEDENCE(ARITH_BOR, 7),
#undef ARITH_PRECEDENCE
};

static const short nodesize[26] /* clang-format off */ = {
    SHELL_ALIGN(sizeof(struct ncmd)),    SHELL_ALIGN(sizeof(struct npipe)),
    SHELL_ALIGN(sizeof(struct nredir)),  SHELL_ALIGN(sizeof(struct nredir)),
    SHELL_ALIGN(sizeof(struct nredir)),  SHELL_ALIGN(sizeof(struct nbinary)),
    SHELL_ALIGN(sizeof(struct nbinary)), SHELL_ALIGN(sizeof(struct nbinary)),
    SHELL_ALIGN(sizeof(struct nif)),     SHELL_ALIGN(sizeof(struct nbinary)),
    SHELL_ALIGN(sizeof(struct nbinary)), SHELL_ALIGN(sizeof(struct nfor)),
    SHELL_ALIGN(sizeof(struct ncase)),   SHELL_ALIGN(sizeof(struct nclist)),
    SHELL_ALIGN(sizeof(struct ndefun)),  SHELL_ALIGN(sizeof(struct narg)),
    SHELL_ALIGN(sizeof(struct nfile)),   SHELL_ALIGN(sizeof(struct nfile)),
    SHELL_ALIGN(sizeof(struct nfile)),   SHELL_ALIGN(sizeof(struct nfile)),
    SHELL_ALIGN(sizeof(struct nfile)),   SHELL_ALIGN(sizeof(struct ndup)),
    SHELL_ALIGN(sizeof(struct ndup)),    SHELL_ALIGN(sizeof(struct nhere)),
    SHELL_ALIGN(sizeof(struct nhere)),   SHELL_ALIGN(sizeof(struct nnot)),
} /* clang-format on */;

/* syntax table used when not in quotes */
static const char basesyntax[] /* clang-format off */ = {
    CEOF,    CWORD,   CCTL,    CCTL,  CCTL,  CCTL,  CCTL,    CCTL,  CCTL,
    CCTL,    CWORD,   CWORD,   CWORD, CWORD, CWORD, CWORD,   CWORD, CWORD,
    CWORD,   CWORD,   CWORD,   CWORD, CWORD, CWORD, CWORD,   CWORD, CWORD,
    CWORD,   CWORD,   CWORD,   CWORD, CWORD, CWORD, CWORD,   CWORD, CWORD,
    CWORD,   CWORD,   CWORD,   CWORD, CWORD, CWORD, CWORD,   CWORD, CWORD,
    CWORD,   CWORD,   CWORD,   CWORD, CWORD, CWORD, CWORD,   CWORD, CWORD,
    CWORD,   CWORD,   CWORD,   CWORD, CWORD, CWORD, CWORD,   CWORD, CWORD,
    CWORD,   CWORD,   CWORD,   CWORD, CWORD, CWORD, CWORD,   CWORD, CWORD,
    CWORD,   CWORD,   CWORD,   CWORD, CWORD, CWORD, CWORD,   CWORD, CWORD,
    CWORD,   CWORD,   CWORD,   CWORD, CWORD, CWORD, CWORD,   CWORD, CWORD,
    CWORD,   CWORD,   CWORD,   CWORD, CWORD, CWORD, CWORD,   CWORD, CWORD,
    CWORD,   CWORD,   CWORD,   CWORD, CWORD, CWORD, CWORD,   CWORD, CWORD,
    CWORD,   CWORD,   CWORD,   CWORD, CWORD, CWORD, CWORD,   CWORD, CWORD,
    CWORD,   CWORD,   CWORD,   CWORD, CWORD, CWORD, CWORD,   CWORD, CWORD,
    CWORD,   CWORD,   CWORD,   CWORD, CWORD, CWORD, CWORD,   CWORD, CWORD,
    CWORD,   CWORD,   CWORD,   CSPCL, CNL,   CWORD, CWORD,   CWORD, CWORD,
    CWORD,   CWORD,   CWORD,   CWORD, CWORD, CWORD, CWORD,   CWORD, CWORD,
    CWORD,   CWORD,   CWORD,   CWORD, CWORD, CWORD, CWORD,   CWORD, CSPCL,
    CWORD,   CDQUOTE, CWORD,   CVAR,  CWORD, CSPCL, CSQUOTE, CSPCL, CSPCL,
    CWORD,   CWORD,   CWORD,   CWORD, CWORD, CWORD, CWORD,   CWORD, CWORD,
    CWORD,   CWORD,   CWORD,   CWORD, CWORD, CWORD, CWORD,   CWORD, CSPCL,
    CSPCL,   CWORD,   CSPCL,   CWORD, CWORD, CWORD, CWORD,   CWORD, CWORD,
    CWORD,   CWORD,   CWORD,   CWORD, CWORD, CWORD, CWORD,   CWORD, CWORD,
    CWORD,   CWORD,   CWORD,   CWORD, CWORD, CWORD, CWORD,   CWORD, CWORD,
    CWORD,   CWORD,   CWORD,   CWORD, CWORD, CBACK, CWORD,   CWORD, CWORD,
    CBQUOTE, CWORD,   CWORD,   CWORD, CWORD, CWORD, CWORD,   CWORD, CWORD,
    CWORD,   CWORD,   CWORD,   CWORD, CWORD, CWORD, CWORD,   CWORD, CWORD,
    CWORD,   CWORD,   CWORD,   CWORD, CWORD, CWORD, CWORD,   CWORD, CWORD,
    CWORD,   CSPCL,   CENDVAR, CWORD, CWORD
} /* clang-format on */;

/* syntax table used when in double quotes */
static const char dqsyntax[] /* clang-format off */ = {
    CEOF,    CWORD,     CCTL,    CCTL,  CCTL,  CCTL,  CCTL,  CCTL,  CCTL,
    CCTL,    CWORD,     CWORD,   CWORD, CWORD, CWORD, CWORD, CWORD, CWORD,
    CWORD,   CWORD,     CWORD,   CWORD, CWORD, CWORD, CWORD, CWORD, CWORD,
    CWORD,   CWORD,     CWORD,   CWORD, CWORD, CWORD, CWORD, CWORD, CWORD,
    CWORD,   CWORD,     CWORD,   CWORD, CWORD, CWORD, CWORD, CWORD, CWORD,
    CWORD,   CWORD,     CWORD,   CWORD, CWORD, CWORD, CWORD, CWORD, CWORD,
    CWORD,   CWORD,     CWORD,   CWORD, CWORD, CWORD, CWORD, CWORD, CWORD,
    CWORD,   CWORD,     CWORD,   CWORD, CWORD, CWORD, CWORD, CWORD, CWORD,
    CWORD,   CWORD,     CWORD,   CWORD, CWORD, CWORD, CWORD, CWORD, CWORD,
    CWORD,   CWORD,     CWORD,   CWORD, CWORD, CWORD, CWORD, CWORD, CWORD,
    CWORD,   CWORD,     CWORD,   CWORD, CWORD, CWORD, CWORD, CWORD, CWORD,
    CWORD,   CWORD,     CWORD,   CWORD, CWORD, CWORD, CWORD, CWORD, CWORD,
    CWORD,   CWORD,     CWORD,   CWORD, CWORD, CWORD, CWORD, CWORD, CWORD,
    CWORD,   CWORD,     CWORD,   CWORD, CWORD, CWORD, CWORD, CWORD, CWORD,
    CWORD,   CWORD,     CWORD,   CWORD, CWORD, CWORD, CWORD, CWORD, CWORD,
    CWORD,   CWORD,     CWORD,   CWORD, CNL,   CWORD, CWORD, CWORD, CWORD,
    CWORD,   CWORD,     CWORD,   CWORD, CWORD, CWORD, CWORD, CWORD, CWORD,
    CWORD,   CWORD,     CWORD,   CWORD, CWORD, CWORD, CWORD, CWORD, CWORD,
    CCTL,    CENDQUOTE, CWORD,   CVAR,  CWORD, CWORD, CWORD, CWORD, CWORD,
    CCTL,    CWORD,     CWORD,   CCTL,  CWORD, CCTL,  CWORD, CWORD, CWORD,
    CWORD,   CWORD,     CWORD,   CWORD, CWORD, CWORD, CWORD, CCTL,  CWORD,
    CWORD,   CCTL,      CWORD,   CCTL,  CWORD, CWORD, CWORD, CWORD, CWORD,
    CWORD,   CWORD,     CWORD,   CWORD, CWORD, CWORD, CWORD, CWORD, CWORD,
    CWORD,   CWORD,     CWORD,   CWORD, CWORD, CWORD, CWORD, CWORD, CWORD,
    CWORD,   CWORD,     CWORD,   CWORD, CCTL,  CBACK, CCTL,  CWORD, CWORD,
    CBQUOTE, CWORD,     CWORD,   CWORD, CWORD, CWORD, CWORD, CWORD, CWORD,
    CWORD,   CWORD,     CWORD,   CWORD, CWORD, CWORD, CWORD, CWORD, CWORD,
    CWORD,   CWORD,     CWORD,   CWORD, CWORD, CWORD, CWORD, CWORD, CWORD,
    CWORD,   CWORD,     CENDVAR, CCTL,  CWORD
} /* clang-format on */;

/* syntax table used when in single quotes */
static const char sqsyntax[] /* clang-format off */ = {
    CEOF,  CWORD, CCTL,  CCTL,  CCTL,  CCTL,  CCTL,      CCTL,  CCTL,
    CCTL,  CWORD, CWORD, CWORD, CWORD, CWORD, CWORD,     CWORD, CWORD,
    CWORD, CWORD, CWORD, CWORD, CWORD, CWORD, CWORD,     CWORD, CWORD,
    CWORD, CWORD, CWORD, CWORD, CWORD, CWORD, CWORD,     CWORD, CWORD,
    CWORD, CWORD, CWORD, CWORD, CWORD, CWORD, CWORD,     CWORD, CWORD,
    CWORD, CWORD, CWORD, CWORD, CWORD, CWORD, CWORD,     CWORD, CWORD,
    CWORD, CWORD, CWORD, CWORD, CWORD, CWORD, CWORD,     CWORD, CWORD,
    CWORD, CWORD, CWORD, CWORD, CWORD, CWORD, CWORD,     CWORD, CWORD,
    CWORD, CWORD, CWORD, CWORD, CWORD, CWORD, CWORD,     CWORD, CWORD,
    CWORD, CWORD, CWORD, CWORD, CWORD, CWORD, CWORD,     CWORD, CWORD,
    CWORD, CWORD, CWORD, CWORD, CWORD, CWORD, CWORD,     CWORD, CWORD,
    CWORD, CWORD, CWORD, CWORD, CWORD, CWORD, CWORD,     CWORD, CWORD,
    CWORD, CWORD, CWORD, CWORD, CWORD, CWORD, CWORD,     CWORD, CWORD,
    CWORD, CWORD, CWORD, CWORD, CWORD, CWORD, CWORD,     CWORD, CWORD,
    CWORD, CWORD, CWORD, CWORD, CWORD, CWORD, CWORD,     CWORD, CWORD,
    CWORD, CWORD, CWORD, CWORD, CNL,   CWORD, CWORD,     CWORD, CWORD,
    CWORD, CWORD, CWORD, CWORD, CWORD, CWORD, CWORD,     CWORD, CWORD,
    CWORD, CWORD, CWORD, CWORD, CWORD, CWORD, CWORD,     CWORD, CWORD,
    CCTL,  CWORD, CWORD, CWORD, CWORD, CWORD, CENDQUOTE, CWORD, CWORD,
    CCTL,  CWORD, CWORD, CCTL,  CWORD, CCTL,  CWORD,     CWORD, CWORD,
    CWORD, CWORD, CWORD, CWORD, CWORD, CWORD, CWORD,     CCTL,  CWORD,
    CWORD, CCTL,  CWORD, CCTL,  CWORD, CWORD, CWORD,     CWORD, CWORD,
    CWORD, CWORD, CWORD, CWORD, CWORD, CWORD, CWORD,     CWORD, CWORD,
    CWORD, CWORD, CWORD, CWORD, CWORD, CWORD, CWORD,     CWORD, CWORD,
    CWORD, CWORD, CWORD, CWORD, CCTL,  CCTL,  CCTL,      CWORD, CWORD,
    CWORD, CWORD, CWORD, CWORD, CWORD, CWORD, CWORD,     CWORD, CWORD,
    CWORD, CWORD, CWORD, CWORD, CWORD, CWORD, CWORD,     CWORD, CWORD,
    CWORD, CWORD, CWORD, CWORD, CWORD, CWORD, CWORD,     CWORD, CWORD,
    CWORD, CWORD, CWORD, CCTL,  CWORD
} /* clang-format on */;

/* syntax table used when in arithmetic */
static const char arisyntax[] /* clang-format off */ = {
    CEOF,    CWORD, CCTL,    CCTL,  CCTL,  CCTL,  CCTL,  CCTL,  CCTL,
    CCTL,    CWORD, CWORD,   CWORD, CWORD, CWORD, CWORD, CWORD, CWORD,
    CWORD,   CWORD, CWORD,   CWORD, CWORD, CWORD, CWORD, CWORD, CWORD,
    CWORD,   CWORD, CWORD,   CWORD, CWORD, CWORD, CWORD, CWORD, CWORD,
    CWORD,   CWORD, CWORD,   CWORD, CWORD, CWORD, CWORD, CWORD, CWORD,
    CWORD,   CWORD, CWORD,   CWORD, CWORD, CWORD, CWORD, CWORD, CWORD,
    CWORD,   CWORD, CWORD,   CWORD, CWORD, CWORD, CWORD, CWORD, CWORD,
    CWORD,   CWORD, CWORD,   CWORD, CWORD, CWORD, CWORD, CWORD, CWORD,
    CWORD,   CWORD, CWORD,   CWORD, CWORD, CWORD, CWORD, CWORD, CWORD,
    CWORD,   CWORD, CWORD,   CWORD, CWORD, CWORD, CWORD, CWORD, CWORD,
    CWORD,   CWORD, CWORD,   CWORD, CWORD, CWORD, CWORD, CWORD, CWORD,
    CWORD,   CWORD, CWORD,   CWORD, CWORD, CWORD, CWORD, CWORD, CWORD,
    CWORD,   CWORD, CWORD,   CWORD, CWORD, CWORD, CWORD, CWORD, CWORD,
    CWORD,   CWORD, CWORD,   CWORD, CWORD, CWORD, CWORD, CWORD, CWORD,
    CWORD,   CWORD, CWORD,   CWORD, CWORD, CWORD, CWORD, CWORD, CWORD,
    CWORD,   CWORD, CWORD,   CWORD, CNL,   CWORD, CWORD, CWORD, CWORD,
    CWORD,   CWORD, CWORD,   CWORD, CWORD, CWORD, CWORD, CWORD, CWORD,
    CWORD,   CWORD, CWORD,   CWORD, CWORD, CWORD, CWORD, CWORD, CWORD,
    CWORD,   CWORD, CWORD,   CVAR,  CWORD, CWORD, CWORD, CLP,   CRP,
    CWORD,   CWORD, CWORD,   CWORD, CWORD, CWORD, CWORD, CWORD, CWORD,
    CWORD,   CWORD, CWORD,   CWORD, CWORD, CWORD, CWORD, CWORD, CWORD,
    CWORD,   CWORD, CWORD,   CWORD, CWORD, CWORD, CWORD, CWORD, CWORD,
    CWORD,   CWORD, CWORD,   CWORD, CWORD, CWORD, CWORD, CWORD, CWORD,
    CWORD,   CWORD, CWORD,   CWORD, CWORD, CWORD, CWORD, CWORD, CWORD,
    CWORD,   CWORD, CWORD,   CWORD, CWORD, CBACK, CWORD, CWORD, CWORD,
    CBQUOTE, CWORD, CWORD,   CWORD, CWORD, CWORD, CWORD, CWORD, CWORD,
    CWORD,   CWORD, CWORD,   CWORD, CWORD, CWORD, CWORD, CWORD, CWORD,
    CWORD,   CWORD, CWORD,   CWORD, CWORD, CWORD, CWORD, CWORD, CWORD,
    CWORD,   CWORD, CENDVAR, CWORD, CWORD
} /* clang-format on */;

/* character classification table */
static const char is_type[] /* clang-format off */ = {
    0,       0,       0,       0,       0,       0,       0,       0,
    0,       0,       0,       0,       0,       0,       0,       0,
    0,       0,       0,       0,       0,       0,       0,       0,
    0,       0,       0,       0,       0,       0,       0,       0,
    0,       0,       0,       0,       0,       0,       0,       0,
    0,       0,       0,       0,       0,       0,       0,       0,
    0,       0,       0,       0,       0,       0,       0,       0,
    0,       0,       0,       0,       0,       0,       0,       0,
    0,       0,       0,       0,       0,       0,       0,       0,
    0,       0,       0,       0,       0,       0,       0,       0,
    0,       0,       0,       0,       0,       0,       0,       0,
    0,       0,       0,       0,       0,       0,       0,       0,
    0,       0,       0,       0,       0,       0,       0,       0,
    0,       0,       0,       0,       0,       0,       0,       0,
    0,       0,       0,       0,       0,       0,       0,       0,
    0,       0,       0,       0,       0,       0,       0,       0,
    0,       0,       0,       0,       0,       0,       0,       0,
    0,       0,       0,       0,       0,       0,       0,       0,
    0,       0,       0,       0,       0,       0,       0,       0,
    0,       0,       0,       0,       0,       0,       0,       0,
    0,       0,       ISSPECL, 0,       ISSPECL, ISSPECL, 0,       0,
    0,       0,       0,       ISSPECL, 0,       0,       ISSPECL, 0,
    0,       ISDIGIT, ISDIGIT, ISDIGIT, ISDIGIT, ISDIGIT, ISDIGIT, ISDIGIT,
    ISDIGIT, ISDIGIT, ISDIGIT, 0,       0,       0,       0,       0,
    ISSPECL, ISSPECL, ISUPPER, ISUPPER, ISUPPER, ISUPPER, ISUPPER, ISUPPER,
    ISUPPER, ISUPPER, ISUPPER, ISUPPER, ISUPPER, ISUPPER, ISUPPER, ISUPPER,
    ISUPPER, ISUPPER, ISUPPER, ISUPPER, ISUPPER, ISUPPER, ISUPPER, ISUPPER,
    ISUPPER, ISUPPER, ISUPPER, ISUPPER, 0,       0,       0,       0,
    ISUNDER, 0,       ISLOWER, ISLOWER, ISLOWER, ISLOWER, ISLOWER, ISLOWER,
    ISLOWER, ISLOWER, ISLOWER, ISLOWER, ISLOWER, ISLOWER, ISLOWER, ISLOWER,
    ISLOWER, ISLOWER, ISLOWER, ISLOWER, ISLOWER, ISLOWER, ISLOWER, ISLOWER,
    ISLOWER, ISLOWER, ISLOWER, ISLOWER, 0,       0,       0,       0,
    0
} /* clang-format on */;

static int aliascmd();
static int bgcmd();
static int breakcmd();
static int cdcmd();
static int commandcmd();
static int dotcmd();
static int echocmd();
static int evalcmd();
static int execcmd();
static int exitcmd();
static int exportcmd();
static int falsecmd();
static int fgcmd();
static int getoptscmd();
static int hashcmd();
static int jobscmd();
static int killcmd();
static int localcmd();
static int printfcmd();
static int pwdcmd();
static int readcmd();
static int returncmd();
static int setcmd();
static int shiftcmd();
static int testcmd();
static int timescmd();
static int trapcmd();
static int truecmd();
static int typecmd();
static int ulimitcmd();
static int umaskcmd();
static int unaliascmd();
static int unsetcmd();
static int waitcmd();

static const struct builtincmd kBuiltinCmds[] = {
    {".", dotcmd, 3},            //
    {":", truecmd, 3},           //
    {"[", testcmd, 0},           //
    {"alias", aliascmd, 6},      //
    {"bg", bgcmd, 2},            //
    {"break", breakcmd, 3},      //
    {"cd", cdcmd, 2},            //
    {"chdir", cdcmd, 0},         //
    {"command", commandcmd, 2},  //
    {"continue", breakcmd, 3},   //
    {"echo", echocmd, 0},        //
    {"eval", NULL, 3},           //
    {"exec", execcmd, 3},        //
    {"exit", exitcmd, 3},        //
    {"export", exportcmd, 7},    //
    {"false", falsecmd, 2},      //
    {"fg", fgcmd, 2},            //
    {"getopts", getoptscmd, 2},  //
    {"hash", hashcmd, 2},        //
    {"jobs", jobscmd, 2},        //
    {"kill", killcmd, 2},        //
    {"local", localcmd, 7},      //
    {"printf", printfcmd, 0},    //
    {"pwd", pwdcmd, 2},          //
    {"read", readcmd, 2},        //
    {"readonly", exportcmd, 7},  //
    {"return", returncmd, 3},    //
    {"set", setcmd, 3},          //
    {"shift", shiftcmd, 3},      //
    {"test", testcmd, 0},        //
    {"times", timescmd, 3},      //
    {"trap", trapcmd, 3},        //
    {"true", truecmd, 2},        //
    {"type", typecmd, 2},        //
    {"ulimit", ulimitcmd, 2},    //
    {"umask", umaskcmd, 2},      //
    {"unalias", unaliascmd, 2},  //
    {"unset", unsetcmd, 3},      //
    {"wait", waitcmd, 2},        //
};

enum token {
  EOI,
  FILRD,
  FILWR,
  FILEX,
  FILEXIST,
  FILREG,
  FILDIR,
  FILCDEV,
  FILBDEV,
  FILFIFO,
  FILSOCK,
  FILSYM,
  FILGZ,
  FILTT,
  FILSUID,
  FILSGID,
  FILSTCK,
  FILNT,
  FILOT,
  FILEQ,
  FILUID,
  FILGID,
  STREZ,
  STRNZ,
  STREQ,
  STRNE,
  STRLT,
  STRGT,
  INTEQ,
  INTNE,
  INTGE,
  INTGT,
  INTLE,
  INTLT,
  UNOT,
  BAND,
  BOR,
  LPAREN,
  RPAREN,
  OPERAND
};

enum token_types { UNOP, BINOP, BUNOP, BBINOP, PAREN };

static struct t_op const ops[] = {
    {"-r", FILRD, UNOP},
    {"-w", FILWR, UNOP},
    {"-x", FILEX, UNOP},
    {"-e", FILEXIST, UNOP},
    {"-f", FILREG, UNOP},
    {"-d", FILDIR, UNOP},
    {"-c", FILCDEV, UNOP},
    {"-b", FILBDEV, UNOP},
    {"-p", FILFIFO, UNOP},
    {"-u", FILSUID, UNOP},
    {"-g", FILSGID, UNOP},
    {"-k", FILSTCK, UNOP},
    {"-s", FILGZ, UNOP},
    {"-t", FILTT, UNOP},
    {"-z", STREZ, UNOP},
    {"-n", STRNZ, UNOP},
    {"-h", FILSYM, UNOP}, /* for backwards compat */
    {"-O", FILUID, UNOP},
    {"-G", FILGID, UNOP},
    {"-L", FILSYM, UNOP},
    {"-S", FILSOCK, UNOP},
    {"=", STREQ, BINOP},
    {"!=", STRNE, BINOP},
    {"<", STRLT, BINOP},
    {">", STRGT, BINOP},
    {"-eq", INTEQ, BINOP},
    {"-ne", INTNE, BINOP},
    {"-ge", INTGE, BINOP},
    {"-gt", INTGT, BINOP},
    {"-le", INTLE, BINOP},
    {"-lt", INTLT, BINOP},
    {"-nt", FILNT, BINOP},
    {"-ot", FILOT, BINOP},
    {"-ef", FILEQ, BINOP},
    {"!", UNOT, BUNOP},
    {"-a", BAND, BBINOP},
    {"-o", BOR, BBINOP},
    {"(", LPAREN, PAREN},
    {")", RPAREN, PAREN},
    {0, 0, 0},
};

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § the unbourne shell » text                                 ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

/*
 * Hack to calculate maximum length.
 * (length * 8 - 1) * log10(2) + 1 + 1 + 12
 * The second 1 is for the minus sign and the 12 is a safety margin.
 */
static inline int max_int_length(int bytes) {
  return (bytes * 8 - 1) * 0.30102999566398119521 + 14;
}

/* prefix -- see if pfx is a prefix of string. */
static char *prefix(const char *string, const char *pfx) {
  while (*pfx) {
    if (*pfx++ != *string++) return 0;
  }
  return (char *)string;
}

/*
 * Wrapper around strcmp for qsort/bsearch/...
 */
static int pstrcmp(const void *a, const void *b) {
  return strcmp(*(const char *const *)a, *(const char *const *)b);
}

/*
 * Find a string is in a sorted array.
 */
static const char *const *findstring(const char *s, const char *const *array,
                                     unsigned nmemb) {
  return bsearch(&s, array, nmemb, sizeof(const char *), pstrcmp);
}

/* Types of operations (passed to the errmsg routine). */
enum ShErrorAction { E_OPEN, E_CREAT, E_EXEC };

/*
 * Return a string describing an error.  The returned string may be a
 * pointer to a static buffer that will be overwritten on the next call.
 * Action describes the operation that got the error.
 */
static const char *errmsg(int e, enum ShErrorAction action) {
  if (e != ENOENT && e != ENOTDIR) return strerror(e);
  switch (action) {
    case E_OPEN:
      return "No such file";
    case E_CREAT:
      return "Directory nonexistent";
    default:
      return "not found";
  }
}

static inline void sigclearmask(void) {
  sigset_t set;
  sigemptyset(&set);
  sigprocmask(SIG_SETMASK, &set, 0);
}

/*
 * Called to raise an exception.  Since C doesn't include exceptions, we
 * just do a longjmp to the exception handler.  The type of exception is
 * stored in the global variable "exception".
 */
wontreturn static void exraise(int e) {
  if (vforked) _exit(exitstatus);
  INTOFF;
  exception = e;
  longjmp(handler->loc, 1);
}

/*
 * Called from trap.c when a SIGINT is received.  (If the user specifies
 * that SIGINT is to be trapped or ignored using the trap builtin, then
 * this routine is not called.)  Suppressint is nonzero when interrupts
 * are held using the INTOFF macro.  (The test for iflag is just
 * defensive programming.)
 */
wontreturn static void onint(void) {
  intpending = 0;
  sigclearmask();
  if (!(rootshell && iflag)) {
    signal(SIGINT, SIG_DFL);
    raise(SIGINT);
  }
  exitstatus = SIGINT + 128;
  exraise(EXINT);
}

static pointer ckmalloc(unsigned nbytes) {
  pointer p;
  if (!(p = malloc(nbytes))) abort();
  return p;
}

static pointer ckrealloc(pointer p, unsigned nbytes) {
  if (!(p = realloc(p, nbytes))) abort();
  return p;
}

#define stackblock()     ((void *)stacknxt)
#define stackblocksize() stacknleft
#define STARTSTACKSTR(p) ((p) = stackblock())
#define STPUTC(c, p)     ((p) = _STPUTC((c), (p)))
#define CHECKSTRSPACE(n, p)              \
  ({                                     \
    char *q = (p);                       \
    unsigned l = (n);                    \
    unsigned m = sstrend - q;            \
    if (l > m) (p) = makestrspace(l, q); \
    0;                                   \
  })
#define USTPUTC(c, p) (*p++ = (c))
#define STACKSTRNUL(p) \
  ((p) == sstrend ? (p = growstackstr(), *p = '\0') : (*p = '\0'))
#define STUNPUTC(p)          (--p)
#define STTOPC(p)            p[-1]
#define STADJUST(amount, p)  (p += (amount))
#define grabstackstr(p)      stalloc((char *)(p) - (char *)stackblock())
#define ungrabstackstr(s, p) stunalloc((s))
#define stackstrend()        ((void *)sstrend)
#define ckfree(p)            free((pointer)(p))

static pointer stalloc(unsigned nbytes) {
  char *p;
  unsigned aligned;
  aligned = SHELL_ALIGN(nbytes);
  if (aligned > stacknleft) {
    unsigned len;
    unsigned blocksize;
    struct stack_block *sp;
    blocksize = aligned;
    if (blocksize < MINSIZE) blocksize = MINSIZE;
    len = sizeof(struct stack_block) - MINSIZE + blocksize;
    if (len < blocksize) abort();
    INTOFF;
    sp = ckmalloc(len);
    sp->prev = stackp;
    stacknxt = sp->space;
    stacknleft = blocksize;
    sstrend = stacknxt + blocksize;
    stackp = sp;
    INTON;
  }
  p = stacknxt;
  stacknxt += aligned;
  stacknleft -= aligned;
  return p;
}

static inline void grabstackblock(unsigned len) {
  stalloc(len);
}

static void pushstackmark(struct stackmark *mark, unsigned len) {
  mark->stackp = stackp;
  mark->stacknxt = stacknxt;
  mark->stacknleft = stacknleft;
  grabstackblock(len);
}

static void popstackmark(struct stackmark *mark) {
  struct stack_block *sp;
  INTOFF;
  while (stackp != mark->stackp) {
    sp = stackp;
    stackp = sp->prev;
    ckfree(sp);
  }
  stacknxt = mark->stacknxt;
  stacknleft = mark->stacknleft;
  sstrend = mark->stacknxt + mark->stacknleft;
  INTON;
}

static void setstackmark(struct stackmark *mark) {
  pushstackmark(mark, stacknxt == stackp->space && stackp != &stackbase);
}

static void stunalloc(pointer p) {
  stacknleft += stacknxt - (char *)p;
  stacknxt = p;
}

/* Like strdup but works with the ash stack. */
static char *sstrdup(const char *p) {
  unsigned len = strlen(p) + 1;
  return memcpy(stalloc(len), p, len);
}

int __xwrite(int, const void *, uint64_t);

static void flushout(struct output *dest) {
  unsigned len;
  len = dest->nextc - dest->buf;
  if (!len || dest->fd < 0) return;
  dest->nextc = dest->buf;
  if ((__xwrite(dest->fd, dest->buf, len))) dest->flags |= OUTPUT_ERR;
}

static void flushall(void) {
  flushout(&output);
}

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § the unbourne shell » output routines                      ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│─╝
        When a builtin command is interrupted we have to discard
                any pending output.
        When a builtin command appears in back quotes, we want to
                save the output of the command in a region obtained
                via malloc, rather than doing a fork and reading the
                output of the command via a pipe. */

static int xvsnprintf(char *outbuf, unsigned length, const char *fmt,
                      va_list ap) {
  int ret;
  INTOFF;
  ret = vsnprintf(outbuf, length, fmt, ap);
  INTON;
  return ret;
}

static int Xvasprintf(char **sp, unsigned size, const char *f, va_list ap) {
  char *s;
  int len;
  va_list ap2;
  va_copy(ap2, ap);
  len = xvsnprintf(*sp, size, f, ap2);
  va_end(ap2);
  if (len < 0) abort();
  if (len < size) return len;
  s = stalloc((len >= stackblocksize() ? len : stackblocksize()) + 1);
  *sp = s;
  len = xvsnprintf(s, len + 1, f, ap);
  return len;
}

static void outmem(const char *p, unsigned len, struct output *dest) {
  unsigned bufsize;
  unsigned offset;
  unsigned nleft;
  nleft = dest->end - dest->nextc;
  if (likely(nleft >= len)) {
  buffered:
    dest->nextc = mempcpy(dest->nextc, p, len);
    return;
  }
  bufsize = dest->bufsize;
  if (!bufsize) {
    (void)0;
  } else if (dest->buf == NULL) {
    offset = 0;
    INTOFF;
    dest->buf = ckrealloc(dest->buf, bufsize);
    dest->bufsize = bufsize;
    dest->end = dest->buf + bufsize;
    dest->nextc = dest->buf + offset;
    INTON;
  } else {
    flushout(dest);
  }
  nleft = dest->end - dest->nextc;
  if (nleft > len) goto buffered;
  if ((__xwrite(dest->fd, p, len))) {
    dest->flags |= OUTPUT_ERR;
  }
}

static void outstr(const char *p, struct output *file) {
  unsigned len;
  len = strlen(p);
  outmem(p, len, file);
}

static void outcslow(int c, struct output *dest) {
  char buf = c;
  outmem(&buf, 1, dest);
}

printfesque(3) static int fmtstr(char *outbuf, unsigned length, const char *fmt,
                                 ...) {
  va_list ap;
  int ret;
  va_start(ap, fmt);
  ret = xvsnprintf(outbuf, length, fmt, ap);
  va_end(ap);
  return ret > (int)length ? length : ret;
}

printfesque(2) static int Xasprintf(char **sp, const char *fmt, ...) {
  va_list ap;
  int ret;
  va_start(ap, fmt);
  ret = Xvasprintf(sp, 0, fmt, ap);
  va_end(ap);
  return ret;
}

static void doformat(struct output *dest, const char *f, va_list ap) {
  struct stackmark smark;
  char *s;
  int len;
  int olen;
  setstackmark(&smark);
  s = dest->nextc;
  olen = dest->end - dest->nextc;
  len = Xvasprintf(&s, olen, f, ap);
  if (likely(olen > len)) {
    dest->nextc += len;
    goto out;
  }
  outmem(s, len, dest);
out:
  popstackmark(&smark);
}

printfesque(1) static void out1fmt(const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  doformat(out1, fmt, ap);
  va_end(ap);
}

printfesque(2) static void outfmt(struct output *file, const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  doformat(file, fmt, ap);
  va_end(ap);
}

static void exvwarning(const char *msg, va_list ap) {
  struct output *errs;
  const char *name;
  const char *fmt;
  errs = out2;
  name = arg0 ? arg0 : "sh";
  if (!commandname) {
    fmt = "%s: %d: ";
  } else {
    fmt = "%s: %d: %s: ";
  }
  outfmt(errs, fmt, name, errlinno, commandname);
  doformat(errs, msg, ap);
  outcslow('\n', errs);
}

/* error/warning routines for external builtins */
printfesque(1) static void sh_warnx(const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  exvwarning(fmt, ap);
  va_end(ap);
}

/*
 * Exverror is called to raise the error exception.  If the second argument
 * is not NULL then error prints an error message using printf style
 * formatting.  It then raises the error exception.
 */
wontreturn static void exverror(int cond, const char *msg, va_list ap) {
  exvwarning(msg, ap);
  flushall();
  exraise(cond);
}

wontreturn static void exerror(int cond, const char *msg, ...) {
  va_list ap;
  va_start(ap, msg);
  exverror(cond, msg, ap);
  va_end(ap);
}

wontreturn static void sh_error(const char *msg, ...) {
  va_list ap;
  exitstatus = 2;
  va_start(ap, msg);
  exverror(EXERROR, msg, ap);
  va_end(ap);
}

wontreturn static void badnum(const char *s) {
  sh_error(illnum, s);
}

wontreturn static void synerror(const char *msg) {
  errlinno = plinno;
  sh_error("Syntax error: %s", msg);
}

wontreturn static void yyerror(const char *s) {
  sh_error("arithmetic expression: %s: \"%s\"", s, arith_startbuf);
}

/*
 * Called when an unexpected token is read during the parse. The
 * argument is the token that is expected, or -1 if more than one type
 * of token can occur at this point.
 */
wontreturn static void synexpect(int token) {
  char msg[64];
  if (token >= 0) {
    fmtstr(msg, 64, "%s unexpected (expecting %s)", tokname[lasttoken],
           tokname[token]);
  } else {
    fmtstr(msg, 64, "%s unexpected", tokname[lasttoken]);
  }
  synerror(msg);
}

wontreturn static void varunset(const char *end, const char *var_,
                                const char *umsg, int varflags) {
  const char *msg;
  const char *tail;
  tail = nullstr;
  msg = "parameter not set";
  if (umsg) {
    if (*end == (char)CTLENDVAR) {
      if (varflags & VSNUL) tail = " or null";
    } else
      msg = umsg;
  }
  sh_error("%.*s: %s%s", end - var_ - 1, var_, msg, tail);
}

/*
 * Convert a string into an integer of type int64.  Allow trailing spaces.
 */
static int64_t atomax(const char *s, int base) {
  char *p;
  int64_t r;
  errno = 0;
  r = strtoimax(s, &p, base);
  if (errno == ERANGE) badnum(s);
  /*
   * Disallow completely blank strings in non-arithmetic (base != 0)
   * contexts.
   */
  if (p == s && base) badnum(s);
  while (isspace((unsigned char)*p)) p++;
  if (*p) badnum(s);
  return r;
}

static int64_t atomax10(const char *s) {
  return atomax(s, 10);
}

/*
 * Convert a string of digits to an integer, printing an error message
 * on failure.
 */
static int number(const char *s) {
  int64_t n = atomax10(s);
  if (n < 0 || n > INT_MAX) badnum(s);
  return n;
}

static inline int64_t getn(const char *s) {
  return atomax10(s);
}

/*
 * When the parser reads in a string, it wants to stick the string on
 * the stack and only adjust the stack pointer when it knows how big the
 * string is. Stackblock (defined in stack.h) returns a pointer to a
 * block of space on top of the stack and stackblocklen returns the
 * length of this block. Growstackblock will grow this space by at least
 * one byte, possibly moving it (like realloc). Grabstackblock actually
 * allocates the part of the block that has been used.
 */
static void growstackblock(unsigned min) {
  unsigned newlen;
  newlen = stacknleft * 2;
  if (newlen < stacknleft) sh_error("Out of space");
  min = SHELL_ALIGN(min | 128);
  if (newlen < min) newlen += min;
  if (stacknxt == stackp->space && stackp != &stackbase) {
    struct stack_block *sp;
    struct stack_block *prevstackp;
    unsigned grosslen;
    INTOFF;
    sp = stackp;
    prevstackp = sp->prev;
    grosslen = newlen + sizeof(struct stack_block) - MINSIZE;
    sp = ckrealloc((pointer)sp, grosslen);
    sp->prev = prevstackp;
    stackp = sp;
    stacknxt = sp->space;
    stacknleft = newlen;
    sstrend = sp->space + newlen;
    INTON;
  } else {
    char *oldspace = stacknxt;
    int oldlen = stacknleft;
    char *p = stalloc(newlen);
    /* free the space we just allocated */
    stacknxt = memcpy(p, oldspace, oldlen);
    stacknleft += newlen;
  }
}

/*
 * The following routines are somewhat easier to use than the above. The
 * user declares a variable of type STACKSTR, which may be declared to
 * be a register. The macro STARTSTACKSTR initializes things. Then the
 * user uses the macro STPUTC to add characters to the string. In
 * effect, STPUTC(c, p) is the same as *p++ = c except that the stack is
 * grown as necessary. When the user is done, she can just leave the
 * string there and refer to it using stackblock(). Or she can allocate
 * the space for it using grabstackstr(). If it is necessary to allow
 * someone else to use the stack temporarily and then continue to grow
 * the string, the user should use grabstack to allocate the space, and
 * then call ungrabstr(p) to return to the previous mode of operation.
 *
 * USTPUTC is like STPUTC except that it doesn't check for overflow.
 * CHECKSTACKSPACE can be called before USTPUTC to ensure that there
 * is space for at least one character.
 */
static void *growstackstr(void) {
  unsigned len = stackblocksize();
  growstackblock(0);
  return (char *)stackblock() + len;
}

static char *growstackto(unsigned len) {
  if (stackblocksize() < len) growstackblock(len);
  return stackblock();
}

/*
 * Make a copy of a string in safe storage.
 */
static char *savestr(const char *s) {
  char *p = strdup(s);
  if (!p) sh_error("Out of space");
  return p;
}

/* Called from CHECKSTRSPACE. */
static char *makestrspace(unsigned newlen, char *p) {
  unsigned len = p - stacknxt;
  return growstackto(len + newlen) + len;
}

static char *stnputs(const char *s, unsigned n, char *p) {
  p = makestrspace(n, p);
  p = mempcpy(p, s, n);
  return p;
}

static char *stputs(const char *s, char *p) {
  return stnputs(s, strlen(s), p);
}

static char *nodesavestr(s)
char *s;
{
  char *rtn = funcstring;
  funcstring = stpcpy(funcstring, s) + 1;
  return rtn;
}

wontreturn static void shellexec(char **, const char *, int);
static char **listvars(int, int, char ***);
static char *argstr(char *p, int flag);
static char *conv_escape(char *, int *);
static char *evalvar(char *, int);
static char *expari(char *start, int flag);
static char *exptilde(char *startp, int flag);
static int shlex(void);
static char *lookupvar(const char *);
static char *mklong(const char *, const char *);
static char *rmescapes(char *, int);
static char *scanleft(char *, char *, char *, char *, int, int);
static char *scanright(char *, char *, char *, char *, int, int);
static char *single_quote(const char *);
static const char *const *findkwd(const char *);
static const char *expandstr(const char *);
static const char *getprompt(void *);
static double getdouble(void);
static enum token t_lex(char **);
static int aexpr(enum token);
static int binop0(void);
static int bltincmd(int, char **);
static int conv_escape_str(char *, char **);
static int decode_signal(const char *, int);
static int decode_signum(const char *);
static int describe_command(struct output *, char *, const char *, int);
static int eprintlist(struct output *, struct strlist *, int);
static int equalf(const char *, const char *);
static int evalbltin(const struct builtincmd *, int, char **, int);
static int evalcase(union node *, int);
static int evalcommand(union node *, int);
static int evalfor(union node *, int);
static int evalfun(struct funcnode *, int, char **, int);
static int evalloop(union node *, int);
static int evalpipe(union node *, int);
static int evalsubshell(union node *, int);
static int filstat(char *, enum token);
static int forkshell(struct job *, union node *, int);
static int getopts(char *, char *, char **);
static int isassignment(const char *p);
static int isoperand(char **);
static int newerf(const char *, const char *);
static int nexpr(enum token);
static int nextopt(const char *);
static int oexpr(enum token);
static int olderf(const char *, const char *);
static int64_t openhere(union node *);
static int openredirect(union node *);
static int options(int);
static int padvance_magic(const char **, const char *, int);
static int patmatch(char *, const char *);
static int pgetc(void);
static int pgetc_eatbnl();
static int pmatch(const char *, const char *);
static int preadbuffer(void);
static ssize_t preadfd(void);
static int primary1(enum token);
static int procargs(int, char **);
static int readtoken(void);
static int readtoken1(int, char const *, char *, int);
static int redirectsafe(union node *, int);
static int savefd(int, int);
static int setinputfile(const char *, int);
static int sh_open(const char *pathname, int flags, int mayfail);
static int showvars(const char *, int, int);
static int stoppedjobs(void);
static int test_file_access(const char *, int);
static int unalias(const char *);
static int waitforjob(struct job *);
static int xxreadtoken(void);
static int64_t arith(const char *);
static int64_t assignment(int var_, int noeval);
static int64_t lookupvarint(const char *);
static long varvalue(char *, int, int, int);
static int64_t setvarint(const char *name, int64_t val, int flags);
static struct Var *setvar(const char *name, const char *val, int flags);
static struct Var *setvareq(char *s, int flags);
static struct alias **__lookupalias(const char *);
static struct alias *freealias(struct alias *);
static struct alias *lookupalias(const char *, int);
static struct funcnode *copyfunc(union node *);
static struct job *makejob(union node *, int);
static struct job *vforkexec(union node *n, char **argv, const char *path,
                             int idx);
static struct localvar_list *pushlocalvars(int push);
static struct nodelist *copynodelist(struct nodelist *);
static struct redirtab *pushredir(union node *redir);
static struct strlist *expsort(struct strlist *);
static struct strlist *msort(struct strlist *, int);
static struct tblentry *cmdlookup(const char *, int);
static uint64_t getuintmax(int);
static union node *andor(void);
static union node *command(void);
static union node *copynode(union node *);
static union node *list(int);
static union node *makename(void);
static union node *parsecmd(int);
static union node *pipeline(void);
static union node *simplecmd(void);
static unsigned esclen(const char *, const char *);
static unsigned memtodest(const char *p, unsigned len, int flags);
static unsigned strtodest(const char *p, int flags);
static void addcmdentry(char *, struct cmdentry *);
static void addfname(char *);
static void check_conversion(const char *, const char *);
static void clearcmdentry(void);
static void defun(union node *);
static void delete_cmd_entry(void);
static void dotrap(void);
static void dupredirect(union node *, int);
static void exitreset(void);
static void expandarg(union node *arg, struct arglist *arglist, int flag);
static void expandmeta(struct strlist *);
static void expbackq(union node *, int);
static void expmeta(char *, unsigned, unsigned);
static void expredir(union node *);
static void find_command(char *, struct cmdentry *, int, const char *);
static void fixredir(union node *, const char *, int);
static void forkreset(void);
static void freeparam(volatile struct shparam *);
static void hashcd(void);
static void ignoresig(int);
static void init(void);
static void minus_o(char *, int);
static void mklocal(char *name, int flags);
static void onsig(int);
static void optschanged(void);
static void parsefname(void);
static void parseheredoc(void);
static void popallfiles(void);
static void popfile(void);
static void poplocalvars(void);
static void popredir(int);
static void popstring(void);
static void prehash(union node *);
static void printalias(const struct alias *);
static void printentry(struct tblentry *);
static void pungetc(void);
static void pushfile(void);
static void pushstring(char *, void *);
static void read_profile(const char *);
static void redirect(union node *, int);
static void reset(void);
static void rmaliases(void);
static void setalias(const char *, const char *);
static void setinputfd(int fd, int push);
static void setinputstring(char *);
static void setinteractive(int);
static void setjobctl(int);
static void setparam(char **);
static void setprompt(int);
static void setsignal(int);
static void showjobs(struct output *, int);
static void sigblockall(sigset_t *oldmask);
static void sizenodelist(struct nodelist *);
static void syntax(const char *, const char *);
static void tryexec(char *, char **, char **);
static void unsetfunc(const char *);
static void unsetvar(const char *);
static void unwindfiles(struct parsefile *);
static void unwindlocalvars(struct localvar_list *stop);
static void unwindredir(struct redirtab *stop);
static unsigned cvtnum(int64_t num, int flags);

static int getchr(void) {
  int val = 0;
  if (*gargv) val = **gargv++;
  return val;
}

static char *getstr(void) {
  char *val = nullstr;
  if (*gargv) val = *gargv++;
  return val;
}

/*
 * Check for a valid number.  This should be elsewhere.
 */
static int is_number(const char *p) {
  do {
    if (!is_digit(*p)) return 0;
  } while (*++p != '\0');
  return 1;
}

static inline void freestdout() {
  output.nextc = output.buf;
  output.flags = 0;
}

static inline void outc(int ch, struct output *file) {
  if (file->nextc == file->end)
    outcslow(ch, file);
  else {
    *file->nextc = ch;
    file->nextc++;
  }
}

static inline char *_STPUTC(int c, char *p) {
  if (p == sstrend) p = growstackstr();
  *p++ = c;
  return p;
}

static void ifsfree(void) {
  struct ifsregion *p = ifsfirst.next;
  if (!p) goto out;
  INTOFF;
  do {
    struct ifsregion *ifsp;
    ifsp = p->next;
    ckfree(p);
    p = ifsp;
  } while (p);
  ifsfirst.next = NULL;
  INTON;
out:
  ifslastp = NULL;
}

static void setalias(const char *name, const char *val) {
  struct alias *ap, **app;
  app = __lookupalias(name);
  ap = *app;
  INTOFF;
  if (ap) {
    if (!(ap->flag & ALIASINUSE)) {
      ckfree(ap->val);
    }
    ap->val = savestr(val);
    ap->flag &= ~ALIASDEAD;
  } else {
    /* not found */
    ap = ckmalloc(sizeof(struct alias));
    ap->name = savestr(name);
    ap->val = savestr(val);
    ap->flag = 0;
    ap->next = 0;
    *app = ap;
  }
  INTON;
}

static int unalias(const char *name) {
  struct alias **app;
  app = __lookupalias(name);
  if (*app) {
    INTOFF;
    *app = freealias(*app);
    INTON;
    return (0);
  }
  return (1);
}

static void rmaliases(void) {
  struct alias *ap, **app;
  int i;
  INTOFF;
  for (i = 0; i < ATABSIZE; i++) {
    app = &atab[i];
    for (ap = *app; ap; ap = *app) {
      *app = freealias(*app);
      if (ap == *app) {
        app = &ap->next;
      }
    }
  }
  INTON;
}

struct alias *lookupalias(const char *name, int check) {
  struct alias *ap = *__lookupalias(name);
  if (check && ap && (ap->flag & ALIASINUSE)) return (NULL);
  return (ap);
}

static int aliascmd(int argc, char **argv) {
  /* TODO - sort output */
  char *n, *v;
  int ret = 0;
  struct alias *ap;
  if (argc == 1) {
    int i;
    for (i = 0; i < ATABSIZE; i++)
      for (ap = atab[i]; ap; ap = ap->next) {
        printalias(ap);
      }
    return (0);
  }
  while ((n = *++argv) != NULL) {
    if ((v = strchr(n + 1, '=')) == NULL) { /* n+1: funny ksh stuff */
      if ((ap = *__lookupalias(n)) == NULL) {
        outfmt(out2, "%s: %s not found\n", "alias", n);
        ret = 1;
      } else
        printalias(ap);
    } else {
      *v++ = '\0';
      setalias(n, v);
    }
  }
  return (ret);
}

static int unaliascmd(int argc, char **argv) {
  int i;
  while ((i = nextopt("a")) != '\0') {
    if (i == 'a') {
      rmaliases();
      return (0);
    }
  }
  for (i = 0; *argptr; argptr++) {
    if (unalias(*argptr)) {
      outfmt(out2, "%s: %s not found\n", "unalias", *argptr);
      i = 1;
    }
  }
  return i;
}

static struct alias *freealias(struct alias *ap) {
  struct alias *next;
  if (ap->flag & ALIASINUSE) {
    ap->flag |= ALIASDEAD;
    return ap;
  }
  next = ap->next;
  ckfree(ap->name);
  ckfree(ap->val);
  ckfree(ap);
  return next;
}

static void printalias(const struct alias *ap) {
  out1fmt("%s=%s\n", ap->name, single_quote(ap->val));
}

static struct alias **__lookupalias(const char *name) {
  unsigned int hashval;
  struct alias **app;
  const char *p;
  unsigned int ch;
  p = name;
  ch = (unsigned char)*p;
  hashval = ch << 4;
  while (ch) {
    hashval += ch;
    ch = (unsigned char)*++p;
  }
  app = &atab[hashval % ATABSIZE];
  for (; *app; app = &(*app)->next) {
    if (equal(name, (*app)->name)) {
      break;
    }
  }
  return app;
}

static int shlex() {
  int value;
  const char *buf = arith_buf;
  const char *p;
  for (;;) {
    value = *buf;
    switch (value) {
      case ' ':
      case '\t':
      case '\n':
        buf++;
        continue;
      default:
        return ARITH_BAD;
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
        yylval.val = strtoimax(buf, (char **)&arith_buf, 0);
        return ARITH_NUM;
      case 'A':
      case 'B':
      case 'C':
      case 'D':
      case 'E':
      case 'F':
      case 'G':
      case 'H':
      case 'I':
      case 'J':
      case 'K':
      case 'L':
      case 'M':
      case 'N':
      case 'O':
      case 'P':
      case 'Q':
      case 'R':
      case 'S':
      case 'T':
      case 'U':
      case 'V':
      case 'W':
      case 'X':
      case 'Y':
      case 'Z':
      case '_':
      case 'a':
      case 'b':
      case 'c':
      case 'd':
      case 'e':
      case 'f':
      case 'g':
      case 'h':
      case 'i':
      case 'j':
      case 'k':
      case 'l':
      case 'm':
      case 'n':
      case 'o':
      case 'p':
      case 'q':
      case 'r':
      case 's':
      case 't':
      case 'u':
      case 'v':
      case 'w':
      case 'x':
      case 'y':
      case 'z':
        p = buf;
        while (buf++, is_in_name(*buf));
        yylval.name = stalloc(buf - p + 1);
        *(char *)mempcpy(yylval.name, p, buf - p) = 0;
        value = ARITH_VAR;
        goto out;
      case '=':
        value += ARITH_ASS - '=';
      checkeq:
        buf++;
      checkeqcur:
        if (*buf != '=') goto out;
        value += 11;
        break;
      case '>':
        switch (*++buf) {
          case '=':
            value += ARITH_GE - '>';
            break;
          case '>':
            value += ARITH_RSHIFT - '>';
            goto checkeq;
          default:
            value += ARITH_GT - '>';
            goto out;
        }
        break;
      case '<':
        switch (*++buf) {
          case '=':
            value += ARITH_LE - '<';
            break;
          case '<':
            value += ARITH_LSHIFT - '<';
            goto checkeq;
          default:
            value += ARITH_LT - '<';
            goto out;
        }
        break;
      case '|':
        if (*++buf != '|') {
          value += ARITH_BOR - '|';
          goto checkeqcur;
        }
        value += ARITH_OR - '|';
        break;
      case '&':
        if (*++buf != '&') {
          value += ARITH_BAND - '&';
          goto checkeqcur;
        }
        value += ARITH_AND - '&';
        break;
      case '!':
        if (*++buf != '=') {
          value += ARITH_NOT - '!';
          goto out;
        }
        value += ARITH_NE - '!';
        break;
      case 0:
        goto out;
      case '(':
        value += ARITH_LPAREN - '(';
        break;
      case ')':
        value += ARITH_RPAREN - ')';
        break;
      case '*':
        value += ARITH_MUL - '*';
        goto checkeq;
      case '/':
        value += ARITH_DIV - '/';
        goto checkeq;
      case '%':
        value += ARITH_REM - '%';
        goto checkeq;
      case '+':
        value += ARITH_ADD - '+';
        goto checkeq;
      case '-':
        value += ARITH_SUB - '-';
        goto checkeq;
      case '~':
        value += ARITH_BNOT - '~';
        break;
      case '^':
        value += ARITH_BXOR - '^';
        goto checkeq;
      case '?':
        value += ARITH_QMARK - '?';
        break;
      case ':':
        value += ARITH_COLON - ':';
        break;
    }
    break;
  }
  buf++;
out:
  arith_buf = buf;
  return value;
}

/*
 * Compares two strings up to the first = or '\0'. The first string must
 * be terminated by '='; the second may be terminated by either '=' or
 * '\0'.
 */
static int varcmp(const char *p, const char *q) {
  int c, d;
  while ((c = *p) == (d = *q)) {
    if (!c || c == '=') goto out;
    p++;
    q++;
  }
  if (c == '=') c = 0;
  if (d == '=') d = 0;
out:
  return c - d;
}

static inline int varequal(const char *a, const char *b) {
  return !varcmp(a, b);
}

/*
 * Search the environment of a builtin command.
 */
static inline char *bltinlookup(const char *name) {
  return lookupvar(name);
}

static inline int arith_prec(int op) {
  return kPrec[op - ARITH_BINOP_MIN];
}

static inline int higher_prec(int op1, int op2) {
  return arith_prec(op1) < arith_prec(op2);
}

static int64_t do_binop(int op, int64_t a, int64_t b) {
  switch (op) {
    default:
    case ARITH_REM:
    case ARITH_DIV:
      if (!b) yyerror("division by zero");
      return op == ARITH_REM ? a % b : a / b;
    case ARITH_MUL:
      return a * b;
    case ARITH_ADD:
      return a + b;
    case ARITH_SUB:
      return a - b;
    case ARITH_LSHIFT:
      return a << b;
    case ARITH_RSHIFT:
      return a >> b;
    case ARITH_LT:
      return a < b;
    case ARITH_LE:
      return a <= b;
    case ARITH_GT:
      return a > b;
    case ARITH_GE:
      return a >= b;
    case ARITH_EQ:
      return a == b;
    case ARITH_NE:
      return a != b;
    case ARITH_BAND:
      return a & b;
    case ARITH_BXOR:
      return a ^ b;
    case ARITH_BOR:
      return a | b;
  }
}

static int64_t primary(int token, union yystype *val, int op, int noeval) {
  int64_t result;
again:
  switch (token) {
    case ARITH_LPAREN:
      result = assignment(op, noeval);
      if (last_token != ARITH_RPAREN) yyerror("expecting ')'");
      last_token = shlex();
      return result;
    case ARITH_NUM:
      last_token = op;
      return val->val;
    case ARITH_VAR:
      last_token = op;
      return noeval ? val->val : lookupvarint(val->name);
    case ARITH_ADD:
      token = op;
      *val = yylval;
      op = shlex();
      goto again;
    case ARITH_SUB:
      *val = yylval;
      return -primary(op, val, shlex(), noeval);
    case ARITH_NOT:
      *val = yylval;
      return !primary(op, val, shlex(), noeval);
    case ARITH_BNOT:
      *val = yylval;
      return ~primary(op, val, shlex(), noeval);
    default:
      yyerror("expecting primary");
  }
}

static int64_t binop2(int64_t a, int op, int prec, int noeval) {
  for (;;) {
    union yystype val;
    int64_t b;
    int op2;
    int token;
    token = shlex();
    val = yylval;
    b = primary(token, &val, shlex(), noeval);
    op2 = last_token;
    if (op2 >= ARITH_BINOP_MIN && op2 < ARITH_BINOP_MAX &&
        higher_prec(op2, op)) {
      b = binop2(b, op2, arith_prec(op), noeval);
      op2 = last_token;
    }
    a = noeval ? b : do_binop(op, a, b);
    if (op2 < ARITH_BINOP_MIN || op2 >= ARITH_BINOP_MAX ||
        arith_prec(op2) >= prec) {
      return a;
    }
    op = op2;
  }
}

static int64_t binop(int token, union yystype *val, int op, int noeval) {
  int64_t a = primary(token, val, op, noeval);
  op = last_token;
  if (op < ARITH_BINOP_MIN || op >= ARITH_BINOP_MAX) return a;
  return binop2(a, op, ARITH_MAX_PREC, noeval);
}

static int64_t and (int token, union yystype *val, int op, int noeval) {
  int64_t a = binop(token, val, op, noeval);
  int64_t b;
  op = last_token;
  if (op != ARITH_AND) return a;
  token = shlex();
  *val = yylval;
  b = and(token, val, shlex(), noeval | !a);
  return a && b;
}

static int64_t or (int token, union yystype *val, int op, int noeval) {
  int64_t a = and(token, val, op, noeval);
  int64_t b;
  op = last_token;
  if (op != ARITH_OR) return a;
  token = shlex();
  *val = yylval;
  b = or (token, val, shlex(), noeval | !!a);
  return a || b;
}

static int64_t cond(int token, union yystype *val, int op, int noeval) {
  int64_t a = or (token, val, op, noeval);
  int64_t b;
  int64_t c;
  if (last_token != ARITH_QMARK) return a;
  b = assignment(shlex(), noeval | !a);
  if (last_token != ARITH_COLON) yyerror("expecting ':'");
  token = shlex();
  *val = yylval;
  c = cond(token, val, shlex(), noeval | !!a);
  return a ? b : c;
}

static int64_t assignment(int var_, int noeval) {
  union yystype val = yylval;
  int op = shlex();
  int64_t result;
  if (var_ != ARITH_VAR) return cond(var_, &val, op, noeval);
  if (op != ARITH_ASS && (op < ARITH_ASS_MIN || op >= ARITH_ASS_MAX)) {
    return cond(var_, &val, op, noeval);
  }
  result = assignment(shlex(), noeval);
  if (noeval) return result;
  return setvarint(
      val.name,
      (op == ARITH_ASS ? result
                       : do_binop(op - 11, lookupvarint(val.name), result)),
      0);
}

static int64_t arith(const char *s) {
  int64_t result;
  arith_buf = arith_startbuf = s;
  result = assignment(shlex(), 0);
  if (last_token) yyerror("expecting EOF");
  return result;
}

/*
 * The cd and pwd commands.
 */
static inline int padvance(const char **path, const char *name) {
  return padvance_magic(path, name, 1);
}

static char *getpwd(void);
static const char *updatepwd(const char *);
static int cdopt(void);
static int docd(const char *, int);
static void setpwd(const char *, int);

static int cdopt() {
  int flags = 0;
  int i, j;
  j = 'L';
  while ((i = nextopt("LP"))) {
    if (i != j) {
      flags ^= CD_PHYSICAL;
      j = i;
    }
  }
  return flags;
}

static int cdcmd(int argc, char **argv) {
  const char *dest;
  const char *path;
  const char *p;
  char c;
  struct stat statb;
  int flags;
  int len;
  flags = cdopt();
  dest = *argptr;
  if (!dest)
    dest = bltinlookup(homestr);
  else if (dest[0] == '-' && dest[1] == '\0') {
    dest = bltinlookup("OLDPWD");
    flags |= CD_PRINT;
  }
  if (!dest) dest = nullstr;
  if (*dest == '/') goto step6;
  if (*dest == '.') {
    c = dest[1];
  dotdot:
    switch (c) {
      case '\0':
      case '/':
        goto step6;
      case '.':
        c = dest[2];
        if (c != '.') goto dotdot;
    }
  }
  if (!*dest) dest = ".";
  path = bltinlookup("CDPATH");
  while (p = path, (len = padvance_magic(&path, dest, 0)) >= 0) {
    c = *p;
    p = stalloc(len);
    if (stat(p, &statb) >= 0 && S_ISDIR(statb.st_mode)) {
      if (c && c != ':') flags |= CD_PRINT;
    docd:
      if (!docd(p, flags)) goto out;
      goto err;
    }
  }
step6:
  p = dest;
  goto docd;
err:
  sh_error("can't cd to %s", dest);
  __builtin_unreachable();
out:
  if (flags & CD_PRINT) out1fmt(snlfmt, curdir);
  return 0;
}

/*
 * Actually do the chdir.  We also call hashcd to let the routines in exec.c
 * know that the current directory has changed.
 */
static int docd(const char *dest, int flags) {
  const char *dir = 0;
  int err;
  TRACE(("docd(\"%s\", %d) called\n", dest, flags));
  INTOFF;
  if (!(flags & CD_PHYSICAL)) {
    dir = updatepwd(dest);
    if (dir) dest = dir;
  }
  err = chdir(dest);
  if (err) goto out;
  setpwd(dir, 1);
  hashcd();
out:
  INTON;
  return err;
}

/*
 * Update curdir (the name of the current directory) in response to a
 * cd command.
 */
static const char *updatepwd(const char *dir) {
  char *new;
  char *p;
  char *cdcomppath;
  const char *lim;
  cdcomppath = sstrdup(dir);
  STARTSTACKSTR(new);
  if (*dir != '/') {
    if (curdir == nullstr) return 0;
    new = stputs(curdir, new);
  }
  new = makestrspace(strlen(dir) + 2, new);
  lim = (char *)stackblock() + 1;
  if (*dir != '/') {
    if (new[-1] != '/') USTPUTC('/', new);
    if (new > lim && *lim == '/') lim++;
  } else {
    USTPUTC('/', new);
    cdcomppath++;
    if (dir[1] == '/' && dir[2] != '/') {
      USTPUTC('/', new);
      cdcomppath++;
      lim++;
    }
  }
  p = strtok(cdcomppath, "/");
  while (p) {
    switch (*p) {
      case '.':
        if (p[1] == '.' && p[2] == '\0') {
          while (new > lim) {
            STUNPUTC(new);
            if (new[-1] == '/') break;
          }
          break;
        } else if (p[1] == '\0')
          break;
        /* fall through */
      default:
        new = stputs(p, new);
        USTPUTC('/', new);
    }
    p = strtok(0, "/");
  }
  if (new > lim) STUNPUTC(new);
  *new = 0;
  return stackblock();
}

/*
 * Find out what the current directory is. If we already know the
 * current directory, this routine returns immediately.
 */
static inline char *getpwd() {
  char buf[PATH_MAX];
  if (getcwd(buf, sizeof(buf))) return savestr(buf);
  sh_warnx("getcwd() failed: %s", strerror(errno));
  return nullstr;
}

static int pwdcmd(int argc, char **argv) {
  int flags;
  const char *dir = curdir;
  flags = cdopt();
  if (flags) {
    if (physdir == nullstr) setpwd(dir, 0);
    dir = physdir;
  }
  out1fmt(snlfmt, dir);
  return 0;
}

static void setpwd(const char *val, int setold) {
  char *oldcur, *dir;
  oldcur = dir = curdir;
  if (setold) {
    setvar("OLDPWD", oldcur, VEXPORT);
  }
  INTOFF;
  if (physdir != nullstr) {
    if (physdir != oldcur) free(physdir);
    physdir = nullstr;
  }
  if (oldcur == val || !val) {
    char *s = getpwd();
    physdir = s;
    if (!val) dir = s;
  } else
    dir = savestr(val);
  if (oldcur != dir && oldcur != nullstr) {
    free(oldcur);
  }
  curdir = dir;
  INTON;
  setvar("PWD", dir, VEXPORT);
}

/*
 * Errors and exceptions.
 */

/*
 * NEOF is returned by parsecmd when it encounters an end of file.  It
 * must be distinct from NULL, so we use the address of a variable that
 * happens to be handy.
 */
#define NEOF ((union node *)&tokpushback)

/*
 * Return of a legal variable name (a letter or underscore followed by zero or
 * more letters, underscores, and digits).
 */
static char *endofname(const char *name) {
  char *p;
  p = (char *)name;
  if (!is_name(*p)) return p;
  while (*++p) {
    if (!is_in_name(*p)) break;
  }
  return p;
}

static inline int goodname(const char *p) {
  return !*endofname(p);
}
static inline int parser_eof(void) {
  return tokpushback && lasttoken == TEOF;
}
static inline int have_traps(void) {
  return trapcnt;
}

static const struct builtincmd kBltin = {
    .name = nullstr,
    .builtin = bltincmd,
    .flags = BUILTIN_REGULAR,
};

/*
 * Evaluate a parse tree.  The value is left in the global variable
 * exitstatus.
 */
static int evaltree(union node *n, int flags) {
  int checkexit = 0;
  int (*evalfn)(union node *, int);
  struct stackmark smark;
  unsigned isor;
  int status = 0;
  setstackmark(&smark);
  if (nflag) goto out;
  if (n == NULL) {
    TRACE(("evaltree(NULL) called\n"));
    goto out;
  }
  dotrap();
  TRACE(("pid %d, evaltree(%p: %d, %d) called\n", getpid(), n, n->type, flags));
  switch (n->type) {
    default:
    case NNOT:
      status = !evaltree(n->nnot.com, EV_TESTED);
      goto setstatus;
    case NREDIR:
      errlinno = lineno = n->nredir.linno;
      if (funcline) lineno -= funcline - 1;
      expredir(n->nredir.redirect);
      pushredir(n->nredir.redirect);
      status = (redirectsafe(n->nredir.redirect, REDIR_PUSH)
                    ?: evaltree(n->nredir.n, flags & EV_TESTED));
      if (n->nredir.redirect) popredir(0);
      goto setstatus;
    case NCMD:
      evalfn = evalcommand;
    checkexit:
      checkexit = ~flags & EV_TESTED;
      goto calleval;
    case NFOR:
      evalfn = evalfor;
      goto calleval;
    case NWHILE:
    case NUNTIL:
      evalfn = evalloop;
      goto calleval;
    case NSUBSHELL:
    case NBACKGND:
      evalfn = evalsubshell;
      goto checkexit;
    case NPIPE:
      evalfn = evalpipe;
      goto checkexit;
    case NCASE:
      evalfn = evalcase;
      goto calleval;
    case NAND:
    case NOR:
    case NSEMI:
      isor = n->type - NAND;
      status =
          evaltree(n->nbinary.ch1, (flags | ((isor >> 1) - 1)) & EV_TESTED);
      if ((!status) == isor || evalskip) break;
      n = n->nbinary.ch2;
    evaln:
      evalfn = evaltree;
    calleval:
      status = evalfn(n, flags);
      goto setstatus;
    case NIF:
      status = evaltree(n->nif.test, EV_TESTED);
      if (evalskip) break;
      if (!status) {
        n = n->nif.ifpart;
        goto evaln;
      } else if (n->nif.elsepart) {
        n = n->nif.elsepart;
        goto evaln;
      }
      status = 0;
      goto setstatus;
    case NDEFUN:
      defun(n);
    setstatus:
      exitstatus = status;
      break;
  }
out:
  dotrap();
  if (eflag && checkexit & status) goto exexit;
  if (flags & EV_EXIT) {
  exexit:
    exraise(EXEND);
  }
  popstackmark(&smark);
  return exitstatus;
}

wontreturn static void evaltreenr(union node *n, int flags) {
  evaltree(n, flags);
  abort();
}

/*
 * Execute a command or commands contained in a string.
 */
static int evalstring(char *s, int flags) {
  union node *n;
  struct stackmark smark;
  int status;
  s = sstrdup(s);
  setinputstring(s);
  setstackmark(&smark);
  status = 0;
  for (; (n = parsecmd(0)) != NEOF; popstackmark(&smark)) {
    int i;
    i = evaltree(n, flags & ~(parser_eof() ? 0 : EV_EXIT));
    if (n) status = i;
    if (evalskip) break;
  }
  popstackmark(&smark);
  popfile();
  stunalloc(s);
  return status;
}

static int evalcmd(int argc, char **argv, int flags) {
  char *p;
  char *concat;
  char **ap;
  if (argc > 1) {
    p = argv[1];
    if (argc > 2) {
      STARTSTACKSTR(concat);
      ap = argv + 2;
      for (;;) {
        concat = stputs(p, concat);
        if ((p = *ap++) == NULL) break;
        STPUTC(' ', concat);
      }
      STPUTC('\0', concat);
      p = grabstackstr(concat);
    }
    return evalstring(p, flags & EV_TESTED);
  }
  return 0;
}

static int skiploop(void) {
  int skip = evalskip;
  switch (skip) {
    case 0:
      break;
    case SKIPBREAK:
    case SKIPCONT:
      if (likely(--skipcount <= 0)) {
        evalskip = 0;
        break;
      }
      skip = SKIPBREAK;
      break;
  }
  return skip;
}

static int evalloop(union node *n, int flags) {
  int skip;
  int status;
  loopnest++;
  status = 0;
  flags &= EV_TESTED;
  do {
    int i;
    i = evaltree(n->nbinary.ch1, EV_TESTED);
    skip = skiploop();
    if (skip == SKIPFUNC) status = i;
    if (skip) continue;
    if (n->type != NWHILE) i = !i;
    if (i != 0) break;
    status = evaltree(n->nbinary.ch2, flags);
    skip = skiploop();
  } while (!(skip & ~SKIPCONT));
  loopnest--;
  return status;
}

static int evalfor(union node *n, int flags) {
  struct arglist arglist;
  union node *argp;
  struct strlist *sp;
  int status;
  errlinno = lineno = n->nfor.linno;
  if (funcline) lineno -= funcline - 1;
  arglist.lastp = &arglist.list;
  for (argp = n->nfor.args; argp; argp = argp->narg.next) {
    expandarg(argp, &arglist, EXP_FULL | EXP_TILDE);
  }
  *arglist.lastp = NULL;
  status = 0;
  loopnest++;
  flags &= EV_TESTED;
  for (sp = arglist.list; sp; sp = sp->next) {
    setvar(n->nfor.var_, sp->text, 0);
    status = evaltree(n->nfor.body, flags);
    if (skiploop() & ~SKIPCONT) break;
  }
  loopnest--;
  return status;
}

/*
 * See if a pattern matches in a case statement.
 */
static int casematch(union node *pattern, char *val) {
  struct stackmark smark;
  int result;
  setstackmark(&smark);
  argbackq = pattern->narg.backquote;
  STARTSTACKSTR(expdest);
  argstr(pattern->narg.text, EXP_TILDE | EXP_CASE);
  ifsfree();
  result = patmatch(stackblock(), val);
  popstackmark(&smark);
  return result;
}

static int evalcase(union node *n, int flags) {
  union node *cp;
  union node *patp;
  struct arglist arglist;
  int status = 0;
  errlinno = lineno = n->ncase.linno;
  if (funcline) lineno -= funcline - 1;
  arglist.lastp = &arglist.list;
  expandarg(n->ncase.expr, &arglist, EXP_TILDE);
  for (cp = n->ncase.cases; cp && evalskip == 0; cp = cp->nclist.next) {
    for (patp = cp->nclist.pattern; patp; patp = patp->narg.next) {
      if (casematch(patp, arglist.list->text)) {
        /* Ensure body is non-empty as otherwise
         * EV_EXIT may prevent us from setting the
         * exit status.
         */
        if (evalskip == 0 && cp->nclist.body) {
          status = evaltree(cp->nclist.body, flags);
        }
        goto out;
      }
    }
  }
out:
  return status;
}

/*
 * Kick off a subshell to evaluate a tree.
 */
static int evalsubshell(union node *n, int flags) {
  struct job *jp;
  int backgnd = (n->type == NBACKGND);
  int status;
  errlinno = lineno = n->nredir.linno;
  if (funcline) lineno -= funcline - 1;
  expredir(n->nredir.redirect);
  INTOFF;
  if (!backgnd && flags & EV_EXIT && !have_traps()) {
    forkreset();
    goto nofork;
  }
  jp = makejob(n, 1);
  if (forkshell(jp, n, backgnd) == 0) {
    flags |= EV_EXIT;
    if (backgnd) flags &= ~EV_TESTED;
  nofork:
    INTON;
    redirect(n->nredir.redirect, 0);
    evaltreenr(n->nredir.n, flags);
    /* never returns */
  }
  status = 0;
  if (!backgnd) status = waitforjob(jp);
  INTON;
  return status;
}

/*
 * Compute the names of the files in a redirection list.
 */
static void expredir(union node *n) {
  union node *redir;
  for (redir = n; redir; redir = redir->nfile.next) {
    struct arglist fn;
    fn.lastp = &fn.list;
    switch (redir->type) {
      case NFROMTO:
      case NFROM:
      case NTO:
      case NCLOBBER:
      case NAPPEND:
        expandarg(redir->nfile.fname, &fn, EXP_TILDE | EXP_REDIR);
        redir->nfile.expfname = fn.list->text;
        break;
      case NFROMFD:
      case NTOFD:
        if (redir->ndup.vname) {
          expandarg(redir->ndup.vname, &fn, EXP_TILDE | EXP_REDIR);
          fixredir(redir, fn.list->text, 1);
        }
        break;
    }
  }
}

/*
 * Evaluate a pipeline.  All the processes in the pipeline are children
 * of the process creating the pipeline.  (This differs from some versions
 * of the shell, which make the last process in a pipeline the parent
 * of all the rest.)
 */
static int evalpipe(union node *n, int flags) {
  struct job *jp;
  struct nodelist *lp;
  int pipelen;
  int prevfd;
  int pip[2];
  int status = 0;
  TRACE(("evalpipe(0x%lx) called\n", (long)n));
  pipelen = 0;
  for (lp = n->npipe.cmdlist; lp; lp = lp->next) pipelen++;
  flags |= EV_EXIT;
  INTOFF;
  jp = makejob(n, pipelen);
  prevfd = -1;
  for (lp = n->npipe.cmdlist; lp; lp = lp->next) {
    prehash(lp->n);
    pip[1] = -1;
    if (lp->next) {
      if (pipe(pip) < 0) {
        close(prevfd);
        sh_error("Pipe call failed");
      }
    }
    if (forkshell(jp, lp->n, n->npipe.backgnd) == 0) {
      INTON;
      if (pip[1] >= 0) {
        close(pip[0]);
      }
      if (prevfd > 0) {
        dup2(prevfd, 0);
        close(prevfd);
      }
      if (pip[1] > 1) {
        dup2(pip[1], 1);
        close(pip[1]);
      }
      evaltreenr(lp->n, flags);
      /* never returns */
    }
    if (prevfd >= 0) close(prevfd);
    prevfd = pip[0];
    close(pip[1]);
  }
  if (n->npipe.backgnd == 0) {
    status = waitforjob(jp);
    TRACE(("evalpipe:  job done exit status %d\n", status));
  }
  INTON;
  return status;
}

/*
 * Execute a command inside back quotes.  If it's a builtin command, we
 * want to save its output in a block obtained from malloc.  Otherwise
 * we fork off a subprocess and get the output of the command via a pipe.
 * Should be called with interrupts off.
 */
static void evalbackcmd(union node *n, struct backcmd *result) {
  int pip[2];
  struct job *jp;
  result->fd = -1;
  result->buf = NULL;
  result->nleft = 0;
  result->jp = NULL;
  if (n == NULL) {
    goto out;
  }
  if (pipe(pip) < 0) sh_error("Pipe call failed");
  jp = makejob(n, 1);
  if (forkshell(jp, n, FORK_NOJOB) == 0) {
    FORCEINTON;
    close(pip[0]);
    if (pip[1] != 1) {
      dup2(pip[1], 1);
      close(pip[1]);
    }
    ifsfree();
    evaltreenr(n, EV_EXIT);
    __builtin_unreachable();
  }
  close(pip[1]);
  result->fd = pip[0];
  result->jp = jp;
out:
  TRACE(("evalbackcmd done: fd=%d buf=0x%x nleft=%d jp=0x%x\n", result->fd,
         result->buf, result->nleft, result->jp));
}

static struct strlist *fill_arglist(struct arglist *arglist,
                                    union node **argpp) {
  struct strlist **lastp = arglist->lastp;
  union node *argp;
  while ((argp = *argpp)) {
    expandarg(argp, arglist, EXP_FULL | EXP_TILDE);
    *argpp = argp->narg.next;
    if (*lastp) break;
  }
  return *lastp;
}

static int parse_command_args(struct arglist *arglist, union node **argpp,
                              const char **path) {
  struct strlist *sp = arglist->list;
  char *cp, c;
  for (;;) {
    sp = unlikely(sp->next != NULL) ? sp->next : fill_arglist(arglist, argpp);
    if (!sp) return 0;
    cp = sp->text;
    if (*cp++ != '-') break;
    if (!(c = *cp++)) break;
    if (c == '-' && !*cp) {
      if (likely(!sp->next) && !fill_arglist(arglist, argpp)) return 0;
      sp = sp->next;
      break;
    }
    do {
      switch (c) {
        case 'p':
          *path = defpath;
          break;
        default:
          /* run 'typecmd' for other options */
          return 0;
      }
    } while ((c = *cp++));
  }
  arglist->list = sp;
  return DO_NOFUNC;
}

/*
 * Execute a simple command.
 */
static int evalcommand(union node *cmd, int flags) {
  struct localvar_list *localvar_stop;
  struct parsefile *file_stop;
  struct redirtab *redir_stop;
  union node *argp;
  struct arglist arglist;
  struct arglist varlist;
  char **argv;
  int argc;
  struct strlist *osp;
  struct strlist *sp;
  struct cmdentry cmdentry;
  struct job *jp;
  char *lastarg;
  const char *path;
  int spclbltin;
  int cmd_flag;
  int execcmd;
  int status;
  char **nargv;
  int vflags;
  int vlocal;
  errlinno = lineno = cmd->ncmd.linno;
  if (funcline) lineno -= funcline - 1;
  /* First expand the arguments. */
  TRACE(("evalcommand(0x%lx, %d) called\n", (long)cmd, flags));
  file_stop = parsefile;
  back_exitstatus = 0;
  cmdentry.cmdtype = CMDBUILTIN;
  cmdentry.u.cmd = &kBltin;
  varlist.lastp = &varlist.list;
  *varlist.lastp = NULL;
  arglist.lastp = &arglist.list;
  *arglist.lastp = NULL;
  cmd_flag = 0;
  execcmd = 0;
  spclbltin = -1;
  vflags = 0;
  vlocal = 0;
  path = NULL;
  argc = 0;
  argp = cmd->ncmd.args;
  if ((osp = fill_arglist(&arglist, &argp))) {
    int pseudovarflag = 0;
    for (;;) {
      find_command(arglist.list->text, &cmdentry, cmd_flag | DO_REGBLTIN,
                   pathval());
      vlocal++;
      /* implement bltin and command here */
      if (cmdentry.cmdtype != CMDBUILTIN) break;
      pseudovarflag = cmdentry.u.cmd->flags & BUILTIN_ASSIGN;
      if (likely(spclbltin < 0)) {
        spclbltin = cmdentry.u.cmd->flags & BUILTIN_SPECIAL;
        vlocal = spclbltin ^ BUILTIN_SPECIAL;
      }
      execcmd = cmdentry.u.cmd == EXECCMD;
      if (likely(cmdentry.u.cmd != COMMANDCMD)) break;
      cmd_flag = parse_command_args(&arglist, &argp, &path);
      if (!cmd_flag) break;
    }
    for (; argp; argp = argp->narg.next) {
      expandarg(argp, &arglist,
                (pseudovarflag && isassignment(argp->narg.text))
                    ? EXP_VARTILDE
                    : EXP_FULL | EXP_TILDE);
    }
    for (sp = arglist.list; sp; sp = sp->next) argc++;
    if (execcmd && argc > 1) vflags = VEXPORT;
  }
  localvar_stop = pushlocalvars(vlocal);
  /* Reserve one extra spot at the front for shellexec. */
  nargv = stalloc(sizeof(char *) * (argc + 2));
  argv = ++nargv;
  for (sp = arglist.list; sp; sp = sp->next) {
    TRACE(("evalcommand arg: %s\n", sp->text));
    *nargv++ = sp->text;
  }
  *nargv = NULL;
  lastarg = NULL;
  if (iflag && funcline == 0 && argc > 0) lastarg = nargv[-1];
  preverrout.fd = 2;
  expredir(cmd->ncmd.redirect);
  redir_stop = pushredir(cmd->ncmd.redirect);
  status = redirectsafe(cmd->ncmd.redirect, REDIR_PUSH | REDIR_SAVEFD2);
  if (unlikely(status)) {
  bail:
    exitstatus = status;
    /* We have a redirection error. */
    if (spclbltin > 0) exraise(EXERROR);
    goto out;
  }
  for (argp = cmd->ncmd.assign; argp; argp = argp->narg.next) {
    struct strlist **spp;
    spp = varlist.lastp;
    expandarg(argp, &varlist, EXP_VARTILDE);
    if (vlocal)
      mklocal((*spp)->text, VEXPORT);
    else
      setvareq((*spp)->text, vflags);
  }
  /* Print the command if xflag is set. */
  if (xflag && !inps4) {
    struct output *out;
    int sep;
    out = &preverrout;
    inps4 = 1;
    outstr(expandstr(ps4val()), out);
    inps4 = 0;
    sep = 0;
    sep = eprintlist(out, varlist.list, sep);
    eprintlist(out, osp, sep);
    outcslow('\n', out);
  }
  /* Now locate the command. */
  if (cmdentry.cmdtype != CMDBUILTIN ||
      !(cmdentry.u.cmd->flags & BUILTIN_REGULAR)) {
    path = unlikely(path != NULL) ? path : pathval(); /* wut */
    find_command(argv[0], &cmdentry, cmd_flag | DO_ERR, path);
  }
  jp = NULL;
  /* Execute the command. */
  switch (cmdentry.cmdtype) {
    case CMDUNKNOWN:
      status = 127;
      goto bail;
    default:
      /* Fork off a child process if necessary. */
      if (!(flags & EV_EXIT) || have_traps()) {
        INTOFF;
        jp = vforkexec(cmd, argv, path, cmdentry.u.index);
        break;
      }
      shellexec(argv, path, cmdentry.u.index);
      __builtin_unreachable();
    case CMDBUILTIN:
      if (evalbltin(cmdentry.u.cmd, argc, argv, flags) &&
          !(exception == EXERROR && spclbltin <= 0)) {
      raise:
        longjmp(handler->loc, 1);
      }
      break;
    case CMDFUNCTION:
      if (evalfun(cmdentry.u.func, argc, argv, flags)) goto raise;
      break;
  }
  status = waitforjob(jp);
  FORCEINTON;
out:
  if (cmd->ncmd.redirect) popredir(execcmd);
  unwindredir(redir_stop);
  unwindfiles(file_stop);
  unwindlocalvars(localvar_stop);
  if (lastarg) {
    /* dsl: I think this is intended to be used to support
     * '_' in 'vi' command mode during line editing...
     * However I implemented that within libedit itself.
     */
    setvar("_", lastarg, 0);
  }
  return status;
}

static int evalbltin(const struct builtincmd *cmd, int argc, char **argv,
                     int flags) {
  char *volatile savecmdname;
  struct jmploc *volatile savehandler;
  struct jmploc jmploc;
  int status;
  int i;

  savecmdname = commandname;
  savehandler = handler;
  if ((i = setjmp(jmploc.loc))) goto cmddone;
  handler = &jmploc;
  commandname = argv[0];
  argptr = argv + 1;
  optptr = NULL; /* initialize nextopt */
  if (cmd == EVALCMD)
    status = evalcmd(argc, argv, flags);
  else
    status = (*cmd->builtin)(argc, argv);
  flushall();
  if (out1->flags) sh_warnx("%s: I/O error", commandname);
  status |= out1->flags;
  exitstatus = status;
cmddone:
  freestdout();
  commandname = savecmdname;
  handler = savehandler;

  return i;
}

/*
 * Free a parse tree.
 */
static void freefunc(struct funcnode *f) {
  if (f && --f->count < 0) ckfree(f);
}

static int evalfun(struct funcnode *func, int argc, char **argv, int flags) {
  volatile struct shparam saveparam;
  struct jmploc *volatile savehandler;
  struct jmploc jmploc;
  int e;
  int savefuncline;
  int saveloopnest;
  saveparam = shellparam;
  savefuncline = funcline;
  saveloopnest = loopnest;
  savehandler = handler;
  if ((e = setjmp(jmploc.loc))) {
    goto funcdone;
  }
  INTOFF;
  handler = &jmploc;
  shellparam.malloc = 0;
  func->count++;
  funcline = func->n.ndefun.linno;
  loopnest = 0;
  INTON;
  shellparam.nparam = argc - 1;
  shellparam.p = argv + 1;
  shellparam.optind = 1;
  shellparam.optoff = -1;
  evaltree(func->n.ndefun.body, flags & EV_TESTED);
funcdone:
  INTOFF;
  loopnest = saveloopnest;
  funcline = savefuncline;
  freefunc(func);
  freeparam(&shellparam);
  shellparam = saveparam;
  handler = savehandler;
  INTON;
  evalskip &= ~(SKIPFUNC | SKIPFUNCDEF);
  return e;
}

/*
 * Search for a command.  This is called before we fork so that the
 * location of the command will be available in the parent as well as
 * the child.  The check for "goodname" is an overly conservative
 * check that the name will not be subject to expansion.
 */
static void prehash(union node *n) {
  struct cmdentry entry;
  if (n->type == NCMD && n->ncmd.args) {
    if (goodname(n->ncmd.args->narg.text)) {
      find_command(n->ncmd.args->narg.text, &entry, 0, pathval());
    }
  }
}

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § the unbourne shell » builtins                             ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│─╝
  Builtin commands whose functions are closely tied to evaluation are
  implemented here. */

static int falsecmd(int argc, char **argv) {
  return 1;
}

static int truecmd(int argc, char **argv) {
  return 0;
}

/* No command given. */
static int bltincmd(int argc, char **argv) {
  /*
   * Preserve exitstatus of a previous possible redirection
   * as POSIX mandates
   */
  return back_exitstatus;
}

/*
 * Handle break and continue commands.  Break, continue, and return are
 * all handled by setting the evalskip flag.  The evaluation routines
 * above all check this flag, and if it is set they start skipping
 * commands rather than executing them.  The variable skipcount is
 * the number of loops to break/continue, or the number of function
 * levels to return.  (The latter is always 1.)  It should probably
 * be an error to break out of more loops than exist, but it isn't
 * in the standard shell so we don't make it one here.
 */
static int breakcmd(int argc, char **argv) {
  int n = argc > 1 ? number(argv[1]) : 1;
  if (n <= 0) badnum(argv[1]);
  if (n > loopnest) n = loopnest;
  if (n > 0) {
    evalskip = (**argv == 'c') ? SKIPCONT : SKIPBREAK;
    skipcount = n;
  }
  return 0;
}

/* The return command. */
static int returncmd(int argc, char **argv) {
  int skip;
  int status;
  /*
   * If called outside a function, do what ksh does;
   * skip the rest of the file.
   */
  if (argv[1]) {
    skip = SKIPFUNC;
    status = number(argv[1]);
  } else {
    skip = SKIPFUNCDEF;
    status = exitstatus;
  }
  evalskip = skip;
  return status;
}

static int execcmd(int argc, char **argv) {
  if (argc > 1) {
    iflag = 0; /* exit on error */
    mflag = 0;
    optschanged();
    shellexec(argv + 1, pathval(), 0);
  }
  return 0;
}

static int eprintlist(struct output *out, struct strlist *sp, int sep) {
  while (sp) {
    const char *p;
    p = &" %s"[!sep]; /* XXX: -Wstring-plus-int: p = " %s" + (1 - sep); */
    sep |= 1;
    outfmt(out, p, sp->text);
    sp = sp->next;
  }
  return sep;
}

/*
 * When commands are first encountered, they are entered in a hash table.
 * This ensures that a full path search will not have to be done for them
 * on each invocation.
 *
 * We should investigate converting to a linear search, even though that
 * would make the command name "hash" a misnomer.
 */

/*
 * Exec a program.  Never returns.  If you change this routine, you may
 * have to change the find_command routine as well.
 */
wontreturn static void shellexec(char **argv, const char *path, int idx) {
  char *cmdname;
  int e;
  char **envp;
  int exerrno;
  envp = environment();
  if (strchr(argv[0], '/') != NULL) {
    tryexec(argv[0], argv, envp);
    e = errno;
  } else {
    e = ENOENT;
    while (padvance(&path, argv[0]) >= 0) {
      cmdname = stackblock();
      if (--idx < 0 && pathopt == NULL) {
        tryexec(cmdname, argv, envp);
        if (errno != ENOENT && errno != ENOTDIR) e = errno;
      }
    }
  }
  /* Map to POSIX errors */
  exerrno = (e == ELOOP || e == ENAMETOOLONG || e == ENOENT || e == ENOTDIR)
                ? 127
                : 126;
  exitstatus = exerrno;
  TRACE(("shellexec failed for %s, errno %d, suppressint %d\n", argv[0], e,
         suppressint));
  exerror(EXEND, "%s: %s", argv[0], errmsg(e, E_EXEC));
}

static void tryexec(char *cmd, char **argv, char **envp) {
  char *const path_bshell = _PATH_BSHELL;
repeat:
  execve(cmd, argv, envp);
  if (cmd != path_bshell && errno == ENOEXEC) {
    *argv-- = cmd;
    *argv = cmd = path_bshell;
    goto repeat;
  }
}

static const char *legal_pathopt(const char *opt, const char *term, int magic) {
  switch (magic) {
    case 0:
      opt = NULL;
      break;
    case 1:
      opt = prefix(opt, "builtin") ?: prefix(opt, "func");
      break;
    default:
      opt += strcspn(opt, term);
      break;
  }
  if (opt && *opt == '%') opt++;
  return opt;
}

/*
 * Do a path search.  The variable path (passed by reference) should be
 * set to the start of the path before the first call; padvance will update
 * this value as it proceeds.  Successive calls to padvance will return
 * the possible path expansions in sequence.  If an option (indicated by
 * a percent sign) appears in the path entry then the global variable
 * pathopt will be set to point to it; otherwise pathopt will be set to
 * NULL.
 *
 * If magic is 0 then pathopt recognition will be disabled.  If magic is
 * 1 we shall recognise %builtin/%func.  Otherwise we shall accept any
 * pathopt.
 */
static int padvance_magic(const char **path, const char *name, int magic) {
  const char *term = "%:";
  const char *lpathopt;
  const char *p;
  char *q;
  const char *start;
  unsigned qlen;
  unsigned len;
  if (*path == NULL) return -1;
  lpathopt = NULL;
  start = *path;
  if (*start == '%' && (p = legal_pathopt(start + 1, term, magic))) {
    lpathopt = start + 1;
    start = p;
    term = ":";
  }
  len = strcspn(start, term);
  p = start + len;
  if (*p == '%') {
    unsigned extra = strchrnul(p, ':') - p;
    if (legal_pathopt(p + 1, term, magic))
      lpathopt = p + 1;
    else
      len += extra;
    p += extra;
  }
  pathopt = lpathopt;
  *path = *p == ':' ? p + 1 : NULL;
  /* "2" is for '/' and '\0' */
  qlen = len + strlen(name) + 2;
  q = growstackto(qlen);
  if (likely(len)) {
    q = mempcpy(q, start, len);
    *q++ = '/';
  }
  strcpy(q, name);
  return qlen;
}

/*** Command hashing code ***/
static int hashcmd(int argc, char **argv) {
  struct tblentry **pp;
  struct tblentry *cmdp;
  int c;
  struct cmdentry entry;
  char *name;
  while ((c = nextopt("r")) != '\0') {
    clearcmdentry();
    return 0;
  }
  if (*argptr == NULL) {
    for (pp = cmdtable; pp < &cmdtable[CMDTABLESIZE]; pp++) {
      for (cmdp = *pp; cmdp; cmdp = cmdp->next) {
        if (cmdp->cmdtype == CMDNORMAL) {
          printentry(cmdp);
        }
      }
    }
    return 0;
  }
  c = 0;
  while ((name = *argptr) != NULL) {
    if ((cmdp = cmdlookup(name, 0)) &&
        (cmdp->cmdtype == CMDNORMAL ||
         (cmdp->cmdtype == CMDBUILTIN &&
          !(cmdp->param.cmd->flags & BUILTIN_REGULAR) && builtinloc > 0))) {
      delete_cmd_entry();
    }
    find_command(name, &entry, DO_ERR, pathval());
    if (entry.cmdtype == CMDUNKNOWN) c = 1;
    argptr++;
  }
  return c;
}

static void printentry(struct tblentry *cmdp) {
  int idx;
  const char *path;
  char *name;
  idx = cmdp->param.index;
  path = pathval();
  do {
    padvance(&path, cmdp->cmdname);
  } while (--idx >= 0);
  name = stackblock();
  outstr(name, out1);
  out1fmt(snlfmt, cmdp->rehash ? "*" : nullstr);
}

static int cmdloop(int top);

/*
 * Read a file containing shell functions.
 */
static void readcmdfile(char *name) {
  setinputfile(name, INPUT_PUSH_FILE);
  cmdloop(0);
  popfile();
}

/*
 * Search the table of builtin commands.
 */
static struct builtincmd *find_builtin(const char *name) {
  return bsearch(&name, kBuiltinCmds,
                 sizeof(kBuiltinCmds) / sizeof(kBuiltinCmds[0]),
                 sizeof(kBuiltinCmds[0]), pstrcmp);
}

/*
 * Resolve a command name.  If you change this routine, you may have to
 * change the shellexec routine as well.
 */
static void find_command(char *name, struct cmdentry *entry, int act,
                         const char *path) {
  char *fullname;
  struct stat statb;
  struct tblentry *cmdp;
  struct builtincmd *bcmd;
  int e, bit, idx, prev, updatetbl, len;
  /* If name contains a slash, don't use PATH or hash table */
  if (strchr(name, '/') != NULL) {
    entry->u.index = -1;
    if (act & DO_ABS) {
      while (stat(name, &statb) < 0) {
        entry->cmdtype = CMDUNKNOWN;
        return;
      }
    }
    entry->cmdtype = CMDNORMAL;
    return;
  }
  updatetbl = (path == pathval());
  if (!updatetbl) act |= DO_ALTPATH;
  /* If name is in the table, check answer will be ok */
  if ((cmdp = cmdlookup(name, 0)) != NULL) {
    switch (cmdp->cmdtype) {
      default:
      case CMDNORMAL:
        bit = DO_ALTPATH | DO_REGBLTIN;
        break;
      case CMDFUNCTION:
        bit = DO_NOFUNC;
        break;
      case CMDBUILTIN:
        bit = cmdp->param.cmd->flags & BUILTIN_REGULAR ? 0 : DO_REGBLTIN;
        break;
    }
    if (act & bit) {
      if (act & bit & DO_REGBLTIN) goto fail;
      updatetbl = 0;
      cmdp = NULL;
    } else if (cmdp->rehash == 0) {
      /* if not invalidated by cd, we're done */
      goto success;
    }
  }
  /* If %builtin not in path, check for builtin next */
  bcmd = find_builtin(name);
  if (bcmd && ((bcmd->flags & BUILTIN_REGULAR) | (act & DO_ALTPATH) |
               (builtinloc <= 0))) {
    goto builtin_success;
  }
  if (act & DO_REGBLTIN) goto fail;
  /* We have to search path. */
  prev = -1;                  /* where to start */
  if (cmdp && cmdp->rehash) { /* doing a rehash */
    if (cmdp->cmdtype == CMDBUILTIN)
      prev = builtinloc;
    else
      prev = cmdp->param.index;
  }
  e = ENOENT;
  idx = -1;
loop:
  while ((len = padvance(&path, name)) >= 0) {
    const char *lpathopt = pathopt;
    fullname = stackblock();
    idx++;
    if (lpathopt) {
      if (*lpathopt == 'b') {
        if (bcmd) goto builtin_success;
        continue;
      } else if (!(act & DO_NOFUNC)) {
        /* handled below */
      } else {
        /* ignore unimplemented options */
        continue;
      }
    }
    /* if rehash, don't redo absolute path names */
    if (fullname[0] == '/' && idx <= prev) {
      if (idx < prev) continue;
      TRACE(("searchexec \"%s\": no change\n", name));
      goto success;
    }
    while (stat(fullname, &statb) < 0) {
      if (errno != ENOENT && errno != ENOTDIR) e = errno;
      goto loop;
    }
    e = EACCES; /* if we fail, this will be the error */
    if (!S_ISREG(statb.st_mode)) continue;
    if (lpathopt) { /* this is a %func directory */
      stalloc(len);
      readcmdfile(fullname);
      if ((cmdp = cmdlookup(name, 0)) == NULL || cmdp->cmdtype != CMDFUNCTION) {
        sh_error("%s not defined in %s", name, fullname);
      }
      stunalloc(fullname);
      goto success;
    }
    TRACE(("searchexec \"%s\" returns \"%s\"\n", name, fullname));
    if (!updatetbl) {
      entry->cmdtype = CMDNORMAL;
      entry->u.index = idx;
      return;
    }
    INTOFF;
    cmdp = cmdlookup(name, 1);
    cmdp->cmdtype = CMDNORMAL;
    cmdp->param.index = idx;
    INTON;
    goto success;
  }
  /* We failed.  If there was an entry for this command, delete it */
  if (cmdp && updatetbl) delete_cmd_entry();
  if (act & DO_ERR) sh_warnx("%s: %s", name, errmsg(e, E_EXEC));
fail:
  entry->cmdtype = CMDUNKNOWN;
  return;
builtin_success:
  if (!updatetbl) {
    entry->cmdtype = CMDBUILTIN;
    entry->u.cmd = bcmd;
    return;
  }
  INTOFF;
  cmdp = cmdlookup(name, 1);
  cmdp->cmdtype = CMDBUILTIN;
  cmdp->param.cmd = bcmd;
  INTON;
success:
  cmdp->rehash = 0;
  entry->cmdtype = cmdp->cmdtype;
  entry->u = cmdp->param;
}

/*
 * Called when a cd is done.  Marks all commands so the next time they
 * are executed they will be rehashed.
 */
static void hashcd(void) {
  struct tblentry **pp;
  struct tblentry *cmdp;
  for (pp = cmdtable; pp < &cmdtable[CMDTABLESIZE]; pp++) {
    for (cmdp = *pp; cmdp; cmdp = cmdp->next) {
      if (cmdp->cmdtype == CMDNORMAL ||
          (cmdp->cmdtype == CMDBUILTIN &&
           !(cmdp->param.cmd->flags & BUILTIN_REGULAR) && builtinloc > 0)) {
        cmdp->rehash = 1;
      }
    }
  }
}

/*
 * Fix command hash table when PATH changed.
 * Called before PATH is changed.  The argument is the new value of PATH;
 * pathval() still returns the old value at this point.
 * Called with interrupts off.
 */
static void changepath(const char *newval) {
  int idx;
  int bltin;
  const char *neu;
  neu = newval;
  idx = 0;
  bltin = -1;
  for (;;) {
    if (*neu == '%' && prefix(neu + 1, "builtin")) {
      bltin = idx;
      break;
    }
    neu = strchr(neu, ':');
    if (!neu) break;
    idx++;
    neu++;
  }
  builtinloc = bltin;
  clearcmdentry();
}

/*
 * Clear out command entries.  The argument specifies the first entry in
 * PATH which has changed.
 */
static void clearcmdentry(void) {
  struct tblentry **tblp;
  struct tblentry **pp;
  struct tblentry *cmdp;
  INTOFF;
  for (tblp = cmdtable; tblp < &cmdtable[CMDTABLESIZE]; tblp++) {
    pp = tblp;
    while ((cmdp = *pp) != NULL) {
      if (cmdp->cmdtype == CMDNORMAL ||
          (cmdp->cmdtype == CMDBUILTIN &&
           !(cmdp->param.cmd->flags & BUILTIN_REGULAR) && builtinloc > 0)) {
        *pp = cmdp->next;
        ckfree(cmdp);
      } else {
        pp = &cmdp->next;
      }
    }
  }
  INTON;
}

/*
 * Locate a command in the command hash table.  If "add" is nonzero,
 * add the command to the table if it is not already present.  The
 * variable "lastcmdentry" is set to point to the address of the link
 * pointing to the entry, so that delete_cmd_entry can delete the
 * entry.
 *
 * Interrupts must be off if called with add != 0.
 */
static struct tblentry *cmdlookup(const char *name, int add) {
  unsigned int hashval;
  const char *p;
  struct tblentry *cmdp;
  struct tblentry **pp;
  p = name;
  hashval = (unsigned char)*p << 4;
  while (*p) hashval += (unsigned char)*p++;
  hashval &= 0x7FFF;
  pp = &cmdtable[hashval % CMDTABLESIZE];
  for (cmdp = *pp; cmdp; cmdp = cmdp->next) {
    if (equal(cmdp->cmdname, name)) break;
    pp = &cmdp->next;
  }
  if (add && cmdp == NULL) {
    cmdp = *pp = ckmalloc(sizeof(struct tblentry) + strlen(name) + 1);
    cmdp->next = NULL;
    cmdp->cmdtype = CMDUNKNOWN;
    strcpy(cmdp->cmdname, name);
  }
  lastcmdentry = pp;
  return cmdp;
}

/*
 * Delete the command entry returned on the last lookup.
 */
static void delete_cmd_entry(void) {
  struct tblentry *cmdp;
  INTOFF;
  cmdp = *lastcmdentry;
  *lastcmdentry = cmdp->next;
  if (cmdp->cmdtype == CMDFUNCTION) freefunc(cmdp->param.func);
  ckfree(cmdp);
  INTON;
}

/*
 * Add a new command entry, replacing any existing command entry for
 * the same name - except special builtins.
 */
static void addcmdentry(char *name, struct cmdentry *entry) {
  struct tblentry *cmdp;
  cmdp = cmdlookup(name, 1);
  if (cmdp->cmdtype == CMDFUNCTION) {
    freefunc(cmdp->param.func);
  }
  cmdp->cmdtype = entry->cmdtype;
  cmdp->param = entry->u;
  cmdp->rehash = 0;
}

/* Define a shell function. */
static void defun(union node *func) {
  struct cmdentry entry;
  INTOFF;
  entry.cmdtype = CMDFUNCTION;
  entry.u.func = copyfunc(func);
  addcmdentry(func->ndefun.text, &entry);
  INTON;
}

/* Delete a function if it exists. */
static void unsetfunc(const char *name) {
  struct tblentry *cmdp;
  if ((cmdp = cmdlookup(name, 0)) != NULL && cmdp->cmdtype == CMDFUNCTION) {
    delete_cmd_entry();
  }
}

/* Locate and print what a word is... */
static int typecmd(int argc, char **argv) {
  int i;
  int err = 0;
  for (i = 1; i < argc; i++) {
    err |= describe_command(out1, argv[i], NULL, 1);
  }
  return err;
}

static int describe_command(struct output *out, char *command, const char *path,
                            int verbose) {
  struct cmdentry entry;
  struct tblentry *cmdp;
  const struct alias *ap;
  if (verbose) {
    outstr(command, out);
  }
  /* First look at the keywords */
  if (findkwd(command)) {
    outstr(verbose ? " is a shell keyword" : command, out);
    goto out;
  }
  /* Then look at the aliases */
  if ((ap = lookupalias(command, 0)) != NULL) {
    if (verbose) {
      outfmt(out, " is an alias for %s", ap->val);
    } else {
      outstr("alias ", out);
      printalias(ap);
      return 0;
    }
    goto out;
  }
  /* Then if the standard search path is used, check if it is
   * a tracked alias.
   */
  if (path == NULL) {
    path = pathval();
    cmdp = cmdlookup(command, 0);
  } else {
    cmdp = NULL;
  }
  if (cmdp != NULL) {
    entry.cmdtype = cmdp->cmdtype;
    entry.u = cmdp->param;
  } else {
    /* Finally use brute force */
    find_command(command, &entry, DO_ABS, path);
  }
  switch (entry.cmdtype) {
    case CMDNORMAL: {
      int j = entry.u.index;
      char *p;
      if (j == -1) {
        p = command;
      } else {
        do {
          padvance(&path, command);
        } while (--j >= 0);
        p = stackblock();
      }
      if (verbose) {
        outfmt(out, " is%s %s", cmdp ? " a tracked alias for" : nullstr, p);
      } else {
        outstr(p, out);
      }
      break;
    }
    case CMDFUNCTION:
      if (verbose) {
        outstr(" is a shell function", out);
      } else {
        outstr(command, out);
      }
      break;
    case CMDBUILTIN:
      if (verbose) {
        outfmt(out, " is a %sshell builtin",
               entry.u.cmd->flags & BUILTIN_SPECIAL ? "special " : nullstr);
      } else {
        outstr(command, out);
      }
      break;
    default:
      if (verbose) {
        outstr(": not found\n", out);
      }
      return 127;
  }
out:
  outc('\n', out);
  return 0;
}

static int commandcmd(argc, argv)
int argc;
char **argv;
{
  char *cmd;
  int c;
  enum {
    VERIFY_BRIEF = 1,
    VERIFY_VERBOSE = 2,
  } verify = 0;
  const char *path = NULL;
  while ((c = nextopt("pvV")) != '\0')
    if (c == 'V')
      verify |= VERIFY_VERBOSE;
    else if (c == 'v')
      verify |= VERIFY_BRIEF;
    else
      path = defpath;
  cmd = *argptr;
  if (verify && cmd) {
    return describe_command(out1, cmd, path, verify - VERIFY_BRIEF);
  }
  return 0;
}

/*
 * Prepare a pattern for a glob(3) call.
 *
 * Returns an stalloced string.
 */
static inline char *preglob(const char *pattern, int flag) {
  flag |= RMESCAPE_GLOB;
  return rmescapes((char *)pattern, flag);
}

static unsigned esclen(const char *start, const char *p) {
  unsigned esc = 0;
  while (p > start && *--p == (char)CTLESC) {
    esc++;
  }
  return esc;
}

static inline const char *getpwhome(const char *name) {
  struct passwd *pw = getpwnam(name);
  return pw ? pw->pw_dir : 0;
}

static void ifsbreakup(char *string, int maxargs, struct arglist *arglist);
static void recordregion(int start, int end, int nulonly);

/*
 * Perform variable substitution and command substitution on an
 * argument, placing the resulting list of arguments in arglist. If
 * EXP_FULL is true, perform splitting and file name expansion. When
 * arglist is NULL, perform here document expansion.
 */
static void expandarg(union node *arg, struct arglist *arglist, int flag) {
  struct strlist *sp;
  char *p;
  argbackq = arg->narg.backquote;
  STARTSTACKSTR(expdest);
  argstr(arg->narg.text, flag);
  if (arglist == NULL) {
    /* here document expanded */
    goto out;
  }
  p = grabstackstr(expdest);
  exparg.lastp = &exparg.list;
  /*
   * TODO - EXP_REDIR
   */
  if (flag & EXP_FULL) {
    ifsbreakup(p, -1, &exparg);
    *exparg.lastp = NULL;
    exparg.lastp = &exparg.list;
    expandmeta(exparg.list);
  } else {
    sp = (struct strlist *)stalloc(sizeof(struct strlist));
    sp->text = p;
    *exparg.lastp = sp;
    exparg.lastp = &sp->next;
  }
  *exparg.lastp = NULL;
  if (exparg.list) {
    *arglist->lastp = exparg.list;
    arglist->lastp = exparg.lastp;
  }
out:
  ifsfree();
}

/*
 * Perform variable and command substitution. If EXP_FULL is set, output
 * CTLESC characters to allow for further processing. Otherwise treat $@
 * like $* since no splitting will be performed.
 */
static char *argstr(char *p, int flag) {
  static const int DOLATSTRLEN = 6;
  static const char spclchars[] = {'=',       ':',    CTLQUOTEMARK, CTLENDVAR,
                                   CTLESC,    CTLVAR, CTLBACKQ,     CTLARI,
                                   CTLENDARI, 0};
  const char *reject = spclchars;
  int c;
  int breakall = (flag & (EXP_WORD | EXP_QUOTED)) == EXP_WORD;
  int inquotes;
  unsigned length;
  int startloc;
  reject += !!(flag & EXP_VARTILDE2);
  reject += flag & EXP_VARTILDE ? 0 : 2;
  inquotes = 0;
  length = 0;
  if (flag & EXP_TILDE) {
    flag &= ~EXP_TILDE;
  tilde:
    if (*p == '~') p = exptilde(p, flag);
  }
start:
  startloc = expdest - (char *)stackblock();
  for (;;) {
    int end;
    length += strcspn(p + length, reject);
    end = 0;
    c = (signed char)p[length];
    if (!(c & 0x80) || c == CTLENDARI || c == CTLENDVAR) {
      /*
       * c == '=' || c == ':' || c == '\0' ||
       * c == CTLENDARI || c == CTLENDVAR
       */
      length++;
      /* c == '\0' || c == CTLENDARI || c == CTLENDVAR */
      end = !!((c - 1) & 0x80);
    }
    if (length > 0 && !(flag & EXP_DISCARD)) {
      int newloc;
      char *q;
      q = stnputs(p, length, expdest);
      q[-1] &= end - 1;
      expdest = q - (flag & EXP_WORD ? end : 0);
      newloc = q - (char *)stackblock() - end;
      if (breakall && !inquotes && newloc > startloc) {
        recordregion(startloc, newloc, 0);
      }
      startloc = newloc;
    }
    p += length + 1;
    length = 0;
    if (end) break;
    switch (c) {
      case '=':
        flag |= EXP_VARTILDE2;
        reject++;
        /* fall through */
      case ':':
        /*
         * sort of a hack - expand tildes in variable
         * assignments (after the first '=' and after ':'s).
         */
        if (*--p == '~') {
          goto tilde;
        }
        continue;
      case CTLQUOTEMARK:
        /* "$@" syntax adherence hack */
        if (!inquotes && !memcmp(p, dolatstr + 1, DOLATSTRLEN - 1)) {
          p = evalvar(p + 1, flag | EXP_QUOTED) + 1;
          goto start;
        }
        inquotes ^= EXP_QUOTED;
      addquote:
        if (flag & QUOTES_ESC) {
          p--;
          length++;
          startloc++;
        }
        break;
      case CTLESC:
        startloc++;
        length++;
        goto addquote;
      case CTLVAR:
        p = evalvar(p, flag | inquotes);
        goto start;
      case CTLBACKQ:
        expbackq(argbackq->n, flag | inquotes);
        goto start;
      case CTLARI:
        p = expari(p, flag | inquotes);
        goto start;
    }
  }
  return p - 1;
}

static char *exptilde(char *startp, int flag) {
  signed char c;
  char *name;
  const char *home;
  char *p;
  p = startp;
  name = p + 1;
  while ((c = *++p) != '\0') {
    switch (c) {
      case CTLESC:
        return (startp);
      case CTLQUOTEMARK:
        return (startp);
      case ':':
        if (flag & EXP_VARTILDE) goto done;
        break;
      case '/':
      case CTLENDVAR:
        goto done;
    }
  }
done:
  if (flag & EXP_DISCARD) goto out;
  *p = '\0';
  if (*name == '\0') {
    home = lookupvar(homestr);
  } else {
    home = getpwhome(name);
  }
  *p = c;
  if (!home) goto lose;
  strtodest(home, flag | EXP_QUOTED);
out:
  return (p);
lose:
  return (startp);
}

static void removerecordregions(int endoff) {
  if (ifslastp == NULL) return;
  if (ifsfirst.endoff > endoff) {
    while (ifsfirst.next != NULL) {
      struct ifsregion *ifsp;
      INTOFF;
      ifsp = ifsfirst.next->next;
      ckfree(ifsfirst.next);
      ifsfirst.next = ifsp;
      INTON;
    }
    if (ifsfirst.begoff > endoff)
      ifslastp = NULL;
    else {
      ifslastp = &ifsfirst;
      ifsfirst.endoff = endoff;
    }
    return;
  }
  ifslastp = &ifsfirst;
  while (ifslastp->next && ifslastp->next->begoff < endoff) {
    ifslastp = ifslastp->next;
  }
  while (ifslastp->next != NULL) {
    struct ifsregion *ifsp;
    INTOFF;
    ifsp = ifslastp->next->next;
    ckfree(ifslastp->next);
    ifslastp->next = ifsp;
    INTON;
  }
  if (ifslastp->endoff > endoff) ifslastp->endoff = endoff;
}

/*
 * Expand arithmetic expression. Backup to start of expression,
 * evaluate, place result in (backed up) result, adjust string position.
 */
static char *expari(char *start, int flag) {
  struct stackmark sm;
  int begoff;
  int endoff;
  int len;
  int64_t result;
  char *p;
  p = stackblock();
  begoff = expdest - p;
  p = argstr(start, flag & EXP_DISCARD);
  if (flag & EXP_DISCARD) goto out;
  start = stackblock();
  endoff = expdest - start;
  start += begoff;
  STADJUST(start - expdest, expdest);
  removerecordregions(begoff);
  if (likely(flag & QUOTES_ESC)) rmescapes(start, 0);
  pushstackmark(&sm, endoff);
  result = arith(start);
  popstackmark(&sm);
  len = cvtnum(result, flag);
  if (likely(!(flag & EXP_QUOTED))) recordregion(begoff, begoff + len, 0);
out:
  return p;
}

/*
 * Expand stuff in backwards quotes.
 */
static void expbackq(union node *cmd, int flag) {
  struct backcmd in;
  int i;
  char buf[128];
  char *p;
  char *dest;
  int startloc;
  struct stackmark smark;
  if (flag & EXP_DISCARD) goto out;
  INTOFF;
  startloc = expdest - (char *)stackblock();
  pushstackmark(&smark, startloc);
  evalbackcmd(cmd, (struct backcmd *)&in);
  popstackmark(&smark);
  p = in.buf;
  i = in.nleft;
  if (i == 0) goto read;
  for (;;) {
    memtodest(p, i, flag);
  read:
    if (in.fd < 0) break;
    do {
      i = read(in.fd, buf, sizeof buf);
    } while (i < 0 && errno == EINTR);
    TRACE(("expbackq: read returns %d\n", i));
    if (i <= 0) break;
    p = buf;
  }
  if (in.buf) ckfree(in.buf);
  if (in.fd >= 0) {
    close(in.fd);
    back_exitstatus = waitforjob(in.jp);
  }
  INTON;
  /* Eat all trailing newlines */
  dest = expdest;
  for (; dest > ((char *)stackblock() + startloc) && dest[-1] == '\n';) {
    STUNPUTC(dest);
  }
  expdest = dest;
  if (!(flag & EXP_QUOTED)) {
    recordregion(startloc, dest - (char *)stackblock(), 0);
  }
  TRACE(("evalbackq: size=%d: \"%.*s\"\n",
         (dest - (char *)stackblock()) - startloc,
         (dest - (char *)stackblock()) - startloc,
         (char *)stackblock() + startloc));
out:
  argbackq = argbackq->next;
}

static char *scanleft(char *startp, char *rmesc, char *rmescend, char *str,
                      int quotes, int zero) {
  char *loc;
  char *loc2;
  char c;
  loc = startp;
  loc2 = rmesc;
  do {
    int match;
    const char *s = loc2;
    c = *loc2;
    if (zero) {
      *loc2 = '\0';
      s = rmesc;
    }
    match = pmatch(str, s);
    *loc2 = c;
    if (match) return loc;
    if (quotes && *loc == (char)CTLESC) loc++;
    loc++;
    loc2++;
  } while (c);
  return 0;
}

static char *scanright(char *startp, char *rmesc, char *rmescend, char *str,
                       int quotes, int zero) {
  int esc = 0;
  char *loc;
  char *loc2;
  for (loc = str - 1, loc2 = rmescend; loc >= startp; loc2--) {
    int match;
    char c = *loc2;
    const char *s = loc2;
    if (zero) {
      *loc2 = '\0';
      s = rmesc;
    }
    match = pmatch(str, s);
    *loc2 = c;
    if (match) return loc;
    loc--;
    if (quotes) {
      if (--esc < 0) {
        esc = esclen(startp, loc);
      }
      if (esc % 2) {
        esc--;
        loc--;
      }
    }
  }
  return 0;
}

static char *subevalvar(char *start, char *str, int strloc, int startloc,
                        int varflags, int flag) {
  int subtype = varflags & VSTYPE;
  int quotes = flag & QUOTES_ESC;
  char *startp;
  char *loc;
  long amount;
  char *rmesc, *rmescend;
  int zero;
  char *(*scan)(char *, char *, char *, char *, int, int);
  char *p;
  p = argstr(start, (flag & EXP_DISCARD) | EXP_TILDE | (str ? 0 : EXP_CASE));
  if (flag & EXP_DISCARD) return p;
  startp = (char *)stackblock() + startloc;
  switch (subtype) {
    case VSASSIGN:
      setvar(str, startp, 0);
      loc = startp;
      goto out;
    case VSQUESTION:
      varunset(start, str, startp, varflags);
      __builtin_unreachable();
  }
  subtype -= VSTRIMRIGHT;
  rmesc = startp;
  rmescend = (char *)stackblock() + strloc;
  if (quotes) {
    rmesc = rmescapes(startp, RMESCAPE_ALLOC | RMESCAPE_GROW);
    if (rmesc != startp) {
      rmescend = expdest;
      startp = (char *)stackblock() + startloc;
    }
  }
  rmescend--;
  str = (char *)stackblock() + strloc;
  preglob(str, 0);
  /* zero = subtype == VSTRIMLEFT || subtype == VSTRIMLEFTMAX */
  zero = subtype >> 1;
  /* VSTRIMLEFT/VSTRIMRIGHTMAX -> scanleft */
  scan = (subtype & 1) ^ zero ? scanleft : scanright;
  loc = scan(startp, rmesc, rmescend, str, quotes, zero);
  if (loc) {
    if (zero) {
      memmove(startp, loc, str - loc);
      loc = startp + (str - loc) - 1;
    }
    *loc = '\0';
  } else
    loc = str - 1;
out:
  amount = loc - expdest;
  STADJUST(amount, expdest);
  /* Remove any recorded regions beyond start of variable */
  removerecordregions(startloc);
  return p;
}

/*
 * Expand a variable, and return a pointer to the next character in the
 * input string.
 */
static char *evalvar(char *p, int flag) {
  int subtype;
  int varflags;
  char *var_;
  int patloc;
  int startloc;
  long varlen;
  int discard;
  int quoted;
  varflags = *p++;
  subtype = varflags & VSTYPE;
  quoted = flag & EXP_QUOTED;
  var_ = p;
  startloc = expdest - (char *)stackblock();
  p = strchr(p, '=') + 1;
again:
  varlen = varvalue(var_, varflags, flag, quoted);
  if (varflags & VSNUL) varlen--;
  discard = varlen < 0 ? EXP_DISCARD : 0;
  switch (subtype) {
    case VSPLUS:
      discard ^= EXP_DISCARD;
      /* fall through */
    case 0:
    case VSMINUS:
      p = argstr(p, flag | EXP_TILDE | EXP_WORD | (discard ^ EXP_DISCARD));
      goto record;
    case VSASSIGN:
    case VSQUESTION:
      p = subevalvar(p, var_, 0, startloc, varflags,
                     (flag & ~QUOTES_ESC) | (discard ^ EXP_DISCARD));
      if ((flag | ~discard) & EXP_DISCARD) goto record;
      varflags &= ~VSNUL;
      subtype = VSNORMAL;
      goto again;
  }
  if ((discard & ~flag) && uflag) varunset(p, var_, 0, 0);
  if (subtype == VSLENGTH) {
    p++;
    if (flag & EXP_DISCARD) return p;
    cvtnum(varlen > 0 ? varlen : 0, flag);
    goto really_record;
  }
  if (subtype == VSNORMAL) goto record;
  flag |= discard;
  if (!(flag & EXP_DISCARD)) {
    /*
     * Terminate the string and start recording the pattern
     * right after it
     */
    STPUTC('\0', expdest);
  }
  patloc = expdest - (char *)stackblock();
  p = subevalvar(p, NULL, patloc, startloc, varflags, flag);
record:
  if ((flag | discard) & EXP_DISCARD) return p;
really_record:
  if (quoted) {
    quoted = *var_ == '@' && shellparam.nparam;
    if (!quoted) return p;
  }
  recordregion(startloc, expdest - (char *)stackblock(), quoted);
  return p;
}

/*
 * Put a string on the stack.
 */
static unsigned memtodest(const char *p, unsigned len, int flags) {
  const char *syntax = flags & EXP_QUOTED ? DQSYNTAX : BASESYNTAX;
  char *q;
  char *s;
  if (unlikely(!len)) return 0;
  q = makestrspace(len * 2, expdest);
  s = q;
  do {
    int c = (signed char)*p++;
    if (c) {
      if ((flags & QUOTES_ESC) &&
          ((syntax[c] == CCTL) || (flags & EXP_QUOTED && syntax[c] == CBACK))) {
        USTPUTC(CTLESC, q);
      }
    } else if (!(flags & EXP_KEEPNUL))
      continue;
    USTPUTC(c, q);
  } while (--len);
  expdest = q;
  return q - s;
}

static unsigned strtodest(const char *p, int flags) {
  unsigned len = strlen(p);
  memtodest(p, len, flags);
  return len;
}

/*
 * Add the value of a specialized variable to the stack string.
 */
static long varvalue(char *name, int varflags, int flags, int quoted) {
  int num;
  char *p;
  int i;
  int sep;
  char sepc;
  char **ap;
  int subtype = varflags & VSTYPE;
  int discard =
      (subtype == VSPLUS || subtype == VSLENGTH) | (flags & EXP_DISCARD);
  long len = 0;
  char c;
  if (!subtype) {
    if (discard) return -1;
    sh_error("Bad substitution");
  }
  flags |= EXP_KEEPNUL;
  flags &= discard ? ~QUOTES_ESC : ~0;
  sep = (flags & EXP_FULL) << CHAR_BIT;
  switch (*name) {
    case '$':
      num = rootpid;
      goto numvar;
    case '?':
      num = exitstatus;
      goto numvar;
    case '#':
      num = shellparam.nparam;
      goto numvar;
    case '!':
      num = backgndpid;
      if (num == 0) return -1;
    numvar:
      len = cvtnum(num, flags);
      break;
    case '-':
      p = makestrspace(NOPTS, expdest);
      for (i = NOPTS - 1; i >= 0; i--) {
        if (optlist[i] && optletters[i]) {
          USTPUTC(optletters[i], p);
          len++;
        }
      }
      expdest = p;
      break;
    case '@':
      if (quoted && sep) goto param;
      /* fall through */
    case '*':
      /* We will set c to 0 or ~0 depending on whether
       * we're doing field splitting.  We won't do field
       * splitting if either we're quoted or sep is zero.
       *
       * Instead of testing (quoted || !sep) the following
       * trick optimises away any branches by using the
       * fact that EXP_QUOTED (which is the only bit that
       * can be set in quoted) is the same as EXP_FULL <<
       * CHAR_BIT (which is the only bit that can be set
       * in sep).
       */
      /* #if EXP_QUOTED >> CHAR_BIT != EXP_FULL */
      /* #error The following two lines expect EXP_QUOTED == EXP_FULL <<
       * CHAR_BIT */
      /* #endif */
      c = !((quoted | ~sep) & EXP_QUOTED) - 1;
      sep &= ~quoted;
      sep |= ifsset() ? (unsigned char)(c & ifsval()[0]) : ' ';
    param:
      sepc = sep;
      if (!(ap = shellparam.p)) return -1;
      while ((p = *ap++)) {
        len += strtodest(p, flags);
        if (*ap && sep) {
          len++;
          memtodest(&sepc, 1, flags);
        }
      }
      break;
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
      num = atoi(name);
      if (num < 0 || num > shellparam.nparam) return -1;
      p = num ? shellparam.p[num - 1] : arg0;
      goto value;
    default:
      p = lookupvar(name);
    value:
      if (!p) return -1;
      len = strtodest(p, flags);
      break;
  }
  if (discard) STADJUST(-len, expdest);
  return len;
}

/*
 * Record the fact that we have to scan this region of the
 * string for IFS characters.
 */
static void recordregion(int start, int end, int nulonly) {
  struct ifsregion *ifsp;
  if (ifslastp == NULL) {
    ifsp = &ifsfirst;
  } else {
    INTOFF;
    ifsp = (struct ifsregion *)ckmalloc(sizeof(struct ifsregion));
    ifsp->next = NULL;
    ifslastp->next = ifsp;
    INTON;
  }
  ifslastp = ifsp;
  ifslastp->begoff = start;
  ifslastp->endoff = end;
  ifslastp->nulonly = nulonly;
}

/*
 * Break the argument string into pieces based upon IFS and add the
 * strings to the argument list.  The regions of the string to be
 * searched for IFS characters have been stored by recordregion.
 * If maxargs is non-negative, at most maxargs arguments will be created, by
 * joining together the last arguments.
 */
static void ifsbreakup(char *string, int maxargs, struct arglist *arglist) {
  struct ifsregion *ifsp;
  struct strlist *sp;
  char *start;
  char *p;
  char *q;
  char *r = NULL;
  const char *ifs, *realifs;
  int ifsspc;
  int nulonly;
  start = string;
  if (ifslastp != NULL) {
    ifsspc = 0;
    nulonly = 0;
    realifs = ifsset() ? ifsval() : defifs;
    ifsp = &ifsfirst;
    do {
      int afternul;
      p = string + ifsp->begoff;
      afternul = nulonly;
      nulonly = ifsp->nulonly;
      ifs = nulonly ? nullstr : realifs;
      ifsspc = 0;
      while (p < string + ifsp->endoff) {
        int c;
        bool isifs;
        bool isdefifs;
        q = p;
        c = *p++;
        if (c == (char)CTLESC) c = *p++;
        isifs = !!strchr(ifs, c);
        isdefifs = false;
        if (isifs) isdefifs = !!strchr(defifs, c);
        /* If only reading one more argument:
         * If we have exactly one field,
         * read that field without its terminator.
         * If we have more than one field,
         * read all fields including their terminators,
         * except for trailing IFS whitespace.
         *
         * This means that if we have only IFS
         * characters left, and at most one
         * of them is non-whitespace, we stop
         * reading here.
         * Otherwise, we read all the remaining
         * characters except for trailing
         * IFS whitespace.
         *
         * In any case, r indicates the start
         * of the characters to remove, or NULL
         * if no characters should be removed.
         */
        if (!maxargs) {
          if (isdefifs) {
            if (!r) r = q;
            continue;
          }
          if (!(isifs && ifsspc)) r = NULL;
          ifsspc = 0;
          continue;
        }
        if (ifsspc) {
          if (isifs) q = p;
          start = q;
          if (isdefifs) continue;
          isifs = false;
        }
        if (isifs) {
          if (!(afternul || nulonly)) ifsspc = isdefifs;
          /* Ignore IFS whitespace at start */
          if (q == start && ifsspc) {
            start = p;
            ifsspc = 0;
            continue;
          }
          if (maxargs > 0 && !--maxargs) {
            r = q;
            continue;
          }
          *q = '\0';
          sp = (struct strlist *)stalloc(sizeof *sp);
          sp->text = start;
          *arglist->lastp = sp;
          arglist->lastp = &sp->next;
          start = p;
          continue;
        }
        ifsspc = 0;
      }
    } while ((ifsp = ifsp->next) != NULL);
    if (nulonly) goto add;
  }
  if (r) *r = '\0';
  if (!*start) return;
add:
  sp = (struct strlist *)stalloc(sizeof *sp);
  sp->text = start;
  *arglist->lastp = sp;
  arglist->lastp = &sp->next;
}

/*
 * Expand shell metacharacters.  At this point, the only control characters
 * should be escapes.  The results are stored in the list exparg.
 */
static void expandmeta(struct strlist *str) {
  static const char metachars[] = {'*', '?', '[', 0};
  /* TODO - EXP_REDIR */
  while (str) {
    struct strlist **savelastp;
    struct strlist *sp;
    char *p;
    unsigned len;
    if (fflag) goto nometa;
    if (!strpbrk(str->text, metachars)) goto nometa;
    savelastp = exparg.lastp;
    INTOFF;
    p = preglob(str->text, RMESCAPE_ALLOC | RMESCAPE_HEAP);
    len = strlen(p);
    expdir_max = len + PATH_MAX;
    expdir = ckmalloc(expdir_max);
    expmeta(p, len, 0);
    ckfree(expdir);
    if (p != str->text) ckfree(p);
    INTON;
    if (exparg.lastp == savelastp) {
      /*
       * no matches
       */
    nometa:
      *exparg.lastp = str;
      rmescapes(str->text, 0);
      exparg.lastp = &str->next;
    } else {
      *exparg.lastp = NULL;
      *savelastp = sp = expsort(*savelastp);
      while (sp->next != NULL) sp = sp->next;
      exparg.lastp = &sp->next;
    }
    str = str->next;
  }
}

/*
 * Do metacharacter (i.e. *, ?, [...]) expansion.
 */
static void expmeta(char *name, unsigned name_len, unsigned expdir_len) {
  char *enddir = expdir + expdir_len;
  char *p;
  const char *cp;
  char *start;
  char *endname;
  int metaflag;
  struct stat statb;
  DIR *dirp;
  struct dirent *dp;
  int atend;
  int matchdot;
  int esc;
  metaflag = 0;
  start = name;
  for (p = name; esc = 0, *p; p += esc + 1) {
    if (*p == '*' || *p == '?')
      metaflag = 1;
    else if (*p == '[') {
      char *q = p + 1;
      if (*q == '!') q++;
      for (;;) {
        if (*q == '\\') q++;
        if (*q == '/' || *q == '\0') break;
        if (*++q == ']') {
          metaflag = 1;
          break;
        }
      }
    } else {
      if (*p == '\\' && p[1]) esc++;
      if (p[esc] == '/') {
        if (metaflag) break;
        start = p + esc + 1;
      }
    }
  }
  if (metaflag == 0) { /* we've reached the end of the file name */
    if (!expdir_len) return;
    p = name;
    do {
      if (*p == '\\' && p[1]) p++;
      *enddir++ = *p;
    } while (*p++);
    if (lstat(expdir, &statb) >= 0) addfname(expdir);
    return;
  }
  endname = p;
  if (name < start) {
    p = name;
    do {
      if (*p == '\\' && p[1]) p++;
      *enddir++ = *p++;
    } while (p < start);
  }
  *enddir = 0;
  cp = expdir;
  expdir_len = enddir - cp;
  if (!expdir_len) cp = ".";
  if ((dirp = opendir(cp)) == NULL) return;
  if (*endname == 0) {
    atend = 1;
  } else {
    atend = 0;
    *endname = '\0';
    endname += esc + 1;
  }
  name_len -= endname - name;
  matchdot = 0;
  p = start;
  if (*p == '\\') p++;
  if (*p == '.') matchdot++;
  while (!int_pending() && (dp = readdir(dirp)) != NULL) {
    if (dp->d_name[0] == '.' && !matchdot) continue;
    if (pmatch(start, dp->d_name)) {
      if (atend) {
        scopy(dp->d_name, enddir);
        addfname(expdir);
      } else {
        unsigned offset;
        unsigned len;
        p = stpcpy(enddir, dp->d_name);
        *p = '/';
        offset = p - expdir + 1;
        len = offset + name_len + NAME_MAX;
        if (len > expdir_max) {
          len += PATH_MAX;
          expdir = ckrealloc(expdir, len);
          expdir_max = len;
        }
        expmeta(endname, name_len, offset);
        enddir = expdir + expdir_len;
      }
    }
  }
  closedir(dirp);
  if (!atend) endname[-esc - 1] = esc ? '\\' : '/';
}

/*
 * Add a file name to the list.
 */
static void addfname(char *name) {
  struct strlist *sp;
  sp = (struct strlist *)stalloc(sizeof *sp);
  sp->text = sstrdup(name);
  *exparg.lastp = sp;
  exparg.lastp = &sp->next;
}

/*
 * Sort the results of file name expansion.  It calculates the number of
 * strings to sort and then calls msort (short for merge sort) to do the
 * work.
 */
static struct strlist *expsort(struct strlist *str) {
  int len;
  struct strlist *sp;
  len = 0;
  for (sp = str; sp; sp = sp->next) len++;
  return msort(str, len);
}

static struct strlist *msort(struct strlist *list, int len) {
  struct strlist *p, *q = NULL;
  struct strlist **lpp;
  int half;
  int n;
  if (len <= 1) return list;
  half = len >> 1;
  p = list;
  for (n = half; --n >= 0;) {
    q = p;
    p = p->next;
  }
  q->next = NULL;           /* terminate first half of list */
  q = msort(list, half);    /* sort first half of list */
  p = msort(p, len - half); /* sort second half */
  lpp = &list;
  for (;;) {
    if (strcmp(p->text, q->text) < 0) {
      *lpp = p;
      lpp = &p->next;
      if ((p = *lpp) == NULL) {
        *lpp = q;
        break;
      }
    } else {
      *lpp = q;
      lpp = &q->next;
      if ((q = *lpp) == NULL) {
        *lpp = p;
        break;
      }
    }
  }
  return list;
}

/*
 * Returns true if the pattern matches the string.
 */
static inline int patmatch(char *pattern, const char *string) {
  return pmatch(preglob(pattern, 0), string);
}

static int ccmatch(const char *p, int chr, const char **r) {
  static const struct class {
    char name[10];
    int (*fn)(int);
  } classes[] = {{.name = ":alnum:]", .fn = isalnum},
                 {.name = ":cntrl:]", .fn = iscntrl},
                 {.name = ":lower:]", .fn = islower},
                 {.name = ":space:]", .fn = isspace},
                 {.name = ":alpha:]", .fn = isalpha},
                 {.name = ":digit:]", .fn = isdigit},
                 {.name = ":print:]", .fn = isprint},
                 {.name = ":upper:]", .fn = isupper},
                 {.name = ":blank:]", .fn = isblank},
                 {.name = ":graph:]", .fn = isgraph},
                 {.name = ":punct:]", .fn = ispunct},
                 {.name = ":xdigit:]", .fn = isxdigit}};
  const struct class *class, *end;
  end = classes + sizeof(classes) / sizeof(classes[0]);
  for (class = classes; class < end; class ++) {
    const char *q;
    q = prefix(p, class->name);
    if (!q) continue;
    *r = q;
    return class->fn(chr);
  }
  *r = 0;
  return 0;
}

static int pmatch(const char *pattern, const char *string) {
  const char *p, *q;
  char c;
  p = pattern;
  q = string;
  for (;;) {
    switch (c = *p++) {
      case '\0':
        goto breakloop;
      case '\\':
        if (*p) {
          c = *p++;
        }
        goto dft;
      case '?':
        if (*q++ == '\0') return 0;
        break;
      case '*':
        c = *p;
        while (c == '*') c = *++p;
        if (c != '\\' && c != '?' && c != '*' && c != '[') {
          while (*q != c) {
            if (*q == '\0') return 0;
            q++;
          }
        }
        do {
          if (pmatch(p, q)) return 1;
        } while (*q++ != '\0');
        return 0;
      case '[': {
        const char *startp;
        int invert, found;
        char chr;
        startp = p;
        invert = 0;
        if (*p == '!') {
          invert++;
          p++;
        }
        found = 0;
        chr = *q;
        if (chr == '\0') return 0;
        c = *p++;
        do {
          if (!c) {
            p = startp;
            c = '[';
            goto dft;
          }
          if (c == '[') {
            const char *r;
            found |= !!ccmatch(p, chr, &r);
            if (r) {
              p = r;
              continue;
            }
          } else if (c == '\\')
            c = *p++;
          if (*p == '-' && p[1] != ']') {
            p++;
            if (*p == '\\') p++;
            if (chr >= c && chr <= *p) found = 1;
            p++;
          } else {
            if (chr == c) found = 1;
          }
        } while ((c = *p++) != ']');
        if (found == invert) return 0;
        q++;
        break;
      }
      dft:
      default:
        if (*q++ != c) return 0;
        break;
    }
  }
breakloop:
  if (*q != '\0') return 0;
  return 1;
}

/*
 * Remove any CTLESC characters from a string.
 */
static char *rmescapes(char *str, int flag) {
  char *p, *q, *r;
  int notescaped;
  int globbing;
  p = strpbrk(str, qchars);
  if (!p) {
    return str;
  }
  q = p;
  r = str;
  if (flag & RMESCAPE_ALLOC) {
    unsigned len = p - str;
    unsigned fulllen = len + strlen(p) + 1;
    if (flag & RMESCAPE_GROW) {
      int strloc = str - (char *)stackblock();
      r = makestrspace(fulllen, expdest);
      str = (char *)stackblock() + strloc;
      p = str + len;
    } else if (flag & RMESCAPE_HEAP) {
      r = ckmalloc(fulllen);
    } else {
      r = stalloc(fulllen);
    }
    q = r;
    if (len > 0) {
      q = mempcpy(q, str, len);
    }
  }
  globbing = flag & RMESCAPE_GLOB;
  notescaped = globbing;
  while (*p) {
    if (*p == (char)CTLQUOTEMARK) {
      p++;
      notescaped = globbing;
      continue;
    }
    if (*p == '\\') {
      /* naked back slash */
      notescaped = 0;
      goto copy;
    }
    if (*p == (char)CTLESC) {
      p++;
      if (notescaped) *q++ = '\\';
    }
    notescaped = globbing;
  copy:
    *q++ = *p++;
  }
  *q = '\0';
  if (flag & RMESCAPE_GROW) {
    expdest = r;
    STADJUST(q - r + 1, expdest);
  }
  return r;
}

/*
 * Our own itoa().
 */
static unsigned cvtnum(int64_t num, int flags) {
  int len = max_int_length(sizeof(num));
  char buf[len];
  len = fmtstr(buf, len, "%ld", num);
  return memtodest(buf, len, flags);
}

static void freestrings(struct strpush *sp) {
  INTOFF;
  do {
    struct strpush *psp;
    if (sp->ap) {
      sp->ap->flag &= ~ALIASINUSE;
      if (sp->ap->flag & ALIASDEAD) unalias(sp->ap->name);
    }
    psp = sp;
    sp = sp->spfree;
    if (psp != &(parsefile->basestrpush)) ckfree(psp);
  } while (sp);
  parsefile->spfree = NULL;
  INTON;
}

static int pgetc_nofree(void) {
  int c;
  if (parsefile->unget) return parsefile->lastc[--parsefile->unget];
  if (--parsefile->nleft >= 0)
    c = (signed char)*parsefile->nextc++;
  else
    c = preadbuffer();
  parsefile->lastc[1] = parsefile->lastc[0];
  parsefile->lastc[0] = c;
  return c;
}

/*
 * Read a character from the script, returning PEOF on end of file.
 * Nul characters in the input are silently discarded.
 */
int pgetc(void) {
  struct strpush *sp = parsefile->spfree;
  if (unlikely(sp)) freestrings(sp);
  return pgetc_nofree();
}

static void AddUniqueCompletion(linenoiseCompletions *c, char *s) {
  size_t i;
  if (!s) return;
  for (i = 0; i < c->len; ++i) {
    if (!strcmp(s, c->cvec[i])) {
      return;
    }
  }
  c->cvec = realloc(c->cvec, ++c->len * sizeof(*c->cvec));
  c->cvec[c->len - 1] = s;
}

static void CompleteCommand(const char *p, const char *q, const char *b,
                            linenoiseCompletions *c) {
  DIR *d;
  size_t i;
  struct dirent *e;
  const char *x, *y, *path;
  struct tblentry **pp, *cmdp;
  for (pp = cmdtable; pp < &cmdtable[CMDTABLESIZE]; pp++) {
    for (cmdp = *pp; cmdp; cmdp = cmdp->next) {
      if (cmdp->cmdtype >= 0 && !strncmp(cmdp->cmdname, p, q - p)) {
        AddUniqueCompletion(c, strdup(cmdp->cmdname));
      }
    }
  }
  for (i = 0; i < ARRAYLEN(kBuiltinCmds); ++i) {
    if (!strncmp(kBuiltinCmds[i].name, p, q - p)) {
      AddUniqueCompletion(c, strdup(kBuiltinCmds[i].name));
    }
  }
  for (y = x = lookupvar("PATH"); *y; x = y + 1) {
    if ((path = strndup(x, (y = strchrnul(x, ':')) - x))) {
      if ((d = opendir(path))) {
        while ((e = readdir(d))) {
          if (e->d_type == DT_REG && !strncmp(e->d_name, p, q - p)) {
            AddUniqueCompletion(c, strdup(e->d_name));
          }
        }
        closedir(d);
      }
      free((void *)path);
    }
  }
}

static void CompleteFilename(const char *p, const char *q, const char *b,
                             linenoiseCompletions *c) {
  DIR *d;
  char *buf;
  const char *g;
  struct dirent *e;
  if ((buf = malloc(512))) {
    if ((g = memrchr(p, '/', q - p))) {
      *(char *)mempcpy(buf, p, MIN(g - p, 511)) = 0;
      p = ++g;
    } else {
      strcpy(buf, ".");
    }
    if ((d = opendir(buf))) {
      while ((e = readdir(d))) {
        if (!strcmp(e->d_name, ".")) continue;
        if (!strcmp(e->d_name, "..")) continue;
        if (!strncmp(e->d_name, p, q - p)) {
          snprintf(buf, 512, "%.*s%s%s", p - b, b, e->d_name,
                   e->d_type == DT_DIR ? "/" : "");
          AddUniqueCompletion(c, strdup(buf));
        }
      }
      closedir(d);
    }
    free(buf);
  }
}

static void ShellCompletion(const char *p, linenoiseCompletions *c) {
  bool slashed;
  const char *q, *b;
  for (slashed = false, b = p, q = (p += strlen(p)); p > b; --p) {
    if (p[-1] == '/' && p[-1] == '\\') slashed = true;
    if (!isalnum(p[-1]) &&
        (p[-1] != '.' && p[-1] != '_' && p[-1] != '-' && p[-1] != '+' &&
         p[-1] != '[' && p[-1] != '/' && p[-1] != '\\')) {
      break;
    }
  }
  if (b == p && !slashed) {
    CompleteCommand(p, q, b, c);
  } else {
    CompleteFilename(p, q, b, c);
  }
}

static char *ShellHint(const char *p, const char **ansi1, const char **ansi2) {
  char *h = 0;
  linenoiseCompletions c = {0};
  ShellCompletion(p, &c);
  if (c.len == 1) {
    h = strdup(c.cvec[0] + strlen(p));
  }
  linenoiseFreeCompletions(&c);
  return h;
}

static ssize_t preadfd(void) {
  ssize_t nr;
  char *p, *buf = parsefile->buf;
  parsefile->nextc = buf;
retry:
  if (!parsefile->fd && isatty(0)) {
    linenoiseSetFreeHintsCallback(free);
    if (!IsWindows()) {
      // TODO(jart): Cache $PATH search.
      linenoiseSetHintsCallback(ShellHint);
      linenoiseSetCompletionCallback(ShellCompletion);
    }
    char ps1[256];
    snprintf(ps1, sizeof(ps1), "%d >: ", exitstatus);
    if ((p = linenoiseWithHistory(ps1, "unbourne"))) {
      nr = min(strlen(p), IBUFSIZ - 2);
      memcpy(buf, p, nr);
      buf[nr++] = '\n';
      free(p);
    } else {
      write(1, "\n", 1);
      nr = 0;
    }
  } else {
    nr = read(parsefile->fd, buf, IBUFSIZ - 1);
  }
  if (nr < 0) {
    if (errno == EINTR) goto retry;
    if (parsefile->fd == 0 && errno == EAGAIN) {
      int flags = fcntl(0, F_GETFL, 0);
      if (flags >= 0 && flags & O_NONBLOCK) {
        flags &= ~O_NONBLOCK;
        if (fcntl(0, F_SETFL, flags) >= 0) {
          outstr("sh: turning off NDELAY mode\n", out2);
          goto retry;
        }
      }
    }
  }
  return nr;
}

/*
 * Refill the input buffer and return the next input character:
 *
 * 1) If a string was pushed back on the input, pop it;
 * 2) If an EOF was pushed back (parsenleft == EOF_NLEFT) or we are reading
 *    from a string so we can't refill the buffer, return EOF.
 * 3) If the is more stuff in this buffer, use it else call read to fill it.
 * 4) Process input up to the next newline, deleting nul characters.
 */
static int preadbuffer(void) {
  char *q;
  int more;
  char savec;
  if (unlikely(parsefile->strpush)) {
    popstring();
    return pgetc_nofree();
  }
  if (unlikely(parsefile->nleft == EOF_NLEFT || parsefile->buf == NULL))
    return PEOF;
  flushall();
  more = parsefile->lleft;
  if (more <= 0) {
  again:
    if ((more = preadfd()) <= 0) {
      parsefile->lleft = parsefile->nleft = EOF_NLEFT;
      return PEOF;
    }
  }
  q = parsefile->nextc;
  /* delete nul characters */
  for (;;) {
    int c;
    more--;
    c = *q;
    if (!c)
      memmove(q, q + 1, more);
    else {
      q++;
      if (c == '\n') {
        parsefile->nleft = q - parsefile->nextc - 1;
        break;
      }
    }
    if (more <= 0) {
      parsefile->nleft = q - parsefile->nextc - 1;
      if (parsefile->nleft < 0) goto again;
      break;
    }
  }
  parsefile->lleft = more;
  savec = *q;
  *q = '\0';
  if (vflag) {
    outstr(parsefile->nextc, out2);
  }
  *q = savec;
  return (signed char)*parsefile->nextc++;
}

/*
 * Undo a call to pgetc.  Only two characters may be pushed back.
 * PEOF may be pushed back.
 */
static void pungetc(void) {
  parsefile->unget++;
}

/*
 * Push a string back onto the input at this current parsefile level.
 * We handle aliases this way.
 */
static void pushstring(char *s, void *ap) {
  struct strpush *sp;
  unsigned len;
  len = strlen(s);
  INTOFF;
  /*dprintf("*** calling pushstring: %s, %d\n", s, len);*/
  if ((unsigned long)parsefile->strpush | (unsigned long)parsefile->spfree) {
    sp = ckmalloc(sizeof(struct strpush));
    sp->prev = parsefile->strpush;
    parsefile->strpush = sp;
  } else
    sp = parsefile->strpush = &(parsefile->basestrpush);
  sp->prevstring = parsefile->nextc;
  sp->prevnleft = parsefile->nleft;
  sp->unget = parsefile->unget;
  sp->spfree = parsefile->spfree;
  memcpy(sp->lastc, parsefile->lastc, sizeof(sp->lastc));
  sp->ap = (struct alias *)ap;
  if (ap) {
    ((struct alias *)ap)->flag |= ALIASINUSE;
    sp->string = s;
  }
  parsefile->nextc = s;
  parsefile->nleft = len;
  parsefile->unget = 0;
  parsefile->spfree = NULL;
  INTON;
}

static void popstring(void) {
  struct strpush *sp = parsefile->strpush;
  INTOFF;
  if (sp->ap) {
    if (parsefile->nextc[-1] == ' ' || parsefile->nextc[-1] == '\t') {
      checkkwd |= CHKALIAS;
    }
    if (sp->string != sp->ap->val) {
      ckfree(sp->string);
    }
  }
  parsefile->nextc = sp->prevstring;
  parsefile->nleft = sp->prevnleft;
  parsefile->unget = sp->unget;
  memcpy(parsefile->lastc, sp->lastc, sizeof(sp->lastc));
  /*dprintf("*** calling popstring: restoring to '%s'\n", parsenextc);*/
  parsefile->strpush = sp->prev;
  parsefile->spfree = sp;
  INTON;
}

/*
 * Set the input to take input from a file.  If push is set, push the
 * old input onto the stack first.
 */
static int setinputfile(const char *fname, int flags) {
  int fd;
  INTOFF;
  fd = sh_open(fname, O_RDONLY, flags & INPUT_NOFILE_OK);
  if (fd < 0) goto out;
  if (fd < 10) fd = savefd(fd, fd);
  setinputfd(fd, flags & INPUT_PUSH_FILE);
out:
  INTON;
  return fd;
}

/*
 * Like setinputfile, but takes an open file descriptor.  Call this with
 * interrupts off.
 */
static void setinputfd(int fd, int push) {
  if (push) {
    pushfile();
    parsefile->buf = 0;
  }
  parsefile->fd = fd;
  if (parsefile->buf == NULL) parsefile->buf = ckmalloc(IBUFSIZ);
  parsefile->lleft = parsefile->nleft = 0;
  plinno = 1;
}

/*
 * Like setinputfile, but takes input from a string.
 */
static void setinputstring(char *string) {
  INTOFF;
  pushfile();
  parsefile->nextc = string;
  parsefile->nleft = strlen(string);
  parsefile->buf = NULL;
  plinno = 1;
  INTON;
}

/*
 * To handle the "." command, a stack of input files is used.  Pushfile
 * adds a new entry to the stack and popfile restores the previous level.
 */
static void pushfile(void) {
  struct parsefile *pf;
  pf = (struct parsefile *)ckmalloc(sizeof(struct parsefile));
  pf->prev = parsefile;
  pf->fd = -1;
  pf->strpush = NULL;
  pf->spfree = NULL;
  pf->basestrpush.prev = NULL;
  pf->unget = 0;
  parsefile = pf;
}

static void popfile(void) {
  struct parsefile *pf = parsefile;
  INTOFF;
  if (pf->fd >= 0) close(pf->fd);
  if (pf->buf) ckfree(pf->buf);
  if (parsefile->spfree) freestrings(parsefile->spfree);
  while (pf->strpush) {
    popstring();
    freestrings(parsefile->spfree);
  }
  parsefile = pf->prev;
  ckfree(pf);
  INTON;
}

static void unwindfiles(struct parsefile *stop) {
  while (parsefile != stop) popfile();
}

/*
 * Return to top level.
 */
static void popallfiles(void) {
  unwindfiles(&basepf);
}

static char *commandtext(union node *);
static int dowait(int, struct job *);
static int getstatus(struct job *);
static int jobno(const struct job *);
static int restartjob(struct job *, int);
static int sprint_status(char *, int, int);
static int waitproc(int, int *);
static struct job *getjob(const char *, int);
static struct job *growjobtab(void);
static void cmdlist(union node *, int);
static void cmdputs(const char *);
static void cmdtxt(union node *);
static void forkchild(struct job *, union node *, int);
static void forkparent(struct job *, union node *, int, int);
static void freejob(struct job *);
static void set_curjob(struct job *, unsigned);
static void showpipe(struct job *, struct output *);
static void xtcsetpgrp(int, int);

static void set_curjob(struct job *jp, unsigned mode) {
  struct job *jp1;
  struct job **jpp, **curp;
  /* first remove from list */
  jpp = curp = &curjob;
  do {
    jp1 = *jpp;
    if (jp1 == jp) break;
    jpp = &jp1->prev_job;
  } while (1);
  *jpp = jp1->prev_job;
  /* Then re-insert in correct position */
  jpp = curp;
  switch (mode) {
    default:
    case CUR_DELETE:
      /* job being deleted */
      break;
    case CUR_RUNNING:
      /* newly created job or backgrounded job,
         put after all stopped jobs. */
      do {
        jp1 = *jpp;
        if (!JOBS || !jp1 || jp1->state != JOBSTOPPED) break;
        jpp = &jp1->prev_job;
      } while (1);
      /* FALLTHROUGH */
    case CUR_STOPPED:
      /* newly stopped job - becomes curjob */
      jp->prev_job = *jpp;
      *jpp = jp;
      break;
  }
}

/*
 * Turn job control on and off.
 *
 * Note:  This code assumes that the third arg to ioctl is a character
 * pointer, which is true on Berkeley systems but not System V.  Since
 * System V doesn't have job control yet, this isn't a problem now.
 *
 * Called with interrupts off.
 */
static void setjobctl(int on) {
  int fd;
  int pgrp;
  if (IsWindows()) return; /* TODO(jart) */
  if (on == jobctl || rootshell == 0) return;
  if (on) {
    int ofd;
    ofd = fd = sh_open(_PATH_TTY, O_RDWR, 1);
    if (fd < 0) {
      fd += 3;
      while (!isatty(fd))
        if (--fd < 0) goto out;
    }
    fd = savefd(fd, ofd);
    do { /* while we are in the background */
      if ((pgrp = tcgetpgrp(fd)) < 0) {
      out:
        sh_warnx("can't access tty; job control turned off");
        mflag = on = 0;
        goto close;
      }
      if (pgrp == getpgrp()) break;
      killpg(0, SIGTTIN);
    } while (1);
    initialpgrp = pgrp;
    setsignal(SIGTSTP);
    setsignal(SIGTTOU);
    setsignal(SIGTTIN);
    pgrp = rootpid;
    setpgid(0, pgrp);
    xtcsetpgrp(fd, pgrp);
  } else {
    /* turning job control off */
    fd = ttyfd;
    pgrp = initialpgrp;
    xtcsetpgrp(fd, pgrp);
    setpgid(0, pgrp);
    setsignal(SIGTSTP);
    setsignal(SIGTTOU);
    setsignal(SIGTTIN);
  close:
    close(fd);
    fd = -1;
  }
  ttyfd = fd;
  jobctl = on;
}

static int decode_signum(const char *string) {
  int signo = -1;
  if (is_number(string)) {
    signo = atoi(string);
    if (signo >= NSIG) signo = -1;
  }
  return signo;
}

static int killcmd(argc, argv)
int argc;
char **argv;
{
  int signo = -1;
  int list = 0;
  int i;
  int pid;
  struct job *jp;
  if (argc <= 1) {
  usage:
    sh_error("Usage: kill [-s sigspec | -signum | -sigspec] [pid | job]... or\n"
             "kill -l [exitstatus]");
  }
  if (**++argv == '-') {
    signo = decode_signal(*argv + 1, 1);
    if (signo < 0) {
      int c;
      while ((c = nextopt("ls:")) != '\0') switch (c) {
          default:
          case 'l':
            list = 1;
            break;
          case 's':
            signo = decode_signal(optionarg, 1);
            if (signo < 0) {
              sh_error("invalid signal number or name: %s", optionarg);
            }
            break;
        }
      argv = argptr;
    } else
      argv++;
  }
  if (!list && signo < 0) signo = SIGTERM;
  if ((signo < 0 || !*argv) ^ list) {
    goto usage;
  }
  if (list) {
    struct output *out;
    out = out1;
    if (!*argv) {
      outstr("0\n", out);
      for (i = 1; i < NSIG; i++) {
        outfmt(out, snlfmt, strsignal(i));
      }
      return 0;
    }
    signo = number(*argv);
    if (signo > 128) signo -= 128;
    if (0 < signo && signo < NSIG)
      outfmt(out, snlfmt, strsignal(signo));
    else
      sh_error("invalid signal number or exit status: %s", *argv);
    return 0;
  }
  i = 0;
  do {
    if (**argv == '%') {
      jp = getjob(*argv, 0);
      pid = -jp->ps[0].pid;
    } else
      pid = **argv == '-' ? -number(*argv + 1) : number(*argv);
    if (kill(pid, signo) != 0) {
      sh_warnx("%s\n", strerror(errno));
      i = 1;
    }
  } while (*++argv);
  return i;
}

static int jobno(const struct job *jp) {
  return jp - jobtab + 1;
}

static int fgcmd(int argc, char **argv) {
  struct job *jp;
  struct output *out;
  int mode;
  int retval;
  mode = (**argv == 'f') ? FORK_FG : FORK_BG;
  nextopt(nullstr);
  argv = argptr;
  out = out1;
  do {
    jp = getjob(*argv, 1);
    if (mode == FORK_BG) {
      set_curjob(jp, CUR_RUNNING);
      outfmt(out, "[%d] ", jobno(jp));
    }
    outstr(jp->ps->cmd, out);
    showpipe(jp, out);
    retval = restartjob(jp, mode);
  } while (*argv && *++argv);
  return retval;
}

static int bgcmd(int argc, char **argv) __attribute__((__alias__("fgcmd")));

static int restartjob(struct job *jp, int mode) {
  struct procstat *ps;
  int i;
  int status;
  int pgid;
  INTOFF;
  if (jp->state == JOBDONE) goto out;
  jp->state = JOBRUNNING;
  pgid = jp->ps->pid;
  if (mode == FORK_FG) xtcsetpgrp(ttyfd, pgid);
  killpg(pgid, SIGCONT);
  ps = jp->ps;
  i = jp->nprocs;
  do {
    if (WIFSTOPPED(ps->status)) {
      ps->status = -1;
    }
  } while (ps++, --i);
out:
  status = (mode == FORK_FG) ? waitforjob(jp) : 0;
  INTON;
  return status;
}

static int sprint_status(char *os, int status, int sigonly) {
  char *s = os;
  int st;
  st = WEXITSTATUS(status);
  if (!WIFEXITED(status)) {
    st = WSTOPSIG(status);
    if (!WIFSTOPPED(status)) st = WTERMSIG(status);
    if (sigonly) {
      if (st == SIGINT || st == SIGPIPE) goto out;
      if (WIFSTOPPED(status)) goto out;
    }
    s = stpncpy(s, strsignal(st), 32);
  } else if (!sigonly) {
    if (st)
      s += fmtstr(s, 16, "Done(%d)", st);
    else
      s = stpcpy(s, "Done");
  }
out:
  return s - os;
}

static void showjob(struct output *out, struct job *jp, int mode) {
  struct procstat *ps;
  struct procstat *psend;
  int col;
  int indent;
  char s[80];
  ps = jp->ps;
  if (mode & SHOW_PGID) {
    /* just output process (group) id of pipeline */
    outfmt(out, "%d\n", ps->pid);
    return;
  }
  col = fmtstr(s, 16, "[%d]   ", jobno(jp));
  indent = col;
  if (jp == curjob)
    s[col - 2] = '+';
  else if (curjob && jp == curjob->prev_job)
    s[col - 2] = '-';
  if (mode & SHOW_PID) col += fmtstr(s + col, 16, "%d ", ps->pid);
  psend = ps + jp->nprocs;
  if (jp->state == JOBRUNNING) {
    scopy("Running", s + col);
    col += strlen("Running");
  } else {
    int status = psend[-1].status;
    if (jp->state == JOBSTOPPED) status = jp->stopstatus;
    col += sprint_status(s + col, status, 0);
  }
  goto start;
  do {
    /* for each process */
    col = fmtstr(s, 48, " |\n%*c%d ", indent, ' ', ps->pid) - 3;
  start:
    outfmt(out, "%s%*c%s", s, 33 - col >= 0 ? 33 - col : 0, ' ', ps->cmd);
    if (!(mode & SHOW_PID)) {
      showpipe(jp, out);
      break;
    }
    if (++ps == psend) {
      outcslow('\n', out);
      break;
    }
  } while (1);
  jp->changed = 0;
  if (jp->state == JOBDONE) {
    TRACE(("showjob: freeing job %d\n", jobno(jp)));
    freejob(jp);
  }
}

static int jobscmd(int argc, char **argv) {
  int mode, m;
  struct output *out;
  mode = 0;
  while ((m = nextopt("lp")))
    if (m == 'l')
      mode = SHOW_PID;
    else
      mode = SHOW_PGID;
  out = out1;
  argv = argptr;
  if (*argv) do
      showjob(out, getjob(*argv, 0), mode);
    while (*++argv);
  else
    showjobs(out, mode);
  return 0;
}

/*
 * Print a list of jobs.  If "change" is nonzero, only print jobs whose
 * statuses have changed since the last call to showjobs.
 */
static void showjobs(struct output *out, int mode) {
  struct job *jp;
  TRACE(("showjobs(%x) called\n", mode));
  /* If not even one job changed, there is nothing to do */
  dowait(DOWAIT_NONBLOCK, NULL);
  for (jp = curjob; jp; jp = jp->prev_job) {
    if (!(mode & SHOW_CHANGED) || jp->changed) showjob(out, jp, mode);
  }
}

/*
 * Mark a job structure as unused.
 */
static void freejob(struct job *jp) {
  struct procstat *ps;
  int i;
  INTOFF;
  for (i = jp->nprocs, ps = jp->ps; --i >= 0; ps++) {
    if (ps->cmd != nullstr) ckfree(ps->cmd);
  }
  if (jp->ps != &jp->ps0) ckfree(jp->ps);
  jp->used = 0;
  set_curjob(jp, CUR_DELETE);
  INTON;
}

static int waitcmd(int argc, char **argv) {
  struct job *job;
  int retval;
  struct job *jp;
  nextopt(nullstr);
  retval = 0;
  argv = argptr;
  if (!*argv) {
    /* wait for all jobs */
    for (;;) {
      jp = curjob;
      while (1) {
        if (!jp) {
          /* no running procs */
          goto out;
        }
        if (jp->state == JOBRUNNING) break;
        jp->waited = 1;
        jp = jp->prev_job;
      }
      if (!dowait(DOWAIT_WAITCMD_ALL, 0)) goto sigout;
    }
  }
  retval = 127;
  do {
    if (**argv != '%') {
      int pid = number(*argv);
      job = curjob;
      goto start;
      do {
        if (job->ps[job->nprocs - 1].pid == pid) break;
        job = job->prev_job;
      start:
        if (!job) goto repeat;
      } while (1);
    } else
      job = getjob(*argv, 0);
    /* loop until process terminated or stopped */
    if (!dowait(DOWAIT_WAITCMD, job)) goto sigout;
    job->waited = 1;
    retval = getstatus(job);
  repeat:;
  } while (*++argv);
out:
  return retval;
sigout:
  retval = 128 + pending_sig;
  goto out;
}

/*
 * Convert a job name to a job structure.
 */
static struct job *getjob(const char *name, int getctl) {
  struct job *jp;
  struct job *found;
  const char *err_msg = "No such job: %s";
  unsigned num;
  int c;
  const char *p;
  char *(*match)(const char *, const char *);
  jp = curjob;
  p = name;
  if (!p) goto currentjob;
  if (*p != '%') goto err;
  c = *++p;
  if (!c) goto currentjob;
  if (!p[1]) {
    if (c == '+' || c == '%') {
    currentjob:
      err_msg = "No current job";
      goto check;
    } else if (c == '-') {
      if (jp) jp = jp->prev_job;
      err_msg = "No previous job";
    check:
      if (!jp) goto err;
      goto gotit;
    }
  }
  if (is_number(p)) {
    num = atoi(p);
    if (num > 0 && num <= njobs) {
      jp = jobtab + num - 1;
      if (jp->used) goto gotit;
      goto err;
    }
  }
  match = prefix;
  if (*p == '?') {
    match = strstr;
    p++;
  }
  found = 0;
  while (jp) {
    if (match(jp->ps[0].cmd, p)) {
      if (found) goto err;
      found = jp;
      err_msg = "%s: ambiguous";
    }
    jp = jp->prev_job;
  }
  if (!found) goto err;
  jp = found;
gotit:
  err_msg = "job %s not created under job control";
  if (getctl && jp->jobctl == 0) goto err;
  return jp;
err:
  sh_error(err_msg, name);
}

/*
 * Return a new job structure.
 * Called with interrupts off.
 */
struct job *makejob(union node *node, int nprocs) {
  int i;
  struct job *jp;
  for (i = njobs, jp = jobtab;; jp++) {
    if (--i < 0) {
      jp = growjobtab();
      break;
    }
    if (jp->used == 0) break;
    if (jp->state != JOBDONE || !jp->waited) continue;
    if (jobctl) continue;
    freejob(jp);
    break;
  }
  memset(jp, 0, sizeof(*jp));
  if (jobctl) jp->jobctl = 1;
  jp->prev_job = curjob;
  curjob = jp;
  jp->used = 1;
  jp->ps = &jp->ps0;
  if (nprocs > 1) {
    jp->ps = ckmalloc(nprocs * sizeof(struct procstat));
  }
  TRACE(("makejob(0x%lx, %d) returns %%%d\n", (long)node, nprocs, jobno(jp)));
  return jp;
}

#if defined(__GNUC__) && __GNUC__ >= 12
#pragma GCC diagnostic ignored "-Wuse-after-free"
#endif

static struct job *growjobtab(void) {
  unsigned len;
  long offset;
  struct job *jp, *jq;
  len = njobs * sizeof(*jp);
  jq = jobtab;
  jp = ckrealloc(jq, len + 4 * sizeof(*jp));
  offset = (char *)jp - (char *)jq;
  if (offset) {
    /* Relocate pointers */
    unsigned l = len;
    jq = (struct job *)((char *)jq + l);
    while (l) {
      l -= sizeof(*jp);
      jq--;
#define joff(p)  ((struct job *)((char *)(p) + l))
#define jmove(p) (p) = (void *)((char *)(p) + offset)
      if (likely(joff(jp)->ps == &jq->ps0)) jmove(joff(jp)->ps);
      if (joff(jp)->prev_job) jmove(joff(jp)->prev_job);
    }
    if (curjob) jmove(curjob);
#undef joff
#undef jmove
  }
  njobs += 4;
  jobtab = jp;
  jp = (struct job *)((char *)jp + len);
  jq = jp + 3;
  do {
    jq->used = 0;
  } while (--jq >= jp);
  return jp;
}

/*
 * Fork off a subshell.  If we are doing job control, give the subshell its
 * own process group.  Jp is a job structure that the job is to be added to.
 * N is the command that will be evaluated by the child.  Both jp and n may
 * be NULL.  The mode parameter can be one of the following:
 *  FORK_FG - Fork off a foreground process.
 *  FORK_BG - Fork off a background process.
 *  FORK_NOJOB - Like FORK_FG, but don't give the process its own
 *               process group even if job control is on.
 *
 * When job control is turned off, background processes have their standard
 * input redirected to /dev/null (except for the second and later processes
 * in a pipeline).
 *
 * Called with interrupts off.
 */
static void forkchild(struct job *jp, union node *n, int mode) {
  int lvforked;
  int oldlvl;
  TRACE(("Child shell %d\n", getpid()));
  oldlvl = shlvl;
  lvforked = vforked;
  if (!lvforked) {
    shlvl++;
    forkreset();
    /* do job control only in root shell */
    jobctl = 0;
  }
  if (mode != FORK_NOJOB && jp->jobctl && !oldlvl) {
    int pgrp;
    if (jp->nprocs == 0)
      pgrp = getpid();
    else
      pgrp = jp->ps[0].pid;
    /* This can fail because we are doing it in the parent also */
    (void)setpgid(0, pgrp);
    if (mode == FORK_FG) xtcsetpgrp(ttyfd, pgrp);
    setsignal(SIGTSTP);
    setsignal(SIGTTOU);
  } else if (mode == FORK_BG) {
    ignoresig(SIGINT);
    ignoresig(SIGQUIT);
    if (jp->nprocs == 0) {
      close(0);
      sh_open(_PATH_DEVNULL, O_RDONLY, 0);
    }
  }
  if (!oldlvl && iflag) {
    setsignal(SIGINT);
    setsignal(SIGQUIT);
    setsignal(SIGTERM);
  }
  if (lvforked) return;
  for (jp = curjob; jp; jp = jp->prev_job) freejob(jp);
}

static void forkparent(struct job *jp, union node *n, int mode, int pid) {
  if (pid < 0) {
    TRACE(("Fork failed, errno=%d", errno));
    if (jp) freejob(jp);
    sh_error("Cannot fork");
    __builtin_unreachable();
  }
  TRACE(("In parent shell:  child = %d\n", pid));
  if (!jp) return;
  if (mode != FORK_NOJOB && jp->jobctl) {
    int pgrp;
    if (jp->nprocs == 0)
      pgrp = pid;
    else
      pgrp = jp->ps[0].pid;
    /* This can fail because we are doing it in the child also */
    (void)setpgid(pid, pgrp);
  }
  if (mode == FORK_BG) {
    backgndpid = pid; /* set $! */
    set_curjob(jp, CUR_RUNNING);
  }
  if (jp) {
    struct procstat *ps = &jp->ps[jp->nprocs++];
    ps->pid = pid;
    ps->status = -1;
    ps->cmd = nullstr;
    if (jobctl && n) ps->cmd = commandtext(n);
  }
}

static int forkshell(struct job *jp, union node *n, int mode) {
  int pid;
  TRACE(("forkshell(%%%d, %p, %d) called\n", jobno(jp), n, mode));
  pid = fork();
  if (pid == 0) {
    forkchild(jp, n, mode);
  } else {
    forkparent(jp, n, mode, pid);
  }
  return pid;
}

static struct job *vforkexec(union node *n, char **argv, const char *path,
                             int idx) {
  struct job *jp;
  int pid;
  jp = makejob(n, 1);
  sigblockall(NULL);
  vforked++;
  pid = vfork();
  if (!pid) {
    forkchild(jp, n, FORK_FG);
    sigclearmask();
    shellexec(argv, path, idx);
    __builtin_unreachable();
  }
  vforked = 0;
  sigclearmask();
  forkparent(jp, n, FORK_FG, pid);
  return jp;
}

/*
 * Wait for job to finish.
 *
 * Under job control we have the problem that while a child process is
 * running interrupts generated by the user are sent to the child but not
 * to the shell.  This means that an infinite loop started by an inter-
 * active user may be hard to kill.  With job control turned off, an
 * interactive user may place an interactive program inside a loop.  If
 * the interactive program catches interrupts, the user doesn't want
 * these interrupts to also abort the loop.  The approach we take here
 * is to have the shell ignore interrupt signals while waiting for a
 * foreground process to terminate, and then send itself an interrupt
 * signal if the child process was terminated by an interrupt signal.
 * Unfortunately, some programs want to do a bit of cleanup and then
 * exit on interrupt; unless these processes terminate themselves by
 * sending a signal to themselves (instead of calling exit) they will
 * confuse this approach.
 *
 * Called with interrupts off.
 */
static int waitforjob(struct job *jp) {
  int st;
  TRACE(("waitforjob(%%%d) called\n", jp ? jobno(jp) : 0));
  dowait(jp ? DOWAIT_BLOCK : DOWAIT_NONBLOCK, jp);
  if (!jp) return exitstatus;
  st = getstatus(jp);
  if (jp->jobctl) {
    xtcsetpgrp(ttyfd, rootpid);
    /*
     * This is truly gross.
     * If we're doing job control, then we did a TIOCSPGRP which
     * caused us (the shell) to no longer be in the controlling
     * session -- so we wouldn't have seen any ^C/SIGINT.  So, we
     * intuit from the subprocess exit status whether a SIGINT
     * occurred, and if so interrupt ourselves.  Yuck.  - mycroft
     */
    if (jp->sigint) raise(SIGINT);
  }
  if (!JOBS || jp->state == JOBDONE) freejob(jp);
  return st;
}

/*
 * Wait for a process to terminate.
 */
static int waitone(int block, struct job *job) {
  int pid;
  int status;
  struct job *jp;
  struct job *thisjob = NULL;
  int state;
  INTOFF;
  TRACE(("dowait(%d) called\n", block));
  pid = waitproc(block, &status);
  TRACE(("wait returns pid %d, status=%d\n", pid, status));
  if (pid <= 0) goto out;
  for (jp = curjob; jp; jp = jp->prev_job) {
    struct procstat *sp;
    struct procstat *spend;
    if (jp->state == JOBDONE) continue;
    state = JOBDONE;
    spend = jp->ps + jp->nprocs;
    sp = jp->ps;
    do {
      if (sp->pid == pid) {
        TRACE(("Job %d: changing status of proc %d from 0x%x to 0x%x\n",
               jobno(jp), pid, sp->status, status));
        sp->status = status;
        thisjob = jp;
      }
      if (sp->status == -1) state = JOBRUNNING;
      if (state == JOBRUNNING) continue;
      if (WIFSTOPPED(sp->status)) {
        jp->stopstatus = sp->status;
        state = JOBSTOPPED;
      }
    } while (++sp < spend);
    if (thisjob) goto gotjob;
  }
  goto out;
gotjob:
  if (state != JOBRUNNING) {
    thisjob->changed = 1;
    if (thisjob->state != state) {
      TRACE(("Job %d: changing state from %d to %d\n", jobno(thisjob),
             thisjob->state, state));
      thisjob->state = state;
      if (state == JOBSTOPPED) {
        set_curjob(thisjob, CUR_STOPPED);
      }
    }
  }
out:
  INTON;
  if (thisjob && thisjob == job) {
    char s[48 + 1];
    int len;
    len = sprint_status(s, status, 1);
    if (len) {
      s[len] = '\n';
      s[len + 1] = 0;
      outstr(s, out2);
    }
  }
  return pid;
}

static int dowait(int block, struct job *jp) {
  int gotchld = *(volatile int *)&gotsigchld;
  int rpid;
  int pid;
  if (jp && jp->state != JOBRUNNING) block = DOWAIT_NONBLOCK;
  if (block == DOWAIT_NONBLOCK && !gotchld) return 1;
  rpid = 1;
  do {
    pid = waitone(block, jp);
    rpid &= !!pid;
    block &= ~DOWAIT_WAITCMD_ALL;
    if (!pid || (jp && jp->state != JOBRUNNING)) block = DOWAIT_NONBLOCK;
  } while (pid >= 0);
  return rpid;
}

/*
 * Do a wait system call.  If block is zero, we return -1 rather than
 * blocking.  If block is DOWAIT_WAITCMD, we return 0 when a signal
 * other than SIGCHLD interrupted the wait.
 *
 * We use sigsuspend in conjunction with a non-blocking wait3 in
 * order to ensure that waitcmd exits promptly upon the reception
 * of a signal.
 *
 * For code paths other than waitcmd we either use a blocking wait3
 * or a non-blocking wait3.  For the latter case the caller of dowait
 * must ensure that it is called over and over again until all dead
 * children have been reaped.  Otherwise zombies may linger.
 */
static int waitproc(int block, int *status) {
  sigset_t oldmask;
  int flags = block == DOWAIT_BLOCK ? 0 : WNOHANG;
  int err;
  if (jobctl) flags |= WUNTRACED;
  do {
    gotsigchld = 0;
    do err = wait3(status, flags, NULL);
    while (err < 0 && errno == EINTR);
    if (err || (err = -!block)) break;
    sigblockall(&oldmask);
    while (!gotsigchld && !pending_sig) sigsuspend(&oldmask);
    sigclearmask();
  } while (gotsigchld);
  return err;
}

/*
 * return 1 if there are stopped jobs, otherwise 0
 */
static int stoppedjobs(void) {
  struct job *jp;
  int retval;
  retval = 0;
  if (job_warning) goto out;
  jp = curjob;
  if (jp && jp->state == JOBSTOPPED) {
    outstr("You have stopped jobs.\n", out2);
    job_warning = 2;
    retval++;
  }
out:
  return retval;
}

/*
 * Return a string identifying a command (to be printed by the
 * jobs command).
 */
static char *commandtext(union node *n) {
  char *name;
  STARTSTACKSTR(cmdnextc);
  cmdtxt(n);
  name = stackblock();
  TRACE(("commandtext: name %p, end %p\n", name, cmdnextc));
  return savestr(name);
}

static void cmdtxt(union node *n) {
  union node *np;
  struct nodelist *lp;
  const char *p;
  char s[2];
  if (!n) return;
  switch (n->type) {
    default:
    case NPIPE:
      lp = n->npipe.cmdlist;
      for (;;) {
        cmdtxt(lp->n);
        lp = lp->next;
        if (!lp) break;
        cmdputs(" | ");
      }
      break;
    case NSEMI:
      p = "; ";
      goto binop;
    case NAND:
      p = " && ";
      goto binop;
    case NOR:
      p = " || ";
    binop:
      cmdtxt(n->nbinary.ch1);
      cmdputs(p);
      n = n->nbinary.ch2;
      goto donode;
    case NREDIR:
    case NBACKGND:
      n = n->nredir.n;
      goto donode;
    case NNOT:
      cmdputs("!");
      n = n->nnot.com;
    donode:
      cmdtxt(n);
      break;
    case NIF:
      cmdputs("if ");
      cmdtxt(n->nif.test);
      cmdputs("; then ");
      if (n->nif.elsepart) {
        cmdtxt(n->nif.ifpart);
        cmdputs("; else ");
        n = n->nif.elsepart;
      } else {
        n = n->nif.ifpart;
      }
      p = "; fi";
      goto dotail;
    case NSUBSHELL:
      cmdputs("(");
      n = n->nredir.n;
      p = ")";
      goto dotail;
    case NWHILE:
      p = "while ";
      goto until;
    case NUNTIL:
      p = "until ";
    until:
      cmdputs(p);
      cmdtxt(n->nbinary.ch1);
      n = n->nbinary.ch2;
      p = "; done";
    dodo:
      cmdputs("; do ");
    dotail:
      cmdtxt(n);
      goto dotail2;
    case NFOR:
      cmdputs("for ");
      cmdputs(n->nfor.var_);
      cmdputs(" in ");
      cmdlist(n->nfor.args, 1);
      n = n->nfor.body;
      p = "; done";
      goto dodo;
    case NDEFUN:
      cmdputs(n->ndefun.text);
      p = "() { ... }";
      goto dotail2;
    case NCMD:
      cmdlist(n->ncmd.args, 1);
      cmdlist(n->ncmd.redirect, 0);
      break;
    case NARG:
      p = n->narg.text;
    dotail2:
      cmdputs(p);
      break;
    case NHERE:
    case NXHERE:
      p = "<<...";
      goto dotail2;
    case NCASE:
      cmdputs("case ");
      cmdputs(n->ncase.expr->narg.text);
      cmdputs(" in ");
      for (np = n->ncase.cases; np; np = np->nclist.next) {
        cmdtxt(np->nclist.pattern);
        cmdputs(") ");
        cmdtxt(np->nclist.body);
        cmdputs(";; ");
      }
      p = "esac";
      goto dotail2;
    case NTO:
      p = ">";
      goto redir;
    case NCLOBBER:
      p = ">|";
      goto redir;
    case NAPPEND:
      p = ">>";
      goto redir;
    case NTOFD:
      p = ">&";
      goto redir;
    case NFROM:
      p = "<";
      goto redir;
    case NFROMFD:
      p = "<&";
      goto redir;
    case NFROMTO:
      p = "<>";
    redir:
      s[0] = n->nfile.fd + '0';
      s[1] = '\0';
      cmdputs(s);
      cmdputs(p);
      if (n->type == NTOFD || n->type == NFROMFD) {
        s[0] = n->ndup.dupfd + '0';
        p = s;
        goto dotail2;
      } else {
        n = n->nfile.fname;
        goto donode;
      }
  }
}

static void cmdlist(union node *np, int sep) {
  for (; np; np = np->narg.next) {
    if (!sep) cmdputs(spcstr);
    cmdtxt(np);
    if (sep && np->narg.next) cmdputs(spcstr);
  }
}

static void cmdputs(const char *s) {
  const char *p, *str;
  char cc[2] = " ";
  char *nextc;
  signed char c;
  int subtype = 0;
  int quoted = 0;
  static const char vstype[VSTYPE + 1][4] = {
      "", "}", "-", "+", "?", "=", "%", "%%", "#", "##",
  };
  nextc = makestrspace((strlen(s) + 1) * 8, cmdnextc);
  p = s;
  while ((c = *p++) != 0) {
    str = 0;
    switch (c) {
      case CTLESC:
        c = *p++;
        break;
      case CTLVAR:
        subtype = *p++;
        if ((subtype & VSTYPE) == VSLENGTH)
          str = "${#";
        else
          str = "${";
        goto dostr;
      case CTLENDVAR:
        str = &"\"}"[!quoted];
        quoted >>= 1;
        subtype = 0;
        goto dostr;
      case CTLBACKQ:
        str = "$(...)";
        goto dostr;
      case CTLARI:
        str = "$((";
        goto dostr;
      case CTLENDARI:
        str = "))";
        goto dostr;
      case CTLQUOTEMARK:
        quoted ^= 1;
        c = '"';
        break;
      case '=':
        if (subtype == 0) break;
        if ((subtype & VSTYPE) != VSNORMAL) quoted <<= 1;
        str = vstype[subtype & VSTYPE];
        if (subtype & VSNUL)
          c = ':';
        else
          goto checkstr;
        break;
      case '\'':
      case '\\':
      case '"':
      case '$':
        /* These can only happen inside quotes */
        cc[0] = c;
        str = cc;
        c = '\\';
        break;
      default:
        break;
    }
    USTPUTC(c, nextc);
  checkstr:
    if (!str) continue;
  dostr:
    while ((c = *str++)) {
      USTPUTC(c, nextc);
    }
  }
  if (quoted & 1) {
    USTPUTC('"', nextc);
  }
  *nextc = 0;
  cmdnextc = nextc;
}

static void showpipe(struct job *jp, struct output *out) {
  struct procstat *sp;
  struct procstat *spend;
  spend = jp->ps + jp->nprocs;
  for (sp = jp->ps + 1; sp < spend; sp++) outfmt(out, " | %s", sp->cmd);
  outcslow('\n', out);
  flushall();
}

static void xtcsetpgrp(int fd, int pgrp) {
  int err;
  sigblockall(NULL);
  err = tcsetpgrp(fd, pgrp);
  sigclearmask();
  if (err) sh_error("Cannot set tty process group (%s)", strerror(errno));
}

static int getstatus(struct job *job) {
  int status;
  int retval;
  status = job->ps[job->nprocs - 1].status;
  retval = WEXITSTATUS(status);
  if (!WIFEXITED(status)) {
    retval = WSTOPSIG(status);
    if (!WIFSTOPPED(status)) {
      /* XXX: limits number of signals */
      retval = WTERMSIG(status);
      if (retval == SIGINT) job->sigint = 1;
    }
    retval += 128;
  }
  TRACE(("getstatus: job %d, nproc %d, status %x, retval %x\n", jobno(job),
         job->nprocs, status, retval));
  return retval;
}

/** handle one line of the read command.
 *  more fields than variables -> remainder shall be part of last variable.
 *  less fields than variables -> remaining variables unset.
 *
 *  @param line complete line of input
 *  @param ac argument count
 *  @param ap argument (variable) list
 *  @param len length of line including trailing '\0'
 */
static void readcmd_handle_line(char *s, int ac, char **ap) {
  struct arglist arglist;
  struct strlist *sl;
  s = grabstackstr(s);
  arglist.lastp = &arglist.list;
  ifsbreakup(s, ac, &arglist);
  *arglist.lastp = NULL;
  ifsfree();
  sl = arglist.list;
  do {
    if (!sl) {
      /* nullify remaining arguments */
      do {
        setvar(*ap, nullstr, 0);
      } while (*++ap);
      return;
    }
    /* set variable to field */
    rmescapes(sl->text, 0);
    setvar(*ap, sl->text, 0);
    sl = sl->next;
  } while (*++ap);
}

/*
 * The read builtin.  The -e option causes backslashes to escape the
 * following character. The -p option followed by an argument prompts
 * with the argument.
 *
 * This uses unbuffered input, which may be avoidable in some cases.
 */
static int readcmd(int argc, char **argv) {
  char **ap;
  char c;
  int rflag;
  char *prompt;
  char *p;
  int startloc;
  int newloc;
  int status;
  int i;
  rflag = 0;
  prompt = NULL;
  while ((i = nextopt("p:r")) != '\0') {
    if (i == 'p')
      prompt = optionarg;
    else
      rflag = 1;
  }
  if (prompt && isatty(0)) {
    outstr(prompt, out2);
  }
  if (!*(ap = argptr)) sh_error("arg count");
  status = 0;
  STARTSTACKSTR(p);
  goto start;
  for (;;) {
    switch (read(0, &c, 1)) {
      case 1:
        break;
      default:
        if (errno == EINTR && !pending_sig) continue;
        /* fall through */
      case 0:
        status = 1;
        goto out;
    }
    if (!c) continue;
    if (newloc >= startloc) {
      if (c == '\n') goto resetbs;
      goto put;
    }
    if (!rflag && c == '\\') {
      newloc = p - (char *)stackblock();
      continue;
    }
    if (c == '\n') break;
  put:
    CHECKSTRSPACE(2, p);
    if (strchr(qchars, c)) USTPUTC(CTLESC, p);
    USTPUTC(c, p);
    if (newloc >= startloc) {
    resetbs:
      recordregion(startloc, newloc, 0);
    start:
      startloc = p - (char *)stackblock();
      newloc = startloc - 1;
    }
  }
out:
  recordregion(startloc, p - (char *)stackblock(), 0);
  STACKSTRNUL(p);
  readcmd_handle_line(p + 1, argc - (ap - argv), ap);
  return status;
}

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § the unbourne shell » builtins » umask                     ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│─╝
  This code was ripped from pdksh 5.2.14 and hacked for use with
  dash by Herbert Xu. Public Domain. */

static int umaskcmd(int argc, char **argv) {
  char *ap;
  int mask;
  int i;
  int symbolic_mode = 0;
  while ((i = nextopt("S")) != '\0') {
    symbolic_mode = 1;
  }
  INTOFF;
  mask = umask(0);
  umask(mask);
  INTON;
  if ((ap = *argptr) == NULL) {
    if (symbolic_mode) {
      char buf[18];
      int j;
      mask = ~mask;
      ap = buf;
      for (i = 0; i < 3; i++) {
        *ap++ = "ugo"[i];
        *ap++ = '=';
        for (j = 0; j < 3; j++)
          if (mask & (1u << (8 - (3 * i + j)))) *ap++ = "rwx"[j];
        *ap++ = ',';
      }
      ap[-1] = '\0';
      out1fmt("%s\n", buf);
    } else {
      out1fmt("%.4o\n", mask);
    }
  } else {
    int new_mask;
    if (isdigit((unsigned char)*ap)) {
      new_mask = 0;
      do {
        if (*ap >= '8' || *ap < '0') sh_error(illnum, *argptr);
        new_mask = (new_mask << 3) + (*ap - '0');
      } while (*++ap != '\0');
    } else {
      int positions, new_val;
      char op;
      mask = ~mask;
      new_mask = mask;
      positions = 0;
      while (*ap) {
        while (*ap && strchr("augo", *ap)) switch (*ap++) {
            case 'a':
              positions |= 0111;
              break;
            case 'u':
              positions |= 0100;
              break;
            case 'g':
              positions |= 0010;
              break;
            case 'o':
              positions |= 0001;
              break;
          }
        if (!positions) positions = 0111; /* default is a */
        if (!strchr("=+-", op = *ap)) break;
        ap++;
        new_val = 0;
        while (*ap && strchr("rwxugoXs", *ap)) switch (*ap++) {
            case 'r':
              new_val |= 04;
              break;
            case 'w':
              new_val |= 02;
              break;
            case 'x':
              new_val |= 01;
              break;
            case 'u':
              new_val |= mask >> 6;
              break;
            case 'g':
              new_val |= mask >> 3;
              break;
            case 'o':
              new_val |= mask >> 0;
              break;
            case 'X':
              if (mask & 0111) new_val |= 01;
              break;
            case 's': /* ignored */
              break;
          }
        new_val = (new_val & 07) * positions;
        switch (op) {
          case '-':
            new_mask &= ~new_val;
            break;
          case '=':
            new_mask = new_val | (new_mask & ~(positions * 07));
            break;
          case '+':
            new_mask |= new_val;
        }
        if (*ap == ',') {
          positions = 0;
          ap++;
        } else if (!strchr("=+-", *ap))
          break;
      }
      if (*ap) {
        sh_error("Illegal mode: %s", *argptr);
        return 1;
      }
      new_mask = ~new_mask;
    }
    umask(new_mask);
  }
  return 0;
}

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § the unbourne shell » builtins » ulimit                    ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│─╝
  This code, originally by Doug Gwyn, Doug Kingston, Eric Gisin, and
  Michael Rendell was ripped from pdksh 5.0.8 and hacked for use with
  ash by J.T. Conklin. Public Domain. */

enum limtype { SOFT = 0x1, HARD = 0x2 };

static void printlim(enum limtype how, const struct rlimit *limit,
                     const struct limits *l) {
  uint64_t val;
  val = limit->rlim_max;
  if (how & SOFT) val = limit->rlim_cur;
  if (val == RLIM_INFINITY)
    out1fmt("unlimited\n");
  else {
    val /= l->factor;
    out1fmt("%ld\n", (int64_t)val);
  }
}

static int ulimitcmd(int argc, char **argv) {
  static const struct limits limits[] = {{(char *)0, 0, 0, '\0'}};
  int c;
  uint64_t val = 0;
  enum limtype how = SOFT | HARD;
  const struct limits *l;
  int set, all = 0;
  int optc, what;
  struct rlimit limit;
  what = 'f';
  while ((optc = nextopt("HSa")) != '\0') {
    switch (optc) {
      case 'H':
        how = HARD;
        break;
      case 'S':
        how = SOFT;
        break;
      case 'a':
        all = 1;
        break;
      default:
        what = optc;
    }
  }
  for (l = limits; l->option != what; l++);
  set = *argptr ? 1 : 0;
  if (set) {
    char *p = *argptr;
    if (all || argptr[1]) sh_error("too many arguments");
    if (strcmp(p, "unlimited") == 0)
      val = RLIM_INFINITY;
    else {
      val = (uint64_t)0;
      while ((c = *p++) >= '0' && c <= '9') {
        val = (val * 10) + (long)(c - '0');
        if (val < (uint64_t)0) break;
      }
      if (c) sh_error("bad number");
      val *= l->factor;
    }
  }
  if (all) {
    for (l = limits; l->name; l++) {
      getrlimit(l->cmd, &limit);
      out1fmt("%-20s ", l->name);
      printlim(how, &limit, l);
    }
    return 0;
  }
  getrlimit(l->cmd, &limit);
  if (set) {
    if (how & HARD) limit.rlim_max = val;
    if (how & SOFT) limit.rlim_cur = val;
    if (setrlimit(l->cmd, &limit) < 0)
      sh_error("error setting limit (%s)", strerror(errno));
  } else {
    printlim(how, &limit, l);
  }
  return 0;
}

/*
 * Produce a possibly single quoted string suitable as input to the shell.
 * The return string is allocated on the stack.
 */
static char *single_quote(const char *s) {
  char *p;
  STARTSTACKSTR(p);
  do {
    char *q;
    unsigned len;
    len = strchrnul(s, '\'') - s;
    q = p = makestrspace(len + 3, p);
    *q++ = '\'';
    q = mempcpy(q, s, len);
    *q++ = '\'';
    s += len;
    STADJUST(q - p, p);
    len = strspn(s, "'");
    if (!len) break;
    q = p = makestrspace(len + 3, p);
    *q++ = '"';
    q = mempcpy(q, s, len);
    *q++ = '"';
    s += len;
    STADJUST(q - p, p);
  } while (*s);
  USTPUTC(0, p);
  return stackblock();
}

/*
 * Process the shell command line arguments.
 */
static int procargs(int argc, char **argv) {
  int i;
  const char *xminusc;
  char **xargv;
  int login;
  xargv = argv;
  login = xargv[0] && xargv[0][0] == '-';
  arg0 = xargv[0];
  if (argc > 0) xargv++;
  for (i = 0; i < NOPTS; i++) optlist[i] = 2;
  argptr = xargv;
  login |= options(1);
  xargv = argptr;
  xminusc = minusc;
  if (*xargv == NULL) {
    if (xminusc) sh_error("-c requires an argument");
    sflag = 1;
  }
  /* JART: fuck tty check this is documented behavior w/ no args */
  if (iflag == 2 && sflag == 1 /* && isatty(0) && isatty(1) */) iflag = 1;
  if (mflag == 2) mflag = iflag;
  for (i = 0; i < NOPTS; i++)
    if (optlist[i] == 2) optlist[i] = 0;
  /* POSIX 1003.2: first arg after -c cmd is $0, remainder $1... */
  if (xminusc) {
    minusc = *xargv++;
    if (*xargv) goto setarg0;
  } else if (!sflag) {
    setinputfile(*xargv, 0);
  setarg0:
    arg0 = *xargv++;
  }
  shellparam.p = xargv;
  shellparam.optind = 1;
  shellparam.optoff = -1;
  /* assert(shellparam.malloc == 0 && shellparam.nparam == 0); */
  while (*xargv) {
    shellparam.nparam++;
    xargv++;
  }
  optschanged();
  return login;
}

static void optschanged(void) {
  setinteractive(iflag);
  setjobctl(mflag);
}

static void setoption(int flag, int val) {
  int i;
  for (i = 0; i < NOPTS; i++)
    if (optletters[i] == flag) {
      optlist[i] = val;
      if (val) {
        /* #%$ hack for ksh semantics */
        if (flag == 'V')
          Eflag = 0;
        else if (flag == 'E')
          Vflag = 0;
      }
      return;
    }
  sh_error("Illegal option -%c", flag);
  __builtin_unreachable();
}

/*
 * Process shell options.  The global variable argptr contains a pointer
 * to the argument list; we advance it past the options.
 */
static int options(int cmdline) {
  char *p;
  int val;
  int c;
  int login = 0;
  if (cmdline) minusc = NULL;
  while ((p = *argptr) != NULL) {
    argptr++;
    if ((c = *p++) == '-') {
      val = 1;
      if (p[0] == '\0' || (p[0] == '-' && p[1] == '\0')) {
        if (!cmdline) {
          /* "-" means turn off -x and -v */
          if (p[0] == '\0') xflag = vflag = 0;
          /* "--" means reset params */
          else if (*argptr == NULL)
            setparam(argptr);
        }
        break; /* "-" or  "--" terminates options */
      }
    } else if (c == '+') {
      val = 0;
    } else {
      argptr--;
      break;
    }
    while ((c = *p++) != '\0') {
      if (c == 'c' && cmdline) {
        minusc = p; /* command is after shell args*/
      } else if (c == 'l' && cmdline) {
        login = 1;
      } else if (c == 'o') {
        minus_o(*argptr, val);
        if (*argptr) argptr++;
      } else {
        setoption(c, val);
      }
    }
  }
  return login;
}

static void minus_o(char *name, int val) {
  int i;
  if (name == NULL) {
    if (val) {
      outstr("Current option settings\n", out1);
      for (i = 0; i < NOPTS; i++) {
        out1fmt("%-16s%s\n", optnames[i], optlist[i] ? "on" : "off");
      }
    } else {
      for (i = 0; i < NOPTS; i++) {
        out1fmt("set %s %s\n", optlist[i] ? "-o" : "+o", optnames[i]);
      }
    }
  } else {
    for (i = 0; i < NOPTS; i++)
      if (equal(name, optnames[i])) {
        optlist[i] = val;
        return;
      }
    sh_error("Illegal option -o %s", name);
  }
}

/*
 * Set the shell parameters.
 */
static void setparam(char **argv) {
  char **newparam;
  char **ap;
  int nparam;
  for (nparam = 0; argv[nparam]; nparam++);
  ap = newparam = ckmalloc((nparam + 1) * sizeof *ap);
  while (*argv) {
    *ap++ = savestr(*argv++);
  }
  *ap = NULL;
  freeparam(&shellparam);
  shellparam.malloc = 1;
  shellparam.nparam = nparam;
  shellparam.p = newparam;
  shellparam.optind = 1;
  shellparam.optoff = -1;
}

/*
 * Free the list of positional parameters.
 */
static void freeparam(volatile struct shparam *param) {
  char **ap;
  if (param->malloc) {
    for (ap = param->p; *ap; ap++) ckfree(*ap);
    ckfree(param->p);
  }
}

/*
 * The shift builtin command.
 */
static int shiftcmd(int argc, char **argv) {
  int n;
  char **ap1, **ap2;
  n = 1;
  if (argc > 1) n = number(argv[1]);
  if (n > shellparam.nparam) sh_error("can't shift that many");
  INTOFF;
  shellparam.nparam -= n;
  for (ap1 = shellparam.p; --n >= 0; ap1++) {
    if (shellparam.malloc) ckfree(*ap1);
  }
  ap2 = shellparam.p;
  while ((*ap2++ = *ap1++) != NULL);
  shellparam.optind = 1;
  shellparam.optoff = -1;
  INTON;
  return 0;
}

/*
 * The set command builtin.
 */
static int setcmd(int argc, char **argv) {
  if (argc == 1) return showvars(nullstr, 0, VUNSET);
  INTOFF;
  options(0);
  optschanged();
  if (*argptr != NULL) {
    setparam(argptr);
  }
  INTON;
  return 0;
}

static void getoptsreset(value) const char *value;
{
  shellparam.optind = number(value) ?: 1;
  shellparam.optoff = -1;
}

/*
 * The getopts builtin.  Shellparam.optnext points to the next argument
 * to be processed.  Shellparam.optptr points to the next character to
 * be processed in the current argument.  If shellparam.optnext is NULL,
 * then it's the first time getopts has been called.
 */
static int getoptscmd(int argc, char **argv) {
  char **optbase;
  if (argc < 3)
    sh_error("Usage: getopts optstring var [arg]");
  else if (argc == 3) {
    optbase = shellparam.p;
    if ((unsigned)shellparam.optind > shellparam.nparam + 1) {
      shellparam.optind = 1;
      shellparam.optoff = -1;
    }
  } else {
    optbase = &argv[3];
    if ((unsigned)shellparam.optind > argc - 2) {
      shellparam.optind = 1;
      shellparam.optoff = -1;
    }
  }
  return getopts(argv[1], argv[2], optbase);
}

static int getopts(char *optstr, char *optvar, char **optfirst) {
  char *p, *q;
  char c = '?';
  int done = 0;
  char s[2];
  char **optnext;
  int ind = shellparam.optind;
  int off = shellparam.optoff;
  shellparam.optind = -1;
  optnext = optfirst + ind - 1;
  if (ind <= 1 || off < 0 || strlen(optnext[-1]) < off)
    p = NULL;
  else
    p = optnext[-1] + off;
  if (p == NULL || *p == '\0') {
    /* Current word is done, advance */
    p = *optnext;
    if (p == NULL || *p != '-' || *++p == '\0') {
    atend:
      p = NULL;
      done = 1;
      goto out;
    }
    optnext++;
    if (p[0] == '-' && p[1] == '\0') /* check for "--" */
      goto atend;
  }
  c = *p++;
  for (q = optstr; *q != c;) {
    if (*q == '\0') {
      if (optstr[0] == ':') {
        s[0] = c;
        s[1] = '\0';
        setvar("OPTARG", s, 0);
      } else {
        outfmt(&errout, "Illegal option -%c\n", c);
        (void)unsetvar("OPTARG");
      }
      c = '?';
      goto out;
    }
    if (*++q == ':') q++;
  }
  if (*++q == ':') {
    if (*p == '\0' && (p = *optnext) == NULL) {
      if (optstr[0] == ':') {
        s[0] = c;
        s[1] = '\0';
        setvar("OPTARG", s, 0);
        c = ':';
      } else {
        outfmt(&errout, "No arg for -%c option\n", c);
        (void)unsetvar("OPTARG");
        c = '?';
      }
      goto out;
    }
    if (p == *optnext) optnext++;
    setvar("OPTARG", p, 0);
    p = NULL;
  } else
    setvar("OPTARG", nullstr, 0);
out:
  ind = optnext - optfirst + 1;
  setvarint("OPTIND", ind, VNOFUNC);
  s[0] = c;
  s[1] = '\0';
  setvar(optvar, s, 0);
  shellparam.optoff = p ? p - *(optnext - 1) : -1;
  shellparam.optind = ind;
  return done;
}

/*
 * XXX - should get rid of.  have all builtins use getopt(3).  the
 * library getopt must have the BSD extension function variable "optreset"
 * otherwise it can't be used within the shell safely.
 *
 * Standard option processing (a la getopt) for builtin routines.  The
 * only argument that is passed to nextopt is the option string; the
 * other arguments are unnecessary.  It return the character, or '\0' on
 * end of input.
 */
static int nextopt(const char *optstring) {
  char *p;
  const char *q;
  char c;
  if ((p = optptr) == NULL || *p == '\0') {
    p = *argptr;
    if (p == NULL || *p != '-' || *++p == '\0') return '\0';
    argptr++;
    if (p[0] == '-' && p[1] == '\0') /* check for "--" */
      return '\0';
  }
  c = *p++;
  for (q = optstring; *q != c;) {
    if (*q == '\0') sh_error("Illegal option -%c", c);
    if (*++q == ':') q++;
  }
  if (*++q == ':') {
    if (*p == '\0' && (p = *argptr++) == NULL) {
      sh_error("No arg for -%c option", c);
    }
    optionarg = p;
    p = NULL;
  }
  optptr = p;
  return c;
}

/* values returned by readtoken */
static int isassignment(const char *p) {
  const char *q = endofname(p);
  if (p == q) return 0;
  return *q == '=';
}

static inline int realeofmark(const char *eofmark) {
  return eofmark && eofmark != FAKEEOFMARK;
}

/*
 * Read and parse a command.  Returns NEOF on end of file.  (NULL is a
 * valid parse tree indicating a blank line.)
 */
static union node *parsecmd(int interact) {
  tokpushback = 0;
  checkkwd = 0;
  heredoclist = 0;
  doprompt = interact;
  if (doprompt) setprompt(doprompt);
  needprompt = 0;
  return list(1);
}

static union node *list(int nlflag) {
  int chknl = nlflag & 1 ? 0 : CHKNL;
  union node *n1, *n2, *n3;
  int tok;
  n1 = NULL;
  for (;;) {
    checkkwd = chknl | CHKKWD | CHKALIAS;
    tok = readtoken();
    switch (tok) {
      case TNL:
        parseheredoc();
        return n1;
      case TEOF:
        if (!n1 && !chknl) n1 = NEOF;
      out_eof:
        parseheredoc();
        tokpushback++;
        lasttoken = TEOF;
        return n1;
    }
    tokpushback++;
    if (nlflag == 2 && tokendlist[tok]) return n1;
    nlflag |= 2;
    n2 = andor();
    tok = readtoken();
    if (tok == TBACKGND) {
      if (n2->type == NPIPE) {
        n2->npipe.backgnd = 1;
      } else {
        if (n2->type != NREDIR) {
          n3 = stalloc(sizeof(struct nredir));
          n3->nredir.n = n2;
          n3->nredir.redirect = NULL;
          n2 = n3;
        }
        n2->type = NBACKGND;
      }
    }
    if (n1 == NULL) {
      n1 = n2;
    } else {
      n3 = (union node *)stalloc(sizeof(struct nbinary));
      n3->type = NSEMI;
      n3->nbinary.ch1 = n1;
      n3->nbinary.ch2 = n2;
      n1 = n3;
    }
    switch (tok) {
      case TEOF:
        goto out_eof;
      case TNL:
        tokpushback++;
        /* fall through */
      case TBACKGND:
      case TSEMI:
        break;
      default:
        if (!chknl) synexpect(-1);
        tokpushback++;
        return n1;
    }
  }
}

static union node *andor(void) {
  union node *n1, *n2, *n3;
  int t;
  n1 = pipeline();
  for (;;) {
    if ((t = readtoken()) == TAND) {
      t = NAND;
    } else if (t == TOR) {
      t = NOR;
    } else {
      tokpushback++;
      return n1;
    }
    checkkwd = CHKNL | CHKKWD | CHKALIAS;
    n2 = pipeline();
    n3 = (union node *)stalloc(sizeof(struct nbinary));
    n3->type = t;
    n3->nbinary.ch1 = n1;
    n3->nbinary.ch2 = n2;
    n1 = n3;
  }
}

static union node *pipeline(void) {
  union node *n1, *n2, *pipenode;
  struct nodelist *lp, *prev;
  int negate;
  negate = 0;
  TRACE(("pipeline: entered\n"));
  if (readtoken() == TNOT) {
    negate = !negate;
    checkkwd = CHKKWD | CHKALIAS;
  } else
    tokpushback++;
  n1 = command();
  if (readtoken() == TPIPE) {
    pipenode = (union node *)stalloc(sizeof(struct npipe));
    pipenode->type = NPIPE;
    pipenode->npipe.backgnd = 0;
    lp = (struct nodelist *)stalloc(sizeof(struct nodelist));
    pipenode->npipe.cmdlist = lp;
    lp->n = n1;
    do {
      prev = lp;
      lp = (struct nodelist *)stalloc(sizeof(struct nodelist));
      checkkwd = CHKNL | CHKKWD | CHKALIAS;
      lp->n = command();
      prev->next = lp;
    } while (readtoken() == TPIPE);
    lp->next = NULL;
    n1 = pipenode;
  }
  tokpushback++;
  if (negate) {
    n2 = (union node *)stalloc(sizeof(struct nnot));
    n2->type = NNOT;
    n2->nnot.com = n1;
    return n2;
  } else
    return n1;
}

static union node *command(void) {
  union node *n1, *n2;
  union node *ap, **app;
  union node *cp, **cpp;
  union node *redir, **rpp;
  union node **rpp2;
  int t;
  int savelinno;
  redir = NULL;
  rpp2 = &redir;
  savelinno = plinno;
  switch (readtoken()) {
    default:
      synexpect(-1);
      __builtin_unreachable();
    case TIF:
      n1 = (union node *)stalloc(sizeof(struct nif));
      n1->type = NIF;
      n1->nif.test = list(0);
      if (readtoken() != TTHEN) synexpect(TTHEN);
      n1->nif.ifpart = list(0);
      n2 = n1;
      while (readtoken() == TELIF) {
        n2->nif.elsepart = (union node *)stalloc(sizeof(struct nif));
        n2 = n2->nif.elsepart;
        n2->type = NIF;
        n2->nif.test = list(0);
        if (readtoken() != TTHEN) synexpect(TTHEN);
        n2->nif.ifpart = list(0);
      }
      if (lasttoken == TELSE)
        n2->nif.elsepart = list(0);
      else {
        n2->nif.elsepart = NULL;
        tokpushback++;
      }
      t = TFI;
      break;
    case TWHILE:
    case TUNTIL: {
      int got;
      n1 = (union node *)stalloc(sizeof(struct nbinary));
      n1->type = (lasttoken == TWHILE) ? NWHILE : NUNTIL;
      n1->nbinary.ch1 = list(0);
      if ((got = readtoken()) != TDO) {
        TRACE(("expecting DO got %s %s\n", tokname[got],
               got == TWORD ? wordtext : ""));
        synexpect(TDO);
      }
      n1->nbinary.ch2 = list(0);
      t = TDONE;
      break;
    }
    case TFOR:
      if (readtoken() != TWORD || quoteflag || !goodname(wordtext))
        synerror("Bad for loop variable");
      n1 = (union node *)stalloc(sizeof(struct nfor));
      n1->type = NFOR;
      n1->nfor.linno = savelinno;
      n1->nfor.var_ = wordtext;
      checkkwd = CHKNL | CHKKWD | CHKALIAS;
      if (readtoken() == TIN) {
        app = &ap;
        while (readtoken() == TWORD) {
          n2 = (union node *)stalloc(sizeof(struct narg));
          n2->type = NARG;
          n2->narg.text = wordtext;
          n2->narg.backquote = backquotelist;
          *app = n2;
          app = &n2->narg.next;
        }
        *app = NULL;
        n1->nfor.args = ap;
        if (lasttoken != TNL && lasttoken != TSEMI) synexpect(-1);
      } else {
        n2 = (union node *)stalloc(sizeof(struct narg));
        n2->type = NARG;
        n2->narg.text = (char *)dolatstr;
        n2->narg.backquote = NULL;
        n2->narg.next = NULL;
        n1->nfor.args = n2;
        /*
         * Newline or semicolon here is optional (but note
         * that the original Bourne shell only allowed NL).
         */
        if (lasttoken != TSEMI) tokpushback++;
      }
      checkkwd = CHKNL | CHKKWD | CHKALIAS;
      if (readtoken() != TDO) synexpect(TDO);
      n1->nfor.body = list(0);
      t = TDONE;
      break;
    case TCASE:
      n1 = (union node *)stalloc(sizeof(struct ncase));
      n1->type = NCASE;
      n1->ncase.linno = savelinno;
      if (readtoken() != TWORD) synexpect(TWORD);
      n1->ncase.expr = n2 = (union node *)stalloc(sizeof(struct narg));
      n2->type = NARG;
      n2->narg.text = wordtext;
      n2->narg.backquote = backquotelist;
      n2->narg.next = NULL;
      checkkwd = CHKNL | CHKKWD | CHKALIAS;
      if (readtoken() != TIN) synexpect(TIN);
      cpp = &n1->ncase.cases;
    next_case:
      checkkwd = CHKNL | CHKKWD;
      t = readtoken();
      while (t != TESAC) {
        if (lasttoken == TLP) readtoken();
        *cpp = cp = (union node *)stalloc(sizeof(struct nclist));
        cp->type = NCLIST;
        app = &cp->nclist.pattern;
        for (;;) {
          *app = ap = (union node *)stalloc(sizeof(struct narg));
          ap->type = NARG;
          ap->narg.text = wordtext;
          ap->narg.backquote = backquotelist;
          if (readtoken() != TPIPE) break;
          app = &ap->narg.next;
          readtoken();
        }
        ap->narg.next = NULL;
        if (lasttoken != TRP) synexpect(TRP);
        cp->nclist.body = list(2);
        cpp = &cp->nclist.next;
        checkkwd = CHKNL | CHKKWD;
        if ((t = readtoken()) != TESAC) {
          if (t != TENDCASE)
            synexpect(TENDCASE);
          else
            goto next_case;
        }
      }
      *cpp = NULL;
      goto redir;
    case TLP:
      n1 = (union node *)stalloc(sizeof(struct nredir));
      n1->type = NSUBSHELL;
      n1->nredir.linno = savelinno;
      n1->nredir.n = list(0);
      n1->nredir.redirect = NULL;
      t = TRP;
      break;
    case TBEGIN:
      n1 = list(0);
      t = TEND;
      break;
    case TWORD:
    case TREDIR:
      tokpushback++;
      return simplecmd();
  }
  if (readtoken() != t) synexpect(t);
redir:
  /* Now check for redirection which may follow command */
  checkkwd = CHKKWD | CHKALIAS;
  rpp = rpp2;
  while (readtoken() == TREDIR) {
    *rpp = n2 = redirnode;
    rpp = &n2->nfile.next;
    parsefname();
  }
  tokpushback++;
  *rpp = NULL;
  if (redir) {
    if (n1->type != NSUBSHELL) {
      n2 = (union node *)stalloc(sizeof(struct nredir));
      n2->type = NREDIR;
      n2->nredir.linno = savelinno;
      n2->nredir.n = n1;
      n1 = n2;
    }
    n1->nredir.redirect = redir;
  }
  return n1;
}

static union node *simplecmd(void) {
  union node *args, **app;
  union node *n = NULL;
  union node *vars, **vpp;
  union node **rpp, *redir;
  int savecheckkwd;
  int savelinno;
  args = NULL;
  app = &args;
  vars = NULL;
  vpp = &vars;
  redir = NULL;
  rpp = &redir;
  savecheckkwd = CHKALIAS;
  savelinno = plinno;
  for (;;) {
    checkkwd = savecheckkwd;
    switch (readtoken()) {
      case TWORD:
        n = (union node *)stalloc(sizeof(struct narg));
        n->type = NARG;
        n->narg.text = wordtext;
        n->narg.backquote = backquotelist;
        if (savecheckkwd && isassignment(wordtext)) {
          *vpp = n;
          vpp = &n->narg.next;
        } else {
          *app = n;
          app = &n->narg.next;
          savecheckkwd = 0;
        }
        break;
      case TREDIR:
        *rpp = n = redirnode;
        rpp = &n->nfile.next;
        parsefname(); /* read name of redirection file */
        break;
      case TLP:
        if (args && app == &args->narg.next && !vars && !redir) {
          struct builtincmd *bcmd;
          const char *name;
          /* We have a function */
          if (readtoken() != TRP) synexpect(TRP);
          name = n->narg.text;
          if (!goodname(name) ||
              ((bcmd = find_builtin(name)) && bcmd->flags & BUILTIN_SPECIAL))
            synerror("Bad function name");
          n->type = NDEFUN;
          checkkwd = CHKNL | CHKKWD | CHKALIAS;
          n->ndefun.text = n->narg.text;
          n->ndefun.linno = plinno;
          n->ndefun.body = command();
          return n;
        }
        /* fall through */
      default:
        tokpushback++;
        goto out;
    }
  }
out:
  *app = NULL;
  *vpp = NULL;
  *rpp = NULL;
  n = (union node *)stalloc(sizeof(struct ncmd));
  n->type = NCMD;
  n->ncmd.linno = savelinno;
  n->ncmd.args = args;
  n->ncmd.assign = vars;
  n->ncmd.redirect = redir;
  return n;
}

static union node *makename(void) {
  union node *n;
  n = (union node *)stalloc(sizeof(struct narg));
  n->type = NARG;
  n->narg.next = NULL;
  n->narg.text = wordtext;
  n->narg.backquote = backquotelist;
  return n;
}

static void fixredir(union node *n, const char *text, int err) {
  TRACE(("Fix redir %s %d\n", text, err));
  if (!err) n->ndup.vname = NULL;
  if (is_digit(text[0]) && text[1] == '\0')
    n->ndup.dupfd = digit_val(text[0]);
  else if (text[0] == '-' && text[1] == '\0')
    n->ndup.dupfd = -1;
  else {
    if (err)
      synerror("Bad fd number");
    else
      n->ndup.vname = makename();
  }
}

static void parsefname(void) {
  union node *n = redirnode;
  if (n->type == NHERE) checkkwd = CHKEOFMARK;
  if (readtoken() != TWORD) synexpect(-1);
  if (n->type == NHERE) {
    struct heredoc *here = heredoc;
    struct heredoc *p;
    if (quoteflag == 0) n->type = NXHERE;
    TRACE(("Here document %d\n", n->type));
    rmescapes(wordtext, 0);
    here->eofmark = wordtext;
    here->next = NULL;
    if (heredoclist == NULL)
      heredoclist = here;
    else {
      for (p = heredoclist; p->next; p = p->next);
      p->next = here;
    }
  } else if (n->type == NTOFD || n->type == NFROMFD) {
    fixredir(n, wordtext, 0);
  } else {
    n->nfile.fname = makename();
  }
}

/*
 * Input any here documents.
 */
static void parseheredoc(void) {
  struct heredoc *here;
  union node *n;
  here = heredoclist;
  heredoclist = 0;
  while (here) {
    if (needprompt) {
      setprompt(2);
    }
    if (here->here->type == NHERE)
      readtoken1(pgetc(), SQSYNTAX, here->eofmark, here->striptabs);
    else
      readtoken1(pgetc_eatbnl(), DQSYNTAX, here->eofmark, here->striptabs);
    n = (union node *)stalloc(sizeof(struct narg));
    n->narg.type = NARG;
    n->narg.next = NULL;
    n->narg.text = wordtext;
    n->narg.backquote = backquotelist;
    here->here->nhere.doc = n;
    here = here->next;
  }
}

static int readtoken(void) {
  int t;
  int kwd = checkkwd;
top:
  t = xxreadtoken();
  /*
   * eat newlines
   */
  if (kwd & CHKNL) {
    while (t == TNL) {
      parseheredoc();
      checkkwd = 0;
      t = xxreadtoken();
    }
  }
  kwd |= checkkwd;
  checkkwd = 0;
  if (t != TWORD || quoteflag) {
    goto out;
  }
  /*
   * check for keywords
   */
  if (kwd & CHKKWD) {
    const char *const *pp;
    const int KWDOFFSET = 13;
    if ((pp = findkwd(wordtext))) {
      lasttoken = t = pp - parsekwd + KWDOFFSET;
      TRACE(("keyword %s recognized\n", tokname[t]));
      goto out;
    }
  }
  if (kwd & CHKALIAS) {
    struct alias *ap;
    if ((ap = lookupalias(wordtext, 1)) != NULL) {
      if (*ap->val) {
        pushstring(ap->val, ap);
      }
      goto top;
    }
  }
out:
  return (t);
}

static void nlprompt(void) {
  plinno++;
  if (doprompt) setprompt(2);
}

static void nlnoprompt(void) {
  plinno++;
  needprompt = doprompt;
}

/*
 * Read the next input token.
 * If the token is a word, we set backquotelist to the list of cmds in
 *  backquotes.  We set quoteflag to true if any part of the word was
 *  quoted.
 * If the token is TREDIR, then we set redirnode to a structure containing
 *  the redirection.
 *
 * [Change comment:  here documents and internal procedures]
 * [Readtoken shouldn't have any arguments.  Perhaps we should make the
 *  word parsing code into a separate routine.  In this case, readtoken
 *  doesn't need to have any internal procedures, but parseword does.
 *  We could also make parseoperator in essence the main routine, and
 *  have parseword (readtoken1?) handle both words and redirection.]
 */
static int xxreadtoken(void) {
#define RETURN(token) return lasttoken = token
  int c;
  if (tokpushback) {
    tokpushback = 0;
    return lasttoken;
  }
  if (needprompt) {
    setprompt(2);
  }
  for (;;) { /* until token or start of word found */
    c = pgetc_eatbnl();
    switch (c) {
      case ' ':
      case '\t':
        continue;
      case '#':
        while ((c = pgetc()) != '\n' && c != PEOF);
        pungetc();
        continue;
      case '\n':
        nlnoprompt();
        RETURN(TNL);
      case PEOF:
        RETURN(TEOF);
      case '&':
        if (pgetc_eatbnl() == '&') RETURN(TAND);
        pungetc();
        RETURN(TBACKGND);
      case '|':
        if (pgetc_eatbnl() == '|') RETURN(TOR);
        pungetc();
        RETURN(TPIPE);
      case ';':
        if (pgetc_eatbnl() == ';') RETURN(TENDCASE);
        pungetc();
        RETURN(TSEMI);
      case '(':
        RETURN(TLP);
      case ')':
        RETURN(TRP);
    }
    break;
  }
  return readtoken1(c, BASESYNTAX, (char *)NULL, 0);
#undef RETURN
}

static int pgetc_eatbnl(void) {
  int c;
  while ((c = pgetc()) == '\\') {
    if (pgetc() != '\n') {
      pungetc();
      break;
    }
    nlprompt();
  }
  return c;
}

static int pgetc_top(struct synstack *stack) {
  return stack->syntax == SQSYNTAX ? pgetc() : pgetc_eatbnl();
}

static void synstack_push(struct synstack **stack, struct synstack *next,
                          const char *syntax) {
  memset(next, 0, sizeof(*next));
  next->syntax = syntax;
  next->next = *stack;
  (*stack)->prev = next;
  *stack = next;
}

static void synstack_pop(struct synstack **stack) {
  *stack = (*stack)->next;
}

/*
 * If eofmark is NULL, read a word or a redirection symbol.  If eofmark
 * is not NULL, read a here document.  In the latter case, eofmark is the
 * word which marks the end of the document and striptabs is true if
 * leading tabs should be stripped from the document.  The argument firstc
 * is the first character of the input token or document.
 *
 * Because C does not have internal subroutines, I have simulated them
 * using goto's to implement the subroutine linkage.  The following macros
 * will run code that appears at the end of readtoken1.
 */

#define CHECKEND()  \
  {                 \
    goto checkend;  \
  checkend_return:; \
  }
#define PARSEREDIR()  \
  {                   \
    goto parseredir;  \
  parseredir_return:; \
  }
#define PARSESUB()  \
  {                 \
    goto parsesub;  \
  parsesub_return:; \
  }
#define PARSEBACKQOLD()  \
  {                      \
    oldstyle = 1;        \
    goto parsebackq;     \
  parsebackq_oldreturn:; \
  }
#define PARSEBACKQNEW()  \
  {                      \
    oldstyle = 0;        \
    goto parsebackq;     \
  parsebackq_newreturn:; \
  }
#define PARSEARITH()  \
  {                   \
    goto parsearith;  \
  parsearith_return:; \
  }

static int readtoken1(int firstc, char const *syntax, char *eofmark,
                      int striptabs) {
  int c = firstc;
  char *out;
  unsigned len;
  struct nodelist *bqlist;
  int quotef;
  int oldstyle;
  /* syntax stack */
  struct synstack synbase = {.syntax = syntax};
  struct synstack *synstack = &synbase;
  if (syntax == DQSYNTAX) synstack->dblquote = 1;
  quotef = 0;
  bqlist = NULL;
  STARTSTACKSTR(out);
loop: {                    /* for each line, until end of word */
  CHECKEND();              /* set c to PEOF if at end of here document */
  for (;;) {               /* until end of line or end of word */
    CHECKSTRSPACE(4, out); /* permit 4 calls to USTPUTC */
    switch (synstack->syntax[c]) {
      case CNL: /* '\n' */
        if (synstack->syntax == BASESYNTAX && !synstack->varnest)
          goto endword; /* exit outer loop */
        USTPUTC(c, out);
        nlprompt();
        c = pgetc_top(synstack);
        goto loop; /* continue outer loop */
      case CWORD:
        USTPUTC(c, out);
        break;
      case CCTL:
        if ((!eofmark) | synstack->dblquote | synstack->varnest)
          USTPUTC(CTLESC, out);
        USTPUTC(c, out);
        break;
      /* backslash */
      case CBACK:
        c = pgetc();
        if (c == PEOF) {
          USTPUTC(CTLESC, out);
          USTPUTC('\\', out);
          pungetc();
        } else {
          if (synstack->dblquote && c != '\\' && c != '`' && c != '$' &&
              (c != '"' || (eofmark != NULL && !synstack->varnest)) &&
              (c != '}' || !synstack->varnest)) {
            USTPUTC(CTLESC, out);
            USTPUTC('\\', out);
          }
          USTPUTC(CTLESC, out);
          USTPUTC(c, out);
          quotef++;
        }
        break;
      case CSQUOTE:
        synstack->syntax = SQSYNTAX;
      quotemark:
        if (eofmark == NULL) {
          USTPUTC(CTLQUOTEMARK, out);
        }
        break;
      case CDQUOTE:
        synstack->syntax = DQSYNTAX;
        synstack->dblquote = 1;
      toggledq:
        if (synstack->varnest) synstack->innerdq ^= 1;
        goto quotemark;
      case CENDQUOTE:
        if (eofmark && !synstack->varnest) {
          USTPUTC(c, out);
          break;
        }
        if (synstack->dqvarnest == 0) {
          synstack->syntax = BASESYNTAX;
          synstack->dblquote = 0;
        }
        quotef++;
        if (c == '"') goto toggledq;
        goto quotemark;
      case CVAR:    /* '$' */
        PARSESUB(); /* parse substitution */
        break;
      case CENDVAR: /* '}' */
        if (!synstack->innerdq && synstack->varnest > 0) {
          if (!--synstack->varnest && synstack->varpushed)
            synstack_pop(&synstack);
          else if (synstack->dqvarnest > 0)
            synstack->dqvarnest--;
          USTPUTC(CTLENDVAR, out);
        } else {
          USTPUTC(c, out);
        }
        break;
      case CLP: /* '(' in arithmetic */
        synstack->parenlevel++;
        USTPUTC(c, out);
        break;
      case CRP: /* ')' in arithmetic */
        if (synstack->parenlevel > 0) {
          USTPUTC(c, out);
          --synstack->parenlevel;
        } else {
          if (pgetc_eatbnl() == ')') {
            USTPUTC(CTLENDARI, out);
            synstack_pop(&synstack);
          } else {
            /*
             * unbalanced parens
             *  (don't 2nd guess - no error)
             */
            pungetc();
            USTPUTC(')', out);
          }
        }
        break;
      case CBQUOTE: /* '`' */
        if (checkkwd & CHKEOFMARK) {
          USTPUTC('`', out);
          break;
        }
        PARSEBACKQOLD();
        break;
      case CEOF:
        goto endword; /* exit outer loop */
      default:
        if (synstack->varnest == 0) goto endword; /* exit outer loop */
        USTPUTC(c, out);
    }
    c = pgetc_top(synstack);
  }
}
endword:
  if (synstack->syntax == ARISYNTAX) synerror("Missing '))'");
  if (synstack->syntax != BASESYNTAX && eofmark == NULL)
    synerror("Unterminated quoted string");
  if (synstack->varnest != 0) {
    /* { */
    synerror("Missing '}'");
  }
  USTPUTC('\0', out);
  len = out - (char *)stackblock();
  out = stackblock();
  if (eofmark == NULL) {
    if ((c == '>' || c == '<') && quotef == 0 && len <= 2 &&
        (*out == '\0' || is_digit(*out))) {
      PARSEREDIR();
      return lasttoken = TREDIR;
    } else {
      pungetc();
    }
  }
  quoteflag = quotef;
  backquotelist = bqlist;
  grabstackblock(len);
  wordtext = out;
  return lasttoken = TWORD;
  /* end of readtoken routine */

  /*
   * Check to see whether we are at the end of the here document.  When this
   * is called, c is set to the first character of the next input line.  If
   * we are at the end of the here document, this routine sets the c to PEOF.
   */
checkend: {
  if (realeofmark(eofmark)) {
    int markloc;
    char *p;
    if (striptabs) {
      while (c == '\t') c = pgetc();
    }
    markloc = out - (char *)stackblock();
    for (p = eofmark; STPUTC(c, out), *p; p++) {
      if (c != *p) goto more_heredoc;
      c = pgetc();
    }
    if (c == '\n' || c == PEOF) {
      c = PEOF;
      nlnoprompt();
    } else {
      int len2;
    more_heredoc:
      p = (char *)stackblock() + markloc + 1;
      len2 = out - p;
      if (len2) {
        len2 -= c < 0;
        c = p[-1];
        if (len2) {
          char *str;
          str = alloca(len2 + 1);
          *(char *)mempcpy(str, p, len2) = 0;
          pushstring(str, NULL);
        }
      }
    }
    STADJUST((char *)stackblock() + markloc - out, out);
  }
  goto checkend_return;
}

  /*
   * Parse a redirection operator.  The variable "out" points to a string
   * specifying the fd to be redirected.  The variable "c" contains the
   * first character of the redirection operator.
   */
parseredir: {
  char fd = *out;
  union node *np;
  np = (union node *)stalloc(sizeof(struct nfile));
  if (c == '>') {
    np->nfile.fd = 1;
    c = pgetc_eatbnl();
    if (c == '>')
      np->type = NAPPEND;
    else if (c == '|')
      np->type = NCLOBBER;
    else if (c == '&')
      np->type = NTOFD;
    else {
      np->type = NTO;
      pungetc();
    }
  } else { /* c == '<' */
    np->nfile.fd = 0;
    switch (c = pgetc_eatbnl()) {
      case '<':
        if (sizeof(struct nfile) != sizeof(struct nhere)) {
          np = (union node *)stalloc(sizeof(struct nhere));
          np->nfile.fd = 0;
        }
        np->type = NHERE;
        heredoc = (struct heredoc *)stalloc(sizeof(struct heredoc));
        heredoc->here = np;
        if ((c = pgetc_eatbnl()) == '-') {
          heredoc->striptabs = 1;
        } else {
          heredoc->striptabs = 0;
          pungetc();
        }
        break;
      case '&':
        np->type = NFROMFD;
        break;
      case '>':
        np->type = NFROMTO;
        break;
      default:
        np->type = NFROM;
        pungetc();
        break;
    }
  }
  if (fd != '\0') np->nfile.fd = digit_val(fd);
  redirnode = np;
  goto parseredir_return;
}

  /*
   * Parse a substitution.  At this point, we have read the dollar sign
   * and nothing else.
   */
parsesub: {
  int subtype;
  int typeloc;
  char *p;
  static const char types[] = "}-+?=";
  c = pgetc_eatbnl();
  if ((checkkwd & CHKEOFMARK) ||
      (c != '(' && c != '{' && !is_name(c) && !is_special(c))) {
    USTPUTC('$', out);
    pungetc();
  } else if (c == '(') { /* $(command) or $((arith)) */
    if (pgetc_eatbnl() == '(') {
      PARSEARITH();
    } else {
      pungetc();
      PARSEBACKQNEW();
    }
  } else {
    const char *newsyn = synstack->syntax;
    USTPUTC(CTLVAR, out);
    typeloc = out - (char *)stackblock();
    STADJUST(1, out);
    subtype = VSNORMAL;
    if (likely(c == '{')) {
      c = pgetc_eatbnl();
      subtype = 0;
    }
  varname:
    if (is_name(c)) {
      do {
        STPUTC(c, out);
        c = pgetc_eatbnl();
      } while (is_in_name(c));
    } else if (is_digit(c)) {
      do {
        STPUTC(c, out);
        c = pgetc_eatbnl();
      } while ((subtype <= 0 || subtype >= VSLENGTH) && is_digit(c));
    } else if (c != '}') {
      int cc = c;
      c = pgetc_eatbnl();
      if (!subtype && cc == '#') {
        subtype = VSLENGTH;
        if (c == '_' || isalnum(c)) goto varname;
        cc = c;
        c = pgetc_eatbnl();
        if (cc == '}' || c != '}') {
          pungetc();
          subtype = 0;
          c = cc;
          cc = '#';
        }
      }
      if (!is_special(cc)) {
        if (subtype == VSLENGTH) subtype = 0;
        goto badsub;
      }
      USTPUTC(cc, out);
    } else
      goto badsub;
    if (subtype == 0) {
      int cc = c;
      switch (c) {
        case ':':
          subtype = VSNUL;
          c = pgetc_eatbnl();
          /*FALLTHROUGH*/
        default:
          p = strchr(types, c);
          if (p == NULL) break;
          subtype |= p - types + VSNORMAL;
          break;
        case '%':
        case '#':
          subtype = c == '#' ? VSTRIMLEFT : VSTRIMRIGHT;
          c = pgetc_eatbnl();
          if (c == cc)
            subtype++;
          else
            pungetc();
          newsyn = BASESYNTAX;
          break;
      }
    } else {
      if (subtype == VSLENGTH && c != '}') subtype = 0;
    badsub:
      pungetc();
    }
    if (newsyn == ARISYNTAX) newsyn = DQSYNTAX;
    if ((newsyn != synstack->syntax || synstack->innerdq) &&
        subtype != VSNORMAL) {
      synstack_push(&synstack, synstack->prev ?: alloca(sizeof(*synstack)),
                    newsyn);
      synstack->varpushed++;
      synstack->dblquote = newsyn != BASESYNTAX;
    }
    *((char *)stackblock() + typeloc) = subtype;
    if (subtype != VSNORMAL) {
      synstack->varnest++;
      if (synstack->dblquote) synstack->dqvarnest++;
    }
    STPUTC('=', out);
  }
  goto parsesub_return;
}

  /*
   * Called to parse command substitutions.  Newstyle is set if the command
   * is enclosed inside $(...); nlpp is a pointer to the head of the linked
   * list of commands (passed by reference), and savelen is the number of
   * characters on the top of the stack which must be preserved.
   */
parsebackq: {
  struct nodelist **nlpp;
  union node *n;
  char *str;
  unsigned savelen;
  struct heredoc *saveheredoclist;
  int uninitialized_var(saveprompt);
  str = NULL;
  savelen = out - (char *)stackblock();
  if (savelen > 0) {
    str = alloca(savelen);
    memcpy(str, stackblock(), savelen);
  }
  if (oldstyle) {
    /* We must read until the closing backquote, giving special
       treatment to some slashes, and then push the string and
       reread it as input, interpreting it normally.  */
    char *pout;
    int pc;
    unsigned psavelen;
    char *pstr;
    STARTSTACKSTR(pout);
    for (;;) {
      if (needprompt) {
        setprompt(2);
      }
      switch (pc = pgetc_eatbnl()) {
        case '`':
          goto done;
        case '\\':
          pc = pgetc();
          if (pc != '\\' && pc != '`' && pc != '$' &&
              (!synstack->dblquote || pc != '"'))
            STPUTC('\\', pout);
          break;
        case PEOF:
          synerror("EOF in backquote substitution");
        case '\n':
          nlnoprompt();
          break;
        default:
          break;
      }
      STPUTC(pc, pout);
    }
  done:
    STPUTC('\0', pout);
    psavelen = pout - (char *)stackblock();
    if (psavelen > 0) {
      pstr = grabstackstr(pout);
      setinputstring(pstr);
    }
  }
  nlpp = &bqlist;
  while (*nlpp) nlpp = &(*nlpp)->next;
  *nlpp = (struct nodelist *)stalloc(sizeof(struct nodelist));
  (*nlpp)->next = NULL;
  saveheredoclist = heredoclist;
  heredoclist = NULL;
  if (oldstyle) {
    saveprompt = doprompt;
    doprompt = 0;
  }
  n = list(2);
  if (oldstyle)
    doprompt = saveprompt;
  else {
    if (readtoken() != TRP) synexpect(TRP);
    setinputstring(nullstr);
  }
  parseheredoc();
  heredoclist = saveheredoclist;
  (*nlpp)->n = n;
  /* Start reading from old file again. */
  popfile();
  /* Ignore any pushed back tokens left from the backquote parsing. */
  if (oldstyle) tokpushback = 0;
  out = growstackto(savelen + 1);
  if (str) {
    memcpy(out, str, savelen);
    STADJUST(savelen, out);
  }
  USTPUTC(CTLBACKQ, out);
  if (oldstyle)
    goto parsebackq_oldreturn;
  else
    goto parsebackq_newreturn;
}

/*
 * Parse an arithmetic expansion (indicate start of one and set state)
 */
parsearith: {
  synstack_push(&synstack, synstack->prev ?: alloca(sizeof(*synstack)),
                ARISYNTAX);
  synstack->dblquote = 1;
  USTPUTC(CTLARI, out);
  goto parsearith_return;
}

} /* end of readtoken */

static void setprompt(int which) {
  struct stackmark smark;
  int show;
  needprompt = 0;
  whichprompt = which;
  show = 0;
  if (show) {
    pushstackmark(&smark, stackblocksize());
    outstr(getprompt(NULL), out2);
    popstackmark(&smark);
  }
}

static const char *expandstr(const char *ps) {
  struct parsefile *file_stop;
  struct jmploc *volatile savehandler;
  struct heredoc *saveheredoclist;
  const char *result;
  int saveprompt;
  struct jmploc jmploc;
  union node n;
  int err;
  file_stop = parsefile;
  setinputstring((char *)ps); /* XXX Fix (char *) cast. */
  saveheredoclist = heredoclist;
  heredoclist = NULL;
  saveprompt = doprompt;
  doprompt = 0;
  result = ps;
  savehandler = handler;
  if (unlikely(err = setjmp(jmploc.loc))) goto out;
  handler = &jmploc;
  readtoken1(pgetc_eatbnl(), DQSYNTAX, FAKEEOFMARK, 0);
  n.narg.type = NARG;
  n.narg.next = NULL;
  n.narg.text = wordtext;
  n.narg.backquote = backquotelist;
  expandarg(&n, NULL, EXP_QUOTED);
  result = stackblock();
out:
  handler = savehandler;
  if (err && exception != EXERROR) longjmp(handler->loc, 1);
  doprompt = saveprompt;
  unwindfiles(file_stop);
  heredoclist = saveheredoclist;
  return result;
}

/*
 * called by editline -- any expansions to the prompt
 *    should be added here.
 */
static const char *getprompt(void *unused) {
  const char *prompt;
  switch (whichprompt) {
    default:
    case 0:
      return nullstr;
    case 1:
      prompt = ps1val();
      break;
    case 2:
      prompt = ps2val();
      break;
  }
  return expandstr(prompt);
}

const char *const *findkwd(const char *s) {
  return findstring(s, parsekwd, sizeof(parsekwd) / sizeof(const char *));
}

static unsigned update_closed_redirs(int fd, int nfd) {
  unsigned val = closed_redirs;
  unsigned bit = 1 << fd;
  if (nfd >= 0)
    closed_redirs &= ~bit;
  else
    closed_redirs |= bit;
  return val & bit;
}

/*
 * Process a list of redirection commands.  If the REDIR_PUSH flag is set,
 * old file descriptors are stashed away so that the redirection can be
 * undone by calling popredir.  If the REDIR_BACKQ flag is set, then the
 * standard output, and the standard error if it becomes a duplicate of
 * stdout, is saved in memory.
 */
static void redirect(union node *redir, int flags) {
  union node *n;
  struct redirtab *sv;
  int i;
  int fd;
  int newfd;
  int *p;
  if (!redir) return;
  sv = NULL;
  INTOFF;
  if (likely(flags & REDIR_PUSH)) sv = redirlist;
  n = redir;
  do {
    newfd = openredirect(n);
    if (newfd < -1) continue;
    fd = n->nfile.fd;
    if (sv) {
      int closed;
      p = &sv->renamed[fd];
      i = *p;
      closed = update_closed_redirs(fd, newfd);
      if (likely(i == EMPTY)) {
        i = CLOSED;
        if (fd != newfd && !closed) {
          i = savefd(fd, fd);
          fd = -1;
        }
      }
      *p = i;
    }
    if (fd == newfd) continue;
    dupredirect(n, newfd);
  } while ((n = n->nfile.next));
  INTON;
  if (flags & REDIR_SAVEFD2 && sv->renamed[2] >= 0)
    preverrout.fd = sv->renamed[2];
}

wontreturn static int sh_open_fail(const char *pathname, int flags, int e) {
  const char *word;
  int action;
  word = "open";
  action = E_OPEN;
  if (flags & O_CREAT) {
    word = "create";
    action = E_CREAT;
  }
  sh_error("cannot %s %s: %s", word, pathname, errmsg(e, action));
}

static int sh_open(const char *pathname, int flags, int mayfail) {
  int fd;
  int e;
  do {
    fd = open(pathname, flags, 0666);
    e = errno;
  } while (fd < 0 && e == EINTR && !pending_sig);
  if (mayfail || fd >= 0) return fd;
  sh_open_fail(pathname, flags, e);
}

static int openredirect(union node *redir) {
  struct stat sb;
  char *fname;
  int flags;
  int f;
  switch (redir->nfile.type) {
    case NFROM:
      flags = O_RDONLY;
    do_open:
      f = sh_open(redir->nfile.expfname, flags, 0);
      break;
    case NFROMTO:
      flags = O_RDWR | O_CREAT;
      goto do_open;
    case NTO:
      /* Take care of noclobber mode. */
      if (Cflag) {
        fname = redir->nfile.expfname;
        if (stat(fname, &sb) < 0) {
          flags = O_WRONLY | O_CREAT | O_EXCL;
          goto do_open;
        }
        if (S_ISREG(sb.st_mode)) goto ecreate;
        f = sh_open(fname, O_WRONLY, 0);
        if (!fstat(f, &sb) && S_ISREG(sb.st_mode)) {
          close(f);
          goto ecreate;
        }
        break;
      }
      /* FALLTHROUGH */
    case NCLOBBER:
      flags = O_WRONLY | O_CREAT | O_TRUNC;
      goto do_open;
    case NAPPEND:
      flags = O_WRONLY | O_CREAT | O_APPEND;
      goto do_open;
    case NTOFD:
    case NFROMFD:
      f = redir->ndup.dupfd;
      if (f == redir->nfile.fd) f = -2;
      break;
    default:
      /* Fall through to eliminate warning. */
    case NHERE:
    case NXHERE:
      f = openhere(redir);
      break;
  }
  return f;
ecreate:
  sh_open_fail(fname, O_CREAT, EEXIST);
}

static void dupredirect(union node *redir, int f) {
  int fd = redir->nfile.fd;
  int err = 0;
  if (redir->nfile.type == NTOFD || redir->nfile.type == NFROMFD) {
    /* if not ">&-" */
    if (f >= 0) {
      if (dup2(f, fd) < 0) {
        err = errno;
        goto err;
      }
      return;
    }
    f = fd;
  } else if (dup2(f, fd) < 0) {
    err = errno;
  }
  close(f);
  if (err < 0) goto err;
  return;
err:
  sh_error("%ld: %s", f, strerror(err));
}

/*
 * Handle here documents.  Normally we fork off a process to write the
 * data to a pipe.  If the document is short, we can stuff the data in
 * the pipe without forking.
 */
static int64_t openhere(union node *redir) {
  char *p;
  int pip[2];
  unsigned len = 0;
  if (pipe(pip) < 0) sh_error("Pipe call failed");
  p = redir->nhere.doc->narg.text;
  if (redir->type == NXHERE) {
    expandarg(redir->nhere.doc, NULL, EXP_QUOTED);
    p = stackblock();
  }
  len = strlen(p);
  if (len <= PIPESIZE) {
    __xwrite(pip[1], p, len);
    goto out;
  }
  if (forkshell((struct job *)NULL, (union node *)NULL, FORK_NOJOB) == 0) {
    close(pip[0]);
    signal(SIGINT, SIG_IGN);
    signal(SIGQUIT, SIG_IGN);
    signal(SIGHUP, SIG_IGN);
    signal(SIGPIPE, SIG_DFL);
    __xwrite(pip[1], p, len);
    _exit(0);
  }
out:
  close(pip[1]);
  return pip[0];
}

/*
 * Undo the effects of the last redirection.
 */
static void popredir(int drop) {
  struct redirtab *rp;
  int i;
  INTOFF;
  rp = redirlist;
  for (i = 0; i < 10; i++) {
    int closed;
    if (rp->renamed[i] == EMPTY) continue;
    closed = drop ? 1 : update_closed_redirs(i, rp->renamed[i]);
    switch (rp->renamed[i]) {
      case CLOSED:
        if (!closed) close(i);
        break;
      default:
        if (!drop) dup2(rp->renamed[i], i);
        close(rp->renamed[i]);
        break;
    }
  }
  redirlist = rp->next;
  ckfree(rp);
  INTON;
}

/*
 * Move a file descriptor to > 10.  Invokes sh_error on error unless
 * the original file dscriptor is not open.
 */
static int savefd(int from, int ofd) {
  int newfd;
  int err;
  newfd = fcntl(from, F_DUPFD, 10);
  err = newfd < 0 ? errno : 0;
  if (err != EBADF) {
    close(ofd);
    if (err) {
      sh_error("%d: %s", from, strerror(err));
    } else {
      fcntl(newfd, F_SETFD, FD_CLOEXEC);
    }
  }
  return newfd;
}

static int redirectsafe(union node *redir, int flags) {
  int err;
  volatile int saveint;
  struct jmploc *volatile savehandler = handler;
  struct jmploc jmploc;
  SAVEINT(saveint);
  if (!(err = setjmp(jmploc.loc) * 2)) {
    handler = &jmploc;
    redirect(redir, flags);
  }
  handler = savehandler;
  if (err && exception != EXERROR) longjmp(handler->loc, 1);
  RESTOREINT(saveint);
  return err;
}

static void unwindredir(struct redirtab *stop) {
  while (redirlist != stop) popredir(0);
}

static struct redirtab *pushredir(union node *redir) {
  struct redirtab *sv;
  struct redirtab *q;
  int i;
  q = redirlist;
  if (!redir) goto out;
  sv = ckmalloc(sizeof(struct redirtab));
  sv->next = q;
  redirlist = sv;
  for (i = 0; i < 10; i++) sv->renamed[i] = EMPTY;
out:
  return q;
}

/*
 * The trap builtin.
 */
static int trapcmd(int argc, char **argv) {
  char *action;
  char **ap;
  int signo;
  nextopt(nullstr);
  ap = argptr;
  if (!*ap) {
    for (signo = 0; signo < NSIG; signo++) {
      if (trap[signo] != NULL) {
        out1fmt("trap -- %s %s\n", single_quote(trap[signo]), strsignal(signo));
      }
    }
    return 0;
  }
  if (!ap[1] || decode_signum(*ap) >= 0)
    action = NULL;
  else
    action = *ap++;
  while (*ap) {
    if ((signo = decode_signal(*ap, 0)) < 0) {
      outfmt(out2, "trap: %s: bad trap\n", *ap);
      return 1;
    }
    INTOFF;
    if (action) {
      if (action[0] == '-' && action[1] == '\0')
        action = NULL;
      else {
        if (*action) trapcnt++;
        action = savestr(action);
      }
    }
    if (trap[signo]) {
      if (*trap[signo]) trapcnt--;
      ckfree(trap[signo]);
    }
    trap[signo] = action;
    if (signo != 0) setsignal(signo);
    INTON;
    ap++;
  }
  return 0;
}

/*
 * Set the signal handler for the specified signal.  The routine figures
 * out what it should be set to.
 */
static void setsignal(int signo) {
  int action;
  int lvforked;
  char *t, tsig;
  struct sigaction act;
  lvforked = vforked;
  if ((t = trap[signo]) == NULL)
    action = S_DFL;
  else if (*t != '\0')
    action = S_CATCH;
  else
    action = S_IGN;
  if (rootshell && action == S_DFL && !lvforked) {
    if (signo == SIGINT) {
      if (iflag || minusc || sflag == 0) action = S_CATCH;
    } else if (signo == SIGQUIT || signo == SIGTERM) {
      if (iflag) action = S_IGN;
    } else if (signo == SIGTSTP || signo == SIGTTOU) {
      if (mflag) action = S_IGN;
    }
  }
  if (signo == SIGCHLD) action = S_CATCH;
  t = &sigmode[signo - 1];
  tsig = *t;
  if (tsig == 0) {
    /*
     * current setting unknown
     */
    if (sigaction(signo, 0, &act) == -1) {
      /*
       * Pretend it worked; maybe we should give a warning
       * here, but other shells don't. We don't alter
       * sigmode, so that we retry every time.
       */
      return;
    }
    if (act.sa_handler == SIG_IGN) {
      if (mflag && (signo == SIGTSTP || signo == SIGTTIN || signo == SIGTTOU)) {
        tsig = S_IGN; /* don't hard ignore these */
      } else
        tsig = S_HARD_IGN;
    } else {
      tsig = S_RESET; /* force to be set */
    }
  }
  if (tsig == S_HARD_IGN || tsig == action) return;
  switch (action) {
    case S_CATCH:
      act.sa_handler = onsig;
      break;
    case S_IGN:
      act.sa_handler = SIG_IGN;
      break;
    default:
      act.sa_handler = SIG_DFL;
  }
  if (!lvforked) *t = action;
  act.sa_flags = 0;
  sigfillset(&act.sa_mask);
  sigaction(signo, &act, 0);
}

/*
 * Ignore a signal.
 */
static void ignoresig(int signo) {
  if (sigmode[signo - 1] != S_IGN && sigmode[signo - 1] != S_HARD_IGN) {
    signal(signo, SIG_IGN);
  }
  if (!vforked) sigmode[signo - 1] = S_HARD_IGN;
}

/*
 * Signal handler.
 */
static void onsig(int signo) {
  if (vforked) return;
  if (signo == SIGCHLD) {
    gotsigchld = 1;
    if (!trap[SIGCHLD]) return;
  }
  gotsig[signo - 1] = 1;
  pending_sig = signo;
  if (signo == SIGINT && !trap[SIGINT]) {
    if (!suppressint) onint();
    intpending = 1;
  }
}

/*
 * Called to execute a trap.  Perhaps we should avoid entering new trap
 * handlers while we are executing a trap handler.
 */
static void dotrap(void) {
  char *p;
  char *q;
  int i;
  int status, last_status;
  if (!pending_sig) return;
  status = savestatus;
  last_status = status;
  if (likely(status < 0)) {
    status = exitstatus;
    savestatus = status;
  }
  pending_sig = 0;
  barrier();
  for (i = 0, q = gotsig; i < NSIG - 1; i++, q++) {
    if (!*q) continue;
    if (evalskip) {
      pending_sig = i + 1;
      break;
    }
    *q = 0;
    p = trap[i + 1];
    if (!p) continue;
    evalstring(p, 0);
    if (evalskip != SKIPFUNC) exitstatus = status;
  }
  savestatus = last_status;
}

/*
 * Controls whether the shell is interactive or not.
 */
static void setinteractive(int on) {
  static int is_interactive;
  if (++on == is_interactive) return;
  is_interactive = on;
  setsignal(SIGINT);
  setsignal(SIGQUIT);
  setsignal(SIGTERM);
}

/*
 * Called to exit the shell.
 */
wontreturn static void exitshell(void) {
  struct jmploc loc;
  char *p;
  savestatus = exitstatus;
  TRACE(("pid %d, exitshell(%d)\n", getpid(), savestatus));
  if (setjmp(loc.loc)) goto out;
  handler = &loc;
  if ((p = trap[0])) {
    trap[0] = NULL;
    evalskip = 0;
    evalstring(p, 0);
    evalskip = SKIPFUNCDEF;
  }
out:
  exitreset();
  /*
   * Disable job control so that whoever had the foreground before we
   * started can get it back.
   */
  if (likely(!setjmp(loc.loc))) setjobctl(0);
  flushall();
  _exit(exitstatus);
}

static int decode_signal(const char *string, int minsig) {
  int signo;
  signo = decode_signum(string);
  if (signo >= 0) return signo;
  for (signo = minsig; signo < NSIG; signo++) {
    if (!strcasecmp(string, strsignal(signo))) {
      return signo;
    }
  }
  return -1;
}

static void sigblockall(sigset_t *oldmask) {
  sigset_t mask;
  sigfillset(&mask);
  sigprocmask(SIG_SETMASK, &mask, oldmask);
}

#define PF(f, func)                                \
  {                                                \
    switch ((char *)param - (char *)array) {       \
      default:                                     \
        (void)Printf(f, array[0], array[1], func); \
        break;                                     \
      case sizeof(*param):                         \
        (void)Printf(f, array[0], func);           \
        break;                                     \
      case 0:                                      \
        (void)Printf(f, func);                     \
        break;                                     \
    }                                              \
  }

#define ASPF(sp, f, func)                                 \
  ({                                                      \
    int ret;                                              \
    switch ((char *)param - (char *)array) {              \
      default:                                            \
        ret = Xasprintf(sp, f, array[0], array[1], func); \
        break;                                            \
      case sizeof(*param):                                \
        ret = Xasprintf(sp, f, array[0], func);           \
        break;                                            \
      case 0:                                             \
        ret = Xasprintf(sp, f, func);                     \
        break;                                            \
    }                                                     \
    ret;                                                  \
  })

static int print_escape_str(const char *f, int *param, int *array, char *s) {
  struct stackmark smark;
  char *p, *q;
  int done;
  int len;
  int total;
  setstackmark(&smark);
  done = conv_escape_str(s, &q);
  p = stackblock();
  len = q - p;
  total = len - 1;
  q[-1] = (!!((f[1] - 's') | done) - 1) & f[2];
  total += !!q[-1];
  if (f[1] == 's') goto easy;
  p = makestrspace(len, q);
  memset(p, 'X', total);
  p[total] = 0;
  q = stackblock();
  total = ASPF(&p, f, p);
  len = strchrnul(p, 'X') - p;
  memcpy(p + len, q, strspn(p + len, "X"));
easy:
  outmem(p, total, out1);
  popstackmark(&smark);
  return done;
}

static int printfcmd(int argc, char *argv[]) {
  static const char kSkip1[] = "#-+ 0";
  static const char kSkip2[] = "*0123456789";
  char *fmt;
  char *format;
  int ch;
  rval = 0;
  nextopt(nullstr);
  argv = argptr;
  format = *argv;
  if (!format) sh_error("usage: printf format [arg ...]");
  gargv = ++argv;
  do {
    /*
     * Basic algorithm is to scan the format string for conversion
     * specifications -- once one is found, find out if the field
     * width or precision is a '*'; if it is, gather up value.
     * Note, format strings are reused as necessary to use up the
     * provided arguments, arguments of zero/null string are
     * provided to use up the format string.
     */
    /* find next format specification */
    for (fmt = format; (ch = *fmt++);) {
      char *start;
      char nextch;
      int array[2];
      int *param;
      if (ch == '\\') {
        int c_ch;
        fmt = conv_escape(fmt, &c_ch);
        ch = c_ch;
        goto pc;
      }
      if (ch != '%' || (*fmt == '%' && (++fmt || 1))) {
      pc:
        outc(ch, out1);
        continue;
      }
      /* Ok - we've found a format specification,
         Save its address for a later printf(). */
      start = fmt - 1;
      param = array;
      /* skip to field width */
      fmt += strspn(fmt, kSkip1);
      if (*fmt == '*') {
        ++fmt;
        *param++ = getuintmax(1);
      } else {
        /* skip to possible '.',
         * get following precision
         */
        fmt += strspn(fmt, kSkip2);
      }
      if (*fmt == '.') {
        ++fmt;
        if (*fmt == '*') {
          ++fmt;
          *param++ = getuintmax(1);
        } else
          fmt += strspn(fmt, kSkip2);
      }
      ch = *fmt;
      if (!ch) sh_error("missing format character");
      /* null terminate format string to we can use it
         as an argument to printf. */
      nextch = fmt[1];
      fmt[1] = 0;
      switch (ch) {
        case 'b':
          *fmt = 's';
          /* escape if a \c was encountered */
          if (print_escape_str(start, param, array, getstr())) goto out;
          *fmt = 'b';
          break;
        case 'c': {
          int p = getchr();
          PF(start, p);
          break;
        }
        case 's': {
          char *p = getstr();
          PF(start, p);
          break;
        }
        case 'd':
        case 'i': {
          uint64_t p = getuintmax(1);
          start = mklong(start, fmt);
          PF(start, p);
          break;
        }
        case 'o':
        case 'u':
        case 'x':
        case 'X': {
          uint64_t p = getuintmax(0);
          start = mklong(start, fmt);
          PF(start, p);
          break;
        }
        case 'a':
        case 'A':
        case 'e':
        case 'E':
        case 'f':
        case 'F':
        case 'g':
        case 'G': {
          double p = getdouble();
          PF(start, p);
          break;
        }
        default:
          sh_error("%s: invalid directive", start);
      }
      *++fmt = nextch;
    }
  } while (gargv != argv && *gargv);
out:
  return rval;
}

/*
 * Print SysV echo(1) style escape string
 *  Halts processing string if a \c escape is encountered.
 */
static int conv_escape_str(char *str, char **sp) {
  int c;
  int ch;
  char *cp;
  /* convert string into a temporary buffer... */
  STARTSTACKSTR(cp);
  do {
    c = ch = *str++;
    if (ch != '\\') continue;
    c = *str++;
    if (c == 'c') {
      /* \c as in SYSV echo - abort all processing.... */
      c = ch = 0x100;
      continue;
    }
    /*
     * %b string octal constants are not like those in C.
     * They start with a \0, and are followed by 0, 1, 2,
     * or 3 octal digits.
     */
    if (c == '0' && isodigit(*str)) str++;
    /* Finally test for sequences valid in the format string */
    str = conv_escape(str - 1, &c);
  } while (STPUTC(c, cp), (char)ch);
  *sp = cp;
  return ch;
}

/*
 * Print "standard" escape characters
 */
static char *conv_escape(char *str, int *conv_ch) {
  int value;
  int ch;
  ch = *str;
  switch (ch) {
    default:
      if (!isodigit(*str)) {
        value = '\\';
        goto out;
      }
      ch = 3;
      value = 0;
      do {
        value <<= 3;
        value += octtobin(*str++);
      } while (isodigit(*str) && --ch);
      goto out;
    case '\\':
      value = '\\';
      break; /* backslash */
    case 'a':
      value = '\a';
      break; /* alert */
    case 'b':
      value = '\b';
      break; /* backspace */
    case 'f':
      value = '\f';
      break; /* form-feed */
    case 'e':
      value = '\e';
      break; /* escape */
    case 'n':
      value = '\n';
      break; /* newline */
    case 'r':
      value = '\r';
      break; /* carriage-return */
    case 't':
      value = '\t';
      break; /* tab */
    case 'v':
      value = '\v';
      break; /* vertical-tab */
  }
  str++;
out:
  *conv_ch = value;
  return str;
}

#define PRIdMAX "ld"

static char *mklong(const char *str, const char *ch) {
  /*
   * Replace a string like "%92.3u" with "%92.3"PRIuMAX.
   *
   * Although C99 does not guarantee it, we assume PRIiMAX,
   * PRIoMAX, PRIuMAX, PRIxMAX, and PRIXMAX are all the same
   * as PRIdMAX with the final 'd' replaced by the corresponding
   * character.
   */
  char *copy;
  unsigned len;
  len = ch - str + sizeof(PRIdMAX);
  STARTSTACKSTR(copy);
  copy = makestrspace(len, copy);
  memcpy(copy, str, len - sizeof(PRIdMAX));
  memcpy(copy + len - sizeof(PRIdMAX), PRIdMAX, sizeof(PRIdMAX));
  copy[len - 2] = *ch;
  return (copy);
}

static uint64_t getuintmax(int sign) {
  uint64_t val = 0;
  char *cp, *ep;
  cp = *gargv;
  if (cp == NULL) goto out;
  gargv++;
  val = (unsigned char)cp[1];
  if (*cp == '\"' || *cp == '\'') goto out;
  errno = 0;
  val = sign ? strtoimax(cp, &ep, 0) : strtoumax(cp, &ep, 0);
  check_conversion(cp, ep);
out:
  return val;
}

static double getdouble(void) {
  double val;
  char *cp, *ep;
  cp = *gargv;
  if (cp == NULL) return 0;
  gargv++;
  if (*cp == '\"' || *cp == '\'') return (unsigned char)cp[1];
  errno = 0;
  val = strtod(cp, &ep);
  check_conversion(cp, ep);
  return val;
}

static void check_conversion(const char *s, const char *ep) {
  if (*ep) {
    if (ep == s)
      sh_warnx("%s: expected numeric value", s);
    else
      sh_warnx("%s: not completely converted", s);
    rval = 1;
  } else if (errno == ERANGE) {
    sh_warnx("%s: %s", s, strerror(ERANGE));
    rval = 1;
  }
}

static int echocmd(int argc, char **argv) {
  const char *lastfmt = snlfmt;
  int nonl;
  if (*++argv && equal(*argv, "-n")) {
    argv++;
    lastfmt = "%s";
  }
  do {
    const char *fmt = "%s ";
    char *s = *argv;
    if (!s || !*++argv) fmt = lastfmt;
    nonl = print_escape_str(fmt, NULL, NULL, s ?: nullstr);
  } while (!nonl && *argv);
  return 0;
}

/*
 * test(1); version 7-like  --  author Erik Baalbergen
 * modified by Eric Gisin to be used as built-in.
 * modified by Arnold Robbins to add SVR3 compatibility
 * (-x -c -b -p -u -g -k) plus Korn's -L -nt -ot -ef and new -S (socket).
 * modified by J.T. Conklin for NetBSD.
 *
 * This program is in the Public Domain.
 */

/* test(1) accepts the following grammar:
        oexpr ::= aexpr | aexpr "-o" oexpr ;
        aexpr ::= nexpr | nexpr "-a" aexpr ;
        nexpr ::= primary | "!" primary
        primary ::= unary-operator operand
                | operand binary-operator operand
                | operand
                | "(" oexpr ")"
                ;
        unary-operator ::= "-r"|"-w"|"-x"|"-f"|"-d"|"-c"|"-b"|"-p"|
                "-u"|"-g"|"-k"|"-s"|"-t"|"-z"|"-n"|"-o"|"-O"|"-G"|"-L"|"-S";
        binary-operator ::= "="|"!="|"-eq"|"-ne"|"-ge"|"-gt"|"-le"|"-lt"|
                        "-nt"|"-ot"|"-ef";
        operand ::= <any legal UNIX file name>
*/

static inline int faccessat_confused_about_superuser(void) {
  return 0;
}

static const struct t_op *getop(const char *s) {
  const struct t_op *op;
  for (op = ops; op->op_text; op++) {
    if (strcmp(s, op->op_text) == 0) return op;
  }
  return NULL;
}

static int testcmd(int argc, char **argv) {
  const struct t_op *op;
  enum token n;
  int res = 1;
  if (*argv[0] == '[') {
    if (*argv[--argc] != ']') sh_error("missing ]");
    argv[argc] = NULL;
  }
  t_wp_op = NULL;
recheck:
  argv++;
  argc--;
  if (argc < 1) return res;
  /*
   * POSIX prescriptions: he who wrote this deserves the Nobel
   * peace prize.
   */
  switch (argc) {
    case 3:
      op = getop(argv[1]);
      if (op && op->op_type == BINOP) {
        n = OPERAND;
        goto eval;
      }
      /* fall through */
    case 4:
      if (!strcmp(argv[0], "(") && !strcmp(argv[argc - 1], ")")) {
        argv[--argc] = NULL;
        argv++;
        argc--;
      } else if (!strcmp(argv[0], "!")) {
        res = 0;
        goto recheck;
      }
  }
  n = t_lex(argv);
eval:
  t_wp = argv;
  res ^= oexpr(n);
  argv = t_wp;
  if (argv[0] != NULL && argv[1] != NULL)
    syntax(argv[0], "unexpected operator");
  return res;
}

static void syntax(const char *op, const char *msg) {
  if (op && *op) {
    sh_error("%s: %s", op, msg);
  } else {
    sh_error("%s", msg);
  }
}

static int oexpr(enum token n) {
  int res = 0;
  for (;;) {
    res |= aexpr(n);
    n = t_lex(t_wp + 1);
    if (n != BOR) break;
    n = t_lex(t_wp += 2);
  }
  return res;
}

static int aexpr(enum token n) {
  int res = 1;
  for (;;) {
    if (!nexpr(n)) res = 0;
    n = t_lex(t_wp + 1);
    if (n != BAND) break;
    n = t_lex(t_wp += 2);
  }
  return res;
}

static int nexpr(enum token n) {
  if (n != UNOT) return primary1(n);
  n = t_lex(t_wp + 1);
  if (n != EOI) t_wp++;
  return !nexpr(n);
}

static int primary1(enum token n) {
  enum token nn;
  int res;
  if (n == EOI) return 0; /* missing expression */
  if (n == LPAREN) {
    if ((nn = t_lex(++t_wp)) == RPAREN) return 0; /* missing expression */
    res = oexpr(nn);
    if (t_lex(++t_wp) != RPAREN) syntax(NULL, "closing paren expected");
    return res;
  }
  if (t_wp_op && t_wp_op->op_type == UNOP) {
    /* unary expression */
    if (*++t_wp == NULL) syntax(t_wp_op->op_text, "argument expected");
    switch (n) {
      case STREZ:
        return strlen(*t_wp) == 0;
      case STRNZ:
        return strlen(*t_wp) != 0;
      case FILTT:
        return isatty(getn(*t_wp));
      case FILRD:
        return test_file_access(*t_wp, R_OK);
      case FILWR:
        return test_file_access(*t_wp, W_OK);
      case FILEX:
        return test_file_access(*t_wp, X_OK);
      default:
        return filstat(*t_wp, n);
    }
  }
  if (t_lex(t_wp + 1), t_wp_op && t_wp_op->op_type == BINOP) {
    return binop0();
  }
  return strlen(*t_wp) > 0;
}

static int binop0(void) {
  const char *opnd1, *opnd2;
  struct t_op const *op;
  opnd1 = *t_wp;
  (void)t_lex(++t_wp);
  op = t_wp_op;
  if ((opnd2 = *++t_wp) == (char *)0) syntax(op->op_text, "argument expected");
  switch (op->op_num) {
    default:
    case STREQ:
      return strcmp(opnd1, opnd2) == 0;
    case STRNE:
      return strcmp(opnd1, opnd2) != 0;
    case STRLT:
      return strcmp(opnd1, opnd2) < 0;
    case STRGT:
      return strcmp(opnd1, opnd2) > 0;
    case INTEQ:
      return getn(opnd1) == getn(opnd2);
    case INTNE:
      return getn(opnd1) != getn(opnd2);
    case INTGE:
      return getn(opnd1) >= getn(opnd2);
    case INTGT:
      return getn(opnd1) > getn(opnd2);
    case INTLE:
      return getn(opnd1) <= getn(opnd2);
    case INTLT:
      return getn(opnd1) < getn(opnd2);
    case FILNT:
      return newerf(opnd1, opnd2);
    case FILOT:
      return olderf(opnd1, opnd2);
    case FILEQ:
      return equalf(opnd1, opnd2);
  }
}

static int filstat(char *nm, enum token mode) {
  struct stat s;
  if (mode == FILSYM ? lstat(nm, &s) : stat(nm, &s)) return 0;
  switch (mode) {
    case FILEXIST:
      return 1;
    case FILREG:
      return S_ISREG(s.st_mode);
    case FILDIR:
      return S_ISDIR(s.st_mode);
    case FILCDEV:
      return S_ISCHR(s.st_mode);
    case FILBDEV:
      return S_ISBLK(s.st_mode);
    case FILFIFO:
      return S_ISFIFO(s.st_mode);
    case FILSOCK:
      return S_ISSOCK(s.st_mode);
    case FILSYM:
      return S_ISLNK(s.st_mode);
    case FILSUID:
      return (s.st_mode & S_ISUID) != 0;
    case FILSGID:
      return (s.st_mode & S_ISGID) != 0;
    case FILSTCK:
      return (s.st_mode & S_ISVTX) != 0;
    case FILGZ:
      return !!s.st_size;
    case FILUID:
      return s.st_uid == geteuid();
    case FILGID:
      return s.st_gid == getegid();
    default:
      return 1;
  }
}

static enum token t_lex(char **tp) {
  struct t_op const *op;
  char *s = *tp;
  if (s == 0) {
    t_wp_op = (struct t_op *)0;
    return EOI;
  }
  op = getop(s);
  if (op && !(op->op_type == UNOP && isoperand(tp)) &&
      !(op->op_num == LPAREN && !tp[1])) {
    t_wp_op = op;
    return op->op_num;
  }
  t_wp_op = (struct t_op *)0;
  return OPERAND;
}

static int isoperand(char **tp) {
  struct t_op const *op;
  char *s;
  if (!(s = tp[1])) return 1;
  if (!tp[2]) return 0;
  op = getop(s);
  return op && op->op_type == BINOP;
}

static int newerf(const char *f1, const char *f2) {
  struct stat b1, b2;
  return (stat(f1, &b1) == 0 && stat(f2, &b2) == 0 &&
          (b1.st_mtim.tv_sec > b2.st_mtim.tv_sec ||
           (b1.st_mtim.tv_sec == b2.st_mtim.tv_sec &&
            (b1.st_mtim.tv_nsec > b2.st_mtim.tv_nsec))));
}

static int olderf(const char *f1, const char *f2) {
  struct stat b1, b2;
  return (stat(f1, &b1) == 0 && stat(f2, &b2) == 0 &&
          (b1.st_mtim.tv_sec < b2.st_mtim.tv_sec ||
           (b1.st_mtim.tv_sec == b2.st_mtim.tv_sec &&
            (b1.st_mtim.tv_nsec < b2.st_mtim.tv_nsec))));
}

static int equalf(const char *f1, const char *f2) {
  struct stat b1, b2;
  return (stat(f1, &b1) == 0 && stat(f2, &b2) == 0 && b1.st_dev == b2.st_dev &&
          b1.st_ino == b2.st_ino);
}

static int has_exec_bit_set(const char *path) {
  struct stat st;
  if (stat(path, &st)) return 0;
  return st.st_mode & (S_IXUSR | S_IXGRP | S_IXOTH);
}

static int test_file_access(const char *path, int mode) {
  if (faccessat_confused_about_superuser() && mode == X_OK && geteuid() == 0 &&
      !has_exec_bit_set(path)) {
    return 0;
  }
  return !faccessat(AT_FDCWD, path, mode, AT_EACCESS);
}

static int timescmd() {
  struct tms buf;
  long int clk_tck = sysconf(_SC_CLK_TCK);
  int mutime, mstime, mcutime, mcstime;
  double utime, stime, cutime, cstime;
  times(&buf);
  utime = (double)buf.tms_utime / clk_tck;
  mutime = utime / 60;
  utime -= mutime * 60.0;
  stime = (double)buf.tms_stime / clk_tck;
  mstime = stime / 60;
  stime -= mstime * 60.0;
  cutime = (double)buf.tms_cutime / clk_tck;
  mcutime = cutime / 60;
  cutime -= mcutime * 60.0;
  cstime = (double)buf.tms_cstime / clk_tck;
  mcstime = cstime / 60;
  cstime -= mcstime * 60.0;
  Printf("%dm%fs %dm%fs\n%dm%fs %dm%fs\n", mutime, utime, mstime, stime,
         mcutime, cutime, mcstime, cstime);
  return 0;
}

/*
 * Find the appropriate entry in the hash table from the name.
 */
static struct Var **hashvar(const char *p) {
  unsigned int hashval;
  hashval = ((unsigned char)*p) << 4;
  while (*p && *p != '=') hashval += (unsigned char)*p++;
  return &vartab[hashval % VTABSIZE];
}

/*
 * This routine initializes the builtin variables.  It is called when the
 * shell is initialized.
 */
static void initvar(void) {
  struct Var *vp;
  struct Var *end;
  struct Var **vpp;
  vp = varinit;
  end = vp + sizeof(varinit) / sizeof(varinit[0]);
  do {
    vpp = hashvar(vp->text);
    vp->next = *vpp;
    *vpp = vp;
  } while (++vp < end);
  /* PS1 depends on uid */
  if (!geteuid()) {
    vps1.text = "PS1=# ";
  }
}

/*
 * Set the value of a variable.  The flags argument is ored with the
 * flags of the variable.  If val is NULL, the variable is unset.
 */
static struct Var *setvar(const char *name, const char *val, int flags) {
  char *p, *q;
  unsigned namelen;
  char *nameeq;
  unsigned vallen;
  struct Var *vp;
  q = endofname(name);
  p = strchrnul(q, '=');
  namelen = p - name;
  if (!namelen || p != q) sh_error("%.*s: bad variable name", namelen, name);
  vallen = 0;
  if (val == NULL) {
    flags |= VUNSET;
  } else {
    vallen = strlen(val);
  }
  INTOFF;
  p = mempcpy(nameeq = ckmalloc(namelen + vallen + 2), name, namelen);
  if (val) {
    *p++ = '=';
    p = mempcpy(p, val, vallen);
  }
  *p = '\0';
  vp = setvareq(nameeq, flags | VNOSAVE);
  INTON;
  return vp;
}

/*
 * Set the given integer as the value of a variable.  The flags argument is
 * ored with the flags of the variable.
 */
static int64_t setvarint(const char *name, int64_t val, int flags) {
  int len = max_int_length(sizeof(val));
  char buf[len];
  fmtstr(buf, len, "%ld", val);
  setvar(name, buf, flags);
  return val;
}

static struct Var **findvar(struct Var **vpp, const char *name) {
  for (; *vpp; vpp = &(*vpp)->next) {
    if (varequal((*vpp)->text, name)) {
      break;
    }
  }
  return vpp;
}

/*
 * Same as setvar except that the variable and value are passed in
 * the first argument as name=value.  Since the first argument will
 * be actually stored in the table, it should not be a string that
 * will go away.
 * Called with interrupts off.
 */
static struct Var *setvareq(char *s, int flags) {
  struct Var *vp, **vpp;
  vpp = hashvar(s);
  flags |= (VEXPORT & (((unsigned)(1 - aflag)) - 1));
  vpp = findvar(vpp, s);
  vp = *vpp;
  if (vp) {
    if (vp->flags & VREADONLY) {
      const char *n;
      if (flags & VNOSAVE) free(s);
      n = vp->text;
      sh_error("%.*s: is read only", strchrnul(n, '=') - n, n);
    }
    if (flags & VNOSET) goto out;
    if (vp->func && (flags & VNOFUNC) == 0) (*vp->func)(strchrnul(s, '=') + 1);
    if ((vp->flags & (VTEXTFIXED | VSTACK)) == 0) ckfree(vp->text);
    if (((flags & (VEXPORT | VREADONLY | VSTRFIXED | VUNSET)) |
         (vp->flags & VSTRFIXED)) == VUNSET) {
      *vpp = vp->next;
      ckfree(vp);
    out_free:
      if ((flags & (VTEXTFIXED | VSTACK | VNOSAVE)) == VNOSAVE) ckfree(s);
      goto out;
    }
    flags |= vp->flags & ~(VTEXTFIXED | VSTACK | VNOSAVE | VUNSET);
  } else {
    if (flags & VNOSET) goto out;
    if ((flags & (VEXPORT | VREADONLY | VSTRFIXED | VUNSET)) == VUNSET)
      goto out_free;
    /* not found */
    vp = ckmalloc(sizeof(*vp));
    vp->next = *vpp;
    vp->func = NULL;
    *vpp = vp;
  }
  if (!(flags & (VTEXTFIXED | VSTACK | VNOSAVE))) s = savestr(s);
  vp->text = s;
  vp->flags = flags;
out:
  return vp;
}

/*
 * Find the value of a variable.  Returns NULL if not set.
 */
static char *lookupvar(const char *name) {
  struct Var *v;
  if ((v = *findvar(hashvar(name), name)) && !(v->flags & VUNSET)) {
    if (v == &vlineno && v->text == linenovar) {
      fmtstr(linenovar + 7, sizeof(linenovar) - 7, "%d", lineno);
    }
    return strchrnul(v->text, '=') + 1;
  }
  return NULL;
}

static int64_t lookupvarint(const char *name) {
  return atomax(lookupvar(name) ?: nullstr, 0);
}

/*
 * Generate a list of variables satisfying the given conditions.
 */
static char **listvars(int on, int off, char ***end) {
  struct Var **vpp;
  struct Var *vp;
  char **ep;
  int mask;
  STARTSTACKSTR(ep);
  vpp = vartab;
  mask = on | off;
  do {
    for (vp = *vpp; vp; vp = vp->next)
      if ((vp->flags & mask) == on) {
        if (ep == stackstrend()) ep = growstackstr();
        *ep++ = (char *)vp->text;
      }
  } while (++vpp < vartab + VTABSIZE);
  if (ep == stackstrend()) ep = growstackstr();
  if (end) *end = ep;
  *ep++ = NULL;
  return grabstackstr(ep);
}

static int vpcmp(const void *a, const void *b) {
  return varcmp(*(const char **)a, *(const char **)b);
}

/*
 * POSIX requires that 'set' (but not export or readonly) output the
 * variables in lexicographic order - by the locale's collating order (sigh).
 * Maybe we could keep them in an ordered balanced binary tree
 * instead of hashed lists.
 * For now just roll 'em through qsort for printing...
 */
static int showvars(const char *prefix, int on, int off) {
  const char *sep;
  char **ep, **epend;
  ep = listvars(on, off, &epend);
  qsort(ep, epend - ep, sizeof(char *), vpcmp);
  sep = *prefix ? spcstr : prefix;
  for (; ep < epend; ep++) {
    const char *p;
    const char *q;
    p = strchrnul(*ep, '=');
    q = nullstr;
    if (*p) q = single_quote(++p);
    out1fmt("%s%s%.*s%s\n", prefix, sep, (int)(p - *ep), *ep, q);
  }
  return 0;
}

/*
 * The export and readonly commands.
 */
static int exportcmd(int argc, char **argv) {
  struct Var *vp;
  char *name;
  const char *p;
  char **aptr;
  int flag = argv[0][0] == 'r' ? VREADONLY : VEXPORT;
  int notp;
  notp = nextopt("p") - 'p';
  if (notp && ((name = *(aptr = argptr)))) {
    do {
      if ((p = strchr(name, '=')) != NULL) {
        p++;
      } else {
        if ((vp = *findvar(hashvar(name), name))) {
          vp->flags |= flag;
          continue;
        }
      }
      setvar(name, p, flag);
    } while ((name = *++aptr) != NULL);
  } else {
    showvars(argv[0], flag, 0);
  }
  return 0;
}

/*
 * The "local" command.
 */
static int localcmd(int argc, char **argv) {
  char *name;
  if (!localvar_stack) sh_error("not in a function");
  argv = argptr;
  while ((name = *argv++) != NULL) {
    mklocal(name, 0);
  }
  return 0;
}

/*
 * Make a variable a local variable.  When a variable is made local, it's
 * value and flags are saved in a localvar structure.  The saved values
 * will be restored when the shell function returns.  We handle the name
 * "-" as a special case.
 */
static void mklocal(char *name, int flags) {
  struct localvar *lvp;
  struct Var **vpp;
  struct Var *vp;
  INTOFF;
  lvp = ckmalloc(sizeof(struct localvar));
  if (name[0] == '-' && name[1] == '\0') {
    char *p;
    p = ckmalloc(sizeof(optlist));
    lvp->text = memcpy(p, optlist, sizeof(optlist));
    vp = NULL;
  } else {
    char *eq;
    vpp = hashvar(name);
    vp = *findvar(vpp, name);
    eq = strchr(name, '=');
    if (vp == NULL) {
      if (eq)
        vp = setvareq(name, VSTRFIXED | flags);
      else
        vp = setvar(name, NULL, VSTRFIXED | flags);
      lvp->flags = VUNSET;
    } else {
      lvp->text = vp->text;
      lvp->flags = vp->flags;
      vp->flags |= VSTRFIXED | VTEXTFIXED;
      if (eq) setvareq(name, flags);
    }
  }
  lvp->vp = vp;
  lvp->next = localvar_stack->lv;
  localvar_stack->lv = lvp;
  INTON;
}

/*
 * Called after a function returns.
 * Interrupts must be off.
 */
static void poplocalvars(void) {
  struct localvar_list *ll;
  struct localvar *lvp, *next;
  struct Var *vp;
  INTOFF;
  ll = localvar_stack;
  localvar_stack = ll->next;
  next = ll->lv;
  ckfree(ll);
  while ((lvp = next) != NULL) {
    next = lvp->next;
    vp = lvp->vp;
    TRACE(("poplocalvar %s\n", vp ? vp->text : "-"));
    if (vp == NULL) { /* $- saved */
      memcpy(optlist, lvp->text, sizeof(optlist));
      ckfree(lvp->text);
      optschanged();
    } else if (lvp->flags == VUNSET) {
      vp->flags &= ~(VSTRFIXED | VREADONLY);
      unsetvar(vp->text);
    } else {
      if (vp->func) (*vp->func)(strchrnul(lvp->text, '=') + 1);
      if ((vp->flags & (VTEXTFIXED | VSTACK)) == 0) ckfree(vp->text);
      vp->flags = lvp->flags;
      vp->text = lvp->text;
    }
    ckfree(lvp);
  }
  INTON;
}

/*
 * Create a new localvar environment.
 */
static struct localvar_list *pushlocalvars(int push) {
  struct localvar_list *ll;
  struct localvar_list *top;
  top = localvar_stack;
  if (!push) goto out;
  INTOFF;
  ll = ckmalloc(sizeof(*ll));
  ll->lv = NULL;
  ll->next = top;
  localvar_stack = ll;
  INTON;
out:
  return top;
}

static void unwindlocalvars(struct localvar_list *stop) {
  while (localvar_stack != stop) poplocalvars();
}

/*
 * The unset builtin command.  We unset the function before we unset the
 * variable to allow a function to be unset when there is a readonly variable
 * with the same name.
 */
static int unsetcmd(int argc, char **argv) {
  char **ap;
  int i;
  int flag = 0;
  while ((i = nextopt("vf")) != '\0') {
    flag = i;
  }
  for (ap = argptr; *ap; ap++) {
    if (flag != 'f') {
      unsetvar(*ap);
      continue;
    }
    if (flag != 'v') unsetfunc(*ap);
  }
  return 0;
}

static void unsetvar(const char *s) {
  setvar(s, 0, 0);
}

/*
 * Initialization code.
 */
static void init() {
  /* from input.c: */
  {
    basepf.nextc = basepf.buf = basebuf;
    basepf.linno = 1;
  }
  /* from trap.c: */
  {
    sigmode[SIGCHLD - 1] = S_DFL;
    setsignal(SIGCHLD);
  }
  /* from var.c: */
  {
    char **envp;
    static char ppid[32] = "PPID=";
    const char *p;
    struct stat st1, st2;
    initvar();
    for (envp = environ; *envp; envp++) {
      p = endofname(*envp);
      if (p != *envp && *p == '=') {
        setvareq(*envp, VEXPORT | VTEXTFIXED);
      }
    }
    setvareq(defifsvar, VTEXTFIXED);
    setvareq(defoptindvar, VTEXTFIXED);
    fmtstr(ppid + 5, sizeof(ppid) - 5, "%ld", (long)getppid());
    setvareq(ppid, VTEXTFIXED);
    p = lookupvar("PWD");
    if (p) {
      if (*p != '/' || stat(p, &st1) || stat(".", &st2) ||
          st1.st_dev != st2.st_dev || st1.st_ino != st2.st_ino) {
        p = 0;
      }
    }
    setpwd(p, 0);
  }
}

/*
 * This routine is called when an error or an interrupt occurs in an
 * interactive shell and control is returned to the main command loop
 * but prior to exitshell.
 */
static void exitreset() {
  /* from eval.c: */
  {
    if (savestatus >= 0) {
      if (exception == EXEXIT || evalskip == SKIPFUNCDEF)
        exitstatus = savestatus;
      savestatus = -1;
    }
    evalskip = 0;
    loopnest = 0;
    inps4 = 0;
  }
  /* from expand.c: */
  { ifsfree(); }
  /* from redir.c: */
  {
    /*
     * Discard all saved file descriptors.
     */
    unwindredir(0);
  }
}

/*
 * This routine is called when we enter a subshell.
 */
static void forkreset() {
  /* from input.c: */
  {
    popallfiles();
    if (parsefile->fd > 0) {
      close(parsefile->fd);
      parsefile->fd = 0;
    }
  }
  /* from main.c: */
  { handler = &main_handler; }
  /* from redir.c: */
  { redirlist = NULL; }
  /* from trap.c: */
  {
    char **tp;
    INTOFF;
    for (tp = trap; tp < &trap[NSIG]; tp++) {
      if (*tp && **tp) { /* trap not NULL or SIG_IGN */
        ckfree(*tp);
        *tp = NULL;
        if (tp != &trap[0]) setsignal(tp - trap);
      }
    }
    trapcnt = 0;
    INTON;
  }
}

/*
 * This routine is called when an error or an interrupt occurs in an
 * interactive shell and control is returned to the main command loop.
 */
static void reset() {
  /* from input.c: */
  {
    /* clear input buffer */
    basepf.lleft = basepf.nleft = 0;
    basepf.unget = 0;
    popallfiles();
  }
  /* from var.c: */
  { unwindlocalvars(0); }
}

static void calcsize(union node *n) {
  if (n == NULL) return;
  funcblocksize += nodesize[n->type];
  switch (n->type) {
    case NCMD:
      calcsize(n->ncmd.redirect);
      calcsize(n->ncmd.args);
      calcsize(n->ncmd.assign);
      break;
    case NPIPE:
      sizenodelist(n->npipe.cmdlist);
      break;
    case NREDIR:
    case NBACKGND:
    case NSUBSHELL:
      calcsize(n->nredir.redirect);
      calcsize(n->nredir.n);
      break;
    case NAND:
    case NOR:
    case NSEMI:
    case NWHILE:
    case NUNTIL:
      calcsize(n->nbinary.ch2);
      calcsize(n->nbinary.ch1);
      break;
    case NIF:
      calcsize(n->nif.elsepart);
      calcsize(n->nif.ifpart);
      calcsize(n->nif.test);
      break;
    case NFOR:
      funcstringsize += strlen(n->nfor.var_) + 1;
      calcsize(n->nfor.body);
      calcsize(n->nfor.args);
      break;
    case NCASE:
      calcsize(n->ncase.cases);
      calcsize(n->ncase.expr);
      break;
    case NCLIST:
      calcsize(n->nclist.body);
      calcsize(n->nclist.pattern);
      calcsize(n->nclist.next);
      break;
    case NDEFUN:
      calcsize(n->ndefun.body);
      funcstringsize += strlen(n->ndefun.text) + 1;
      break;
    case NARG:
      sizenodelist(n->narg.backquote);
      funcstringsize += strlen(n->narg.text) + 1;
      calcsize(n->narg.next);
      break;
    case NTO:
    case NCLOBBER:
    case NFROM:
    case NFROMTO:
    case NAPPEND:
      calcsize(n->nfile.fname);
      calcsize(n->nfile.next);
      break;
    case NTOFD:
    case NFROMFD:
      calcsize(n->ndup.vname);
      calcsize(n->ndup.next);
      break;
    case NHERE:
    case NXHERE:
      calcsize(n->nhere.doc);
      calcsize(n->nhere.next);
      break;
    case NNOT:
      calcsize(n->nnot.com);
      break;
  };
}

/*
 * Make a copy of a parse tree.
 */
static struct funcnode *copyfunc(union node *n) {
  struct funcnode *f;
  unsigned blocksize;
  funcblocksize = offsetof(struct funcnode, n);
  funcstringsize = 0;
  calcsize(n);
  blocksize = funcblocksize;
  f = ckmalloc(blocksize + funcstringsize);
  funcblock = (char *)f + offsetof(struct funcnode, n);
  funcstring = (char *)f + blocksize;
  copynode(n);
  f->count = 0;
  return f;
}

static void sizenodelist(struct nodelist *lp) {
  while (lp) {
    funcblocksize += SHELL_ALIGN(sizeof(struct nodelist));
    calcsize(lp->n);
    lp = lp->next;
  }
}

static union node *copynode(union node *n) {
  union node *new;
  if (n == NULL) return NULL;
  new = funcblock;
  funcblock = (char *)funcblock + nodesize[n->type];
  switch (n->type) {
    case NCMD:
      new->ncmd.redirect = copynode(n->ncmd.redirect);
      new->ncmd.args = copynode(n->ncmd.args);
      new->ncmd.assign = copynode(n->ncmd.assign);
      new->ncmd.linno = n->ncmd.linno;
      break;
    case NPIPE:
      new->npipe.cmdlist = copynodelist(n->npipe.cmdlist);
      new->npipe.backgnd = n->npipe.backgnd;
      break;
    case NREDIR:
    case NBACKGND:
    case NSUBSHELL:
      new->nredir.redirect = copynode(n->nredir.redirect);
      new->nredir.n = copynode(n->nredir.n);
      new->nredir.linno = n->nredir.linno;
      break;
    case NAND:
    case NOR:
    case NSEMI:
    case NWHILE:
    case NUNTIL:
      new->nbinary.ch2 = copynode(n->nbinary.ch2);
      new->nbinary.ch1 = copynode(n->nbinary.ch1);
      break;
    case NIF:
      new->nif.elsepart = copynode(n->nif.elsepart);
      new->nif.ifpart = copynode(n->nif.ifpart);
      new->nif.test = copynode(n->nif.test);
      break;
    case NFOR:
      new->nfor.var_ = nodesavestr(n->nfor.var_);
      new->nfor.body = copynode(n->nfor.body);
      new->nfor.args = copynode(n->nfor.args);
      new->nfor.linno = n->nfor.linno;
      break;
    case NCASE:
      new->ncase.cases = copynode(n->ncase.cases);
      new->ncase.expr = copynode(n->ncase.expr);
      new->ncase.linno = n->ncase.linno;
      break;
    case NCLIST:
      new->nclist.body = copynode(n->nclist.body);
      new->nclist.pattern = copynode(n->nclist.pattern);
      new->nclist.next = copynode(n->nclist.next);
      break;
    case NDEFUN:
      new->ndefun.body = copynode(n->ndefun.body);
      new->ndefun.text = nodesavestr(n->ndefun.text);
      new->ndefun.linno = n->ndefun.linno;
      break;
    case NARG:
      new->narg.backquote = copynodelist(n->narg.backquote);
      new->narg.text = nodesavestr(n->narg.text);
      new->narg.next = copynode(n->narg.next);
      break;
    case NTO:
    case NCLOBBER:
    case NFROM:
    case NFROMTO:
    case NAPPEND:
      new->nfile.fname = copynode(n->nfile.fname);
      new->nfile.fd = n->nfile.fd;
      new->nfile.next = copynode(n->nfile.next);
      break;
    case NTOFD:
    case NFROMFD:
      new->ndup.vname = copynode(n->ndup.vname);
      new->ndup.dupfd = n->ndup.dupfd;
      new->ndup.fd = n->ndup.fd;
      new->ndup.next = copynode(n->ndup.next);
      break;
    case NHERE:
    case NXHERE:
      new->nhere.doc = copynode(n->nhere.doc);
      new->nhere.fd = n->nhere.fd;
      new->nhere.next = copynode(n->nhere.next);
      break;
    case NNOT:
      new->nnot.com = copynode(n->nnot.com);
      break;
  };
  new->type = n->type;
  return new;
}

static struct nodelist *copynodelist(struct nodelist *lp) {
  struct nodelist *start;
  struct nodelist **lpp;
  lpp = &start;
  while (lp) {
    *lpp = funcblock;
    funcblock = (char *)funcblock + SHELL_ALIGN(sizeof(struct nodelist));
    (*lpp)->n = copynode(lp->n);
    lp = lp->next;
    lpp = &(*lpp)->next;
  }
  *lpp = NULL;
  return start;
}

/*
 * Read and execute commands.  "Top" is nonzero for the top level command
 * loop; it turns on prompting if the shell is interactive.
 */
static int cmdloop(int top) {
  union node *n;
  struct stackmark smark;
  int inter;
  int status = 0;
  int numeof = 0;
  TRACE(("cmdloop(%d) called\n", top));
  for (;;) {
    int skip;
    setstackmark(&smark);
    if (jobctl) showjobs(out2, SHOW_CHANGED);
    inter = 0;
    if (iflag && top) {
      inter++;
      /* chkmail(); */
    }
    n = parsecmd(inter);
    /* showtree(n); DEBUG */
    if (n == NEOF) {
      if (!top || numeof >= 50) break;
      if (!stoppedjobs()) {
        if (!Iflag) {
          if (iflag) outcslow('\n', out2);
          break;
        }
        outstr("\nUse \"exit\" to leave shell.\n", out2);
      }
      numeof++;
    } else {
      int i;
      job_warning = (job_warning == 2) ? 1 : 0;
      numeof = 0;
      i = evaltree(n, 0);
      if (n) status = i;
    }
    popstackmark(&smark);
    skip = evalskip;
    if (skip) {
      evalskip &= ~(SKIPFUNC | SKIPFUNCDEF);
      break;
    }
  }
  return status;
}

/*
 * Read /etc/profile or .profile.  Return on error.
 */
static void read_profile(const char *name) {
  name = expandstr(name);
  if (setinputfile(name, INPUT_PUSH_FILE | INPUT_NOFILE_OK) < 0) return;
  cmdloop(0);
  popfile();
}

/*
 * Take commands from a file.  To be compatible we should do a path
 * search for the file, which is necessary to find sub-commands.
 */
static char *find_dot_file(char *basename) {
  char *fullname;
  const char *path = pathval();
  struct stat statb;
  int len;
  /* don't try this for absolute or relative paths */
  if (strchr(basename, '/')) return basename;
  while ((len = padvance(&path, basename)) >= 0) {
    fullname = stackblock();
    if ((!pathopt || *pathopt == 'f') && !stat(fullname, &statb) &&
        S_ISREG(statb.st_mode)) {
      /* This will be freed by the caller. */
      return stalloc(len);
    }
  }
  /* not found in the PATH */
  sh_error("%s: not found", basename);
}

static int dotcmd(int argc, char **argv) {
  int status = 0;
  nextopt(nullstr);
  argv = argptr;
  if (*argv) {
    char *fullname;
    fullname = find_dot_file(*argv);
    setinputfile(fullname, INPUT_PUSH_FILE);
    commandname = fullname;
    status = cmdloop(0);
    popfile();
  }
  return status;
}

static int exitcmd(int argc, char **argv) {
  if (stoppedjobs()) return 0;
  if (argc > 1) savestatus = number(argv[1]);
  exraise(EXEXIT);
}

/**
 * Main routine.  We initialize things, parse the arguments, execute
 * profiles if we're a login shell, and then call cmdloop to execute
 * commands.  The setjmp call sets up the location to jump to when an
 * exception occurs.  When an exception occurs the variable "state"
 * is used to figure out how far we had gotten.
 */
int main(int argc, char **argv) {
  char *shinit;
  volatile int state;
  struct stackmark smark;
  int login;
  state = 0;
  if (unlikely(setjmp(main_handler.loc))) {
    int e;
    int s;
    exitreset();
    e = exception;
    s = state;
    if (e == EXEND || e == EXEXIT || s == 0 || iflag == 0 || shlvl) exitshell();
    reset();
    if (e == EXINT) {
      outcslow('\n', out2);
    }
    popstackmark(&smark);
    FORCEINTON; /* enable interrupts */
    if (s == 1) {
      goto state1;
    } else if (s == 2) {
      goto state2;
    } else if (s == 3) {
      goto state3;
    } else {
      goto state4;
    }
  }
  handler = &main_handler;
  rootpid = getpid();
  init();
  setstackmark(&smark);
  login = procargs(argc, argv);
  if (login) {
    state = 1;
    read_profile("/etc/profile");
  state1:
    state = 2;
    read_profile("$HOME/.profile");
  }
state2:
  state = 3;
  if (iflag) {
    if ((shinit = lookupvar("ENV")) != NULL && *shinit != '\0') {
      read_profile(shinit);
    }
  }
  popstackmark(&smark);
state3:
  state = 4;
  if (minusc) evalstring(minusc, sflag ? 0 : EV_EXIT);
  if (sflag || minusc == NULL) {
  state4: /* XXX ??? - why isn't this before the "if" statement */
    cmdloop(1);
  }
  exitshell();
}
