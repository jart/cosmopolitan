// Copyright 2025 Justine Alexandra Roberts Tunney
//
// Permission to use, copy, modify, and/or distribute this software for
// any purpose with or without fee is hereby granted, provided that the
// above copyright notice and this permission notice appear in all copies.
//
// THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
// WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE
// AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
// DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
// PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
// TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
// PERFORMANCE OF THIS SOFTWARE.

#include <cosmo.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include "libc/intrin/kprintf.h"

// test access(), faccessat(), etc. system calls

void test_directory(void) {
  if (!getuid())
    return;
  char path[] = "/tmp/access_test.XXXXXX";
  if (!mkdtemp(path))
    exit(1);
  if (access(path, F_OK))
    exit(2);
  if (access(path, R_OK))
    exit(3);
  if (access(path, W_OK))
    exit(4);
  if (access(path, X_OK))
    exit(4);
  if (chmod(path, 0400))
    exit(5);
  if (access(path, R_OK))
    exit(6);
  if (!access(path, W_OK))
    exit(6);
  if (errno != EACCES)
    exit(7);
  if (rmdir(path))
    exit(8);
}

void test_regular(void) {
  if (!getuid())
    return;
  int fd;
  char path[] = "/tmp/access_test.XXXXXX";
  if ((fd = mkstemp(path)) == -1)
    exit(10);
  if (close(fd))
    exit(11);
  if (access(path, F_OK))
    exit(12);
  if (access(path, W_OK))
    exit(13);
  if (!access(path, X_OK))
    exit(14);
  if (errno != EACCES)
    exit(15);
  if (chmod(path, 0400))
    exit(16);
  if (!access(path, W_OK))
    exit(17);
  if (errno != EACCES)
    exit(18);
  if (unlink(path))
    exit(19);
}

void test_symlink(void) {
  if (!getuid())
    return;
  char dir[] = "/tmp/access_test.XXXXXX";
  if (!mkdtemp(dir))
    exit(20);
  char exe[PATH_MAX];
  strcpy(exe, dir);
  strcat(exe, "/exe");
  int fd;
  if ((fd = creat(exe, 0755)) == -1)
    exit(21);
  char buf[64] = "MZ";
  if (write(fd, buf, 64) != 64)
    exit(22);
  if (close(fd))
    exit(23);
  char sym[PATH_MAX];
  strcpy(sym, dir);
  strcat(sym, "/sym");
  if (symlink("exe", sym))
    exit(24);
  if (access(sym, F_OK))
    exit(25);
  if (access(sym, R_OK | W_OK | X_OK))
    exit(26);
  if (!IsFreebsd())
    if (faccessat(AT_FDCWD, sym, F_OK, AT_SYMLINK_NOFOLLOW))
      exit(27);
  if (!IsFreebsd())
    if (faccessat(AT_FDCWD, sym, W_OK, AT_SYMLINK_NOFOLLOW))
      exit(28);
  if (!IsFreebsd())
    if (faccessat(AT_FDCWD, sym, X_OK, AT_SYMLINK_NOFOLLOW))
      exit(29);
  if (unlink(sym))
    exit(30);
  if (unlink(exe))
    exit(31);
  if (rmdir(dir))
    exit(32);
}

void test_symlinks_are_always_executable(void) {
  if (!getuid())
    return;
  char dir[] = "/tmp/access_test.XXXXXX";
  if (!mkdtemp(dir))
    exit(33);
  char reg[PATH_MAX];
  strcpy(reg, dir);
  strcat(reg, "/reg");
  int fd;
  if ((fd = creat(reg, 0644)) == -1)
    exit(34);
  if (close(fd))
    exit(35);
  char sym[PATH_MAX];
  strcpy(sym, dir);
  strcat(sym, "/sym");
  if (symlink("reg", sym))
    exit(36);
  if (!IsFreebsd())
    if (faccessat(AT_FDCWD, sym, X_OK, AT_SYMLINK_NOFOLLOW))
      exit(37);
  if (unlink(sym))
    exit(38);
  if (unlink(reg))
    exit(39);
  if (rmdir(dir))
    exit(40);
}

void test_dirfd(void) {
  char dir[] = "/tmp/access_test.XXXXXX";
  if (!mkdtemp(dir))
    exit(41);
  int dirfd;
  if ((dirfd = open(dir, O_RDONLY | O_DIRECTORY)) == -1)
    exit(42);
  int fd;
  if ((fd = openat(dirfd, "reg", O_WRONLY | O_CREAT | O_TRUNC, 0644)) == -1)
    exit(43);
  if (close(fd))
    exit(44);
  if (symlinkat("reg", dirfd, "sym"))
    exit(44);
  if (faccessat(dirfd, "sym", F_OK, 0))
    exit(45);
  if (unlinkat(dirfd, "sym", 0))
    exit(46);
  if (unlinkat(dirfd, "reg", 0))
    exit(47);
  if (close(dirfd))
    exit(48);
  if (rmdir(dir))
    exit(49);
}

