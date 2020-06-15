#ifndef COSMOPOLITAN_LIBC_PASSWD_H_
#define COSMOPOLITAN_LIBC_PASSWD_H_
#include "libc/calls/weirdtypes.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

struct FILE;

struct passwd {
  char *pw_name;
  char *pw_passwd;
  uint32_t pw_uid;
  uint32_t pw_gid;
  char *pw_gecos;
  char *pw_dir;
  char *pw_shell;
};

void setpwent(void);
void endpwent(void);
struct passwd *getpwent(void);
struct passwd *getpwuid(uint32_t);
struct passwd *getpwnam(const char *);
int getpwuid_r(uint32_t, struct passwd *, char *, size_t, struct passwd **);
int getpwnam_r(const char *, struct passwd *, char *, size_t, struct passwd **);
struct passwd *fgetpwent(struct FILE *);
int putpwent(const struct passwd *, struct FILE *);

struct group {
  char *gr_name;
  char *gr_passwd;
  int32_t gr_gid;
  char **gr_mem;
};

struct group *getgrgid(gid_t);
struct group *getgrnam(const char *);
int getgrgid_r(gid_t, struct group *, char *, size_t, struct group **);
int getgrnam_r(const char *, struct group *, char *, size_t, struct group **);
struct group *getgrent(void);
void endgrent(void);
void setgrent(void);
struct group *fgetgrent(struct FILE *);
int putgrent(const struct group *, struct FILE *);
int getgrouplist(const char *, gid_t, gid_t *, int *);
int setgroups(size_t, const int32_t *);
int initgroups(const char *, int32_t);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_PASSWD_H_ */
