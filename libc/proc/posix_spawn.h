#ifndef COSMOPOLITAN_LIBC_STDIO_SPAWN_H_
#define COSMOPOLITAN_LIBC_STDIO_SPAWN_H_
#include "libc/calls/struct/rlimit.h"
#include "libc/calls/struct/sched_param.h"
#include "libc/calls/struct/sigset.h"

#define POSIX_SPAWN_USEVFORK      0
#define POSIX_SPAWN_RESETIDS      1
#define POSIX_SPAWN_SETPGROUP     2
#define POSIX_SPAWN_SETSIGDEF     4
#define POSIX_SPAWN_SETSIGMASK    8
#define POSIX_SPAWN_SETSCHEDPARAM 16
#define POSIX_SPAWN_SETSCHEDULER  32
#define POSIX_SPAWN_SETSID        128
#define POSIX_SPAWN_SETRLIMIT     256

COSMOPOLITAN_C_START_

typedef struct _posix_spawna *posix_spawnattr_t;
typedef struct _posix_faction *posix_spawn_file_actions_t;

int posix_spawn(int *, const char *, const posix_spawn_file_actions_t *,
                const posix_spawnattr_t *, char *const[], char *const[]);
int posix_spawnp(int *, const char *, const posix_spawn_file_actions_t *,
                 const posix_spawnattr_t *, char *const[], char *const[]);

int posix_spawn_file_actions_init(posix_spawn_file_actions_t *) libcesque;
int posix_spawn_file_actions_destroy(posix_spawn_file_actions_t *) libcesque;
int posix_spawn_file_actions_addclose(posix_spawn_file_actions_t *,
                                      int) libcesque;
int posix_spawn_file_actions_adddup2(posix_spawn_file_actions_t *, int,
                                     int) libcesque;
int posix_spawn_file_actions_addopen(posix_spawn_file_actions_t *, int,
                                     const char *, int, unsigned) libcesque;
int posix_spawn_file_actions_addchdir_np(posix_spawn_file_actions_t *,
                                         const char *) libcesque;
int posix_spawn_file_actions_addfchdir_np(posix_spawn_file_actions_t *,
                                          int) libcesque;

int posix_spawnattr_init(posix_spawnattr_t *) libcesque;
int posix_spawnattr_destroy(posix_spawnattr_t *) libcesque;
int posix_spawnattr_getflags(const posix_spawnattr_t *, short *) libcesque;
int posix_spawnattr_setflags(posix_spawnattr_t *, short) libcesque;
int posix_spawnattr_getpgroup(const posix_spawnattr_t *, int *) libcesque;
int posix_spawnattr_setpgroup(posix_spawnattr_t *, int) libcesque;
int posix_spawnattr_getschedpolicy(const posix_spawnattr_t *, int *) libcesque;
int posix_spawnattr_setschedpolicy(posix_spawnattr_t *, int) libcesque;
int posix_spawnattr_getschedparam(const posix_spawnattr_t *,
                                  struct sched_param *) libcesque;
int posix_spawnattr_setschedparam(posix_spawnattr_t *,
                                  const struct sched_param *) libcesque;
int posix_spawnattr_getsigmask(const posix_spawnattr_t *, sigset_t *) libcesque;
int posix_spawnattr_setsigmask(posix_spawnattr_t *, const sigset_t *) libcesque;
int posix_spawnattr_getsigdefault(const posix_spawnattr_t *,
                                  sigset_t *) libcesque;
int posix_spawnattr_setsigdefault(posix_spawnattr_t *,
                                  const sigset_t *) libcesque;
int posix_spawnattr_getrlimit(const posix_spawnattr_t *, int,
                              struct rlimit *) libcesque;
int posix_spawnattr_setrlimit(posix_spawnattr_t *, int,
                              const struct rlimit *) libcesque;

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_STDIO_SPAWN_H_ */
