#ifndef COSMOPOLITAN_LIBC_COMPAT_INCLUDE_WINDOWS_H_
#define COSMOPOLITAN_LIBC_COMPAT_INCLUDE_WINDOWS_H_
#include "libc/nt/accounting.h"
#include "libc/nt/automation.h"
#include "libc/nt/console.h"
#include "libc/nt/debug.h"
#include "libc/nt/dll.h"
#include "libc/nt/errors.h"
#include "libc/nt/events.h"
#include "libc/nt/files.h"
#include "libc/nt/ipc.h"
#include "libc/nt/memory.h"
#include "libc/nt/paint.h"
#include "libc/nt/process.h"
#include "libc/nt/registry.h"
#include "libc/nt/synchronization.h"
#include "libc/nt/thread.h"
#include "libc/nt/windows.h"
#include "libc/nt/winsock.h"

/* #if defined(__GNUC__) */
/* #pragma GCC diagnostic ignored "-Wint-conversion" */
/* #endif */
#undef NULL
#define NULL 0

#define FARPROC  wambda
#define NEARPROC wambda
#define PROC     wambda

#define LONG  int32_t  /* [sic] */
#define WCHAR char16_t /* [sic] */
#define BOOL  bool32   /* [sic] */

#define TRUE  true
#define FALSE false

#define PVOID    void*
#define PVOID64  void*
#define LPCVOID  const void*
#define CHAR     char
#define SHORT    short
#define CONST    const
#define VOID     void
#define INT8     signed char
#define PINT8    signed char*
#define INT16    int16_t
#define PINT16   int16_t*
#define INT32    int32_t
#define PINT32   int32_t*
#define INT64    int64_t
#define PINT64   int64_t*
#define UINT8    unsigned char
#define PUINT8   unsigned char*
#define UINT16   uint16_t
#define PUINT16  uint16_t*
#define UINT32   uint32_t
#define PUINT32  uint32_t*
#define UINT64   uint64_t
#define PUINT64  uint64_t*
#define LONG32   int32_t
#define PLONG32  int32_t*
#define ULONG32  uint32_t
#define PULONG32 uint32_t*
#define DWORD32  uint32_t
#define PDWORD32 uint32_t*

#define INT_PTR        intptr_t
#define PINT_PTR       intptr_t*
#define UINT_PTR       uintptr_t
#define PUINT_PTR      uintptr_t*
#define LONG_PTR       intptr_t
#define PLONG_PTR      int32_t**
#define ULONG_PTR      uintptr_t
#define PULONG_PTR     uint32_t**
#define POINTER_64_INT int64_t*
#define __int3264      int64_t

#define SHANDLE_PTR int64_t
#define HANDLE_PTR  uint64_t

#define UHALF_PTR  uint32_t
#define PUHALF_PTR uint32_t*
#define HALF_PTR   int32_t
#define PHALF_PTR  int32_t*

#define SIZE_T     size_t
#define PSIZE_T    size_t*
#define SSIZE_T    ssize_t
#define PSSIZE_T   ssize_t*
#define DWORD_PTR  ULONG_PTR
#define PDWORD_PTR ULONG_PTR*
#define LONG64     int64_t
#define PLONG64    int64_t*
#define ULONG64    uint64_t
#define PULONG64   uint64_t*
#define DWORD64    uint64_t
#define PDWORD64   uint64_t*
#define KAFFINITY  ULONG_PTR
#define PKAFFINITY KAFFINITY*
#define KPRIORITY  LONG

#define PWCHAR         WCHAR*
#define LPWCH          WCHAR*
#define PWCH           WCHAR*
#define LPCWCH         CONST WCHAR*
#define PCWCH          CONST WCHAR*
#define NWPSTR         WCHAR*
#define LPWSTR         WCHAR*
#define PWSTR          WCHAR*
#define PZPWSTR        PWSTR*
#define PCZPWSTR       CONST PWSTR*
#define LPUWSTR        WCHAR forcealign(1)*
#define PUWSTR         WCHAR forcealign(1)*
#define LPCWSTR        CONST WCHAR*
#define PCWSTR         CONST WCHAR*
#define PZPCWSTR       PCWSTR*
#define LPCUWSTR       CONST WCHAR forcealign(1)*
#define PCUWSTR        CONST WCHAR forcealign(1)*
#define PCHAR          CHAR*
#define LPCH           CHAR*
#define PCH            CHAR*
#define LPCCH          CONST CHAR*
#define PCCH           CONST CHAR*
#define NPSTR          CHAR*
#define LPSTR          CHAR*
#define PSTR           CHAR*
#define PZPSTR         PSTR*
#define PCZPSTR        CONST PSTR*
#define LPCSTR         CONST CHAR*
#define PCSTR          CONST CHAR*
#define PZPCSTR        PCSTR*
#define TCHAR          WCHAR
#define PTCHAR         WCHAR*
#define TBYTE          WCHAR
#define PTBYTE         WCHAR*
#define LPTCH          LPWSTR
#define PTCH           LPWSTR
#define PTSTR          LPWSTR
#define LPTSTR         LPWSTR
#define PCTSTR         LPCWSTR
#define LPCTSTR        LPCWSTR
#define PUTSTR         LPUWSTR
#define LPUTSTR        LPUWSTR
#define PCUTSTR        LPCUWSTR
#define LPCUTSTR       LPCUWSTR
#define LP             LPWSTR
#define PSHORT         int16_t*
#define PLONG          int32_t*
#define HANDLE         int64_t
#define PHANDLE        HANDLE*
#define FCHAR          BYTE
#define FSHORT         WORD
#define FLONG          DWORD
#define HRESULT        LONG
#define CCHAR          char
#define LCID           DWORD
#define PLCID          PDWORD
#define LANGID         WORD
#define LONGLONG       int64_t
#define ULONGLONG      uint64_t
#define USN            LONGLONG
#define PLONGLONG      LONGLONG*
#define PULONGLONG     ULONGLONG*
#define DWORDLONG      ULONGLONG
#define PDWORDLONG     DWORDLONG*
#define LARGE_INTEGER  int64_t
#define PLARGE_INTEGER int64_t*

#define ULONG        uint32_t
#define PULONG       ULONG*
#define USHORT       unsigned short
#define PUSHORT      USHORT*
#define UCHAR        unsigned char
#define PUCHAR       UCHAR*
#define PSZ          char*
#define DWORD        uint32_t
#define WINBOOL      BOOL
#define BOOLEAN      BOOL
#define BYTE         unsigned char
#define WORD         unsigned short
#define FLOAT        float
#define PFLOAT       FLOAT*
#define PBOOL        WINBOOL*
#define PBOOLEAN     WINBOOL*
#define LPBOOL       WINBOOL*
#define PBYTE        BYTE*
#define LPBYTE       BYTE*
#define PINT         int*
#define LPINT        int*
#define PWORD        WORD*
#define LPWORD       WORD*
#define LPLONG       int32_t*
#define PDWORD       DWORD*
#define LPDWORD      DWORD*
#define LPVOID       void*
#define LPCVOID      const void*
#define INT          int
#define UINT         unsigned int
#define PUINT        unsigned int*
#define WPARAM       UINT_PTR
#define LPARAM       LONG_PTR
#define LRESULT      LONG_PTR
#define ATOM         WORD
#define SPHANDLE     HANDLE*
#define LPHANDLE     HANDLE*
#define HGLOBAL      HANDLE
#define HLOCAL       HANDLE
#define GLOBALHANDLE HANDLE
#define LOCALHANDLE  HANDLE
#define HGDIOBJ      void*
#define PHKEY        HKEY*
#define HMODULE      HINSTANCE
#define HFILE        int
#define HCURSOR      HICON
#define COLORREF     DWORD
#define LPCOLORREF   DWORD*
#define ACCESS_MASK  ULONG
#define REGSAM       ACCESS_MASK
#define HKEY         int64_t

#define NTSTATUS      LONG
#define HACCEL        int64_t
#define HBITMAP       int64_t
#define HBRUSH        int64_t
#define HCOLORSPACE   int64_t
#define HDC           int64_t
#define HGLRC         int64_t
#define HDESK         int64_t
#define HENHMETAFILE  int64_t
#define HFONT         int64_t
#define HICON         int64_t
#define HMENU         int64_t
#define HMETAFILE     int64_t
#define HINSTANCE     int64_t
#define HPALETTE      int64_t
#define HPEN          int64_t
#define HRGN          int64_t
#define HRSRC         int64_t
#define HSTR          int64_t
#define HTASK         int64_t
#define HWINSTA       int64_t
#define HKL           int64_t
#define HMONITOR      int64_t
#define HWINEVENTHOOK int64_t
#define HUMPD         int64_t
#define HWND          int64_t

#define _GENERIC_MAPPING     NtGenericMapping
#define GENERIC_MAPPING      struct NtGenericMapping
#define PGENERIC_MAPPING     struct NtGenericMapping*
#define _UNICODE_STRING      NtUnicodeString
#define UNICODE_STRING       struct NtUnicodeString
#define PUNICODE_STRING      struct NtUnicodeString*
#define _IO_COUNTERS         NtIoCounters
#define IO_COUNTERS          struct NtIoCounters
#define PIO_COUNTERS         struct NtIoCounters*
#define _FILE_TIME           NtFileTime
#define FILE_TIME            struct NtFileTime
#define PFILE_TIME           struct NtFileTime*
#define _FILETIME            NtFileTime
#define FILETIME             struct NtFileTime
#define PFILETIME            struct NtFileTime*
#define _CLIENT_ID           NtClientId
#define CLIENT_ID            struct NtClientId
#define PCLIENT_ID           struct NtClientId*
#define _SYSTEM_THREADS      NtSystemThreads
#define SYSTEM_THREADS       struct NtSystemThreads
#define PSYSTEM_THREADS      struct NtSystemThreads*
#define _VM_COUNTERS         NtVmCounters
#define VM_COUNTERS          struct NtVmCounters
#define PVM_COUNTERS         struct NtVmCounters*
#define _SECURITY_DESCRIPTOR NtSecurityDescriptor
#define SECURITY_DESCRIPTOR  struct NtSecurityDescriptor
#define PSECURITY_DESCRIPTOR struct NtSecurityDescriptor*

