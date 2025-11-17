#ifndef LIBBSD_STRING_H
#define LIBBSD_STRING_H
COSMOPOLITAN_C_START_

size_t strlcpy(char *, const char *, size_t);
size_t strlcat(char *, const char *, size_t);
void explicit_bzero(void *, size_t);

COSMOPOLITAN_C_END_
#endif /* LIBBSD_STRING_H */
