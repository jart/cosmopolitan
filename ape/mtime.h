#ifndef COSMOPOLITAN_APE_MTIME_H_
#define COSMOPOLITAN_APE_MTIME_H_
#include "libc/dos.h"

/**
 * @fileoverview Deterministic last modified timestamp embedding.
 */

#ifndef MTIME_YEAR
#define MTIME_YEAR 2019
#endif

#ifndef MTIME_MONTH
#define MTIME_MONTH 1
#endif

#ifndef MTIME_DAY
#define MTIME_DAY 1
#endif

#ifndef MTIME_HOUR
#define MTIME_HOUR 0
#endif

#ifndef MTIME_MINUTES
#define MTIME_MINUTES 0
#endif

#ifndef MTIME_SECONDS
#define MTIME_SECONDS 0
#endif

#ifndef ZIP_MTIME_DATE
#define ZIP_MTIME_DATE DOS_DATE(MTIME_YEAR, MTIME_MONTH, MTIME_DAY)
#endif

#ifndef ZIP_MTIME_TIME
#define ZIP_MTIME_TIME DOS_TIME(MTIME_HOUR, MTIME_MINUTES, MTIME_SECONDS)
#endif

#endif /* COSMOPOLITAN_APE_MTIME_H_ */
