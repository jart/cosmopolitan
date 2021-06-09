/*
  qdos/qdos.c

  Copyright (c) 1990-2005 Info-ZIP.  All rights reserved.

  See the accompanying file LICENSE, version 2005-Feb-10 or later
  (the contents of which are also included in zip.h) for terms of use.
  If, for some reason, all these files are missing, the Info-ZIP license
  also may be found at:  ftp://ftp.info-zip.org/pub/infozip/license.html
*/
/*
 * Yes this  file is necessary; the QDOS file system is the most
 * ludicrous known to man (even more so than VMS!).
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <unistd.h>

#include "zip.h"
#include "crypt.h"
#include "ttyio.h"

#ifdef QDOS

# include <qdos.h>

#if CRYPT

char *getp(m, p, n)
    ZCONST char *m;              /* prompt for password */
    char *p;                    /* return value: line input */
    int n;                      /* bytes available in p[] */
{
    int c;                      /* one-byte buffer for read() to use */
    int i;                      /* number of characters input */
    char *w;                    /* warning on retry */

    /* get password */
    w = "";
    sd_cure(getchid(0), -1);    /* enable cursor */
    do {
        fputs(w, stderr);       /* warning if back again */
        fputs(m, stderr);       /* display prompt and flush */
        fflush(stderr);
        i = 0;
        do {
            c = getch();
            if (c == 0xc2) {
                if (i > 0) {
                    i--; /* the `del' keys works */
                    fputs("\b \b", stderr);
                }
            }
            else if (i < n) {
                p[i++] = c;     /* truncate past n */
                if(c != '\n') putc('*', stderr);
            }
        } while (c != '\n');

        putc('\n', stderr);  fflush(stderr);
        w = "(line too long--try again)\n";
    } while (p[i-1] != '\n');

    p[i-1] = 0;                 /* terminate at newline */
    sd_curs(getchid(0), -1);    /* suppress cursor */
    return p;                   /* return pointer to password */

} /* end function getp() */

#endif /* CRYPT */


#define __attribute__(p)

int newname(char *, int, int);

#else /* !QDOS */
#define QDOS_FLMAX 36

short qlflag = 0;

struct qdirect  {
    long            d_length __attribute__ ((packed));  /* file length */
    unsigned char   d_access __attribute__ ((packed));  /* file access type */
    unsigned char   d_type __attribute__ ((packed));    /* file type */
    long            d_datalen __attribute__ ((packed)); /* data length */
    long            d_reserved __attribute__ ((packed));/* Unused */
    short           d_szname __attribute__ ((packed));  /* size of name */
    char            d_name[QDOS_FLMAX] __attribute__ ((packed));/* name area */
    long            d_update __attribute__ ((packed));  /* last update */
    long            d_refdate __attribute__ ((packed));
    long            d_backup __attribute__ ((packed));   /* EOD */
    } ;
#endif /* ?QDOS */

#define SHORTID 0x4afb          /* in big-endian order !! */
#define LONGID  "QDOS02"
#define EXTRALEN (sizeof(struct qdirect) + 8)

typedef struct
{
    unsigned short shortid __attribute__ ((packed));
    struct
    {
        unsigned char lo __attribute__ ((packed));
        unsigned char hi __attribute__ ((packed));
    } len __attribute__ ((packed));
    char        longid[8] __attribute__ ((packed));
    struct      qdirect     header __attribute__ ((packed));
} qdosextra;

#ifdef USE_EF_UT_TIME
local int GetExtraTime(struct zlist far *z, iztimes *z_utim, unsigned ut_flg);
#endif

#ifdef QDOS

#define rev_short(x) (x)
#define rev_long(x) (x)

char _prog_name[] = "zip";
char _copyright[] = "(c) Info-ZIP Group";
long _stack = 16*1024;
char *  _endmsg = NULL;

extern void consetup_title(chanid_t,struct WINDOWDEF *);
void (*_consetup)(chanid_t,struct WINDOWDEF *) = consetup_title;

struct WINDOWDEF _condetails =
{
    2,
    1,
    0,
    7,
    500,
    220,
    2,
    30
};

extern short qlwait;
extern short dtype;

#define CHECKDIR(p1) (((p1).d_type == dtype) && (((p1).d_length % 64) == 0))

char * stpcpy (char *d, ZCONST char *s)
{
    while(*d++ = *s++)
        ; /* Null loop */
    return d-1;
}

