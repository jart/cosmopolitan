/* Library function for scanning an archive file.
Copyright (C) 1987-2023 Free Software Foundation, Inc.
This file is part of GNU Make.

GNU Make is free software; you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation; either version 3 of the License, or (at your option) any later
version.

GNU Make is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program.  If not, see <https://www.gnu.org/licenses/>.  */

#include "makeint.h"

#ifdef TEST
/* Hack, the real error() routine eventually pulls in die from main.c */
#define error(a, b, c, d)
#endif

#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#else
#include <sys/file.h>
#endif

#ifndef NO_ARCHIVES

#ifdef VMS
#include <lbrdef.h>
#include <mhddef.h>
#include <credef.h>
#include <descrip.h>
#include <ctype.h>
#include <ssdef.h>
#include <stsdef.h>
#include <rmsdef.h>

/* This symbol should be present in lbrdef.h. */
#if !defined LBR$_HDRTRUNC
#pragma extern_model save
#pragma extern_model globalvalue
extern unsigned int LBR$_HDRTRUNC;
#pragma extern_model restore
#endif

#include <unixlib.h>
#include <lbr$routines.h>

const char *
vmsify (const char *name, int type);

/* Time conversion from VMS to Unix
   Conversion from local time (stored in library) to GMT (needed for gmake)
   Note: The tm_gmtoff element is a VMS extension to the ANSI standard. */
static time_t
vms_time_to_unix(void *vms_time)
{
  struct tm *tmp;
  time_t unix_time;

  unix_time = decc$fix_time(vms_time);
  tmp = localtime(&unix_time);
  unix_time -= tmp->tm_gmtoff;

  return unix_time;
}


/* VMS library routines need static variables for callback */
static void *VMS_lib_idx;

static const void *VMS_saved_arg;

static intmax_t (*VMS_function) ();

static intmax_t VMS_function_ret;


/* This is a callback procedure for lib$get_index */
static int
VMS_get_member_info(struct dsc$descriptor_s *module, unsigned long *rfa)
{
  int status, i;
  const int truncated = 0; /* Member name may be truncated */
  time_t member_date; /* Member date */
  char *filename;
  unsigned int buffer_length; /* Actual buffer length */

  /* Unused constants - Make does not actually use most of these */
  const int file_desc = -1; /* archive file descriptor for reading the data */
  const int header_position = 0; /* Header position */
  const int data_position = 0; /* Data position in file */
  const int data_size = 0; /* Data size */
  const int uid = 0; /* member gid */
  const int gid = 0; /* member gid */
  const int mode = 0; /* member protection mode */
  /* End of unused constants */

  static struct dsc$descriptor_s bufdesc =
    { 0, DSC$K_DTYPE_T, DSC$K_CLASS_S, NULL };

  /* Only need the module definition */
  struct mhddef *mhd;

  /* If a previous callback is non-zero, just return that status */
  if (VMS_function_ret)
    {
      return SS$_NORMAL;
    }

  /* lbr_set_module returns more than just the module header. So allocate
     a buffer which is big enough: the maximum LBR$C_MAXHDRSIZ. That's at
     least bigger than the size of struct mhddef.
     If the request is too small, a buffer truncated warning is issued so
     it can be reissued with a larger buffer.
     We do not care if the buffer is truncated, so that is still a success. */
  mhd = xmalloc(LBR$C_MAXHDRSIZ);
  bufdesc.dsc$a_pointer = (char *) mhd;
  bufdesc.dsc$w_length = LBR$C_MAXHDRSIZ;

  status = lbr$set_module(&VMS_lib_idx, rfa, &bufdesc, &buffer_length, 0);

  if ((status != LBR$_HDRTRUNC) && !$VMS_STATUS_SUCCESS(status))
    {
      ON(error, NILF,
          _("lbr$set_module() failed to extract module info, status = %d"),
          status);

      lbr$close(&VMS_lib_idx);

      return status;
    }

#ifdef TEST
  /* When testing this code, it is useful to know the length returned */
  printf ("Input length = %d, actual = %u\n",
          bufdesc.dsc$w_length, buffer_length);
#endif

  /* Conversion from VMS time to C time.
     VMS defectlet - mhddef is sub-optimal, for the time, it has a 32 bit
     longword, mhd$l_datim, and a 32 bit fill instead of two longwords, or
     equivalent. */
  member_date = vms_time_to_unix(&mhd->mhd$l_datim);
  free(mhd);

  /* Here we have a problem.  The module name on VMS does not have
     a file type, but the filename pattern in the "VMS_saved_arg"
     may have one.
     But only the method being called knows how to interpret the
     filename pattern.
     There are currently two different formats being used.
     This means that we need a VMS specific code in those methods
     to handle it. */
  filename = xmalloc(module->dsc$w_length + 1);

  /* TODO: We may need an option to preserve the case of the module
     For now force the module name to lower case */
  for (i = 0; i < module->dsc$w_length; i++)
    filename[i] = _tolower((unsigned char )module->dsc$a_pointer[i]);

  filename[i] = '\0';

  VMS_function_ret = (*VMS_function)(file_desc, filename, truncated,
      header_position, data_position, data_size, member_date, uid, gid, mode,
      VMS_saved_arg);

  free(filename);
  return SS$_NORMAL;
}


