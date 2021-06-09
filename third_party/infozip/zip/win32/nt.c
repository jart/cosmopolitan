/*
  win32/nt.c - Zip 3

  Copyright (c) 1990-2007 Info-ZIP.  All rights reserved.

  See the accompanying file LICENSE, version 2007-Mar-4 or later
  (the contents of which are also included in zip.h) for terms of use.
  If, for some reason, all these files are missing, the Info-ZIP license
  also may be found at:  ftp://ftp.info-zip.org/pub/infozip/license.html
*/
/*++

Copyright (c) 1996  Scott Field

Module Name:

    nt.c (formerly nt_zip.c)

Abstract:

    This module implements WinNT security descriptor operations for the
    Win32 Info-ZIP project.  Operation such as querying file security,
    using/querying local and remote privileges.  The contents of this module
    are only relevant when the code is running on Windows NT, and the target
    volume supports persistent Acl storage.

    User privileges that allow accessing certain privileged aspects of the
    security descriptor (such as the Sacl) are only used if the user specified
    to do so.

    In the future, this module may be expanded to support storage of
    OS/2 EA data, Macintosh resource forks, and hard links, which are all
    supported by NTFS.

Author:

    Scott Field (sfield@microsoft.com)  27-Sep-96

--*/

#include "../zip.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#ifdef __RSXNT__
#  include "../win32/rsxntwin.h"
#endif
#include "../win32/nt.h"

#ifdef NTSD_EAS         /* This file is only needed for NTSD handling */

/* Borland C++ does not define FILE_SHARE_DELETE. Others also? */
#ifndef FILE_SHARE_DELETE
#  define FILE_SHARE_DELETE 0x00000004
#endif

/* This macro definition is missing in old versions of MS' winbase.h. */
#ifndef InterlockedExchangePointer
#  define InterlockedExchangePointer(Target, Value) \
      (PVOID)InterlockedExchange((PLONG)(Target), (LONG)(Value))
#endif

/* private prototypes */

static BOOL Initialize(VOID);
#if 0   /* currently unused */
static BOOL Shutdown(VOID);
#endif
static VOID GetRemotePrivilegesGet(CHAR *FileName, PDWORD dwRemotePrivileges);
static VOID InitLocalPrivileges(VOID);


BOOL bZipInitialized = FALSE;  /* module level stuff initialized? */
HANDLE hZipInitMutex = NULL;   /* prevent multiple initialization */

BOOL g_bBackupPrivilege = FALSE;    /* for local get file security override */
BOOL g_bZipSaclPrivilege = FALSE;      /* for local get sacl operations, only when
                                       backup privilege not present */

/* our single cached volume capabilities structure that describes the last
   volume root we encountered.  A single entry like this works well in the
   zip/unzip scenario for a number of reasons:
   1. typically one extraction path during unzip.
   2. typically process one volume at a time during zip, and then move
      on to the next.
   3. no cleanup code required and no memory leaks.
   4. simple code.

   This approach should be reworked to a linked list approach if we expect to
   be called by many threads which are processing a variety of input/output
   volumes, since lock contention and stale data may become a bottleneck. */

VOLUMECAPS g_VolumeCaps;
CRITICAL_SECTION VolumeCapsLock;


static BOOL Initialize(VOID)
{
    HANDLE hMutex;
    HANDLE hOldMutex;

    if(bZipInitialized) return TRUE;

    hMutex = CreateMutex(NULL, TRUE, NULL);
    if(hMutex == NULL) return FALSE;

    hOldMutex = (HANDLE)InterlockedExchangePointer((void *)&hZipInitMutex,
                                                   hMutex);

    if(hOldMutex != NULL) {
        /* somebody setup the mutex already */
        InterlockedExchangePointer((void *)&hZipInitMutex,
                                   hOldMutex);

        CloseHandle(hMutex); /* close new, un-needed mutex */

        /* wait for initialization to complete and return status */
        WaitForSingleObject(hOldMutex, INFINITE);
        ReleaseMutex(hOldMutex);

        return bZipInitialized;
    }

    /* initialize module level resources */

    InitializeCriticalSection( &VolumeCapsLock );
    memset(&g_VolumeCaps, 0, sizeof(VOLUMECAPS));

    InitLocalPrivileges();

    bZipInitialized = TRUE;

    ReleaseMutex(hMutex); /* release correct mutex */

    return TRUE;
}