static jobid_t chowner(chanid_t chan)
{
    extern char *_sys_var;
    char *scht;
    long *cdb;
    long jid;

    scht = *((char **)(_sys_var + 0x78));
    cdb = *(long **)((long *)scht  + (chan & 0xffff));
    jid = *(cdb + 2);
    return jid;
}

void QDOSexit(void)
{
    jobid_t me,you;

    me = getpid();
    you = chowner(getchid(0));

    if((me == you) && ((qlflag & 4) == 0))
    {
        if(isatty(0) && isatty(2) && qlwait)
        {
            char c = 0;
            fputs("Press a key to exit", stderr);
            if((io_fbyte(getchid(0), qlwait, &c) == 0) && c == 27)
            {
                io_fbyte(getchid(0), -1, &c);
            }
        }
    }
    exit(ZE_OK);
}

/* Access seems to be *always* broken in c68 */
/* Not accurate, just works */

int access (char *f, int mode)
{
    struct stat st;
    int fd;

    if((fd = stat(f, &st)) == 0)
    {
        switch(fd)
        {
        case F_OK:
            break;
        case R_OK:
            fd = (st.st_mode & 0444) == 0;
            break;
        case W_OK:
            fd = (st.st_mode & 0222) == 0;
            break;
        case X_OK:
            fd = (st.st_mode & 0111) == 0;
            break;
        default:
            fd = -1;
            break;
        }
    }
    return fd;
}

/* Fixup a Mickey Mouse file naming system */

char * Unix2ql (char *qlname, char **dot)
{
    static char path[64];
    char name[64];
    char *q, *r, *s;

    strcpy(name, qlname);
    if(*name == '~')
    {
        r = name+1;
        getcwd(path, sizeof(path));
        q = path + strlen(path);
        if(*(q-1) != '_')
        {
            *q++ = '_';
        }
    }
    else
    {
        q = path;
        r = name;
    }

    if(*r == '/')
    {
        r++;
    }

    strcpy(q, r);

    while (*q)
    {
        if(*q == '/' || *q == '.')
        {
            if(*q == '.' && dot)
            {
                *dot = name + (q - path);
            }
            *q = '_';
        }

        q++;
    }
    return path;
}

#if 0                                 /* Not used in ZIP */

GuessAltName(char *name, char *dot)
{
    if(dot)
    {
        *dot = '.';
    }
    else
    {
        if((dot = strrchr(name, '_')))
        {
            *dot = '.';
        }
    }
}

#endif /* 0 */

short devlen(char *p)
{
    char defpath[40];
    short deflen = 0, ok = 0;

    getcwd(defpath, sizeof(defpath));
    deflen = strlen(defpath);
    if(deflen)
    {
        if(strnicmp(p, defpath, deflen) == 0)
        {
            ok = 1;
        }
    }

    if(!ok)
    {
        if(isdirdev(p))
        {
            deflen = 5;
        }
        else
        {
            deflen = 0;
        }
    }
    return deflen;
}

char * ql2Unix (char *qlname)
{
    struct stat st;
    int sts;
    char *p, *r, *s, *ldp;
    char *pnam = NULL;
    static char path[64];
    short  deflen;
    char name[64];

    strcpy(name, qlname);
    strcpy(path, name);

    deflen = devlen(qlname);

    p = name + deflen;
    pnam = path + deflen;

    if(s = strrchr(p, '_'))
    {
        *s = 0;
        sts = stat(name, &st);
        if(deflen && sts ==0 && (st.st_mode & S_IFDIR))
        {
            *(path+(s-name)) = '/';
        }
        else
        {
            *(path+(s-name)) = '.';
        }
    }

    ldp = p;
    for(r = p; *r; r++)
    {
        if(r != ldp && *r == '_')
        {
            *r = 0;
            if(deflen)
            {
                sts = stat(name, &st);
            }
            else
                sts = -1;

            if(sts ==0 && (st.st_mode & S_IFDIR))
            {
                *(path+(r-name)) = '/';
                ldp = r + 1;
            }
            else
            {
                *(path+(r-name)) = '_';
            }
            *r = '_';
        }
    }
    return pnam;
}

char *LastDir(char *ws)
{
    char *p;
    char *q = ws;
    struct stat s;

    for(p = ws; *p; p++)
    {
        if(p != ws && *p == '_')
        {
            char c;

            p++;
            c = *p;
            *p = 0;
            if(stat(ws, &s) == 0 && S_ISDIR(s.st_mode))
            {
                q = p;
            }
            *p = c;
        }
    }
    return q;
}

# ifndef UTIL

