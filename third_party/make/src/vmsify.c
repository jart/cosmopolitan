/* vmsify.c -- Module for vms <-> unix file name conversion
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

/* Written by Klaus Kämpf (kkaempf@progis.de)
   of proGIS Software, Aachen, Germany */


#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "makeint.h"

#if VMS
#include <unixlib.h>
#include <stdlib.h>
#include <jpidef.h>
#include <descrip.h>
#include <uaidef.h>
#include <ssdef.h>
#include <starlet.h>
#include <lib$routines.h>
/* Initialize a string descriptor (struct dsc$descriptor_s) for an
   arbitrary string.   ADDR is a pointer to the first character
   of the string, and LEN is the length of the string. */

#define INIT_DSC_S(dsc, addr, len) do { \
  (dsc).dsc$b_dtype = DSC$K_DTYPE_T;    \
  (dsc).dsc$b_class = DSC$K_CLASS_S;    \
  (dsc).dsc$w_length = (len);           \
  (dsc).dsc$a_pointer = (addr);         \
} while (0)

/* Initialize a string descriptor (struct dsc$descriptor_s) for a
   NUL-terminated string.  S is a pointer to the string; the length
   is determined by calling strlen(). */

#define INIT_DSC_CSTRING(dsc, s) INIT_DSC_S(dsc, s, strlen(s))
#endif

/*
  copy 'from' to 'to' up to but not including 'upto'
  return 0 if eos on from
  return 1 if upto found

  return 'to' at last char + 1
  return 'from' at match + 1 or eos if no match

  if as_dir == 1, change all '.' to '_'
  else change all '.' but the last to '_'
*/

static int
copyto (char **to, const char **from, char upto, int as_dir)
{
  const char *s;

  s = strrchr (*from, '.');

  while (**from)
    {
      if (**from == upto)
	{
	  do
	    {
	      (*from)++;
	    }
	  while (**from == upto);
	  return 1;
	}
      if (**from == '.')
	{
	  if ((as_dir == 1)
	      || (*from != s))
	    **to = '_';
	  else
	    **to = '.';
	}
      else
	{
#ifdef HAVE_CASE_INSENSITIVE_FS
	  if (isupper ((unsigned char)**from))
	    **to = tolower ((unsigned char)**from);
	  else
#endif
	    **to = **from;
	}
      (*to)++;
      (*from)++;
    }

  return 0;
}


/*
  get translation of logical name

*/

static char *
trnlog (const char *name)
{
  int stat;
  static char reslt[1024];
  $DESCRIPTOR (reslt_dsc, reslt);
  short resltlen;
  struct dsc$descriptor_s name_dsc;
  char *s;

  /*
   * the string isn't changed, but there is no string descriptor with
   * "const char *dsc$a_pointer"
   */
  INIT_DSC_CSTRING (name_dsc, (char *)name);

  stat = lib$sys_trnlog (&name_dsc, &resltlen, &reslt_dsc);

  if ((stat&1) == 0)
    {
      return "";
    }
  if (stat == SS$_NOTRAN)
    {
      return "";
    }
  reslt[resltlen] = '\0';

  s = xmalloc (resltlen+1);
  strcpy (s, reslt);
  return s;
}

static char *
showall (char *s)
{
  static char t[512];
  char *pt;

  pt = t;
  if (strchr (s, '\\') == 0)
    return s;
  while (*s)
    {
      if (*s == '\\')
	{
	  *pt++ = *s;
	}
      *pt++ = *s++;
    }
  return pt;
}


enum namestate { N_START, N_DEVICE, N_OPEN, N_DOT, N_CLOSED, N_DONE };

/*
  convert unix style name to vms style
  type = 0 -> name is a full name (directory and filename part)
  type = 1 -> name is a directory
  type = 2 -> name is a filename without directory

  The following conversions are applied
			(0)		(1)			(2)
	input		full name	dir name		file name

1	./		<cwd>		[]			<current directory>.dir
2	../		<home of cwd>	<home of cwd>		<home of cwd>.dir

3	//		<dev of cwd>:	<dev of cwd>:[000000]	<dev of cwd>:000000.dir
4	//a		a:		a:			a:
5	//a/		a:		a:			a:000000.dir

9	/		[000000]	[000000]		000000.dir
10	/a		[000000]a	[a]			[000000]a
11	/a/		[a]		[a]			[000000]a.dir
12	/a/b		[a]b		[a.b]			[a]b
13	/a/b/		[a.b]		[a.b]			[a]b.dir
14	/a/b/c		[a.b]c		[a.b.c]			[a.b]c
15	/a/b/c/		[a.b.c]		[a.b.c]			[a.b]c.dir

16	a		a		[.a]			a
17	a/		[.a]		[.a]			a.dir
18	a/b		[.a]b		[.a.b]			[.a]b
19	a/b/		[.a.b]		[.a.b]			[.a]b.dir
20	a/b/c		[.a.b]c		[.a.b.c]		[.a.b]c
21	a/b/c/		[.a.b.c]	[.a.b.c]		[.a.b]c.dir

22	a.b.c		a_b.c		[.a_b_c]		a_b_c.dir

23	[x][y]z		[x.y]z		[x.y]z			[x.y]z
24	[x][.y]z	[x.y]z		[x.y]z			[x.y]z

25  filenames with '$'  are left unchanged if they contain no '/'
25  filenames with ':' are left unchanged
26  filenames with a single pair of '[' ']' are left unchanged

  The input string is not written to.  The result is also const because
  it's a static buffer; we don't want to change it.
*/