#define _OBJECT_ALL_INFORMATION          NtObjectAllInformation
#define OBJECT_ALL_INFORMATION           struct NtObjectAllinformation
#define POBJECT_ALL_INFORMATION          struct NtObjectAllInformation*
#define _OBJECT_TYPE_INFORMATION         NtObjectTypeInformation
#define OBJECT_TYPE_INFORMATION          struct NtObjectTypeInformation
#define POBJECT_TYPE_INFORMATION         struct NtObjectTypeInformation*
#define _OBJECT_NAME_INFORMATION         NtObjectNameInformation
#define OBJECT_NAME_INFORMATION          struct NtObjectNameInformation
#define POBJECT_NAME_INFORMATION         struct NtObjectNameInformation*
#define _OBJECT_BASIC_INFORMATION        NtObjectBasicInformation
#define OBJECT_BASIC_INFORMATION         struct NtObjectBasicInformation
#define POBJECT_BASIC_INFORMATION        struct NtObjectBasicInformation*
#define _FILE_ACCESS_INFORMATION         NtFileAccessInformation
#define FILE_ACCESS_INFORMATION          struct NtFileAccessInformation
#define PFILE_ACCESS_INFORMATION         struct NtFileAccessInformation*
#define _FILE_ALIGNMENT_INFORMATION      NtFileAlignmentInformation
#define FILE_ALIGNMENT_INFORMATION       struct NtFileAlignmentInformation
#define PFILE_ALIGNMENT_INFORMATION      struct NtFileAlignmentInformation*
#define _FILE_ALL_INFORMATION            NtFileAllInformation
#define FILE_ALL_INFORMATION             struct NtFileAllInformation
#define PFILE_ALL_INFORMATION            struct NtFileAllInformation*
#define _FILE_ALLOCATION_INFORMATION     NtFileAllocationInformation
#define FILE_ALLOCATION_INFORMATION      struct NtFileAllocationInformation
#define PFILE_ALLOCATION_INFORMATION     struct NtFileAllocationInformation*
#define _FILE_BASIC_INFORMATION          NtFileBasicInformation
#define FILE_BASIC_INFORMATION           struct NtFileBasicInformation
#define PFILE_BASIC_INFORMATION          struct NtFileBasicInformation*
#define _FILE_BOTH_DIR_INFORMATION       NtFileBothDirectoryInformation
#define FILE_BOTH_DIR_INFORMATION        struct NtFileBothDirectoryInformation
#define PFILE_BOTH_DIR_INFORMATION       struct NtFileBothDirectoryInformation*
#define _FILE_BOTH_DIRECTORY_INFORMATION NtFileBothDirectoryInformation
#define FILE_BOTH_DIRECTORY_INFORMATION  struct NtFileBothDirectoryInformation
#define PFILE_BOTH_DIRECTORY_INFORMATION struct NtFileBothDirectoryInformation*
#define _FILE_DIRECTORY_INFORMATION      NtFileDirectoryInformation
#define FILE_DIRECTORY_INFORMATION       struct NtFileDirectoryInformation
#define PFILE_DIRECTORY_INFORMATION      struct NtFileDirectoryInformation*
#define _FILE_DISPOSITION_INFORMATION    NtFileDispositionInformation
#define FILE_DISPOSITION_INFORMATION     struct NtFileDispositionInformation
#define PFILE_DISPOSITION_INFORMATION    struct NtFileDispositionInformation*
#define _FILE_EA_INFORMATION             NtFileEaInformation
#define FILE_EA_INFORMATION              struct NtFileEaInformation
#define PFILE_EA_INFORMATION             struct NtFileEaInformation*
#define _FILE_INTERNAL_INFORMATION       NtFileInternalInformation
#define FILE_INTERNAL_INFORMATION        struct NtFileInternalInformation
#define PFILE_INTERNAL_INFORMATION       struct NtFileInternalInformation*
#define _FILE_MODE_INFORMATION           NtFileModeInformation
#define FILE_MODE_INFORMATION            struct NtFileModeInformation
#define PFILE_MODE_INFORMATION           struct NtFileModeInformation*
#define _FILE_NAME_INFORMATION           NtFileNameInformation
#define FILE_NAME_INFORMATION            struct NtFileNameInformation
#define PFILE_NAME_INFORMATION           struct NtFileNameInformation*
#define _FILE_NAMES_INFORMATION          NtFileNamesInformation
#define FILE_NAMES_INFORMATION           struct NtFileNamesInformation
#define PFILE_NAMES_INFORMATION          struct NtFileNamesInformation*
#define _FILE_POSITION_INFORMATION       NtFilePositionInformation
#define FILE_POSITION_INFORMATION        struct NtFilePositionInformation
#define PFILE_POSITION_INFORMATION       struct NtFilePositionInformation*
#define _FILE_RENAME_INFORMATION         NtFileRenameInformation
#define FILE_RENAME_INFORMATION          struct NtFileRenameInformation
#define PFILE_RENAME_INFORMATION         struct NtFileRenameInformation*
#define _FILE_STANDARD_INFORMATION       NtFileStandardInformation
#define FILE_STANDARD_INFORMATION        struct NtFileStandardInformation
#define PFILE_STANDARD_INFORMATION       struct NtFileStandardInformation*
#define _FILE_STREAM_INFORMATION         NtFileStreamInformation
#define FILE_STREAM_INFORMATION          struct NtFileStreamInformation
#define PFILE_STREAM_INFORMATION         struct NtFileStreamInformation*
#define _KERNEL_USER_TIMES               NtKernelUserTimes
#define KERNEL_USER_TIMES                struct NtKernelUserTimes
#define PKERNEL_USER_TIMES               struct NtKernelUserTimes*
#define _PROCESS_BASIC_INFORMATION       NtProcessBasicInformation
#define PROCESS_BASIC_INFORMATION        struct NtProcessBasicInformation
#define PPROCESS_BASIC_INFORMATION       struct NtProcessBasicInformation*
#define _SYSTEM_BASIC_INFORMATION        NtSystemBasicInformation
#define SYSTEM_BASIC_INFORMATION         struct NtSystemBasicInformation
#define PSYSTEM_BASIC_INFORMATION        struct NtSystemBasicInformation*
#define _SYSTEM_EXCEPTION_INFORMATION    NtSystemExceptionInformation
#define SYSTEM_EXCEPTION_INFORMATION     struct NtSystemExceptionInformation
#define PSYSTEM_EXCEPTION_INFORMATION    struct NtSystemExceptionInformation*
#define _SYSTEM_HANDLE_ENTRY             NtSystemHandleEntry
#define SYSTEM_HANDLE_ENTRY              struct NtSystemHandleEntry
#define PSYSTEM_HANDLE_ENTRY             struct NtSystemHandleEntry*
#define _SYSTEM_HANDLE_INFORMATION       NtSystemHandleInformation
#define SYSTEM_HANDLE_INFORMATION        struct NtSystemHandleInformation
#define PSYSTEM_HANDLE_INFORMATION       struct NtSystemHandleInformation*
#define _SYSTEM_INTERRUPT_INFORMATION    NtSystemInterruptInformation
#define SYSTEM_INTERRUPT_INFORMATION     struct NtSystemInterruptInformation
#define PSYSTEM_INTERRUPT_INFORMATION    struct NtSystemInterruptInformation*
#define _SYSTEM_LOOKASIDE_INFORMATION    NtSystemLookasideInformation
#define SYSTEM_LOOKASIDE_INFORMATION     struct NtSystemLookasideInformation
#define PSYSTEM_LOOKASIDE_INFORMATION    struct NtSystemLookasideInformation*
#define _SYSTEM_PERFORMANCE_INFORMATION  NtSystemPerformanceInformation
#define SYSTEM_PERFORMANCE_INFORMATION   struct NtSystemPerformanceInformation
#define PSYSTEM_PERFORMANCE_INFORMATION  struct NtSystemPerformanceInformation*
#define _SYSTEM_PROCESS_INFORMATION      NtSystemProcessInformation
#define SYSTEM_PROCESS_INFORMATION       struct NtSystemProcessInformation
#define PSYSTEM_PROCESS_INFORMATION      struct NtSystemProcessInformation*
#define _SYSTEM_PROCESSOR_INFORMATION    NtSystemProcessorInformation
#define SYSTEM_PROCESSOR_INFORMATION     struct NtSystemProcessorInformation
#define PSYSTEM_PROCESSOR_INFORMATION    struct NtSystemProcessorInformation*
#define _SYSTEM_TIMEOFDAY_INFORMATION    NtSystemTimeofdayInformation
#define SYSTEM_TIMEOFDAY_INFORMATION     struct NtSystemTimeofdayInformation
#define PSYSTEM_TIMEOFDAY_INFORMATION    struct NtSystemTimeofdayInformation*

#define _SYSTEM_REGISTRY_QUOTA_INFORMATION NtSystemRegistryQuotaInformation
#define SYSTEM_REGISTRY_QUOTA_INFORMATION \
  struct NtSystemRegistryQuotaInformation
#define PSYSTEM_REGISTRY_QUOTA_INFORMATION \
  struct NtSystemRegistryQuotaInformation*
#define _SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION \
  NtSystemProcessorPerformanceInformation
#define SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION \
  struct NtSystemProcessorPerformanceInformation
#define PSYSTEM_PROCESSOR_PERFORMANCE_INFORMATION \
  struct NtSystemProcessorPerformanceInformation*
#define _FILE_FULL_DIR_INFORMATION       NtFileFullDirectoryInformation
#define FILE_FULL_DIR_INFORMATION        struct NtFileFullDirectoryInformation
#define PFILE_FULL_DIR_INFORMATION       struct NtFileFullDirectoryInformation*
#define _FILE_FULL_DIRECTORY_INFORMATION NtFileFullDirectoryInformation
#define FILE_FULL_DIRECTORY_INFORMATION  struct NtFileFullDirectoryInformation
#define PFILE_FULL_DIRECTORY_INFORMATION struct NtFileFullDirectoryInformation*
#define _FILE_ATTRIBUTE_TAG_INFORMATION  NtFileAttributeTagInformation
#define FILE_ATTRIBUTE_TAG_INFORMATION   struct NtFileAttributeTagInformation
#define PFILE_ATTRIBUTE_TAG_INFORMATION  struct NtFileAttributeTagInformation*
#define _FILE_PIPE_LOCAL_INFORMATION     NtFilePipeLocalInformation
#define FILE_PIPE_LOCAL_INFORMATION      struct NtFilePipeLocalInformation
#define PFILE_PIPE_LOCAL_INFORMATION     struct NtFilePipeLocalInformation*
#define _FILE_NETWORK_OPEN_INFORMATION   NtFileNetworkOpenInformation
#define FILE_NETWORK_OPEN_INFORMATION    struct NtFileNetworkOpenInformation
#define PFILE_NETWORK_OPEN_INFORMATION   struct NtFileNetworkOpenInformation*
#define _FILE_MAILSLOT_QUERY_INFORMATION NtFileMailslotQueryInformation
#define FILE_MAILSLOT_QUERY_INFORMATION  struct NtFileMailslotQueryInformation
#define PFILE_MAILSLOT_QUERY_INFORMATION struct NtFileMailslotQueryInformation*
#define _FILE_MAILSLOT_SET_INFORMATION   NtFileMailslotSetInformation
#define FILE_MAILSLOT_SET_INFORMATION    struct NtFileMailslotSetInformation
#define PFILE_MAILSLOT_SET_INFORMATION   struct NtFileMailslotSetInformation*
#define _FILE_FULL_EA_INFORMATION        NtFileFullEaInformation
#define FILE_FULL_EA_INFORMATION         struct NtFileFullEaInformation
#define PFILE_FULL_EA_INFORMATION        struct NtFileFullEaInformation*

#define _LUID NtLuid
#define LUID  struct NtLuid
#define PLUID struct NtLuid*

#define _LUID_AND_ATTRIBUTES NtLuidAndAttributes
#define LUID_AND_ATTRIBUTES  struct NtLuidAndAttributes
#define PLUID_AND_ATTRIBUTES struct NtLuidAndAttributes*

#define _PRIVILEGE_SET NtPrivilegeSet
#define PRIVILEGE_SET  struct NtPrivilegeSet
#define PPRIVILEGE_SET struct NtPrivilegeSet*

#define _IMAGE_FILE_HEADER NtImageFileHeader
#define IMAGE_FILE_HEADER  struct NtImageFileHeader
#define PIMAGE_FILE_HEADER struct NtImageFileHeader*

#define _IMAGE_DOS_HEADER NtImageDosHeader
#define IMAGE_DOS_HEADER  struct NtImageDosHeader
#define PIMAGE_DOS_HEADER struct NtImageDosHeader*

#define _BY_HANDLE_FILE_INFORMATION  NtByHandleFileInformation
#define BY_HANDLE_FILE_INFORMATION   struct NtByHandleFileInformation
#define PBY_HANDLE_FILE_INFORMATION  struct NtByHandleFileInformation*
#define LPBY_HANDLE_FILE_INFORMATION struct NtByHandleFileInformation*

#define _WIN32_FILE_ATTRIBUTE_DATA  NtWin32FileAttributeData
#define WIN32_FILE_ATTRIBUTE_DATA   struct NtWin32FileAttributeData
#define LPWIN32_FILE_ATTRIBUTE_DATA struct NtWin32FileAttributeData*

#define _FILE_END_OF_FILE_INFORMATION FileEndOfFileInformation
#define FILE_END_OF_FILE_INFORMATION  struct FileEndOfFileInformation
#define PFILE_END_OF_FILE_INFORMATION struct FileEndOfFileInformation*

#define _GET_FILEEX_INFO_LEVELS  NtGetFileexInfoLevels
#define GET_FILEEX_INFO_LEVELS   int
#define LPGET_FILEEX_INFO_LEVELS int*

#define _WIN32_FIND_DATA  NtWin32FindData
#define WIN32_FIND_DATA   struct NtWin32FindData
#define LPWIN32_FIND_DATA struct NtWin32FindData*

