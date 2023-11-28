/*
  Copyright (c) 1990-2010 Info-ZIP.  All rights reserved.

  See the accompanying file LICENSE, version 2009-Jan-02 or later
  (the contents of which are also included in unzip.h) for terms of use.
  If, for some reason, all these files are missing, the Info-ZIP license
  also may be found at:  ftp://ftp.info-zip.org/pub/infozip/license.html
*/
/*---------------------------------------------------------------------------

  unix.c

  Unix-specific routines for use with Info-ZIP's UnZip 5.41 and later.

  Contains:  readdir()
             do_wild()           <-- generic enough to put in fileio.c?
             mapattr()
             mapname()
             checkdir()
             mkdir()
             close_outfile()
             defer_dir_attribs()
             set_direc_attribs()
             stamp_file()
             version()

  ---------------------------------------------------------------------------*/


#define UNZIP_INTERNAL
#include "libc/calls/struct/dirent.h"
#include "libc/log/log.h"
#include "libc/time/time.h"
#include "third_party/unzip/unzip.h"
#include "libc/time/struct/utimbuf.h"
#include "third_party/unzip/globals.h"

#ifdef USE_ICONV_MAPPING
#endif /* USE_ICONV_MAPPING */

#ifdef SCO_XENIX
#  define SYSNDIR
#else  /* SCO Unix, AIX, DNIX, TI SysV, Coherent 4.x, ... */
#  if defined(__convexc__) || defined(SYSV) || defined(CRAY) || defined(BSD4_4)
#    define DIRENT
#  endif
#endif
#if defined(_AIX) || defined(__mpexl)
#  define DIRENT
#endif
#ifdef COHERENT
#  if defined(_I386) || (defined(__COHERENT__) && (__COHERENT__ >= 0x420))
#    define DIRENT
#  endif
#endif

#ifdef _POSIX_VERSION
#  ifndef DIRENT
#    define DIRENT
#  endif
#endif

#ifdef DIRENT
#else
#  ifdef SYSV
#    ifdef SYSNDIR
#    else
#    endif
#  else /* !SYSV */
#    ifndef NO_SYSDIR
#    endif
#  endif /* ?SYSV */
#  ifndef dirent
#    define dirent direct
#  endif
#endif /* ?DIRENT */

#if defined( UNIX) && defined( __APPLE__)
#endif /* defined( UNIX) && defined( __APPLE__) */

#ifdef SET_DIR_ATTRIB
typedef struct uxdirattr {      /* struct for holding unix style directory */
    struct uxdirattr *next;     /*  info until can be sorted and set at end */
    char *fn;                   /* filename of directory */
    union {
        iztimes t3;             /* mtime, atime, ctime */
        ztimbuf t2;             /* modtime, actime */
    } u;
    unsigned perms;             /* same as min_info.file_attr */
    int have_uidgid;            /* flag */
    ulg uidgid[2];
    char fnbuf[1];              /* buffer stub for directory name */
} uxdirattr;
#define UxAtt(d)  ((uxdirattr *)d)    /* typecast shortcut */
#endif /* SET_DIR_ATTRIB */

#ifdef ACORN_FTYPE_NFS
/* Acorn bits for NFS filetyping */
typedef struct {
  uch ID[2];
  uch size[2];
  uch ID_2[4];
  uch loadaddr[4];
  uch execaddr[4];
  uch attr[4];
} RO_extra_block;

#endif /* ACORN_FTYPE_NFS */

/* static int created_dir;      */      /* used in mapname(), checkdir() */
/* static int renamed_fullpath; */      /* ditto */

static unsigned filtattr OF((__GPRO__ unsigned perms));


/*****************************/
/* Strings used multiple     */
/* times in unix.c           */
/*****************************/

#ifndef MTS
/* messages of code for setting file/directory attributes */
static ZCONST char CannotSetItemUidGid[] =
  "warning:  cannot set UID %lu and/or GID %lu for %s\n          %s\n";
static ZCONST char CannotSetUidGid[] =
  " (warning) cannot set UID %lu and/or GID %lu\n          %s";
static ZCONST char CannotSetItemTimestamps[] =
  "warning:  cannot set modif./access times for %s\n          %s\n";
static ZCONST char CannotSetTimestamps[] =
  " (warning) cannot set modif./access times\n          %s";
#endif /* !MTS */


#ifndef SFX

/**********************/
/* Function do_wild() */   /* for porting: dir separator; match(ignore_case) */
/**********************/

char *do_wild(__G__ wildspec)
    __GDEF
    ZCONST char *wildspec;  /* only used first time on a given dir */
{
/* these statics are now declared in SYSTEM_SPECIFIC_GLOBALS in unxcfg.h:
    static DIR *wild_dir = (DIR *)NULL;
    static ZCONST char *wildname;
    static char *dirname, matchname[FILNAMSIZ];
    static int notfirstcall=FALSE, have_dirname, dirnamelen;
*/
    struct dirent *file;

    /* Even when we're just returning wildspec, we *always* do so in
     * matchname[]--calling routine is allowed to append four characters
     * to the returned string, and wildspec may be a pointer to argv[].
     */
    if (!G.notfirstcall) {  /* first call:  must initialize everything */
        G.notfirstcall = TRUE;

        if (!iswild(wildspec)) {
            strncpy(G.matchname, wildspec, FILNAMSIZ);
            G.matchname[FILNAMSIZ-1] = '\0';
            G.have_dirname = FALSE;
            G.wild_dir = NULL;
            return G.matchname;
        }

        /* break the wildspec into a directory part and a wildcard filename */
        if ((G.wildname = (ZCONST char *)strrchr(wildspec, '/')) == NULL) {
            G.dirname = ".";
            G.dirnamelen = 1;
            G.have_dirname = FALSE;
            G.wildname = wildspec;
        } else {
            ++G.wildname;     /* point at character after '/' */
            G.dirnamelen = G.wildname - wildspec;
            if ((G.dirname = (char *)malloc(G.dirnamelen+1)) == (char *)NULL) {
                Info(slide, 0x201, ((char *)slide,
                  "warning:  cannot allocate wildcard buffers\n"));
                strncpy(G.matchname, wildspec, FILNAMSIZ);
                G.matchname[FILNAMSIZ-1] = '\0';
                return G.matchname; /* but maybe filespec was not a wildcard */
            }
            strncpy(G.dirname, wildspec, G.dirnamelen);
            G.dirname[G.dirnamelen] = '\0';   /* terminate for strcpy below */
            G.have_dirname = TRUE;
        }

        if ((G.wild_dir = (zvoid *)opendir(G.dirname)) != (zvoid *)NULL) {
            while ((file = readdir((DIR *)G.wild_dir)) !=
                   (struct dirent *)NULL) {
                Trace((stderr, "do_wild:  readdir returns %s\n",
                  FnFilter1(file->d_name)));
                if (file->d_name[0] == '.' && G.wildname[0] != '.')
                    continue; /* Unix:  '*' and '?' do not match leading dot */
                if (match(file->d_name, G.wildname, 0 WISEP) &&/*0=case sens.*/
                    /* skip "." and ".." directory entries */
                    strcmp(file->d_name, ".") && strcmp(file->d_name, "..")) {
                    Trace((stderr, "do_wild:  match() succeeds\n"));
                    if (G.have_dirname) {
                        strcpy(G.matchname, G.dirname);
                        strcpy(G.matchname+G.dirnamelen, file->d_name);
                    } else
                        strcpy(G.matchname, file->d_name);
                    return G.matchname;
                }
            }
            /* if we get to here directory is exhausted, so close it */
            closedir((DIR *)G.wild_dir);
            G.wild_dir = (zvoid *)NULL;
        }
        Trace((stderr, "do_wild:  opendir(%s) returns NULL\n",
          FnFilter1(G.dirname)));

        /* return the raw wildspec in case that works (e.g., directory not
         * searchable, but filespec was not wild and file is readable) */
        strncpy(G.matchname, wildspec, FILNAMSIZ);
        G.matchname[FILNAMSIZ-1] = '\0';
        return G.matchname;
    }

    /* last time through, might have failed opendir but returned raw wildspec */
    if ((DIR *)G.wild_dir == (DIR *)NULL) {
        G.notfirstcall = FALSE; /* nothing left--reset for new wildspec */
        if (G.have_dirname)
            free(G.dirname);
        return (char *)NULL;
    }

    /* If we've gotten this far, we've read and matched at least one entry
     * successfully (in a previous call), so dirname has been copied into
     * matchname already.
     */
    while ((file = readdir((DIR *)G.wild_dir)) != (struct dirent *)NULL) {
        Trace((stderr, "do_wild:  readdir returns %s\n",
          FnFilter1(file->d_name)));
        if (file->d_name[0] == '.' && G.wildname[0] != '.')
            continue;   /* Unix:  '*' and '?' do not match leading dot */
        if (match(file->d_name, G.wildname, 0 WISEP)) { /* 0 == case sens. */
            Trace((stderr, "do_wild:  match() succeeds\n"));
            if (G.have_dirname) {
                /* strcpy(G.matchname, G.dirname); */
                strcpy(G.matchname+G.dirnamelen, file->d_name);
            } else
                strcpy(G.matchname, file->d_name);
            return G.matchname;
        }
    }

    closedir((DIR *)G.wild_dir);  /* at least one entry read; nothing left */
    G.wild_dir = (zvoid *)NULL;
    G.notfirstcall = FALSE;       /* reset for new wildspec */
    if (G.have_dirname)
        free(G.dirname);
    return (char *)NULL;

} /* end function do_wild() */

