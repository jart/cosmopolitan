/* VMS functions
Copyright (C) 1996-2020 Free Software Foundation, Inc.
This file is part of GNU Make.

GNU Make is free software; you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation; either version 3 of the License, or (at your option) any later
version.

GNU Make is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program.  If not, see <http://www.gnu.org/licenses/>.  */

#include "makeint.h"
#include "debug.h"
#include "job.h"

#include <ctype.h>
#include <string.h>

#ifdef __DECC
#include <starlet.h>
#endif

#include <rms.h>
#include "vmsdir.h"

#ifdef HAVE_VMSDIR_H

DIR *
opendir (char *dspec)
{
  struct DIR *dir  = xcalloc (sizeof (struct DIR));
  struct NAM *dnam = xmalloc (sizeof (struct NAM));
  struct FAB *dfab = &dir->fab;
  char *searchspec = xmalloc (MAXNAMLEN + 1);

  *dfab = cc$rms_fab;
  *dnam = cc$rms_nam;
  sprintf (searchspec, "%s*.*;", dspec);

  dfab->fab$l_fna = searchspec;
  dfab->fab$b_fns = strlen (searchspec);
  dfab->fab$l_nam = dnam;

  *dnam = cc$rms_nam;
  dnam->nam$l_esa = searchspec;
  dnam->nam$b_ess = MAXNAMLEN;

  if (! (sys$parse (dfab) & 1))
    {
      free (dir);
      free (dnam);
      free (searchspec);
      return (NULL);
    }

  return dir;
}

#define uppercasify(str) \
  do \
    { \
      char *tmp; \
      for (tmp = (str); *tmp != '\0'; tmp++) \
        if (islower ((unsigned char)*tmp)) \
          *tmp = toupper ((unsigned char)*tmp); \
    } \
  while (0)

struct direct *
readdir (DIR *dir)
{
  struct FAB *dfab = &dir->fab;
  struct NAM *dnam = (struct NAM *)(dfab->fab$l_nam);
  struct direct *dentry = &dir->dir;
  int i;

  memset (dentry, 0, sizeof *dentry);

  dnam->nam$l_rsa = dir->d_result;
  dnam->nam$b_rss = MAXNAMLEN;

  DB (DB_VERBOSE, ("."));

  if (!((i = sys$search (dfab)) & 1))
    {
      DB (DB_VERBOSE, (_("sys$search() failed with %d\n"), i));
      return (NULL);
    }

  dentry->d_off = 0;
  if (dnam->nam$w_fid == 0)
    dentry->d_fileno = 1;
  else
    dentry->d_fileno = dnam->nam$w_fid[0] + (dnam->nam$w_fid[1] << 16);

  dentry->d_reclen = sizeof (struct direct);
  dentry->d_namlen = dnam->nam$b_name + dnam->nam$b_type;
  strncpy (dentry->d_name, dnam->nam$l_name, dentry->d_namlen);
  dentry->d_name[dentry->d_namlen] = '\0';

#ifdef HAVE_CASE_INSENSITIVE_FS
  uppercasify (dentry->d_name);
#endif

  return (dentry);
}

int
closedir (DIR *dir)
{
  if (dir != NULL)
    {
      struct FAB *dfab = &dir->fab;
      struct NAM *dnam = (struct NAM *)(dfab->fab$l_nam);
      if (dnam != NULL)
        free (dnam->nam$l_esa);
      free (dnam);
      free (dir);
    }

  return 0;
}
#endif /* compiled for OpenVMS prior to V7.x */

/* Argv0 will be a full vms file specification, like
   node$dka100:[utils.gnumake]make.exe;47
   prefix it with "mcr " to make it a vms command, executable for DCL. */
const char *
vms_command(const char* argv0)
{
  size_t l = strlen(argv0) + 1;
  char* s = xmalloc(l + 4);
  memcpy(s, "mcr ", 4);
  memcpy(s+4, argv0, l);
  return s;
}

/* Argv0 aka argv[0] will be a full vms file specification, like
   node$dka100:[utils.gnumake]make.exe;47, set up by the CRTL.
   The vms progname should be ^^^^, the file name without
   file type .exe and ;version.
   Use sys$parse to get the name part of the file specification. That is
   in the above example, pick up "make" and return a copy of that string.
   If something goes wrong in sys$parse (unlikely, this is a VMS/CRTL supplied
   file specification) or if there is an empty name part (not easy to produce,
   but it is possible) just return "make".
   Somes notes ...
   NAM[L]$M_SYNCHK requests a syntax check, only.
   NAM is for ODS2 names (shorter parts, output usually converted to UPPERCASE).
   NAML is for ODS2/ODS5 names (longer parts, output unchanged).
   NAM$M_NO_SHORT_UPCASE may not be available for older versions of VMS.
   NAML is not available on older versions of VMS (NAML$C_BID not defined).
   argv[0] on older versions of VMS (no extended parse style and no
   CRTL feature DECC$ARGV_PARSE_STYLE) is always in lowercase. */
const char *
vms_progname(const char* argv0)
{
  int status;
  static struct FAB fab;
  char *progname;
  const char *fallback = "make";

#ifdef NAML$C_BID
  static char esa[NAML$C_MAXRSS];
  static struct NAML naml;
#else
  static char esa[NAM$C_MAXRSS];
  static struct NAM nam;
#endif

  fab = cc$rms_fab;
  fab.fab$l_fna = (char*)argv0;
  fab.fab$b_fns = strlen(argv0);

#ifdef NAML$C_BID
  fab.fab$l_naml = &naml;
  naml = cc$rms_naml;
  naml.naml$l_long_expand = esa;
  naml.naml$l_long_expand_alloc = NAML$C_MAXRSS;
  naml.naml$b_nop = NAML$M_SYNCHK;
  naml.naml$l_input_flags = NAML$M_NO_SHORT_OUTPUT;
#else
  fab.fab$l_nam = &nam;
  nam = cc$rms_nam;
  nam.nam$l_esa = esa;
  nam.nam$b_ess = NAM$C_MAXRSS;
# ifdef NAM$M_NO_SHORT_UPCASE
  nam.nam$b_nop = NAM$M_SYNCHK | NAM$M_NO_SHORT_UPCASE;
# else
  nam.nam$b_nop = NAM$M_SYNCHK;
# endif
#endif

  status = sys$parse(&fab);
  if (!(status & 1))
    return fallback;

#ifdef NAML$C_BID
  if (naml.naml$l_long_name_size == 0)
    return fallback;
  progname = xmalloc(naml.naml$l_long_name_size + 1);
  memcpy(progname, naml.naml$l_long_name, naml.naml$l_long_name_size);
  progname[naml.naml$l_long_name_size] = '\0';
#else
  if (nam.nam$b_name == 0)
    return fallback;
  progname = xmalloc(nam.nam$b_name + 1);
# ifdef NAM$M_NO_SHORT_UPCASE
  memcpy(progname, nam.nam$l_name, nam.nam$b_name);
# else
    {
      int i;
      for (i = 0; i < nam.nam$b_name; i++)
        progname[i] = tolower(nam.nam$l_name[i]);
    }
# endif
  progname[nam.nam$b_name] = '\0';
#endif

  return progname;
}