#define _FINDEX_INFO_LEVELS            NtFindexInfoLevels
#define FINDEX_INFO_LEVELS             int
#define FindExInfoStandard             kNtFindExInfoStandard
#define FindExInfoBasic                kNtFindExInfoBasic
#define FindExInfoMaxInfoLevel         kNtFindExInfoMaxInfoLevel
#define FIND_FIRST_EX_CASE_SENSITIVE   kNtFindFirstExCaseSensitive
#define FIND_FIRST_EX_LARGE_FETCH      kNtFindFirstExLargeFetch
#define _FINDEX_SEARCH_OPS             NtFindexSearchOps
#define FINDEX_SEARCH_OPS              int
#define FindExSearchNameMatch          kNtFindExSearchNameMatch
#define FindExSearchLimitToDirectories kNtFindExSearchLimitToDirectories
#define FindExSearchLimitToDevices     kNtFindExSearchLimitToDevices
#define FindExSearchMaxSearchOp        kNtFindExSearchMaxSearchOp

#define GetFileExInfoStandard kNtGetFileExInfoStandard
#define GetFileExMaxInfoLevel kNtGetFile_MAX

#define MOVEFILE_REPLACE_EXISTING      kNtMovefileReplaceExisting
#define MOVEFILE_COPY_ALLOWED          kNtMovefileCopyAllowed
#define MOVEFILE_DELAY_UNTIL_REBOOT    kNtMovefileDelayUntilReboot
#define MOVEFILE_CREATE_HARDLINK       kNtMovefileCreateHardlink
#define MOVEFILE_FAIL_IF_NOT_TRACKABLE kNtMovefileFailIfNotTrackable
#define MOVEFILE_WRITE_THROUGH         kNtMovefileWriteThrough

#define OFFER_PRIORITY             int
#define VmOfferPriorityVeryLow     kNtVmOfferPriorityVeryLow
#define VmOfferPriorityLow         kNtVmOfferPriorityLow
#define VmOfferPriorityBelowNormal kNtVmOfferPriorityBelowNormal
#define VmOfferPriorityNormal      kNtVmOfferPriorityNormal

#define _KWAIT_REASON             uint32_t
#define KWAIT_REASON              uint32_t
#define _OBJECT_INFORMATION_CLASS NtObjectInformationClass
#define OBJECT_INFORMATION_CLASS  int
#define _PROCESSINFOCLASS         NtProcessinfoclass
#define PROCESSINFOCLASS          int
#define _THREAD_STATE             NtThreadState
#define THREAD_STATE              int
#define _TOKEN_TYPE               NtTokenType
#define TOKEN_TYPE                int

#define _THREADINFOCLASS Nthreadinfoclass
#define THREADINFOCLASS  int

#define _THREAD_INFORMATION_CLASS NtThreadInformationClass
#define THREAD_INFORMATION_CLASS  int
#define PTHREAD_INFORMATION_CLASS int*

#define OWNER_SECURITY_INFORMATION     kNtOwnerSecurityInformation
#define GROUP_SECURITY_INFORMATION     kNtGroupSecurityInformation
#define DACL_SECURITY_INFORMATION      kNtDaclSecurityInformation
#define SACL_SECURITY_INFORMATION      kNtSaclSecurityInformation
#define LABEL_SECURITY_INFORMATION     kNtLabelSecurityInformation
#define ATTRIBUTE_SECURITY_INFORMATION kNtAttributeSecurityInformation
#define SCOPE_SECURITY_INFORMATION     kNtScopeSecurityInformation
#define PROCESS_TRUST_LABEL_SECURITY_INFORMATION \
  kNtProcessTrustLabelSecurityInformation
#define ACCESS_FILTER_SECURITY_INFORMATION  kNtAccessFilterSecurityInformation
#define BACKUP_SECURITY_INFORMATION         kNtBackupSecurityInformation
#define PROTECTED_DACL_SECURITY_INFORMATION kNtProtectedDaclSecurityInformation
#define PROTECTED_SACL_SECURITY_INFORMATION kNtProtectedSaclSecurityInformation
#define UNPROTECTED_DACL_SECURITY_INFORMATION \
  kNtUnprotectedDaclSecurityInformation
#define UNPROTECTED_SACL_SECURITY_INFORMATION \
  kNtUnprotectedSaclSecurityInformation

#define STARTF_USESHOWWINDOW    kNtNtStartfUseshowwindow
#define STARTF_USESIZE          kNtNtStartfUsesize
#define STARTF_USEPOSITION      kNtNtStartfUseposition
#define STARTF_USECOUNTCHARS    kNtNtStartfUsecountchars
#define STARTF_USEFILLATTRIBUTE kNtNtStartfUsefillattribute
#define STARTF_RUNFULLSCREEN    kNtNtStartfRunfullscreen
#define STARTF_FORCEONFEEDBACK  kNtNtStartfForceonfeedback
#define STARTF_FORCEOFFFEEDBACK kNtNtStartfForceofffeedback
#define STARTF_USESTDHANDLES    kNtNtStartfUsestdhandles
#define STARTF_USEHOTKEY        kNtNtStartfUsehotkey
#define STARTF_TITLEISLINKNAME  kNtNtStartfTitleislinkname
#define STARTF_TITLEISAPPID     kNtNtStartfTitleisappid
#define STARTF_PREVENTPINNING   kNtNtStartfPreventpinning
#define STARTF_UNTRUSTEDSOURCE  kNtNtStartfUntrustedsource

#define MEM_COMMIT               kNtMemCommit
#define MEM_RESERVE              kNtMemReserve
#define MEM_DECOMMIT             kNtMemDecommit
#define MEM_RELEASE              kNtMemRelease
#define MEM_FREE                 kNtMemFree
#define MEM_PRIVATE              kNtMemPrivate
#define MEM_MAPPED               kNtMemMapped
#define MEM_RESET                kNtMemReset
#define MEM_TOP_DOWN             kNtMemTopDown
#define MEM_WRITE_WATCH          kNtMemWriteWatch
#define MEM_PHYSICAL             kNtMemPhysical
#define MEM_LARGE_PAGES          kNtMemLargePages
#define MEM_4MB_PAGES            kNtMem4mbPages
#define PAGE_NOACCESS            kNtPageNoaccess
#define PAGE_READONLY            kNtPageReadonly
#define PAGE_READWRITE           kNtPageReadwrite
#define PAGE_WRITECOPY           kNtPageWritecopy
#define PAGE_EXECUTE             kNtPageExecute
#define PAGE_EXECUTE_READ        kNtPageExecuteRead
#define PAGE_EXECUTE_READWRITE   kNtPageExecuteReadwrite
#define PAGE_EXECUTE_WRITECOPY   kNtPageExecuteWritecopy
#define PAGE_GUARD               kNtPageGuard
#define PAGE_NOCACHE             kNtPageNocache
#define PAGE_WRITECOMBINE        kNtPageWritecombine
#define FILE_MAP_COPY            kNtFileMapCopy
#define FILE_MAP_WRITE           kNtFileMapWrite
#define FILE_MAP_READ            kNtFileMapRead
#define FILE_MAP_EXECUTE         kNtFileMapExecute
#define FILE_MAP_RESERVE         kNtFileMapReserve
#define FILE_MAP_TARGETS_INVALID kNtFileMapTargetsInvalid
#define FILE_MAP_LARGE_PAGES     kNtFileMapLargePages

#define SECTION_QUERY                kNtSectionQuery
#define SECTION_MAP_WRITE            kNtSectionMapWrite
#define SECTION_MAP_READ             kNtSectionMapRead
#define SECTION_MAP_EXECUTE          kNtSectionMapExecute
#define SECTION_EXTEND_SIZE          kNtSectionExtendSize
#define SECTION_MAP_EXECUTE_EXPLICIT kNtSectionMapExecuteExplicit

#define CTRL_CEVENT         kNtCtrlCEvent
#define CTRL_BREAK_EVENT    kNtCtrlBreakEvent
#define CTRL_CLOSE_EVENT    kNtCtrlCloseEvent
#define CTRL_LOGOFF_EVENT   kNtCtrlLogoffEvent
#define CTRL_SHUTDOWN_EVENT kNtCtrlShutdownEvent

#define FILE_ATTRIBUTE_NORMAL              kNtFileAttributeNormal
#define FILE_ATTRIBUTE_HIDDEN              kNtFileAttributeHidden
#define FILE_FLAG_WRITE_THROUGH            kNtFileFlagWriteThrough
#define FILE_FLAG_OVERLAPPED               kNtFileFlagOverlapped
#define FILE_FLAG_NO_BUFFERING             kNtFileFlagNoBuffering
#define FILE_FLAG_RANDOM_ACCESS            kNtFileFlagRandomAccess
#define FILE_FLAG_SEQUENTIAL_SCAN          kNtFileFlagSequentialScan
#define FILE_FLAG_DELETE_ON_CLOSE          kNtFileFlagDeleteOnClose
#define FILE_FLAG_BACKUP_SEMANTICS         kNtFileFlagBackupSemantics
#define FILE_FLAG_POSIX_SEMANTICS          kNtFileFlagPosixSemantics
#define FILE_FLAG_OPEN_REPARSE_POINT       kNtFileFlagOpenReparsePoint
#define FILE_FLAG_OPEN_NO_RECALL           kNtFileFlagOpenNoRecall
#define FILE_FLAG_FIRST_PIPE_INSTANCE      kNtFileFlagFirstPipeInstance
#define FILE_LIST_DIRECTORY                kNtFileListDirectory
#define FILE_ATTRIBUTE_ARCHIVE             kNtFileAttributeArchive
#define FILE_ATTRIBUTE_COMPRESSED          kNtFileAttributeCompressed
#define FILE_ATTRIBUTE_DEVICE              kNtFileAttributeDevice
#define FILE_ATTRIBUTE_DIRECTORY           kNtFileAttributeDirectory
#define FILE_ATTRIBUTE_ENCRYPTED           kNtFileAttributeEncrypted
#define FILE_ATTRIBUTE_NOT_CONTENT_INDEXED kNtFileAttributeNotContentIndexed
#define FILE_ATTRIBUTE_OFFLINE             kNtFileAttributeOffline
#define FILE_ATTRIBUTE_READONLY            kNtFileAttributeReadonly
#define FILE_ATTRIBUTE_REPARSE_POINT       kNtFileAttributeReparsePoint
#define FILE_ATTRIBUTE_SPARSE_FILE         kNtFileAttributeSparseFile
#define FILE_ATTRIBUTE_SYSTEM              kNtFileAttributeSystem
#define FILE_ATTRIBUTE_TEMPORARY           kNtFileAttributeTemporary

#define CREATE_NEW           kNtCreateNew
#define CREATE_ALWAYS        kNtCreateAlways
#define OPEN_EXISTING        kNtOpenExisting
#define OPEN_ALWAYS          kNtOpenAlways
#define TRUNCATE_EXISTING    kNtTruncateExisting
#define FILE_SHARE_EXCLUSIVE kNtFileShareExclusive
#define FILE_SHARE_READ      kNtFileShareRead
#define FILE_SHARE_WRITE     kNtFileShareWrite
#define FILE_SHARE_DELETE    kNtFileShareDelete

#define INVALID_HANDLE_VALUE kNtInvalidHandleValue
#define STD_INPUT_HANDLE     kNtStdInputHandle
#define STD_OUTPUT_HANDLE    kNtStdOutputHandle
#define STD_ERROR_HANDLE     kNtStdErrorHandle

#define CONSOLE_NO_SELECTION          kNtConsoleNoSelection
#define CONSOLE_SELECTION_IN_PROGRESS kNtConsoleSelectionInProgress
#define CONSOLE_SELECTION_NOT_EMPTY   kNtConsoleSelectionNotEmpty
#define CONSOLE_MOUSE_SELECTION       kNtConsoleMouseSelection
#define CONSOLE_MOUSE_DOWN            kNtConsoleMouseDown

