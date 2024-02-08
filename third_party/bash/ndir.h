/* <dir.h> -- definitions for 4.2BSD-compatible directory access.
   last edit:	09-Jul-1983	D A Gwyn. */

/* Size of directory block. */
#define DIRBLKSIZ 512

/* NOTE:  MAXNAMLEN must be one less than a multiple of 4 */

#if defined (VMS)
#  define MAXNAMLEN (DIR$S_NAME + 7)	/* 80 plus room for version #.  */
#  define MAXFULLSPEC NAM$C_MAXRSS	/* Maximum full spec */
#else
#  define MAXNAMLEN 15			/* Maximum filename length. */
#endif /* VMS */

/* Data from readdir (). */
struct direct {
  long d_ino;			/* Inode number of entry. */
  unsigned short d_reclen;	/* Length of this record. */
  unsigned short d_namlen;	/* Length of string in d_name. */
  char d_name[MAXNAMLEN + 1];	/* Name of file. */
};

/* Stream data from opendir (). */
typedef struct {
  int dd_fd;			/* File descriptor. */
  int dd_loc;			/* Offset in block. */
  int dd_size;			/* Amount of valid data. */
  char	dd_buf[DIRBLKSIZ];	/* Directory block. */
} DIR;

extern DIR *opendir ();
extern struct direct *readdir ();
extern long telldir ();
extern void seekdir (), closedir ();

#define rewinddir(dirp) seekdir (dirp, 0L)
