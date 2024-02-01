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
#include "libc/calls/createfileflags.internal.h"
#include "libc/calls/internal.h"
#include "libc/calls/sig.internal.h"
#include "libc/calls/state.internal.h"
#include "libc/calls/struct/fd.internal.h"
#include "libc/calls/struct/iovec.h"
#include "libc/calls/struct/sigset.internal.h"
#include "libc/calls/syscall_support-nt.internal.h"
#include "libc/cosmo.h"
#include "libc/errno.h"
#include "libc/fmt/itoa.h"
#include "libc/intrin/describeflags.internal.h"
#include "libc/intrin/dll.h"
#include "libc/intrin/nomultics.internal.h"
#include "libc/intrin/strace.internal.h"
#include "libc/intrin/weaken.h"
#include "libc/macros.internal.h"
#include "libc/nt/console.h"
#include "libc/nt/createfile.h"
#include "libc/nt/enum/accessmask.h"
#include "libc/nt/enum/consolemodeflags.h"
#include "libc/nt/enum/creationdisposition.h"
#include "libc/nt/enum/filesharemode.h"
#include "libc/nt/enum/vk.h"
#include "libc/nt/enum/wait.h"
#include "libc/nt/errors.h"
#include "libc/nt/runtime.h"
#include "libc/nt/struct/inputrecord.h"
#include "libc/nt/synchronization.h"
#include "libc/str/str.h"
#include "libc/str/utf16.h"
#include "libc/sysv/consts/limits.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/sicode.h"
#include "libc/sysv/consts/sig.h"
#include "libc/sysv/errfuns.h"
#include "libc/thread/posixthread.internal.h"
#include "libc/thread/thread.h"
#include "libc/thread/tls.h"
#ifdef __x86_64__

/**
 * @fileoverview Cosmopolitan Standard Input
 *
 * This file implements pollable terminal i/o for Windows consoles. On
 * Windows 10 the "virtual terminal processing" feature works great on
 * output but their solution for input processing isn't good enough to
 * support running Linux programs like Emacs. This polyfill fixes that
 * and it most importantly ensures we can poll() standard input, which
 * would otherwise have been impossible. We aren't using threads. What
 * we do instead is have termios behaviors e.g. canonical mode editing
 * happen on demand as a side effect of read/poll/ioctl activity.
 */

struct VirtualKey {
  int vk;
  int normal_str;
  int shift_str;
  int ctrl_str;
  int shift_ctrl_str;
};

#define S(s) W(s "\0\0")
#define W(s) (s[3] << 24 | s[2] << 16 | s[1] << 8 | s[0])

static const struct VirtualKey kVirtualKey[] = {
    {kNtVkUp, S("A"), S("1;2A"), S("1;5A"), S("1;6A")},
    {kNtVkDown, S("B"), S("1;2B"), S("1;5B"), S("1;6B")},
    {kNtVkRight, S("C"), S("1;2C"), S("1;5C"), S("1;6C")},
    {kNtVkLeft, S("D"), S("1;2D"), S("1;5D"), S("1;6D")},
    {kNtVkInsert, S("2~"), S("2;2~"), S("2;5~"), S("2;6~")},
    {kNtVkDelete, S("3~"), S("3;2~"), S("3;5~"), S("3;6~")},
    {kNtVkHome, S("H"), S("1;2H"), S("1;5H"), S("1;6H")},
    {kNtVkEnd, S("F"), S("1;2F"), S("1;5F"), S("1;6F")},
    {kNtVkPrior, S("5~"), S("5;2~"), S("5;5~"), S("5;6~")},
    {kNtVkNext, S("6~"), S("6;2~"), S("6;5~"), S("6;6~")},
    {kNtVkF1, -S("OP"), S("1;2P"), S("11^"), S("1;6P")},
    {kNtVkF2, -S("OQ"), S("1;2Q"), S("12^"), S("1;6Q")},
    {kNtVkF3, -S("OR"), S("1;2R"), S("13^"), S("1;6R")},
    {kNtVkF4, -S("OS"), S("1;2S"), S("14^"), S("1;6S")},
    {kNtVkF5, S("15~"), S("28~"), S("15^"), S("28^")},
    {kNtVkF6, S("17~"), S("29~"), S("17^"), S("29^")},
    {kNtVkF7, S("18~"), S("31~"), S("18^"), S("31^")},
    {kNtVkF8, S("19~"), S("32~"), S("19^"), S("32^")},
    {kNtVkF9, S("20~"), S("33~"), S("20^"), S("33^")},
    {kNtVkF10, S("21~"), S("34~"), S("21^"), S("34^")},
    {kNtVkF11, S("23~"), S("23$"), S("23^"), S("23@")},
    {kNtVkF12, S("24~"), S("24$"), S("24^"), S("24@")},
    {0},
};