void test_dirfd_via_symlink(void) {
  char root[] = "/tmp/access_test.XXXXXX";
  if (!mkdtemp(root))
    exit(50);
  int rootfd;
  if ((rootfd = open(root, O_RDONLY | O_DIRECTORY)) == -1)
    exit(51);
  if (mkdirat(rootfd, "dir", 0755))
    exit(52);
  if (symlinkat("dir", rootfd, "dirsym"))
    exit(53);
  int dirfd;
  if ((dirfd = openat(rootfd, "dirsym", O_RDONLY | O_DIRECTORY)) == -1)
    exit(54);
  int fd;
  if ((fd = openat(dirfd, "reg", O_WRONLY | O_CREAT | O_TRUNC, 0644)) == -1)
    exit(55);
  if (close(fd))
    exit(56);
  if (symlinkat("reg", dirfd, "sym"))
    exit(57);
  if (faccessat(dirfd, "sym", F_OK, 0))
    exit(58);
  if (unlinkat(dirfd, "sym", 0))
    exit(59);
  if (unlinkat(dirfd, "reg", 0))
    exit(60);
  if (close(dirfd))
    exit(61);
  if (unlinkat(rootfd, "dirsym", AT_REMOVEDIR))
    exit(62);
  if (unlinkat(rootfd, "dir", AT_REMOVEDIR))
    exit(63);
  if (close(rootfd))
    exit(64);
  if (rmdir(root))
    exit(65);
}

void test_enotdir(void) {
  if (!getuid())
    return;
  char root[] = "/tmp/access_test.XXXXXX";
  if (!mkdtemp(root))
    exit(66);
  int dirfd;
  if ((dirfd = open(root, O_RDONLY | O_DIRECTORY)) == -1)
    exit(67);
  if (mkdirat(dirfd, "dir", 0755))
    exit(68);
  int fd;
  if ((fd = openat(dirfd, "reg", O_WRONLY | O_CREAT | O_TRUNC, 0444)) == -1)
    exit(69);
  if (faccessat(dirfd, "reg", R_OK, 0))
    exit(70);
  if (!faccessat(dirfd, "reg", W_OK, 0) || errno != EACCES)
    exit(71);
  if (close(fd))
    exit(72);
  if (symlinkat("dir", dirfd, "dirsym"))
    exit(73);
  if (symlinkat("reg", dirfd, "regsym"))
    exit(74);
  if (faccessat(dirfd, "regsym", R_OK, 0))
    exit(75);
  if (!faccessat(dirfd, "regsym", W_OK, 0) || errno != EACCES)
    exit(76);
  if (!IsFreebsd())
    if (faccessat(dirfd, "regsym", W_OK, AT_SYMLINK_NOFOLLOW))
      exit(77);
  if ((fd = openat(dirfd, "reg", O_RDONLY | O_DIRECTORY, 0644)) != -1)
    exit(78);
  if (errno != ENOTDIR)
    exit(79);
  if ((fd = openat(dirfd, "regsym", O_RDONLY | O_DIRECTORY, 0644)) != -1)
    exit(80);
  if (errno != ENOTDIR)
    exit(81);
  if ((fd = openat(dirfd, "dirsym", O_RDONLY | O_DIRECTORY, 0644)) == -1)
    exit(82);
  if (close(fd))
    exit(83);
  if ((fd = openat(dirfd, "dirsym", O_RDONLY | O_NOFOLLOW, 0644)) != -1)
    exit(84);
  if (errno != ELOOP)
    exit(85);
  if (unlinkat(dirfd, "dirsym", 0))
    exit(86);
  if (unlinkat(dirfd, "regsym", 0))
    exit(87);
  if (unlinkat(dirfd, "dir", AT_REMOVEDIR))
    exit(88);
  if (unlinkat(dirfd, "reg", 0))
    exit(89);
  if (close(dirfd))
    exit(90);
  if (rmdir(root))
    exit(91);
}

void test_readonlydir_fchmod(void) {
  if (!getuid())
    return;
  char dir[] = "/tmp/access_test.XXXXXX";
  if (!mkdtemp(dir))
    exit(92);
  int dirfd;
  if ((dirfd = open(dir, O_RDONLY | O_DIRECTORY)) == -1)
    exit(93);
  if (fchmod(dirfd, 0555))
    exit(94);
  if (!access(dir, W_OK) || errno != EACCES)
    exit(95);
  if (close(dirfd))
    exit(96);
  if (rmdir(dir))
    exit(97);
}