/* Takes three arguments ARCHIVE, FUNCTION and ARG.

   Open the archive named ARCHIVE, find its members one by one,
   and for each one call FUNCTION with the following arguments:
     archive file descriptor for reading the data,
     member name,
     member name might be truncated flag,
     member header position in file,
     member data position in file,
     member data size,
     member date,
     member uid,
     member gid,
     member protection mode,
     ARG.

   NOTE: on VMS systems, only name, date, and arg are meaningful!

   The descriptor is poised to read the data of the member
   when FUNCTION is called.  It does not matter how much
   data FUNCTION reads.

   If FUNCTION returns nonzero, we immediately return
   what FUNCTION returned.

   Returns -1 if archive does not exist,
   Returns -2 if archive has invalid format.
   Returns 0 if have scanned successfully.  */

intmax_t
ar_scan (const char *archive, ar_member_func_t function, const void *varg)
{
  char *vms_archive;

  static struct dsc$descriptor_s libdesc =
    { 0, DSC$K_DTYPE_T, DSC$K_CLASS_S, NULL };

  const unsigned long func = LBR$C_READ;
  const unsigned long type = LBR$C_TYP_UNK;
  const unsigned long index = 1;
  unsigned long lib_idx;
  int status;

  VMS_saved_arg = varg;

  /* Null archive string can show up in test and cause an access violation */
  if (archive == NULL)
    {
      /* Null filenames do not exist */
      return -1;
    }

  /* archive path name must be in VMS format */
  vms_archive = (char *) vmsify(archive, 0);

  status = lbr$ini_control(&VMS_lib_idx, &func, &type, 0);

  if (!$VMS_STATUS_SUCCESS(status))
    {
      ON(error, NILF, _("lbr$ini_control() failed with status = %d"), status);
      return -2;
    }

  libdesc.dsc$a_pointer = vms_archive;
  libdesc.dsc$w_length = strlen(vms_archive);

  status = lbr$open(&VMS_lib_idx, &libdesc, 0, NULL, 0, NULL, 0);

  if (!$VMS_STATUS_SUCCESS(status))
    {

      /* TODO: A library format failure could mean that this is a file
         generated by the GNU AR utility and in that case, we need to
         take the UNIX codepath.  This will also take a change to the
         GNV AR wrapper program. */

      switch (status)
        {
      case RMS$_FNF:
        /* Archive does not exist */
        return -1;
      default:
#ifndef TEST
        OSN(error, NILF,
            _("unable to open library '%s' to lookup member status %d"),
            archive, status);
#endif
        /* For library format errors, specification says to return -2 */
        return -2;
        }
    }

  VMS_function = function;

  /* Clear the return status, as we are supposed to stop calling the
     callback function if it becomes non-zero, and this is a static
     variable. */
  VMS_function_ret = 0;

  status = lbr$get_index(&VMS_lib_idx, &index, VMS_get_member_info, NULL, 0);

  lbr$close(&VMS_lib_idx);

  /* Unless a failure occurred in the lbr$ routines, return the
     the status from the 'function' routine. */
  if ($VMS_STATUS_SUCCESS(status))
    {
      return VMS_function_ret;
    }

  /* This must be something wrong with the library and an error
     message should already have been printed. */
  return -2;
}