// TODO: How can we configure `less` to not need this bloat?
static const struct VirtualKey kDecckm[] = {
    {kNtVkUp, -S("OA"), -S("OA"), S("A"), S("A")},
    {kNtVkDown, -S("OB"), -S("OB"), S("B"), S("B")},
    {kNtVkRight, -S("OC"), -S("OC"), S("C"), S("C")},
    {kNtVkLeft, -S("OD"), -S("OD"), S("D"), S("D")},
    {kNtVkPrior, S("5~"), S("5;2~"), S("5;5~"), S("5;6~")},
    {kNtVkNext, S("6~"), S("6;2~"), S("6;5~"), S("6;6~")},
    {0},
};

#define KEYSTROKE_CONTAINER(e) DLL_CONTAINER(struct Keystroke, elem, e)

struct Keystroke {
  char buf[23];
  unsigned char buflen;
  struct Dll elem;
};

struct Keystrokes {
  atomic_uint once;
  bool end_of_file;
  bool ohno_decckm;
  uint16_t utf16hs;
  int16_t freekeys;
  int64_t cin, cot;
  struct Dll *list;
  struct Dll *line;
  struct Dll *free;
  pthread_mutex_t lock;
  const struct VirtualKey *vkt;
  struct Keystroke pool[512];
};

static struct Keystrokes __keystroke;

textwindows void WipeKeystrokes(void) {
  bzero(&__keystroke, sizeof(__keystroke));
}

static textwindows void FreeKeystrokeImpl(struct Dll *key) {
  dll_make_first(&__keystroke.free, key);
  ++__keystroke.freekeys;
}

static textwindows struct Keystroke *NewKeystroke(void) {
  struct Dll *e = dll_first(__keystroke.free);
  struct Keystroke *k = KEYSTROKE_CONTAINER(e);
  dll_remove(&__keystroke.free, &k->elem);
  --__keystroke.freekeys;
  // TODO(jart): What's wrong with GCC 12.3?
  asm("" : "+r"(k));
  k->buflen = 0;
  return k;
}

static textwindows void FreeKeystroke(struct Dll **list, struct Dll *key) {
  dll_remove(list, key);
  FreeKeystrokeImpl(key);
}

static textwindows void FreeKeystrokes(struct Dll **list) {
  struct Dll *key;
  while ((key = dll_first(*list))) {
    FreeKeystroke(list, key);
  }
}

static textwindows void OpenConsole(void) {
  __keystroke.vkt = kVirtualKey;
  __keystroke.cin = CreateFile(u"CONIN$", kNtGenericRead | kNtGenericWrite,
                               kNtFileShareRead, 0, kNtOpenExisting, 0, 0);
  __keystroke.cot = CreateFile(u"CONOUT$", kNtGenericRead | kNtGenericWrite,
                               kNtFileShareWrite, 0, kNtOpenExisting, 0, 0);
  for (int i = 0; i < ARRAYLEN(__keystroke.pool); ++i) {
    dll_init(&__keystroke.pool[i].elem);
    FreeKeystrokeImpl(&__keystroke.pool[i].elem);
  }
}