void test_readonlydir_chmod(void) {
  if (!getuid())
    return;
  char dir[] = "/tmp/access_test.XXXXXX";
  if (!mkdtemp(dir))
    exit(98);
  if (chmod(dir, 0555))
    exit(99);
  if (!access(dir, W_OK) || errno != EACCES)
    exit(100);
  if (rmdir(dir))
    exit(101);
}

void test_readonlydir_via_symlink(void) {
  if (!getuid())
    return;
  char root[] = "/tmp/access_test.XXXXXX";
  if (!mkdtemp(root))
    exit(102);
  int rootfd;
  if ((rootfd = open(root, O_RDONLY)) == -1)
    exit(103);
  if (mkdirat(rootfd, "dir", 0755))
    exit(104);
  if (symlinkat("dir", rootfd, "sym"))
    exit(105);
  if (faccessat(rootfd, "dir", W_OK, 0))
    exit(106);
  if (faccessat(rootfd, "sym", W_OK, 0))
    exit(107);
  if (fchmodat(rootfd, "sym", 0555, 0))
    exit(108);
  if (!faccessat(rootfd, "sym", W_OK, 0) || errno != EACCES)
    exit(109);
  if (!faccessat(rootfd, "dir", W_OK, 0) || errno != EACCES)
    exit(110);
  if (!IsFreebsd() && !IsLinux()) {
    // FreeBSD doesn't have faccessat(AT_SYMLINK_NOFOLLOW)
    // Linux 6.8.0 EOPNOTSUPPs when making symlinks read-only
    if (faccessat(rootfd, "sym", W_OK, AT_SYMLINK_NOFOLLOW))
      exit(111);
    if (fchmodat(rootfd, "sym", 0400, AT_SYMLINK_NOFOLLOW))
      exit(112);
    if (!faccessat(rootfd, "sym", W_OK, AT_SYMLINK_NOFOLLOW) || errno != EACCES)
      exit(113);
  }
  if (unlinkat(rootfd, "sym", 0))
    exit(114);
  if (unlinkat(rootfd, "dir", AT_REMOVEDIR))
    exit(115);
  if (close(rootfd))
    exit(116);
  if (rmdir(root))
    exit(117);
}

const char *bad_pointer = (const char *)77;

void test_efault(void) {
  if (access(0, 0) != -1)
    exit(120);
  if (errno != EFAULT)
    exit(121);
  if (access(bad_pointer, 0) != -1)
    exit(122);
  if (errno != EFAULT)
    exit(123);
}

void test_eloop(void) {
  char dir[] = "/tmp/access_test.XXXXXX";
  if (!mkdtemp(dir))
    exit(130);
  int dirfd;
  if ((dirfd = open(dir, O_RDONLY)) == -1)
    exit(131);
  if (symlinkat("sym", dirfd, "sym"))
    exit(132);
  if (faccessat(dirfd, "sym", F_OK, 0) != -1)
    exit(133);
  if (errno != ELOOP)
    exit(134);
  if (unlinkat(dirfd, "sym", 0))
    exit(135);
  if (close(dirfd))
    exit(136);
  if (rmdir(dir))
    exit(137);
}

void test_regular_trailing_slash(void) {
  char dir[] = "/tmp/access_test.XXXXXX";
  if (!mkdtemp(dir))
    exit(140);
  int dirfd;
  if ((dirfd = open(dir, O_RDONLY)) == -1)
    exit(141);
  int fd;
  if ((fd = openat(dirfd, "reg", O_WRONLY | O_CREAT | O_TRUNC, 0644)) == -1)
    exit(142);
  if (close(fd))
    exit(143);
  if (faccessat(dirfd, "reg/", F_OK, 0) != -1)
    exit(144);
  if (errno != ENOTDIR)
    exit(145);
  if (unlinkat(dirfd, "reg", 0))
    exit(146);
  if (close(dirfd))
    exit(147);
  if (rmdir(dir))
    exit(148);
}

