#ifndef COSMOPOLITAN_LIBC_CALLS_STRUCT_DIRENT_H_
#define COSMOPOLITAN_LIBC_CALLS_STRUCT_DIRENT_H_
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

DIR *fdopendir(int) libcesque __wur;
DIR *opendir(const char *) libcesque __wur;
int closedir(DIR *) libcesque;
int dirfd(DIR *) libcesque;
long telldir(DIR *) libcesque;
struct dirent *readdir(DIR *) libcesque;
int readdir_r(DIR *, struct dirent *, struct dirent **) libcesque;
void rewinddir(DIR *) libcesque;
void seekdir(DIR *, long) libcesque;
int alphasort(const struct dirent **, const struct dirent **) libcesque;
int versionsort(const struct dirent **, const struct dirent **) libcesque;
int scandir(const char *, struct dirent ***, int (*)(const struct dirent *),
            int (*)(const struct dirent **, const struct dirent **)) libcesque;

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_CALLS_STRUCT_DIRENT_H_ */
