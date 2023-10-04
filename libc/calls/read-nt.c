/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
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
#include "libc/calls/internal.h"
#include "libc/calls/sig.internal.h"
#include "libc/calls/struct/fd.internal.h"
#include "libc/calls/struct/iovec.h"
#include "libc/calls/syscall_support-nt.internal.h"
#include "libc/calls/ttydefaults.h"
#include "libc/errno.h"
#include "libc/fmt/itoa.h"
#include "libc/intrin/atomic.h"
#include "libc/intrin/describeflags.internal.h"
#include "libc/intrin/dll.h"
#include "libc/intrin/kprintf.h"
#include "libc/intrin/nomultics.internal.h"
#include "libc/intrin/strace.internal.h"
#include "libc/intrin/weaken.h"
#include "libc/macros.internal.h"
#include "libc/mem/mem.h"
#include "libc/nt/console.h"
#include "libc/nt/enum/consolemodeflags.h"
#include "libc/nt/enum/filetype.h"
#include "libc/nt/enum/vk.h"
#include "libc/nt/enum/wait.h"
#include "libc/nt/errors.h"
#include "libc/nt/events.h"
#include "libc/nt/files.h"
#include "libc/nt/runtime.h"
#include "libc/nt/struct/inputrecord.h"
#include "libc/nt/synchronization.h"
#include "libc/nt/thread.h"
#include "libc/nt/thunk/msabi.h"
#include "libc/str/str.h"
#include "libc/str/utf16.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/sig.h"
#include "libc/sysv/consts/termios.h"
#include "libc/sysv/errfuns.h"
#include "libc/thread/thread.h"
#include "libc/thread/tls.h"
#ifdef __x86_64__

static const struct {
  int vk;
  int normal_str;
  int shift_str;
  int ctrl_str;
  int shift_ctrl_str;
} kVirtualKey[] = {
#define S(s) W(s "\0\0")
#define W(s) (s[3] << 24 | s[2] << 16 | s[1] << 8 | s[0])
    {kNtVkUp, S("A"), S("1;2A"), S("1;5A"), S("1;6A")},
    {kNtVkDown, S("B"), S("1;2B"), S("1;5B"), S("1;6B")},
    {kNtVkLeft, S("D"), S("1;2D"), S("1;5D"), S("1;6D")},
    {kNtVkRight, S("C"), S("1;2C"), S("1;5C"), S("1;6C")},
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
#undef W
#undef S
};

#define KEYSTROKE_CONTAINER(e) DLL_CONTAINER(struct Keystroke, elem, e)

struct Keystroke {
  char buf[23];
  unsigned char buflen;
  struct Dll elem;
};

struct Keystrokes {
  struct Dll *list;
  struct Dll *line;
  struct Dll *free;
  bool end_of_file;
  unsigned char pc;
  uint16_t utf16hs;
  pthread_mutex_t lock;
  struct Keystroke pool[512];
};

static struct Keystrokes __keystroke;

static textwindows void LockKeystrokes(void) {
  pthread_mutex_lock(&__keystroke.lock);
}

static textwindows void UnlockKeystrokes(void) {
  pthread_mutex_unlock(&__keystroke.lock);
}

static textwindows uint64_t BlockSignals(void) {
  return atomic_exchange(&__get_tls()->tib_sigmask, -1);
}

static textwindows void UnblockSignals(uint64_t mask) {
  atomic_store_explicit(&__get_tls()->tib_sigmask, mask, memory_order_release);
}

static textwindows int RaiseSignal(int sig) {
  __get_tls()->tib_sigpending |= 1ull << (sig - 1);
  return 0;
}