#define MB_OK                        kNtMbOk
#define MB_OKCANCEL                  kNtMbOkcancel
#define MB_ABORTRETRYIGNORE          kNtMbAbortretryignore
#define MB_YESNOCANCEL               kNtMbYesnocancel
#define MB_YESNO                     kNtMbYesno
#define MB_RETRYCANCEL               kNtMbRetrycancel
#define MB_CANCELTRYCONTINUE         kNtMbCanceltrycontinue
#define MB_ICONHAND                  kNtMbIconhand
#define MB_ICONQUESTION              kNtMbIconquestion
#define MB_ICONEXCLAMATION           kNtMbIconexclamation
#define MB_ICONASTERISK              kNtMbIconasterisk
#define MB_USERICON                  kNtMbUsericon
#define MB_ICONWARNING               kNtMbIconwarning
#define MB_ICONERROR                 kNtMbIconerror
#define MB_ICONINFORMATION           kNtMbIconinformation
#define MB_ICONSTOP                  kNtMbIconstop
#define MB_DEFBUTTON1                kNtMbDefbutton1
#define MB_DEFBUTTON2                kNtMbDefbutton2
#define MB_DEFBUTTON3                kNtMbDefbutton3
#define MB_DEFBUTTON4                kNtMbDefbutton4
#define MB_APPLMODAL                 kNtMbApplmodal
#define MB_SYSTEMMODAL               kNtMbSystemmodal
#define MB_TASKMODAL                 kNtMbTaskmodal
#define MB_HELP                      kNtMbHelp
#define MB_NOFOCUS                   kNtMbNofocus
#define MB_SETFOREGROUND             kNtMbSetforeground
#define MB_DEFAULT_DESKTOP_ONLY      kNtMbDefaultDesktopOnly
#define MB_TOPMOST                   kNtMbTopmost
#define MB_RIGHT                     kNtMbRight
#define MB_RTLREADING                kNtMbRtlreading
#define MB_SERVICE_NOTIFICATION      kNtMbServiceNotification
#define MB_SERVICE_NOTIFICATION_NT3X kNtMbServiceNotificationNt3x
#define MB_TYPEMASK                  kNtMbTypemask
#define MB_ICONMASK                  kNtMbIconmask
#define MB_DEFMASK                   kNtMbDefmask
#define MB_MODEMASK                  kNtMbModemask
#define MB_MISCMASK                  kNtMbMiscmask

#define IDOK       kNtIdok
#define IDCANCEL   kNtIdcancel
#define IDABORT    kNtIdabort
#define IDRETRY    kNtIdretry
#define IDIGNORE   kNtIdignore
#define IDYES      kNtIdyes
#define IDNO       kNtIdno
#define IDCLOSE    kNtIdclose
#define IDHELP     kNtIdhelp
#define IDTRYAGAIN kNtIdtryagain
#define IDCONTINUE kNtIdcontinue

#define PROCESS_TERMINATE                 kNtProcessTerminate
#define PROCESS_CREATE_THREAD             kNtProcessCreateThread
#define PROCESS_SET_SESSIONID             kNtProcessSetSessionid
#define PROCESS_VM_OPERATION              kNtProcessVmOperation
#define PROCESS_VM_READ                   kNtProcessVmRead
#define PROCESS_VM_WRITE                  kNtProcessVmWrite
#define PROCESS_DUP_HANDLE                kNtProcessDupHandle
#define PROCESS_CREATE_PROCESS            kNtProcessCreateProcess
#define PROCESS_SET_QUOTA                 kNtProcessSetQuota
#define PROCESS_SET_INFORMATION           kNtProcessSetInformation
#define PROCESS_QUERY_INFORMATION         kNtProcessQueryInformation
#define PROCESS_SUSPEND_RESUME            kNtProcessSuspendResume
#define PROCESS_QUERY_LIMITED_INFORMATION kNtProcessQueryLimitedInformation
#define PROCESS_SET_LIMITED_INFORMATION   kNtProcessSetLimitedInformation
#define PROCESS_ALL_ACCESS                kNtProcessAllAccess

#define GENERIC_READ             kNtGenericRead
#define GENERIC_WRITE            kNtGenericWrite
#define GENERIC_EXECUTE          kNtGenericExecute
#define GENERIC_ALL              kNtGenericAll
#define DELETE                   kNtDelete
#define READ_CONTROL             kNtReadControl
#define WRITE_DAC                kNtWriteDac
#define WRITE_OWNER              kNtWriteOwner
#define SYNCHRONIZE              kNtSynchronize
#define STANDARD_RIGHTS_REQUIRED kNtStandardRightsRequired
#define STANDARD_RIGHTS_READ     kNtStandardRightsRead
#define STANDARD_RIGHTS_WRITE    kNtStandardRightsWrite
#define STANDARD_RIGHTS_EXECUTE  kNtStandardRightsExecute
#define STANDARD_RIGHTS_ALL      kNtStandardRightsAll
#define SPECIFIC_RIGHTS_ALL      kNtSpecificRightsAll
#define ACCESS_SYSTEM_SECURITY   kNtAccessSystemSecurity
#define MAXIMUM_ALLOWED          kNtMaximumAllowed
#define GENERIC_READ             kNtGenericRead
#define GENERIC_WRITE            kNtGenericWrite
#define GENERIC_EXECUTE          kNtGenericExecute
#define GENERIC_ALL              kNtGenericAll

#define FILE_TYPE_UNKNOWN kNtFileTypeUnknown
#define FILE_TYPE_DISK    kNtFileTypeDisk
#define FILE_TYPE_CHAR    kNtFileTypeChar
#define FILE_TYPE_PIPE    kNtFileTypePipe
#define FILE_TYPE_REMOTE  kNtFileTypeRemote

#define NT_DEBUG_PROCESS                    kNtDebugProcess
#define NT_DEBUG_ONLY_THIS_PROCESS          kNtDebugOnlyThisProcess
#define NT_CREATE_SUSPENDED                 kNtCreateSuspended
#define NT_DETACHED_PROCESS                 kNtDetachedProcess
#define NT_CREATE_NEW_CONSOLE               kNtCreateNewConsole
#define NT_NORMAL_PRIORITY_CLASS            kNtNormalPriorityClass
#define NT_IDLE_PRIORITY_CLASS              kNtIdlePriorityClass
#define NT_HIGH_PRIORITY_CLASS              kNtHighPriorityClass
#define NT_REALTIME_PRIORITY_CLASS          kNtRealtimePriorityClass
#define NT_CREATE_NEW_PROCESS_GROUP         kNtCreateNewProcessGroup
#define NT_CREATE_UNICODE_ENVIRONMENT       kNtCreateUnicodeEnvironment
#define NT_CREATE_SEPARATE_WOW_VDM          kNtCreateSeparateWowVdm
#define NT_CREATE_SHARED_WOW_VDM            kNtCreateSharedWowVdm
#define NT_CREATE_FORCEDOS                  kNtCreateForcedos
#define NT_BELOW_NORMAL_PRIORITY_CLASS      kNtBelowNormalPriorityClass
#define NT_ABOVE_NORMAL_PRIORITY_CLASS      kNtAboveNormalPriorityClass
#define NT_INHERIT_PARENT_AFFINITY          kNtInheritParentAffinity
#define NT_CREATE_PROTECTED_PROCESS         kNtCreateProtectedProcess
#define NT_EXTENDED_STARTUPINFO_PRESENT     kNtExtendedStartupinfoPresent
#define NT_PROCESS_MODE_BACKGROUND_BEGIN    kNtProcessModeBackgroundBegin
#define NT_PROCESS_MODE_BACKGROUND_END      kNtProcessModeBackgroundEnd
#define NT_CREATE_SECURE_PROCESS            kNtCreateSecureProcess
#define NT_CREATE_BREAKAWAY_FROM_JOB        kNtCreateBreakawayFromJob
#define NT_CREATE_PRESERVE_CODE_AUTHZ_LEVEL kNtCreatePreserveCodeAuthzLevel
#define NT_CREATE_DEFAULT_ERROR_MODE        kNtCreateDefaultErrorMode
#define NT_CREATE_NO_WINDOW                 kNtCreateNoWindow
#define NT_PROFILE_USER                     kNtProfileUser
#define NT_PROFILE_KERNEL                   kNtProfileKernel
#define NT_PROFILE_SERVER                   kNtProfileServer
#define NT_CREATE_IGNORE_SYSTEM_DEFAULT     kNtCreateIgnoreSystemDefault

#define FILE_READ_DATA            kNtFileReadData
#define FILE_WRITE_DATA           kNtFileWriteData
#define FILE_ADD_FILE             kNtFileAddFile
#define FILE_APPEND_DATA          kNtFileAppendData
#define FILE_ADD_SUBDIRECTORY     kNtFileAddSubdirectory
#define FILE_CREATE_PIPE_INSTANCE kNtFileCreatePipeInstance
#define FILE_READ_EA              kNtFileReadEa
#define FILE_WRITE_EA             kNtFileWriteEa
#define FILE_EXECUTE              kNtFileExecute
#define FILE_TRAVERSE             kNtFileTraverse
#define FILE_DELETE_CHILD         kNtFileDeleteChild
#define FILE_READ_ATTRIBUTES      kNtFileReadAttributes
#define FILE_WRITE_ATTRIBUTES     kNtFileWriteAttributes
#define FILE_ALL_ACCESS           kNtFileAllAccess
#define FILE_GENERIC_READ         kNtFileGenericRead
#define FILE_GENERIC_WRITE        kNtFileGenericWrite
#define FILE_GENERIC_EXECUTE      kNtFileGenericExecute

#define TOKEN_PRIMARY       kNtTokenPrimary
#define TOKEN_IMPERSONATION kNtTokenImpersonation

#define TOKEN_PRIMARY           kNtTokenPrimary
#define TOKEN_IMPERSONATION     kNtTokenImpersonation
#define SECURITY_ANONYMOUS      kNtSecurityAnonymous
#define SECURITY_IDENTIFICATION kNtSecurityIdentification
#define SECURITY_IMPERSONATION  kNtSecurityImpersonation
#define SECURITY_DELEGATION     kNtSecurityDelegation

#define TOKEN_DUPLICATE                 kNtTokenDuplicate
#define TOKEN_IMPERSONATE               kNtTokenImpersonate
#define TOKEN_QUERY                     kNtTokenQuery
#define TOKEN_QUERY_SOURCE              kNtTokenQuerySource
#define TOKEN_ADJUST_PRIVILEGES         kNtTokenAdjustPrivileges
#define TOKEN_ADJUST_GROUPS             kNtTokenAdjustGroups
#define TOKEN_ADJUST_DEFAULT            kNtTokenAdjustDefault
#define TOKEN_ADJUST_SESSIONID          kNtTokenAdjustSessionid
#define TOKEN_ALL_ACCESS_P              kNtTokenAllAccessP
#define TOKEN_ALL_ACCESS                kNtTokenAllAccess
#define TOKEN_READ                      kNtTokenRead
#define TOKEN_WRITE                     kNtTokenWrite
#define TOKEN_EXECUTE                   kNtTokenExecute
#define TOKEN_TRUST_CONSTRAINT_MASK     kNtTokenTrustConstraintMask
#define TOKEN_ACCESS_PSEUDO_HANDLE_WIN8 kNtTokenAccessPseudoHandleWin8
#define TOKEN_ACCESS_PSEUDO_HANDLE      kNtTokenAccessPseudoHandle

#define FOREGROUND_BLUE      kNtForegroundBlue
#define FOREGROUND_GREEN     kNtForegroundGreen
#define FOREGROUND_RED       kNtForegroundRed
#define FOREGROUND_INTENSITY kNtForegroundIntensity
#define BACKGROUND_BLUE      kNtBackgroundBlue
#define BACKGROUND_GREEN     kNtBackgroundGreen
#define BACKGROUND_RED       kNtBackgroundRed
#define BACKGROUND_INTENSITY kNtBackgroundIntensity

#define UNLEN 256

#define DUPLICATE_CLOSE_SOURCE kNtDuplicateCloseSource
#define DUPLICATE_SAME_ACCESS  kNtDuplicateSameAccess

