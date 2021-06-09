/*
  Copyright (c) 1990-1999 Info-ZIP.  All rights reserved.

  See the accompanying file LICENSE, version 1999-Oct-05 or later
  (the contents of which are also included in zip.h) for terms of use.
  If, for some reason, both of these files are missing, the Info-ZIP license
  also may be found at:  ftp://ftp.cdrom.com/pub/infozip/license.html
*/
#pragma library
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sc.h>
#include <peek.h>
#include <lub.h>
#include <fdb.h>
#include <fsa.h>
#include "theos/stat.h"

/* replacement for standard library functions stat and fstat */

int _stat_(struct stat* st, struct fdb* fdb);
int _dstat_(struct stat* st);

#define peekucb() peeknuc()

/* map THEOS protection code to Unix modes */

unsigned short _tm2um_(char protect)
{
    unsigned short umask = 0;

    if (!(protect & _FDB_READ_PROTECT))
        umask = S_IRUSR|S_IRGRP;

    if (!(protect & _FDB_WRITE_PROTECT))
        umask |= S_IWUSR|S_IWGRP;

    if (!(protect & _FDB_EXECUTE_PROTECT))
        umask |= S_IXUSR|S_IXGRP;

    if (!(protect & _FDB_ERASE_PROTECT))
        umask |= S_IEUSR|S_IEGRP;

    if (!(protect & _FDB_SHARED_READ_PROTECT)) {
        if (_osmajor > 3)
            umask |= S_IROTH|S_IXOTH;
        else
            umask |= S_IROTH;
    }

    if (!(protect & _FDB_SHARED_WRITE_PROTECT))
        umask |= S_IWOTH;

    if (!(protect & _FDB_MODIFIED)) {
        if (_osmajor > 3)
            umask |= S_IMODF;
        else
            umask |= S_IXOTH;
    }

    if (protect & _FDB_NOT_HIDDEN)
        umask |= S_INHID;

    return umask;
}

/* map Unix modes to THEOS protections */

char _um2tm_(unsigned short mask)
{
    char protect = 0;

    if (!(mask & (S_IRUSR|S_IRGRP)))
        protect |= _FDB_READ_PROTECT;

    if (!(mask & (S_IWUSR|S_IWGRP)))
        protect |= _FDB_WRITE_PROTECT;

    if (!(mask & (S_IXUSR|S_IXGRP)))
        protect |= _FDB_EXECUTE_PROTECT;

    if (!(mask & (S_IEUSR|S_IEGRP)))
        protect |= _FDB_ERASE_PROTECT;

    if (_osmajor < 4) {
        if (!(mask & S_IROTH))
            protect |= _FDB_SHARED_READ_PROTECT;
    } else {
        if (!(mask & (S_IROTH|S_IXOTH)))
            protect |= _FDB_SHARED_READ_PROTECT;
    }

    if (!(mask & S_IWOTH))
        protect |= _FDB_SHARED_WRITE_PROTECT;

    if (mask & S_IMODF && _osmajor > 3)
        protect |= _FDB_MODIFIED;

    if (mask & S_INHID && _osmajor > 3)
        protect |= _FDB_NOT_HIDDEN;

    return protect;
}

/* root directory stat */

static int rdirstat(const char* fn, struct stat *st)
{
    register char* p = strchr(fn, ':');
    char drive;

    drive = p ? p[1] : 'S';

    if (drive >= 'a' && drive <= 'Z')
        drive -= 0x40;

    memset(st, 0, sizeof(struct stat));

    if (getlub(drive - 'A') != 255) {
        st->st_org = _FDB_STAT_DIRECTORY;
        st->st_mode = S_IFDIR|S_IRUSR|S_IWUSR|S_IROTH|S_IWOTH;
        st->st_nlink = 1;
        st->st_dev = st->st_rdev = drive - 'A';
        st->st_uid = st->st_gid = getuid();
        st->st_protect = _FDB_ERASE_PROTECT;
        return 0;
    }
    errno = _errnum = ENOENT;
    _errarg = fn;
    return -1;
}

#ifdef LOCATE_BUG

/* locate fails when stating a file in root dir from a directory with a
 * relative path. Workaround by setting directory to root dir
 * getting the file directory block, then restoring the current directory.
 */

struct fdb* __locate(const char* fn, char* buf, short* drv)
{
    struct fdb* fdb;
    char buf2[FILENAME_MAX];
    char cwd[FILENAME_MAX];
    char drive[3];
    char* p;
    char* q;

    /* return if file found */
    if (fdb = _locate(fn, buf, drv))
        return fdb;

    /* if file name does not contain a path delimiter it really does not exist.
     */
    strcpy(buf2, fn);

    if ((p = strrchr(buf2, '/')) == NULL)
        return NULL;