#else /* !VMS */

/* SCO Unix's compiler defines both of these.  */
#ifdef  M_UNIX
#undef  M_XENIX
#endif

/* On the sun386i and in System V rel 3, ar.h defines two different archive
   formats depending upon whether you have defined PORTAR (normal) or PORT5AR
   (System V Release 1).  There is no default, one or the other must be defined
   to have a nonzero value.  */

#if (!defined (PORTAR) || PORTAR == 0) && (!defined (PORT5AR) || PORT5AR == 0)
#undef  PORTAR
#ifdef M_XENIX
/* According to Jim Sievert <jas1@rsvl.unisys.com>, for SCO XENIX defining
   PORTAR to 1 gets the wrong archive format, and defining it to 0 gets the
   right one.  */
#define PORTAR 0
#else
#define PORTAR 1
#endif
#endif

/* On AIX, define these symbols to be sure to get both archive formats.
   AIX 4.3 introduced the "big" archive format to support 64-bit object
   files, so on AIX 4.3 systems we need to support both the "normal" and
   "big" archive formats.  An archive's format is indicated in the
   "fl_magic" field of the "FL_HDR" structure.  For a normal archive,
   this field will be the string defined by the AIAMAG symbol.  For a
   "big" archive, it will be the string defined by the AIAMAGBIG symbol
   (at least on AIX it works this way).

   Note: we'll define these symbols regardless of which AIX version
   we're compiling on, but this is okay since we'll use the new symbols
   only if they're present.  */
#ifdef _AIX
# define __AR_SMALL__
# define __AR_BIG__
#endif

#ifndef WINDOWS32
# if !defined (__ANDROID__) && !defined (__BEOS__) && !defined(MK_OS_ZOS)
#  include <ar.h>
# else
   /* These platforms don't have <ar.h> but have archives in the same format
    * as many other Unices.  This was taken from GNU binutils for BeOS.
    */
#  define ARMAG "!<arch>\n"     /* String that begins an archive file.  */
#  define SARMAG 8              /* Size of that string.  */
#  define ARFMAG "`\n"          /* String in ar_fmag at end of each header.  */
struct ar_hdr
  {
    char ar_name[16];           /* Member file name, sometimes / terminated. */
    char ar_date[12];           /* File date, decimal seconds since Epoch.  */
    char ar_uid[6], ar_gid[6];  /* User and group IDs, in ASCII decimal.  */
    char ar_mode[8];            /* File mode, in ASCII octal.  */
    char ar_size[10];           /* File size, in ASCII decimal.  */
    char ar_fmag[2];            /* Always contains ARFMAG.  */
  };
# endif
# define TOCHAR(_m)     (_m)
#else
/* These should allow us to read Windows (VC++) libraries (according to Frank
 * Libbrecht <frankl@abzx.belgium.hp.com>)
 */
