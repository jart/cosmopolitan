#ifndef IO_H
#define IO_H
#include "third_party/python/Modules/_decimal/libmpdec/mpdecimal.h"

#if SIZE_MAX == MPD_SIZE_MAX
  #define mpd_strtossize _mpd_strtossize
#else
static inline mpd_ssize_t
mpd_strtossize(const char *s, char **end, int base)
{
    int64_t retval;

    errno = 0;
    retval = _mpd_strtossize(s, end, base);
    if (errno == 0 && (retval > MPD_SSIZE_MAX || retval < MPD_SSIZE_MIN)) {
        errno = ERANGE;
    }
    if (errno == ERANGE) {
        return (retval < 0) ? MPD_SSIZE_MIN : MPD_SSIZE_MAX;
    }

    return (mpd_ssize_t)retval;
}
#endif

#endif