    /* get drive name from file path */
    q = strrchr(buf2, ':');

    /* cat drive name if any to directory path */
    if (q) {
        strncpy(drive, q, 2);
        drive[2] = '\0';
        strcpy(p, q);
    } else
        *p = '\0';
    /* save current directory */
    getcwd(cwd, FILENAME_MAX);
    /* chdir to directory path */
    chdir(buf2);
    /* get File Directory Block */
    p = strrchr(fn, '/');
    fdb = _locate(p + 1, buf, drv);
    /* restore current directory */
    chdir(cwd);
    return fdb;
}

#undef _locate
#define _locate() __locate()

/* same cause, same consequence for fopen and open.
*/

FILE* _fopen(const char* fn, const char* mode)
{
    FILE* fp;
    char buf[FILENAME_MAX];
    short drv;

    /* prepend a path to current dir to avoid use of default library */
    if (*fn != '.' && *fn != '/') {
        strcpy(buf, "./");
        strcat(buf, fn);
        return fopen(buf, mode);
    }

    if (fp = fopen(fn, mode))
        return fp;

    /* see comment for _locate */
    if (_locate(fn, buf, &drv)) {
        fn = strrchr(fn, '/');
        return fopen(fn, mode);
    }
    return NULL;
}

#undef open
int open(const char*, int, ...);

int __open(const char* fn, int mode)
{
    int fd;
    char buf[FILENAME_MAX];
    short drv;

    /* prepend a path to current dir to avoid use of default library */
    if (*fn != '.' && *fn != '/') {
        strcpy(buf, "./");
        strcat(buf, fn);
        return open(buf, mode);
    }

    if ((fd = open(fn, mode)) != EOF)
        return fd;

    /* see comment for _locate */
    if (_locate(fn, buf, &drv)) {
        fn = strrchr(fn, '/');
        if (fn)
            return open(fn, mode);
    }
    return EOF;
}
#endif

/* replacement for standard file stat */

int _stat(const char *_fn, struct stat *st)
{
    char buf[FILENAME_MAX], buf2[FILENAME_MAX], buf3[FILENAME_MAX];
    register struct fdb* fdb;
    register char* p;
    register char* fn;

    fn = strcpy(buf3, _fn);

    if (p = strrchr(fn, ':'))
        *p = 0;

    /* on current drive ./:d and .:m point to current dir
     * on another drive to root directory, workaround to avoid it */

    if (! strcmp(fn, "/") || ! strcmp(fn, ".") || ! strcmp(fn, "./")) {
        if (p == NULL) {
            /* current dir on current drive */
            fn = getcwd(buf2, FILENAME_MAX);
            /* getcwd returns NULL on root dir on drive S */
            if (fn == NULL)
                fn = strcpy(buf2, "/:S");
            /* getcwd returns /:d on root dir on any other drive */
            if (fn[1] == ':')
                return rdirstat(fn, st);
        } else {
            *p = ':';
            return rdirstat(fn, st);
        }
        if (p)
            *p = ':';
    } else {
        if (p)
            *p = ':';
        if (*fn != '.' && *fn != '/') {
            strcpy(buf2, "./");
            fn = strcat(buf2, fn);
        }
    }

    if (buf2 != fn)
        strcpy(buf2, fn);
    /* remove trailing slash before optional disk name */
    if (p = strrchr(buf2, '/')) {
        if (p[1] == ':') {
            *p = p[1];
            p[1] = p[2];
            p[2] = p[3];
        } else if (p[1] == '\0')
            *p = '\0';
    }
    /* if fn is a file get file directory block structure and device */
    if (fdb = _locate(buf2, buf, &st->st_dev)) {
        /* is it a file from another user... */
        if (strchr(buf2, '\\')
        /* a public system file... */
         || fdb->fileowner == 0
        /* or a file from the current user account ? */
         || fdb->fileowner == getuid())
        /* yes, return stat */
            return _stat_(st, fdb);
        else {
            /* no, say file doesn't exist */
            errno = _errnum = ENOENT;
            _errarg = fn;
            return -1;
        }
    }
    /* else should be a device, get device number from device name */
    st->st_rdev = st->st_dev = _lub_name(*fn == ':' ? fn+1 : fn);
    /* if it is really a device return device status */
    if (st->st_dev != -1 && getlub(st->st_dev) != 255)
        return _dstat_(st);
    /* neither an existing file or a device name, return EOF */
    st->st_rdev = st->st_dev = 0;
    errno = _errnum = ENOENT;
    _errarg = fn;
    return -1;
}

/* replacement for fstat */