static textwindows int AddSignal(int sig) {
  atomic_fetch_or_explicit(&__get_tls()->tib_sigpending, 1ull << (sig - 1),
                           memory_order_relaxed);
  return 0;
}

static textwindows void InitConsole(void) {
  cosmo_once(&__keystroke.once, OpenConsole);
}

static textwindows void LockKeystrokes(void) {
  pthread_mutex_lock(&__keystroke.lock);
}

static textwindows void UnlockKeystrokes(void) {
  pthread_mutex_unlock(&__keystroke.lock);
}

textwindows int64_t GetConsoleInputHandle(void) {
  InitConsole();
  return __keystroke.cin;
}

textwindows int64_t GetConsoleOutputHandle(void) {
  InitConsole();
  return __keystroke.cot;
}

static textwindows bool IsMouseModeCommand(int x) {
  return x == 1000 ||  // SET_VT200_MOUSE
         x == 1002 ||  // SET_BTN_EVENT_MOUSE
         x == 1006 ||  // SET_SGR_EXT_MODE_MOUSE
         x == 1015;    // SET_URXVT_EXT_MODE_MOUSE
}

static textwindows int GetVirtualKey(uint16_t vk, bool shift, bool ctrl) {
  for (int i = 0; __keystroke.vkt[i].vk; ++i) {
    if (__keystroke.vkt[i].vk == vk) {
      if (shift && ctrl) {
        return __keystroke.vkt[i].shift_ctrl_str;
      } else if (shift) {
        return __keystroke.vkt[i].shift_str;
      } else if (ctrl) {
        return __keystroke.vkt[i].ctrl_str;
      } else {
        return __keystroke.vkt[i].normal_str;
      }
    }
  }
  return 0;
}

