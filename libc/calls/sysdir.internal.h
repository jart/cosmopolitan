#ifndef COSMOPOLITAN_LIBC_CALLS_SYSDIR_H_
#define COSMOPOLITAN_LIBC_CALLS_SYSDIR_H_
COSMOPOLITAN_C_START_

const char *GetHostsTxtPath(char *, size_t);
const char *GetServicesTxtPath(char *, size_t);
const char *GetProtocolsTxtPath(char *, size_t);
char *GetSystemDirectoryPath(char *, const char *, size_t);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_CALLS_SYSDIR_H_ */