const char *
vmsify (const char *name, int type)
{
/* max 255 device
   max 39 directory
   max 39 filename
   max 39 filetype
   max 5 version
*/
/* todo: VMSMAXPATHLEN is defined for ODS2 names: it needs to be adjusted. */
#define VMSMAXPATHLEN 512

  enum namestate nstate;
  static char vmsname[VMSMAXPATHLEN+1];
  const char *fptr;
  const char *t;
  char *vptr;
  int as_dir;
  int count;
  const char *s;
  const char *s1;
  const char *s2;

  if (name == 0)
    return 0;
  fptr = name;
  vptr = vmsname;
  nstate = N_START;

  /* case 25a */
  t = strpbrk (name, "$:");

  if (t != 0)
    {
//      const char *s1;
//      const char *s2;

      if (type == 1)
        {
          s1 = strchr (t+1, '[');
          s2 = strchr (t+1, ']');
        }

      if (*t == '$')
        {
          if (strchr (name, '/') == 0)
            {
              strcpy (vmsname, name);
              if ((type == 1) && (s1 != 0) && (s2 == 0))
                strcat (vmsname, "]");
              return vmsname;
            }
        }
      else
        {
          strcpy (vmsname, name);
          if ((type == 1) && (s1 != 0) && (s2 == 0))
            strcat (vmsname, "]");
          return vmsname;
        }
    }

  /* case 26 */
  t = strchr (name, '[');

  if (t != 0)
    {
//      const char *s;
//      const char *s1 = strchr (t+1, '[');
      s1 = strchr (t+1, '[');
      if (s1 == 0)
	{
          strcpy (vmsname, name);
	  if ((type == 1) && (strchr (t+1, ']') == 0))
            strcat (vmsname, "]");
          return vmsname;
	}
      s1--;
      if (*s1 != ']')
	{
          strcpy (vmsname, name);
	  return vmsname;		/* not ][, keep unchanged */
	}

      /* we have ][ */

      s = name;

      /* s  -> starting char
	 s1 -> ending ']'  */
      do
	{
	  strncpy (vptr, s, s1-s);	/* copy up to but not including ']' */
	  vptr += s1-s;
	  if (*s1 == 0)
	    break;
	  s = s1 + 1;			/* s -> char behind ']' */
	  if (*s != '[')		/* was '][' ? */
	    break;			/* no, last ] found, exit */
	  s++;
	  if (*s != '.')
	    *vptr++ = '.';
	  s1 = strchr (s, ']');
	  if (s1 == 0)			/* no closing ] */
	    s1 = s + strlen (s);
	}
      while (1);

      *vptr++ = ']';

      fptr = s;

    }
  else		/* no [ in name */
    {
      int state = 0;
      int rooted = 1;	/* flag if logical is rooted, else insert [000000] */

      do
	{
      switch (state)
	{
	  case 0:				/* start of loop */
	    if (*fptr == '/')
	      {
		fptr++;
		state = 1;
	      }
	    else if (*fptr == '.')
	      {
		fptr++;
		state = 10;
	      }
	    else
	      state = 2;
	    break;

	  case 1:				/* '/' at start */
	    if (*fptr == '/')
	      {
		fptr++;
		state = 3;
	      }
	    else
	      state = 4;
	    break;

	  case 2:				/* no '/' at start */
            {
            const char *s = strchr (fptr, '/');
	    if (s == 0)			/* no '/' (16) */
	      {
		if (type == 1)
		  {
		    strcpy (vptr, "[.");
		    vptr += 2;
		  }
		copyto (&vptr, &fptr, 0, (type==1));
		if (type == 1)
		  *vptr++ = ']';
		state = -1;
	      }
	    else			/* found '/' (17..21) */
	      {
		if ((type == 2)
		    && (*(s+1) == 0))	/* 17(2) */
		  {
		    copyto (&vptr, &fptr, '/', 1);
		    state = 7;
		  }
		else
		  {
		    strcpy (vptr, "[.");
		    vptr += 2;
		    copyto (&vptr, &fptr, '/', 1);
		    nstate = N_OPEN;
		    state = 9;
		  }
	      }
	    break;
            }

	  case 3:				/* '//' at start */
            {
//            const char *s;
//            const char *s1;
            char *vp;
	    while (*fptr == '/')	/* collapse all '/' */
	      fptr++;
	    if (*fptr == 0)		/* just // */
	      {
		char cwdbuf[VMSMAXPATHLEN+1];

		s1 = getcwd(cwdbuf, VMSMAXPATHLEN);
		if (s1 == 0)
		  {
                    vmsname[0] = '\0';
		    return vmsname;	/* FIXME, err getcwd */
		  }
		s = strchr (s1, ':');
		if (s == 0)
		  {
                    vmsname[0] = '\0';
		    return vmsname;	/* FIXME, err no device */
		  }
		strncpy (vptr, s1, s-s1+1);
		vptr += s-s1+1;
		state = -1;
		break;
	      }

	    s = vptr;

	    if (copyto (&vptr, &fptr, '/', 1) == 0)	/* copy device part */
	      {
		*vptr++ = ':';
		state = -1;
		break;
	      }
	    *vptr = ':';
	    nstate = N_DEVICE;
	    if (*fptr == 0)	/* just '//a/' */
	      {
		strcpy (vptr+1, "[000000]");
		vptr += 9;
		state = -1;
		break;
	      }
	    *vptr = 0;
				/* check logical for [000000] insertion */
	    vp = trnlog (s);
	    if (*vp != '\0')
	      {			/* found translation */
		for (;;)	/* loop over all nested logicals */
		  {
		    char *vp2 = vp + strlen (vp) - 1;
		    if (*vp2 == ':')	/* translation ends in ':' */
		      {
			vp2 = trnlog (vp);
			free (vp);
			if (*vp2 == 0)
			  {
			    rooted = 0;
			    break;
			  }
			vp = vp2;
			continue;	/* next iteration */
		      }
		    if (*vp2 == ']')	/* translation ends in ']' */
		      {
			if (*(vp2-1) == '.')	/* ends in '.]' */
			  {
			    if (strncmp (fptr, "000000", 6) != 0)
			      rooted = 0;
			  }
			else
			  {
			    strcpy (vmsname, s1);
			    vp = strchr (vmsname, ']');
			    *vp = '.';
			    nstate = N_DOT;
			    vptr = vp;
			  }
		      }
		    break;
		  }
		free (vp);
	      }
	    else
	      rooted = 0;

	    if (*vptr == 0)
	      {
		nstate = N_DEVICE;
	        *vptr++ = ':';
	      }
	    else
	      vptr++;

	    if (rooted == 0)
	      {
		nstate = N_DOT;
	        strcpy (vptr, "[000000.");
		vptr += 8;
		vp = vptr-1;
	      }
	    else
	      vp = 0;

            /* vp-> '.' after 000000 or NULL */

	    s = strchr (fptr, '/');
	    if (s == 0)
	      {				/* no next '/' */
		if (*(vptr-1) == '.')
		  *(vptr-1) = ']';
		else if (rooted == 0)
		  *vptr++ = ']';
		copyto (&vptr, &fptr, 0, (type == 1));
		state = -1;
		break;
	      }
	    else
	      {
		while (*(s+1) == '/')	/* skip multiple '/' */
		  s++;
	      }

	    if ((rooted != 0)
	        && (*(vptr-1) != '.'))
	      {
		*vptr++ = '[';
		nstate = N_DOT;
	      }
	    else
	      if ((nstate == N_DOT)
		 && (vp != 0)
		 && (*(s+1) == 0))
		{
		  if (type == 2)
		    {
		      *vp = ']';
		      nstate = N_CLOSED;
		    }
		}
	    state = 9;
	    break;
            }
	  case 4:				/* single '/' at start (9..15) */
	    if (*fptr == 0)
	      state = 5;
	    else
	      state = 6;
	    break;

	  case 5:				/* just '/' at start (9) */
	    if (type != 2)
	      {
	        *vptr++ = '[';
		nstate = N_OPEN;
	      }
	    strcpy (vptr, "000000");
	    vptr += 6;
	    if (type == 2)
	      state = 7;
	    else
	      state = 8;
	    break;

	  case 6:		/* chars following '/' at start 10..15 */
            {
            const char *s;
	    *vptr++ = '[';
	    nstate = N_OPEN;
	    s = strchr (fptr, '/');
	    if (s == 0)			/* 10 */
	      {
		if (type != 1)
		  {
		    strcpy (vptr, "000000]");
		    vptr += 7;
		  }
		copyto (&vptr, &fptr, 0, (type == 1));
		if (type == 1)
		  {
		    *vptr++ = ']';
		  }
		state = -1;
	      }
	    else			/* 11..15 */
	      {
		if ( (type == 2)
		   && (*(s+1) == 0))	/* 11(2) */
		  {
		    strcpy (vptr, "000000]");
		    nstate = N_CLOSED;
		    vptr += 7;
		  }
		copyto (&vptr, &fptr, '/', (*(vptr-1) != ']'));
		state = 9;
	      }
	    break;
            }

	  case 7:				/* add '.dir' and exit */
	    if ((nstate == N_OPEN)
		|| (nstate == N_DOT))
	      {
		char *vp = vptr-1;
		while (vp > vmsname)
		  {
		    if (*vp == ']')
		      {
			break;
		      }
		    if (*vp == '.')
		      {
			*vp = ']';
			break;
		      }
		    vp--;
		  }
	      }
	    strcpy (vptr, ".dir");
	    vptr += 4;
	    state = -1;
	    break;

	  case 8:				/* add ']' and exit */
	    *vptr++ = ']';
	    state = -1;
	    break;

	  case 9:			/* 17..21, fptr -> 1st '/' + 1 */
            {
            const char *s;
	    if (*fptr == 0)
	      {
		if (type == 2)
		  {
		    state = 7;
		  }
		else
		  state = 8;
		break;
	      }
	    s = strchr (fptr, '/');
	    if (s == 0)
	      {
		if (type != 1)
		  {
		    if (nstate == N_OPEN)
		      {
			*vptr++ = ']';
			nstate = N_CLOSED;
		      }
		    as_dir = 0;
		  }
		else
		  {
		    if (nstate == N_OPEN)
		      {
			*vptr++ = '.';
			nstate = N_DOT;
		      }
		    as_dir = 1;
		  }
	      }
	    else
	      {
		while (*(s+1) == '/')
		  s++;
		if ( (type == 2)
		    && (*(s+1) == 0))		/* 19(2), 21(2)*/
		  {
		    if (nstate != N_CLOSED)
		      {
			*vptr++ = ']';
			nstate = N_CLOSED;
		      }
		    as_dir = 1;
		  }
		else
		  {
		    if (nstate == N_OPEN)
		      {
			*vptr++ = '.';
			nstate = N_DOT;
		      }
		    as_dir = 1;
		  }
	      }
	    if ( (*fptr == '.')			/* check for '..' or '../' */
		&& (*(fptr+1) == '.')
		&& ((*(fptr+2) == '/')
		    || (*(fptr+2) == 0)) )
	      {
                char *vp;
		fptr += 2;
		if (*fptr == '/')
		  {
		    do
		      {
			fptr++;
		      }
		    while (*fptr == '/');
		  }
		else if (*fptr == 0)
		  type = 1;
		vptr--;				/* vptr -> '.' or ']' */
		vp = vptr;
		for (;;)
		  {
		    vp--;
		    if (*vp == '.')		/* one back */
		      {
			vptr = vp;
			nstate = N_OPEN;
			break;
		      }
		    if (*vp == '[')		/* top level reached */
		      {
			if (*fptr == 0)
			  {
			    strcpy (vp, "[000000]");
			    vptr = vp + 8;
			    nstate = N_CLOSED;
			    s = 0;
			    break;
			  }
			else
			  {
			    vptr = vp+1;
			    nstate = N_OPEN;
			    break;
			  }
		      }
		  }
	      }
	    else
	      {
		copyto (&vptr, &fptr, '/', as_dir);
		if (nstate == N_DOT)
		  nstate = N_OPEN;
	      }
	    if (s == 0)
	      {					/* 18,20 */
		if (type == 1)
		  *vptr++ = ']';
		state = -1;
	      }
	    else
	      {
		if (*(s+1) == 0)
		  {
		    if (type == 2)		/* 19,21 */
		      {
		        state = 7;
		      }
		    else
		      {
			*vptr++ = ']';
			state = -1;
		      }
		  }
	      }
	    break;
            }

	  case 10:				/* 1,2 first is '.' */
	    if (*fptr == '.')
	      {
		fptr++;
		state = 11;
	      }
	    else
	      state = 12;
	    break;

	  case 11:				/* 2, '..' at start */
	    count = 1;
	    if (*fptr != 0)
	      {
		if (*fptr != '/')		/* got ..xxx */
		  {
                    strcpy (vmsname, name);
		    return vmsname;
		  }
		do				/* got ../ */
		  {
		    fptr++;
		    while (*fptr == '/') fptr++;
		    if (*fptr != '.')
		      break;
		    if (*(fptr+1) != '.')
		      break;
		    fptr += 2;
		    if ((*fptr == 0)
			|| (*fptr == '/'))
		      count++;
		  }
		while (*fptr == '/');
	      }
	    {					/* got '..' or '../' */
              char *vp;
	      char cwdbuf[VMSMAXPATHLEN+1];

	      vp = getcwd(cwdbuf, VMSMAXPATHLEN);
	      if (vp == 0)
		{
                  vmsname[0] = '\0';
		  return vmsname;    /* FIXME, err getcwd */
		}
	      strcpy (vptr, vp);
	      vp = strchr (vptr, ']');
	      if (vp != 0)
		{
		  nstate = N_OPEN;
		  while (vp > vptr)
		    {
		      vp--;
		      if (*vp == '[')
			{
			  vp++;
			  strcpy (vp, "000000]");
			  state = -1;
			  break;
			}
		      else if (*vp == '.')
			{
			  if (--count == 0)
			    {
			      if (*fptr == 0)	/* had '..' or '../' */
				{
				  *vp++ = ']';
				  state = -1;
				}
			      else			/* had '../xxx' */
				{
				  state = 9;
				}
			      *vp = '\0';
			      break;
			    }
			}
		    }
		}
	      vptr += strlen (vptr);
	    }
	    break;

	  case 12:				/* 1, '.' at start */
	    if (*fptr != 0)
	      {
		if (*fptr != '/')
		  {
                    strcpy (vmsname, name);
		    return vmsname;
		  }
		while (*fptr == '/')
		  fptr++;
	      }

	    {
              char *vp;
	      char cwdbuf[VMSMAXPATHLEN+1];

	      vp = getcwd(cwdbuf, VMSMAXPATHLEN);
	      if (vp == 0)
		{
                  vmsname[0] = '\0';
		  return vmsname;    /*FIXME, err getcwd */
		}
	      strcpy (vptr, vp);
            }
            if (*fptr == 0)
              {
                state = -1;
                break;
              }
            else
              {
                char *vp = strchr (vptr, ']');
                if (vp == 0)
                  {
                    state = -1;
                    break;
                  }
                *vp = '\0';
                nstate = N_OPEN;
                vptr += strlen (vptr);
                state = 9;
              }
	    break;
	}

	}
      while (state > 0);


    }


  /* directory conversion done
     fptr -> filename part of input string
     vptr -> free space in vmsname
  */

  *vptr++ = 0;

  return vmsname;
}



