#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_S_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_S_H_

#define S_ISVTX  01000 /* THE STICKY BIT */
#define S_ISGID  02000 /* the setgid bit */
#define S_ISUID  04000 /* the setuid bit */
#define S_IXUSR  00100 /* user  --x; just use octal */
#define S_IWUSR  00200 /* user  -w-; just use octal */
#define S_IRUSR  00400 /* user  r--; just use octal */
#define S_IRWXU  00700 /* user  rwx; just use octal */
#define S_IXGRP  00010 /* group --x; just use octal */
#define S_IWGRP  00020 /* group -w-; just use octal */
#define S_IRGRP  00040 /* group r--; just use octal */
#define S_IRWXG  00070 /* group rwx; just use octal */
#define S_IXOTH  00001 /* other --x; just use octal */
#define S_IWOTH  00002 /* other -w-; just use octal */
#define S_IROTH  00004 /* other r--; just use octal */
#define S_IRWXO  00007 /* other rwx; just use octal */
#define S_IREAD  00400 /* just use octal */
#define S_IEXEC  00100 /* just use octal */
#define S_IWRITE 00200 /* just use octal */

#define S_IFIFO  (1 << 12)  /* pipe */
#define S_IFCHR  (2 << 12)  /* character device */
#define S_IFDIR  (4 << 12)  /* directory */
#define S_IFBLK  (6 << 12)  /* block device */
#define S_IFREG  (8 << 12)  /* regular file */
#define S_IFLNK  (10 << 12) /* symbolic link */
#define S_IFSOCK (12 << 12) /* socket */
#define S_IFMT   (15 << 12) /* mask of file types above */

#endif /* COSMOPOLITAN_LIBC_SYSV_CONSTS_S_H_ */