#define IMAGE_FILE_MACHINE_UNKNOWN     kNtImageFileMachineUnknown
#define IMAGE_FILE_MACHINE_TARGET_HOST kNtImageFileMachineTargetHost
#define IMAGE_FILE_MACHINE_I386        kNtImageFileMachineI386
#define IMAGE_FILE_MACHINE_R3000       kNtImageFileMachineR3000
#define IMAGE_FILE_MACHINE_R4000       kNtImageFileMachineR4000
#define IMAGE_FILE_MACHINE_R10000      kNtImageFileMachineR10000
#define IMAGE_FILE_MACHINE_WCEMIPSV2   kNtImageFileMachineWcemipsv2
#define IMAGE_FILE_MACHINE_ALPHA       kNtImageFileMachineAlpha
#define IMAGE_FILE_MACHINE_SH3         kNtImageFileMachineSh3
#define IMAGE_FILE_MACHINE_SH3DSP      kNtImageFileMachineSh3dsp
#define IMAGE_FILE_MACHINE_SH3E        kNtImageFileMachineSh3e
#define IMAGE_FILE_MACHINE_SH4         kNtImageFileMachineSh4
#define IMAGE_FILE_MACHINE_SH5         kNtImageFileMachineSh5
#define IMAGE_FILE_MACHINE_ARM         kNtImageFileMachineArm
#define IMAGE_FILE_MACHINE_THUMB       kNtImageFileMachineThumb
#define IMAGE_FILE_MACHINE_ARMNT       kNtImageFileMachineArmnt
#define IMAGE_FILE_MACHINE_AM33        kNtImageFileMachineAm33
#define IMAGE_FILE_MACHINE_POWERPC     kNtImageFileMachinePowerpc
#define IMAGE_FILE_MACHINE_POWERPCFP   kNtImageFileMachinePowerpcfp
#define IMAGE_FILE_MACHINE_IA64        kNtImageFileMachineIa64
#define IMAGE_FILE_MACHINE_MIPS16      kNtImageFileMachineMips16
#define IMAGE_FILE_MACHINE_ALPHA64     kNtImageFileMachineAlpha64
#define IMAGE_FILE_MACHINE_MIPSFPU     kNtImageFileMachineMipsfpu
#define IMAGE_FILE_MACHINE_MIPSFPU16   kNtImageFileMachineMipsfpu16
#define IMAGE_FILE_MACHINE_AXP64       IMAGE_FILE_MACHINE_ALPHA64
#define IMAGE_FILE_MACHINE_TRICORE     kNtImageFileMachineTricore
#define IMAGE_FILE_MACHINE_CEF         kNtImageFileMachineCef
#define IMAGE_FILE_MACHINE_EBC         kNtImageFileMachineEbc
#define IMAGE_FILE_MACHINE_NEXGEN32E   kNtImageFileMachineNexgen32e
#define IMAGE_FILE_MACHINE_M32R        kNtImageFileMachineM32r
#define IMAGE_FILE_MACHINE_ARM64       kNtImageFileMachineArm64
#define IMAGE_FILE_MACHINE_CEE         kNtImageFileMachineCee

#define PE_32BIT kNtPe32bit
#define PE_64BIT kNtPe64bit

#define IMAGE_DLLCHARACTERISTICS_HIGH_ENTROPY_VA \
  kNtImageDllcharacteristicsHighEntropyVa
#define IMAGE_DLLCHARACTERISTICS_DYNAMIC_BASE \
  kNtImageDllcharacteristicsDynamicBase
#define IMAGE_DLLCHARACTERISTICS_FORCE_INTEGRITY \
  kNtImageDllcharacteristicsForceIntegrity
#define IMAGE_DLLCHARACTERISTICS_NX_COMPAT kNtImageDllcharacteristicsNxCompat
#define IMAGE_DLLCHARACTERISTICS_NO_ISOLATION \
  kNtImageDllcharacteristicsNoIsolation
#define IMAGE_DLLCHARACTERISTICS_NO_SEH  kNtImageDllcharacteristicsNoSeh
#define IMAGE_DLLCHARACTERISTICS_NO_BIND kNtImageDllcharacteristicsNoBind
#define IMAGE_DLLCHARACTERISTICS_APPCONTAINER \
  kNtImageDllcharacteristicsAppcontainer
#define IMAGE_DLLCHARACTERISTICS_WDM_DRIVER kNtImageDllcharacteristicsWdmDriver
#define IMAGE_DLLCHARACTERISTICS_GUARD_CF   kNtImageDllcharacteristicsGuardCf
#define IMAGE_DLLCHARACTERISTICS_TERMINAL_SERVER_AWARE \
  kNtImageDllcharacteristicsTerminalServerAware

#define IMAGE_SUBSYSTEM_UNKNOWN         kNtImageSubsystemUnknown
#define IMAGE_SUBSYSTEM_NATIVE          kNtImageSubsystemNative
#define IMAGE_SUBSYSTEM_WINDOWS_GUI     kNtImageSubsystemWindowsGui
#define IMAGE_SUBSYSTEM_WINDOWS_CUI     kNtImageSubsystemWindowsCui
#define IMAGE_SUBSYSTEM_OS2_CUI         kNtImageSubsystemOs2Cui
#define IMAGE_SUBSYSTEM_POSIX_CUI       kNtImageSubsystemPosixCui
#define IMAGE_SUBSYSTEM_NATIVE_WINDOWS  kNtImageSubsystemNativeWindows
#define IMAGE_SUBSYSTEM_WINDOWS_CE_GUI  kNtImageSubsystemWindowsCeGui
#define IMAGE_SUBSYSTEM_EFI_APPLICATION kNtImageSubsystemEfiApplication
#define IMAGE_SUBSYSTEM_EFI_BOOT_SERVICE_DRIVER \
  kNtImageSubsystemEfiBootServiceDriver
#define IMAGE_SUBSYSTEM_EFI_RUNTIME_DRIVER kNtImageSubsystemEfiRuntimeDriver
#define IMAGE_SUBSYSTEM_EFI_ROM            kNtImageSubsystemEfiRom
#define IMAGE_SUBSYSTEM_XBOX               kNtImageSubsystemXbox
#define IMAGE_SUBSYSTEM_WINDOWS_BOOT_APPLICATION \
  kNtImageSubsystemWindowsBootApplication
#define IMAGE_SUBSYSTEM_XBOX_CODE_CATALOG kNtImageSubsystemXboxCodeCatalog

#define IMAGE_FILE_RELOCS_STRIPPED         kNtImageFileRelocsStripped
#define IMAGE_FILE_EXECUTABLE_IMAGE        kNtImageFileExecutableImage
#define IMAGE_FILE_LINE_NUMS_STRIPPED      kNtImageFileLineNumsStripped
#define IMAGE_FILE_LOCAL_SYMS_STRIPPED     kNtImageFileLocalSymsStripped
#define IMAGE_FILE_AGGRESIVE_WS_TRIM       kNtImageFileAggresiveWsTrim
#define IMAGE_FILE_LARGE_ADDRESS_AWARE     kNtImageFileLargeAddressAware
#define IMAGE_FILE_BYTES_REVERSED_LO       kNtImageFileBytesReversedLo
#define IMAGE_FILE_32BIT_MACHINE           kNtImageFile_32bitMachine
#define IMAGE_FILE_DEBUG_STRIPPED          kNtImageFileDebugStripped
#define IMAGE_FILE_REMOVABLE_RUN_FROM_SWAP kNtImageFileRemovableRunFromSwap
#define IMAGE_FILE_NET_RUN_FROM_SWAP       kNtImageFileNetRunFromSwap
#define IMAGE_FILE_SYSTEM                  kNtImageFileSystem
#define IMAGE_FILE_DLL                     kNtImageFileDll
#define IMAGE_FILE_UP_SYSTEM_ONLY          kNtImageFileUpSystemOnly
#define IMAGE_FILE_BYTES_REVERSED_HI       kNtImageFileBytesReversedHi

#define IMAGE_DOS_SIGNATURE    kNtImageDosSignature
#define IMAGE_OS2_SIGNATURE    kNtImageOs2Signature
#define IMAGE_OS2_SIGNATURE_LE kNtImageOs2SignatureLe
#define IMAGE_VXD_SIGNATURE    kNtImageVxdSignature
#define IMAGE_NT_SIGNATURE     kNtImageNtSignature

#define RICHKEY                     kNtRichkey
#define COOKIE_DEFAULT              kNtCookieDefault
#define SIZE_OF_80387_REGISTERS     kNtSizeOf_80387Registers
#define MAXIMUM_SUPPORTED_EXTENSION kNtMaximumSupportedExtension

#define PE_SUBSYSTEM_WINDOWS_GUI kNtPeSubsystemWindowsGui
#define PE_SUBSYSTEM_WINDOWS_CUI kNtPeSubsystemWindowsCui

#define PE_FILE_RELOCS_STRIPPED     kNtPeFileRelocsStripped
#define PE_FILE_EXECUTABLE_IMAGE    kNtPeFileExecutableImage
#define PE_FILE_LINE_NUMS_STRIPPED  kNtPeFileLineNumsStripped
#define PE_FILE_LOCAL_SYMS_STRIPPED kNtPeFileLocalSymsStripped
#define PE_FILE_32BIT_MACHINE       kNtPeFile_32bitMachine
#define PE_FILE_DLL                 kNtPeFileDll

#define PE_SECTION_CNT_CODE               kNtPeSectionCntCode
#define PE_SECTION_CNT_INITIALIZED_DATA   kNtPeSectionCntInitializedData
#define PE_SECTION_CNT_UNINITIALIZED_DATA kNtPeSectionCntUninitializedData
#define PE_SECTION_GPREL                  kNtPeSectionGprel
#define PE_SECTION_MEM_DISCARDABLE        kNtPeSectionMemDiscardable
#define PE_SECTION_MEM_NOT_CACHED         kNtPeSectionMemNotCached
#define PE_SECTION_MEM_NOT_PAGED          kNtPeSectionMemNotPaged
#define PE_SECTION_MEM_SHARED             kNtPeSectionMemShared
#define PE_SECTION_MEM_EXECUTE            kNtPeSectionMemExecute
#define PE_SECTION_MEM_READ               kNtPeSectionMemRead
#define PE_SECTION_MEM_WRITE              kNtPeSectionMemWrite

#define PE_GUARD_CF_INSTRUMENTED           kNtPeGuardCfInstrumented
#define PE_GUARD_CFW_INSTRUMENTED          kNtPeGuardCfwInstrumented
#define PE_GUARD_CF_FUNCTION_TABLE_PRESENT kNtPeGuardCfFunctionTablePresent
#define PE_GUARD_SECURITY_COOKIE_UNUSED    kNtPeGuardSecurityCookieUnused

#define PE_REL_BASED_ABSOLUTE       kNtPeRelBasedAbsolute
#define PE_REL_BASED_HIGH           kNtPeRelBasedHigh
#define PE_REL_BASED_LOW            kNtPeRelBasedLow
#define PE_REL_BASED_HIGHLOW        kNtPeRelBasedHighlow
#define PE_REL_BASED_HIGHADJ        kNtPeRelBasedHighadj
#define PE_REL_BASED_MIPS_JMPADDR   kNtPeRelBasedMipsJmpaddr
#define PE_REL_BASED_SECTION        kNtPeRelBasedSection
#define PE_REL_BASED_REL32          kNtPeRelBasedRel32
#define PE_REL_BASED_MIPS_JMPADDR16 kNtPeRelBasedMipsJmpaddr16
#define PE_REL_BASED_IA64_IMM64     kNtPeRelBasedIa64Imm64
#define PE_REL_BASED_DIR64          kNtPeRelBasedDir64
#define PE_REL_BASED_HIGH3ADJ       kNtPeRelBasedHigh3adj

#define IMAGE_DIRECTORY_ENTRY_EXPORT         kNtImageDirectoryEntryExport
#define IMAGE_DIRECTORY_ENTRY_IMPORT         kNtImageDirectoryEntryImport
#define IMAGE_DIRECTORY_ENTRY_RESOURCE       kNtImageDirectoryEntryResource
#define IMAGE_DIRECTORY_ENTRY_EXCEPTION      kNtImageDirectoryEntryException
#define IMAGE_DIRECTORY_ENTRY_SECURITY       kNtImageDirectoryEntrySecurity
#define IMAGE_DIRECTORY_ENTRY_BASERELOC      kNtImageDirectoryEntryBasereloc
#define IMAGE_DIRECTORY_ENTRY_DEBUG          kNtImageDirectoryEntryDebug
#define IMAGE_DIRECTORY_ENTRY_ARCHITECTURE   kNtImageDirectoryEntryArchitecture
#define IMAGE_DIRECTORY_ENTRY_GLOBALPTR      kNtImageDirectoryEntryGlobalptr
#define IMAGE_DIRECTORY_ENTRY_TLS            kNtImageDirectoryEntryTls
#define IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG    kNtImageDirectoryEntryLoadConfig
#define IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT   kNtImageDirectoryEntryBoundImport
#define IMAGE_DIRECTORY_ENTRY_IAT            kNtImageDirectoryEntryIat
#define IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT   kNtImageDirectoryEntryDelayImport
#define IMAGE_DIRECTORY_ENTRY_COM_DESCRIPTOR kNtImageDirectoryEntryComDescriptor

