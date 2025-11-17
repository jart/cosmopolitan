/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2022 Justine Alexandra Roberts Tunney                              │
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
#include "libc/atomic.h"
#include "libc/calls/sig.internal.h"
#include "libc/calls/state.internal.h"
#include "libc/calls/syscall-nt.internal.h"
#include "libc/calls/syscall_support-nt.internal.h"
#include "libc/dce.h"
#include "libc/intrin/atomic.h"
#include "libc/intrin/describebacktrace.h"
#include "libc/intrin/weaken.h"
#include "libc/limits.h"
#include "libc/nt/files.h"
#include "libc/nt/memory.h"
#include "libc/nt/runtime.h"
#include "libc/nt/synchronization.h"
#include "libc/nt/thread.h"
#include "libc/nt/thunk/msabi.h"
#include "libc/runtime/internal.h"
#include "libc/sysv/consts/sig.h"
#include "libc/sysv/pib.h"
#ifdef __x86_64__

__msabi extern typeof(DeleteFile) *const __imp_DeleteFileW;
__msabi extern typeof(GetCurrentThreadId) *const __imp_GetCurrentThreadId;
__msabi extern typeof(TerminateProcess) *const __imp_TerminateProcess;
__msabi extern typeof(UnmapViewOfFile) *const __imp_UnmapViewOfFile;

atomic_ulong __fake_process_signals;
alignas(64) static atomic_ulong __sig_lock_obj;

textwindows static bool __sig_trylock(void) {
  unsigned tid = __imp_GetCurrentThreadId();
  unsigned long word =
      atomic_load_explicit(&__sig_lock_obj, memory_order_relaxed);
  if (!word) {
    if (atomic_compare_exchange_strong_explicit(&__sig_lock_obj, &word, tid,
                                                memory_order_acquire,
                                                memory_order_relaxed))
      return true;
  } else if ((unsigned)word == tid) {
    if (atomic_compare_exchange_strong_explicit(
            &__sig_lock_obj, &word, word + 0x100000000, memory_order_acquire,
            memory_order_relaxed))
      return true;
  }
  return false;
}

textwindows void __sig_lock(void) {
  for (;;)
    if (__sig_trylock())
      return;
}

textwindows void __sig_unlock(void) {
  for (;;) {
    unsigned long word =
        atomic_load_explicit(&__sig_lock_obj, memory_order_relaxed);
#ifdef MODE_DBG
    if ((unsigned)word != __imp_GetCurrentThreadId()) {
      atomic_store_explicit(&__sig_lock_obj, 0, memory_order_relaxed);
      __imp_TerminateProcess(-1, 157);
      __builtin_unreachable();
    }
#endif
    unsigned long word2;
    if (word >> 32) {
      word2 = word - 0x100000000;
    } else {
      word2 = 0;
    }
    if (atomic_compare_exchange_weak_explicit(&__sig_lock_obj, &word, word2,
                                              memory_order_release,
                                              memory_order_relaxed))
      return;
  }
}

textwindows void __sig_wipe(void) {
  atomic_store_explicit(&__sig_lock_obj, 0, memory_order_relaxed);
}

/**
 * Terminates the calling process and all of its threads.
 */
textwindows void TerminateThisProcess(uint32_t dwExitCode) {

  // if this is a vfork()'d process then _Exit() becomes longjmp()
  if (_weaken(sys_vfork_nt_exit))
    _weaken(sys_vfork_nt_exit)(dwExitCode);

  // delete sig file
  for (long attempt = 0; attempt < 32; ++attempt) {
    if (__get_pib()->sigpending != &__fake_process_signals) {
      if (__sig_trylock()) {
        __imp_UnmapViewOfFile(__get_pib()->sigpending);
        char16_t path[128];
        __imp_DeleteFileW(__sig_process_path(path, __get_pib()->pid));
        __get_pib()->sigpending = &__fake_process_signals;
        __sig_unlock();
        break;
      } else {
        SleepEx(0, 0);
      }
    } else {
      break;
    }
  }

  // TODO(jart): wait for vfork() threads to die first?

  // TODO(jart): wait for fork() to copy memory into suspended child?

  // "When a process terminates itself, TerminateProcess stops execution
  //  of the calling thread and does not return." -Quoth MSDN
  __imp_TerminateProcess(-1, dwExitCode);
  __builtin_unreachable();
}

#endif /* __x86_64__ */