static textwindows int ProcessKeyEvent(const struct NtInputRecord *r, char *p) {

  uint32_t c = r->Event.KeyEvent.uChar.UnicodeChar;
  uint16_t vk = r->Event.KeyEvent.wVirtualKeyCode;
  uint16_t cks = r->Event.KeyEvent.dwControlKeyState;

  // ignore keyup events
  if (!r->Event.KeyEvent.bKeyDown && (!c || vk != kNtVkMenu)) {
    return 0;
  }

#if 0
  // this code is useful for troubleshooting why keys don't work
  kprintf("bKeyDown=%hhhd wVirtualKeyCode=%s wVirtualScanCode=%s "
          "UnicodeChar=%#x[%#lc] dwControlKeyState=%s\n",
          r->Event.KeyEvent.bKeyDown,
          DescribeVirtualKeyCode(r->Event.KeyEvent.wVirtualKeyCode),
          DescribeVirtualKeyCode(r->Event.KeyEvent.wVirtualScanCode),
          r->Event.KeyEvent.uChar.UnicodeChar,
          r->Event.KeyEvent.uChar.UnicodeChar,
          DescribeControlKeyState(r->Event.KeyEvent.dwControlKeyState));
#endif

  // turn arrow/function keys into vt100/ansi/xterm byte sequences
  int n = 0;
  int v = GetVirtualKey(vk, !!(cks & kNtShiftPressed),
                        !!(cks & (kNtLeftCtrlPressed | kNtRightCtrlPressed)));
  if (v) {
    p[n++] = 033;
    if (cks & (kNtLeftAltPressed | kNtRightAltPressed)) {
      p[n++] = 033;
    }
    if (v > 0) {
      p[n++] = '[';
    } else {
      v = -v;
    }
    do p[n++] = v;
    while ((v >>= 8));
    return n;
  }

  // ^/ (crtl+slash) maps to ^_ (ctrl-hyphen) on linux
  if (vk == kNtVkOem_2 && (cks & (kNtLeftCtrlPressed | kNtRightCtrlPressed))) {
    p[n++] = 037;
    return n;
  }

  // handle cases where win32 doesn't provide character
  if (!c) {
    if (vk == '2' && (cks & (kNtLeftCtrlPressed | kNtRightCtrlPressed))) {
      c = 0;  // ctrl-2 → "\000"
    } else if (isascii(vk) && isdigit(vk) &&
               (cks & (kNtLeftCtrlPressed | kNtRightCtrlPressed))) {
      c = 030 + (vk - '0');  // e.g. ctrl-3 → "\033"
    } else if (isascii(vk) && isgraph(vk) &&
               (cks & (kNtLeftCtrlPressed | kNtRightCtrlPressed))) {
      c = vk ^ 0100;  // e.g. ctrl-alt-b → "\033\002"
    } else {
      return 0;
    }
  }

  // convert utf-16 to utf-32
  if (IsHighSurrogate(c)) {
    __keystroke.utf16hs = c;
    return 0;
  }
  if (IsLowSurrogate(c)) {
    c = MergeUtf16(__keystroke.utf16hs, c);
  }

  // enter sends \r with raw terminals
  // make it a multics newline instead
  if (c == '\r' && !(__ttyconf.magic & kTtyNoCr2Nl)) {
    c = '\n';
  }

  // ctrl-space (^@) is literally zero
  if (c == ' ' && (cks & (kNtLeftCtrlPressed | kNtRightCtrlPressed))) {
    c = '\0';
  }

  // make backspace (^?) distinguishable from ctrl-h (^H)
  if (c == kNtVkBack && !(cks & (kNtLeftCtrlPressed | kNtRightCtrlPressed))) {
    c = 0177;
  }

  // handle ctrl-\ and ctrl-c
  // note we define _POSIX_VDISABLE as zero
  // tcsetattr() lets anyone reconfigure these keybindings
  if (c && !(__ttyconf.magic & kTtyNoIsigs)) {
    if (c == __ttyconf.vintr) {
      return AddSignal(SIGINT);
    } else if (c == __ttyconf.vquit) {
      return AddSignal(SIGQUIT);
    }
  }

  // handle ctrl-d which generates end-of-file, unless pending line data
  // is present, in which case we flush that without the newline instead
  if (c && c == __ttyconf.veof && !(__ttyconf.magic & kTtyUncanon)) {
    if (dll_is_empty(__keystroke.line)) {
      __keystroke.end_of_file = true;
    } else {
      dll_make_last(&__keystroke.list, __keystroke.line);
      __keystroke.line = 0;
    }
    return 0;
  }

  // insert esc prefix when alt is held
  // for example "h" becomes "\033h" (alt-h)
  // if up arrow is "\033[A" then alt-up is "\033\033[A"
  if ((cks & (kNtLeftAltPressed | kNtRightAltPressed)) &&
      r->Event.KeyEvent.bKeyDown) {
    p[n++] = 033;
  }

  // finally apply thompson-pike varint encoding
  uint64_t w = tpenc(c);
  do p[n++] = w;
  while ((w >>= 8));
  return n;
}