#define IMAGE_SCN_TYPE_NO_PAD            kNtImageScnTypeNoPad
#define IMAGE_SCN_CNT_CODE               kNtImageScnCntCode
#define IMAGE_SCN_CNT_INITIALIZED_DATA   kNtImageScnCntInitializedData
#define IMAGE_SCN_CNT_UNINITIALIZED_DATA kNtImageScnCntUninitializedData
#define IMAGE_SCN_LNK_OTHER              kNtImageScnLnkOther
#define IMAGE_SCN_LNK_INFO               kNtImageScnLnkInfo
#define IMAGE_SCN_LNK_REMOVE             kNtImageScnLnkRemove
#define IMAGE_SCN_LNK_COMDAT             kNtImageScnLnkComdat
#define IMAGE_SCN_NO_DEFER_SPEC_EXC      kNtImageScnNoDeferSpecExc
#define IMAGE_SCN_GPREL                  kNtImageScnGprel
#define IMAGE_SCN_MEM_FARDATA            kNtImageScnMemFardata
#define IMAGE_SCN_MEM_PURGEABLE          kNtImageScnMemPurgeable
#define IMAGE_SCN_MEM_16BIT              kNtImageScnMem16bit
#define IMAGE_SCN_MEM_LOCKED             kNtImageScnMemLocked
#define IMAGE_SCN_MEM_PRELOAD            kNtImageScnMemPreload

#define IMAGE_SCN_ALIGN_1BYTES    kNtImageScnAlign1bytes
#define IMAGE_SCN_ALIGN_2BYTES    kNtImageScnAlign2bytes
#define IMAGE_SCN_ALIGN_4BYTES    kNtImageScnAlign4bytes
#define IMAGE_SCN_ALIGN_8BYTES    kNtImageScnAlign8bytes
#define IMAGE_SCN_ALIGN_16BYTES   kNtImageScnAlign16bytes
#define IMAGE_SCN_ALIGN_32BYTES   kNtImageScnAlign32bytes
#define IMAGE_SCN_ALIGN_64BYTES   kNtImageScnAlign64bytes
#define IMAGE_SCN_ALIGN_128BYTES  kNtImageScnAlign128bytes
#define IMAGE_SCN_ALIGN_256BYTES  kNtImageScnAlign256bytes
#define IMAGE_SCN_ALIGN_512BYTES  kNtImageScnAlign512bytes
#define IMAGE_SCN_ALIGN_1024BYTES kNtImageScnAlign1024bytes
#define IMAGE_SCN_ALIGN_2048BYTES kNtImageScnAlign2048bytes
#define IMAGE_SCN_ALIGN_4096BYTES kNtImageScnAlign4096bytes
#define IMAGE_SCN_ALIGN_8192BYTES kNtImageScnAlign8192bytes
#define IMAGE_SCN_ALIGN_MASK      kNtImageScnAlignMask

#define IMAGE_SCN_LNK_NRELOC_OVFL kNtImageScnLnkNrelocOvfl
#define IMAGE_SCN_MEM_DISCARDABLE kNtImageScnMemDiscardable
#define IMAGE_SCN_MEM_NOT_CACHED  kNtImageScnMemNotCached
#define IMAGE_SCN_MEM_NOT_PAGED   kNtImageScnMemNotPaged
#define IMAGE_SCN_MEM_SHARED      kNtImageScnMemShared
#define IMAGE_SCN_MEM_EXECUTE     kNtImageScnMemExecute
#define IMAGE_SCN_MEM_READ        kNtImageScnMemRead
#define IMAGE_SCN_MEM_WRITE       kNtImageScnMemWrite
#define IMAGE_SCN_SCALE_INDEX     kNtImageScnScaleIndex

#define IMAGE_SYM_UNDEFINED              kNtImageSymUndefined
#define IMAGE_SYM_ABSOLUTE               kNtImageSymAbsolute
#define IMAGE_SYM_DEBUG                  kNtImageSymDebug
#define IMAGE_SYM_SECTION_MAX            kNtImageSymSectionMax
#define IMAGE_SYM_SECTION_MAX_EX         kNtImageSymSectionMaxEx
#define IMAGE_SYM_TYPE_NULL              kNtImageSymTypeNull
#define IMAGE_SYM_TYPE_VOID              kNtImageSymTypeVoid
#define IMAGE_SYM_TYPE_CHAR              kNtImageSymTypeChar
#define IMAGE_SYM_TYPE_SHORT             kNtImageSymTypeShort
#define IMAGE_SYM_TYPE_INT               kNtImageSymTypeInt
#define IMAGE_SYM_TYPE_LONG              kNtImageSymTypeLong
#define IMAGE_SYM_TYPE_FLOAT             kNtImageSymTypeFloat
#define IMAGE_SYM_TYPE_DOUBLE            kNtImageSymTypeDouble
#define IMAGE_SYM_TYPE_STRUCT            kNtImageSymTypeStruct
#define IMAGE_SYM_TYPE_UNION             kNtImageSymTypeUnion
#define IMAGE_SYM_TYPE_ENUM              kNtImageSymTypeEnum
#define IMAGE_SYM_TYPE_MOE               kNtImageSymTypeMoe
#define IMAGE_SYM_TYPE_BYTE              kNtImageSymTypeByte
#define IMAGE_SYM_TYPE_WORD              kNtImageSymTypeWord
#define IMAGE_SYM_TYPE_UINT              kNtImageSymTypeUint
#define IMAGE_SYM_TYPE_DWORD             kNtImageSymTypeDword
#define IMAGE_SYM_TYPE_PCODE             kNtImageSymTypePcode
#define IMAGE_SYM_DTYPE_NULL             kNtImageSymDtypeNull
#define IMAGE_SYM_DTYPE_POINTER          kNtImageSymDtypePointer
#define IMAGE_SYM_DTYPE_FUNCTION         kNtImageSymDtypeFunction
#define IMAGE_SYM_DTYPE_ARRAY            kNtImageSymDtypeArray
#define IMAGE_SYM_CLASS_END_OF_FUNCTION  kNtImageSymClassEndOfFunction
#define IMAGE_SYM_CLASS_NULL             kNtImageSymClassNull
#define IMAGE_SYM_CLASS_AUTOMATIC        kNtImageSymClassAutomatic
#define IMAGE_SYM_CLASS_EXTERNAL         kNtImageSymClassExternal
#define IMAGE_SYM_CLASS_STATIC           kNtImageSymClassStatic
#define IMAGE_SYM_CLASS_REGISTER         kNtImageSymClassRegister
#define IMAGE_SYM_CLASS_EXTERNAL_DEF     kNtImageSymClassExternalDef
#define IMAGE_SYM_CLASS_LABEL            kNtImageSymClassLabel
#define IMAGE_SYM_CLASS_UNDEFINED_LABEL  kNtImageSymClassUndefinedLabel
#define IMAGE_SYM_CLASS_MEMBER_OF_STRUCT kNtImageSymClassMemberOfStruct
#define IMAGE_SYM_CLASS_ARGUMENT         kNtImageSymClassArgument
#define IMAGE_SYM_CLASS_STRUCT_TAG       kNtImageSymClassStructTag
#define IMAGE_SYM_CLASS_MEMBER_OF_UNION  kNtImageSymClassMemberOfUnion
#define IMAGE_SYM_CLASS_UNION_TAG        kNtImageSymClassUnionTag
#define IMAGE_SYM_CLASS_TYPE_DEFINITION  kNtImageSymClassTypeDefinition
#define IMAGE_SYM_CLASS_UNDEFINED_STATIC kNtImageSymClassUndefinedStatic
#define IMAGE_SYM_CLASS_ENUM_TAG         kNtImageSymClassEnumTag
#define IMAGE_SYM_CLASS_MEMBER_OF_ENUM   kNtImageSymClassMemberOfEnum
#define IMAGE_SYM_CLASS_REGISTER_PARAM   kNtImageSymClassRegisterParam
#define IMAGE_SYM_CLASS_BIT_FIELD        kNtImageSymClassBitField
#define IMAGE_SYM_CLASS_FAR_EXTERNAL     kNtImageSymClassFarExternal
#define IMAGE_SYM_CLASS_BLOCK            kNtImageSymClassBlock
#define IMAGE_SYM_CLASS_FUNCTION         kNtImageSymClassFunction
#define IMAGE_SYM_CLASS_END_OF_STRUCT    kNtImageSymClassEndOfStruct
#define IMAGE_SYM_CLASS_FILE             kNtImageSymClassFile
#define IMAGE_SYM_CLASS_SECTION          kNtImageSymClassSection
#define IMAGE_SYM_CLASS_WEAK_EXTERNAL    kNtImageSymClassWeakExternal
#define IMAGE_SYM_CLASS_CLR_TOKEN        kNtImageSymClassClrToken

#define IMAGE_COMDAT_SELECT_NODUPLICATES kNtImageComdatSelectNoduplicates
#define IMAGE_COMDAT_SELECT_ANY          kNtImageComdatSelectAny
#define IMAGE_COMDAT_SELECT_SAME_SIZE    kNtImageComdatSelectSameSize
#define IMAGE_COMDAT_SELECT_EXACT_MATCH  kNtImageComdatSelectExactMatch
#define IMAGE_COMDAT_SELECT_ASSOCIATIVE  kNtImageComdatSelectAssociative
#define IMAGE_COMDAT_SELECT_LARGEST      kNtImageComdatSelectLargest
#define IMAGE_COMDAT_SELECT_NEWEST       kNtImageComdatSelectNewest

#define IMAGE_WEAK_EXTERN_SEARCH_NOLIBRARY kNtImageWeakExternSearchNolibrary
#define IMAGE_WEAK_EXTERN_SEARCH_LIBRARY   kNtImageWeakExternSearchLibrary
#define IMAGE_WEAK_EXTERN_SEARCH_ALIAS     kNtImageWeakExternSearchAlias
#define IMAGE_WEAK_EXTERN_ANTI_DEPENDENCY  kNtImageWeakExternAntiDependency

#define IMAGE_REL_NEXGEN32E_ABSOLUTE kNtImageRelNexgen32eAbsolute
#define IMAGE_REL_NEXGEN32E_ADDR64   kNtImageRelNexgen32eAddr64
#define IMAGE_REL_NEXGEN32E_ADDR32   kNtImageRelNexgen32eAddr32
#define IMAGE_REL_NEXGEN32E_ADDR32NB kNtImageRelNexgen32eAddr32nb
#define IMAGE_REL_NEXGEN32E_REL32    kNtImageRelNexgen32eRel32
#define IMAGE_REL_NEXGEN32E_REL32_1  kNtImageRelNexgen32eRel321
#define IMAGE_REL_NEXGEN32E_REL32_2  kNtImageRelNexgen32eRel322
#define IMAGE_REL_NEXGEN32E_REL32_3  kNtImageRelNexgen32eRel323
#define IMAGE_REL_NEXGEN32E_REL32_4  kNtImageRelNexgen32eRel324
#define IMAGE_REL_NEXGEN32E_REL32_5  kNtImageRelNexgen32eRel325
#define IMAGE_REL_NEXGEN32E_SECTION  kNtImageRelNexgen32eSection
#define IMAGE_REL_NEXGEN32E_SECREL   kNtImageRelNexgen32eSecrel
#define IMAGE_REL_NEXGEN32E_SECREL7  kNtImageRelNexgen32eSecrel7
#define IMAGE_REL_NEXGEN32E_TOKEN    kNtImageRelNexgen32eToken
#define IMAGE_REL_NEXGEN32E_SREL32   kNtImageRelNexgen32eSrel32
#define IMAGE_REL_NEXGEN32E_PAIR     kNtImageRelNexgen32ePair
#define IMAGE_REL_NEXGEN32E_SSPAN32  kNtImageRelNexgen32eSspan32

