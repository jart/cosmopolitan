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
#include "libc/calls/struct/statfs-meta.internal.h"
#include "libc/dce.h"
#include "libc/str/str.h"

static const char *DescribeStatfsTypeLinux(int64_t x) {
  switch (x) {
    case 0xadf5:
      return "adfs";
    case 0xadff:
      return "affs";
    case 0x0187:
      return "autofs";
    case 0x1373:
      return "devfs";
    case 0x1cd1:
      return "devpts";
    case 0xf15f:
      return "ecryptfs";
    case 0x137d:
      return "ext1";
    case 0xef51:
      return "ext2_old";
    case 0xef53:
      return "ext";  // ext2, ext3, ext4
    case 0x4244:
      return "hfs";
    case 0x9660:
      return "isofs";
    case 0x72b6:
      return "jffs2";
    case 0x137f:
      return "minix";
    case 0x138f:
      return "minix2";
    case 0x2468:
      return "minix2";
    case 0x2478:
      return "minix22";
    case 0x4d5a:
      return "minix3";
    case 0x4d44:
      return "msdos";
    case 0x564c:
      return "ncp";
    case 0x6969:
      return "nfs";
    case 0x3434:
      return "nilfs";
    case 0x9fa1:
      return "openprom";
    case 0x9fa0:
      return "proc";
    case 0x002f:
      return "qnx4";
    case 0x7275:
      return "romfs";
    case 0x517b:
      return "smb";
    case 0x9fa2:
      return "usbdevice";
    case 0x27e0eb:
      return "cgroup";
    case 0xbad1dea:
      return "futexfs";
    case 0x5346414f:
      return "afs";
    case 0x09041934:
      return "anon_inode_fs";
    case 0x62646576:
      return "bdevfs";
    case 0x42465331:
      return "befs";
    case 0x1badface:
      return "bfs";
    case 0x42494e4d:
      return "binfmtfs";
    case 0xcafe4a11:
      return "bpf_fs";
    case 0x9123683e:
      return "btrfs";
    case 0x73727279:
      return "btrfs_test";
    case 0x63677270:
      return "cgroup2";
    case 0xff534d42:
      return "cifs_number";
    case 0x73757245:
      return "coda";
    case 0x012ff7b7:
      return "coh";
    case 0x28cd3d45:
      return "cramfs";
    case 0x64626720:
      return "debugfs";
    case 0xde5e81e4:
      return "efivarfs";
    case 0x00414a53:
      return "efs";
    case 0xf2f52010:
      return "f2fs";
    case 0x65735546:
      return "fuse";
    case 0x00c0ffee:
      return "hostfs";
    case 0xf995e849:
      return "hpfs";
    case 0x958458f6:
      return "hugetlbfs";
    case 0x3153464a:
      return "jfs";
    case 0x19800202:
      return "mqueue";
    case 0x11307854:
      return "mtd_inode_fs";
    case 0x6e736673:
      return "nsfs";
    case 0x5346544e:
      return "ntfs_sb";
    case 0x7461636f:
      return "ocfs2";
    case 0x794c7630:
      return "overlayfs";
    case 0x50495045:
      return "pipefs";
    case 0x6165676c:
      return "pstorefs";
    case 0x68191122:
      return "qnx6";
    case 0x858458f6:
      return "ramfs";
    case 0x52654973:
      return "reiserfs";
    case 0x73636673:
      return "securityfs";
    case 0xf97cff8c:
      return "selinux";
    case 0x43415d53:
      return "smack";
    case 0x534f434b:
      return "sockfs";
    case 0x73717368:
      return "squashfs";
    case 0x62656572:
      return "sysfs";
    case 0x012ff7b6:
      return "sysv2";
    case 0x012ff7b5:
      return "sysv4";
    case 0x01021994:
      return "tmpfs";
    case 0x74726163:
      return "tracefs";
    case 0x15013346:
      return "udf";
    case 0x00011954:
      return "ufs";
    case 0x01021997:
      return "v9fs";
    case 0xa501fcf5:
      return "vxfs";
    case 0xabba1974:
      return "xenfs";
    case 0x012ff7b4:
      return "xenix";
    case 0x58465342:
      return "xfs";
    case 0x012fd16d:
      return "_xiafs";
    case 0x2fc12fc1:
      return "zfs";
    case 0x4253584e:
      return "apfs";
    default:
      return "unknown";
  }
}

