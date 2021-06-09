/*
  win32/nt.h - Zip 3

  Copyright (c) 1990-2003 Info-ZIP.  All rights reserved.

  See the accompanying file LICENSE, version 2003-May-08 or later
  (the contents of which are also included in zip.h) for terms of use.
  If, for some reason, both of these files are missing, the Info-ZIP license
  also may be found at:  ftp://ftp.info-zip.org/pub/infozip/license.html
*/
#ifndef _NT_ZIP_H
#define _NT_ZIP_H

/* central header for EF_NTSD "SD" extra field */

#define EF_NTSD_MAX_VER_SUPPORT (0)
                        /* describes maximum ver# we know how to handle */

typedef struct
{
  USHORT nID;
  USHORT nSize;
  ULONG lSize;
}
EF_NTSD_C_HEADER, *PEF_NTSD_C_HEADER;

#define EF_NTSD_C_LEN (sizeof(EF_NTSD_C_HEADER))

/* local header for EF_NTSD "SD" extra field */

#pragma pack(1) /* bytes following structure immediately follow BYTE Version */

typedef struct
{
  USHORT nID;   /* tag for this extra block type */
  USHORT nSize; /* total data size for this block */
  ULONG lSize;  /* uncompressed security descriptor data size */
  BYTE Version; /* Version of uncompressed security descriptor data format */
}
IZ_PACKED EF_NTSD_L_HEADER, *PEF_NTSD_L_HEADER;

#pragma pack()

/*  ...followed by... */
/*  SHORT CType;  compression type */
/*  ULONG EACRC;  CRC value for uncompressed security descriptor data */
/*  <var.> Variable length data */


#define EF_NTSD_L_LEN (EF_NTSD_C_LEN + sizeof(BYTE))
                                /* avoid alignment size computation */

#define NTSD_BUFFERSIZE (1024)  /* threshold to cause malloc() */

#define OVERRIDE_BACKUP     1   /* we have SeBackupPrivilege on remote */
#define OVERRIDE_RESTORE    2   /* we have SeRestorePrivilege on remote */
#define OVERRIDE_SACL       4   /* we have SeSystemSecurityPrivilege on remote */

typedef struct {
    BOOL bValid;                /* are our contents valid? */
    BOOL bProcessDefer;         /* process deferred entry yet? */
    BOOL bUsePrivileges;        /* use privilege overrides? */
    DWORD dwFileSystemFlags;    /* describes target file system */
    BOOL bRemote;               /* is volume remote? */
    DWORD dwRemotePrivileges;   /* relevant only on remote volumes */
    DWORD dwFileAttributes;
    char RootPath[MAX_PATH+1];  /* path to network / filesystem */
} VOLUMECAPS, *PVOLUMECAPS, *LPVOLUMECAPS;

BOOL SecurityGet(char *resource, PVOLUMECAPS VolumeCaps, unsigned char *buffer,
                 DWORD *cbBuffer);
BOOL ZipGetVolumeCaps(char *rootpath, char *name, PVOLUMECAPS VolumeCaps);

#endif /* _NT_ZIP_H */