# include <windows.h>
# include <windef.h>
# include <io.h>
# define ARMAG      IMAGE_ARCHIVE_START
# define SARMAG     IMAGE_ARCHIVE_START_SIZE
# define ar_hdr     _IMAGE_ARCHIVE_MEMBER_HEADER
# define ar_name    Name
# define ar_mode    Mode
# define ar_size    Size
# define ar_date    Date
# define ar_uid     UserID
# define ar_gid     GroupID
/* In Windows the member names have type BYTE so we must cast them.  */
# define TOCHAR(_m)     ((char *)(_m))
#endif

/* Cray's <ar.h> apparently defines this.  */
#ifndef AR_HDR_SIZE
# define   AR_HDR_SIZE  (sizeof (struct ar_hdr))
#endif

#include "intprops.h"

#include "output.h"


static uintmax_t
parse_int (const char *ptr, const size_t len, const int base, uintmax_t max,
           const char *type, const char *archive, const char *name)
{
  const char *const ep = ptr + len;
  const int maxchar = '0' + base - 1;
  uintmax_t val = 0;

  /* In all the versions I know of the spaces come last, but be safe.  */
  while (ptr < ep && *ptr == ' ')
    ++ptr;

  while (ptr < ep && *ptr != ' ')
    {
      uintmax_t nv;

      if (*ptr < '0' || *ptr > maxchar)
        OSSS (fatal, NILF,
              _("Invalid %s for archive %s member %s"), type, archive, name);
      nv = (val * base) + (*ptr - '0');
      if (nv < val || nv > max)
        OSSS (fatal, NILF,
              _("Invalid %s for archive %s member %s"), type, archive, name);
      val = nv;
      ++ptr;
    }

  return val;
}

/* Takes three arguments ARCHIVE, FUNCTION and ARG.

   Open the archive named ARCHIVE, find its members one by one,
   and for each one call FUNCTION with the following arguments:
     archive file descriptor for reading the data,
     member name,
     member name might be truncated flag,
     member header position in file,
     member data position in file,
     member data size,
     member date,
     member uid,
     member gid,
     member protection mode,
     ARG.

   The descriptor is poised to read the data of the member
   when FUNCTION is called.  It does not matter how much
   data FUNCTION reads.

   If FUNCTION returns nonzero, we immediately return
   what FUNCTION returned.

   Returns -1 if archive does not exist,
   Returns -2 if archive has invalid format.
   Returns 0 if have scanned successfully.  */

