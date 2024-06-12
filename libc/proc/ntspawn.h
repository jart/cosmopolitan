#ifndef COSMOPOLITAN_NTSPAWN_H_
#define COSMOPOLITAN_NTSPAWN_H_
#include "libc/nt/struct/processinformation.h"
#include "libc/nt/struct/startupinfo.h"
COSMOPOLITAN_C_START_

struct NtSpawnArgs {
  int64_t dirhand;
  const char *prog;
  char *const *argv;
  char *const *envp;
  char *const *extravars;
  uint32_t dwCreationFlags;
  const char16_t *opt_lpCurrentDirectory;
  int64_t opt_hParentProcess;
  int64_t *opt_lpExplicitHandleList;
  uint32_t dwExplicitHandleCount;
  const struct NtStartupInfo *lpStartupInfo;
  struct NtProcessInformation *opt_out_lpProcessInformation;
};

int mkntenvblock(char16_t[32767], char *const[], char *const[], char[32767]);
int ntspawn(struct NtSpawnArgs *);
size_t mkntcmdline(char16_t *, char *const[], size_t);
void mungentpath(char *);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_NTSPAWN_H_ */
