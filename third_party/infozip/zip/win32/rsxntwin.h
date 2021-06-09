/*
  win32/rsxntwin.h - Zip 3

  Copyright (c) 1990-2007 Info-ZIP.  All rights reserved.

  See the accompanying file LICENSE, version 2007-Mar-4 or later
  (the contents of which are also included in zip.h) for terms of use.
  If, for some reason, all these files are missing, the Info-ZIP license
  also may be found at:  ftp://ftp.info-zip.org/pub/infozip/license.html
*/
/* rsxntwin.h
 *
 * fills some gaps in the rsxnt 1.3 win32 header files (<windows.h>) that are
 * required for compiling Info-ZIP sources for Win NT / Win 95
 */

#ifdef __RSXNT__
#if !defined (_RSXNTWIN_H)
#define _RSXNTWIN_H

#ifdef TFUNCT   /* TFUNCT is undefined when MSSDK headers are used */

#ifdef __cplusplus
extern "C" {
#endif

#define PASCAL __stdcall

#define ANYSIZE_ARRAY 1

#ifndef TIME_ZONE_ID_UNKNOWN
#  define TIME_ZONE_ID_UNKNOWN  0
#endif
#ifndef TIME_ZONE_ID_INVALID
#  define TIME_ZONE_ID_INVALID  (DWORD)0xFFFFFFFFL
#endif

#define FILE_ATTRIBUTE_HIDDEN   0x00000002
#define FILE_ATTRIBUTE_SYSTEM   0x00000004

#define FILE_SHARE_DELETE       0x00000004

#define FILE_PERSISTENT_ACLS    0x00000008

#define HFILE_ERROR        ((HFILE)-1)

#define FS_PERSISTENT_ACLS      FILE_PERSISTENT_ACLS


BOOL WINAPI DosDateTimeToFileTime(WORD, WORD, LPFILETIME);


#ifndef SetVolumeLabel
#define SetVolumeLabel TFUNCT(SetVolumeLabel)
#endif
BOOL WINAPI SetVolumeLabel(LPCTSTR, LPCTSTR);


#ifndef GetDriveType
#define GetDriveType TFUNCT(GetDriveType)
#endif
DWORD GetDriveType(LPCTSTR);

#define DRIVE_UNKNOWN     0
#define DRIVE_REMOVABLE   2
#define DRIVE_FIXED       3
#define DRIVE_REMOTE      4
#define DRIVE_CDROM       5
#define DRIVE_RAMDISK     6

#ifndef SearchPath
#define SearchPath TFUNCT(SearchPath)
#endif
BOOL WINAPI SearchPath(LPCTSTR, LPCTSTR, LPCTSTR, UINT, LPTSTR, LPTSTR *);

#define ERROR_SUCCESS                   0
#define ERROR_INSUFFICIENT_BUFFER       122

LONG WINAPI InterlockedExchange(LPLONG, LONG);

#define ACCESS_SYSTEM_SECURITY          0x01000000L

typedef PVOID PSECURITY_DESCRIPTOR;
typedef PVOID PSID;
typedef struct _ACL {
    BYTE  AclRevision;
    BYTE  Sbz1;
    WORD   AclSize;
    WORD   AceCount;
    WORD   Sbz2;
} ACL;
typedef ACL *PACL;

typedef struct _LUID {
    DWORD LowPart;
    LONG HighPart;
} LUID, *PLUID;

typedef struct _LUID_AND_ATTRIBUTES {
    LUID Luid;
    DWORD Attributes;
    } LUID_AND_ATTRIBUTES, * PLUID_AND_ATTRIBUTES;

typedef struct _TOKEN_PRIVILEGES {
    DWORD PrivilegeCount;
    LUID_AND_ATTRIBUTES Privileges[ANYSIZE_ARRAY];
} TOKEN_PRIVILEGES, *PTOKEN_PRIVILEGES;

#define TOKEN_QUERY             0x0008
#define TOKEN_ADJUST_PRIVILEGES 0x0020

BOOL WINAPI OpenProcessToken(HANDLE, DWORD, PHANDLE);
BOOL WINAPI AdjustTokenPrivileges(HANDLE, BOOL, PTOKEN_PRIVILEGES, DWORD,
                PTOKEN_PRIVILEGES, PDWORD);

#ifndef LookupPrivilegeValue
#define LookupPrivilegeValue TFUNCT(LookupPrivilegeValue)
#endif
BOOL WINAPI LookupPrivilegeValue(LPCTSTR, LPCTSTR, PLUID);

typedef DWORD SECURITY_INFORMATION, *PSECURITY_INFORMATION;
#define OWNER_SECURITY_INFORMATION      0x00000001L
#define GROUP_SECURITY_INFORMATION      0x00000002L
#define DACL_SECURITY_INFORMATION       0x00000004L
#define SACL_SECURITY_INFORMATION       0x00000008L

typedef WORD SECURITY_DESCRIPTOR_CONTROL, *PSECURITY_DESCRIPTOR_CONTROL;
#define SE_DACL_PRESENT         0x0004
#define SE_SACL_PRESENT         0x0010

#define SE_PRIVILEGE_ENABLED    0x00000002L

#define SE_SECURITY_NAME                  TEXT("SeSecurityPrivilege")
#define SE_BACKUP_NAME                    TEXT("SeBackupPrivilege")
#define SE_RESTORE_NAME                   TEXT("SeRestorePrivilege")

BOOL WINAPI GetKernelObjectSecurity(HANDLE, SECURITY_INFORMATION,
                PSECURITY_DESCRIPTOR, DWORD, LPDWORD);
BOOL WINAPI SetKernelObjectSecurity(HANDLE, SECURITY_INFORMATION,
                PSECURITY_DESCRIPTOR);
BOOL WINAPI IsValidSid(PSID);
BOOL WINAPI IsValidAcl(PACL);
BOOL WINAPI InitializeSecurityDescriptor(PSECURITY_DESCRIPTOR);
BOOL WINAPI IsValidSecurityDescriptor(PSECURITY_DESCRIPTOR);
DWORD WINAPI GetSecurityDescriptorLength(PSECURITY_DESCRIPTOR);
BOOL WINAPI GetSecurityDescriptorControl(PSECURITY_DESCRIPTOR,
                PSECURITY_DESCRIPTOR_CONTROL, LPDWORD);
BOOL WINAPI SetSecurityDescriptorControl(PSECURITY_DESCRIPTOR,
                SECURITY_DESCRIPTOR_CONTROL, SECURITY_DESCRIPTOR_CONTROL);
BOOL WINAPI GetSecurityDescriptorDacl(PSECURITY_DESCRIPTOR,
                                      LPBOOL, PACL *, LPBOOL);
BOOL WINAPI SetSecurityDescriptorDacl(PSECURITY_DESCRIPTOR, BOOL, PACL, BOOL);
BOOL WINAPI GetSecurityDescriptorSacl(PSECURITY_DESCRIPTOR,
                                      LPBOOL, PACL *, LPBOOL);
BOOL WINAPI SetSecurityDescriptorSacl(PSECURITY_DESCRIPTOR, BOOL, PACL, BOOL);
BOOL WINAPI GetSecurityDescriptorOwner(PSECURITY_DESCRIPTOR, PSID *, LPBOOL);
BOOL WINAPI SetSecurityDescriptorOwner(PSECURITY_DESCRIPTOR, PSID, BOOL);
BOOL WINAPI GetSecurityDescriptorGroup(PSECURITY_DESCRIPTOR, PSID *, LPBOOL);
BOOL WINAPI SetSecurityDescriptorGroup(PSECURITY_DESCRIPTOR, PSID, BOOL);
VOID WINAPI InitializeCriticalSection();

#ifdef __cplusplus
}
#endif

#endif /* TFUNCT */

#ifndef CP_UTF8
#  define CP_UTF8               65001           /* UTF-8 translation */
#endif

#endif /* !defined (_RSXNTWIN_H) */
#endif /* __RSXNT__ */
