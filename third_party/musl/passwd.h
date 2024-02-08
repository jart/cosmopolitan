#ifndef COSMOPOLITAN_LIBC_PASSWD_H_
#define COSMOPOLITAN_LIBC_PASSWD_H_
#include "libc/stdio/stdio.h"
#include "libc/calls/weirdtypes.h"
COSMOPOLITAN_C_START_

struct passwd {
  char *pw_name;
  char *pw_passwd;
  uid_t pw_uid;
  gid_t pw_gid;
  char *pw_gecos;
  char *pw_dir;
  char *pw_shell;
};

void setpwent(void);
void endpwent(void);
struct passwd *getpwent(void);
struct passwd *getpwuid(uid_t);
struct passwd *getpwnam(const char *);
int getpwuid_r(uid_t, struct passwd *, char *, size_t, struct passwd **);
int getpwnam_r(const char *, struct passwd *, char *, size_t, struct passwd **);
struct passwd *fgetpwent(FILE *);
int putpwent(const struct passwd *, FILE *);

struct group {
  char *gr_name;
  char *gr_passwd;
  gid_t gr_gid;
  char **gr_mem;
};

struct group *getgrgid(gid_t);
struct group *getgrnam(const char *);
int getgrgid_r(gid_t, struct group *, char *, size_t, struct group **);
int getgrnam_r(const char *, struct group *, char *, size_t, struct group **);
struct group *getgrent(void);
void endgrent(void);
void setgrent(void);
struct group *fgetgrent(FILE *);
int putgrent(const struct group *, FILE *);
int getgrouplist(const char *, gid_t, gid_t *, int *);
int initgroups(const char *, gid_t);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_PASSWD_H_ */