// To use the tty mouse events feature:
//   - write(1, "\e[?1000;1002;1015;1006h") to enable
//   - write(1, "\e[?1000;1002;1015;1006l") to disable
// See o//examples/ttyinfo.com and o//tool/viz/life.com
static textwindows int ProcessMouseEvent(const struct NtInputRecord *r,
                                         char *b) {
  char *p = b;
  unsigned char e = 0;
  uint32_t currentbs = __ttyconf.mousebs;
  uint32_t ev = r->Event.MouseEvent.dwEventFlags;
  uint32_t bs = r->Event.MouseEvent.dwButtonState;
  ev &= kNtMouseMoved | kNtMouseWheeled;
  bs &= kNtFromLeft1stButtonPressed | kNtRightmostButtonPressed;
  if (ev & kNtMouseWheeled) {
    // scroll wheel (unnatural mode)
    bool isup = ((int)r->Event.MouseEvent.dwButtonState >> 16) > 0;
    if (__ttyconf.magic & kTtyXtMouse) {
      if (r->Event.MouseEvent.dwControlKeyState &
          (kNtLeftCtrlPressed | kNtRightCtrlPressed)) {
        e = isup ? 80 : 81;
      } else {
        e = isup ? 64 : 65;
      }
      goto OutputXtermMouseEvent;
    } else if (!(r->Event.MouseEvent.dwControlKeyState &
                 (kNtShiftPressed | kNtLeftCtrlPressed | kNtRightCtrlPressed |
                  kNtLeftAltPressed | kNtRightAltPressed))) {
      // we disable mouse highlighting when the tty is put in raw mode
      // to mouse wheel events with widely understood vt100 arrow keys
      *p++ = 033;
      *p++ = !__keystroke.ohno_decckm ? '[' : 'O';
      if (isup) {
        *p++ = 'A';
      } else {
        *p++ = 'B';
      }
    }
  } else if ((bs || currentbs) && (__ttyconf.magic & kTtyXtMouse)) {
    if (bs && (ev & kNtMouseMoved) && currentbs) {
      e |= 32;  // dragging
    }
    if ((bs | currentbs) & kNtRightmostButtonPressed) {
      e |= 2;  // right
    }
  OutputXtermMouseEvent:
    *p++ = 033;
    *p++ = '[';
    *p++ = '<';
    p = FormatInt32(p, e);
    *p++ = ';';
    p = FormatInt32(p, (r->Event.MouseEvent.dwMousePosition.X + 1) & 0x7fff);
    *p++ = ';';
    p = FormatInt32(p, (r->Event.MouseEvent.dwMousePosition.Y + 1) & 0x7fff);
    if (!bs && currentbs) {
      *p++ = 'm';  // up
    } else {
      *p++ = 'M';  // down
    }
    __ttyconf.mousebs = bs;
  }
  return p - b;
}

static textwindows int ConvertConsoleInputToAnsi(const struct NtInputRecord *r,
                                                 char p[hasatleast 23]) {
  switch (r->EventType) {
    case kNtKeyEvent:
      return ProcessKeyEvent(r, p);
    case kNtMouseEvent:
      return ProcessMouseEvent(r, p);
    case kNtWindowBufferSizeEvent:
      return AddSignal(SIGWINCH);
    default:
      return 0;
  }
}

static textwindows void WriteTty(const char *p, size_t n) {
  WriteFile(__keystroke.cot, p, n, 0, 0);
}

static textwindows bool IsCtl(int c) {
  return isascii(c) && iscntrl(c) && c != '\n' && c != '\t';
}

static textwindows void WriteCtl(const char *p, size_t n) {
  size_t i;
  for (i = 0; i < n; ++i) {
    if (IsCtl(p[i])) {
      char ctl[2];
      ctl[0] = '^';
      ctl[1] = p[i] ^ 0100;
      WriteTty(ctl, 2);
    } else {
      WriteTty(p + i, 1);
    }
  }
}

static textwindows void EchoTty(const char *p, size_t n) {
  if (__ttyconf.magic & kTtyEchoRaw) {
    WriteTty(p, n);
  } else {
    WriteCtl(p, n);
  }
}

static textwindows void EraseCharacter(void) {
  WriteTty("\b \b", 3);
}

static textwindows bool EraseKeystroke(void) {
  struct Dll *e;
  if ((e = dll_last(__keystroke.line))) {
    struct Keystroke *k = KEYSTROKE_CONTAINER(e);
    FreeKeystroke(&__keystroke.line, e);
    for (int i = k->buflen; i--;) {
      if ((k->buf[i] & 0300) == 0200) continue;  // utf-8 cont
      EraseCharacter();
      if (!(__ttyconf.magic & kTtyEchoRaw) && IsCtl(k->buf[i])) {
        EraseCharacter();
      }
    }
    return true;
  } else {
    return false;
  }
}