static textwindows int GetVirtualKey(uint16_t vk, bool shift, bool ctrl) {
  for (int i = 0; i < ARRAYLEN(kVirtualKey); ++i) {
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

static textwindows int ProcessKeyEvent(const struct NtInputRecord *r, char *p) {

  uint16_t c = r->Event.KeyEvent.uChar.UnicodeChar;
  uint16_t vk = r->Event.KeyEvent.wVirtualKeyCode;
  uint16_t cks = r->Event.KeyEvent.dwControlKeyState;

  // ignore keyup events
  if (!r->Event.KeyEvent.bKeyDown && (!c || vk != kNtVkMenu)) {
    return 0;
  }

#if 0
  kprintf("bKeyDown=%hhhd wVirtualKeyCode=%s wVirtualScanCode=%s "
          "UnicodeChar=%#x[%#lc] dwControlKeyState=%s\n",
          r->Event.KeyEvent.bKeyDown,
          DescribeVirtualKeyCode(r->Event.KeyEvent.wVirtualKeyCode),
          DescribeVirtualKeyCode(r->Event.KeyEvent.wVirtualScanCode),
          r->Event.KeyEvent.uChar.UnicodeChar,
          r->Event.KeyEvent.uChar.UnicodeChar,
          DescribeControlKeyState(r->Event.KeyEvent.dwControlKeyState));
#endif

  // process arrow keys, function keys, etc.
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

  // ^/ should be interpreted as ^_
  if (vk == kNtVkOem_2 && (cks & (kNtLeftCtrlPressed | kNtRightCtrlPressed))) {
    p[n++] = 037;
    return n;
  }

  // everything needs a unicode mapping from here on out
  // handle some stuff microsoft doesn't encode, e.g. ctrl+alt+b
  if (!c) {
    if (isgraph(vk) && (cks & (kNtLeftCtrlPressed | kNtRightCtrlPressed))) {
      c = CTRL(vk);
    } else {
      return 0;
    }
  }

  // shift-tab is backtab or ^[Z
  if (vk == kNtVkTab && (cks & (kNtShiftPressed))) {
    p[n++] = 033;
    p[n++] = '[';
    p[n++] = 'Z';
    return n;
  }

  // translate utf-16 into utf-32
  if (IsHighSurrogate(c)) {
    __keystroke.utf16hs = c;
    return 0;
  }
  if (IsLowSurrogate(c)) {
    c = MergeUtf16(__keystroke.utf16hs, c);
  }

  // enter sends \r in a raw terminals
  // make it a multics newline instead
  if (c == '\r' && !(__ttyconf.magic & kTtyNoCr2Nl)) {
    c = '\n';
  }

  // microsoft doesn't encode ctrl-space (^@) as nul
  // detecting it is also impossible w/ kNtEnableVirtualTerminalInput
  if (c == ' ' && (cks & (kNtLeftCtrlPressed | kNtRightCtrlPressed))) {
    c = '\0';
  }

  // make it possible to distinguish ctrl-h (^H) from backspace (^?)
  if (c == kNtVkBack && !(cks & (kNtLeftCtrlPressed | kNtRightCtrlPressed))) {
    c = 0177;
  }

  // handle ctrl-c and ctrl-\, which tcsetattr() is able to remap
  if (c && !(__ttyconf.magic & kTtyNoIsigs)) {
    if (c == __ttyconf.vintr) {
      return RaiseSignal(SIGINT);
    } else if (c == __ttyconf.vquit) {
      return RaiseSignal(SIGQUIT);
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
  if ((cks & (kNtLeftAltPressed | kNtRightAltPressed)) &&
      r->Event.KeyEvent.bKeyDown) {
    p[n++] = 033;
  }

  // convert utf-32 to utf-8
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
    if (!(r->Event.MouseEvent.dwControlKeyState &
          (kNtShiftPressed | kNtLeftCtrlPressed | kNtRightCtrlPressed |
           kNtLeftAltPressed | kNtRightAltPressed))) {
      bool isup = ((int)r->Event.MouseEvent.dwButtonState >> 16) > 0;
      if (__ttyconf.magic & kTtyXtMouse) {
        e = isup ? 80 : 81;
        goto OutputXtermMouseEvent;
      } else {
        // we disable mouse highlighting when the tty is put in raw mode
        // to mouse wheel events with widely understood vt100 arrow keys
        *p++ = 033;
        *p++ = '[';
        if (isup) {
          *p++ = 'A';  // \e[A up
        } else {
          *p++ = 'B';  // \e[B down
        }
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
      return RaiseSignal(SIGWINCH);
    default:
      return 0;
  }
}

static textwindows struct Keystroke *NewKeystroke(void) {
  struct Dll *e;
  struct Keystroke *k = 0;
  int i, n = ARRAYLEN(__keystroke.pool);
  if (atomic_load_explicit(&__keystroke.pc, memory_order_acquire) < n &&
      (i = atomic_fetch_add(&__keystroke.pc, 1)) < n) {
    k = __keystroke.pool + i;
  } else {
    if ((e = dll_first(__keystroke.free))) {
      k = KEYSTROKE_CONTAINER(e);
      dll_remove(&__keystroke.free, &k->elem);
    }
    if (!k) {
      if (_weaken(malloc)) {
        k = _weaken(malloc)(sizeof(struct Keystroke));
      } else {
        enomem();
        return 0;
      }
    }
  }
  if (k) {
    bzero(k, sizeof(*k));
    dll_init(&k->elem);
  }
  return k;
}

static textwindows void WriteTty(struct Fd *f, const char *p, size_t n) {
  int64_t hOutput;
  uint32_t dwConsoleMode;
  if (f->kind == kFdConsole) {
    hOutput = f->extra;
  } else if (g_fds.p[1].kind == kFdFile &&
             GetConsoleMode(g_fds.p[1].handle, &dwConsoleMode)) {
    hOutput = g_fds.p[1].handle;
  } else if (g_fds.p[2].kind == kFdFile &&
             GetConsoleMode(g_fds.p[2].handle, &dwConsoleMode)) {
    hOutput = g_fds.p[2].handle;
  } else {
    hOutput = g_fds.p[1].handle;
  }
  WriteFile(hOutput, p, n, 0, 0);
}

static textwindows bool IsCtl(int c) {
  return isascii(c) && iscntrl(c) && c != '\n' && c != '\t';
}

static textwindows void WriteTtyCtl(struct Fd *f, const char *p, size_t n) {
  size_t i;
  for (i = 0; i < n; ++i) {
    if (IsCtl(p[i])) {
      char ctl[2];
      ctl[0] = '^';
      ctl[1] = p[i] ^ 0100;
      WriteTty(f, ctl, 2);
    } else {
      WriteTty(f, p + i, 1);
    }
  }
}

static textwindows void EchoTty(struct Fd *f, const char *p, size_t n) {
  if (__ttyconf.magic & kTtyEchoRaw) {
    WriteTty(f, p, n);
  } else {
    WriteTtyCtl(f, p, n);
  }
}

static textwindows bool EraseKeystroke(struct Fd *f) {
  struct Dll *e;
  if ((e = dll_last(__keystroke.line))) {
    struct Keystroke *k = KEYSTROKE_CONTAINER(e);
    dll_remove(&__keystroke.line, e);
    dll_make_first(&__keystroke.free, e);
    for (int i = k->buflen; i--;) {
      if ((k->buf[i] & 0300) == 0200) continue;
      WriteTty(f, "\b \b", 3);
      if (!(__ttyconf.magic & kTtyEchoRaw) && IsCtl(k->buf[i])) {
        WriteTty(f, "\b \b", 3);
      }
    }
    return true;
  } else {
    return false;
  }
}

static textwindows void IngestConsoleInputRecord(struct Fd *f,
                                                 struct NtInputRecord *r) {

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
    EraseKeystroke(f);
    return;
  }

  // handle kill in canonical mode
  if (len == 1 && buf[0] &&                 //
      (buf[0] & 255) == __ttyconf.vkill &&  //
      !(__ttyconf.magic & kTtyUncanon)) {
    while (EraseKeystroke(f)) {
    }
    return;
  }

  // allocate an object to hold this keystroke
  struct Keystroke *k;
  if (!(k = NewKeystroke())) {
    STRACE("ran out of memory to hold keystroke %#.*s", len, buf);
    return;
  }
  memcpy(k->buf, buf, sizeof(k->buf));
  k->buflen = len;

  // echo input if it was successfully recorded
  // assuming the win32 console isn't doing it already
  if (!(__ttyconf.magic & kTtySilence)) {
    EchoTty(f, buf, len);
  }

  // save keystroke to appropriate list
  if (__ttyconf.magic & kTtyUncanon) {
    dll_make_last(&__keystroke.list, &k->elem);
  } else {
    dll_make_last(&__keystroke.line, &k->elem);

    // handle end-of-line in canonical mode
    if (len == 1 && buf[0] &&
        ((buf[0] & 255) == '\n' ||            //
         (buf[0] & 255) == __ttyconf.veol ||  //
         (buf[0] & 255) == __ttyconf.veol2)) {
      dll_make_last(&__keystroke.list, __keystroke.line);
      __keystroke.line = 0;
      return;
    }
  }
}

static textwindows void IngestConsoleInput(struct Fd *f) {
  uint32_t i, n;
  struct NtInputRecord records[16];
  if (!__keystroke.end_of_file) {
    do {
      if (GetNumberOfConsoleInputEvents(f->handle, &n)) {
        if (n) {
          n = MIN(ARRAYLEN(records), n);
          if (ReadConsoleInput(f->handle, records, n, &n)) {
            for (i = 0; i < n && !__keystroke.end_of_file; ++i) {
              IngestConsoleInputRecord(f, records + i);
            }
          } else {
            STRACE("ReadConsoleInput failed w/ %d", GetLastError());
            __keystroke.end_of_file = true;
            break;
          }
        }
      } else {
        STRACE("GetNumberOfConsoleInputRecords failed w/ %d", GetLastError());
        __keystroke.end_of_file = true;
        break;
      }
    } while (n == ARRAYLEN(records));
  }
}

textwindows int FlushConsoleInputBytes(int64_t handle) {
  int rc;
  uint64_t m;
  m = BlockSignals();
  LockKeystrokes();
  if (FlushConsoleInputBuffer(handle)) {
    dll_make_first(&__keystroke.free, __keystroke.list);
    __keystroke.list = 0;
    dll_make_first(&__keystroke.free, __keystroke.line);
    __keystroke.line = 0;
    rc = 0;
  } else {
    rc = __winerr();
  }
  UnlockKeystrokes();
  UnblockSignals(m);
  return rc;
}

textwindows int CountConsoleInputBytes(struct Fd *f) {
  int count = 0;
  struct Dll *e;
  uint64_t m = BlockSignals();
  LockKeystrokes();
  IngestConsoleInput(f);
  for (e = dll_first(__keystroke.list); e; e = dll_next(__keystroke.list, e)) {
    count += KEYSTROKE_CONTAINER(e)->buflen;
  }
  if (!count && __keystroke.end_of_file) {
    count = -1;
  }
  UnlockKeystrokes();
  UnblockSignals(m);
  return count;
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
    } else {
      dll_remove(&__keystroke.list, e);
      dll_make_first(&__keystroke.free, e);
    }
    k->buflen = remain;
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

static textwindows ssize_t ReadFromWindowsConsole(struct Fd *f, void *data,
                                                  size_t size) {
  int rc = -1;
  for (;;) {
    bool done = false;
    uint64_t m;
    m = BlockSignals();
    LockKeystrokes();
    IngestConsoleInput(f);
    done = DigestConsoleInput(data, size, &rc);
    UnlockKeystrokes();
    UnblockSignals(m);
    if (done) break;
    if (f->flags & O_NONBLOCK) return eagain();
    uint32_t ms = __SIG_POLL_INTERVAL_MS;
    if (!__ttyconf.vmin) {
      if (!__ttyconf.vtime) {
        return 0;
      } else {
        ms = __ttyconf.vtime * 100;
      }
    }
    if (_check_interrupts(kSigOpRestartable)) return -1;
    if (__pause_thread(ms)) {
      if (errno == EAGAIN) {
        errno = EINTR;  // TODO(jart): Why do we need it?
      }
      return -1;
    }
  }
  return rc;
}

textwindows ssize_t sys_read_nt_impl(int fd, void *data, size_t size,
                                     int64_t offset) {

  bool32 ok;
  struct Fd *f;
  uint32_t got;
  int64_t handle;
  struct PosixThread *pt;

  f = g_fds.p + fd;
  handle = f->handle;
  pt = _pthread_self();
  pt->abort_errno = EAGAIN;
  size = MIN(size, 0x7ffff000);

  bool pwriting = offset != -1;
  bool seekable = f->kind == kFdFile && GetFileType(handle) == kNtFileTypeDisk;
  bool nonblock = !!(f->flags & O_NONBLOCK);

  if (pwriting && !seekable) {
    return espipe();
  }
  if (!pwriting) {
    offset = 0;
  }

  uint32_t cm;
  if (!seekable && (f->kind == kFdConsole || GetConsoleMode(handle, &cm))) {
    return ReadFromWindowsConsole(f, data, size);
  }

  if (!pwriting && seekable) {
    pthread_mutex_lock(&f->lock);
    offset = f->pointer;
  }

  struct NtOverlapped overlap = {.hEvent = CreateEvent(0, 0, 0, 0),
                                 .Pointer = offset};
  // the win32 manual says it's important to *not* put &got here
  // since for overlapped i/o, we always use GetOverlappedResult
  ok = ReadFile(handle, data, size, 0, &overlap);
  if (!ok && GetLastError() == kNtErrorIoPending) {
  BlockingOperation:
    if (!nonblock) {
      pt->ioverlap = &overlap;
      pt->iohandle = handle;
    }
    if (nonblock) {
      CancelIoEx(handle, &overlap);
    } else if (_check_interrupts(kSigOpRestartable)) {
    Interrupted:
      pt->abort_errno = errno;
      CancelIoEx(handle, &overlap);
    } else {
      for (;;) {
        uint32_t i;
        if (g_fds.stdin.inisem) {
          ReleaseSemaphore(g_fds.stdin.inisem, 1, 0);
        }
        i = WaitForSingleObject(overlap.hEvent, __SIG_IO_INTERVAL_MS);
        if (i == kNtWaitTimeout) {
          if (_check_interrupts(kSigOpRestartable)) {
            goto Interrupted;
          }
        } else {
          break;
        }
      }
    }
    pt->ioverlap = 0;
    pt->iohandle = 0;
    ok = true;
  }
  if (ok) {
    // overlapped is allocated on stack, so it's important we wait
    // for windows to acknowledge that it's done using that memory
    ok = GetOverlappedResult(handle, &overlap, &got, nonblock);
    if (!ok && GetLastError() == kNtErrorIoIncomplete) {
      goto BlockingOperation;
    }
  }
  CloseHandle(overlap.hEvent);

  if (!pwriting && seekable) {
    if (ok) f->pointer = offset + got;
    pthread_mutex_unlock(&f->lock);
  }

  if (ok) {
    return got;
  }

  errno_t err;
  if (_weaken(pthread_testcancel_np) &&
      (err = _weaken(pthread_testcancel_np)())) {
    return ecanceled();
  }

  switch (GetLastError()) {
    case kNtErrorBrokenPipe:    // broken pipe
    case kNtErrorNoData:        // closing named pipe
    case kNtErrorHandleEof:     // pread read past EOF
      return 0;                 //
    case kNtErrorAccessDenied:  // read doesn't return EACCESS
      return ebadf();           //
    case kNtErrorOperationAborted:
      errno = pt->abort_errno;
      return -1;
    default:
      return __winerr();
  }
}

textwindows ssize_t sys_read_nt(int fd, const struct iovec *iov, size_t iovlen,
                                int64_t opt_offset) {
  ssize_t rc;
  size_t i, total;
  if (opt_offset < -1) return einval();
  while (iovlen && !iov[0].iov_len) iov++, iovlen--;
  if (iovlen) {
    for (total = i = 0; i < iovlen; ++i) {
      // TODO(jart): disable cancelations after first iteration
      if (!iov[i].iov_len) continue;
      rc = sys_read_nt_impl(fd, iov[i].iov_base, iov[i].iov_len, opt_offset);
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
    return sys_read_nt_impl(fd, NULL, 0, opt_offset);
  }
}

#endif /* __x86_64__ */