intmax_t
ar_scan (const char *archive, ar_member_func_t function, const void *arg)
{
#ifdef AIAMAG
  FL_HDR fl_header;
# ifdef AIAMAGBIG
  int big_archive = 0;
  FL_HDR_BIG fl_header_big;
# endif
#endif
  char *namemap = 0;
  unsigned int namemap_size = 0;
  int desc = open (archive, O_RDONLY, 0);
  if (desc < 0)
    return -1;

#ifdef SARMAG
  {
    char buf[SARMAG];
    int nread;
    nread = readbuf (desc, buf, SARMAG);
    if (nread != SARMAG || memcmp (buf, ARMAG, SARMAG))
      goto invalid;
  }
#else
#ifdef AIAMAG
  {
    int nread;
    nread = readbuf (desc, &fl_header, FL_HSZ);
    if (nread != FL_HSZ)
      goto invalid;

#ifdef AIAMAGBIG
    /* If this is a "big" archive, then set the flag and
       re-read the header into the "big" structure. */
    if (!memcmp (fl_header.fl_magic, AIAMAGBIG, SAIAMAG))
      {
        off_t o;

        big_archive = 1;

        /* seek back to beginning of archive */
        EINTRLOOP (o, lseek (desc, 0, 0));
        if (o < 0)
          goto invalid;

        /* re-read the header into the "big" structure */
        nread = readbuf (desc, &fl_header_big, FL_HSZ_BIG);
        if (nread != FL_HSZ_BIG)
          goto invalid;
      }
    else
#endif
       /* Check to make sure this is a "normal" archive. */
      if (memcmp (fl_header.fl_magic, AIAMAG, SAIAMAG))
        goto invalid;
  }
#else
  {
#ifndef M_XENIX
    int buf;
#else
    unsigned short int buf;
#endif
    int nread;
    nread = readbuf (desc, &buf, sizeof (buf));
    if (nread != sizeof (buf) || buf != ARMAG)
      goto invalid;
  }
#endif
#endif

  /* Now find the members one by one.  */
  {
#ifdef SARMAG
    long int member_offset = SARMAG;
#else
#ifdef AIAMAG
    long int member_offset;
    long int last_member_offset;

#ifdef AIAMAGBIG
    if ( big_archive )
      {
        sscanf (fl_header_big.fl_fstmoff, "%20ld", &member_offset);
        sscanf (fl_header_big.fl_lstmoff, "%20ld", &last_member_offset);
      }
    else
#endif
      {
        sscanf (fl_header.fl_fstmoff, "%12ld", &member_offset);
        sscanf (fl_header.fl_lstmoff, "%12ld", &last_member_offset);
      }

    if (member_offset == 0)
      {
        /* Empty archive.  */
        close (desc);
        return 0;
      }
#else
#ifndef M_XENIX
    long int member_offset = sizeof (int);
#else   /* Xenix.  */
    long int member_offset = sizeof (unsigned short int);
#endif  /* Not Xenix.  */
#endif
#endif

    while (1)
      {
        ssize_t nread;
        struct ar_hdr member_header;
#ifdef AIAMAGBIG
        struct ar_hdr_big member_header_big;
#endif
#ifdef AIAMAG
# define ARNAME_MAX 255
        char name[ARNAME_MAX + 1];
        int name_len;
        intmax_t dateval;
        int uidval, gidval;
        long int data_offset;
#else
# define ARNAME_MAX (int)sizeof(member_header.ar_name)
        char namebuf[ARNAME_MAX + 1];
        char *name;
        int is_namemap;         /* Nonzero if this entry maps long names.  */
        int long_name = 0;
#endif
        long int eltsize;
        unsigned int eltmode;
        intmax_t eltdate;
        int eltuid, eltgid;
        intmax_t fnval;
        off_t o;

        memset(&member_header, '\0', sizeof (member_header));

        EINTRLOOP (o, lseek (desc, member_offset, 0));
        if (o < 0)
          goto invalid;

#ifdef AIAMAG
#define       AR_MEMHDR_SZ(x) (sizeof(x) - sizeof (x._ar_name))

#ifdef AIAMAGBIG
        if (big_archive)
          {
            nread = readbuf (desc, &member_header_big,
                             AR_MEMHDR_SZ(member_header_big));

            if (nread != AR_MEMHDR_SZ(member_header_big))
              goto invalid;

            sscanf (member_header_big.ar_namlen, "%4d", &name_len);
            if (name_len < 1 || name_len > ARNAME_MAX)
              goto invalid;

            nread = readbuf (desc, name, name_len);
            if (nread != name_len)
              goto invalid;

            name[name_len] = '\0';

            sscanf (member_header_big.ar_date, "%12" SCNdMAX, &dateval);
            sscanf (member_header_big.ar_uid, "%12d", &uidval);
            sscanf (member_header_big.ar_gid, "%12d", &gidval);
            sscanf (member_header_big.ar_mode, "%12o", &eltmode);
            sscanf (member_header_big.ar_size, "%20ld", &eltsize);

            data_offset = (member_offset + AR_MEMHDR_SZ(member_header_big)
                           + name_len + 2);
          }
        else
#endif
          {
            nread = readbuf (desc, &member_header,
                             AR_MEMHDR_SZ(member_header));

            if (nread != AR_MEMHDR_SZ(member_header))
              goto invalid;

            sscanf (member_header.ar_namlen, "%4d", &name_len);
            if (name_len < 1 || name_len > ARNAME_MAX)
              goto invalid;

            nread = readbuf (desc, name, name_len);
            if (nread != name_len)
              goto invalid;

            name[name_len] = '\0';

            sscanf (member_header.ar_date, "%12" SCNdMAX, &dateval);
            sscanf (member_header.ar_uid, "%12d", &uidval);
            sscanf (member_header.ar_gid, "%12d", &gidval);
            sscanf (member_header.ar_mode, "%12o", &eltmode);
            sscanf (member_header.ar_size, "%12ld", &eltsize);

            data_offset = (member_offset + AR_MEMHDR_SZ(member_header)
                           + name_len + 2);
          }
        data_offset += data_offset % 2;

        fnval =
          (*function) (desc, name, 0,
                       member_offset, data_offset, eltsize,
                       dateval, uidval, gidval,
                       eltmode, arg);

#else   /* Not AIAMAG.  */
        nread = readbuf (desc, &member_header, AR_HDR_SIZE);
        if (nread == 0)
          /* No data left means end of file; that is OK.  */
          break;

        if (nread != AR_HDR_SIZE
#if defined(ARFMAG) || defined(ARFZMAG)
            || (
# ifdef ARFMAG
                memcmp (member_header.ar_fmag, ARFMAG, 2)
# else
                1
# endif
                &&
# ifdef ARFZMAG
                memcmp (member_header.ar_fmag, ARFZMAG, 2)
# else
                1
# endif
               )
#endif
            )
          goto invalid;

        name = namebuf;
        memcpy (name, member_header.ar_name, sizeof member_header.ar_name);
        {
          char *p = name + sizeof member_header.ar_name;
          do
            *p = '\0';
          while (p > name && *--p == ' ');

#ifndef AIAMAG
          /* If the member name is "//" or "ARFILENAMES/" this may be
             a list of file name mappings.  The maximum file name
             length supported by the standard archive format is 14
             characters.  This member will actually always be the
             first or second entry in the archive, but we don't check
             that.  */
          is_namemap = (!strcmp (name, "//")
                        || !strcmp (name, "ARFILENAMES/"));
#endif  /* Not AIAMAG. */

          /* On some systems, there is a slash after each member name.  */
          if (*p == '/')
            *p = '\0';

#ifndef AIAMAG
          /* If the member name starts with a space or a slash, this
             is an index into the file name mappings (used by GNU ar).
             Otherwise if the member name looks like #1/NUMBER the
             real member name appears in the element data (used by
             4.4BSD).  */
          if (! is_namemap
              && (name[0] == ' ' || name[0] == '/')
              && namemap != 0)
            {
              const char* err;
              unsigned int name_off = make_toui (name + 1, &err);
              size_t name_len;

              if (err|| name_off >= namemap_size)
                goto invalid;

              name = namemap + name_off;
              name_len = strlen (name);
              if (name_len < 1)
                goto invalid;
              long_name = 1;
            }
          else if (name[0] == '#'
                   && name[1] == '1'
                   && name[2] == '/')
            {
              const char* err;
              unsigned int name_len = make_toui (name + 3, &err);

              if (err || name_len == 0 || name_len >= MIN (PATH_MAX, INT_MAX))
                goto invalid;

              name = alloca (name_len + 1);
              nread = readbuf (desc, name, name_len);
              if (nread < 0 || (unsigned int) nread != name_len)
                goto invalid;

              name[name_len] = '\0';

              long_name = 1;
            }
#endif /* Not AIAMAG. */
        }

#ifndef M_XENIX
#define PARSE_INT(_m, _t, _b, _n) \
        (_t) parse_int (TOCHAR (member_header._m), sizeof (member_header._m), \
                        _b, TYPE_MAXIMUM (_t), _n, archive, name)

        eltmode = PARSE_INT (ar_mode, unsigned int, 8, "mode");
        eltsize = PARSE_INT (ar_size, long, 10, "size");
        eltdate = PARSE_INT (ar_date, intmax_t, 10, "date");
        eltuid = PARSE_INT (ar_uid, int, 10, "uid");
        eltgid = PARSE_INT (ar_gid, int, 10, "gid");
#undef PARSE_INT
#else   /* Xenix.  */
        eltmode = (unsigned short int) member_header.ar_mode;
        eltsize = member_header.ar_size;
#endif  /* Not Xenix.  */

        fnval =
          (*function) (desc, name, ! long_name, member_offset,
                       member_offset + AR_HDR_SIZE, eltsize,
#ifndef M_XENIX
                       eltdate, eltuid, eltgid,
#else   /* Xenix.  */
                       member_header.ar_date,
                       member_header.ar_uid,
                       member_header.ar_gid,
#endif  /* Not Xenix.  */
                       eltmode, arg);

#endif  /* AIAMAG.  */

        if (fnval)
          {
            (void) close (desc);
            return fnval;
          }

#ifdef AIAMAG
        if (member_offset == last_member_offset)
          /* End of the chain.  */
          break;

#ifdef AIAMAGBIG
        if (big_archive)
         sscanf (member_header_big.ar_nxtmem, "%20ld", &member_offset);
        else
#endif
          sscanf (member_header.ar_nxtmem, "%12ld", &member_offset);

        if (lseek (desc, member_offset, 0) != member_offset)
          goto invalid;
#else

        /* If this member maps archive names, we must read it in.  The
           name map will always precede any members whose names must
           be mapped.  */
        if (is_namemap)
          {
            char *clear;
            char *limit;

            if (eltsize > INT_MAX)
              goto invalid;
            namemap = alloca (eltsize + 1);
            nread = readbuf (desc, namemap, eltsize);
            if (nread != eltsize)
              goto invalid;
            namemap_size = eltsize;

            /* The names are separated by newlines.  Some formats have
               a trailing slash.  Null terminate the strings for
               convenience.  */
            limit = namemap + eltsize;
            for (clear = namemap; clear < limit; clear++)
              {
                if (*clear == '\n')
                  {
                    *clear = '\0';
                    if (clear[-1] == '/')
                      clear[-1] = '\0';
                  }
              }
            *limit = '\0';

            is_namemap = 0;
          }

        member_offset += AR_HDR_SIZE + eltsize;
        if (member_offset % 2 != 0)
          member_offset++;
#endif
      }
  }

  close (desc);
  return 0;

 invalid:
  close (desc);
  return -2;
}
#endif /* !VMS */

