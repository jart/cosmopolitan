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
#include "libc/assert.h"
#include "libc/calls/createfileflags.internal.h"
#include "libc/calls/internal.h"
#include "libc/calls/sig.internal.h"
#include "libc/calls/state.internal.h"
#include "libc/calls/struct/iovec.h"
#include "libc/calls/struct/sigset.internal.h"
#include "libc/calls/struct/timespec.h"
#include "libc/calls/struct/timespec.internal.h"
#include "libc/calls/syscall_support-nt.internal.h"
#include "libc/cosmo.h"
#include "libc/ctype.h"
#include "libc/errno.h"
#include "libc/fmt/itoa.h"
#include "libc/intrin/describeflags.h"
#include "libc/intrin/dll.h"
#include "libc/intrin/fds.h"
#include "libc/intrin/kprintf.h"
#include "libc/intrin/nomultics.h"
#include "libc/intrin/strace.h"
#include "libc/intrin/weaken.h"
#include "libc/macros.h"
#include "libc/nt/console.h"
#include "libc/nt/createfile.h"
#include "libc/nt/enum/accessmask.h"
#include "libc/nt/enum/consolemodeflags.h"
#include "libc/nt/enum/creationdisposition.h"
#include "libc/nt/enum/filesharemode.h"
#include "libc/nt/enum/vk.h"
#include "libc/nt/enum/wait.h"
#include "libc/nt/errors.h"
#include "libc/nt/events.h"
#include "libc/nt/memory.h"
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

static struct VirtualKey kVirtualKey[] = {
    {kNtVkUp, S("A"), S("1;2A"), S("1;5A"), S("1;6A")},     // order matters
    {kNtVkDown, S("B"), S("1;2B"), S("1;5B"), S("1;6B")},   // order matters
    {kNtVkRight, S("C"), S("1;2C"), S("1;5C"), S("1;6C")},  // order matters
    {kNtVkLeft, S("D"), S("1;2D"), S("1;5D"), S("1;6D")},   // order matters
    {kNtVkEnd, S("F"), S("1;2F"), S("1;5F"), S("1;6F")},    // order matters
    {kNtVkHome, S("H"), S("1;2H"), S("1;5H"), S("1;6H")},   // order matters
    {kNtVkInsert, S("2~"), S("2;2~"), S("2;5~"), S("2;6~")},
    {kNtVkDelete, S("3~"), S("3;2~"), S("3;5~"), S("3;6~")},
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
  bool bypass_mode;
  uint16_t utf16hs;
  size_t free_keys;
  int64_t cin, cot;
  struct Dll *list;
  struct Dll *line;
  struct Dll *free;
};

static struct Keystrokes __keystroke;
static pthread_mutex_t __keystroke_lock = PTHREAD_MUTEX_INITIALIZER;

textwindows void sys_read_nt_wipe_keystrokes(void) {
  bzero(&__keystroke, sizeof(__keystroke));
  _pthread_mutex_wipe_np(&__keystroke_lock);
}

textwindows static void FreeKeystrokeImpl(struct Dll *key) {
  dll_make_first(&__keystroke.free, key);
  ++__keystroke.free_keys;
}

textwindows static struct Keystroke *AllocKeystroke(void) {
  struct Keystroke *k;
  if (!(k = HeapAlloc(GetProcessHeap(), 0, sizeof(struct Keystroke))))
    return 0;
  dll_init(&k->elem);
  return k;
}

textwindows static struct Keystroke *NewKeystroke(void) {
  struct Dll *e;
  struct Keystroke *k;
  if ((e = dll_first(__keystroke.free))) {
    dll_remove(&__keystroke.free, e);
    k = KEYSTROKE_CONTAINER(e);
    --__keystroke.free_keys;
  } else {
    // PopulateKeystrokes() should make this branch impossible
    if (!(k = AllocKeystroke()))
      return 0;
  }
  k->buflen = 0;
  return k;
}

textwindows static void FreeKeystroke(struct Dll **list, struct Dll *key) {
  dll_remove(list, key);
  FreeKeystrokeImpl(key);
}