static int add_dir(char * dnam)
{
    int e = ZE_OK;
    char *p;
    short nlen;

    nlen = strlen(dnam);
    if(p = malloc(nlen + 2))
    {
        strncpy (p, dnam, nlen);
        if(*(p+nlen) != '_')
        {
            *(p+nlen) = '_';
            *(p+nlen+1) = '\0';
        }
        if ((e = newname(p, 1, 0)) != ZE_OK)
        {
                free(p);
        }
    }
    else
    {
        e = ZE_MEM;
    }
    return e;
}

int qlwild (char *dnam, short dorecurse, short l)
{
    static char match[40] = {0};
    static char ddev[8] =  {0};
    static short nc;
    static short llen;
    static char base[40];

    int chid;
    struct qdirect qd;
    char *dp;
    int e = ZE_MISS;

    if (l == 0)
    {
        nc = 0;
        *base = '\0';
        if (isdirdev (dnam))
        {
            dp = dnam;
            strncpy (ddev, dnam, 5);
        }
        else
        {

            char *p;
            char temp[40];

            getcwd (temp, 40);

            llen = strlen(temp);
            p = (temp + llen - 1);
            if (*p != '_')
            {
                *p++ = '_';
                *p = 0;
            }

            strncpy (ddev, temp, 5);
            dp = base;
            p = stpcpy (dp, temp);
            strcpy (p, dnam);
        }

        {
            char *q = isshexp (dp);
            if(q)
            {
                strcpy (match, dp + 5);
                if (q)
                {
                    while (q != dp && *q != '_')
                    {
                        q--;
                    }
                    *(++q) = '\0';
                }
            }
            else
            {
                struct stat s;
                if (stat(dp, &s) == 0)
                {
                    if (!(s.st_mode & S_IFDIR))
                    {
                        return procname(dp, 0);
                    }
                }
                else
                {
                    return ZE_MISS;  /* woops, no wildcards! */
                }
            }

        }
    }
    else
    {
        dp = dnam;
    }

    if ((chid = io_open (dp, 4L)) > 0)
    {
        int id = 0;
        while (io_fstrg (chid, -1, &qd, 64) > 0)
        {
            short j;

            if (qd.d_szname)
            {
                if (CHECKDIR(qd))
                {
                    if(dorecurse)
                    {
                        char fnam[256], *p;

                        p = stpcpy (fnam, ddev);
                        strncpy (p, qd.d_name, qd.d_szname);
                        *(p + qd.d_szname) = 0;
                        e = qlwild (fnam, dorecurse, l+1);
                    }
                    else
                    {
                        continue;
                    }
                }
                else
                {
                    char nam[48];
                    strcpy(nam, ddev);

                    strncpy (nam + 5, qd.d_name, qd.d_szname);
                    *(nam + 5 + qd.d_szname) = 0;

                    if (MATCH (match, nam + 5, 0) == 1)
                    {
                        if(dirnames && l && id == 0)
                        {
                            id = 1;
                            if((e = add_dir(dp)) != ZE_OK)
                            {
                                return e;
                            }
                        }

                        if((e = procname(nam, 0)) == ZE_OK)
                        {
                            nc++;
                        }
                    }
                }
            }
        }
        io_close (chid);
    }

    if (l == 0)
    {
        *ddev = 0;
        *match = 0;
        e = (nc) ? ZE_OK : ZE_MISS;
    }
    return e;

}

int wild(char *p)
{
    return qlwild(p, recurse, 0);
}
# endif /* !UTIL */

/*
 * Return QDOS error, 0 if exec 1 if found but not exe or rel
 */
int qlstat(char *name, struct qdirect *qs, char *flag)
{
    int r;
    r = qstat(name, qs);
    if(r == 0)
    {
        if(qs->d_type == 0)
        {
            r = 1;
        }
        else if(CHECKDIR(*qs))
        {
            r = 255;
        }
    }
    return r;
}

#else /* !QDOS */

long rev_long (ulg l)
{
    uch cc[4];
    cc[0] = (uch)(l >> 24);
    cc[1] = (uch)((l >> 16) & 0xff);
    cc[2] = (uch)((l >> 8) & 0xff);
    cc[3] = (uch)(l & 0xff);
    return *(ulg *)cc;
}

short rev_short (ush s)
{
    uch cc[2];
    cc[0] = (uch)((s >> 8) & 0xff);
    cc[1] = (uch)(s & 0xff);
    return *(ush *)cc;
}

#define O_BINARY 0