/* Return nonzero iff NAME matches MEM.
   If TRUNCATED is nonzero, MEM may be truncated to
   sizeof (struct ar_hdr.ar_name) - 1.  */

int
ar_name_equal (const char *name, const char *mem, int truncated)
{
  const char *p;

  p = strrchr (name, '/');
  if (p != 0)
    name = p + 1;

#ifndef VMS
  if (truncated)
    {
#ifdef AIAMAG
      /* TRUNCATED should never be set on this system.  */
      abort ();
#else
      struct ar_hdr hdr;
#if !defined (__hpux) && !defined (cray)
      return strneq (name, mem, sizeof (hdr.ar_name) - 1);
#else
      return strneq (name, mem, sizeof (hdr.ar_name) - 2);
#endif /* !__hpux && !cray */
#endif /* !AIAMAG */
    }

  return !strcmp (name, mem);
#else
  /* VMS members do not have suffixes, but the filenames usually
     have.
     Do we need to strip VMS disk/directory format paths?

     Most VMS compilers etc. by default are case insensitive
     but produce uppercase external names, incl. module names.
     However the VMS librarian (ar) and the linker by default
     are case sensitive: they take what they get, usually
     uppercase names. So for the non-default settings of the
     compilers etc. there is a need to have a case sensitive
     mode. */
  {
    int len;
    len = strlen(mem);
    int match;
    char *dot;
    if ((dot=strrchr(name,'.')))
      match = (len == dot - name) && !strncasecmp(name, mem, len);
    else
      match = !strcasecmp (name, mem);
    return match;
  }
#endif /* !VMS */
}

