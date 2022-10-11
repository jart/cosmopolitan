#ifndef COSMOPOLITAN_LIBC_CALLS_STRUCT_DIRENT_H_
#define COSMOPOLITAN_LIBC_CALLS_STRUCT_DIRENT_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

struct dirent {      /* linux getdents64 abi */
  uint64_t d_ino;    /* inode number */
  int64_t d_off;     /* implementation-dependent location number */
  uint16_t d_reclen; /* byte length of this whole struct and string */
  uint8_t d_type;    /* DT_REG, DT_DIR, DT_UNKNOWN, DT_BLK, etc. */
  char d_name[256];  /* NUL-terminated basename */
};

struct dirstream;
typedef struct dirstream DIR;

DIR *fdopendir(int) dontdiscard;
DIR *opendir(const char *) dontdiscard;
int closedir(DIR *);
int dirfd(DIR *);
long telldir(DIR *);
struct dirent *readdir(DIR *);
int readdir_r(DIR *, struct dirent *, struct dirent **);
void rewinddir(DIR *);
void seekdir(DIR *, long);
int alphasort(const struct dirent **, const struct dirent **);
int versionsort(const struct dirent **, const struct dirent **);
int scandir(const char *, struct dirent ***, int (*)(const struct dirent *),
            int (*)(const struct dirent **, const struct dirent **));

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_CALLS_STRUCT_DIRENT_H_ */