int qlstat(char *name, struct qdirect *qs, char *flag)
{
    int r = -1;
    int n, fd;
    struct stat s;
    struct _ntc_
    {
        long id;
        long dlen;
    } ntc;

    *flag = 0;
    if((fd = open(name, O_RDONLY | O_BINARY)) > 0)
    {
        short nl;

        fstat(fd, &s);
        lseek(fd, -8, SEEK_END);
        read(fd, &ntc, 8);
        qs->d_length = rev_long(s.st_size);
        qs->d_update = rev_long(s.st_ctime + 283996800);

        nl = strlen(name);
        if(nl > QDOS_FLMAX)
        {
            nl = QDOS_FLMAX;
            *flag = 1;
        }
        qs->d_szname = rev_short(nl);
        memcpy(qs->d_name, name, nl);

        if(ntc.id == *(long *)"XTcc")
        {
            qs->d_datalen = ntc.dlen;    /* This is big endian */
            qs->d_type = 1;
            r = 0;
        }
        else
        {
            qs->d_type = 0;
            qs->d_datalen = 0;
            r = 1;
        }
        close(fd);
        return r;
    }
    else
    {
        fprintf(stderr, "Fails %d\n", fd);
        return r;
    }
}

#endif /* ?QDOS */

#ifdef USE_EF_UT_TIME

#define EB_L_UT_SIZE    (EB_HEADSIZE + eb_l_ut_len)
#define EB_C_UT_SIZE    (EB_HEADSIZE + eb_c_ut_len)

#ifdef UNIX
#define EB_L_UX2_SIZE     (EB_HEADSIZE + EB_UX2_MINLEN)
#define EB_C_UX2_SIZE     EB_HEADSIZE
#define EF_L_UT_UX2_SIZE  (EB_L_UT_SIZE + EB_L_UX2_SIZE)
#define EF_C_UT_UX2_SIZE  (EB_C_UT_SIZE + EB_C_UX2_SIZE)
#else
#define EF_L_UT_UX2_SIZE  EB_L_UT_SIZE
#define EF_C_UT_UX2_SIZE  EB_C_UT_SIZE
#endif

local int GetExtraTime(struct zlist far *z, iztimes *z_utim, unsigned ut_flg)
{
  char *eb_l_ptr;
  char *eb_c_ptr;
  char *eb_pt;
  extent eb_l_ut_len = 0;
  extent eb_c_ut_len = 0;

#ifdef UNIX
  struct stat s;

  /* For the full sized UT local field including the UID/GID fields, we
   * have to stat the file, again.  */
  if (stat(z->name, &s))
    return ZE_OPEN;
  /* update times in z_utim, stat() call might have changed atime... */
  z_utim->mtime = s.st_mtime;
  z_utim->atime = s.st_atime;
  z_utim->ctime = s.st_mtime;   /* best guess (st_ctime != creation time) */
#endif /* UNIX */

#ifdef IZ_CHECK_TZ
  if (!zp_tz_is_valid)
    ut_flg = 0;          /* disable UT e.f creation if no valid TZ info */
#endif
  if (ut_flg != 0) {
    if (ut_flg & EB_UT_FL_MTIME)
      eb_l_ut_len = eb_c_ut_len = 1;
    if (ut_flg & EB_UT_FL_ATIME)
      eb_l_ut_len++;
    if (ut_flg & EB_UT_FL_CTIME)
      eb_l_ut_len++;

    eb_l_ut_len = EB_UT_LEN(eb_l_ut_len);
    eb_c_ut_len = EB_UT_LEN(eb_c_ut_len);
  }

  if (EF_L_UT_UX2_SIZE > EB_HEADSIZE) {
    if(z->ext)
      eb_l_ptr = realloc(z->extra, (z->ext + EF_L_UT_UX2_SIZE));
    else
      eb_l_ptr = malloc(EF_L_UT_UX2_SIZE);

    if (eb_l_ptr == NULL)
      return ZE_MEM;

    if(z->cext)
      eb_c_ptr = realloc(z->cextra, (z->cext + EF_C_UT_UX2_SIZE));
    else
      eb_c_ptr = malloc(EF_C_UT_UX2_SIZE);

    if (eb_c_ptr == NULL)
      return ZE_MEM;

    z->extra = eb_l_ptr;
    eb_l_ptr += z->ext;
    z->ext += EF_L_UT_UX2_SIZE;

    if (ut_flg != 0) {
      eb_l_ptr[0]  = 'U';
      eb_l_ptr[1]  = 'T';
      eb_l_ptr[2]  = eb_l_ut_len;       /* length of data part of e.f. */
      eb_l_ptr[3]  = 0;
      eb_l_ptr[4]  = ut_flg;
      eb_pt = eb_l_ptr + 5;
      if (ut_flg & EB_UT_FL_MTIME) {
        *eb_pt++ = (char)(z_utim->mtime);
        *eb_pt++ = (char)(z_utim->mtime >> 8);
        *eb_pt++ = (char)(z_utim->mtime >> 16);
        *eb_pt++ = (char)(z_utim->mtime >> 24);
      }
      if (ut_flg & EB_UT_FL_ATIME) {
        *eb_pt++ = (char)(z_utim->atime);
        *eb_pt++ = (char)(z_utim->atime >> 8);
        *eb_pt++ = (char)(z_utim->atime >> 16);
        *eb_pt++ = (char)(z_utim->atime >> 24);
      }
      if (ut_flg & EB_UT_FL_CTIME) {
        *eb_pt++ = (char)(z_utim->ctime);
        *eb_pt++ = (char)(z_utim->ctime >> 8);
        *eb_pt++ = (char)(z_utim->ctime >> 16);
        *eb_pt++ = (char)(z_utim->ctime >> 24);
      }
    }
#ifdef UNIX
    else {
      eb_pt = eb_l_ptr;
    }
    *eb_pt++ = 'U';
    *eb_pt++ = 'x';
    *eb_pt++ = EB_UX2_MINLEN;            /* length of data part of local e.f. */
    *eb_pt++  = 0;
    *eb_pt++ = (char)(s.st_uid);
    *eb_pt++ = (char)(s.st_uid >> 8);
    *eb_pt++ = (char)(s.st_gid);
    *eb_pt++ = (char)(s.st_gid >> 8);
#endif /* UNIX */

    z->cextra = eb_c_ptr;
    eb_c_ptr += z->cext;
    z->cext += EF_C_UT_UX2_SIZE;

    if (ut_flg != 0) {
      memcpy(eb_c_ptr, eb_l_ptr, EB_C_UT_SIZE);
      eb_c_ptr[EB_LEN] = eb_c_ut_len;
    }
#ifdef UNIX
    memcpy(eb_c_ptr+EB_C_UT_SIZE, eb_l_ptr+EB_L_UT_SIZE, EB_C_UX2_SIZE);
    eb_c_ptr[EB_LEN+EB_C_UT_SIZE] = 0;
#endif /* UNIX */
  }

  return ZE_OK;
}