#ifndef VMS
/* ARGSUSED */
static intmax_t
ar_member_pos (int desc UNUSED, const char *mem, int truncated,
               long int hdrpos, long int datapos UNUSED, long int size UNUSED,
               intmax_t date UNUSED, int uid UNUSED, int gid UNUSED,
               unsigned int mode UNUSED, const void *name)
{
  if (!ar_name_equal (name, mem, truncated))
    return 0;
  return hdrpos;
}

/* Set date of member MEMNAME in archive ARNAME to current time.
   Returns 0 if successful,
   -1 if file ARNAME does not exist,
   -2 if not a valid archive,
   -3 if other random system call error (including file read-only),
   1 if valid but member MEMNAME does not exist.  */

int
ar_member_touch (const char *arname, const char *memname)
{
  intmax_t pos = ar_scan (arname, ar_member_pos, memname);
  off_t opos;
  int fd;
  struct ar_hdr ar_hdr;
  off_t o;
  int r;
  int datelen;
  struct stat statbuf;

  if (pos < 0)
    return (int) pos;
  if (!pos)
    return 1;

  opos = (off_t) pos;

  EINTRLOOP (fd, open (arname, O_RDWR, 0666));
  if (fd < 0)
    return -3;
  /* Read in this member's header */
  EINTRLOOP (o, lseek (fd, opos, 0));
  if (o < 0)
    goto lose;
  r = readbuf (fd, &ar_hdr, AR_HDR_SIZE);
  if (r != AR_HDR_SIZE)
    goto lose;
  /* The file's mtime is the time we we want.  */
  EINTRLOOP (r, fstat (fd, &statbuf));
  if (r < 0)
    goto lose;
  /* Advance member's time to that time */
#if defined(ARFMAG) || defined(ARFZMAG) || defined(AIAMAG) || defined(WINDOWS32)
  datelen = snprintf (TOCHAR (ar_hdr.ar_date), sizeof ar_hdr.ar_date,
                      "%" PRIdMAX, (intmax_t) statbuf.st_mtime);
  if (! (0 <= datelen && datelen < (int) sizeof ar_hdr.ar_date))
    goto lose;
  memset (ar_hdr.ar_date + datelen, ' ', sizeof ar_hdr.ar_date - datelen);
#else
  ar_hdr.ar_date = statbuf.st_mtime;
#endif
  /* Write back this member's header */
  EINTRLOOP (o, lseek (fd, opos, 0));
  if (o < 0)
    goto lose;
  r = writebuf (fd, &ar_hdr, AR_HDR_SIZE);
  if (r != AR_HDR_SIZE)
    goto lose;
  close (fd);
  return 0;

 lose:
  r = errno;
  close (fd);
  errno = r;
  return -3;
}
#endif

#ifdef TEST

intmax_t
describe_member (int desc, const char *name, int truncated,
                 long int hdrpos, long int datapos, long int size,
                 intmax_t date, int uid, int gid, unsigned int mode,
                 const void *arg)
{
  extern char *ctime ();
  time_t d = date;
  char const *ds;

  printf (_("Member '%s'%s: %ld bytes at %ld (%ld).\n"),
          name, truncated ? _(" (name might be truncated)") : "",
          size, hdrpos, datapos);
  ds = ctime (&d);
  printf (_("  Date %s"), ds ? ds : "?");
  printf (_("  uid = %d, gid = %d, mode = 0%o.\n"), uid, gid, mode);

  return 0;
}

int
main (int argc, char **argv)
{
  ar_scan (argv[1], describe_member, NULL);
  return 0;
}

#endif  /* TEST.  */
#endif  /* NO_ARCHIVES.  */