void test_file_in_readonlydir(void) {
  if (!getuid())
    return;
  char root[] = "/tmp/access_test.XXXXXX";
  if (!mkdtemp(root))
    exit(149);
  int rootfd;
  if ((rootfd = open(root, O_RDONLY)) == -1)
    exit(150);
  int fd;
  if ((fd = openat(rootfd, "reg", O_RDWR | O_CREAT | O_TRUNC, 0600)) == -1)
    exit(151);
  if (close(fd))
    exit(152);
  if (chmod(root, 0500))
    exit(153);
  // we CAN write to a file in a read-only directory
  if (faccessat(rootfd, "reg", W_OK, 0))
    exit(154);
  // we can NOT unlink a file in a read-only directory
  if (!unlinkat(rootfd, "reg", 0) || errno != EACCES)
    exit(155);
  // we can NOT rename a file within a read-only directory
  if (!renameat(rootfd, "reg", rootfd, "dog") || errno != EACCES)
    exit(156);
  char sibling[] = "/tmp/access_test.XXXXXX";
  if (!mkdtemp(sibling))
    exit(157);
  int siblingfd;
  if ((siblingfd = open(sibling, O_RDONLY)) == -1)
    exit(158);
  // we can NOT rename a file out of a read-only directory
  if (!renameat(rootfd, "reg", siblingfd, "reg") || errno != EACCES)
    exit(159);
  if (close(siblingfd))
    exit(160);
  if (rmdir(sibling))
    exit(161);
  if (chmod(root, 0700))
    exit(162);
  if (unlinkat(rootfd, "reg", 0))
    exit(163);
  if (close(rootfd))
    exit(164);
  if (rmdir(root))
    exit(165);
}

void test_unlink_readonly_file(void) {
  if (!getuid())
    return;
  char root[] = "/tmp/access_test.XXXXXX";
  if (!mkdtemp(root))
    exit(166);
  int rootfd;
  if ((rootfd = open(root, O_RDONLY)) == -1)
    exit(167);
  int fd;
  if ((fd = openat(rootfd, "reg", O_RDWR | O_CREAT | O_TRUNC, 0400)) == -1)
    exit(168);
  if (close(fd))
    exit(169);
  if (unlinkat(rootfd, "reg", 0))
    exit(170);
  if (close(rootfd))
    exit(171);
  if (rmdir(root))
    exit(172);
}

void test_file_in_dir_in_readonlydir(void) {
  if (!getuid())
    return;
  char root[] = "/tmp/access_test.XXXXXX";
  if (!mkdtemp(root))
    exit(173);
  int rootfd;
  if ((rootfd = open(root, O_RDONLY)) == -1)
    exit(174);
  int fd;
  if (mkdirat(rootfd, "dir", 0700))
    exit(175);
  int dirfd;
  if ((dirfd = openat(rootfd, "dir", O_RDONLY)) == -1)
    exit(176);
  if ((fd = openat(dirfd, "reg", O_RDWR | O_CREAT | O_TRUNC, 0600)) == -1)
    exit(175);
  if (close(fd))
    exit(152);
  if (chmod(root, 0500))
    exit(153);
  // we CAN write to a file in a read-only subdirectory
  if (faccessat(dirfd, "reg", W_OK, 0))
    exit(154);
  // we CAN rename a file within a read-only subdirectory
  if (renameat(dirfd, "reg", dirfd, "dog"))
    exit(156);
  // we CAN unlink a file in a read-only subdirectory
  if (unlinkat(dirfd, "dog", 0))
    exit(155);
  if (!unlinkat(rootfd, "dir", AT_REMOVEDIR) || errno != EACCES)
    exit(163);
  if (chmod(root, 0700))
    exit(162);
  if (unlinkat(rootfd, "dir", AT_REMOVEDIR))
    exit(163);
  if (close(rootfd))
    exit(164);
  if (rmdir(root))
    exit(165);
}

void test_link_to_readonly_file(void) {
  if (!getuid())
    return;
  char root[] = "/tmp/access_test.XXXXXX";
  if (!mkdtemp(root))
    exit(166);
  int rootfd;
  if ((rootfd = open(root, O_RDONLY)) == -1)
    exit(167);
  int fd;
  if ((fd = openat(rootfd, "reg", O_RDWR | O_CREAT | O_TRUNC, 0400)) == -1)
    exit(168);
  if (close(fd))
    exit(169);
  if (linkat(rootfd, "reg", rootfd, "linko", 0))
    exit(169);
  if (unlinkat(rootfd, "linko", 0))
    exit(170);
  if (unlinkat(rootfd, "reg", 0))
    exit(170);
  if (close(rootfd))
    exit(171);
  if (rmdir(root))
    exit(172);
}

int main(void) {
  test_regular();
  test_directory();
  test_symlink();
  test_symlinks_are_always_executable();
  test_dirfd();
  test_enotdir();
  test_readonlydir_fchmod();
  test_readonlydir_chmod();
  test_readonlydir_via_symlink();
  test_efault();
  test_eloop();
  test_regular_trailing_slash();
  test_file_in_readonlydir();
  test_unlink_readonly_file();
  test_file_in_dir_in_readonlydir();
  test_link_to_readonly_file();
}