textwindows static void FreeKeystrokes(struct Dll **list) {
  struct Dll *key;
  while ((key = dll_first(*list)))
    FreeKeystroke(list, key);
}

textwindows static void PopulateKeystrokes(size_t want) {
  struct Keystroke *k;
  while (__keystroke.free_keys < want) {
    if ((k = AllocKeystroke())) {
      FreeKeystrokeImpl(&k->elem);
    } else {
      break;
    }
  }
}

textwindows static void OpenConsole(void) {
  __keystroke.cin = CreateFile(u"CONIN$", kNtGenericRead | kNtGenericWrite,
                               kNtFileShareRead, 0, kNtOpenExisting, 0, 0);
  __keystroke.cot = CreateFile(u"CONOUT$", kNtGenericRead | kNtGenericWrite,
                               kNtFileShareWrite, 0, kNtOpenExisting, 0, 0);
}

textwindows static int AddSignal(int sig) {
  atomic_fetch_or_explicit(&__get_tls()->tib_sigpending, 1ull << (sig - 1),
                           memory_order_relaxed);
  return 0;
}

textwindows static void InitConsole(void) {
  cosmo_once(&__keystroke.once, OpenConsole);
}

textwindows static void LockKeystrokes(void) {
  _pthread_mutex_lock(&__keystroke_lock);
}

textwindows static void UnlockKeystrokes(void) {
  _pthread_mutex_unlock(&__keystroke_lock);
}

textwindows int64_t GetConsoleInputHandle(void) {
  InitConsole();
  return __keystroke.cin;
}

textwindows int64_t GetConsoleOutputHandle(void) {
  InitConsole();
  return __keystroke.cot;
}

textwindows static bool IsMouseModeCommand(int x) {
  return x == 1000 ||  // SET_VT200_MOUSE
         x == 1002 ||  // SET_BTN_EVENT_MOUSE
         x == 1006 ||  // SET_SGR_EXT_MODE_MOUSE
         x == 1015;    // SET_URXVT_EXT_MODE_MOUSE
}

textwindows static int GetVirtualKey(uint16_t vk, bool shift, bool ctrl) {
  for (int i = 0; kVirtualKey[i].vk; ++i) {
    if (kVirtualKey[i].vk == vk) {
      if (shift && ctrl) {
        return kVirtualKey[i].shift_ctrl_str;
      } else if (shift) {
        return kVirtualKey[i].shift_str;
      } else if (ctrl) {
        return kVirtualKey[i].ctrl_str;
      } else {
        return kVirtualKey[i].normal_str;
      }
    }
  }
  return 0;
}