/*
  convert from vms-style to unix-style

  dev:[dir1.dir2]	//dev/dir1/dir2/
*/

const char *
unixify (const char *name)
{
  static char piece[512];
  const char *s;
  char *p;

  if (strchr (name, '/') != 0)		/* already in unix style */
    {
      strcpy (piece, name);
      return piece;
    }

  p = piece;
  *p = 0;

  /* device part */

  s = strchr (name, ':');

  if (s != 0)
    {
      int l = s - name;
      *p++ = '/';
      *p++ = '/';
      strncpy (p, name, l);
      p += l;
    }

  /* directory part */

  *p++ = '/';
  s = strchr (name, '[');

  if (s != 0)
    {
      s++;
      switch (*s)
        {
	  case ']':		/* [] */
	    strcat (p, "./");
	    break;
	  case '-':		/* [- */
	    strcat (p, "../");
	    break;
	  case '.':
	    strcat (p, "./");	/* [. */
	    break;
	  default:
	    s--;
	    break;
        }
      s++;
      while (*s)
        {
	  if (*s == '.')
	    *p++ = '/';
	  else
	    *p++ = *s;
	  s++;
	  if (*s == ']')
	    {
	      s++;
	      break;
	    }
        }
      if (*s != 0)		/* more after ']' ?? */
        {
	  if (*(p-1) != '/')
	    *p++ = '/';
	  strcpy (p, s);		/* copy it anyway */
        }
    }

  else		/* no '[' anywhere */

    {
      *p++ = 0;
    }

  /* force end with '/' */

  if (*(p-1) != '/')
    *p++ = '/';
  *p = 0;

  return piece;
}

/* EOF */
