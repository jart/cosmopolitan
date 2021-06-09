/* 2004-12-12 SMS.
 *
 * Emergency replacement UNIXIO.H for GNU C, for use as needed.
 * Install as GNU_CC_INCLUDE:[000000]UNIXIO.H
 */

#ifndef __UNIXIO_LOADED
#define __UNIXIO_LOADED 1

#include <sys/types.h>

#include <stdlib.h>

#ifndef SEEK_SET
# define SEEK_SET 0
#endif /* ndef SEEK_SET */

#ifndef SEEK_CUR
# define SEEK_CUR 1
#endif /* ndef SEEK_CUR */

#ifndef SEEK_END
# define SEEK_END 2
#endif /* ndef SEEK_END */

#endif  /* ndef __UNIXIO_LOADED */

