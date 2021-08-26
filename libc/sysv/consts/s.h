#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_S_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_S_H_

#define S_IFIFO  0010000 /* pipe */
#define S_IFCHR  0020000 /* character device */
#define S_IFDIR  0040000 /* directory */
#define S_IFBLK  0060000 /* block device */
#define S_IFREG  0100000 /* regular file */
#define S_IFLNK  0120000 /* symbolic link */
#define S_IFSOCK 0140000 /* socket */
#define S_IFMT   0170000 /* mask of file types above */

#define S_ISVTX  0001000 /* THE STICKY BIT */
#define S_ISGID  0002000 /* the setgid bit */
#define S_ISUID  0004000 /* the setuid bit */
#define S_IXUSR  0000100 /* user  --x; just use octal */
#define S_IWUSR  0000200 /* user  -w-; just use octal */
#define S_IRUSR  0000400 /* user  r--; just use octal */
#define S_IRWXU  0000700 /* user  rwx; just use octal */
#define S_IXGRP  0000010 /* group --x; just use octal */
#define S_IWGRP  0000020 /* group -w-; just use octal */
#define S_IRGRP  0000040 /* group r--; just use octal */
#define S_IRWXG  0000070 /* group rwx; just use octal */
#define S_IXOTH  0000001 /* other --x; just use octal */
#define S_IWOTH  0000002 /* other -w-; just use octal */
#define S_IROTH  0000004 /* other r--; just use octal */
#define S_IRWXO  0000007 /* other rwx; just use octal */
#define S_IREAD  0000400 /* just use octal */
#define S_IEXEC  0000100 /* just use octal */
#define S_IWRITE 0000200 /* just use octal */

#endif /* COSMOPOLITAN_LIBC_SYSV_CONSTS_S_H_ */