#define IMAGE_REL_BASED_ABSOLUTE           kNtImageRelBasedAbsolute
#define IMAGE_REL_BASED_HIGH               kNtImageRelBasedHigh
#define IMAGE_REL_BASED_LOW                kNtImageRelBasedLow
#define IMAGE_REL_BASED_HIGHLOW            kNtImageRelBasedHighlow
#define IMAGE_REL_BASED_HIGHADJ            kNtImageRelBasedHighadj
#define IMAGE_REL_BASED_MACHINE_SPECIFIC_5 kNtImageRelBasedMachineSpecific5
#define IMAGE_REL_BASED_RESERVED           kNtImageRelBasedReserved
#define IMAGE_REL_BASED_MACHINE_SPECIFIC_7 kNtImageRelBasedMachineSpecific7
#define IMAGE_REL_BASED_MACHINE_SPECIFIC_8 kNtImageRelBasedMachineSpecific8
#define IMAGE_REL_BASED_MACHINE_SPECIFIC_9 kNtImageRelBasedMachineSpecific9
#define IMAGE_REL_BASED_DIR64              kNtImageRelBasedDir64

#define IMAGE_ARCHIVE_START_SIZE       kNtImageArchiveStartSize
#define IMAGE_ARCHIVE_START            kNtImageArchiveStart
#define IMAGE_ARCHIVE_END              kNtImageArchiveEnd
#define IMAGE_ARCHIVE_PAD              kNtImageArchivePad
#define IMAGE_ARCHIVE_LINKER_MEMBER    kNtImageArchiveLinkerMember
#define IMAGE_ARCHIVE_LONGNAMES_MEMBER kNtImageArchiveLongnamesMember
#define IMAGE_ARCHIVE_HYBRIDMAP_MEMBER kNtImageArchiveHybridmapMember

#define IMAGE_ORDINAL_FLAG             kNtImageOrdinalFlag
#define IMAGE_ORDINAL(Ordinal)         NtImageOrdinal(Ordinal)
#define IMAGE_SNAP_BY_ORDINAL(Ordinal) NtImageSnapByOrdinal(Ordinal)

#define IMAGE_RESOURCE_NAME_IS_STRING    kNtImageResourceNameIsString
#define IMAGE_RESOURCE_DATA_IS_DIRECTORY kNtImageResourceDataIsDirectory

#define IMAGE_DYNAMIC_RELOCATION_GUARD_RF_PROLOGUE \
  kNtImageDynamicRelocationGuardRfPrologue
#define IMAGE_DYNAMIC_RELOCATION_GUARD_RF_EPILOGUE \
  kNtImageDynamicRelocationGuardRfEpilogue

#define IMAGE_HOT_PATCH_BASE_OBLIGATORY  kNtImageHotPatchBaseObligatory
#define IMAGE_HOT_PATCH_CHUNK_INVERSE    kNtImageHotPatchChunkInverse
#define IMAGE_HOT_PATCH_CHUNK_OBLIGATORY kNtImageHotPatchChunkObligatory
#define IMAGE_HOT_PATCH_CHUNK_RESERVED   kNtImageHotPatchChunkReserved
#define IMAGE_HOT_PATCH_CHUNK_TYPE       kNtImageHotPatchChunkType
#define IMAGE_HOT_PATCH_CHUNK_SOURCE_RVA kNtImageHotPatchChunkSourceRva
#define IMAGE_HOT_PATCH_CHUNK_TARGET_RVA kNtImageHotPatchChunkTargetRva
#define IMAGE_HOT_PATCH_CHUNK_SIZE       kNtImageHotPatchChunkSize
#define IMAGE_HOT_PATCH_NONE             kNtImageHotPatchNone
#define IMAGE_HOT_PATCH_FUNCTION         kNtImageHotPatchFunction
#define IMAGE_HOT_PATCH_ABSOLUTE         kNtImageHotPatchAbsolute
#define IMAGE_HOT_PATCH_REL32            kNtImageHotPatchRel32
#define IMAGE_HOT_PATCH_CALL_TARGET      kNtImageHotPatchCallTarget
#define IMAGE_HOT_PATCH_INDIRECT         kNtImageHotPatchIndirect
#define IMAGE_HOT_PATCH_NO_CALL_TARGET   kNtImageHotPatchNoCallTarget
#define IMAGE_HOT_PATCH_DYNAMIC_VALUE    kNtImageHotPatchDynamicValue
#define IMAGE_GUARD_CF_INSTRUMENTED      kNtImageGuardCfInstrumented
#define IMAGE_GUARD_CFW_INSTRUMENTED     kNtImageGuardCfwInstrumented
#define IMAGE_GUARD_CF_FUNCTION_TABLE_PRESENT \
  kNtImageGuardCfFunctionTablePresent
#define IMAGE_GUARD_SECURITY_COOKIE_UNUSED kNtImageGuardSecurityCookieUnused
#define IMAGE_GUARD_PROTECT_DELAYLOAD_IAT  kNtImageGuardProtectDelayloadIat
#define IMAGE_GUARD_DELAYLOAD_IAT_IN_ITS_OWN_SECTION \
  kNtImageGuardDelayloadIatInItsOwnSection
#define IMAGE_GUARD_CF_EXPORT_SUPPRESSION_INFO_PRESENT \
  kNtImageGuardCfExportSuppressionInfoPresent
#define IMAGE_GUARD_CF_ENABLE_EXPORT_SUPPRESSION \
  kNtImageGuardCfEnableExportSuppression
#define IMAGE_GUARD_CF_LONGJUMP_TABLE_PRESENT \
  kNtImageGuardCfLongjumpTablePresent
#define IMAGE_GUARD_RF_INSTRUMENTED kNtImageGuardRfInstrumented
#define IMAGE_GUARD_RF_ENABLE       kNtImageGuardRfEnable
#define IMAGE_GUARD_RF_STRICT       kNtImageGuardRfStrict
#define IMAGE_GUARD_CF_FUNCTION_TABLE_SIZE_MASK \
  kNtImageGuardCfFunctionTableSizeMask
#define IMAGE_GUARD_CF_FUNCTION_TABLE_SIZE_SHIFT \
  kNtImageGuardCfFunctionTableSizeShift
#define IMAGE_GUARD_FLAG_FID_SUPPRESSED    kNtImageGuardFlagFidSuppressed
#define IMAGE_GUARD_FLAG_EXPORT_SUPPRESSED kNtImageGuardFlagExportSuppressed

#define IMAGE_ENCLAVE_IMPORT_MATCH_NONE      kNtImageEnclaveImportMatchNone
#define IMAGE_ENCLAVE_IMPORT_MATCH_UNIQUE_ID kNtImageEnclaveImportMatchUniqueId
#define IMAGE_ENCLAVE_IMPORT_MATCH_AUTHOR_ID kNtImageEnclaveImportMatchAuthorId
#define IMAGE_ENCLAVE_IMPORT_MATCH_FAMILY_ID kNtImageEnclaveImportMatchFamilyId
#define IMAGE_ENCLAVE_IMPORT_MATCH_IMAGE_ID  kNtImageEnclaveImportMatchImageId

#define IMAGE_DEBUG_TYPE_UNKNOWN       kNtImageDebugTypeUnknown
#define IMAGE_DEBUG_TYPE_COFF          kNtImageDebugTypeCoff
#define IMAGE_DEBUG_TYPE_CODEVIEW      kNtImageDebugTypeCodeview
#define IMAGE_DEBUG_TYPE_FPO           kNtImageDebugTypeFpo
#define IMAGE_DEBUG_TYPE_MISC          kNtImageDebugTypeMisc
#define IMAGE_DEBUG_TYPE_EXCEPTION     kNtImageDebugTypeException
#define IMAGE_DEBUG_TYPE_FIXUP         kNtImageDebugTypeFixup
#define IMAGE_DEBUG_TYPE_OMAP_TO_SRC   kNtImageDebugTypeOmapToSrc
#define IMAGE_DEBUG_TYPE_OMAP_FROM_SRC kNtImageDebugTypeOmapFromSrc
#define IMAGE_DEBUG_TYPE_BORLAND       kNtImageDebugTypeBorland
#define IMAGE_DEBUG_TYPE_RESERVED10    kNtImageDebugTypeReserved10
#define IMAGE_DEBUG_TYPE_CLSID         kNtImageDebugTypeClsid
#define IMAGE_DEBUG_TYPE_VC_FEATURE    kNtImageDebugTypeVcFeature
#define IMAGE_DEBUG_TYPE_POGO          kNtImageDebugTypePogo
#define IMAGE_DEBUG_TYPE_ILTCG         kNtImageDebugTypeIltcg
#define IMAGE_DEBUG_TYPE_MPX           kNtImageDebugTypeMpx
#define IMAGE_DEBUG_TYPE_REPRO         kNtImageDebugTypeRepro

#define FRAME_FPO    kNtFrameFpo
#define FRAME_TRAP   kNtFrameTrap
#define FRAME_TSS    kNtFrameTss
#define FRAME_NONFPO kNtFrameNonfpo

#define IMAGE_SIZEOF_SHORT_NAME          kNtImageSizeofShortName
#define IMAGE_SIZEOF_SECTION_HEADER      kNtImageSizeofSectionHeader
#define IMAGE_SIZEOF_SYMBOL              kNtImageSizeofSymbol
#define IMAGE_ENCLAVE_LONG_ID_LENGTH     kNtImageEnclaveLongIdLength
#define IMAGE_ENCLAVE_SHORT_ID_LENGTH    kNtImageEnclaveShortIdLength
#define IMAGE_NUMBEROF_DIRECTORY_ENTRIES kNtImageNumberofDirectoryEntries

#define FILE_BEGIN   kNtFileBegin
#define FILE_CURRENT kNtFileCurrent
#define FILE_END     kNtFileEnd

#define WSADATA   struct NtWsaData
#define LPWSADATA struct NtWsaData*

#define FD_MAX_EVENTS 10

#define CSADDR_INFO   struct NtCsAddrInfo
#define PCSADDR_INFO  struct NtCsAddrInfo*
#define LPCSADDR_INFO struct NtCsAddrInfo*

#define AFPROTOCOLS   struct NtAfProtocols
#define PAFPROTOCOLS  struct NtAfProtocols*
#define LPAFPROTOCOLS struct NtAfProtocols*

#define WSAECOMPARATOR   int
#define PWSAECOMPARATOR  int*
#define LPWSAECOMPARATOR int*

#define WSANETWORKEVENTS   struct NtWsaNetworkEvents
#define PWSANETWORKEVENTS  struct NtWsaNetworkEvents*
#define LPWSANETWORKEVENTS struct NtWsaNetworkEvents*

#define WSANSCLASSINFOW   struct NtWsansClassInfo
#define PWSANSCLASSINFOW  struct NtWsansClassInfo*
#define LPWSANSCLASSINFOW struct NtWsansClassInfo*

#define WSASERVICECLASSINFOW   struct NtWsaServiceClassInfo
#define PWSASERVICECLASSINFOW  struct NtWsaServiceClassInfo*
#define LPWSASERVICECLASSINFOW struct NtWsaServiceClassInfo*

#define WSANAMESPACE_INFOEX   struct NtWsaNamespaceInfoEx
#define PWSANAMESPACE_INFOEX  struct NtWsaNamespaceInfoEx*
#define LPWSANAMESPACE_INFOEX struct NtWsaNamespaceInfoEx*

#define WSAQUERYSET   struct NtWsaQuerySet
#define PWSAQUERYSET  struct NtWsaQuerySet*
#define LPWSAQUERYSET struct NtWsaQuerySet*

#define WSAVERSION   struct NtWsaVersion
#define PWSAVERSION  struct NtWsaVersion*
#define LPWSAVERSION struct NtWsaVersion*

#define SOCKADDR   struct sockaddr
#define PSOCKADDR  struct sockaddr*
#define LPSOCKADDR struct sockaddr*

#define SOCKET_ADDRESS   struct NtSocketAddress
#define PSOCKET_ADDRESS  struct NtSocketAddress*
#define LPSOCKET_ADDRESS struct NtSocketAddress*

#define SOCKET_ADDRESS_LIST   struct NtSocketAddressList
#define PSOCKET_ADDRESS_LIST  struct NtSocketAddressList*
#define LPSOCKET_ADDRESS_LIST struct NtSocketAddressList*

#define FLOWSPEC   struct NtFlowSpec
#define LPFLOWSPEC struct NtFlowSpec*

#define QOS   struct NtQos
#define LPQOS struct NtQos*

#define _WSAPROTOCOLCHAIN  NtWsaProtocolChain
#define WSAPROTOCOLCHAIN   struct NtWsaProtocolChain
#define LPWSAPROTOCOLCHAIN struct NtWsaProtocolChain*