#if 0   /* currently not used ! */
static BOOL Shutdown(VOID)
{
    /* really need to free critical sections, disable enabled privilges, etc,
       but doing so brings up possibility of race conditions if those resources
       are about to be used.  The easiest way to handle this is let these
       resources be freed when the process terminates... */

    return TRUE;
}
#endif /* never */


static VOID GetRemotePrivilegesGet(char *FileName, PDWORD dwRemotePrivileges)
{
    HANDLE hFile;

    *dwRemotePrivileges = 0;

    /* see if we have the SeBackupPrivilege */

    hFile = CreateFileA(
        FileName,
        ACCESS_SYSTEM_SECURITY | GENERIC_READ | READ_CONTROL,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
        NULL,
        OPEN_EXISTING,
        FILE_FLAG_BACKUP_SEMANTICS,
        NULL
        );

    if(hFile != INVALID_HANDLE_VALUE) {
        /* no remote way to determine SeBackupPrivilege -- just try a read
           to simulate it */
        SECURITY_INFORMATION si = DACL_SECURITY_INFORMATION | SACL_SECURITY_INFORMATION;
        PSECURITY_DESCRIPTOR sd;
        DWORD cbBuf = 0;

        GetKernelObjectSecurity(hFile, si, NULL, cbBuf, &cbBuf);

        if(ERROR_INSUFFICIENT_BUFFER == GetLastError()) {
            if((sd = HeapAlloc(GetProcessHeap(), 0, cbBuf)) != NULL) {
                if(GetKernelObjectSecurity(hFile, si, sd, cbBuf, &cbBuf)) {
                    *dwRemotePrivileges |= OVERRIDE_BACKUP;
                }
                HeapFree(GetProcessHeap(), 0, sd);
            }
        }

        CloseHandle(hFile);
    } else {

        /* see if we have the SeSecurityPrivilege */
        /* note we don't need this if we have SeBackupPrivilege */

        hFile = CreateFileA(
            FileName,
            ACCESS_SYSTEM_SECURITY,
            FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, /* maximum sharing */
            NULL,
            OPEN_EXISTING,
            0,
            NULL
            );

        if(hFile != INVALID_HANDLE_VALUE) {
            CloseHandle(hFile);
            *dwRemotePrivileges |= OVERRIDE_SACL;
        }
    }
}


