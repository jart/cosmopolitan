#if 0
/*─────────────────────────────────────────────────────────────────╗
│ To the extent possible under law, Justine Tunney has waived      │
│ all copyright and related or neighboring rights to this file,    │
│ as it is written in the following disclaimers:                   │
│   • http://unlicense.org/                                        │
│   • http://creativecommons.org/publicdomain/zero/1.0/            │
╚─────────────────────────────────────────────────────────────────*/
#endif
#include "libc/calls/struct/statfs.h"
#include "libc/dce.h"
#include "libc/fmt/conv.h"
#include "libc/log/check.h"
#include "libc/nt/enum/statfs.h"
#include "libc/stdio/stdio.h"
#include "libc/sysv/consts/st.h"

dontinline void ShowIt(const char *path) {
  char ibuf[21];
  struct statfs sf = {0};
  CHECK_NE(-1, statfs(path, &sf));

  printf("filesystem %s\n", path);
  printf("f_type    = %#x (%s)\n", sf.f_type, sf.f_fstypename);
  sizefmt(ibuf, sf.f_bsize, 1024);
  printf("f_bsize   = %,zu (%sb %s)\n", sf.f_bsize, ibuf,
         "optimal transfer block size");
  sizefmt(ibuf, sf.f_frsize, 1024);
  printf("f_frsize  = %,zu (%sb %s)\n", sf.f_frsize, ibuf, "fragment size");
  sizefmt(ibuf, sf.f_blocks * sf.f_frsize, 1024);
  printf("f_blocks  = %,zu (%sb %s)\n", sf.f_blocks, ibuf,
         "total data blocks in filesystem");
  sizefmt(ibuf, sf.f_bfree * sf.f_frsize, 1024);
  printf("f_bfree   = %,zu (%sb %s)\n", sf.f_bfree, ibuf,
         "free blocks in filesystem");
  sizefmt(ibuf, sf.f_bavail * sf.f_frsize, 1024);
  printf("f_bavail  = %,zu (%sb %s)\n", sf.f_bavail, ibuf,
         "free blocks available to use");
  printf("f_files   = %,zu (%s)\n", sf.f_files,
         "total file nodes in filesystem");
  printf("f_ffree   = %,zu (%s)\n", sf.f_ffree,
         "free file nodes in filesystem");
  printf("f_fsid    = %#lx (%s)\n",
         sf.f_fsid.__val[0] | (uint64_t)sf.f_fsid.__val[1] << 32,
         "filesystem id");
  printf("f_owner   = %#lx (%s)\n", sf.f_owner, "user that created mount");
  printf("f_namelen = %,zu (%s)\n", sf.f_namelen,
         "maximum length of filenames");

  printf("f_flags   = %#x", sf.f_flags);

  if (!IsWindows()) {
    if (ST_RDONLY && (sf.f_flags & ST_RDONLY)) {
      printf(" ST_RDONLY");
    }
    if (ST_NOSUID && (sf.f_flags & ST_NOSUID)) {
      printf(" ST_NOSUID");
    }
    if (ST_NODEV && (sf.f_flags & ST_NODEV)) {
      printf(" ST_NODEV");
    }
    if (ST_NOEXEC && (sf.f_flags & ST_NOEXEC)) {
      printf(" ST_NOEXEC");
    }
    if (ST_SYNCHRONOUS && (sf.f_flags & ST_SYNCHRONOUS)) {
      printf(" ST_SYNCHRONOUS");
    }
    if (ST_MANDLOCK && (sf.f_flags & ST_MANDLOCK)) {
      printf(" ST_MANDLOCK");
    }
    if (ST_WRITE && (sf.f_flags & ST_WRITE)) {
      printf(" ST_WRITE");
    }
    if (ST_APPEND && (sf.f_flags & ST_APPEND)) {
      printf(" ST_APPEND");
    }
    if (ST_IMMUTABLE && (sf.f_flags & ST_IMMUTABLE)) {
      printf(" ST_IMMUTABLE");
    }
    if (ST_NOATIME && (sf.f_flags & ST_NOATIME)) {
      printf(" ST_NOATIME");
    }
    if (ST_NODIRATIME && (sf.f_flags & ST_NODIRATIME)) {
      printf(" ST_NODIRATIME");
    }
    if (ST_RELATIME && (sf.f_flags & ST_RELATIME)) {
      printf(" ST_RELATIME");
    }

  } else {
    if (sf.f_flags & kNtFileCasePreservedNames) {
      printf(" CasePreservedNames");
    }
    if (sf.f_flags & kNtFileCaseSensitiveSearch) {
      printf(" CaseSensitiveSearch");
    }
    if (sf.f_flags & kNtFileFileCompression) {
      printf(" FileCompression");
    }
    if (sf.f_flags & kNtFileNamedStreams) {
      printf(" NamedStreams");
    }
    if (sf.f_flags & kNtFilePersistentAcls) {
      printf(" PersistentAcls");
    }
    if (sf.f_flags & kNtFileReadOnlyVolume) {
      printf(" ReadOnlyVolume");
    }
    if (sf.f_flags & kNtFileSequentialWriteOnce) {
      printf(" SequentialWriteOnce");
    }
    if (sf.f_flags & kNtFileSupportsEncryption) {
      printf(" SupportsEncryption");
    }
    if (sf.f_flags & kNtFileSupportsExtendedAttributes) {
      printf(" SupportsExtendedAttributes");
    }
    if (sf.f_flags & kNtFileSupportsHardLinks) {
      printf(" SupportsHardLinks");
    }
    if (sf.f_flags & kNtFileSupportsObjectIds) {
      printf(" SupportsObjectIds");
    }
    if (sf.f_flags & kNtFileSupportsOpenByFileId) {
      printf(" SupportsOpenByFileId");
    }
    if (sf.f_flags & kNtFileSupportsReparsePoints) {
      printf(" SupportsReparsePoints");
    }
    if (sf.f_flags & kNtFileSupportsSparseFiles) {
      printf(" SupportsSparseFiles");
    }
    if (sf.f_flags & kNtFileSupportsTransactions) {
      printf(" SupportsTransactions");
    }
    if (sf.f_flags & kNtFileSupportsUsnJournal) {
      printf(" SupportsUsnJournal");
    }
    if (sf.f_flags & kNtFileUnicodeOnDisk) {
      printf(" UnicodeOnDisk");
    }
    if (sf.f_flags & kNtFileVolumeIsCompressed) {
      printf(" VolumeIsCompressed");
    }
    if (sf.f_flags & kNtFileVolumeQuotas) {
      printf(" VolumeQuotas");
    }
    if (sf.f_flags & kNtFileSupportsBlockRefcounting) {
      printf(" SupportsBlockRefcounting");
    }
  }
  printf("\n");

  printf("\n");
}

int main(int argc, char *argv[]) {
  if (argc <= 1) {
    ShowIt("/");
  } else {
    for (int i = 1; i < argc; ++i) {
      ShowIt(argv[i]);
    }
  }
}