#endif /* !SFX */




#ifndef S_ISUID
# define S_ISUID        0004000 /* set user id on execution */
#endif
#ifndef S_ISGID
# define S_ISGID        0002000 /* set group id on execution */
#endif
#ifndef S_ISVTX
# define S_ISVTX        0001000 /* save swapped text even after use */
#endif

/************************/
/*  Function filtattr() */
/************************/
/* This is used to clear or keep the SUID and SGID bits on file permissions.
 * It's possible that a file in an archive could have one of these bits set
 * and, unknown to the person unzipping, could allow others to execute the
 * file as the user or group.  The new option -K bypasses this check.
 */

static unsigned filtattr(__G__ perms)
    __GDEF
    unsigned perms;
{
    /* keep setuid/setgid/tacky perms? */
    if (!uO.K_flag)
        perms &= ~(S_ISUID | S_ISGID | S_ISVTX);

    return (0xffff & perms);
} /* end function filtattr() */





/**********************/
/* Function mapattr() */
/**********************/

int mapattr(__G)
    __GDEF
{
    int r;
    ulg tmp = G.crec.external_file_attributes;

    G.pInfo->file_attr = 0;
    /* initialized to 0 for check in "default" branch below... */

    switch (G.pInfo->hostnum) {
        case AMIGA_:
            tmp = (unsigned)(tmp>>17 & 7);   /* Amiga RWE bits */
            G.pInfo->file_attr = (unsigned)(tmp<<6 | tmp<<3 | tmp);
            break;
        case THEOS_:
            tmp &= 0xF1FFFFFFL;
            if ((tmp & 0xF0000000L) != 0x40000000L)
                tmp &= 0x01FFFFFFL;     /* not a dir, mask all ftype bits */
            else
                tmp &= 0x41FFFFFFL;     /* leave directory bit as set */
            /* fall through! */
        case UNIX_:
        case VMS_:
        case ACORN_:
        case ATARI_:
        case ATHEOS_:
        case BEOS_:
        case QDOS_:
        case TANDEM_:
            r = FALSE;
            G.pInfo->file_attr = (unsigned)(tmp >> 16);
            if (G.pInfo->file_attr == 0 && G.extra_field) {
                /* Some (non-Info-ZIP) implementations of Zip for Unix and
                 * VMS (and probably others ??) leave 0 in the upper 16-bit
                 * part of the external_file_attributes field. Instead, they
                 * store file permission attributes in some extra field.
                 * As a work-around, we search for the presence of one of
                 * these extra fields and fall back to the MSDOS compatible
                 * part of external_file_attributes if one of the known
                 * e.f. types has been detected.
                 * Later, we might implement extraction of the permission
                 * bits from the VMS extra field. But for now, the work-around
                 * should be sufficient to provide "readable" extracted files.
                 * (For ASI Unix e.f., an experimental remap of the e.f.
                 * mode value IS already provided!)
                 */
                ush ebID;
                unsigned ebLen;
                uch *ef = G.extra_field;
                unsigned ef_len = G.crec.extra_field_length;

                while (!r && ef_len >= EB_HEADSIZE) {
                    ebID = makeword(ef);
                    ebLen = (unsigned)makeword(ef+EB_LEN);
                    if (ebLen > (ef_len - EB_HEADSIZE))
                        /* discoverd some e.f. inconsistency! */
                        break;
                    switch (ebID) {
                      case EF_ASIUNIX:
                        if (ebLen >= (EB_ASI_MODE+2)) {
                            G.pInfo->file_attr =
                              (unsigned)makeword(ef+(EB_HEADSIZE+EB_ASI_MODE));
                            /* force stop of loop: */
                            ef_len = (ebLen + EB_HEADSIZE);
                            break;
                        }
                        /* else: fall through! */
                      case EF_PKVMS:
                        /* "found nondecypherable e.f. with perm. attr" */
                        r = TRUE;
                      default:
                        break;
                    }
                    ef_len -= (ebLen + EB_HEADSIZE);
                    ef += (ebLen + EB_HEADSIZE);
                }
            }
            if (!r) {
#ifdef SYMLINKS
                /* Check if the file is a (POSIX-compatible) symbolic link.
                 * We restrict symlink support to those "made-by" hosts that
                 * are known to support symbolic links.
                 */
                G.pInfo->symlink = S_ISLNK(G.pInfo->file_attr) &&
                                   SYMLINK_HOST(G.pInfo->hostnum);
#endif
                return 0;
            }
            /* fall through! */
        /* all remaining cases:  expand MSDOS read-only bit into write perms */
        case FS_FAT_:
            /* PKWARE's PKZip for Unix marks entries as FS_FAT_, but stores the
             * Unix attributes in the upper 16 bits of the external attributes
             * field, just like Info-ZIP's Zip for Unix.  We try to use that
             * value, after a check for consistency with the MSDOS attribute
             * bits (see below).
             */
            G.pInfo->file_attr = (unsigned)(tmp >> 16);
            /* fall through! */
        case FS_HPFS_:
        case FS_NTFS_:
        case MAC_:
        case TOPS20_:
        default:
            /* Ensure that DOS subdir bit is set when the entry's name ends
             * in a '/'.  Some third-party Zip programs fail to set the subdir
             * bit for directory entries.
             */
            if ((tmp & 0x10) == 0) {
                extent fnlen = strlen(G.filename);
                if (fnlen > 0 && G.filename[fnlen-1] == '/')
                    tmp |= 0x10;
            }
            /* read-only bit --> write perms; subdir bit --> dir exec bit */
            tmp = !(tmp & 1) << 1  |  (tmp & 0x10) >> 4;
            if ((G.pInfo->file_attr & 0700) == (unsigned)(0400 | tmp<<6)) {
                /* keep previous G.pInfo->file_attr setting, when its "owner"
                 * part appears to be consistent with DOS attribute flags!
                 */
#ifdef SYMLINKS
                /* Entries "made by FS_FAT_" could have been zipped on a
                 * system that supports POSIX-style symbolic links.
                 */
                G.pInfo->symlink = S_ISLNK(G.pInfo->file_attr) &&
                                   (G.pInfo->hostnum == FS_FAT_);
#endif
                return 0;
            }
            G.pInfo->file_attr = (unsigned)(0444 | tmp<<6 | tmp<<3 | tmp);
            break;
    } /* end switch (host-OS-created-by) */

    /* for originating systems with no concept of "group," "other," "system": */
    umask( (int)(tmp=umask(0)) );    /* apply mask to expanded r/w(/x) perms */
    G.pInfo->file_attr &= ~tmp;

    return 0;

} /* end function mapattr() */





/************************/
/*  Function mapname()  */
/************************/

int mapname(__G__ renamed)
    __GDEF
    int renamed;