BOOL ZipGetVolumeCaps(
    char *rootpath,         /* filepath, or NULL */
    char *name,             /* filename associated with rootpath */
    PVOLUMECAPS VolumeCaps  /* result structure describing capabilities */
    )
{
    char TempRootPath[MAX_PATH + 1];
    DWORD cchTempRootPath = 0;
    BOOL bSuccess = TRUE;   /* assume success until told otherwise */

    if(!bZipInitialized) if(!Initialize()) return FALSE;

    /* process the input path to produce a consistent path suitable for
       compare operations and also suitable for certain picky Win32 API
       that don't like forward slashes */

    if(rootpath != NULL && rootpath[0] != '\0') {
        DWORD i;

        cchTempRootPath = lstrlen(rootpath);
        if(cchTempRootPath > MAX_PATH) return FALSE;

        /* copy input, converting forward slashes to back slashes as we go */

        for(i = 0 ; i <= cchTempRootPath ; i++) {
            if(rootpath[i] == '/') TempRootPath[i] = '\\';
            else TempRootPath[i] = rootpath[i];
        }

        /* check for UNC and Null terminate or append trailing \ as appropriate */

        /* possible valid UNCs we are passed follow:
           \\machine\foo\bar (path is \\machine\foo\)
           \\machine\foo     (path is \\machine\foo\)
           \\machine\foo\
           \\.\c$\           (FIXFIX: Win32API doesn't like this - GetComputerName())
           LATERLATER: handling mounted DFS drives in the future will require
                       slightly different logic which isn't available today.
                       This is required because directories can point at
                       different servers which have differing capabilities.
         */

        if(TempRootPath[0] == '\\' && TempRootPath[1] == '\\') {
            DWORD slash = 0;

            for(i = 2 ; i < cchTempRootPath ; i++) {
                if(TempRootPath[i] == '\\') {
                    slash++;

                    if(slash == 2) {
                        i++;
                        TempRootPath[i] = '\0';
                        cchTempRootPath = i;
                        break;
                    }
                }
            }

            /* if there was only one slash found, just tack another onto the end */

            if(slash == 1 && TempRootPath[cchTempRootPath] != '\\') {
                TempRootPath[cchTempRootPath] = TempRootPath[0]; /* '\' */
                TempRootPath[cchTempRootPath+1] = '\0';
                cchTempRootPath++;
            }

        } else {

            if(TempRootPath[1] == ':') {

                /* drive letter specified, truncate to root */
                TempRootPath[2] = '\\';
                TempRootPath[3] = '\0';
                cchTempRootPath = 3;
            } else {

                /* must be file on current drive */
                TempRootPath[0] = '\0';
                cchTempRootPath = 0;
            }

        }

    } /* if path != NULL */

    /* grab lock protecting cached entry */
    EnterCriticalSection( &VolumeCapsLock );

    if(!g_VolumeCaps.bValid || lstrcmpi(g_VolumeCaps.RootPath, TempRootPath) != 0) {

        /* no match found, build up new entry */

        DWORD dwFileSystemFlags;
        DWORD dwRemotePrivileges = 0;
        BOOL bRemote = FALSE;

        /* release lock during expensive operations */
        LeaveCriticalSection( &VolumeCapsLock );

        bSuccess = GetVolumeInformation(
            (TempRootPath[0] == '\0') ? NULL : TempRootPath,
            NULL, 0,
            NULL, NULL,
            &dwFileSystemFlags,
            NULL, 0);

        /* only if target volume supports Acls, and we were told to use
           privileges do we need to go out and test for the remote case */

        if(bSuccess && (dwFileSystemFlags & FS_PERSISTENT_ACLS) && VolumeCaps->bUsePrivileges) {
            if(GetDriveType( (TempRootPath[0] == '\0') ? NULL : TempRootPath ) == DRIVE_REMOTE) {
                bRemote = TRUE;

                /* make a determination about our remote capabilities */

                GetRemotePrivilegesGet(name, &dwRemotePrivileges);
            }
        }

        /* always take the lock again, since we release it below */
        EnterCriticalSection( &VolumeCapsLock );

        /* replace the existing data if successful */
        if(bSuccess) {

            lstrcpynA(g_VolumeCaps.RootPath, TempRootPath, cchTempRootPath+1);
            g_VolumeCaps.bProcessDefer = FALSE;
            g_VolumeCaps.dwFileSystemFlags = dwFileSystemFlags;
            g_VolumeCaps.bRemote = bRemote;
            g_VolumeCaps.dwRemotePrivileges = dwRemotePrivileges;
            g_VolumeCaps.bValid = TRUE;
        }
    }

    if(bSuccess) {
        /* copy input elements */
        g_VolumeCaps.bUsePrivileges = VolumeCaps->bUsePrivileges;
        g_VolumeCaps.dwFileAttributes = VolumeCaps->dwFileAttributes;

        /* give caller results */
        memcpy(VolumeCaps, &g_VolumeCaps, sizeof(VOLUMECAPS));
    } else {
        g_VolumeCaps.bValid = FALSE;
    }

    LeaveCriticalSection( &VolumeCapsLock ); /* release lock */

    return bSuccess;
}