static textwindows void IngestConsoleInputRecord(struct NtInputRecord *r) {

  // convert win32 console event into ansi
  int len;
  char buf[23];
  if (!(len = ConvertConsoleInputToAnsi(r, buf))) {
    return;
  }

  // handle backspace in canonical mode
  if (len == 1 && buf[0] &&                  //
      (buf[0] & 255) == __ttyconf.verase &&  //
      !(__ttyconf.magic & kTtyUncanon)) {
    EraseKeystroke();
    return;
  }

  // handle kill in canonical mode
  if (len == 1 && buf[0] &&                 //
      (buf[0] & 255) == __ttyconf.vkill &&  //
      !(__ttyconf.magic & kTtyUncanon)) {
    while (EraseKeystroke()) {
    }
    return;
  }

  // allocate object to hold keystroke
  struct Keystroke *k = NewKeystroke();
  memcpy(k->buf, buf, sizeof(k->buf));
  k->buflen = len;

  // echo input if it was successfully recorded
  // assuming the win32 console isn't doing it already
  if (!(__ttyconf.magic & kTtySilence)) {
    EchoTty(buf, len);
  }

  // save keystroke to appropriate list
  if (__ttyconf.magic & kTtyUncanon) {
    dll_make_last(&__keystroke.list, &k->elem);
  } else {
    dll_make_last(&__keystroke.line, &k->elem);

    // flush canonical mode line if oom or enter
    if (!__keystroke.freekeys || (len == 1 && buf[0] &&
                                  ((buf[0] & 255) == '\n' ||            //
                                   (buf[0] & 255) == __ttyconf.veol ||  //
                                   (buf[0] & 255) == __ttyconf.veol2))) {
      dll_make_last(&__keystroke.list, __keystroke.line);
      __keystroke.line = 0;
    }
  }
}

static textwindows void IngestConsoleInput(void) {
  uint32_t i, n;
  struct NtInputRecord records[16];
  for (;;) {
    if (!__keystroke.freekeys) return;
    if (__keystroke.end_of_file) return;
    if (!GetNumberOfConsoleInputEvents(__keystroke.cin, &n)) {
      goto UnexpectedEof;
    }
    if (!n) return;
    n = MIN(__keystroke.freekeys, MIN(ARRAYLEN(records), n));
    if (!ReadConsoleInput(__keystroke.cin, records, n, &n)) {
      goto UnexpectedEof;
    }
    for (i = 0; i < n && !__keystroke.end_of_file; ++i) {
      IngestConsoleInputRecord(records + i);
    }
  }
UnexpectedEof:
  STRACE("console read error %d", GetLastError());
  __keystroke.end_of_file = true;
}

// Discards all unread stdin bytes.
textwindows int FlushConsoleInputBytes(void) {
  BLOCK_SIGNALS;
  InitConsole();
  LockKeystrokes();
  FlushConsoleInputBuffer(__keystroke.cin);
  FreeKeystrokes(&__keystroke.list);
  FreeKeystrokes(&__keystroke.line);
  UnlockKeystrokes();
  ALLOW_SIGNALS;
  return 0;
}

// Returns number of stdin bytes that may be read without blocking.
textwindows int CountConsoleInputBytes(void) {
  struct Dll *e;
  int count = 0;
  BLOCK_SIGNALS;
  InitConsole();
  LockKeystrokes();
  IngestConsoleInput();
  for (e = dll_first(__keystroke.list); e; e = dll_next(__keystroke.list, e)) {
    count += KEYSTROKE_CONTAINER(e)->buflen;
  }
  if (!count && __keystroke.end_of_file) {
    count = -1;
  }
  UnlockKeystrokes();
  ALLOW_SIGNALS;
  return count;
}