#endif /* USE_EF_UT_TIME */


int set_extra_field (struct zlist *z, iztimes *z_utim )
{
    int rv = 0;
    int last_rv = 0;
    char flag = 0;

    if ((qlflag & 3) != 1)
    {
        qdosextra       *lq, *cq;
        if ((lq = (qdosextra *) calloc(sizeof(qdosextra), 1)) == NULL)
            return ZE_MEM;
        if ((cq = (qdosextra *) calloc(sizeof(qdosextra), 1)) == NULL)
            return ZE_MEM;

        rv = qlstat(z->name, &(lq->header), &flag);

        if (rv == 0 || (rv == 1 && (qlflag & 2)))
        {
            lq->shortid = rev_short((short) SHORTID);
            lq->len.lo  = (unsigned char)(EXTRALEN & 0xff);
            lq->len.hi  = (unsigned char)(EXTRALEN >> 8);
            strcpy(lq->longid, LONGID);

            memcpy(cq, lq, sizeof(qdosextra));

            z->ext      =   sizeof(qdosextra);
            z->cext     =   sizeof(qdosextra);
            z->extra    =   (void *) lq;
            z->cextra   =   (void *) cq;
            fprintf (stderr, " %c",
                     lq->header.d_datalen ? '*' : '#');
        }
        else if (rv == -1)
        {
            fprintf(stderr,
                    "%s: warning: cannot stat %s, no file header added\n",
                    "zip", z->name);
        }
        if(flag)
        {
            fputs (" !", stderr);
        }
     }
    last_rv = (rv == -1 ? ZE_OPEN : ZE_OK);

#ifdef USE_EF_UT_TIME
# ifdef QDOS
#   define IZ_UT_FLAGS EB_UT_FL_MTIME
# endif
# ifdef UNIX
#   define IZ_UT_FLAGS (EB_UT_FL_MTIME | EB_UT_FL_ATIME)
# endif
# ifndef IZ_UT_FLAGS
#   define IZ_UT_FLAGS EB_UT_FL_MTIME
# endif

    rv = GetExtraTime(z, z_utim, IZ_UT_FLAGS);
    if (rv != ZE_OK)
        last_rv = rv;
#endif /* USE_EF_UT_TIME */

    return last_rv;
}