/*
 * returns:
 *  MPN_OK          - no problem detected
 *  MPN_INF_TRUNC   - caution (truncated filename)
 *  MPN_INF_SKIP    - info "skip entry" (dir doesn't exist)
 *  MPN_ERR_SKIP    - error -> skip entry
 *  MPN_ERR_TOOLONG - error -> path is too long
 *  MPN_NOMEM       - error (memory allocation failed) -> skip entry
 *  [also MPN_VOL_LABEL, MPN_CREATED_DIR]
 */
{
    char pathcomp[FILNAMSIZ];      /* path-component buffer */
    char *pp, *cp=(char *)NULL;    /* character pointers */
    char *lastsemi=(char *)NULL;   /* pointer to last semi-colon in pathcomp */
#ifdef ACORN_FTYPE_NFS
    char *lastcomma=(char *)NULL;  /* pointer to last comma in pathcomp */
    RO_extra_block *ef_spark;      /* pointer Acorn FTYPE ef block */
#endif
    int killed_ddot = FALSE;       /* is set when skipping "../" pathcomp */
    int error = MPN_OK;
    register unsigned workch;      /* hold the character being tested */


/*---------------------------------------------------------------------------
    Initialize various pointers and counters and stuff.
  ---------------------------------------------------------------------------*/

    if (G.pInfo->vollabel)
        return MPN_VOL_LABEL;   /* can't set disk volume labels in Unix */

    /* can create path as long as not just freshening, or if user told us */
    G.create_dirs = (!uO.fflag || renamed);

    G.created_dir = FALSE;      /* not yet */

    /* user gave full pathname:  don't prepend rootpath */
    G.renamed_fullpath = (renamed && (*G.filename == '/'));

    if (checkdir(__G__ (char *)NULL, INIT) == MPN_NOMEM)
        return MPN_NOMEM;       /* initialize path buffer, unless no memory */

    *pathcomp = '\0';           /* initialize translation buffer */
    pp = pathcomp;              /* point to translation buffer */
    if (uO.jflag)               /* junking directories */
        cp = (char *)strrchr(G.filename, '/');
    if (cp == (char *)NULL)     /* no '/' or not junking dirs */
        cp = G.filename;        /* point to internal zipfile-member pathname */
    else
        ++cp;                   /* point to start of last component of path */

/*---------------------------------------------------------------------------
    Begin main loop through characters in filename.
  ---------------------------------------------------------------------------*/

    while ((workch = (uch)*cp++) != 0) {

        switch (workch) {
            case '/':             /* can assume -j flag not given */
                *pp = '\0';
                if (strcmp(pathcomp, ".") == 0) {
                    /* don't bother appending "./" to the path */
                    *pathcomp = '\0';
                } else if (!uO.ddotflag && strcmp(pathcomp, "..") == 0) {
                    /* "../" dir traversal detected, skip over it */
                    *pathcomp = '\0';
                    killed_ddot = TRUE;     /* set "show message" flag */
                }
                /* when path component is not empty, append it now */
                if (*pathcomp != '\0' &&
                    ((error = checkdir(__G__ pathcomp, APPEND_DIR))
                     & MPN_MASK) > MPN_INF_TRUNC)
                    return error;
                pp = pathcomp;    /* reset conversion buffer for next piece */
                lastsemi = (char *)NULL; /* leave direct. semi-colons alone */
                break;

#ifdef __CYGWIN__   /* Cygwin runs on Win32, apply FAT/NTFS filename rules */
            case ':':         /* drive spec not stored, so no colon allowed */
            case '\\':        /* '\\' may come as normal filename char (not */
            case '<':         /*  dir sep char!) from unix-like file system */
            case '>':         /* no redirection symbols allowed either */
            case '|':         /* no pipe signs allowed */
            case '"':         /* no double quotes allowed */
            case '?':         /* no wildcards allowed */
            case '*':
                *pp++ = '_';  /* these rules apply equally to FAT and NTFS */
                break;
#endif

            case ';':             /* VMS version (or DEC-20 attrib?) */
                lastsemi = pp;
                *pp++ = ';';      /* keep for now; remove VMS ";##" */
                break;            /*  later, if requested */

#ifdef ACORN_FTYPE_NFS
            case ',':             /* NFS filetype extension */
                lastcomma = pp;
                *pp++ = ',';      /* keep for now; may need to remove */
                break;            /*  later, if requested */
#endif

#ifdef MTS
            case ' ':             /* change spaces to underscore under */
                *pp++ = '_';      /*  MTS; leave as spaces under Unix */
                break;
#endif

            default:
                /* disable control character filter when requested,
                 * else allow 8-bit characters (e.g. UTF-8) in filenames:
                 */
                if (uO.cflxflag ||
                    (isprint(workch) || (128 <= workch && workch <= 254)))
                    *pp++ = (char)workch;
        } /* end switch */

    } /* end while loop */

    /* Show warning when stripping insecure "parent dir" path components */
    if (killed_ddot && QCOND2) {
        Info(slide, 0, ((char *)slide,
          "warning:  skipped \"../\" path component(s) in %s\n",
          FnFilter1(G.filename)));
        if (!(error & ~MPN_MASK))
            error = (error & MPN_MASK) | PK_WARN;
    }

/*---------------------------------------------------------------------------
    Report if directory was created (and no file to create:  filename ended
    in '/'), check name to be sure it exists, and combine path and name be-
    fore exiting.
  ---------------------------------------------------------------------------*/

    if (G.filename[strlen(G.filename) - 1] == '/') {
        checkdir(__G__ G.filename, GETPATH);
        if (G.created_dir) {
            if (QCOND2) {
                Info(slide, 0, ((char *)slide, "   creating: %s\n",
                  FnFilter1(G.filename)));
            }
#ifndef NO_CHMOD
            if (uO.X_flag >= 0)
            {
                /* Filter out security-relevant attributes bits. */
                G.pInfo->file_attr = filtattr(__G__ G.pInfo->file_attr);
                /* When extracting non-UNIX directories or when extracting
                 * without UID/GID restoration or SGID preservation, any
                 * SGID flag inherited from the parent directory should be
                 * maintained to allow files extracted into this new folder
                 * to inherit the GID setting from the parent directory.
                 */
                if (G.pInfo->hostnum != UNIX_ ||
                 !((uO.X_flag > 0) || uO.K_flag)) {
                    /* preserve SGID bit when inherited from parent dir */
                    if (!SSTAT(G.filename, &G.statbuf)) {
                        G.pInfo->file_attr |= G.statbuf.st_mode & S_ISGID;
                    } else {
                        perror("Could not read directory attributes");
                    }
                }

                /* set approx. dir perms (make sure can still read/write in dir) */
                if (chmod(G.filename, G.pInfo->file_attr | 0700))
                    perror("chmod (directory attributes) error");
            }
#endif
            /* set dir time (note trailing '/') */
            return (error & ~MPN_MASK) | MPN_CREATED_DIR;
        }
        /* dir existed already; don't look for data to extract */
        return (error & ~MPN_MASK) | MPN_INF_SKIP;
    }

    *pp = '\0';                   /* done with pathcomp:  terminate it */

    /* if not saving them, remove VMS version numbers (appended ";###") */
    if (!uO.V_flag && lastsemi) {
        pp = lastsemi + 1;
        if (*pp != '\0') {        /* At least one digit is required. */
            while (isdigit((uch)(*pp)))
                ++pp;
            if (*pp == '\0')      /* only digits between ';' and end:  nuke */
                *lastsemi = '\0';
        }
    }

    /* On UNIX (and compatible systems), "." and ".." are reserved for
     * directory navigation and cannot be used as regular file names.
     * These reserved one-dot and two-dot names are mapped to "_" and "__".
     */
    if (strcmp(pathcomp, ".") == 0)
        *pathcomp = '_';
    else if (strcmp(pathcomp, "..") == 0)
        strcpy(pathcomp, "__");

#ifdef ACORN_FTYPE_NFS
    /* translate Acorn filetype information if asked to do so */
    if (uO.acorn_nfs_ext &&
        (ef_spark = (RO_extra_block *)
                    getRISCOSexfield(G.extra_field, G.lrec.extra_field_length))
        != (RO_extra_block *)NULL)
    {
        /* file *must* have a RISC OS extra field */
        long ft = (long)makelong(ef_spark->loadaddr);
        /*32-bit*/
        if (lastcomma) {
            pp = lastcomma + 1;
            while (isxdigit((uch)(*pp))) ++pp;
            if (pp == lastcomma+4 && *pp == '\0') *lastcomma='\0'; /* nuke */
        }
        if ((ft & 1<<31)==0) ft=0x000FFD00;
        sprintf(pathcomp+strlen(pathcomp), ",%03x", (int)(ft>>8) & 0xFFF);
    }
#endif /* ACORN_FTYPE_NFS */

    if (*pathcomp == '\0') {
        Info(slide, 1, ((char *)slide, "mapname:  conversion of %s failed\n",
          FnFilter1(G.filename)));
        return (error & ~MPN_MASK) | MPN_ERR_SKIP;
    }

    checkdir(__G__ pathcomp, APPEND_NAME);  /* returns 1 if truncated: care? */
    checkdir(__G__ G.filename, GETPATH);

    return error;

} /* end function mapname() */




