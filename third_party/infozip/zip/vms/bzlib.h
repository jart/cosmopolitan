/* 2007-01-13 SMS.
 * VMS-specific BZLIB.H jacket header file to ensure compatibility with
 * BZIP2 code compiled using /NAMES = AS_IS.
 *
 * The logical name INCL_BZIP2 must point to the BZIP2 source directory.
 *
 * A "names as_is" prototype for bz_internal_error() is included for the
 * same reason.  See bzip2 "bzlib_private.h".  Note that this "names
 * as_is" prototype must be the first to be read by the compiler, but
 * one or more other prototypes (perhaps with the default "names"
 * attributes) should cause no trouble.
 */

#pragma names save
#pragma names as_is

#include "INCL_BZIP2:BZLIB.H"

extern void bz_internal_error ( int errcode );

#pragma names restore
