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
#include "libc/log/check.h"
#include "libc/stdio/stdio.h"

const char *DescribeStatfsType(int64_t x) {
  switch (x) {
    case 0xadf5:
      return "ADFS_SUPER_MAGIC";
    case 0xadff:
      return "AFFS_SUPER_MAGIC";
    case 0x0187:
      return "AUTOFS_SUPER_MAGIC";
    case 0x1373:
      return "DEVFS_SUPER_MAGIC";
    case 0x1cd1:
      return "DEVPTS_SUPER_MAGIC";
    case 0xf15f:
      return "ECRYPTFS_SUPER_MAGIC";
    case 0x137d:
      return "EXT_SUPER_MAGIC";
    case 0xef51:
      return "EXT2_OLD_SUPER_MAGIC";
    case 0xef53:
      return "EXT2/3/4_SUPER_MAGIC";
    case 0x4244:
      return "HFS_SUPER_MAGIC";
    case 0x9660:
      return "ISOFS_SUPER_MAGIC";
    case 0x72b6:
      return "JFFS2_SUPER_MAGIC";
    case 0x137f:
      return "MINIX_SUPER_MAGIC";
    case 0x138f:
      return "MINIX_SUPER_MAGIC2";
    case 0x2468:
      return "MINIX2_SUPER_MAGIC";
    case 0x2478:
      return "MINIX2_SUPER_MAGIC2";
    case 0x4d5a:
      return "MINIX3_SUPER_MAGIC";
    case 0x4d44:
      return "MSDOS_SUPER_MAGIC";
    case 0x564c:
      return "NCP_SUPER_MAGIC";
    case 0x6969:
      return "NFS_SUPER_MAGIC";
    case 0x3434:
      return "NILFS_SUPER_MAGIC";
    case 0x9fa1:
      return "OPENPROM_SUPER_MAGIC";
    case 0x9fa0:
      return "PROC_SUPER_MAGIC";
    case 0x002f:
      return "QNX4_SUPER_MAGIC";
    case 0x7275:
      return "ROMFS_MAGIC";
    case 0x517b:
      return "SMB_SUPER_MAGIC";
    case 0x9fa2:
      return "USBDEVICE_SUPER_MAGIC";
    case 0x27e0eb:
      return "CGROUP_SUPER_MAGIC";
    case 0xbad1dea:
      return "FUTEXFS_SUPER_MAGIC";
    case 0x5346414f:
      return "AFS_SUPER_MAGIC";
    case 0x09041934:
      return "ANON_INODE_FS_MAGIC";
    case 0x62646576:
      return "BDEVFS_MAGIC";
    case 0x42465331:
      return "BEFS_SUPER_MAGIC";
    case 0x1badface:
      return "BFS_MAGIC";
    case 0x42494e4d:
      return "BINFMTFS_MAGIC";
    case 0xcafe4a11:
      return "BPF_FS_MAGIC";
    case 0x9123683e:
      return "BTRFS_SUPER_MAGIC";
    case 0x73727279:
      return "BTRFS_TEST_MAGIC";
    case 0x63677270:
      return "CGROUP2_SUPER_MAGIC";
    case 0xff534d42:
      return "CIFS_MAGIC_NUMBER";
    case 0x73757245:
      return "CODA_SUPER_MAGIC";
    case 0x012ff7b7:
      return "COH_SUPER_MAGIC";
    case 0x28cd3d45:
      return "CRAMFS_MAGIC";
    case 0x64626720:
      return "DEBUGFS_MAGIC";
    case 0xde5e81e4:
      return "EFIVARFS_MAGIC";
    case 0x00414a53:
      return "EFS_SUPER_MAGIC";
    case 0xf2f52010:
      return "F2FS_SUPER_MAGIC";
    case 0x65735546:
      return "FUSE_SUPER_MAGIC";
    case 0x00c0ffee:
      return "HOSTFS_SUPER_MAGIC";
    case 0xf995e849:
      return "HPFS_SUPER_MAGIC";
    case 0x958458f6:
      return "HUGETLBFS_MAGIC";
    case 0x3153464a:
      return "JFS_SUPER_MAGIC";
    case 0x19800202:
      return "MQUEUE_MAGIC";
    case 0x11307854:
      return "MTD_INODE_FS_MAGIC";
    case 0x6e736673:
      return "NSFS_MAGIC";
    case 0x5346544e:
      return "NTFS_SB_MAGIC";
    case 0x7461636f:
      return "OCFS2_SUPER_MAGIC";
    case 0x794c7630:
      return "OVERLAYFS_SUPER_MAGIC";
    case 0x50495045:
      return "PIPEFS_MAGIC";
    case 0x6165676c:
      return "PSTOREFS_MAGIC";
    case 0x68191122:
      return "QNX6_SUPER_MAGIC";
    case 0x858458f6:
      return "RAMFS_MAGIC";
    case 0x52654973:
      return "REISERFS_SUPER_MAGIC";
    case 0x73636673:
      return "SECURITYFS_MAGIC";
    case 0xf97cff8c:
      return "SELINUX_MAGIC";
    case 0x43415d53:
      return "SMACK_MAGIC";
    case 0x534f434b:
      return "SOCKFS_MAGIC";
    case 0x73717368:
      return "SQUASHFS_MAGIC";
    case 0x62656572:
      return "SYSFS_MAGIC";
    case 0x012ff7b6:
      return "SYSV2_SUPER_MAGIC";
    case 0x012ff7b5:
      return "SYSV4_SUPER_MAGIC";
    case 0x01021994:
      return "TMPFS_MAGIC";
    case 0x74726163:
      return "TRACEFS_MAGIC";
    case 0x15013346:
      return "UDF_SUPER_MAGIC";
    case 0x00011954:
      return "UFS_MAGIC";
    case 0x01021997:
      return "V9FS_MAGIC";
    case 0xa501fcf5:
      return "VXFS_SUPER_MAGIC";
    case 0xabba1974:
      return "XENFS_SUPER_MAGIC";
    case 0x012ff7b4:
      return "XENIX_SUPER_MAGIC";
    case 0x58465342:
      return "XFS_SUPER_MAGIC";
    case 0x012fd16d:
      return "_XIAFS_SUPER_MAGIC";
    default:
      break;
  }
  return "UNKNOWN";
}