// Intercept ANSI TTY commands that enable features.
textwindows void InterceptTerminalCommands(const char *data, size_t size) {
  int i;
  unsigned x;
  bool ismouse;
  uint32_t cm, cm2;
  enum { ASC, ESC, CSI, CMD } t;
  GetConsoleMode(GetConsoleInputHandle(), &cm), cm2 = cm;
  for (ismouse = false, x = i = t = 0; i < size; ++i) {
    switch (t) {
      case ASC:
        if (data[i] == 033) {
          t = ESC;
        }
        break;
      case ESC:
        if (data[i] == '[') {
          t = CSI;
        } else {
          t = ASC;
        }
        break;
      case CSI:
        if (data[i] == '?') {
          t = CMD;
          x = 0;
        } else {
          t = ASC;
        }
        break;
      case CMD:
        if ('0' <= data[i] && data[i] <= '9') {
          x *= 10;
          x += data[i] - '0';
        } else if (data[i] == ';') {
          ismouse |= IsMouseModeCommand(x);
          x = 0;
        } else if (data[i] == 'h') {
          if (x == 1) {
            __keystroke.vkt = kDecckm;  // \e[?1h decckm on
            __keystroke.ohno_decckm = true;
          } else if ((ismouse |= IsMouseModeCommand(x))) {
            __ttyconf.magic |= kTtyXtMouse;
            cm2 |= kNtEnableMouseInput;
            cm2 &= ~kNtEnableQuickEditMode;  // take mouse
          }
          t = ASC;
        } else if (data[i] == 'l') {
          if (x == 1) {
            __keystroke.vkt = kVirtualKey;  // \e[?1l decckm off
            __keystroke.ohno_decckm = false;
          } else if ((ismouse |= IsMouseModeCommand(x))) {
            __ttyconf.magic &= ~kTtyXtMouse;
            cm2 |= kNtEnableQuickEditMode;  // release mouse
          }
          t = ASC;
        }
        break;
      default:
        __builtin_unreachable();
    }
  }
  if (cm2 != cm) {
    SetConsoleMode(GetConsoleInputHandle(), cm2);
  }
}

static textwindows bool DigestConsoleInput(char *data, size_t size, int *rc) {

  // handle eof once available input is consumed
  if (dll_is_empty(__keystroke.list) && __keystroke.end_of_file) {
    *rc = 0;
    return true;
  }

  // copy keystroke(s) into user buffer
  int toto = 0;
  struct Dll *e;
  while (size && (e = dll_first(__keystroke.list))) {
    struct Keystroke *k = KEYSTROKE_CONTAINER(e);
    uint32_t got = MIN(size, k->buflen);
    uint32_t remain = k->buflen - got;
    if (got) {
      memcpy(data, k->buf, got);
      data += got;
      size -= got;
      toto += got;
    }
    if (remain) {
      memmove(k->buf, k->buf + got, remain);
      k->buflen = remain;
    } else {
      FreeKeystroke(&__keystroke.list, e);
    }
    if ((__ttyconf.magic & kTtyUncanon) && toto >= __ttyconf.vmin) {
      break;
    }
  }

  // return result
  if (toto) {
    *rc = toto;
    return true;
  } else {
    return false;
  }
}