#define _WSAPROTOCOL_INFO  NtWsaProtocolInfo
#define WSAPROTOCOL_INFO   struct NtWsaProtocolInfo
#define LPWSAPROTOCOL_INFO struct NtWsaProtocolInfo*

#define _WSABUF  NtIovec
#define WSABUF   struct NtIovec
#define LPWSABUF struct NtIovec*

#define _GUID  NtGuid
#define GUID   struct NtGuid
#define LPGUID struct NtGuid*

#define ADDRINFOEX   struct NtAddrInfoEx
#define LPADDRINFOEX struct NtAddrInfoEx*

#define WSAEVENT               HANDLE
#define GROUP                  uint32_t
#define WSAOVERLAPPED          OVERLAPPED
#define INVALID_SOCKET         -1ULL
#define SOCKET_ERROR           -1
#define WSA_INVALID_EVENT      -1L
#define WAIT_FAILED            -1U
#define WSA_WAIT_FAILED        -1U
#define SOCKET                 uint64_t
#define WSA_WAIT_IO_COMPLETION 0xc0
#define WSA_WAIT_TIMEOUT       258

#define LPCONDITIONPROC                    NtConditionProc
#define LPWSAOVERLAPPED_COMPLETION_ROUTINE NtWsaOverlappedCompletionRoutine

#define WSACOMPLETIONTYPE      int
#define PWSACOMPLETIONTYPE     int*
#define LPWSACOMPLETIONTYPE    int*
#define NSP_NOTIFY_IMMEDIATELY kNtNspNotifyImmediately
#define NSP_NOTIFY_HWND        kNtNspNotifyHwnd
#define NSP_NOTIFY_EVENT       kNtNspNotifyEvent
#define NSP_NOTIFY_PORT        kNtNspNotifyPort
#define NSP_NOTIFY_APC         kNtNspNotifyApc

#define WSACOMPLETION   struct NtWsaCompletion
#define PWSACOMPLETION  struct NtWsaCompletion*
#define LPWSACOMPLETION struct NtWsaCompletion*

#define WSAPOLLFD   struct pollfd
#define PWSAPOLLFD  struct pollfd*
#define LPWSAPOLLFD struct pollfd*

#define SD_RECEIVE SHUT_RD
#define SD_SEND    SHUT_WR
#define SD_BOTH    SHUT_RDWR

#define WSAMSG   struct NtMsgHdr
#define PWSAMSG  struct NtMsgHdr*
#define LPWSAMSG struct NtMsgHdr*

#define _MEMORYSTATUSEX  NtMemoryStatusEx
#define MEMORYSTATUSEX   struct NtMemoryStatusEx
#define LPMEMORYSTATUSEX struct NtMemoryStatusEx*

#define HKEY_CLASSES_ROOT                kNtHkeyClassesRoot
#define HKEY_CURRENT_USER                kNtHkeyCurrentUser
#define HKEY_LOCAL_MACHINE               kNtHkeyLocalMachine
#define HKEY_USERS                       kNtHkeyUsers
#define HKEY_PERFORMANCE_DATA            kNtHkeyPerformanceData
#define HKEY_PERFORMANCE_TEXT            kNtHkeyPerformanceText
#define HKEY_PERFORMANCE_NLSTEXT         kNtHkeyPerformanceNlstext
#define HKEY_CURRENT_CONFIG              kNtHkeyCurrentConfig
#define HKEY_DYN_DATA                    kNtHkeyDynData
#define HKEY_CURRENT_USER_LOCAL_SETTINGS kNtHkeyCurrentUserLocalSettings
#define KEY_READ                         kNtKeyRead
#define KEY_WRITE                        kNtKeyWrite
#define KEY_EXECUTE                      kNtKeyExecute
#define KEY_ALL_ACCESS                   kNtKeyAllAccess
#define REG_NONE                         kNtRegNone
#define REG_SZ                           kNtRegSz
#define REG_EXPAND_SZ                    kNtRegExpandSz
#define REG_BINARY                       kNtRegBinary
#define REG_DWORD                        kNtRegDword
#define REG_DWORD_BIG_ENDIAN             kNtRegDwordBigEndian
#define REG_LINK                         kNtRegLink
#define REG_MULTI_SZ                     kNtRegMultiSz
#define REG_RESOURCE_LIST                kNtRegResourceList
#define REG_FULL_RESOURCE_DESCRIPTOR     kNtRegFullResourceDescriptor
#define REG_RESOURCE_REQUIREMENTS_LIST   kNtRegResourceRequirementsList
#define REG_QWORD                        kNtRegQword
#define RRF_RT_REG_NONE                  kNtRrfRtRegNone
#define RRF_RT_REG_SZ                    kNtRrfRtRegSz
#define RRF_RT_REG_EXPAND_SZ             kNtRrfRtRegExpandSz
#define RRF_RT_REG_BINARY                kNtRrfRtRegBinary
#define RRF_RT_REG_DWORD                 kNtRrfRtRegDword
#define RRF_RT_REG_MULTI_SZ              kNtRrfRtRegMultiSz
#define RRF_RT_REG_QWORD                 kNtRrfRtRegQword
#define RRF_RT_DWORD                     kNtRrfRtDword
#define RRF_RT_QWORD                     kNtRrfRtQword
#define RRF_RT_ANY                       kNtRrfRtAny
#define RRF_SUBKEY_WOW6464KEY            kNtRrfSubkeyWow6464key
#define RRF_SUBKEY_WOW6432KEY            kNtRrfSubkeyWow6432key
#define RRF_WOW64_MASK                   kNtRrfWow64Mask
#define RRF_NOEXPAND                     kNtRrfNoexpand
#define RRF_ZEROONFAILURE                kNtRrfZeroonfailure

#define NUMA_NO_PREFERRED_NODE kNtNumaNoPreferredNode

#define TF_DISCONNECT         kNtTfDisconnect
#define TF_REUSE_SOCKET       kNtTfReuseSocket
#define TF_WRITE_BEHIND       kNtTfWriteBehind
#define TF_USE_DEFAULT_WORKER kNtTfUseDefaultWorker
#define TF_USE_SYSTEM_THREAD  kNtTfUseSystemThread
#define TF_USE_KERNEL_APC     kNtTfUseKernelApc

#define SIO_UDP_CONNRESET       kNtSioUdpConnreset
#define SIO_SOCKET_CLOSE_NOTIFY kNtSioSocketCloseNotify
#define SIO_UDP_NETRESET        kNtSioUdpNetreset

#define THREAD_TERMINATE                 kNtThreadTerminate
#define THREAD_SUSPEND_RESUME            kNtThreadSuspendResume
#define THREAD_GET_CONTEXT               kNtThreadGetContext
#define THREAD_SET_CONTEXT               kNtThreadSetContext
#define THREAD_QUERY_INFORMATION         kNtThreadQueryInformation
#define THREAD_SET_INFORMATION           kNtThreadSetInformation
#define THREAD_SET_THREAD_TOKEN          kNtThreadSetThreadToken
#define THREAD_IMPERSONATE               kNtThreadImpersonate
#define THREAD_DIRECT_IMPERSONATION      kNtThreadDirectImpersonation
#define THREAD_SET_LIMITED_INFORMATION   kNtThreadSetLimitedInformation
#define THREAD_QUERY_LIMITED_INFORMATION kNtThreadQueryLimitedInformation
#define THREAD_RESUME                    kNtThreadResume
#define THREAD_ALL_ACCESS                kNtThreadAllAccess

#define _FILE_SEGMENT_ELEMENT NtFileSegmentElement
#define FILE_SEGMENT_ELEMENT  union NtFileSegmentElement
#define PFILE_SEGMENT_ELEMENT union NtFileSegmentElement*

#define FileBasicInfo                  kNtFileBasicInfo
#define FileStandardInfo               kNtFileStandardInfo
#define FileNameInfo                   kNtFileNameInfo
#define FileRenameInfo                 kNtFileRenameInfo
#define FileDispositionInfo            kNtFileDispositionInfo
#define FileAllocationInfo             kNtFileAllocationInfo
#define FileEndOfFileInfo              kNtFileEndOfFileInfo
#define FileStreamInfo                 kNtFileStreamInfo
#define FileCompressionInfo            kNtFileCompressionInfo
#define FileAttributeTagInfo           kNtFileAttributeTagInfo
#define FileIdBothDirectoryInfo        kNtFileIdBothDirectoryInfo
#define FileIdBothDirectoryRestartInfo kNtFileIdBothDirectoryRestartInfo
#define FileIoPriorityHintInfo         kNtFileIoPriorityHintInfo
#define FileRemoteProtocolInfo         kNtFileRemoteProtocolInfo
#define FileFullDirectoryInfo          kNtFileFullDirectoryInfo
#define FileFullDirectoryRestartInfo   kNtFileFullDirectoryRestartInfo
#define FileStorageInfo                kNtFileStorageInfo
#define FileAlignmentInfo              kNtFileAlignmentInfo
#define FileIdInfo                     kNtFileIdInfo
#define FileIdExtdDirectoryInfo        kNtFileIdExtdDirectoryInfo
#define FileIdExtdDirectoryRestartInfo kNtFileIdExtdDirectoryRestartInfo
#define FileDispositionInfoEx          kNtFileDispositionInfoEx
#define FileRenameInfoEx               kNtFileRenameInfoEx

#define _FILE_FULL_DIR_INFO NtFileFullDirectoryInformation
#define FILE_FULL_DIR_INFO  struct NtFileFullDirectoryInformation
#define PFILE_FULL_DIR_INFO struct NtFileFullDirectoryInformation*

#define _FILE_BASIC_INFO NtFileBasicInformation
#define FILE_BASIC_INFO  struct NtFileBasicInformation
#define PFILE_BASIC_INFO struct NtFileBasicInformation*

#define _FILE_STANDARD_INFO NtFileStandardInformation
#define FILE_STANDARD_INFO  struct NtFileStandardInformation
#define PFILE_STANDARD_INFO struct NtFileStandardInformation*

#define HANDLE_FLAG_INHERIT            kNtHandleFlagInherit
#define HANDLE_FLAG_PROTECT_FROM_CLOSE kNtHandleFlagProtectFromClose

#define SYMBOLIC_LINK_FLAG_DIRECTORY kNtSymbolicLinkFlagDirectory

#define NT_FORMAT_MESSAGE_ALLOCATE_BUFFER kNtFormatMessageAllocateBuffer
#define NT_FORMAT_MESSAGE_IGNORE_INSERTS  kNtFormatMessageIgnoreInserts
#define NT_FORMAT_MESSAGE_FROM_STRING     kNtFormatMessageFromString
#define NT_FORMAT_MESSAGE_FROM_HMODULE    kNtFormatMessageFromHmodule
#define NT_FORMAT_MESSAGE_FROM_SYSTEM     kNtFormatMessageFromSystem
#define NT_FORMAT_MESSAGE_ARGUMENT_ARRAY  kNtFormatMessageArgumentArray
#define NT_FORMAT_MESSAGE_MAX_WIDTH_MASK  kNtFormatMessageMaxWidthMask

#define THREAD_BASE_PRIORITY_IDLE  kNtThreadBasePriorityIdle
#define THREAD_BASE_PRIORITY_MIN   kNtThreadBasePriorityMin
#define THREAD_BASE_PRIORITY_MAX   kNtThreadBasePriorityMax
#define THREAD_BASE_PRIORITY_LOWRT kNtThreadBasePriorityLowrt

#define THREAD_PRIORITY_IDLE          kNtThreadPriorityIdle
#define THREAD_PRIORITY_LOWEST        kNtThreadPriorityLowest
#define THREAD_PRIORITY_BELOW_NORMAL  kNtThreadPriorityBelowNormal
#define THREAD_PRIORITY_NORMAL        kNtThreadPriorityNormal
#define THREAD_PRIORITY_ABOVE_NORMAL  kNtThreadPriorityAboveNormal
#define THREAD_PRIORITY_HIGHEST       kNtThreadPriorityHighest
#define THREAD_PRIORITY_TIME_CRITICAL kNtThreadPriorityTimeCritical

#endif /* COSMOPOLITAN_LIBC_COMPAT_INCLUDE_WINDOWS_H_ */