void ShowIt(const char *path) {
  struct statfs sf = {0};
  CHECK_NE(-1, statfs(path, &sf));

  printf("filesystem %s\n", path);
  printf("f_type    = %#x (%s)\n", sf.f_type, DescribeStatfsType(sf.f_type));
  printf("f_bsize   = %'zu (%s)\n", sf.f_bsize, "optimal transfer block size");
  printf("f_blocks  = %'zu (%s)\n", sf.f_blocks,
         "total data blocks in filesystem");
  printf("f_bfree   = %'zu (%s)\n", sf.f_bfree, "free blocks in filesystem");
  printf("f_bavail  = %'zu (%s)\n", sf.f_bavail, "free blocks available to");
  printf("f_files   = %'zu (%s)\n", sf.f_files,
         "total file nodes in filesystem");
  printf("f_ffree   = %'zu (%s)\n", sf.f_ffree,
         "free file nodes in filesystem");
  printf("f_fsid    = %d:%d (%s)\n", (sf.f_fsid & 0xffff0000) >> 020,
         (sf.f_fsid & 0x0000ffff) >> 000, "filesystem id");
  printf("f_namelen = %'zu (%s)\n", sf.f_namelen,
         "maximum length of filenames");
  printf("f_frsize  = %'zu (%s)\n", sf.f_frsize, "fragment size");

  printf("f_flags   = %#x", sf.f_flags);
  if (sf.f_flags & 1) printf(" ST_RDONLY");
  if (sf.f_flags & 2) printf(" ST_NOSUID");
  if (sf.f_flags & 4) printf(" ST_NODEV");
  if (sf.f_flags & 8) printf(" ST_NOEXEC");
  if (sf.f_flags & 16) printf(" ST_SYNCHRONOUS");
  if (sf.f_flags & 64) printf(" ST_MANDLOCK");
  if (sf.f_flags & 128) printf(" ST_WRITE");
  if (sf.f_flags & 256) printf(" ST_APPEND");
  if (sf.f_flags & 512) printf(" ST_IMMUTABLE");
  if (sf.f_flags & 1024) printf(" ST_NOATIME");
  if (sf.f_flags & 2048) printf(" ST_NODIRATIME");
  if (sf.f_flags & 4096) printf(" ST_RELATIME");
  printf("\n");

  printf("\n");
}

int main(int argc, char *argv[]) {
  if (!IsLinux()) return 1;
  ShowIt("/");
}