BOOL SecurityGet(
    char *resource,
    PVOLUMECAPS VolumeCaps,
    unsigned char *buffer,
    DWORD *cbBuffer
    )
{
    HANDLE hFile;
    DWORD dwDesiredAccess;
    DWORD dwFlags;
    PSECURITY_DESCRIPTOR sd = (PSECURITY_DESCRIPTOR)buffer;
    SECURITY_INFORMATION RequestedInfo;
    BOOL bBackupPrivilege = FALSE;
    BOOL bSaclPrivilege = FALSE;
    BOOL bSuccess = FALSE;

    DWORD cchResourceLen;

    if(!bZipInitialized) if(!Initialize()) return FALSE;

    /* see if we are dealing with a directory */
    /* rely on the fact resource has a trailing [back]slash, rather
       than calling expensive GetFileAttributes() */

    cchResourceLen = lstrlenA(resource);

    if(resource[cchResourceLen-1] == '/' || resource[cchResourceLen-1] == '\\')
        VolumeCaps->dwFileAttributes |= FILE_ATTRIBUTE_DIRECTORY;

    /* setup privilege usage based on if told we can use privileges, and if so,
       what privileges we have */

    if(VolumeCaps->bUsePrivileges) {
        if(VolumeCaps->bRemote) {
            /* use remotely determined privileges */
            if(VolumeCaps->dwRemotePrivileges & OVERRIDE_BACKUP)
                bBackupPrivilege = TRUE;

            if(VolumeCaps->dwRemotePrivileges & OVERRIDE_SACL)
                bSaclPrivilege = TRUE;
        } else {
            /* use local privileges */
            bBackupPrivilege = g_bBackupPrivilege;
            bSaclPrivilege = g_bZipSaclPrivilege;
        }
    }

    /* always try to read the basic security information:  Dacl, Owner, Group */

    dwDesiredAccess = READ_CONTROL;

    RequestedInfo = OWNER_SECURITY_INFORMATION |
                    GROUP_SECURITY_INFORMATION |
                    DACL_SECURITY_INFORMATION;

    /* if we have the SeBackupPrivilege or SeSystemSecurityPrivilege, read
       the Sacl, too */

    if(bBackupPrivilege || bSaclPrivilege) {
        dwDesiredAccess |= ACCESS_SYSTEM_SECURITY;
        RequestedInfo |= SACL_SECURITY_INFORMATION;
    }

    dwFlags = 0;

    /* if we have the backup privilege, specify that */
    /* opening a directory requires FILE_FLAG_BACKUP_SEMANTICS */

    if(bBackupPrivilege || (VolumeCaps->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
        dwFlags |= FILE_FLAG_BACKUP_SEMANTICS;

    hFile = CreateFileA(
        resource,
        dwDesiredAccess,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, /* maximum sharing */
        NULL,
        OPEN_EXISTING,
        dwFlags,
        NULL
        );

    if(hFile == INVALID_HANDLE_VALUE) return FALSE;

    if(GetKernelObjectSecurity(hFile, RequestedInfo, sd, *cbBuffer, cbBuffer)) {
        *cbBuffer = GetSecurityDescriptorLength( sd );
        bSuccess = TRUE;
    }

    CloseHandle(hFile);

    return bSuccess;
}

static VOID InitLocalPrivileges(VOID)
{
    HANDLE hToken;
    TOKEN_PRIVILEGES tp;

    /* try to enable some interesting privileges that give us the ability
       to get some security information that we normally cannot.

       note that enabling privileges is only relevant on the local machine;
       when accessing files that are on a remote machine, any privileges
       that are present on the remote machine get enabled by default. */

    if(!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken))
        return;

    tp.PrivilegeCount = 1;
    tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

    /* try to enable SeBackupPrivilege.
       if this succeeds, we can read all aspects of the security descriptor */

    if(LookupPrivilegeValue(NULL, SE_BACKUP_NAME, &tp.Privileges[0].Luid)) {
        if(AdjustTokenPrivileges(hToken, FALSE, &tp, 0, NULL, NULL) &&
           GetLastError() == ERROR_SUCCESS) g_bBackupPrivilege = TRUE;
    }

    /* try to enable SeSystemSecurityPrivilege if SeBackupPrivilege not present.
       if this succeeds, we can read the Sacl */

    if(!g_bBackupPrivilege &&
        LookupPrivilegeValue(NULL, SE_SECURITY_NAME, &tp.Privileges[0].Luid)) {

        if(AdjustTokenPrivileges(hToken, FALSE, &tp, 0, NULL, NULL) &&
           GetLastError() == ERROR_SUCCESS) g_bZipSaclPrivilege = TRUE;
    }

    CloseHandle(hToken);
}
#endif /* NTSD_EAS */