textwindows static int ProcessKeyEvent(const struct NtInputRecord *r, char *p) {

  uint32_t c = r->Event.KeyEvent.uChar.UnicodeChar;
  uint16_t vk = r->Event.KeyEvent.wVirtualKeyCode;
  uint16_t cks = r->Event.KeyEvent.dwControlKeyState;

  // ignore keyup events
  if (!r->Event.KeyEvent.bKeyDown && (!c || vk != kNtVkMenu))
    return 0;

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
    do
      p[n++] = v;
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
  if (IsLowSurrogate(c))
    c = MergeUtf16(__keystroke.utf16hs, c);

  // enter sends \r with raw terminals
  // make it a multics newline instead
  if (c == '\r' && !(__ttyconf.magic & kTtyNoCr2Nl))
    c = '\n';

  // ctrl-space (^@) is literally zero
  if (c == ' ' && (cks & (kNtLeftCtrlPressed | kNtRightCtrlPressed)))
    c = '\0';

  // make backspace (^?) distinguishable from ctrl-h (^H)
  if (c == kNtVkBack && !(cks & (kNtLeftCtrlPressed | kNtRightCtrlPressed)))
    c = 0177;

  // handle ctrl-\ and ctrl-c
  // note we define _POSIX_VDISABLE as zero
  // tcsetattr() lets anyone reconfigure these keybindings
  if (c && !(__ttyconf.magic & kTtyNoIsigs) && !__keystroke.bypass_mode) {
    char b[] = {c};
    if (c == __ttyconf.vintr) {
      EchoConsoleNt(b, 1, false);
      return AddSignal(SIGINT);
    } else if (c == __ttyconf.vquit) {
      EchoConsoleNt(b, 1, false);
      return AddSignal(SIGQUIT);
    }
  }

  // handle ctrl-d which generates end-of-file, unless pending line data
  // is present, in which case we flush that without the newline instead
  if (c && c == __ttyconf.veof &&  //
      !__keystroke.bypass_mode &&  //
      !(__ttyconf.magic & kTtyUncanon)) {
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
  do
    p[n++] = w;
  while ((w >>= 8));
  return n;
}

// To use the tty mouse events feature:
//   - write(1, "\e[?1000;1002;1015;1006h") to enable
//   - write(1, "\e[?1000;1002;1015;1006l") to disable
// See o//examples/ttyinfo and o//tool/viz/life
textwindows static int ProcessMouseEvent(const struct NtInputRecord *r,
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
      for (int i = 0; i < 3; ++i) {
        *p++ = 033;
        *p++ = !__keystroke.ohno_decckm ? '[' : 'O';
        if (isup) {
          *p++ = 'A';
        } else {
          *p++ = 'B';
        }
      }
    }
  } else if ((bs || currentbs) && (__ttyconf.magic & kTtyXtMouse)) {
    if (bs && (ev & kNtMouseMoved) && currentbs)
      e |= 32;  // dragging
    if ((bs | currentbs) & kNtRightmostButtonPressed)
      e |= 2;  // right
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

textwindows static int ConvertConsoleInputToAnsi(const struct NtInputRecord *r,
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

textwindows static void WriteTty(const char *p, size_t n) {
  WriteFile(__keystroke.cot, p, n, 0, 0);
}

textwindows static bool IsCtl(int c, bool escape_harder) {
  return isascii(c) && iscntrl(c) &&
         (escape_harder || (c != '\n' && c != '\t'));
}

textwindows static void WriteCtl(const char *p, size_t n, bool escape_harder) {
  size_t i;
  for (i = 0; i < n; ++i) {
    if (IsCtl(p[i], escape_harder)) {
      char ctl[2];
      ctl[0] = '^';
      ctl[1] = p[i] ^ 0100;
      WriteTty(ctl, 2);
    } else {
      WriteTty(p + i, 1);
    }
  }
}

textwindows void EchoConsoleNt(const char *p, size_t n, bool escape_harder) {
  InitConsole();
  if (!(__ttyconf.magic & kTtySilence)) {
    if (__ttyconf.magic & kTtyEchoRaw) {
      WriteTty(p, n);
    } else {
      WriteCtl(p, n, escape_harder);
    }
  }
}

textwindows static void EraseCharacter(bool should_echo) {
  if (should_echo)
    WriteTty("\b \b", 3);
}

textwindows static bool EraseKeystroke(bool should_echo) {
  struct Dll *e;
  if ((e = dll_last(__keystroke.line))) {
    struct Keystroke *k = KEYSTROKE_CONTAINER(e);
    FreeKeystroke(&__keystroke.line, e);
    for (int i = k->buflen; i--;) {
      if ((k->buf[i] & 0300) == 0200)
        continue;  // utf-8 cont
      EraseCharacter(should_echo);
      if (!(__ttyconf.magic & kTtyEchoRaw) && IsCtl(k->buf[i], true))
        EraseCharacter(should_echo);
    }
    return true;
  } else {
    return false;
  }
}

textwindows static int IsLookingAtSpace(void) {
  struct Dll *e;
  if ((e = dll_last(__keystroke.line))) {
    struct Keystroke *k = KEYSTROKE_CONTAINER(e);
    return k->buflen == 1 && isascii(k->buf[0]) && isspace(k->buf[0]);
  } else {
    return -1;
  }
}

textwindows static void IngestConsoleInputRecord(struct NtInputRecord *r) {

  // convert win32 console event into ansi
  int len;
  char buf[23];
  if (!(len = ConvertConsoleInputToAnsi(r, buf)))
    return;

  // handle ctrl-v in canonical mode
  // the next keystroke will bypass input processing
  if (!(__ttyconf.magic & kTtyUncanon) &&   // ICANON
      !(__ttyconf.magic & kTtyNoIexten)) {  // IEXTEN
    if (__keystroke.bypass_mode) {
      struct Keystroke *k = NewKeystroke();
      if (!k)
        return;
      memcpy(k->buf, buf, sizeof(k->buf));
      k->buflen = len;
      dll_make_last(&__keystroke.line, &k->elem);
      EchoConsoleNt(buf, len, true);
      __keystroke.bypass_mode = false;
      return;
    } else if (len == 1 && buf[0] &&  //
               (buf[0] & 255) == __ttyconf.vlnext) {
      __keystroke.bypass_mode = true;
      if (!(__ttyconf.magic & kTtySilence) &&  // ECHO
          !(__ttyconf.magic & kTtyEchoRaw))    // ECHOCTL
        WriteTty("^\b", 2);
      return;
    }
  }

  // handle backspace in canonical mode
  if (len == 1 && buf[0] &&                  //
      (buf[0] & 255) == __ttyconf.verase &&  //
      !(__ttyconf.magic & kTtyUncanon) &&    //
      !(__ttyconf.magic & kTtyNoIexten)) {
    bool should_visually_erase =             //
        !(__ttyconf.magic & kTtySilence) &&  // ECHO
        !(__ttyconf.magic & kTtyNoEchoe);    // ECHOE
    EraseKeystroke(should_visually_erase);
    if (!(__ttyconf.magic & kTtySilence) &&  // ECHO
        (__ttyconf.magic & kTtyNoEchoe) &&   // !ECHOE
        !(__ttyconf.magic & kTtyEchoRaw))    // ECHOCTL
      WriteCtl(buf, len, true);
    return;
  }

  // handle ctrl-w in canonical mode
  // this lets you erase the last word
  if (len == 1 && buf[0] &&                   //
      (buf[0] & 255) == __ttyconf.vwerase &&  //
      !(__ttyconf.magic & kTtyUncanon) &&     //
      !(__ttyconf.magic & kTtyNoIexten)) {
    bool should_visually_erase =             //
        !(__ttyconf.magic & kTtySilence) &&  // ECHO
        !(__ttyconf.magic & kTtyNoEchoe);    // ECHOE
    while (IsLookingAtSpace() == 1)
      EraseKeystroke(should_visually_erase);
    while (IsLookingAtSpace() == 0)
      EraseKeystroke(should_visually_erase);
    if (!(__ttyconf.magic & kTtySilence) &&  // ECHO
        (__ttyconf.magic & kTtyNoEchoe) &&   // !ECHOE
        !(__ttyconf.magic & kTtyEchoRaw))    // ECHOCTL
      WriteCtl(buf, len, true);
    return;
  }

  // handle kill in canonical mode
  // this clears the line you're editing
  if (len == 1 && buf[0] &&                 //
      (buf[0] & 255) == __ttyconf.vkill &&  //
      !(__ttyconf.magic & kTtyUncanon) &&   //
      !(__ttyconf.magic & kTtyNoIexten)) {
    bool should_visually_kill =              //
        !(__ttyconf.magic & kTtySilence) &&  // ECHO
        !(__ttyconf.magic & kTtyNoEchok) &&  // ECHOK
        !(__ttyconf.magic & kTtyNoEchoke);   // ECHOKE
    while (EraseKeystroke(should_visually_kill)) {
    }
    if (!(__ttyconf.magic & kTtySilence) &&  // ECHO
        !(__ttyconf.magic & kTtyNoEchok) &&  // ECHOK
        (__ttyconf.magic & kTtyNoEchoke) &&  // !ECHOKE
        !(__ttyconf.magic & kTtyEchoRaw))    // ECHOCTL
      WriteCtl(buf, len, true);
    return;
  }

  // handle ctrl-r in canonical mode
  // this reprints the line you're editing
  if (len == 1 && buf[0] &&                    //
      (buf[0] & 255) == __ttyconf.vreprint &&  //
      !(__ttyconf.magic & kTtyUncanon) &&      // ICANON
      !(__ttyconf.magic & kTtyNoIexten) &&     // IEXTEN
      !(__ttyconf.magic & kTtySilence)) {      // ECHO
    struct Dll *e;
    if (!(__ttyconf.magic & kTtyEchoRaw))
      WriteCtl(buf, len, true);
    WriteTty("\r\n", 2);
    for (e = dll_first(__keystroke.line); e;
         e = dll_next(__keystroke.line, e)) {
      struct Keystroke *k = KEYSTROKE_CONTAINER(e);
      WriteCtl(k->buf, k->buflen, true);
    }
    return;
  }

  // allocate object to hold keystroke
  struct Keystroke *k = NewKeystroke();
  if (!k)
    return;
  memcpy(k->buf, buf, sizeof(k->buf));
  k->buflen = len;

  // echo input if it was successfully recorded
  // assuming the win32 console isn't doing it already
  EchoConsoleNt(buf, len, false);

  // save keystroke to appropriate list
  if (__ttyconf.magic & kTtyUncanon) {
    dll_make_last(&__keystroke.list, &k->elem);
  } else {
    dll_make_last(&__keystroke.line, &k->elem);

    // flush canonical mode line on enter
    if (len == 1 && buf[0] &&
        ((buf[0] & 255) == '\n' ||            //
         (buf[0] & 255) == __ttyconf.veol ||  //
         ((buf[0] & 255) == __ttyconf.veol2 &&
          !(__ttyconf.magic & kTtyNoIexten)))) {
      dll_make_last(&__keystroke.list, __keystroke.line);
      __keystroke.line = 0;
    }
  }
}

textwindows static void IngestConsoleInput(void) {
  uint32_t i, n;
  struct NtInputRecord records[16];
  for (;;) {
    if (__keystroke.end_of_file)
      return;
    if (!GetNumberOfConsoleInputEvents(__keystroke.cin, &n))
      goto UnexpectedEof;
    if (n > ARRAYLEN(records))
      n = ARRAYLEN(records);
    PopulateKeystrokes(n + 1);
    if (n > __keystroke.free_keys)
      n = __keystroke.free_keys;
    if (!n)
      return;
    if (!ReadConsoleInput(__keystroke.cin, records, n, &n))
      goto UnexpectedEof;
    for (i = 0; i < n && !__keystroke.end_of_file; ++i)
      IngestConsoleInputRecord(records + i);
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
  for (e = dll_first(__keystroke.list); e; e = dll_next(__keystroke.list, e))
    count += KEYSTROKE_CONTAINER(e)->buflen;
  if (!count && __keystroke.end_of_file)
    count = -1;
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
            // \e[?1h decckm on
            __keystroke.ohno_decckm = true;
            kVirtualKey[0].normal_str = -S("OA");  // kNtVkUp
            kVirtualKey[1].normal_str = -S("OB");  // kNtVkDown
            kVirtualKey[2].normal_str = -S("OC");  // kNtVkRight
            kVirtualKey[3].normal_str = -S("OD");  // kNtVkLeft
            kVirtualKey[4].normal_str = -S("OF");  // kNtVkEnd
            kVirtualKey[5].normal_str = -S("OH");  // kNtVkHome
          } else if ((ismouse |= IsMouseModeCommand(x))) {
            __ttyconf.magic |= kTtyXtMouse;
            cm2 |= kNtEnableMouseInput;
            cm2 &= ~kNtEnableQuickEditMode;  // take mouse
          }
          t = ASC;
        } else if (data[i] == 'l') {
          if (x == 1) {
            // \e[?1l decckm off
            __keystroke.ohno_decckm = false;
            kVirtualKey[0].normal_str = S("A");  // kNtVkUp
            kVirtualKey[1].normal_str = S("B");  // kNtVkDown
            kVirtualKey[2].normal_str = S("C");  // kNtVkRight
            kVirtualKey[3].normal_str = S("D");  // kNtVkLeft
            kVirtualKey[4].normal_str = S("F");  // kNtVkEnd
            kVirtualKey[5].normal_str = S("H");  // kNtVkHome
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
  if (cm2 != cm)
    SetConsoleMode(GetConsoleInputHandle(), cm2);
}

textwindows static bool DigestConsoleInput(char *data, size_t size, int *rc) {

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
    if ((__ttyconf.magic & kTtyUncanon) && toto >= __ttyconf.vmin)
      break;
  }

  // return result
  if (toto) {
    *rc = toto;
    return true;
  } else {
    return false;
  }
}

textwindows static uint32_t DisableProcessedInput(void) {
  // the time has come to ensure that ctrl-v ctrl-c works in icanon mode
  // we're perfectly capable of generating a SIGINT or SIGQUIT ourselves
  // while the cosmo termios driver is in control; so we disable windows
  // console input processing for now; we'll turn it back on when we are
  // done, since it's useful for ensuring asynchronous signal deliveries
  uint32_t inmode = 0;
  if (GetConsoleMode(__keystroke.cin, &inmode))
    if (inmode & kNtEnableProcessedInput)
      SetConsoleMode(__keystroke.cin, inmode & ~kNtEnableProcessedInput);
  return inmode;
}

textwindows static void RestoreProcessedInput(uint32_t inmode) {
  // re-enable win32 console input processing, if it was enabled when we
  // started, and no signal handler callbacks changed things in-between.
  if (inmode & kNtEnableProcessedInput) {
    uint32_t inmode2;
    if (GetConsoleMode(__keystroke.cin, &inmode2))
      if (inmode2 == (inmode & ~kNtEnableProcessedInput))
        SetConsoleMode(__keystroke.cin, inmode);
  }
}

textwindows static int CountConsoleInputBytesBlockingImpl(uint32_t ms,
                                                          sigset_t waitmask,
                                                          bool restartable) {
  InitConsole();
  struct timespec deadline =
      timespec_add(sys_clock_gettime_monotonic_nt(), timespec_frommillis(ms));
  for (;;) {
    int sig = 0;
    intptr_t sev;
    if (!(sev = CreateEvent(0, 0, 0, 0)))
      return __winerr();
    struct PosixThread *pt = _pthread_self();
    pt->pt_event = sev;
    pt->pt_blkmask = waitmask;
    atomic_store_explicit(&pt->pt_blocker, PT_BLOCKER_EVENT,
                          memory_order_release);
    if (_check_cancel() == -1) {
      atomic_store_explicit(&pt->pt_blocker, 0, memory_order_release);
      CloseHandle(sev);
      return -1;
    }
    if (_weaken(__sig_get) && (sig = _weaken(__sig_get)(waitmask))) {
      atomic_store_explicit(&pt->pt_blocker, 0, memory_order_release);
      CloseHandle(sev);
      goto DeliverSignal;
    }
    struct timespec now = sys_clock_gettime_monotonic_nt();
    struct timespec remain = timespec_subz(deadline, now);
    int64_t millis = timespec_tomillis(remain);
    uint32_t waitms = MIN(millis, 0xffffffffu);
    intptr_t hands[] = {__keystroke.cin, sev};
    uint32_t wi = WaitForMultipleObjects(2, hands, 0, waitms);
    atomic_store_explicit(&pt->pt_blocker, 0, memory_order_release);
    CloseHandle(sev);
    if (wi == -1u)
      return __winerr();

    // check for wait timeout
    if (wi == kNtWaitTimeout)
      return etimedout();

    // handle event on console handle. this means we can now read from the
    // conosle without blocking. so the first thing we do is slurp up your
    // keystroke data. some of those keystrokes might cause a signal to be
    // raised. so we need to check for pending signals again and handle it
    if (wi == 0) {
      int got = CountConsoleInputBytes();
      // we might have read a keystroke that generated a signal
      if (_weaken(__sig_get) && (sig = _weaken(__sig_get)(waitmask)))
        goto DeliverSignal;
      if (got == -1)
        // this is a bona fide eof and console errors are logged to strace
        return 0;
      if (got == 0)
        // this can happen for multiple reasons. first our driver controls
        // user interactions in canonical mode. secondly we could lose the
        // race with another thread that's reading input.
        continue;
      return got;
    }

    if (wi == 1 && _weaken(__sig_get) && (sig = _weaken(__sig_get)(waitmask))) {
      // handle event on throwaway semaphore, it is poked by signal delivery
    DeliverSignal:;
      int handler_was_called = 0;
      do {
        handler_was_called |= _weaken(__sig_relay)(sig, SI_KERNEL, waitmask);
      } while ((sig = _weaken(__sig_get)(waitmask)));
      if (_check_cancel() == -1)
        return -1;
      if (handler_was_called & SIG_HANDLED_NO_RESTART)
        return eintr();
      if (handler_was_called & SIG_HANDLED_SA_RESTART)
        if (!restartable)
          return eintr();
    }
  }
}

textwindows static int CountConsoleInputBytesBlocking(uint32_t ms,
                                                      sigset_t waitmask) {
  int got = CountConsoleInputBytes();
  if (got == -1)
    return 0;
  if (got > 0)
    return got;
  uint32_t inmode = DisableProcessedInput();
  int rc = CountConsoleInputBytesBlockingImpl(ms, waitmask, true);
  RestoreProcessedInput(inmode);
  return rc;
}

textwindows static int WaitToReadFromConsole(struct Fd *f, sigset_t waitmask) {
  uint32_t ms = -1;
  if (!__ttyconf.vmin) {
    if (!__ttyconf.vtime) {
      return 0;  // non-blocking w/o raising eagain
    } else {
      ms = __ttyconf.vtime * 100;
    }
  }
  if (f->flags & _O_NONBLOCK)
    return eagain();
  int olderr = errno;
  int rc = CountConsoleInputBytesBlockingImpl(ms, waitmask, true);
  if (rc == -1 && errno == ETIMEDOUT) {
    // read() never raises ETIMEDOUT so if vtime elapses we raise an EOF
    errno = olderr;
    rc = 0;
  }
  return rc;
}

textwindows static ssize_t ReadFromConsole(struct Fd *f, void *data,
                                           size_t size, sigset_t waitmask) {
  int rc;
  InitConsole();
  uint32_t inmode = DisableProcessedInput();
  do {
    LockKeystrokes();
    IngestConsoleInput();
    bool done = DigestConsoleInput(data, size, &rc);
    UnlockKeystrokes();
    if (done)
      break;
  } while ((rc = WaitToReadFromConsole(f, waitmask)) > 0);
  RestoreProcessedInput(inmode);
  return rc;
}

textwindows ssize_t ReadBuffer(int fd, void *data, size_t size, int64_t offset,
                               sigset_t waitmask) {

  // switch to terminal polyfill if reading from win32 console
  struct Fd *f = g_fds.p + fd;

  if (f->kind == kFdDevNull)
    return 0;

  if (f->kind == kFdDevRandom) {
    ProcessPrng(data, size);
    return size;
  }

  if (f->kind == kFdConsole)
    return ReadFromConsole(f, data, size, waitmask);

  // perform heavy lifting
  ssize_t rc;
  rc = sys_readwrite_nt(fd, data, size, offset, f->handle, waitmask, ReadFile);
  if (rc != -2)
    return rc;

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

textwindows static ssize_t ReadIovecs(int fd, const struct iovec *iov,
                                      size_t iovlen, int64_t opt_offset,
                                      sigset_t waitmask) {
  ssize_t rc;
  size_t i, total;
  if (opt_offset < -1)
    return einval();
  while (iovlen && !iov[0].iov_len)
    iov++, iovlen--;
  if (iovlen) {
    for (total = i = 0; i < iovlen; ++i) {
      if (!iov[i].iov_len)
        continue;
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
      if (opt_offset != -1)
        opt_offset += rc;
      if (rc < iov[i].iov_len)
        break;
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