void statfs2cosmo(struct statfs *f, const union statfs_meta *m) {
  int64_t f_type;
  int64_t f_bsize;
  int64_t f_blocks;
  int64_t f_bfree;
  int64_t f_bavail;
  int64_t f_files;
  int64_t f_ffree;
  fsid_t f_fsid;
  int64_t f_namelen;
  int64_t f_frsize;
  int64_t f_flags;
  int32_t f_owner;
  char f_fstypename[16];

  if (IsLinux()) {
    f_type = m->linux.f_type;
    f_bsize = m->linux.f_bsize;
    f_blocks = m->linux.f_blocks;
    f_bfree = m->linux.f_bfree;
    f_bavail = m->linux.f_bavail;
    f_files = m->linux.f_files;
    f_ffree = m->linux.f_ffree;
    f_fsid = m->linux.f_fsid;
    f_namelen = m->linux.f_namelen;
    f_frsize = m->linux.f_frsize;
    f_flags = m->linux.f_flags;
    f_owner = 0;
    bzero(f_fstypename, 16);
    strcpy(f_fstypename, DescribeStatfsTypeLinux(m->linux.f_type));

  } else if (IsFreebsd()) {
    f_type = m->freebsd.f_type;
    f_bsize = m->freebsd.f_iosize;
    f_blocks = m->freebsd.f_blocks;
    f_bfree = m->freebsd.f_bfree;
    f_bavail = m->freebsd.f_bavail;
    f_files = m->freebsd.f_files;
    f_ffree = m->freebsd.f_ffree;
    f_fsid = m->freebsd.f_fsid;
    f_namelen = m->freebsd.f_namemax;
    f_frsize = m->freebsd.f_bsize;
    f_flags = m->freebsd.f_flags;
    f_owner = m->freebsd.f_owner;
    memcpy(f_fstypename, m->freebsd.f_fstypename, 16);

  } else if (IsXnu()) {
    f_type = m->xnu.f_type;
    f_bsize = m->xnu.f_iosize;
    f_blocks = m->xnu.f_blocks;
    f_bfree = m->xnu.f_bfree;
    f_bavail = m->xnu.f_bavail;
    f_files = m->xnu.f_files;
    f_ffree = m->xnu.f_ffree;
    f_fsid = m->xnu.f_fsid;
    f_namelen = 255;
    f_frsize = m->xnu.f_bsize;
    f_flags = m->xnu.f_flags;
    f_owner = m->xnu.f_owner;
    memcpy(f_fstypename, m->xnu.f_fstypename, 16);

  } else if (IsOpenbsd()) {
    f_type = f->f_type;
    f_bsize = m->openbsd.f_iosize;
    f_blocks = m->openbsd.f_blocks;
    f_bfree = m->openbsd.f_bfree;
    f_bavail = m->openbsd.f_bavail;
    f_files = m->openbsd.f_files;
    f_ffree = m->openbsd.f_ffree;
    f_fsid = m->openbsd.f_fsid;
    f_namelen = m->openbsd.f_namemax;
    f_frsize = m->openbsd.f_bsize;
    f_flags = m->openbsd.f_flags;
    f_owner = m->openbsd.f_owner;
    memcpy(f_fstypename, m->openbsd.f_fstypename, 16);

  } else if (IsNetbsd()) {
    f_type = m->netbsd.f_type;
    f_bsize = m->netbsd.f_iosize;
    f_blocks = m->netbsd.f_blocks;
    f_bfree = m->netbsd.f_bfree;
    f_bavail = m->netbsd.f_bavail;
    f_files = m->netbsd.f_files;
    f_ffree = m->netbsd.f_ffree;
    f_fsid = m->netbsd.f_fsid;
    f_namelen = 511;
    f_frsize = m->netbsd.f_bsize;
    f_flags = m->netbsd.f_flags;
    f_owner = m->netbsd.f_owner;
    memcpy(f_fstypename, m->netbsd.f_fstypename, 16);

  } else {
    notpossible;
  }

  f->f_type = f_type;
  f->f_bsize = f_bsize;
  f->f_blocks = f_blocks;
  f->f_bfree = f_bfree;
  f->f_bavail = f_bavail;
  f->f_files = f_files;
  f->f_ffree = f_ffree;
  f->f_fsid = f_fsid;
  f->f_namelen = f_namelen;
  f->f_frsize = f_frsize;
  f->f_flags = f_flags;
  f->f_owner = f_owner;
  memcpy(f->f_fstypename, f_fstypename, 16);
}