#if 0  /*========== NOTES ==========*/

  extract-to dir:      a:path/
  buildpath:           path1/path2/ ...   (NULL-terminated)
  pathcomp:                filename

  mapname():
    loop over chars in zipfile member name
      checkdir(path component, COMPONENT | CREATEDIR) --> map as required?
        (d:/tmp/unzip/)                    (disk:[tmp.unzip.)
        (d:/tmp/unzip/jj/)                 (disk:[tmp.unzip.jj.)
        (d:/tmp/unzip/jj/temp/)            (disk:[tmp.unzip.jj.temp.)
    finally add filename itself and check for existence? (could use with rename)
        (d:/tmp/unzip/jj/temp/msg.outdir)  (disk:[tmp.unzip.jj.temp]msg.outdir)
    checkdir(name, GETPATH)     -->  copy path to name and free space

#endif /* 0 */




/***********************/
/* Function checkdir() */
/***********************/

int checkdir(__G__ pathcomp, flag)
    __GDEF
    char *pathcomp;
    int flag;
/*
 * returns:
 *  MPN_OK          - no problem detected
 *  MPN_INF_TRUNC   - (on APPEND_NAME) truncated filename
 *  MPN_INF_SKIP    - path doesn't exist, not allowed to create
 *  MPN_ERR_SKIP    - path doesn't exist, tried to create and failed; or path
 *                    exists and is not a directory, but is supposed to be
 *  MPN_ERR_TOOLONG - path is too long
 *  MPN_NOMEM       - can't allocate memory for filename buffers
 */
{
 /* static int rootlen = 0; */  /* length of rootpath */
 /* static char *rootpath;  */  /* user's "extract-to" directory */
 /* static char *buildpath; */  /* full path (so far) to extracted file */
 /* static char *end;       */  /* pointer to end of buildpath ('\0') */

#   define FN_MASK   7
#   define FUNCTION  (flag & FN_MASK)



/*---------------------------------------------------------------------------
    APPEND_DIR:  append the path component to the path being built and check
    for its existence.  If doesn't exist and we are creating directories, do
    so for this one; else signal success or error as appropriate.
  ---------------------------------------------------------------------------*/

    if (FUNCTION == APPEND_DIR) {
        int too_long = FALSE;
#ifdef SHORT_NAMES
        char *old_end = end;
#endif

        Trace((stderr, "appending dir segment [%s]\n", FnFilter1(pathcomp)));
        while ((*G.end = *pathcomp++) != '\0')
            ++G.end;
#ifdef SHORT_NAMES   /* path components restricted to 14 chars, typically */
        if ((G.end-old_end) > FILENAME_MAX)  /* GRR:  proper constant? */
            *(G.end = old_end + FILENAME_MAX) = '\0';
#endif

        /* GRR:  could do better check, see if overrunning buffer as we go:
         * check end-buildpath after each append, set warning variable if
         * within 20 of FILNAMSIZ; then if var set, do careful check when
         * appending.  Clear variable when begin new path. */

        /* next check: need to append '/', at least one-char name, '\0' */
        if ((G.end-G.buildpath) > FILNAMSIZ-3)
            too_long = TRUE;                    /* check if extracting dir? */
        if (SSTAT(G.buildpath, &G.statbuf)) {   /* path doesn't exist */
            if (!G.create_dirs) { /* told not to create (freshening) */
                free(G.buildpath);
                return MPN_INF_SKIP;    /* path doesn't exist: nothing to do */
            }
            if (too_long) {
                Info(slide, 1, ((char *)slide,
                  "checkdir error:  path too long: %s\n",
                  FnFilter1(G.buildpath)));
                free(G.buildpath);
                /* no room for filenames:  fatal */
                return MPN_ERR_TOOLONG;
            }
            if (mkdir(G.buildpath, 0777) == -1) {   /* create the directory */
                Info(slide, 1, ((char *)slide,
                  "checkdir error:  cannot create %s\n\
                 %s\n\
                 unable to process %s.\n",
                  FnFilter2(G.buildpath),
                  strerror(errno),
                  FnFilter1(G.filename)));
                free(G.buildpath);
                /* path didn't exist, tried to create, failed */
                return MPN_ERR_SKIP;
            }
            G.created_dir = TRUE;
        } else if (!S_ISDIR(G.statbuf.st_mode)) {
            Info(slide, 1, ((char *)slide,
              "checkdir error:  %s exists but is not directory\n\
                 unable to process %s.\n",
              FnFilter2(G.buildpath), FnFilter1(G.filename)));
            free(G.buildpath);
            /* path existed but wasn't dir */
            return MPN_ERR_SKIP;
        }
        if (too_long) {
            Info(slide, 1, ((char *)slide,
              "checkdir error:  path too long: %s\n", FnFilter1(G.buildpath)));
            free(G.buildpath);
            /* no room for filenames:  fatal */
            return MPN_ERR_TOOLONG;
        }
        *G.end++ = '/';
        *G.end = '\0';
        Trace((stderr, "buildpath now = [%s]\n", FnFilter1(G.buildpath)));
        return MPN_OK;

    } /* end if (FUNCTION == APPEND_DIR) */

/*---------------------------------------------------------------------------
    GETPATH:  copy full path to the string pointed at by pathcomp, and free
    G.buildpath.
  ---------------------------------------------------------------------------*/

    if (FUNCTION == GETPATH) {
        strcpy(pathcomp, G.buildpath);
        Trace((stderr, "getting and freeing path [%s]\n",
          FnFilter1(pathcomp)));
        free(G.buildpath);
        G.buildpath = G.end = (char *)NULL;
        return MPN_OK;
    }

/*---------------------------------------------------------------------------
    APPEND_NAME:  assume the path component is the filename; append it and
    return without checking for existence.
  ---------------------------------------------------------------------------*/

    if (FUNCTION == APPEND_NAME) {
#ifdef SHORT_NAMES
        char *old_end = end;
#endif

        Trace((stderr, "appending filename [%s]\n", FnFilter1(pathcomp)));
        while ((*G.end = *pathcomp++) != '\0') {
            ++G.end;
#ifdef SHORT_NAMES  /* truncate name at 14 characters, typically */
            if ((G.end-old_end) > FILENAME_MAX)    /* GRR:  proper constant? */
                *(G.end = old_end + FILENAME_MAX) = '\0';
#endif
            if ((G.end-G.buildpath) >= FILNAMSIZ) {
                *--G.end = '\0';
                Info(slide, 0x201, ((char *)slide,
                  "checkdir warning:  path too long; truncating\n\
                   %s\n                -> %s\n",
                  FnFilter1(G.filename), FnFilter2(G.buildpath)));
                return MPN_INF_TRUNC;   /* filename truncated */
            }
        }
        Trace((stderr, "buildpath now = [%s]\n", FnFilter1(G.buildpath)));
        /* could check for existence here, prompt for new name... */
        return MPN_OK;
    }

/*---------------------------------------------------------------------------
    INIT:  allocate and initialize buffer space for the file currently being
    extracted.  If file was renamed with an absolute path, don't prepend the
    extract-to path.
  ---------------------------------------------------------------------------*/

/* GRR:  for VMS and TOPS-20, add up to 13 to strlen */

    if (FUNCTION == INIT) {
        Trace((stderr, "initializing buildpath to "));
#ifdef ACORN_FTYPE_NFS
        if ((G.buildpath = (char *)malloc(strlen(G.filename)+G.rootlen+
                                          (uO.acorn_nfs_ext ? 5 : 1)))
#else
        if ((G.buildpath = (char *)malloc(strlen(G.filename)+G.rootlen+1))
#endif
            == (char *)NULL)
            return MPN_NOMEM;
        if ((G.rootlen > 0) && !G.renamed_fullpath) {
            strcpy(G.buildpath, G.rootpath);
            G.end = G.buildpath + G.rootlen;
        } else {
            *G.buildpath = '\0';
            G.end = G.buildpath;
        }
        Trace((stderr, "[%s]\n", FnFilter1(G.buildpath)));
        return MPN_OK;
    }

/*---------------------------------------------------------------------------
    ROOT:  if appropriate, store the path in rootpath and create it if
    necessary; else assume it's a zipfile member and return.  This path
    segment gets used in extracting all members from every zipfile specified
    on the command line.
  ---------------------------------------------------------------------------*/

#if (!defined(SFX) || defined(SFX_EXDIR))
    if (FUNCTION == ROOT) {
        Trace((stderr, "initializing root path to [%s]\n",
          FnFilter1(pathcomp)));
        if (pathcomp == (char *)NULL) {
            G.rootlen = 0;
            return MPN_OK;
        }
        if (G.rootlen > 0)      /* rootpath was already set, nothing to do */
            return MPN_OK;
        if ((G.rootlen = strlen(pathcomp)) > 0) {
            char *tmproot;

            if ((tmproot = (char *)malloc(G.rootlen+2)) == (char *)NULL) {
                G.rootlen = 0;
                return MPN_NOMEM;
            }
            strcpy(tmproot, pathcomp);
            if (tmproot[G.rootlen-1] == '/') {
                tmproot[--G.rootlen] = '\0';
            }
            if (G.rootlen > 0 && (SSTAT(tmproot, &G.statbuf) ||
                                  !S_ISDIR(G.statbuf.st_mode)))
            {   /* path does not exist */
                if (!G.create_dirs /* || iswild(tmproot) */ ) {
                    free(tmproot);
                    G.rootlen = 0;
                    /* skip (or treat as stored file) */
                    return MPN_INF_SKIP;
                }
                /* create the directory (could add loop here scanning tmproot
                 * to create more than one level, but why really necessary?) */
                if (mkdir(tmproot, 0777) == -1) {
                    Info(slide, 1, ((char *)slide,
                      "checkdir:  cannot create extraction directory: %s\n\
           %s\n",
                      FnFilter1(tmproot), strerror(errno)));
                    free(tmproot);
                    G.rootlen = 0;
                    /* path didn't exist, tried to create, and failed: */
                    /* file exists, or 2+ subdir levels required */
                    return MPN_ERR_SKIP;
                }
            }
            tmproot[G.rootlen++] = '/';
            tmproot[G.rootlen] = '\0';
            if ((G.rootpath = (char *)realloc(tmproot, G.rootlen+1)) == NULL) {
                free(tmproot);
                G.rootlen = 0;
                return MPN_NOMEM;
            }
            Trace((stderr, "rootpath now = [%s]\n", FnFilter1(G.rootpath)));
        }
        return MPN_OK;
    }
#endif /* !SFX || SFX_EXDIR */

/*---------------------------------------------------------------------------
    END:  free rootpath, immediately prior to program exit.
  ---------------------------------------------------------------------------*/

    if (FUNCTION == END) {
        Trace((stderr, "freeing rootpath\n"));
        if (G.rootlen > 0) {
            free(G.rootpath);
            G.rootlen = 0;
        }
        return MPN_OK;
    }

    return MPN_INVALID; /* should never reach */

} /* end function checkdir() */





#ifdef NO_MKDIR

/********************/
/* Function mkdir() */
/********************/

int mkdir_(path, mode)
    ZCONST char *path;
    int mode;   /* ignored */
/*
 * returns:   0 - successful
 *           -1 - failed (errno not set, however)
 */
{
    char command[FILNAMSIZ+40]; /* buffer for system() call */

    /* GRR 930416:  added single quotes around path to avoid bug with
     * creating directories with ampersands in name; not yet tested */
    sprintf(command, "IFS=\" \t\n\" /bin/mkdir '%s' 2>/dev/null", path);
    if (system(command))
        return -1;
    return 0;
}

#endif /* NO_MKDIR */




#if (!defined(MTS) || defined(SET_DIR_ATTRIB))
static int get_extattribs OF((__GPRO__ iztimes *pzt, ulg z_uidgid[2]));

static int get_extattribs(__G__ pzt, z_uidgid)
    __GDEF
    iztimes *pzt;
    ulg z_uidgid[2];
{
/*---------------------------------------------------------------------------
    Convert from MSDOS-format local time and date to Unix-format 32-bit GMT
    time:  adjust base year from 1980 to 1970, do usual conversions from
    yy/mm/dd hh:mm:ss to elapsed seconds, and account for timezone and day-
    light savings time differences.  If we have a Unix extra field, however,
    we're laughing:  both mtime and atime are ours.  On the other hand, we
    then have to check for restoration of UID/GID.
  ---------------------------------------------------------------------------*/
    int have_uidgid_flg;
    unsigned eb_izux_flg;

    eb_izux_flg = (G.extra_field ? ef_scan_for_izux(G.extra_field,
                   G.lrec.extra_field_length, 0, G.lrec.last_mod_dos_datetime,
#ifdef IZ_CHECK_TZ
                   (G.tz_is_valid ? pzt : NULL),
#else
                   pzt,
#endif
                   z_uidgid) : 0);
    if (eb_izux_flg & EB_UT_FL_MTIME) {
        TTrace((stderr, "\nget_extattribs:  Unix e.f. modif. time = %ld\n",
          pzt->mtime));
    } else {
        pzt->mtime = dos_to_unix_time(G.lrec.last_mod_dos_datetime);
    }
    if (eb_izux_flg & EB_UT_FL_ATIME) {
        TTrace((stderr, "get_extattribs:  Unix e.f. access time = %ld\n",
          pzt->atime));
    } else {
        pzt->atime = pzt->mtime;
        TTrace((stderr, "\nget_extattribs:  modification/access times = %ld\n",
          pzt->mtime));
    }

    /* if -X option was specified and we have UID/GID info, restore it */
    have_uidgid_flg =
#ifdef RESTORE_UIDGID
            ((uO.X_flag > 0) && (eb_izux_flg & EB_UX2_VALID));
#else
            0;
#endif
    return have_uidgid_flg;
}
#endif /* !MTS || SET_DIR_ATTRIB */



#ifndef MTS

/****************************/
/* Function close_outfile() */
/****************************/

void close_outfile(__G)    /* GRR: change to return PK-style warning level */
    __GDEF
{
    union {
        iztimes t3;             /* mtime, atime, ctime */
        ztimbuf t2;             /* modtime, actime */
    } zt;
    ulg z_uidgid[2];
    int have_uidgid_flg;

    have_uidgid_flg = get_extattribs(__G__ &(zt.t3), z_uidgid);

/*---------------------------------------------------------------------------
    If symbolic links are supported, allocate storage for a symlink control
    structure, put the uncompressed "data" and other required info in it, and
    add the structure to the "deferred symlinks" chain.  Since we know it's a
    symbolic link to start with, we shouldn't have to worry about overflowing
    unsigned ints with unsigned longs.
  ---------------------------------------------------------------------------*/

#ifdef SYMLINKS
    if (G.symlnk) {
        extent ucsize = (extent)G.lrec.ucsize;
# ifdef SET_SYMLINK_ATTRIBS
        extent attribsize = sizeof(unsigned) +
                            (have_uidgid_flg ? sizeof(z_uidgid) : 0);
# else
        extent attribsize = 0;
# endif
        /* size of the symlink entry is the sum of
         *  (struct size (includes 1st '\0') + 1 additional trailing '\0'),
         *  system specific attribute data size (might be 0),
         *  and the lengths of name and link target.
         */
        extent slnk_entrysize = (sizeof(slinkentry) + 1) + attribsize +
                                ucsize + strlen(G.filename);
        slinkentry *slnk_entry;

        if (slnk_entrysize < ucsize) {
            Info(slide, 0x201, ((char *)slide,
              "warning:  symbolic link (%s) failed: mem alloc overflow\n",
              FnFilter1(G.filename)));
            fclose(G.outfile);
            return;
        }

        if ((slnk_entry = (slinkentry *)malloc(slnk_entrysize)) == NULL) {
            Info(slide, 0x201, ((char *)slide,
              "warning:  symbolic link (%s) failed: no mem\n",
              FnFilter1(G.filename)));
            fclose(G.outfile);
            return;
        }
        slnk_entry->next = NULL;
        slnk_entry->targetlen = ucsize;
        slnk_entry->attriblen = attribsize;
# ifdef SET_SYMLINK_ATTRIBS
        memcpy(slnk_entry->buf, &(G.pInfo->file_attr),
               sizeof(unsigned));
        if (have_uidgid_flg)
            memcpy(slnk_entry->buf + 4, z_uidgid, sizeof(z_uidgid));
# endif
        slnk_entry->target = slnk_entry->buf + slnk_entry->attriblen;
        slnk_entry->fname = slnk_entry->target + ucsize + 1;
        strcpy(slnk_entry->fname, G.filename);

        /* move back to the start of the file to re-read the "link data" */
        rewind(G.outfile);

        if (fread(slnk_entry->target, 1, ucsize, G.outfile) != ucsize)
        {
            Info(slide, 0x201, ((char *)slide,
              "warning:  symbolic link (%s) failed\n",
              FnFilter1(G.filename)));
            free(slnk_entry);
            fclose(G.outfile);
            return;
        }
        fclose(G.outfile);                  /* close "link" file for good... */
        slnk_entry->target[ucsize] = '\0';
        if (QCOND2)
            Info(slide, 0, ((char *)slide, "-> %s ",
              FnFilter1(slnk_entry->target)));
        /* add this symlink record to the list of deferred symlinks */
        if (G.slink_last != NULL)
            G.slink_last->next = slnk_entry;
        else
            G.slink_head = slnk_entry;
        G.slink_last = slnk_entry;
        return;
    }
#endif /* SYMLINKS */

#ifdef QLZIP
    if (G.extra_field) {
        static void qlfix OF((__GPRO__ uch *ef_ptr, unsigned ef_len));

        qlfix(__G__ G.extra_field, G.lrec.extra_field_length);
    }
#endif

#if (defined(NO_FCHOWN))
    fclose(G.outfile);
#endif

    /* if -X option was specified and we have UID/GID info, restore it */
    if (have_uidgid_flg
        /* check that both uid and gid values fit into their data sizes */
        && ((ulg)(uid_t)(z_uidgid[0]) == z_uidgid[0])
        && ((ulg)(gid_t)(z_uidgid[1]) == z_uidgid[1])) {
        TTrace((stderr, "close_outfile:  restoring Unix UID/GID info\n"));
#if (defined(NO_FCHOWN))
        if (chown(G.filename, (uid_t)z_uidgid[0], (gid_t)z_uidgid[1]))
#else
        if (fchown(fileno(G.outfile), (uid_t)z_uidgid[0], (gid_t)z_uidgid[1]))
#endif
        {
            if (uO.qflag)
                Info(slide, 0x201, ((char *)slide, CannotSetItemUidGid,
                  z_uidgid[0], z_uidgid[1], FnFilter1(G.filename),
                  strerror(errno)));
            else
                Info(slide, 0x201, ((char *)slide, CannotSetUidGid,
                  z_uidgid[0], z_uidgid[1], strerror(errno)));
        }
    }

#if (!defined(NO_FCHOWN) && defined(NO_FCHMOD))
    fclose(G.outfile);
#endif

#if (!defined(NO_FCHOWN) && !defined(NO_FCHMOD))
/*---------------------------------------------------------------------------
    Change the file permissions from default ones to those stored in the
    zipfile.
  ---------------------------------------------------------------------------*/

# if defined( UNIX) && defined( __APPLE__)
    /* 2009-04-19 SMS.
     * Skip fchmod() for an AppleDouble file.  (Doing the normal file
     * is enough, and fchmod() will fail on a "/rsrc" pseudo-file.)
     */
    if (!G.apple_double)
    {
# endif /* defined( UNIX) && defined( __APPLE__) */

    if (uO.X_flag >= 0)
    {
        if (fchmod(fileno(G.outfile), filtattr(__G__ G.pInfo->file_attr)))
            perror("fchmod (file attributes) error");
    }

# if defined( UNIX) && defined( __APPLE__)
    }
# endif /* defined( UNIX) && defined( __APPLE__) */

    fclose(G.outfile);
#endif /* !NO_FCHOWN && !NO_FCHMOD */

# if defined( UNIX) && defined( __APPLE__)
    /* 2009-04-19 SMS.
     * Skip utime() for an AppleDouble file.  (Doing the normal file
     * is enough, and utime() will fail on a "/rsrc" pseudo-file.)
     */
    if (!G.apple_double)
    {
# endif /* defined( UNIX) && defined( __APPLE__) */

    /* skip restoring time stamps on user's request */
    if (uO.D_flag <= 1) {
        /* set the file's access and modification times */
        if (utime(G.filename, (void *)&(zt.t2))) {
            if (uO.qflag)
                Info(slide, 0x201, ((char *)slide, CannotSetItemTimestamps,
                  FnFilter1(G.filename), strerror(errno)));
            else
                Info(slide, 0x201, ((char *)slide, CannotSetTimestamps,
                  strerror(errno)));
        }
    }

# if defined( UNIX) && defined( __APPLE__)
    }
# endif /* defined( UNIX) && defined( __APPLE__) */

#if (defined(NO_FCHOWN) || defined(NO_FCHMOD))
/*---------------------------------------------------------------------------
    Change the file permissions from default ones to those stored in the
    zipfile.
  ---------------------------------------------------------------------------*/

#ifndef NO_CHMOD
    if (uO.X_flag >= 0)
    {
        if (chmod(G.filename, filtattr(__G__ G.pInfo->file_attr)))
            perror("chmod (file attributes) error");
    }
#endif /* ndef NO_CHMOD */
#endif /* NO_FCHOWN || NO_FCHMOD */

} /* end function close_outfile() */

#endif /* !MTS */


#if (defined(SYMLINKS) && defined(SET_SYMLINK_ATTRIBS))
int set_symlnk_attribs(__G__ slnk_entry)
    __GDEF
    slinkentry *slnk_entry;
{
    ulg z_uidgid[2];
    (void)z_uidgid;

    if (slnk_entry->attriblen > 0) {
# if (!defined(NO_LCHOWN))
      if (slnk_entry->attriblen > sizeof(unsigned)) {
        ulg *z_uidgid_p = (zvoid *)(slnk_entry->buf + sizeof(unsigned));
        /* check that both uid and gid values fit into their data sizes */
        if (((ulg)(uid_t)(z_uidgid_p[0]) == z_uidgid_p[0]) &&
            ((ulg)(gid_t)(z_uidgid_p[1]) == z_uidgid_p[1])) {
          TTrace((stderr,
            "set_symlnk_attribs:  restoring Unix UID/GID info for\n\
        %s\n",
            FnFilter1(slnk_entry->fname)));
          if (lchown(slnk_entry->fname,
                     (uid_t)z_uidgid_p[0], (gid_t)z_uidgid_p[1]))
          {
            Info(slide, 0x201, ((char *)slide, CannotSetItemUidGid,
              z_uidgid_p[0], z_uidgid_p[1], FnFilter1(slnk_entry->fname),
              strerror(errno)));
          }
        }
      }
# endif /* !NO_LCHOWN */
# if (!defined(NO_LCHMOD))
      if (uO.X_flag >= 0)
      {
          TTrace((stderr,
            "set_symlnk_attribs:  restoring Unix attributes for\n        %s\n",
            FnFilter1(slnk_entry->fname)));
          if (lchmod(slnk_entry->fname,
           filtattr(__G__ *(unsigned *)(zvoid *)slnk_entry->buf)))
              perror("lchmod (file attributes) error");
        }
# endif /* !NO_LCHMOD */
    }
    /* currently, no error propagation... */
    return PK_OK;
} /* end function set_symlnk_attribs() */
#endif /* SYMLINKS && SET_SYMLINK_ATTRIBS */


#ifdef SET_DIR_ATTRIB
/* messages of code for setting directory attributes */
#  ifndef NO_CHMOD
  static ZCONST char DirlistChmodFailed[] =
    "warning:  cannot set permissions for %s\n          %s\n";
#  endif


int defer_dir_attribs(__G__ pd)
    __GDEF
    direntry **pd;
{
    uxdirattr *d_entry;

    d_entry = (uxdirattr *)malloc(sizeof(uxdirattr) + strlen(G.filename));
    *pd = (direntry *)d_entry;
    if (d_entry == (uxdirattr *)NULL) {
        return PK_MEM;
    }
    d_entry->fn = d_entry->fnbuf;
    strcpy(d_entry->fn, G.filename);

    d_entry->perms = G.pInfo->file_attr;

    d_entry->have_uidgid = get_extattribs(__G__ &(d_entry->u.t3),
                                          d_entry->uidgid);
    return PK_OK;
} /* end function defer_dir_attribs() */


int set_direc_attribs(__G__ d)
    __GDEF
    direntry *d;
{
    int errval = PK_OK;

    if (UxAtt(d)->have_uidgid &&
        /* check that both uid and gid values fit into their data sizes */
        ((ulg)(uid_t)(UxAtt(d)->uidgid[0]) == UxAtt(d)->uidgid[0]) &&
        ((ulg)(gid_t)(UxAtt(d)->uidgid[1]) == UxAtt(d)->uidgid[1]) &&
        chown(UxAtt(d)->fn, (uid_t)UxAtt(d)->uidgid[0],
              (gid_t)UxAtt(d)->uidgid[1]))
    {
        Info(slide, 0x201, ((char *)slide, CannotSetItemUidGid,
          UxAtt(d)->uidgid[0], UxAtt(d)->uidgid[1], FnFilter1(d->fn),
          strerror(errno)));
        if (!errval)
            errval = PK_WARN;
    }
    /* Skip restoring directory time stamps on user' request. */
    if (uO.D_flag <= 0) {
        /* restore directory timestamps */
        if (utime(d->fn, (void *)&UxAtt(d)->u.t2)) {
            Info(slide, 0x201, ((char *)slide, CannotSetItemTimestamps,
              FnFilter1(d->fn), strerror(errno)));
            if (!errval)
                errval = PK_WARN;
        }
    }
#ifndef NO_CHMOD
    if (uO.X_flag >= 0)
    {
        if (chmod(d->fn, UxAtt(d)->perms)) {
            Info(slide, 0x201, ((char *)slide, DirlistChmodFailed,
              FnFilter1(d->fn), strerror(errno)));
            if (!errval)
                errval = PK_WARN;
        }
    }
#endif /* !NO_CHMOD */
    return errval;
} /* end function set_direc_attribs() */

#endif /* SET_DIR_ATTRIB */




#ifdef TIMESTAMP

/***************************/
/*  Function stamp_file()  */
/***************************/

int stamp_file(fname, modtime)
    ZCONST char *fname;
    time_t modtime;
{
    ztimbuf tp;

    tp.modtime = tp.actime = modtime;
    return (utime(fname, (void *)&tp));

} /* end function stamp_file() */

#endif /* TIMESTAMP */




#ifndef SFX

/************************/
/*  Function version()  */
/************************/

void version(__G)
    __GDEF
{
#if (defined(__GNUC__) && defined(NX_CURRENT_COMPILER_RELEASE))
    char cc_namebuf[40];
    char cc_versbuf[40];
#else
#if (defined(__SUNPRO_C))
    char cc_versbuf[17];
#else
#if (defined(__HP_cc) || defined(__IBMC__))
    char cc_versbuf[25];
#else
#if (defined(__DECC_VER))
    char cc_versbuf[17];
    int cc_verstyp;
#else
#if (defined(CRAY) && defined(_RELEASE))
    char cc_versbuf[40];
#endif /* (CRAY && _RELEASE) */
#endif /* __DECC_VER */
#endif /* __HP_cc || __IBMC__ */
#endif /* __SUNPRO_C */
#endif /* (__GNUC__ && NX_CURRENT_COMPILER_RELEASE) */

#if ((defined(CRAY) || defined(cray)) && defined(_UNICOS))
    char os_namebuf[40];
#else
#if defined(__NetBSD__)
    char os_namebuf[40];
#endif
#endif

    /* Pyramid, NeXT have problems with huge macro expansion, too:  no Info() */
    sprintf((char *)slide, LoadFarString(CompiledWith),

#ifdef __GNUC__
#  ifdef NX_CURRENT_COMPILER_RELEASE
      (sprintf(cc_namebuf, "NeXT DevKit %d.%02d ",
        NX_CURRENT_COMPILER_RELEASE/100, NX_CURRENT_COMPILER_RELEASE%100),
       cc_namebuf),
      (strlen(__VERSION__) > 8)? "(gcc)" :
        (sprintf(cc_versbuf, "(gcc %s)", __VERSION__), cc_versbuf),
#  else
      "gcc ", __VERSION__,
#  endif
#else
#if defined(__SUNPRO_C)
      "Sun C ", (sprintf(cc_versbuf, "version %x", __SUNPRO_C), cc_versbuf),
#else
#if (defined(__HP_cc))
      "HP C ",
      (((__HP_cc% 100) == 0) ?
      (sprintf(cc_versbuf, "version A.%02d.%02d",
      (__HP_cc/ 10000), ((__HP_cc% 10000)/ 100))) :
      (sprintf(cc_versbuf, "version A.%02d.%02d.%02d",
      (__HP_cc/ 10000), ((__HP_cc% 10000)/ 100), (__HP_cc% 100))),
      cc_versbuf),
#else
#if (defined(__DECC_VER))
      "DEC C ",
      (sprintf(cc_versbuf, "%c%d.%d-%03d",
               ((cc_verstyp = (__DECC_VER / 10000) % 10) == 6 ? 'T' :
                (cc_verstyp == 8 ? 'S' : 'V')),
               __DECC_VER / 10000000,
               (__DECC_VER % 10000000) / 100000, __DECC_VER % 1000),
               cc_versbuf),
#else
#if defined(CRAY) && defined(_RELEASE)
      "cc ", (sprintf(cc_versbuf, "version %d", _RELEASE), cc_versbuf),
#else
#ifdef __IBMC__
      "IBM C ",
      (sprintf(cc_versbuf, "version %d.%d.%d",
               (__IBMC__ / 100), ((__IBMC__ / 10) % 10), (__IBMC__ % 10)),
               cc_versbuf),
#else
#ifdef __VERSION__
#   ifndef IZ_CC_NAME
#    define IZ_CC_NAME "cc "
#   endif
      IZ_CC_NAME, __VERSION__
#else
#   ifndef IZ_CC_NAME
#    define IZ_CC_NAME "cc"
#   endif
      IZ_CC_NAME, "",
#endif /* ?__VERSION__ */
#endif /* ?__IBMC__ */
#endif /* ?(CRAY && _RELEASE) */
#endif /* ?__DECC_VER */
#endif /* ?__HP_cc */
#endif /* ?__SUNPRO_C */
#endif /* ?__GNUC__ */

#ifndef IZ_OS_NAME
#  define IZ_OS_NAME "Unix"
#endif
      IZ_OS_NAME,

#if defined(sgi) || defined(__sgi)
      " (Silicon Graphics IRIX)",
#else
#ifdef sun
#  if defined(UNAME_P) && defined(UNAME_R) && defined(UNAME_S) 
      " ("UNAME_S" "UNAME_R" "UNAME_P")",
#  else
#  ifdef sparc
#    ifdef __SVR4
      " (Sun SPARC/Solaris)",
#    else /* may or may not be SunOS */
      " (Sun SPARC)",
#    endif
#  else
#  if defined(sun386) || defined(i386)
      " (Sun 386i)",
#  else
#  if defined(mc68020) || defined(__mc68020__)
      " (Sun 3)",
#  else /* mc68010 or mc68000:  Sun 2 or earlier */
      " (Sun 2)",
#  endif
#  endif
#  endif
#  endif
#else /* def sun */
#ifdef __hpux
#  if defined(UNAME_M) && defined(UNAME_R) && defined(UNAME_S) 
      " ("UNAME_S" "UNAME_R" "UNAME_M")",
#  else
      " (HP-UX)",
#  endif
#else
#ifdef __osf__
#  if defined( SIZER_V)
      " (Tru64 "SIZER_V")"
#  else /* defined( SIZER_V) */
      " (Tru64)",
#  endif /* defined( SIZER_V) [else] */
#else
#ifdef _AIX
#  if defined( UNAME_R) && defined( UNAME_S) && defined( UNAME_V)
      " ("UNAME_S" "UNAME_V"."UNAME_R")",
#  else /*  */
      " (IBM AIX)",
#  endif /* [else] */
#else
#ifdef aiws
      " (IBM RT/AIX)",
#else
#if defined(CRAY) || defined(cray)
#  ifdef _UNICOS
      (sprintf(os_namebuf, " (Cray UNICOS release %d)", _UNICOS), os_namebuf),
#  else
      " (Cray UNICOS)",
#  endif
#else
#if defined(uts) || defined(UTS)
      " (Amdahl UTS)",
#else
#ifdef NeXT
#  ifdef mc68000
      " (NeXTStep/black)",
#  else
      " (NeXTStep for Intel)",
#  endif
#else              /* the next dozen or so are somewhat order-dependent */
#ifdef LINUX
#  if defined( UNAME_M) && defined( UNAME_O)
      " ("UNAME_O" "UNAME_M")",
#  else
#    ifdef __ELF__
      " (Linux ELF)",
#    else
      " (Linux a.out)",
#    endif
#  endif
#else
#ifdef MINIX
      " (Minix)",
#else
#ifdef M_UNIX
      " (SCO Unix)",
#else
#ifdef M_XENIX
      " (SCO Xenix)",
#else
#ifdef __NetBSD__
#  ifdef NetBSD0_8
      (sprintf(os_namebuf, " (NetBSD 0.8%c)", (char)(NetBSD0_8 - 1 + 'A')),
       os_namebuf),
#  else
#  ifdef NetBSD0_9
      (sprintf(os_namebuf, " (NetBSD 0.9%c)", (char)(NetBSD0_9 - 1 + 'A')),
       os_namebuf),
#  else
#  ifdef NetBSD1_0
      (sprintf(os_namebuf, " (NetBSD 1.0%c)", (char)(NetBSD1_0 - 1 + 'A')),
       os_namebuf),
#  else
      (BSD4_4 == 0.5)? " (NetBSD before 0.9)" : " (NetBSD 1.1 or later)",
#  endif
#  endif
#  endif
#else
#ifdef __FreeBSD__
      (BSD4_4 == 0.5)? " (FreeBSD 1.x)" : " (FreeBSD 2.0 or later)",
#else
#ifdef __bsdi__
      (BSD4_4 == 0.5)? " (BSD/386 1.0)" : " (BSD/386 1.1 or later)",
#else
#ifdef __386BSD__
      (BSD4_4 == 1)? " (386BSD, post-4.4 release)" : " (386BSD)",
#else
#ifdef __CYGWIN__
      " (Cygwin)",
#else
#if defined(i686) || defined(__i686) || defined(__i686__)
      " (Intel 686)",
#else
#if defined(i586) || defined(__i586) || defined(__i586__)
      " (Intel 586)",
#else
#if defined(i486) || defined(__i486) || defined(__i486__)
      " (Intel 486)",
#else
#if defined(i386) || defined(__i386) || defined(__i386__)
      " (Intel 386)",
#else
#ifdef pyr
      " (Pyramid)",
#else
#ifdef ultrix
#  ifdef mips
      " (DEC/MIPS)",
#  else
#  ifdef vax
      " (DEC/VAX)",
#  else /* __alpha? */
      " (DEC/Alpha)",
#  endif
#  endif
#else
#ifdef gould
      " (Gould)",
#else
#ifdef MTS
      " (MTS)",
#else
#ifdef __convexc__
      " (Convex)",
#else
#ifdef __QNX__
      " (QNX 4)",
#else
#ifdef __QNXNTO__
      " (QNX Neutrino)",
#else
#ifdef Lynx
      " (LynxOS)",
#else
#ifdef __APPLE__
#  if defined(UNAME_P) && defined(UNAME_R) && defined(UNAME_S) 
      " ("UNAME_S" "UNAME_R" "UNAME_P")",
#  else
#  ifdef __i386__
      " (Mac OS X Intel i32)",
#  else
#  ifdef __ppc__
      " (Mac OS X PowerPC)",
#  else
#  ifdef __ppc64__
      " (Mac OS X PowerPC64)",
#  else
      " (Mac OS X"),
#  endif /* __ppc64__ */
#  endif /* __ppc__ */
#  endif /* __i386__ */
#  endif
#else
      "",
#endif /* Apple */
#endif /* Lynx */
#endif /* QNX Neutrino */
#endif /* QNX 4 */
#endif /* Convex */
#endif /* MTS */
#endif /* Gould */
#endif /* DEC */
#endif /* Pyramid */
#endif /* 386 */
#endif /* 486 */
#endif /* 586 */
#endif /* 686 */
#endif /* Cygwin */
#endif /* 386BSD */
#endif /* BSDI BSD/386 */
#endif /* NetBSD */
#endif /* FreeBSD */
#endif /* SCO Xenix */
#endif /* SCO Unix */
#endif /* Minix */
#endif /* Linux */
#endif /* NeXT */
#endif /* Amdahl */
#endif /* Cray */
#endif /* RT/AIX */
#endif /* AIX */
#endif /* OSF/1 */
#endif /* HP-UX */
#endif /* Sun */
#endif /* SGI */

#ifdef __DATE__
      " on ", __DATE__
#else
      "", ""
#endif
    );

    (*G.message)((zvoid *)&G, slide, (ulg)strlen((char *)slide), 0);

} /* end function version() */

#endif /* !SFX */




#ifdef QLZIP

struct qdirect  {
    long            d_length __attribute__ ((packed));  /* file length */
    unsigned char   d_access __attribute__ ((packed));  /* file access type */
    unsigned char   d_type __attribute__ ((packed));    /* file type */
    long            d_datalen __attribute__ ((packed)); /* data length */
    long            d_reserved __attribute__ ((packed));/* Unused */
    short           d_szname __attribute__ ((packed));  /* size of name */
    char            d_name[36] __attribute__ ((packed));/* name area */
    long            d_update __attribute__ ((packed));  /* last update */
    long            d_refdate __attribute__ ((packed));
    long            d_backup __attribute__ ((packed));   /* EOD */
};

#define LONGID  "QDOS02"
#define EXTRALEN (sizeof(struct qdirect) + 8)
#define JBLONGID    "QZHD"
#define JBEXTRALEN  (sizeof(jbextra)  - 4 * sizeof(char))

typedef struct {
    char        eb_header[4] __attribute__ ((packed));  /* place_holder */
    char        longid[8] __attribute__ ((packed));
    struct      qdirect     header __attribute__ ((packed));
} qdosextra;

typedef struct {
    char        eb_header[4];                           /* place_holder */
    char        longid[4];
    struct      qdirect     header;
} jbextra;



/*  The following two functions SH() and LG() convert big-endian short
 *  and long numbers into native byte order.  They are some kind of
 *  counterpart to the generic UnZip's makeword() and makelong() functions.
 */
static ush SH(ush val)
{
    uch swapbuf[2];

    swapbuf[1] = (uch)(val & 0xff);
    swapbuf[0] = (uch)(val >> 8);
    return (*(ush *)swapbuf);
}



static ulg LG(ulg val)
{
    /*  convert the big-endian unsigned long number `val' to the machine
     *  dependent representation
     */
    ush swapbuf[2];

    swapbuf[1] = SH((ush)(val & 0xffff));
    swapbuf[0] = SH((ush)(val >> 16));
    return (*(ulg *)swapbuf);
}



static void qlfix(__G__ ef_ptr, ef_len)
    __GDEF
    uch *ef_ptr;
    unsigned ef_len;
{
    while (ef_len >= EB_HEADSIZE)
    {
        unsigned    eb_id  = makeword(EB_ID + ef_ptr);
        unsigned    eb_len = makeword(EB_LEN + ef_ptr);

        if (eb_len > (ef_len - EB_HEADSIZE)) {
            /* discovered some extra field inconsistency! */
            Trace((stderr,
              "qlfix: block length %u > rest ef_size %u\n", eb_len,
              ef_len - EB_HEADSIZE));
            break;
        }

        switch (eb_id) {
          case EF_QDOS:
          {
            struct _ntc_
            {
                long id;
                long dlen;
            } ntc;
            long dlen = 0;

            qdosextra   *extra = (qdosextra *)ef_ptr;
            jbextra     *jbp   = (jbextra   *)ef_ptr;

            if (!strncmp(extra->longid, LONGID, strlen(LONGID)))
            {
                if (eb_len != EXTRALEN)
                    if (uO.qflag)
                        Info(slide, 0x201, ((char *)slide,
                          "warning:  invalid length in Qdos field for %s\n",
                          FnFilter1(G.filename)));
                    else
                        Info(slide, 0x201, ((char *)slide,
                          "warning:  invalid length in Qdos field"));

                if (extra->header.d_type)
                {
                    dlen = extra->header.d_datalen;
                }
            }

            if (!strncmp(jbp->longid, JBLONGID, strlen(JBLONGID)))
            {
                if (eb_len != JBEXTRALEN)
                    if (uO.qflag)
                        Info(slide, 0x201, ((char *)slide,
                          "warning:  invalid length in QZ field for %s\n",
                          FnFilter1(G.filename)));
                    else
                        Info(slide, 0x201, ((char *)slide,
                          "warning:  invalid length in QZ field"));
                if (jbp->header.d_type)
                {
                    dlen = jbp->header.d_datalen;
                }
            }

            if ((long)LG(dlen) > 0)
            {
                zfseeko(G.outfile, -8, SEEK_END);
                fread(&ntc, 8, 1, G.outfile);
                if (ntc.id != *(long *)"XTcc")
                {
                    ntc.id = *(long *)"XTcc";
                    ntc.dlen = dlen;
                    fwrite (&ntc, 8, 1, G.outfile);
                }
                Info(slide, 0x201, ((char *)slide, "QData = %d", LG(dlen)));
            }
            return;     /* finished, cancel further extra field scanning */
          }

          default:
            Trace((stderr,"qlfix: unknown extra field block, ID=%d\n",
               eb_id));
        }

        /* Skip this extra field block */
        ef_ptr += (eb_len + EB_HEADSIZE);
        ef_len -= (eb_len + EB_HEADSIZE);
    }
}
#endif /* QLZIP */


#ifdef USE_ICONV_MAPPING
typedef struct {
    char *local_charset;
    char *archive_charset;
} CHARSET_MAP;

/* A mapping of local <-> archive charsets used by default to convert filenames
 * of DOS/Windows Zip archives. Currently very basic. */
static CHARSET_MAP dos_charset_map[] = {
    { "ANSI_X3.4-1968", "CP850" },
    { "ISO-8859-1", "CP850" },
    { "CP1252", "CP850" },
    { "UTF-8", "CP866" },
    { "KOI8-R", "CP866" },
    { "KOI8-U", "CP866" },
    { "ISO-8859-5", "CP866" }
};

char OEM_CP[MAX_CP_NAME] = "";
char ISO_CP[MAX_CP_NAME] = "";

/* Try to guess the default value of OEM_CP based on the current locale.
 * ISO_CP is left alone for now. */
void init_conversion_charsets()
{
    const char *local_charset;
    int i;

    /* Make a guess only if OEM_CP not already set. */ 
    if(*OEM_CP == '\0') {
    	local_charset = nl_langinfo(CODESET);
    	for(i = 0; i < sizeof(dos_charset_map)/sizeof(CHARSET_MAP); i++)
    		if(!strcasecmp(local_charset, dos_charset_map[i].local_charset)) {
    			strncpy(OEM_CP, dos_charset_map[i].archive_charset,
    					sizeof(OEM_CP));
    			break;
    		}
    }
}

/* Convert a string from one encoding to the current locale using iconv().
 * Be as non-intrusive as possible. If error is encountered during
 * convertion just leave the string intact. */
static void charset_to_intern(char *string, char *from_charset)
{
    iconv_t cd;
    char *s,*d, *buf;
    size_t slen, dlen, buflen;
    const char *local_charset;

    if (*from_charset == '\0')
        return;

    buf = NULL;
    local_charset = nl_langinfo(CODESET);

    if ((cd = iconv_open(local_charset, from_charset)) == (iconv_t)-1)
        return;

    slen = strlen(string);
    s = string;
    dlen = buflen = 2 * slen;
    d = buf = malloc(buflen + 1);
    if (d) {
        bzero(buf, buflen);
        if(iconv(cd, &s, &slen, &d, &dlen) != (size_t)-1)
            strncpy(string, buf, buflen);
        free(buf);
    }
    iconv_close(cd);
}

/* Convert a string from OEM_CP to the current locale charset. */
inline void oem_intern(char *string)
{
    charset_to_intern(string, OEM_CP);
}

/* Convert a string from ISO_CP to the current locale charset. */
inline void iso_intern(char *string)
{
    charset_to_intern(string, ISO_CP);
}
#endif /* USE_ICONV_MAPPING */


#if defined( UNIX) && defined( __APPLE__)

/* Determine if the volume where "path" resides supports getattrlist()
 * and setattrlist(), that is, if we can do the special AppleDouble
 * file processing using setattrlist().  Otherwise, we should pretend
 * that "-J" is in effect, to bypass the special AppleDouble processing,
 * and leave the separate file elements separate.
 *
 * Return value   Meaning
 *           -1   Error.  See errno.
 *            0   Volume does not support getattrlist() and setattrlist().
 *            1   Volume does support getattrlist() and setattrlist().
 */
int vol_attr_ok( const char *path)
{

    int sts;
    struct statfs statfs_buf;
    struct attrlist attr_list_volattr;
    struct attr_bufr_volattr {
        unsigned int  ret_length;
        vol_capabilities_attr_t  vol_caps;
    } attr_bufr_volattr;

    /* Get file system info (in particular, the mounted volume name) for
     * the specified path.
     */
    sts = statfs( path, &statfs_buf);

    /* If that worked, get the interesting volume capability attributes. */
    if (sts == 0)
    {
        /* Clear attribute list structure. */
        memset( &attr_list_volattr, 0, sizeof( attr_list_volattr));
        /* Set attribute list bits for volume capabilities. */
        attr_list_volattr.bitmapcount = ATTR_BIT_MAP_COUNT;
        attr_list_volattr.volattr = ATTR_VOL_INFO| ATTR_VOL_CAPABILITIES;

        sts = getattrlist( statfs_buf.f_mntonname,  /* Path. */
                           &attr_list_volattr,      /* Attrib list. */
                           &attr_bufr_volattr,      /* Dest buffer. */
                           sizeof( attr_bufr_volattr),  /* Dest buffer size. */
                           0);

        if (sts == 0)
        {
            /* Set a valid return value. */
            sts = ((attr_bufr_volattr.vol_caps.capabilities[
             VOL_CAPABILITIES_INTERFACES]&
             VOL_CAP_INT_ATTRLIST) != 0);
        }
    }
    return sts;
}
#endif /* defined( UNIX) && defined( __APPLE__) */


/* 2006-03-23 SMS.
 * Emergency replacement for strerror().  (Useful on SunOS 4.*.)
 * Enable by specifying "LOCAL_UNZIP=-DNEED_STRERROR=1" on the "make"
 * command line.
 */
