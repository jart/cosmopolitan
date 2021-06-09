/*
 * @(#) dir.h 1.4 87/11/06   Public Domain.
 *
 *  A public domain implementation of BSD directory routines for
 *  MS-DOS.  Written by Michael Rendell ({uunet,utai}michael@garfield),
 *  August 1987
 *
 *  Ported to OS/2 by Kai Uwe Rommel
 *  Addition of other OS/2 file system specific code
 *  Placed into the public domain
 */


#define MAXNAMLEN  256
#define MAXPATHLEN 256

#define _A_RONLY    0x01
#define _A_HIDDEN   0x02
#define _A_SYSTEM   0x04
#define _A_VOLID    0x08
#define _A_DIR      0x10
#define _A_ARCHIVE  0x20


struct dirent
{
  ino_t    d_ino;                   /* a bit of a farce */
  int      d_reclen;                /* more farce */
  int      d_namlen;                /* length of d_name */
  char     d_name[MAXNAMLEN + 1];   /* null terminated */
  /* nonstandard fields */
  long     d_size;                  /* size in bytes */
  unsigned d_mode;                  /* DOS or OS/2 file attributes */
  unsigned d_time;
  unsigned d_date;
};

/* The fields d_size and d_mode are extensions by me (Kai Uwe Rommel).
 * The find_first and find_next calls deliver this data without any extra cost.
 * If this data is needed, these fields save a lot of extra calls to stat()
 * (each stat() again performs a find_first call !).
 */

struct _dircontents
{
  char *_d_entry;
  long _d_size;
  unsigned _d_mode, _d_time, _d_date;
  struct _dircontents *_d_next;
};

typedef struct _dirdesc
{
  int  dd_id;                   /* uniquely identify each open directory */
  long dd_loc;                  /* where we are in directory entry is this */
  struct _dircontents *dd_contents;   /* pointer to contents of dir */
  struct _dircontents *dd_cp;         /* pointer to current position */
}
DIR;


extern DIR *opendir(const char *);
extern struct dirent *readdir(DIR *);
extern void seekdir(DIR *, long);
extern long telldir(DIR *);
extern void closedir(DIR *);
#define rewinddir(dirp) seekdir(dirp, 0L)

int GetFileMode(char *name);
ulg GetFileTime(char *name);
void SetFileTime(char *path, ulg stamp);
char *getVolumeLabel(int drive, unsigned long *time, unsigned long *mode,
                     time_t *utim);

int IsFileNameValid(char *name);
int IsFileSystemFAT(char *dir);
void ChangeNameForFAT(char *name);

char *GetLongNameEA(const char *name);
char *GetLongPathEA(const char *name);
void GetEAs(char *name, char **bufptr, size_t *size,
                        char **cbufptr, size_t *csize);

char *StringLower(char *);