static textwindows int WaitForConsole(struct Fd *f, sigset_t waitmask) {
  int sig;
  int64_t sem;
  uint32_t wi, ms = -1;
  if (!__ttyconf.vmin) {
    if (!__ttyconf.vtime) {
      return 0;  // non-blocking w/o raising eagain
    } else {
      ms = __ttyconf.vtime * 100;
    }
  }
  if (_check_cancel() == -1) return -1;
  if (f->flags & _O_NONBLOCK) return eagain();
  if (_weaken(__sig_get) && (sig = _weaken(__sig_get)(waitmask))) {
    goto DeliverSignal;
  }
  struct PosixThread *pt = _pthread_self();
  pt->pt_blkmask = waitmask;
  pt->pt_semaphore = sem = CreateSemaphore(0, 0, 1, 0);
  atomic_store_explicit(&pt->pt_blocker, PT_BLOCKER_SEM, memory_order_release);
  wi = WaitForMultipleObjects(2, (int64_t[2]){__keystroke.cin, sem}, 0, ms);
  atomic_store_explicit(&pt->pt_blocker, 0, memory_order_release);
  CloseHandle(sem);
  if (wi == kNtWaitTimeout) return 0;  // vtime elapsed
  if (wi == 0) return -2;              // console data
  if (wi != 1) return __winerr();      // wait failed
  if (_weaken(__sig_get)) {
    if (!(sig = _weaken(__sig_get)(waitmask))) return eintr();
  DeliverSignal:
    int handler_was_called = _weaken(__sig_relay)(sig, SI_KERNEL, waitmask);
    if (_check_cancel() == -1) return -1;
    if (!(handler_was_called & SIG_HANDLED_NO_RESTART)) return -2;
  }
  return eintr();
}

static textwindows ssize_t ReadFromConsole(struct Fd *f, void *data,
                                           size_t size, sigset_t waitmask) {
  int rc;
  InitConsole();
  do {
    LockKeystrokes();
    IngestConsoleInput();
    bool done = DigestConsoleInput(data, size, &rc);
    UnlockKeystrokes();
    if (done) return rc;
  } while ((rc = WaitForConsole(f, waitmask)) == -2);
  return rc;
}

textwindows ssize_t ReadBuffer(int fd, void *data, size_t size, int64_t offset,
                               sigset_t waitmask) {

  // switch to terminal polyfill if reading from win32 console
  struct Fd *f = g_fds.p + fd;

  if (f->kind == kFdDevNull) {
    return 0;
  }

  if (f->kind == kFdConsole) {
    return ReadFromConsole(f, data, size, waitmask);
  }

  // perform heavy lifting
  ssize_t rc;
  rc = sys_readwrite_nt(fd, data, size, offset, f->handle, waitmask, ReadFile);
  if (rc != -2) return rc;

  // mops up win32 errors
  switch (GetLastError()) {
    case kNtErrorBrokenPipe:    // broken pipe
    case kNtErrorNoData:        // closing named pipe
    case kNtErrorHandleEof:     // pread read past EOF
      return 0;                 //
    case kNtErrorAccessDenied:  // read doesn't return EACCESS
      return ebadf();           //
    default:
      return __winerr();
  }
}

static textwindows ssize_t ReadIovecs(int fd, const struct iovec *iov,
                                      size_t iovlen, int64_t opt_offset,
                                      sigset_t waitmask) {
  ssize_t rc;
  size_t i, total;
  if (opt_offset < -1) return einval();
  while (iovlen && !iov[0].iov_len) iov++, iovlen--;
  if (iovlen) {
    for (total = i = 0; i < iovlen; ++i) {
      if (!iov[i].iov_len) continue;
      rc =
          ReadBuffer(fd, iov[i].iov_base, iov[i].iov_len, opt_offset, waitmask);
      if (rc == -1) {
        if (total && errno != ECANCELED) {
          return total;
        } else {
          return -1;
        }
      }
      total += rc;
      if (opt_offset != -1) opt_offset += rc;
      if (rc < iov[i].iov_len) break;
    }
    return total;
  } else {
    return ReadBuffer(fd, NULL, 0, opt_offset, waitmask);
  }
}

textwindows ssize_t sys_read_nt(int fd, const struct iovec *iov, size_t iovlen,
                                int64_t opt_offset) {
  ssize_t rc;
  sigset_t m = __sig_block();
  rc = ReadIovecs(fd, iov, iovlen, opt_offset, m);
  __sig_unblock(m);
  return rc;
}

#endif /* __x86_64__ */
