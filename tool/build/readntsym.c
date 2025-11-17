#/*─────────────────────────────────────────────────────────────────╗
 │ To the extent possible under law, Justine Tunney has waived      │
 │ all copyright and related or neighboring rights to this file,    │
 │ as it is written in the following disclaimers:                   │
 │   • http://unlicense.org/                                        │
 │   • http://creativecommons.org/publicdomain/zero/1.0/            │
 ╚─────────────────────────────────────────────────────────────────*/
#include "libc/assert.h"
#include "libc/calls/syscall_support-nt.internal.h"
#include "libc/dce.h"
#include "libc/intrin/kprintf.h"
#include "libc/nt/createfile.h"
#include "libc/nt/enum/accessmask.h"
#include "libc/nt/enum/creationdisposition.h"
#include "libc/nt/enum/fileflagandattributes.h"
#include "libc/nt/enum/filesharemode.h"
#include "libc/nt/enum/fsctl.h"
#include "libc/nt/enum/io.h"
#include "libc/nt/errors.h"
#include "libc/nt/events.h"
#include "libc/nt/files.h"
#include "libc/nt/runtime.h"
#include "libc/nt/struct/reparsedatabuffer.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/sysv/errfuns.h"

#define BIG_REPARSE_DATA_BUFFER_SIZE 16384

static struct ReadSymlinkNt {
  union {
    struct NtReparseDataBuffer rdb;
    char rdb_bytes[BIG_REPARSE_DATA_BUFFER_SIZE];
  };
} m[1];

static void doit(const char *path) {
  char16_t path16[PATH_MAX];
  if (__mkntpath(path, path16) == -1) {
    perror(path);
    return;
  }

  // open file
  int64_t hSymlink;
  if ((hSymlink =
           CreateFile(path16, kNtFileReadAttributes,
                      kNtFileShareRead | kNtFileShareWrite | kNtFileShareDelete,
                      0, kNtOpenExisting,
                      kNtFileFlagOpenReparsePoint | kNtFileFlagBackupSemantics,
                      0)) == -1) {
    kprintf("%hs: CreateFile failed %d\n", path16, GetLastError());
    return;
  }

  uintptr_t hEvent;
  unassert((hEvent = CreateEventTls()));
  uint32_t dwBytesReturned;
  struct NtOverlapped overlap = {.hEvent = hEvent};
  bool32 ok =
      DeviceIoControl(hSymlink, kNtFsctlGetReparsePoint, 0, 0, &m->rdb,
                      BIG_REPARSE_DATA_BUFFER_SIZE, &dwBytesReturned, &overlap);
  if (!ok && GetLastError() == kNtErrorIoPending)
    ok = GetOverlappedResult(hSymlink, &overlap, &dwBytesReturned, true);
  CloseEventTls(hEvent);
  if (!ok) {
    kprintf("%hs: DeviceIoControl failed %d\n", path16, GetLastError());
    CloseHandle(hSymlink);
    return;
  }

  CloseHandle(hSymlink);

  kprintf("\n");
  kprintf("%s\n", path);
  kprintf("%hs\n", path16);
  kprintf("ReparseTag = %#x\n", m->rdb.ReparseTag);
  kprintf("ReparseDataLength = %#x\n", m->rdb.ReparseDataLength);
  kprintf("Reserved = %#x\n", m->rdb.Reserved);

  if (m->rdb.ReparseTag == kNtIoReparseTagSymlink) {
    kprintf("SubstituteNameOffset = %#x\n",
            m->rdb.SymbolicLinkReparseBuffer.SubstituteNameOffset);
    kprintf("SubstituteNameLength = %#x\n",
            m->rdb.SymbolicLinkReparseBuffer.SubstituteNameLength);
    kprintf("PrintNameOffset = %#x\n",
            m->rdb.SymbolicLinkReparseBuffer.PrintNameOffset);
    kprintf("PrintNameLength = %#x\n",
            m->rdb.SymbolicLinkReparseBuffer.PrintNameLength);
    kprintf("Flags = %#x\n", m->rdb.SymbolicLinkReparseBuffer.Flags);
    kprintf(
        "SubstituteName = %#.*hs\n",
        m->rdb.SymbolicLinkReparseBuffer.SubstituteNameLength /
            sizeof(char16_t),
        (char16_t *)((char *)m->rdb.SymbolicLinkReparseBuffer.PathBuffer +
                     m->rdb.SymbolicLinkReparseBuffer.SubstituteNameOffset));
    kprintf("PrintName = %#.*hs\n",
            m->rdb.SymbolicLinkReparseBuffer.PrintNameLength / sizeof(char16_t),
            (char16_t *)((char *)m->rdb.SymbolicLinkReparseBuffer.PathBuffer +
                         m->rdb.SymbolicLinkReparseBuffer.PrintNameOffset));
  }
}

int main(int argc, char *argv[]) {
  if (!IsWindows()) {
    fprintf(stderr, "error: this program is meant for windows\n");
    exit(1);
  }
  for (int i = 1; i < argc; ++i)
    doit(argv[i]);
}