int _fstat(int fd, struct stat *st)
{
    unsigned short fsanum;
    struct fsa fsa;
    register FILE *fp;
    int status;
    register int i;
    register char *p;

    if (fd < FOPEN_MAX) {
        fp = &stdin[fd];
        /* get File Save Area number */
        if (_fcntl(fp,1,0) & 0x80) {
            fsanum = (unsigned short) _fcntl(fp,83,0);
            st->st_dev = (unsigned short) _fcntl(fp,5,0);

            if (st->st_dev >= A_DISK && st->st_dev <= Z_DISK) {
                /* if opened file is a disk file */
                /* copy far fsa in protected segment to local fsa */
                for (i = 0, fsanum *= sizeof(fsa), p = (char *) &fsa;
                     i < (sizeof(fsa));
                     i++, fsanum++, p++)
                    *p = _peekfsa((char *) fsanum);
                /* build stat structure from fsa */
                status = _stat_(st, (struct fdb*) &fsa);
                /* get blocksize */
                if ((st->st_blksize = _fcntl(fp,817,0)) == 0)
                    st->st_blksize = BUFSIZ;
                return status;
            }
            /* return device status */
            return _dstat_(st);
        }
    }
    errno = _errnum = EBADF;
    return -1;
}

static int _isprt(int dev)
{
    return IS_PRT_LUB(dev);
}

/* device stat */

int _dstat_(st)
register struct stat* st;
{
    register struct ucb* ucb;

    ucb = getucb(st->st_dev);
    st->st_ino = 0;
    if (st->st_dev <= Z_DISK
     || (st->st_dev >= TAPE1 && st->st_dev <= TAPE4)) {
        st->st_mode = S_IFBLK | S_IWUSR | S_IRUSR;
        if (peekucb(&ucb->devowner) == 255)
            st->st_mode |= S_IWGRP | S_IWOTH | S_IRGRP | S_IROTH;
    } else {
        st->st_mode = S_IFCHR | S_IWUSR;
        if (_isprt(st->st_dev))
            st->st_mode |= S_IRUSR;
        if (peekucb(&ucb->devowner) == 255) {
            st->st_mode |= S_IWGRP | S_IWOTH;
            if (_isprt(st->st_dev))
                st->st_mode |= S_IRGRP | S_IROTH;
        }
    }
    st->st_nlink = 1;
    st->st_uid = st->st_gid = getuid();
    st->st_size = 0;
    st->st_atime = st->st_mtime = st->st_ctime = 0;
    st->st_rlen = 0;
    st->st_klen = 0;
    st->st_grow = 0;
    st->st_blksize = 0;
    return 0;
}

/* regular file stat */

int _stat_(st, fdb)
register struct stat* st;
register struct fdb* fdb;
{
    st->st_rdev = st->st_dev;
    st->st_ino = 0;
    st->st_org = fdb->filestat;

    /* map fdb file status to stat mode */
    switch (fdb->filestat) {
    case _FDB_STAT_LIBRARY:         st->st_mode = S_IFLIB;    break;
    case _FDB_STAT_DIRECTORY:       st->st_mode = S_IFDIR;    break;
    case _FDB_STAT_STREAM:          st->st_mode = S_IFREG;    break;
    case _FDB_STAT_RELATIVE:        st->st_mode = S_IFREL;    break;
    case _FDB_STAT_KEYED:           st->st_mode = S_IFKEY;    break;
    case _FDB_STAT_INDEXED:         st->st_mode = S_IFIND;    break;
    case _FDB_STAT_RANDOM:          st->st_mode = S_IFRND;    break;
    case _FDB_STAT_PROGRAM:         st->st_mode = S_IFR16;    break;
    case _FDB_STAT_16_BIT_PROGRAM:  st->st_mode = S_IFP16;    break;
    case _FDB_STAT_32_BIT_PROGRAM:  st->st_mode = S_IFP32;    break;
    }

    /* map theos file protection codes to stat mode */
    st->st_mode |= _tm2um_(st->st_protect = fdb->protect);
    st->st_nlink = 1;
    st->st_uid = st->st_gid = fdb->fileowner;
    st->st_size = fdb->filesize;
    st->st_atime = st->st_mtime = st->st_ctime = getfiledate(fdb);
    st->st_blksize = 0;
    /* specific theos information */
    st->st_rlen = fdb->reclen;
    st->st_klen = fdb->keylen;
    st->st_grow = fdb->filegrow;
    return 0;
}

#include <direct.h>

/* standard diropen fails on path endung with a '/', workaround */

struct dirent* _opendir(const char* dirpath)
{
    int l;
    char dirp[FILENAME_MAX];
    struct dirent* dir;

    if (dirpath && (l = strlen(dirpath))) {
        if (dirpath[l - 1] == '/') {
            strcpy(dirp, dirpath);
            dirp[l - 1] = '\0';
            return opendir(dirp);
        }
    }
    return opendir(dirpath);
}
