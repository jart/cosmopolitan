#ifndef COSMOPOLITAN_LIBC_COMPAT_INCLUDE_WINDOWS_H_
#define COSMOPOLITAN_LIBC_COMPAT_INCLUDE_WINDOWS_H_
#include "libc/nt/accounting.h"
#include "libc/nt/automation.h"
#include "libc/nt/console.h"
#include "libc/nt/debug.h"
#include "libc/nt/dll.h"
#include "libc/nt/enum/keyaccess.h"
#include "libc/nt/enum/regtype.h"
#include "libc/nt/errors.h"
#include "libc/nt/events.h"
#include "libc/nt/files.h"
#include "libc/nt/ipc.h"
#include "libc/nt/memory.h"
#include "libc/nt/nls.h"
#include "libc/nt/paint.h"
#include "libc/nt/process.h"
#include "libc/nt/registry.h"
#include "libc/nt/synchronization.h"
#include "libc/nt/thread.h"
#include "libc/nt/thunk/msabi.h"
#include "libc/nt/windows.h"
#include "libc/nt/winsock.h"
#include "libc/str/str.h"
/* clang-format off */

/* #if defined(__GNUC__) */
/* #pragma GCC diagnostic ignored "-Wint-conversion" */
/* #endif */
#undef NULL
#define NULL 0

#define WINAPI __msabi
#define STDMETHODCALLTYPE __msabi
#define CALLBACK __msabi

#define FARPROC wambda
#define NEARPROC wambda
#define PROC wambda

#define LONG int32_t /* [sic] */
#define WCHAR char16_t /* [sic] */
#define BOOL bool32 /* [sic] */

#define TRUE 1
#define FALSE 0

#define PVOID void*
#define PVOID64 void*
#define LPCVOID const void*
#define CHAR char
#define SHORT short
#define CONST const
#define VOID void
#define INT8 signed char
#define PINT8 signed char*
#define INT16 int16_t
#define PINT16 int16_t*
#define INT32 int32_t
#define PINT32 int32_t*
#define INT64 int64_t
#define PINT64 int64_t*
#define UINT8 unsigned char
#define PUINT8 unsigned char*
#define UINT16 uint16_t
#define PUINT16 uint16_t*
#define UINT32 uint32_t
#define PUINT32 uint32_t*
#define UINT64 uint64_t
#define PUINT64 uint64_t*
#define LONG32 int32_t
#define PLONG32 int32_t*
#define ULONG32 uint32_t
#define PULONG32 uint32_t*
#define DWORD32 uint32_t
#define PDWORD32 uint32_t*

#define INT_PTR intptr_t
#define PINT_PTR intptr_t*
#define UINT_PTR uintptr_t
#define PUINT_PTR uintptr_t*
#define LONG_PTR intptr_t
#define PLONG_PTR int32_t**
#define ULONG_PTR uintptr_t
#define PULONG_PTR uint32_t**
#define POINTER_64_INT int64_t*
#define __int3264 int64_t

#define SHANDLE_PTR int64_t
#define HANDLE_PTR uint64_t

#define UHALF_PTR uint32_t
#define PUHALF_PTR uint32_t*
#define HALF_PTR int32_t
#define PHALF_PTR int32_t*

#define SIZE_T size_t
#define PSIZE_T size_t*
#define SSIZE_T ssize_t
#define PSSIZE_T ssize_t*
#define DWORD_PTR ULONG_PTR
#define PDWORD_PTR ULONG_PTR*
#define LONG64 int64_t
#define PLONG64 int64_t*
#define ULONG64 uint64_t
#define PULONG64 uint64_t*
#define DWORD64 uint64_t
#define PDWORD64 uint64_t*
#define KAFFINITY ULONG_PTR
#define PKAFFINITY KAFFINITY*
#define KPRIORITY LONG

#define PWCHAR WCHAR*
#define LPWCH WCHAR*
#define PWCH WCHAR*
#define LPCWCH CONST WCHAR*
#define PCWCH CONST WCHAR*
#define NWPSTR WCHAR*
#define LPWSTR WCHAR*
#define PWSTR WCHAR*
#define PZPWSTR PWSTR*
#define PCZPWSTR CONST PWSTR*
#define LPUWSTR WCHAR forcealign(1)*
#define PUWSTR WCHAR forcealign(1)*
#define LPCWSTR CONST WCHAR*
#define PCWSTR CONST WCHAR*
#define PZPCWSTR PCWSTR*
#define LPCUWSTR CONST WCHAR forcealign(1)*
#define PCUWSTR CONST WCHAR forcealign(1)*
#define PCHAR CHAR*
#define LPCH CHAR*
#define PCH CHAR*
#define LPCCH CONST CHAR*
#define PCCH CONST CHAR*
#define NPSTR CHAR*
#define LPSTR CHAR*
#define PSTR CHAR*
#define PZPSTR PSTR*
#define PCZPSTR CONST PSTR*
#define LPCSTR CONST CHAR*
#define PCSTR CONST CHAR*
#define PZPCSTR PCSTR*
#define TCHAR WCHAR
#define PTCHAR WCHAR*
#define TBYTE WCHAR
#define PTBYTE WCHAR*
#define LPTCH LPWSTR
#define PTCH LPWSTR
#define PTSTR LPWSTR
#define LPTSTR LPWSTR
#define PCTSTR LPCWSTR
#define LPCTSTR LPCWSTR
#define PUTSTR LPUWSTR
#define LPUTSTR LPUWSTR
#define PCUTSTR LPCUWSTR
#define LPCUTSTR LPCUWSTR
#define LP LPWSTR
#define PSHORT int16_t*
#define PLONG int32_t*
#define HANDLE int64_t
#define PHANDLE HANDLE*
#define FCHAR BYTE
#define FSHORT WORD
#define FLONG DWORD
#define HRESULT LONG
#define CCHAR char
#define LCID DWORD
#define PLCID PDWORD
#define LANGID WORD
#define LONGLONG int64_t
#define ULONGLONG uint64_t
#define USN LONGLONG
#define PLONGLONG LONGLONG*
#define PULONGLONG ULONGLONG*
#define DWORDLONG ULONGLONG
#define PDWORDLONG DWORDLONG*
#define LARGE_INTEGER int64_t
#define PLARGE_INTEGER int64_t*

#define ULONG uint32_t
#define PULONG ULONG*
#define USHORT unsigned short
#define PUSHORT USHORT*
#define UCHAR unsigned char
#define PUCHAR UCHAR*
#define PSZ char*
#define DWORD uint32_t
#define WINBOOL BOOL
#define BOOLEAN BOOL
#define BYTE unsigned char
#define WORD unsigned short
#define FLOAT float
#define PFLOAT FLOAT*
#define PBOOL WINBOOL*
#define PBOOLEAN WINBOOL*
#define LPBOOL WINBOOL*
#define PBYTE BYTE*
#define LPBYTE BYTE*
#define PINT int*
#define LPINT int*
#define PWORD WORD*
#define LPWORD WORD*
#define LPLONG int32_t*
#define PDWORD DWORD*
#define LPDWORD DWORD*
#define LPVOID void*
#define LPCVOID const void*
#define INT int
#define UINT unsigned int
#define PUINT unsigned int*
#define WPARAM UINT_PTR
#define LPARAM LONG_PTR
#define LRESULT LONG_PTR
#define ATOM WORD
#define SPHANDLE HANDLE*
#define LPHANDLE HANDLE*
#define HGLOBAL HANDLE
#define HLOCAL HANDLE
#define GLOBALHANDLE HANDLE
#define LOCALHANDLE HANDLE
#define HGDIOBJ void*
#define PHKEY HKEY*
#define HMODULE HINSTANCE
#define HFILE int
#define HCURSOR HICON
#define COLORREF DWORD
#define LPCOLORREF DWORD*
#define ACCESS_MASK ULONG
#define REGSAM ACCESS_MASK
#define HKEY int64_t
#define SCODE LONG

#define NTSTATUS LONG
#define HACCEL int64_t
#define HBITMAP int64_t
#define HBRUSH int64_t
#define HCOLORSPACE int64_t
#define HDC int64_t
#define HGLRC int64_t
#define HDESK int64_t
#define HENHMETAFILE int64_t
#define HFONT int64_t
#define HICON int64_t
#define HMENU int64_t
#define HMETAFILE int64_t
#define HINSTANCE int64_t
#define HPALETTE int64_t
#define HPEN int64_t
#define HRGN int64_t
#define HRSRC int64_t
#define HSTR int64_t
#define HTASK int64_t
#define HWINSTA int64_t
#define HKL int64_t
#define HMONITOR int64_t
#define HWINEVENTHOOK int64_t
#define HUMPD int64_t
#define HWND int64_t

#define PDH_FUNCTION LONG

#define PDH_HCOUNTER HANDLE
#define PDH_HQUERY HANDLE
#define PDH_HLOG HANDLE

#define ADDRESS_FAMILY uint16_t
#define TUNNEL_TYPE uint32_t
#define NET_IF_CONNECTION_TYPE uint32_t
#define NET_IF_COMPARTMENT_ID uint32_t
#define IFTYPE uint32_t
#define NL_PREFIX_ORIGIN uint32_t
#define NL_SUFFIX_ORIGIN uint32_t
#define NL_DAD_STATE uint32_t
#define NET_IF_NETWORK_GUID struct NtGuid
#define IP_PREFIX_ORIGIN NL_PREFIX_ORIGIN
#define IP_SUFFIX_ORIGIN NL_SUFFIX_ORIGIN
#define IP_DAD_STATE NL_DAD_STATE
#define IP_ADAPTER_ADDRESSES struct NtIpAdapterAddresses
#define PIP_ADAPTER_ADDRESSES struct NtIpAdapterAddresses*
#define IP_ADAPTER_UNICAST_ADDRESS struct NtIpAdapterUnicastAddressLh
#define PIP_ADAPTER_UNICAST_ADDRESS struct NtIpAdapterUnicastAddressLh*
#define IP_ADAPTER_ANYCAST_ADDRESS struct NtIpAdapterAnycastAddressXp
#define PIP_ADAPTER_ANYCAST_ADDRESS struct NtIpAdapterAnycastAddressXp*
#define IP_ADAPTER_MULTICAST_ADDRESS struct NtIpAdapterMulticastAddressXp
#define PIP_ADAPTER_MULTICAST_ADDRESS struct NtIpAdapterMulticastAddressXp*
#define IP_ADAPTER_DNS_SERVER_ADDRESS struct NtIpAdapterDnsServerAddressXp
#define IP_ADAPTER_PREFIX struct NtIpAdapterPrefixXp
#define PIP_ADAPTER_PREFIX struct NtIpAdapterPrefixXp*

#define _GENERIC_MAPPING NtGenericMapping
#define GENERIC_MAPPING struct NtGenericMapping
#define PGENERIC_MAPPING struct NtGenericMapping*
#define _UNICODE_STRING NtUnicodeString
#define UNICODE_STRING struct NtUnicodeString
#define PUNICODE_STRING struct NtUnicodeString*
#define _IO_COUNTERS NtIoCounters
#define IO_COUNTERS struct NtIoCounters
#define PIO_COUNTERS struct NtIoCounters*
#define _FILE_TIME NtFileTime
#define FILE_TIME struct NtFileTime
#define PFILE_TIME struct NtFileTime*
#define _FILETIME NtFileTime
#define FILETIME struct NtFileTime
#define PFILETIME struct NtFileTime*
#define _CLIENT_ID NtClientId
#define CLIENT_ID struct NtClientId
#define PCLIENT_ID struct NtClientId*
#define _SYSTEM_THREADS NtSystemThreads
#define SYSTEM_THREADS struct NtSystemThreads
#define PSYSTEM_THREADS struct NtSystemThreads*
#define _VM_COUNTERS NtVmCounters
#define VM_COUNTERS struct NtVmCounters
#define PVM_COUNTERS struct NtVmCounters*
#define _SECURITY_DESCRIPTOR NtSecurityDescriptor
#define SECURITY_DESCRIPTOR struct NtSecurityDescriptor
#define PSECURITY_DESCRIPTOR struct NtSecurityDescriptor*

#define _OBJECT_ALL_INFORMATION NtObjectAllInformation
#define OBJECT_ALL_INFORMATION struct NtObjectAllinformation
#define POBJECT_ALL_INFORMATION struct NtObjectAllInformation*
#define _OBJECT_TYPE_INFORMATION NtObjectTypeInformation
#define OBJECT_TYPE_INFORMATION struct NtObjectTypeInformation
#define POBJECT_TYPE_INFORMATION struct NtObjectTypeInformation*
#define _OBJECT_NAME_INFORMATION NtObjectNameInformation
#define OBJECT_NAME_INFORMATION struct NtObjectNameInformation
#define POBJECT_NAME_INFORMATION struct NtObjectNameInformation*
#define _OBJECT_BASIC_INFORMATION NtObjectBasicInformation
#define OBJECT_BASIC_INFORMATION struct NtObjectBasicInformation
#define POBJECT_BASIC_INFORMATION struct NtObjectBasicInformation*
#define _FILE_ACCESS_INFORMATION NtFileAccessInformation
#define FILE_ACCESS_INFORMATION struct NtFileAccessInformation
#define PFILE_ACCESS_INFORMATION struct NtFileAccessInformation*
#define _FILE_ALIGNMENT_INFORMATION NtFileAlignmentInformation
#define FILE_ALIGNMENT_INFORMATION struct NtFileAlignmentInformation
#define PFILE_ALIGNMENT_INFORMATION struct NtFileAlignmentInformation*
#define _FILE_ALL_INFORMATION NtFileAllInformation
#define FILE_ALL_INFORMATION struct NtFileAllInformation
#define PFILE_ALL_INFORMATION struct NtFileAllInformation*
#define _FILE_ALLOCATION_INFORMATION NtFileAllocationInformation
#define FILE_ALLOCATION_INFORMATION struct NtFileAllocationInformation
#define PFILE_ALLOCATION_INFORMATION struct NtFileAllocationInformation*
#define _FILE_BASIC_INFORMATION NtFileBasicInformation
#define FILE_BASIC_INFORMATION struct NtFileBasicInformation
#define PFILE_BASIC_INFORMATION struct NtFileBasicInformation*
#define _FILE_BOTH_DIR_INFORMATION NtFileBothDirectoryInformation
#define FILE_BOTH_DIR_INFORMATION struct NtFileBothDirectoryInformation
#define PFILE_BOTH_DIR_INFORMATION struct NtFileBothDirectoryInformation*
#define _FILE_BOTH_DIRECTORY_INFORMATION NtFileBothDirectoryInformation
#define FILE_BOTH_DIRECTORY_INFORMATION struct NtFileBothDirectoryInformation
#define PFILE_BOTH_DIRECTORY_INFORMATION struct NtFileBothDirectoryInformation*
#define _FILE_DIRECTORY_INFORMATION NtFileDirectoryInformation
#define FILE_DIRECTORY_INFORMATION struct NtFileDirectoryInformation
#define PFILE_DIRECTORY_INFORMATION struct NtFileDirectoryInformation*
#define _FILE_DISPOSITION_INFORMATION NtFileDispositionInformation
#define FILE_DISPOSITION_INFORMATION struct NtFileDispositionInformation
#define PFILE_DISPOSITION_INFORMATION struct NtFileDispositionInformation*
#define _FILE_EA_INFORMATION NtFileEaInformation
#define FILE_EA_INFORMATION struct NtFileEaInformation
#define PFILE_EA_INFORMATION struct NtFileEaInformation*
#define _FILE_INTERNAL_INFORMATION NtFileInternalInformation
#define FILE_INTERNAL_INFORMATION struct NtFileInternalInformation
#define PFILE_INTERNAL_INFORMATION struct NtFileInternalInformation*
#define _FILE_MODE_INFORMATION NtFileModeInformation
#define FILE_MODE_INFORMATION struct NtFileModeInformation
#define PFILE_MODE_INFORMATION struct NtFileModeInformation*
#define _FILE_NAME_INFORMATION NtFileNameInformation
#define FILE_NAME_INFORMATION struct NtFileNameInformation
#define PFILE_NAME_INFORMATION struct NtFileNameInformation*
#define _FILE_NAMES_INFORMATION NtFileNamesInformation
#define FILE_NAMES_INFORMATION struct NtFileNamesInformation
#define PFILE_NAMES_INFORMATION struct NtFileNamesInformation*
#define _FILE_POSITION_INFORMATION NtFilePositionInformation
#define FILE_POSITION_INFORMATION struct NtFilePositionInformation
#define PFILE_POSITION_INFORMATION struct NtFilePositionInformation*
#define _FILE_RENAME_INFORMATION NtFileRenameInformation
#define FILE_RENAME_INFORMATION struct NtFileRenameInformation
#define PFILE_RENAME_INFORMATION struct NtFileRenameInformation*
#define _FILE_STANDARD_INFORMATION NtFileStandardInformation
#define FILE_STANDARD_INFORMATION struct NtFileStandardInformation
#define PFILE_STANDARD_INFORMATION struct NtFileStandardInformation*
#define _FILE_STREAM_INFORMATION NtFileStreamInformation
#define FILE_STREAM_INFORMATION struct NtFileStreamInformation
#define PFILE_STREAM_INFORMATION struct NtFileStreamInformation*
#define _FILE_NOTIFY_INFORMATION NtFileNotifyInformation
#define FILE_NOTIFY_INFORMATION struct NtFileNotifyInformation
#define PFILE_NOTIFY_INFORMATION struct NtFileNotifyInformation*
#define _KERNEL_USER_TIMES NtKernelUserTimes
#define KERNEL_USER_TIMES struct NtKernelUserTimes
#define PKERNEL_USER_TIMES struct NtKernelUserTimes*
#define _PROCESS_BASIC_INFORMATION NtProcessBasicInformation
#define PROCESS_BASIC_INFORMATION struct NtProcessBasicInformation
#define PPROCESS_BASIC_INFORMATION struct NtProcessBasicInformation*
#define _SYSTEM_BASIC_INFORMATION NtSystemBasicInformation
#define SYSTEM_BASIC_INFORMATION struct NtSystemBasicInformation
#define PSYSTEM_BASIC_INFORMATION struct NtSystemBasicInformation*
#define _SYSTEM_EXCEPTION_INFORMATION NtSystemExceptionInformation
#define SYSTEM_EXCEPTION_INFORMATION struct NtSystemExceptionInformation
#define PSYSTEM_EXCEPTION_INFORMATION struct NtSystemExceptionInformation*
#define _SYSTEM_HANDLE_ENTRY NtSystemHandleEntry
#define SYSTEM_HANDLE_ENTRY struct NtSystemHandleEntry
#define PSYSTEM_HANDLE_ENTRY struct NtSystemHandleEntry*
#define _SYSTEM_HANDLE_INFORMATION NtSystemHandleInformation
#define SYSTEM_HANDLE_INFORMATION struct NtSystemHandleInformation
#define PSYSTEM_HANDLE_INFORMATION struct NtSystemHandleInformation*
#define _SYSTEM_INTERRUPT_INFORMATION NtSystemInterruptInformation
#define SYSTEM_INTERRUPT_INFORMATION struct NtSystemInterruptInformation
#define PSYSTEM_INTERRUPT_INFORMATION struct NtSystemInterruptInformation*
#define _SYSTEM_LOOKASIDE_INFORMATION NtSystemLookasideInformation
#define SYSTEM_LOOKASIDE_INFORMATION struct NtSystemLookasideInformation
#define PSYSTEM_LOOKASIDE_INFORMATION struct NtSystemLookasideInformation*
#define _SYSTEM_PERFORMANCE_INFORMATION NtSystemPerformanceInformation
#define SYSTEM_PERFORMANCE_INFORMATION struct NtSystemPerformanceInformation
#define PSYSTEM_PERFORMANCE_INFORMATION struct NtSystemPerformanceInformation*
#define _SYSTEM_PROCESS_INFORMATION NtSystemProcessInformation
#define SYSTEM_PROCESS_INFORMATION struct NtSystemProcessInformation
#define PSYSTEM_PROCESS_INFORMATION struct NtSystemProcessInformation*
#define _SYSTEM_PROCESSOR_INFORMATION NtSystemProcessorInformation
#define SYSTEM_PROCESSOR_INFORMATION struct NtSystemProcessorInformation
#define PSYSTEM_PROCESSOR_INFORMATION struct NtSystemProcessorInformation*
#define _SYSTEM_TIMEOFDAY_INFORMATION NtSystemTimeofdayInformation
#define SYSTEM_TIMEOFDAY_INFORMATION struct NtSystemTimeofdayInformation
#define PSYSTEM_TIMEOFDAY_INFORMATION struct NtSystemTimeofdayInformation*

#define _SYSTEM_REGISTRY_QUOTA_INFORMATION NtSystemRegistryQuotaInformation
#define SYSTEM_REGISTRY_QUOTA_INFORMATION struct NtSystemRegistryQuotaInformation
#define PSYSTEM_REGISTRY_QUOTA_INFORMATION struct NtSystemRegistryQuotaInformation*
#define _SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION NtSystemProcessorPerformanceInformation
#define SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION struct NtSystemProcessorPerformanceInformation
#define PSYSTEM_PROCESSOR_PERFORMANCE_INFORMATION struct NtSystemProcessorPerformanceInformation*
#define _FILE_FULL_DIR_INFORMATION NtFileFullDirectoryInformation
#define FILE_FULL_DIR_INFORMATION struct NtFileFullDirectoryInformation
#define PFILE_FULL_DIR_INFORMATION struct NtFileFullDirectoryInformation*
#define _FILE_FULL_DIRECTORY_INFORMATION NtFileFullDirectoryInformation
#define FILE_FULL_DIRECTORY_INFORMATION struct NtFileFullDirectoryInformation
#define PFILE_FULL_DIRECTORY_INFORMATION struct NtFileFullDirectoryInformation*
#define _FILE_ATTRIBUTE_TAG_INFORMATION NtFileAttributeTagInformation
#define FILE_ATTRIBUTE_TAG_INFORMATION struct NtFileAttributeTagInformation
#define PFILE_ATTRIBUTE_TAG_INFORMATION struct NtFileAttributeTagInformation*
#define _FILE_PIPE_LOCAL_INFORMATION NtFilePipeLocalInformation
#define FILE_PIPE_LOCAL_INFORMATION struct NtFilePipeLocalInformation
#define PFILE_PIPE_LOCAL_INFORMATION struct NtFilePipeLocalInformation*
#define _FILE_NETWORK_OPEN_INFORMATION NtFileNetworkOpenInformation
#define FILE_NETWORK_OPEN_INFORMATION struct NtFileNetworkOpenInformation
#define PFILE_NETWORK_OPEN_INFORMATION struct NtFileNetworkOpenInformation*
#define _FILE_MAILSLOT_QUERY_INFORMATION NtFileMailslotQueryInformation
#define FILE_MAILSLOT_QUERY_INFORMATION struct NtFileMailslotQueryInformation
#define PFILE_MAILSLOT_QUERY_INFORMATION struct NtFileMailslotQueryInformation*
#define _FILE_MAILSLOT_SET_INFORMATION NtFileMailslotSetInformation
#define FILE_MAILSLOT_SET_INFORMATION struct NtFileMailslotSetInformation
#define PFILE_MAILSLOT_SET_INFORMATION struct NtFileMailslotSetInformation*
#define _FILE_FULL_EA_INFORMATION NtFileFullEaInformation
#define FILE_FULL_EA_INFORMATION struct NtFileFullEaInformation
#define PFILE_FULL_EA_INFORMATION struct NtFileFullEaInformation*
#define _PDH_FMT_COUNTERVALUE NtPdhFmtCountervalue
#define PDH_FMT_COUNTERVALUE struct NtPdhFmtCountervalue
#define PPDH_FMT_COUNTERVALUE struct NtPdhFmtCountervalue*

#define _LUID NtLuid
#define LUID struct NtLuid
#define PLUID struct NtLuid*

#define _LUID_AND_ATTRIBUTES NtLuidAndAttributes
#define LUID_AND_ATTRIBUTES struct NtLuidAndAttributes
#define PLUID_AND_ATTRIBUTES struct NtLuidAndAttributes*

#define _PRIVILEGE_SET NtPrivilegeSet
#define PRIVILEGE_SET struct NtPrivilegeSet
#define PPRIVILEGE_SET struct NtPrivilegeSet*

#define _IMAGE_FILE_HEADER NtImageFileHeader
#define IMAGE_FILE_HEADER struct NtImageFileHeader
#define PIMAGE_FILE_HEADER struct NtImageFileHeader*

#define _IMAGE_DOS_HEADER NtImageDosHeader
#define IMAGE_DOS_HEADER struct NtImageDosHeader
#define PIMAGE_DOS_HEADER struct NtImageDosHeader*

#define _BY_HANDLE_FILE_INFORMATION NtByHandleFileInformation
#define BY_HANDLE_FILE_INFORMATION struct NtByHandleFileInformation
#define PBY_HANDLE_FILE_INFORMATION struct NtByHandleFileInformation*
#define LPBY_HANDLE_FILE_INFORMATION struct NtByHandleFileInformation*

#define _WIN32_FILE_ATTRIBUTE_DATA NtWin32FileAttributeData
#define WIN32_FILE_ATTRIBUTE_DATA struct NtWin32FileAttributeData
#define LPWIN32_FILE_ATTRIBUTE_DATA struct NtWin32FileAttributeData*

#define _FILE_END_OF_FILE_INFORMATION FileEndOfFileInformation
#define FILE_END_OF_FILE_INFORMATION struct FileEndOfFileInformation
#define PFILE_END_OF_FILE_INFORMATION struct FileEndOfFileInformation*

#define _GET_FILEEX_INFO_LEVELS NtGetFileexInfoLevels
#define GET_FILEEX_INFO_LEVELS int
#define LPGET_FILEEX_INFO_LEVELS int*

#define _WIN32_FIND_DATA NtWin32FindData
#define WIN32_FIND_DATA struct NtWin32FindData
#define LPWIN32_FIND_DATA struct NtWin32FindData*

#define _FINDEX_INFO_LEVELS NtFindexInfoLevels
#define FINDEX_INFO_LEVELS int
#define FindExInfoStandard kNtFindExInfoStandard
#define FindExInfoBasic kNtFindExInfoBasic
#define FindExInfoMaxInfoLevel kNtFindExInfoMaxInfoLevel
#define FIND_FIRST_EX_CASE_SENSITIVE kNtFindFirstExCaseSensitive
#define FIND_FIRST_EX_LARGE_FETCH kNtFindFirstExLargeFetch
#define _FINDEX_SEARCH_OPS NtFindexSearchOps
#define FINDEX_SEARCH_OPS int
#define FindExSearchNameMatch kNtFindExSearchNameMatch
#define FindExSearchLimitToDirectories kNtFindExSearchLimitToDirectories
#define FindExSearchLimitToDevices kNtFindExSearchLimitToDevices
#define FindExSearchMaxSearchOp kNtFindExSearchMaxSearchOp

#define GetFileExInfoStandard kNtGetFileExInfoStandard
#define GetFileExMaxInfoLevel kNtGetFile_MAX

#define MOVEFILE_REPLACE_EXISTING kNtMovefileReplaceExisting
#define MOVEFILE_COPY_ALLOWED kNtMovefileCopyAllowed
#define MOVEFILE_DELAY_UNTIL_REBOOT kNtMovefileDelayUntilReboot
#define MOVEFILE_CREATE_HARDLINK kNtMovefileCreateHardlink
#define MOVEFILE_FAIL_IF_NOT_TRACKABLE kNtMovefileFailIfNotTrackable
#define MOVEFILE_WRITE_THROUGH kNtMovefileWriteThrough

#define OFFER_PRIORITY int
#define VmOfferPriorityVeryLow kNtVmOfferPriorityVeryLow
#define VmOfferPriorityLow kNtVmOfferPriorityLow
#define VmOfferPriorityBelowNormal kNtVmOfferPriorityBelowNormal
#define VmOfferPriorityNormal kNtVmOfferPriorityNormal

#define _KWAIT_REASON uint32_t
#define KWAIT_REASON uint32_t
#define _OBJECT_INFORMATION_CLASS int
#define OBJECT_INFORMATION_CLASS int
#define _PROCESSINFOCLASS int
#define PROCESSINFOCLASS int
#define _THREAD_STATE int
#define THREAD_STATE int
#define _TOKEN_TYPE int
#define TOKEN_TYPE int

#define _THREADINFOCLASS int
#define THREADINFOCLASS int

#define _THREAD_INFORMATION_CLASS int
#define THREAD_INFORMATION_CLASS int
#define PTHREAD_INFORMATION_CLASS int*

#define OWNER_SECURITY_INFORMATION kNtOwnerSecurityInformation
#define GROUP_SECURITY_INFORMATION kNtGroupSecurityInformation
#define DACL_SECURITY_INFORMATION kNtDaclSecurityInformation
#define SACL_SECURITY_INFORMATION kNtSaclSecurityInformation
#define LABEL_SECURITY_INFORMATION kNtLabelSecurityInformation
#define ATTRIBUTE_SECURITY_INFORMATION kNtAttributeSecurityInformation
#define SCOPE_SECURITY_INFORMATION kNtScopeSecurityInformation
#define PROCESS_TRUST_LABEL_SECURITY_INFORMATION kNtProcessTrustLabelSecurityInformation
#define ACCESS_FILTER_SECURITY_INFORMATION kNtAccessFilterSecurityInformation
#define BACKUP_SECURITY_INFORMATION kNtBackupSecurityInformation
#define PROTECTED_DACL_SECURITY_INFORMATION kNtProtectedDaclSecurityInformation
#define PROTECTED_SACL_SECURITY_INFORMATION kNtProtectedSaclSecurityInformation
#define UNPROTECTED_DACL_SECURITY_INFORMATION kNtUnprotectedDaclSecurityInformation
#define UNPROTECTED_SACL_SECURITY_INFORMATION kNtUnprotectedSaclSecurityInformation

#include "libc/nt/enum/startf.h"
#define STARTF_USESHOWWINDOW kNtStartfUseshowwindow
#define STARTF_USESIZE kNtStartfUsesize
#define STARTF_USEPOSITION kNtStartfUseposition
#define STARTF_USECOUNTCHARS kNtStartfUsecountchars
#define STARTF_USEFILLATTRIBUTE kNtStartfUsefillattribute
#define STARTF_RUNFULLSCREEN kNtStartfRunfullscreen
#define STARTF_FORCEONFEEDBACK kNtStartfForceonfeedback
#define STARTF_FORCEOFFFEEDBACK kNtStartfForceofffeedback
#define STARTF_USESTDHANDLES kNtStartfUsestdhandles
#define STARTF_USEHOTKEY kNtStartfUsehotkey
#define STARTF_TITLEISLINKNAME kNtStartfTitleislinkname
#define STARTF_TITLEISAPPID kNtStartfTitleisappid
#define STARTF_PREVENTPINNING kNtStartfPreventpinning
#define STARTF_UNTRUSTEDSOURCE kNtStartfUntrustedsource

#include "libc/nt/enum/memflags.h"
#define MEM_COMMIT kNtMemCommit
#define MEM_RESERVE kNtMemReserve
#define MEM_DECOMMIT kNtMemDecommit
#define MEM_RELEASE kNtMemRelease
#define MEM_FREE kNtMemFree
#define MEM_PRIVATE kNtMemPrivate
#define MEM_MAPPED kNtMemMapped
#define MEM_RESET kNtMemReset
#define MEM_TOP_DOWN kNtMemTopDown
#define MEM_WRITE_WATCH kNtMemWriteWatch
#define MEM_PHYSICAL kNtMemPhysical
#define MEM_LARGE_PAGES kNtMemLargePages
#define MEM_4MB_PAGES kNtMem4mbPages

#include "libc/nt/enum/pageflags.h"
#define PAGE_NOACCESS kNtPageNoaccess
#define PAGE_READONLY kNtPageReadonly
#define PAGE_READWRITE kNtPageReadwrite
#define PAGE_WRITECOPY kNtPageWritecopy
#define PAGE_EXECUTE kNtPageExecute
#define PAGE_EXECUTE_READ kNtPageExecuteRead
#define PAGE_EXECUTE_READWRITE kNtPageExecuteReadwrite
#define PAGE_EXECUTE_WRITECOPY kNtPageExecuteWritecopy
#define PAGE_GUARD kNtPageGuard
#define PAGE_NOCACHE kNtPageNocache
#define PAGE_WRITECOMBINE kNtPageWritecombine
#define FILE_MAP_COPY kNtFileMapCopy
#define FILE_MAP_WRITE kNtFileMapWrite
#define FILE_MAP_READ kNtFileMapRead
#define FILE_MAP_EXECUTE kNtFileMapExecute
#define FILE_MAP_RESERVE kNtFileMapReserve
#define FILE_MAP_TARGETS_INVALID kNtFileMapTargetsInvalid
#define FILE_MAP_LARGE_PAGES kNtFileMapLargePages

#define SECTION_QUERY kNtSectionQuery
#define SECTION_MAP_WRITE kNtSectionMapWrite
#define SECTION_MAP_READ kNtSectionMapRead
#define SECTION_MAP_EXECUTE kNtSectionMapExecute
#define SECTION_EXTEND_SIZE kNtSectionExtendSize
#define SECTION_MAP_EXECUTE_EXPLICIT kNtSectionMapExecuteExplicit

#include "libc/nt/enum/ctrlevent.h"
#define CTRL_CEVENT kNtCtrlCEvent
#define CTRL_BREAK_EVENT kNtCtrlBreakEvent
#define CTRL_CLOSE_EVENT kNtCtrlCloseEvent
#define CTRL_LOGOFF_EVENT kNtCtrlLogoffEvent
#define CTRL_SHUTDOWN_EVENT kNtCtrlShutdownEvent

#include "libc/nt/enum/fileflagandattributes.h"
#define FILE_ATTRIBUTE_NORMAL kNtFileAttributeNormal
#define FILE_ATTRIBUTE_HIDDEN kNtFileAttributeHidden
#define FILE_FLAG_WRITE_THROUGH kNtFileFlagWriteThrough
#define FILE_FLAG_OVERLAPPED kNtFileFlagOverlapped
#define FILE_FLAG_NO_BUFFERING kNtFileFlagNoBuffering
#define FILE_FLAG_RANDOM_ACCESS kNtFileFlagRandomAccess
#define FILE_FLAG_SEQUENTIAL_SCAN kNtFileFlagSequentialScan
#define FILE_FLAG_DELETE_ON_CLOSE kNtFileFlagDeleteOnClose
#define FILE_FLAG_BACKUP_SEMANTICS kNtFileFlagBackupSemantics
#define FILE_FLAG_POSIX_SEMANTICS kNtFileFlagPosixSemantics
#define FILE_FLAG_OPEN_REPARSE_POINT kNtFileFlagOpenReparsePoint
#define FILE_FLAG_OPEN_NO_RECALL kNtFileFlagOpenNoRecall
#define FILE_FLAG_FIRST_PIPE_INSTANCE kNtFileFlagFirstPipeInstance
#define FILE_LIST_DIRECTORY kNtFileListDirectory
#define FILE_ATTRIBUTE_ARCHIVE kNtFileAttributeArchive
#define FILE_ATTRIBUTE_COMPRESSED kNtFileAttributeCompressed
#define FILE_ATTRIBUTE_DEVICE kNtFileAttributeDevice
#define FILE_ATTRIBUTE_DIRECTORY kNtFileAttributeDirectory
#define FILE_ATTRIBUTE_ENCRYPTED kNtFileAttributeEncrypted
#define FILE_ATTRIBUTE_NOT_CONTENT_INDEXED kNtFileAttributeNotContentIndexed
#define FILE_ATTRIBUTE_OFFLINE kNtFileAttributeOffline
#define FILE_ATTRIBUTE_READONLY kNtFileAttributeReadonly
#define FILE_ATTRIBUTE_REPARSE_POINT kNtFileAttributeReparsePoint
#define FILE_ATTRIBUTE_SPARSE_FILE kNtFileAttributeSparseFile
#define FILE_ATTRIBUTE_SYSTEM kNtFileAttributeSystem
#define FILE_ATTRIBUTE_TEMPORARY kNtFileAttributeTemporary

#include "libc/nt/enum/creationdisposition.h"
#define CREATE_NEW kNtCreateNew
#define CREATE_ALWAYS kNtCreateAlways
#define OPEN_EXISTING kNtOpenExisting
#define OPEN_ALWAYS kNtOpenAlways
#define TRUNCATE_EXISTING kNtTruncateExisting

#include "libc/nt/enum/filesharemode.h"
#define FILE_SHARE_EXCLUSIVE kNtFileShareExclusive
#define FILE_SHARE_READ kNtFileShareRead
#define FILE_SHARE_WRITE kNtFileShareWrite
#define FILE_SHARE_DELETE kNtFileShareDelete

#include "libc/nt/enum/filenotify.h"
#define FILE_NOTIFY_CHANGE_FILE_NAME kNtFileNotifyChangeFileName
#define FILE_NOTIFY_CHANGE_DIR_NAME kNtFileNotifyChangeDirName
#define FILE_NOTIFY_CHANGE_ATTRIBUTES kNtFileNotifyChangeAttributes
#define FILE_NOTIFY_CHANGE_SIZE kNtFileNotifyChangeSize
#define FILE_NOTIFY_CHANGE_LAST_WRITE kNtFileNotifyChangeLastWrite
#define FILE_NOTIFY_CHANGE_LAST_ACCESS kNtFileNotifyChangeLastAccess
#define FILE_NOTIFY_CHANGE_CREATION kNtFileNotifyChangeCreation
#define FILE_NOTIFY_CHANGE_SECURITY kNtFileNotifyChangeSecurity

#include "libc/nt/runtime.h"
#define INVALID_HANDLE_VALUE kNtInvalidHandleValue
#define STD_INPUT_HANDLE kNtStdInputHandle
#define STD_OUTPUT_HANDLE kNtStdOutputHandle
#define STD_ERROR_HANDLE kNtStdErrorHandle

#include "libc/nt/enum/consoleselectionflags.h"
#define CONSOLE_NO_SELECTION kNtConsoleNoSelection
#define CONSOLE_SELECTION_IN_PROGRESS kNtConsoleSelectionInProgress
#define CONSOLE_SELECTION_NOT_EMPTY kNtConsoleSelectionNotEmpty
#define CONSOLE_MOUSE_SELECTION kNtConsoleMouseSelection
#define CONSOLE_MOUSE_DOWN kNtConsoleMouseDown

#include "libc/nt/enum/mb.h"
#define MB_OK kNtMbOk
#define MB_OKCANCEL kNtMbOkcancel
#define MB_ABORTRETRYIGNORE kNtMbAbortretryignore
#define MB_YESNOCANCEL kNtMbYesnocancel
#define MB_YESNO kNtMbYesno
#define MB_RETRYCANCEL kNtMbRetrycancel
#define MB_CANCELTRYCONTINUE kNtMbCanceltrycontinue
#define MB_ICONHAND kNtMbIconhand
#define MB_ICONQUESTION kNtMbIconquestion
#define MB_ICONEXCLAMATION kNtMbIconexclamation
#define MB_ICONASTERISK kNtMbIconasterisk
#define MB_USERICON kNtMbUsericon
#define MB_ICONWARNING kNtMbIconwarning
#define MB_ICONERROR kNtMbIconerror
#define MB_ICONINFORMATION kNtMbIconinformation
#define MB_ICONSTOP kNtMbIconstop
#define MB_DEFBUTTON1 kNtMbDefbutton1
#define MB_DEFBUTTON2 kNtMbDefbutton2
#define MB_DEFBUTTON3 kNtMbDefbutton3
#define MB_DEFBUTTON4 kNtMbDefbutton4
#define MB_APPLMODAL kNtMbApplmodal
#define MB_SYSTEMMODAL kNtMbSystemmodal
#define MB_TASKMODAL kNtMbTaskmodal
#define MB_HELP kNtMbHelp
#define MB_NOFOCUS kNtMbNofocus
#define MB_SETFOREGROUND kNtMbSetforeground
#define MB_DEFAULT_DESKTOP_ONLY kNtMbDefaultDesktopOnly
#define MB_TOPMOST kNtMbTopmost
#define MB_RIGHT kNtMbRight
#define MB_RTLREADING kNtMbRtlreading
#define MB_SERVICE_NOTIFICATION kNtMbServiceNotification
#define MB_SERVICE_NOTIFICATION_NT3X kNtMbServiceNotificationNt3x
#define MB_TYPEMASK kNtMbTypemask
#define MB_ICONMASK kNtMbIconmask
#define MB_DEFMASK kNtMbDefmask
#define MB_MODEMASK kNtMbModemask
#define MB_MISCMASK kNtMbMiscmask

#include "libc/nt/enum/dialogresult.h"
#define IDOK kNtIdok
#define IDCANCEL kNtIdcancel
#define IDABORT kNtIdabort
#define IDRETRY kNtIdretry
#define IDIGNORE kNtIdignore
#define IDYES kNtIdyes
#define IDNO kNtIdno
#define IDCLOSE kNtIdclose
#define IDHELP kNtIdhelp
#define IDTRYAGAIN kNtIdtryagain
#define IDCONTINUE kNtIdcontinue

#include "libc/nt/enum/processaccess.h"
#define PROCESS_TERMINATE kNtProcessTerminate
#define PROCESS_CREATE_THREAD kNtProcessCreateThread
#define PROCESS_SET_SESSIONID kNtProcessSetSessionid
#define PROCESS_VM_OPERATION kNtProcessVmOperation
#define PROCESS_VM_READ kNtProcessVmRead
#define PROCESS_VM_WRITE kNtProcessVmWrite
#define PROCESS_DUP_HANDLE kNtProcessDupHandle
#define PROCESS_CREATE_PROCESS kNtProcessCreateProcess
#define PROCESS_SET_QUOTA kNtProcessSetQuota
#define PROCESS_SET_INFORMATION kNtProcessSetInformation
#define PROCESS_QUERY_INFORMATION kNtProcessQueryInformation
#define PROCESS_SUSPEND_RESUME kNtProcessSuspendResume
#define PROCESS_QUERY_LIMITED_INFORMATION kNtProcessQueryLimitedInformation
#define PROCESS_SET_LIMITED_INFORMATION kNtProcessSetLimitedInformation
#define PROCESS_ALL_ACCESS kNtProcessAllAccess

#include "libc/nt/enum/accessmask.h"
#define GENERIC_READ kNtGenericRead
#define GENERIC_WRITE kNtGenericWrite
#define GENERIC_EXECUTE kNtGenericExecute
#define GENERIC_ALL kNtGenericAll
#define DELETE kNtDelete
#define READ_CONTROL kNtReadControl
#define WRITE_DAC kNtWriteDac
#define WRITE_OWNER kNtWriteOwner
#define SYNCHRONIZE kNtSynchronize
#define STANDARD_RIGHTS_REQUIRED kNtStandardRightsRequired
#define STANDARD_RIGHTS_READ kNtStandardRightsRead
#define STANDARD_RIGHTS_WRITE kNtStandardRightsWrite
#define STANDARD_RIGHTS_EXECUTE kNtStandardRightsExecute
#define STANDARD_RIGHTS_ALL kNtStandardRightsAll
#define SPECIFIC_RIGHTS_ALL kNtSpecificRightsAll
#define ACCESS_SYSTEM_SECURITY kNtAccessSystemSecurity
#define MAXIMUM_ALLOWED kNtMaximumAllowed
#define FILE_READ_DATA kNtFileReadData
#define FILE_LIST_DIRECTORY kNtFileListDirectory
#define FILE_WRITE_DATA kNtFileWriteData
#define FILE_ADD_FILE kNtFileAddFile
#define FILE_APPEND_DATA kNtFileAppendData
#define FILE_ADD_SUBDIRECTORY kNtFileAddSubdirectory
#define FILE_CREATE_PIPE_INSTANCE kNtFileCreatePipeInstance
#define FILE_READ_EA kNtFileReadEa
#define FILE_WRITE_EA kNtFileWriteEa
#define FILE_EXECUTE kNtFileExecute
#define FILE_TRAVERSE kNtFileTraverse
#define FILE_DELETE_CHILD kNtFileDeleteChild
#define FILE_READ_ATTRIBUTES kNtFileReadAttributes
#define FILE_WRITE_ATTRIBUTES kNtFileWriteAttributes
#define FILE_ALL_ACCESS kNtFileAllAccess
#define FILE_GENERIC_READ kNtFileGenericRead
#define FILE_GENERIC_WRITE kNtFileGenericWrite
#define FILE_GENERIC_EXECUTE kNtFileGenericExecute
#define TOKEN_ASSIGN_PRIMARY kNtTokenAssignPrimary
#define TOKEN_DUPLICATE kNtTokenDuplicate
#define TOKEN_IMPERSONATE kNtTokenImpersonate
#define TOKEN_QUERY kNtTokenQuery
#define TOKEN_QUERY_SOURCE kNtTokenQuerySource
#define TOKEN_ADJUST_PRIVILEGES kNtTokenAdjustPrivileges
#define TOKEN_ADJUST_GROUPS kNtTokenAdjustGroups
#define TOKEN_ADJUST_DEFAULT kNtTokenAdjustDefault
#define TOKEN_ADJUST_SESSIONID kNtTokenAdjustSessionid
#define TOKEN_ALL_ACCESS_P kNtTokenAllAccessP
#define TOKEN_ALL_ACCESS kNtTokenAllAccess
#define TOKEN_READ kNtTokenRead
#define TOKEN_WRITE kNtTokenWrite
#define TOKEN_EXECUTE kNtTokenExecute
#define TOKEN_TRUST_CONSTRAINT_MASK kNtTokenTrustConstraintMask
#define TOKEN_ACCESS_PSEUDO_HANDLE_WIN8 kNtTokenAccessPseudoHandleWin8
#define TOKEN_ACCESS_PSEUDO_HANDLE kNtTokenAccessPseudoHandle

#define FILE_TYPE_UNKNOWN kNtFileTypeUnknown
#define FILE_TYPE_DISK kNtFileTypeDisk
#define FILE_TYPE_CHAR kNtFileTypeChar
#define FILE_TYPE_PIPE kNtFileTypePipe
#define FILE_TYPE_REMOTE kNtFileTypeRemote

#define NT_DEBUG_PROCESS kNtDebugProcess
#define NT_DEBUG_ONLY_THIS_PROCESS kNtDebugOnlyThisProcess
#define NT_CREATE_SUSPENDED kNtCreateSuspended
#define NT_DETACHED_PROCESS kNtDetachedProcess
#define NT_CREATE_NEW_CONSOLE kNtCreateNewConsole
#define NT_NORMAL_PRIORITY_CLASS kNtNormalPriorityClass
#define NT_IDLE_PRIORITY_CLASS kNtIdlePriorityClass
#define NT_HIGH_PRIORITY_CLASS kNtHighPriorityClass
#define NT_REALTIME_PRIORITY_CLASS kNtRealtimePriorityClass
#define NT_CREATE_NEW_PROCESS_GROUP kNtCreateNewProcessGroup
#define NT_CREATE_UNICODE_ENVIRONMENT kNtCreateUnicodeEnvironment
#define NT_CREATE_SEPARATE_WOW_VDM kNtCreateSeparateWowVdm
#define NT_CREATE_SHARED_WOW_VDM kNtCreateSharedWowVdm
#define NT_CREATE_FORCEDOS kNtCreateForcedos
#define NT_BELOW_NORMAL_PRIORITY_CLASS kNtBelowNormalPriorityClass
#define NT_ABOVE_NORMAL_PRIORITY_CLASS kNtAboveNormalPriorityClass
#define NT_INHERIT_PARENT_AFFINITY kNtInheritParentAffinity
#define NT_CREATE_PROTECTED_PROCESS kNtCreateProtectedProcess
#define NT_EXTENDED_STARTUPINFO_PRESENT kNtExtendedStartupinfoPresent
#define NT_PROCESS_MODE_BACKGROUND_BEGIN kNtProcessModeBackgroundBegin
#define NT_PROCESS_MODE_BACKGROUND_END kNtProcessModeBackgroundEnd
#define NT_CREATE_SECURE_PROCESS kNtCreateSecureProcess
#define NT_CREATE_BREAKAWAY_FROM_JOB kNtCreateBreakawayFromJob
#define NT_CREATE_PRESERVE_CODE_AUTHZ_LEVEL kNtCreatePreserveCodeAuthzLevel
#define NT_CREATE_DEFAULT_ERROR_MODE kNtCreateDefaultErrorMode
#define NT_CREATE_NO_WINDOW kNtCreateNoWindow
#define NT_PROFILE_USER kNtProfileUser
#define NT_PROFILE_KERNEL kNtProfileKernel
#define NT_PROFILE_SERVER kNtProfileServer
#define NT_CREATE_IGNORE_SYSTEM_DEFAULT kNtCreateIgnoreSystemDefault

#define FILE_READ_DATA kNtFileReadData
#define FILE_WRITE_DATA kNtFileWriteData
#define FILE_ADD_FILE kNtFileAddFile
#define FILE_APPEND_DATA kNtFileAppendData
#define FILE_ADD_SUBDIRECTORY kNtFileAddSubdirectory
#define FILE_CREATE_PIPE_INSTANCE kNtFileCreatePipeInstance
#define FILE_READ_EA kNtFileReadEa
#define FILE_WRITE_EA kNtFileWriteEa
#define FILE_EXECUTE kNtFileExecute
#define FILE_TRAVERSE kNtFileTraverse
#define FILE_DELETE_CHILD kNtFileDeleteChild
#define FILE_READ_ATTRIBUTES kNtFileReadAttributes
#define FILE_WRITE_ATTRIBUTES kNtFileWriteAttributes
#define FILE_ALL_ACCESS kNtFileAllAccess
#define FILE_GENERIC_READ kNtFileGenericRead
#define FILE_GENERIC_WRITE kNtFileGenericWrite
#define FILE_GENERIC_EXECUTE kNtFileGenericExecute

#define TOKEN_PRIMARY kNtTokenPrimary
#define TOKEN_IMPERSONATION kNtTokenImpersonation

#define TOKEN_PRIMARY kNtTokenPrimary
#define TOKEN_IMPERSONATION kNtTokenImpersonation
#define SECURITY_ANONYMOUS kNtSecurityAnonymous
#define SECURITY_IDENTIFICATION kNtSecurityIdentification
#define SECURITY_IMPERSONATION kNtSecurityImpersonation
#define SECURITY_DELEGATION kNtSecurityDelegation

#define TOKEN_DUPLICATE kNtTokenDuplicate
#define TOKEN_IMPERSONATE kNtTokenImpersonate
#define TOKEN_QUERY kNtTokenQuery
#define TOKEN_QUERY_SOURCE kNtTokenQuerySource
#define TOKEN_ADJUST_PRIVILEGES kNtTokenAdjustPrivileges
#define TOKEN_ADJUST_GROUPS kNtTokenAdjustGroups
#define TOKEN_ADJUST_DEFAULT kNtTokenAdjustDefault
#define TOKEN_ADJUST_SESSIONID kNtTokenAdjustSessionid
#define TOKEN_ALL_ACCESS_P kNtTokenAllAccessP
#define TOKEN_ALL_ACCESS kNtTokenAllAccess
#define TOKEN_READ kNtTokenRead
#define TOKEN_WRITE kNtTokenWrite
#define TOKEN_EXECUTE kNtTokenExecute
#define TOKEN_TRUST_CONSTRAINT_MASK kNtTokenTrustConstraintMask
#define TOKEN_ACCESS_PSEUDO_HANDLE_WIN8 kNtTokenAccessPseudoHandleWin8
#define TOKEN_ACCESS_PSEUDO_HANDLE kNtTokenAccessPseudoHandle

#define FOREGROUND_BLUE kNtForegroundBlue
#define FOREGROUND_GREEN kNtForegroundGreen
#define FOREGROUND_RED kNtForegroundRed
#define FOREGROUND_INTENSITY kNtForegroundIntensity
#define BACKGROUND_BLUE kNtBackgroundBlue
#define BACKGROUND_GREEN kNtBackgroundGreen
#define BACKGROUND_RED kNtBackgroundRed
#define BACKGROUND_INTENSITY kNtBackgroundIntensity

#define UNLEN 256

#define DUPLICATE_CLOSE_SOURCE kNtDuplicateCloseSource
#define DUPLICATE_SAME_ACCESS kNtDuplicateSameAccess

#define IMAGE_FILE_MACHINE_UNKNOWN kNtImageFileMachineUnknown
#define IMAGE_FILE_MACHINE_TARGET_HOST kNtImageFileMachineTargetHost
#define IMAGE_FILE_MACHINE_I386 kNtImageFileMachineI386
#define IMAGE_FILE_MACHINE_R3000 kNtImageFileMachineR3000
#define IMAGE_FILE_MACHINE_R4000 kNtImageFileMachineR4000
#define IMAGE_FILE_MACHINE_R10000 kNtImageFileMachineR10000
#define IMAGE_FILE_MACHINE_WCEMIPSV2 kNtImageFileMachineWcemipsv2
#define IMAGE_FILE_MACHINE_ALPHA kNtImageFileMachineAlpha
#define IMAGE_FILE_MACHINE_SH3 kNtImageFileMachineSh3
#define IMAGE_FILE_MACHINE_SH3DSP kNtImageFileMachineSh3dsp
#define IMAGE_FILE_MACHINE_SH3E kNtImageFileMachineSh3e
#define IMAGE_FILE_MACHINE_SH4 kNtImageFileMachineSh4
#define IMAGE_FILE_MACHINE_SH5 kNtImageFileMachineSh5
#define IMAGE_FILE_MACHINE_ARM kNtImageFileMachineArm
#define IMAGE_FILE_MACHINE_THUMB kNtImageFileMachineThumb
#define IMAGE_FILE_MACHINE_ARMNT kNtImageFileMachineArmnt
#define IMAGE_FILE_MACHINE_AM33 kNtImageFileMachineAm33
#define IMAGE_FILE_MACHINE_POWERPC kNtImageFileMachinePowerpc
#define IMAGE_FILE_MACHINE_POWERPCFP kNtImageFileMachinePowerpcfp
#define IMAGE_FILE_MACHINE_IA64 kNtImageFileMachineIa64
#define IMAGE_FILE_MACHINE_MIPS16 kNtImageFileMachineMips16
#define IMAGE_FILE_MACHINE_ALPHA64 kNtImageFileMachineAlpha64
#define IMAGE_FILE_MACHINE_MIPSFPU kNtImageFileMachineMipsfpu
#define IMAGE_FILE_MACHINE_MIPSFPU16 kNtImageFileMachineMipsfpu16
#define IMAGE_FILE_MACHINE_AXP64 IMAGE_FILE_MACHINE_ALPHA64
#define IMAGE_FILE_MACHINE_TRICORE kNtImageFileMachineTricore
#define IMAGE_FILE_MACHINE_CEF kNtImageFileMachineCef
#define IMAGE_FILE_MACHINE_EBC kNtImageFileMachineEbc
#define IMAGE_FILE_MACHINE_NEXGEN32E kNtImageFileMachineNexgen32e
#define IMAGE_FILE_MACHINE_M32R kNtImageFileMachineM32r
#define IMAGE_FILE_MACHINE_ARM64 kNtImageFileMachineArm64
#define IMAGE_FILE_MACHINE_CEE kNtImageFileMachineCee

#define PE_32BIT kNtPe32bit
#define PE_64BIT kNtPe64bit

#define IMAGE_DLLCHARACTERISTICS_HIGH_ENTROPY_VA kNtImageDllcharacteristicsHighEntropyVa
#define IMAGE_DLLCHARACTERISTICS_DYNAMIC_BASE kNtImageDllcharacteristicsDynamicBase
#define IMAGE_DLLCHARACTERISTICS_FORCE_INTEGRITY kNtImageDllcharacteristicsForceIntegrity
#define IMAGE_DLLCHARACTERISTICS_NX_COMPAT kNtImageDllcharacteristicsNxCompat
#define IMAGE_DLLCHARACTERISTICS_NO_ISOLATION kNtImageDllcharacteristicsNoIsolation
#define IMAGE_DLLCHARACTERISTICS_NO_SEH kNtImageDllcharacteristicsNoSeh
#define IMAGE_DLLCHARACTERISTICS_NO_BIND kNtImageDllcharacteristicsNoBind
#define IMAGE_DLLCHARACTERISTICS_APPCONTAINER kNtImageDllcharacteristicsAppcontainer
#define IMAGE_DLLCHARACTERISTICS_WDM_DRIVER kNtImageDllcharacteristicsWdmDriver
#define IMAGE_DLLCHARACTERISTICS_GUARD_CF kNtImageDllcharacteristicsGuardCf
#define IMAGE_DLLCHARACTERISTICS_TERMINAL_SERVER_AWARE kNtImageDllcharacteristicsTerminalServerAware

#define IMAGE_SUBSYSTEM_UNKNOWN kNtImageSubsystemUnknown
#define IMAGE_SUBSYSTEM_NATIVE kNtImageSubsystemNative
#define IMAGE_SUBSYSTEM_WINDOWS_GUI kNtImageSubsystemWindowsGui
#define IMAGE_SUBSYSTEM_WINDOWS_CUI kNtImageSubsystemWindowsCui
#define IMAGE_SUBSYSTEM_OS2_CUI kNtImageSubsystemOs2Cui
#define IMAGE_SUBSYSTEM_POSIX_CUI kNtImageSubsystemPosixCui
#define IMAGE_SUBSYSTEM_NATIVE_WINDOWS kNtImageSubsystemNativeWindows
#define IMAGE_SUBSYSTEM_WINDOWS_CE_GUI kNtImageSubsystemWindowsCeGui
#define IMAGE_SUBSYSTEM_EFI_APPLICATION kNtImageSubsystemEfiApplication
#define IMAGE_SUBSYSTEM_EFI_BOOT_SERVICE_DRIVER kNtImageSubsystemEfiBootServiceDriver
#define IMAGE_SUBSYSTEM_EFI_RUNTIME_DRIVER kNtImageSubsystemEfiRuntimeDriver
#define IMAGE_SUBSYSTEM_EFI_ROM kNtImageSubsystemEfiRom
#define IMAGE_SUBSYSTEM_XBOX kNtImageSubsystemXbox
#define IMAGE_SUBSYSTEM_WINDOWS_BOOT_APPLICATION kNtImageSubsystemWindowsBootApplication
#define IMAGE_SUBSYSTEM_XBOX_CODE_CATALOG kNtImageSubsystemXboxCodeCatalog

#define IMAGE_FILE_RELOCS_STRIPPED kNtImageFileRelocsStripped
#define IMAGE_FILE_EXECUTABLE_IMAGE kNtImageFileExecutableImage
#define IMAGE_FILE_LINE_NUMS_STRIPPED kNtImageFileLineNumsStripped
#define IMAGE_FILE_LOCAL_SYMS_STRIPPED kNtImageFileLocalSymsStripped
#define IMAGE_FILE_AGGRESIVE_WS_TRIM kNtImageFileAggresiveWsTrim
#define IMAGE_FILE_LARGE_ADDRESS_AWARE kNtImageFileLargeAddressAware
#define IMAGE_FILE_BYTES_REVERSED_LO kNtImageFileBytesReversedLo
#define IMAGE_FILE_32BIT_MACHINE kNtImageFile_32bitMachine
#define IMAGE_FILE_DEBUG_STRIPPED kNtImageFileDebugStripped
#define IMAGE_FILE_REMOVABLE_RUN_FROM_SWAP kNtImageFileRemovableRunFromSwap
#define IMAGE_FILE_NET_RUN_FROM_SWAP kNtImageFileNetRunFromSwap
#define IMAGE_FILE_SYSTEM kNtImageFileSystem
#define IMAGE_FILE_DLL kNtImageFileDll
#define IMAGE_FILE_UP_SYSTEM_ONLY kNtImageFileUpSystemOnly
#define IMAGE_FILE_BYTES_REVERSED_HI kNtImageFileBytesReversedHi

#define IMAGE_DOS_SIGNATURE kNtImageDosSignature
#define IMAGE_OS2_SIGNATURE kNtImageOs2Signature
#define IMAGE_OS2_SIGNATURE_LE kNtImageOs2SignatureLe
#define IMAGE_VXD_SIGNATURE kNtImageVxdSignature
#define IMAGE_NT_SIGNATURE kNtImageNtSignature

#define RICHKEY kNtRichkey
#define COOKIE_DEFAULT kNtCookieDefault
#define SIZE_OF_80387_REGISTERS kNtSizeOf_80387Registers
#define MAXIMUM_SUPPORTED_EXTENSION kNtMaximumSupportedExtension

#define PE_SUBSYSTEM_WINDOWS_GUI kNtPeSubsystemWindowsGui
#define PE_SUBSYSTEM_WINDOWS_CUI kNtPeSubsystemWindowsCui

#define PE_FILE_RELOCS_STRIPPED kNtPeFileRelocsStripped
#define PE_FILE_EXECUTABLE_IMAGE kNtPeFileExecutableImage
#define PE_FILE_LINE_NUMS_STRIPPED kNtPeFileLineNumsStripped
#define PE_FILE_LOCAL_SYMS_STRIPPED kNtPeFileLocalSymsStripped
#define PE_FILE_32BIT_MACHINE kNtPeFile_32bitMachine
#define PE_FILE_DLL kNtPeFileDll

#define PE_SECTION_CNT_CODE kNtPeSectionCntCode
#define PE_SECTION_CNT_INITIALIZED_DATA kNtPeSectionCntInitializedData
#define PE_SECTION_CNT_UNINITIALIZED_DATA kNtPeSectionCntUninitializedData
#define PE_SECTION_GPREL kNtPeSectionGprel
#define PE_SECTION_MEM_DISCARDABLE kNtPeSectionMemDiscardable
#define PE_SECTION_MEM_NOT_CACHED kNtPeSectionMemNotCached
#define PE_SECTION_MEM_NOT_PAGED kNtPeSectionMemNotPaged
#define PE_SECTION_MEM_SHARED kNtPeSectionMemShared
#define PE_SECTION_MEM_EXECUTE kNtPeSectionMemExecute
#define PE_SECTION_MEM_READ kNtPeSectionMemRead
#define PE_SECTION_MEM_WRITE kNtPeSectionMemWrite

#define PE_GUARD_CF_INSTRUMENTED kNtPeGuardCfInstrumented
#define PE_GUARD_CFW_INSTRUMENTED kNtPeGuardCfwInstrumented
#define PE_GUARD_CF_FUNCTION_TABLE_PRESENT kNtPeGuardCfFunctionTablePresent
#define PE_GUARD_SECURITY_COOKIE_UNUSED kNtPeGuardSecurityCookieUnused

#define PE_REL_BASED_ABSOLUTE kNtPeRelBasedAbsolute
#define PE_REL_BASED_HIGH kNtPeRelBasedHigh
#define PE_REL_BASED_LOW kNtPeRelBasedLow
#define PE_REL_BASED_HIGHLOW kNtPeRelBasedHighlow
#define PE_REL_BASED_HIGHADJ kNtPeRelBasedHighadj
#define PE_REL_BASED_MIPS_JMPADDR kNtPeRelBasedMipsJmpaddr
#define PE_REL_BASED_SECTION kNtPeRelBasedSection
#define PE_REL_BASED_REL32 kNtPeRelBasedRel32
#define PE_REL_BASED_MIPS_JMPADDR16 kNtPeRelBasedMipsJmpaddr16
#define PE_REL_BASED_IA64_IMM64 kNtPeRelBasedIa64Imm64
#define PE_REL_BASED_DIR64 kNtPeRelBasedDir64
#define PE_REL_BASED_HIGH3ADJ kNtPeRelBasedHigh3adj

#define IMAGE_DIRECTORY_ENTRY_EXPORT kNtImageDirectoryEntryExport
#define IMAGE_DIRECTORY_ENTRY_IMPORT kNtImageDirectoryEntryImport
#define IMAGE_DIRECTORY_ENTRY_RESOURCE kNtImageDirectoryEntryResource
#define IMAGE_DIRECTORY_ENTRY_EXCEPTION kNtImageDirectoryEntryException
#define IMAGE_DIRECTORY_ENTRY_SECURITY kNtImageDirectoryEntrySecurity
#define IMAGE_DIRECTORY_ENTRY_BASERELOC kNtImageDirectoryEntryBasereloc
#define IMAGE_DIRECTORY_ENTRY_DEBUG kNtImageDirectoryEntryDebug
#define IMAGE_DIRECTORY_ENTRY_ARCHITECTURE kNtImageDirectoryEntryArchitecture
#define IMAGE_DIRECTORY_ENTRY_GLOBALPTR kNtImageDirectoryEntryGlobalptr
#define IMAGE_DIRECTORY_ENTRY_TLS kNtImageDirectoryEntryTls
#define IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG kNtImageDirectoryEntryLoadConfig
#define IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT kNtImageDirectoryEntryBoundImport
#define IMAGE_DIRECTORY_ENTRY_IAT kNtImageDirectoryEntryIat
#define IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT kNtImageDirectoryEntryDelayImport
#define IMAGE_DIRECTORY_ENTRY_COM_DESCRIPTOR kNtImageDirectoryEntryComDescriptor

#define IMAGE_SCN_TYPE_NO_PAD kNtImageScnTypeNoPad
#define IMAGE_SCN_CNT_CODE kNtImageScnCntCode
#define IMAGE_SCN_CNT_INITIALIZED_DATA kNtImageScnCntInitializedData
#define IMAGE_SCN_CNT_UNINITIALIZED_DATA kNtImageScnCntUninitializedData
#define IMAGE_SCN_LNK_OTHER kNtImageScnLnkOther
#define IMAGE_SCN_LNK_INFO kNtImageScnLnkInfo
#define IMAGE_SCN_LNK_REMOVE kNtImageScnLnkRemove
#define IMAGE_SCN_LNK_COMDAT kNtImageScnLnkComdat
#define IMAGE_SCN_NO_DEFER_SPEC_EXC kNtImageScnNoDeferSpecExc
#define IMAGE_SCN_GPREL kNtImageScnGprel
#define IMAGE_SCN_MEM_FARDATA kNtImageScnMemFardata
#define IMAGE_SCN_MEM_PURGEABLE kNtImageScnMemPurgeable
#define IMAGE_SCN_MEM_16BIT kNtImageScnMem16bit
#define IMAGE_SCN_MEM_LOCKED kNtImageScnMemLocked
#define IMAGE_SCN_MEM_PRELOAD kNtImageScnMemPreload

#define IMAGE_SCN_ALIGN_1BYTES kNtImageScnAlign1bytes
#define IMAGE_SCN_ALIGN_2BYTES kNtImageScnAlign2bytes
#define IMAGE_SCN_ALIGN_4BYTES kNtImageScnAlign4bytes
#define IMAGE_SCN_ALIGN_8BYTES kNtImageScnAlign8bytes
#define IMAGE_SCN_ALIGN_16BYTES kNtImageScnAlign16bytes
#define IMAGE_SCN_ALIGN_32BYTES kNtImageScnAlign32bytes
#define IMAGE_SCN_ALIGN_64BYTES kNtImageScnAlign64bytes
#define IMAGE_SCN_ALIGN_128BYTES kNtImageScnAlign128bytes
#define IMAGE_SCN_ALIGN_256BYTES kNtImageScnAlign256bytes
#define IMAGE_SCN_ALIGN_512BYTES kNtImageScnAlign512bytes
#define IMAGE_SCN_ALIGN_1024BYTES kNtImageScnAlign1024bytes
#define IMAGE_SCN_ALIGN_2048BYTES kNtImageScnAlign2048bytes
#define IMAGE_SCN_ALIGN_4096BYTES kNtImageScnAlign4096bytes
#define IMAGE_SCN_ALIGN_8192BYTES kNtImageScnAlign8192bytes
#define IMAGE_SCN_ALIGN_MASK kNtImageScnAlignMask

#define IMAGE_SCN_LNK_NRELOC_OVFL kNtImageScnLnkNrelocOvfl
#define IMAGE_SCN_MEM_DISCARDABLE kNtImageScnMemDiscardable
#define IMAGE_SCN_MEM_NOT_CACHED kNtImageScnMemNotCached
#define IMAGE_SCN_MEM_NOT_PAGED kNtImageScnMemNotPaged
#define IMAGE_SCN_MEM_SHARED kNtImageScnMemShared
#define IMAGE_SCN_MEM_EXECUTE kNtImageScnMemExecute
#define IMAGE_SCN_MEM_READ kNtImageScnMemRead
#define IMAGE_SCN_MEM_WRITE kNtImageScnMemWrite
#define IMAGE_SCN_SCALE_INDEX kNtImageScnScaleIndex

#define IMAGE_SYM_UNDEFINED kNtImageSymUndefined
#define IMAGE_SYM_ABSOLUTE kNtImageSymAbsolute
#define IMAGE_SYM_DEBUG kNtImageSymDebug
#define IMAGE_SYM_SECTION_MAX kNtImageSymSectionMax
#define IMAGE_SYM_SECTION_MAX_EX kNtImageSymSectionMaxEx
#define IMAGE_SYM_TYPE_NULL kNtImageSymTypeNull
#define IMAGE_SYM_TYPE_VOID kNtImageSymTypeVoid
#define IMAGE_SYM_TYPE_CHAR kNtImageSymTypeChar
#define IMAGE_SYM_TYPE_SHORT kNtImageSymTypeShort
#define IMAGE_SYM_TYPE_INT kNtImageSymTypeInt
#define IMAGE_SYM_TYPE_LONG kNtImageSymTypeLong
#define IMAGE_SYM_TYPE_FLOAT kNtImageSymTypeFloat
#define IMAGE_SYM_TYPE_DOUBLE kNtImageSymTypeDouble
#define IMAGE_SYM_TYPE_STRUCT kNtImageSymTypeStruct
#define IMAGE_SYM_TYPE_UNION kNtImageSymTypeUnion
#define IMAGE_SYM_TYPE_ENUM kNtImageSymTypeEnum
#define IMAGE_SYM_TYPE_MOE kNtImageSymTypeMoe
#define IMAGE_SYM_TYPE_BYTE kNtImageSymTypeByte
#define IMAGE_SYM_TYPE_WORD kNtImageSymTypeWord
#define IMAGE_SYM_TYPE_UINT kNtImageSymTypeUint
#define IMAGE_SYM_TYPE_DWORD kNtImageSymTypeDword
#define IMAGE_SYM_TYPE_PCODE kNtImageSymTypePcode
#define IMAGE_SYM_DTYPE_NULL kNtImageSymDtypeNull
#define IMAGE_SYM_DTYPE_POINTER kNtImageSymDtypePointer
#define IMAGE_SYM_DTYPE_FUNCTION kNtImageSymDtypeFunction
#define IMAGE_SYM_DTYPE_ARRAY kNtImageSymDtypeArray
#define IMAGE_SYM_CLASS_END_OF_FUNCTION kNtImageSymClassEndOfFunction
#define IMAGE_SYM_CLASS_NULL kNtImageSymClassNull
#define IMAGE_SYM_CLASS_AUTOMATIC kNtImageSymClassAutomatic
#define IMAGE_SYM_CLASS_EXTERNAL kNtImageSymClassExternal
#define IMAGE_SYM_CLASS_STATIC kNtImageSymClassStatic
#define IMAGE_SYM_CLASS_REGISTER kNtImageSymClassRegister
#define IMAGE_SYM_CLASS_EXTERNAL_DEF kNtImageSymClassExternalDef
#define IMAGE_SYM_CLASS_LABEL kNtImageSymClassLabel
#define IMAGE_SYM_CLASS_UNDEFINED_LABEL kNtImageSymClassUndefinedLabel
#define IMAGE_SYM_CLASS_MEMBER_OF_STRUCT kNtImageSymClassMemberOfStruct
#define IMAGE_SYM_CLASS_ARGUMENT kNtImageSymClassArgument
#define IMAGE_SYM_CLASS_STRUCT_TAG kNtImageSymClassStructTag
#define IMAGE_SYM_CLASS_MEMBER_OF_UNION kNtImageSymClassMemberOfUnion
#define IMAGE_SYM_CLASS_UNION_TAG kNtImageSymClassUnionTag
#define IMAGE_SYM_CLASS_TYPE_DEFINITION kNtImageSymClassTypeDefinition
#define IMAGE_SYM_CLASS_UNDEFINED_STATIC kNtImageSymClassUndefinedStatic
#define IMAGE_SYM_CLASS_ENUM_TAG kNtImageSymClassEnumTag
#define IMAGE_SYM_CLASS_MEMBER_OF_ENUM kNtImageSymClassMemberOfEnum
#define IMAGE_SYM_CLASS_REGISTER_PARAM kNtImageSymClassRegisterParam
#define IMAGE_SYM_CLASS_BIT_FIELD kNtImageSymClassBitField
#define IMAGE_SYM_CLASS_FAR_EXTERNAL kNtImageSymClassFarExternal
#define IMAGE_SYM_CLASS_BLOCK kNtImageSymClassBlock
#define IMAGE_SYM_CLASS_FUNCTION kNtImageSymClassFunction
#define IMAGE_SYM_CLASS_END_OF_STRUCT kNtImageSymClassEndOfStruct
#define IMAGE_SYM_CLASS_FILE kNtImageSymClassFile
#define IMAGE_SYM_CLASS_SECTION kNtImageSymClassSection
#define IMAGE_SYM_CLASS_WEAK_EXTERNAL kNtImageSymClassWeakExternal
#define IMAGE_SYM_CLASS_CLR_TOKEN kNtImageSymClassClrToken

#define IMAGE_COMDAT_SELECT_NODUPLICATES kNtImageComdatSelectNoduplicates
#define IMAGE_COMDAT_SELECT_ANY kNtImageComdatSelectAny
#define IMAGE_COMDAT_SELECT_SAME_SIZE kNtImageComdatSelectSameSize
#define IMAGE_COMDAT_SELECT_EXACT_MATCH kNtImageComdatSelectExactMatch
#define IMAGE_COMDAT_SELECT_ASSOCIATIVE kNtImageComdatSelectAssociative
#define IMAGE_COMDAT_SELECT_LARGEST kNtImageComdatSelectLargest
#define IMAGE_COMDAT_SELECT_NEWEST kNtImageComdatSelectNewest

#define IMAGE_WEAK_EXTERN_SEARCH_NOLIBRARY kNtImageWeakExternSearchNolibrary
#define IMAGE_WEAK_EXTERN_SEARCH_LIBRARY kNtImageWeakExternSearchLibrary
#define IMAGE_WEAK_EXTERN_SEARCH_ALIAS kNtImageWeakExternSearchAlias
#define IMAGE_WEAK_EXTERN_ANTI_DEPENDENCY kNtImageWeakExternAntiDependency

#define IMAGE_REL_NEXGEN32E_ABSOLUTE kNtImageRelNexgen32eAbsolute
#define IMAGE_REL_NEXGEN32E_ADDR64 kNtImageRelNexgen32eAddr64
#define IMAGE_REL_NEXGEN32E_ADDR32 kNtImageRelNexgen32eAddr32
#define IMAGE_REL_NEXGEN32E_ADDR32NB kNtImageRelNexgen32eAddr32nb
#define IMAGE_REL_NEXGEN32E_REL32 kNtImageRelNexgen32eRel32
#define IMAGE_REL_NEXGEN32E_REL32_1 kNtImageRelNexgen32eRel321
#define IMAGE_REL_NEXGEN32E_REL32_2 kNtImageRelNexgen32eRel322
#define IMAGE_REL_NEXGEN32E_REL32_3 kNtImageRelNexgen32eRel323
#define IMAGE_REL_NEXGEN32E_REL32_4 kNtImageRelNexgen32eRel324
#define IMAGE_REL_NEXGEN32E_REL32_5 kNtImageRelNexgen32eRel325
#define IMAGE_REL_NEXGEN32E_SECTION kNtImageRelNexgen32eSection
#define IMAGE_REL_NEXGEN32E_SECREL kNtImageRelNexgen32eSecrel
#define IMAGE_REL_NEXGEN32E_SECREL7 kNtImageRelNexgen32eSecrel7
#define IMAGE_REL_NEXGEN32E_TOKEN kNtImageRelNexgen32eToken
#define IMAGE_REL_NEXGEN32E_SREL32 kNtImageRelNexgen32eSrel32
#define IMAGE_REL_NEXGEN32E_PAIR kNtImageRelNexgen32ePair
#define IMAGE_REL_NEXGEN32E_SSPAN32 kNtImageRelNexgen32eSspan32

#define IMAGE_REL_BASED_ABSOLUTE kNtImageRelBasedAbsolute
#define IMAGE_REL_BASED_HIGH kNtImageRelBasedHigh
#define IMAGE_REL_BASED_LOW kNtImageRelBasedLow
#define IMAGE_REL_BASED_HIGHLOW kNtImageRelBasedHighlow
#define IMAGE_REL_BASED_HIGHADJ kNtImageRelBasedHighadj
#define IMAGE_REL_BASED_MACHINE_SPECIFIC_5 kNtImageRelBasedMachineSpecific5
#define IMAGE_REL_BASED_RESERVED kNtImageRelBasedReserved
#define IMAGE_REL_BASED_MACHINE_SPECIFIC_7 kNtImageRelBasedMachineSpecific7
#define IMAGE_REL_BASED_MACHINE_SPECIFIC_8 kNtImageRelBasedMachineSpecific8
#define IMAGE_REL_BASED_MACHINE_SPECIFIC_9 kNtImageRelBasedMachineSpecific9
#define IMAGE_REL_BASED_DIR64 kNtImageRelBasedDir64

#define IMAGE_ARCHIVE_START_SIZE kNtImageArchiveStartSize
#define IMAGE_ARCHIVE_START kNtImageArchiveStart
#define IMAGE_ARCHIVE_END kNtImageArchiveEnd
#define IMAGE_ARCHIVE_PAD kNtImageArchivePad
#define IMAGE_ARCHIVE_LINKER_MEMBER kNtImageArchiveLinkerMember
#define IMAGE_ARCHIVE_LONGNAMES_MEMBER kNtImageArchiveLongnamesMember
#define IMAGE_ARCHIVE_HYBRIDMAP_MEMBER kNtImageArchiveHybridmapMember

#define IMAGE_ORDINAL_FLAG kNtImageOrdinalFlag
#define IMAGE_ORDINAL(Ordinal) NtImageOrdinal(Ordinal)
#define IMAGE_SNAP_BY_ORDINAL(Ordinal) NtImageSnapByOrdinal(Ordinal)

#define IMAGE_RESOURCE_NAME_IS_STRING kNtImageResourceNameIsString
#define IMAGE_RESOURCE_DATA_IS_DIRECTORY kNtImageResourceDataIsDirectory

#define IMAGE_DYNAMIC_RELOCATION_GUARD_RF_PROLOGUE kNtImageDynamicRelocationGuardRfPrologue
#define IMAGE_DYNAMIC_RELOCATION_GUARD_RF_EPILOGUE kNtImageDynamicRelocationGuardRfEpilogue

#define IMAGE_HOT_PATCH_BASE_OBLIGATORY kNtImageHotPatchBaseObligatory
#define IMAGE_HOT_PATCH_CHUNK_INVERSE kNtImageHotPatchChunkInverse
#define IMAGE_HOT_PATCH_CHUNK_OBLIGATORY kNtImageHotPatchChunkObligatory
#define IMAGE_HOT_PATCH_CHUNK_RESERVED kNtImageHotPatchChunkReserved
#define IMAGE_HOT_PATCH_CHUNK_TYPE kNtImageHotPatchChunkType
#define IMAGE_HOT_PATCH_CHUNK_SOURCE_RVA kNtImageHotPatchChunkSourceRva
#define IMAGE_HOT_PATCH_CHUNK_TARGET_RVA kNtImageHotPatchChunkTargetRva
#define IMAGE_HOT_PATCH_CHUNK_SIZE kNtImageHotPatchChunkSize
#define IMAGE_HOT_PATCH_NONE kNtImageHotPatchNone
#define IMAGE_HOT_PATCH_FUNCTION kNtImageHotPatchFunction
#define IMAGE_HOT_PATCH_ABSOLUTE kNtImageHotPatchAbsolute
#define IMAGE_HOT_PATCH_REL32 kNtImageHotPatchRel32
#define IMAGE_HOT_PATCH_CALL_TARGET kNtImageHotPatchCallTarget
#define IMAGE_HOT_PATCH_INDIRECT kNtImageHotPatchIndirect
#define IMAGE_HOT_PATCH_NO_CALL_TARGET kNtImageHotPatchNoCallTarget
#define IMAGE_HOT_PATCH_DYNAMIC_VALUE kNtImageHotPatchDynamicValue
#define IMAGE_GUARD_CF_INSTRUMENTED kNtImageGuardCfInstrumented
#define IMAGE_GUARD_CFW_INSTRUMENTED kNtImageGuardCfwInstrumented
#define IMAGE_GUARD_CF_FUNCTION_TABLE_PRESENT kNtImageGuardCfFunctionTablePresent
#define IMAGE_GUARD_SECURITY_COOKIE_UNUSED kNtImageGuardSecurityCookieUnused
#define IMAGE_GUARD_PROTECT_DELAYLOAD_IAT kNtImageGuardProtectDelayloadIat
#define IMAGE_GUARD_DELAYLOAD_IAT_IN_ITS_OWN_SECTION kNtImageGuardDelayloadIatInItsOwnSection
#define IMAGE_GUARD_CF_EXPORT_SUPPRESSION_INFO_PRESENT kNtImageGuardCfExportSuppressionInfoPresent
#define IMAGE_GUARD_CF_ENABLE_EXPORT_SUPPRESSION kNtImageGuardCfEnableExportSuppression
#define IMAGE_GUARD_CF_LONGJUMP_TABLE_PRESENT kNtImageGuardCfLongjumpTablePresent
#define IMAGE_GUARD_RF_INSTRUMENTED kNtImageGuardRfInstrumented
#define IMAGE_GUARD_RF_ENABLE kNtImageGuardRfEnable
#define IMAGE_GUARD_RF_STRICT kNtImageGuardRfStrict
#define IMAGE_GUARD_CF_FUNCTION_TABLE_SIZE_MASK kNtImageGuardCfFunctionTableSizeMask
#define IMAGE_GUARD_CF_FUNCTION_TABLE_SIZE_SHIFT kNtImageGuardCfFunctionTableSizeShift
#define IMAGE_GUARD_FLAG_FID_SUPPRESSED kNtImageGuardFlagFidSuppressed
#define IMAGE_GUARD_FLAG_EXPORT_SUPPRESSED kNtImageGuardFlagExportSuppressed

#define IMAGE_ENCLAVE_IMPORT_MATCH_NONE kNtImageEnclaveImportMatchNone
#define IMAGE_ENCLAVE_IMPORT_MATCH_UNIQUE_ID kNtImageEnclaveImportMatchUniqueId
#define IMAGE_ENCLAVE_IMPORT_MATCH_AUTHOR_ID kNtImageEnclaveImportMatchAuthorId
#define IMAGE_ENCLAVE_IMPORT_MATCH_FAMILY_ID kNtImageEnclaveImportMatchFamilyId
#define IMAGE_ENCLAVE_IMPORT_MATCH_IMAGE_ID kNtImageEnclaveImportMatchImageId

#define IMAGE_DEBUG_TYPE_UNKNOWN kNtImageDebugTypeUnknown
#define IMAGE_DEBUG_TYPE_COFF kNtImageDebugTypeCoff
#define IMAGE_DEBUG_TYPE_CODEVIEW kNtImageDebugTypeCodeview
#define IMAGE_DEBUG_TYPE_FPO kNtImageDebugTypeFpo
#define IMAGE_DEBUG_TYPE_MISC kNtImageDebugTypeMisc
#define IMAGE_DEBUG_TYPE_EXCEPTION kNtImageDebugTypeException
#define IMAGE_DEBUG_TYPE_FIXUP kNtImageDebugTypeFixup
#define IMAGE_DEBUG_TYPE_OMAP_TO_SRC kNtImageDebugTypeOmapToSrc
#define IMAGE_DEBUG_TYPE_OMAP_FROM_SRC kNtImageDebugTypeOmapFromSrc
#define IMAGE_DEBUG_TYPE_BORLAND kNtImageDebugTypeBorland
#define IMAGE_DEBUG_TYPE_RESERVED10 kNtImageDebugTypeReserved10
#define IMAGE_DEBUG_TYPE_CLSID kNtImageDebugTypeClsid
#define IMAGE_DEBUG_TYPE_VC_FEATURE kNtImageDebugTypeVcFeature
#define IMAGE_DEBUG_TYPE_POGO kNtImageDebugTypePogo
#define IMAGE_DEBUG_TYPE_ILTCG kNtImageDebugTypeIltcg
#define IMAGE_DEBUG_TYPE_MPX kNtImageDebugTypeMpx
#define IMAGE_DEBUG_TYPE_REPRO kNtImageDebugTypeRepro

#define FRAME_FPO kNtFrameFpo
#define FRAME_TRAP kNtFrameTrap
#define FRAME_TSS kNtFrameTss
#define FRAME_NONFPO kNtFrameNonfpo

#define IMAGE_SIZEOF_SHORT_NAME kNtImageSizeofShortName
#define IMAGE_SIZEOF_SECTION_HEADER kNtImageSizeofSectionHeader
#define IMAGE_SIZEOF_SYMBOL kNtImageSizeofSymbol
#define IMAGE_ENCLAVE_LONG_ID_LENGTH kNtImageEnclaveLongIdLength
#define IMAGE_ENCLAVE_SHORT_ID_LENGTH kNtImageEnclaveShortIdLength
#define IMAGE_NUMBEROF_DIRECTORY_ENTRIES kNtImageNumberofDirectoryEntries

#define FILE_BEGIN kNtFileBegin
#define FILE_CURRENT kNtFileCurrent
#define FILE_END kNtFileEnd

#define WSADATA struct NtWsaData
#define LPWSADATA struct NtWsaData*

#define FD_MAX_EVENTS 10

#define CSADDR_INFO struct NtCsAddrInfo
#define PCSADDR_INFO struct NtCsAddrInfo*
#define LPCSADDR_INFO struct NtCsAddrInfo*

#define AFPROTOCOLS struct NtAfProtocols
#define PAFPROTOCOLS struct NtAfProtocols*
#define LPAFPROTOCOLS struct NtAfProtocols*

#define WSAECOMPARATOR int
#define PWSAECOMPARATOR int*
#define LPWSAECOMPARATOR int*

#define WSANETWORKEVENTS struct NtWsaNetworkEvents
#define PWSANETWORKEVENTS struct NtWsaNetworkEvents*
#define LPWSANETWORKEVENTS struct NtWsaNetworkEvents*

#define WSANSCLASSINFOW struct NtWsansClassInfo
#define PWSANSCLASSINFOW struct NtWsansClassInfo*
#define LPWSANSCLASSINFOW struct NtWsansClassInfo*

#define WSASERVICECLASSINFOW struct NtWsaServiceClassInfo
#define PWSASERVICECLASSINFOW struct NtWsaServiceClassInfo*
#define LPWSASERVICECLASSINFOW struct NtWsaServiceClassInfo*

#define WSANAMESPACE_INFOEX struct NtWsaNamespaceInfoEx
#define PWSANAMESPACE_INFOEX struct NtWsaNamespaceInfoEx*
#define LPWSANAMESPACE_INFOEX struct NtWsaNamespaceInfoEx*

#define WSAQUERYSET struct NtWsaQuerySet
#define PWSAQUERYSET struct NtWsaQuerySet*
#define LPWSAQUERYSET struct NtWsaQuerySet*

#define WSAVERSION struct NtWsaVersion
#define PWSAVERSION struct NtWsaVersion*
#define LPWSAVERSION struct NtWsaVersion*

#define SOCKADDR struct sockaddr
#define PSOCKADDR struct sockaddr*
#define LPSOCKADDR struct sockaddr*

#define SOCKET_ADDRESS struct NtSocketAddress
#define PSOCKET_ADDRESS struct NtSocketAddress*
#define LPSOCKET_ADDRESS struct NtSocketAddress*

#define REPARSE_DATA_BUFFER struct NtReparseDataBuffer
#define PREPARSE_DATA_BUFFER struct NtReparseDataBuffer*
#define LPREPARSE_DATA_BUFFER struct NtReparseDataBuffer*

#define SOCKET_ADDRESS_LIST struct NtSocketAddressList
#define PSOCKET_ADDRESS_LIST struct NtSocketAddressList*
#define LPSOCKET_ADDRESS_LIST struct NtSocketAddressList*

#define FLOWSPEC struct NtFlowSpec
#define LPFLOWSPEC struct NtFlowSpec*

#define QOS struct NtQos
#define LPQOS struct NtQos*

#define _WSAPROTOCOLCHAIN NtWsaProtocolChain
#define WSAPROTOCOLCHAIN struct NtWsaProtocolChain
#define LPWSAPROTOCOLCHAIN struct NtWsaProtocolChain*

#define _WSAPROTOCOL_INFO NtWsaProtocolInfo
#define WSAPROTOCOL_INFO struct NtWsaProtocolInfo
#define LPWSAPROTOCOL_INFO struct NtWsaProtocolInfo*

#define _WSABUF NtIovec
#define WSABUF struct NtIovec
#define LPWSABUF struct NtIovec*

#define _GUID NtGuid
#define GUID struct NtGuid
#define LPGUID struct NtGuid*

#define IID GUID
#define IsEqualGUID(guid1, guid2)                       \
  (!memcmp((guid1), (guid2), sizeof(GUID)))
#define IsEqualIID IsEqualGUID

#define ADDRINFOEX struct NtAddrInfoEx
#define LPADDRINFOEX struct NtAddrInfoEx*

#define WSAEVENT HANDLE
#define GROUP uint32_t
#define _OVERLAPPED NtOverlapped
#define OVERLAPPED struct NtOverlapped
#define LOVERLAPPED struct NtOverlapped*
#define WSAOVERLAPPED OVERLAPPED
#define INVALID_SOCKET -1ULL
#define SOCKET_ERROR -1
#define WSA_INVALID_EVENT -1L

#define WAIT_FAILED -1U
#define STATUS_WAIT_0 0
#define WAIT_FAILED 0xFFFFFFFFu
#define WAIT_OBJECT_0 0
#define STATUS_ABANDONED_WAIT_0 128
#define WAIT_ABANDONED 128
#define WAIT_ABANDONED_0 128
#define INFINITE 0xFFFFFFFF

#define SOCKET uint64_t
#define WSA_WAIT_IO_COMPLETION 0xc0
#define WSA_WAIT_TIMEOUT 258

#define LPCONDITIONPROC NtConditionProc
#define LPWSAOVERLAPPED_COMPLETION_ROUTINE NtWsaOverlappedCompletionRoutine

#define WSACOMPLETIONTYPE int
#define PWSACOMPLETIONTYPE int*
#define LPWSACOMPLETIONTYPE int*
#define NSP_NOTIFY_IMMEDIATELY kNtNspNotifyImmediately
#define NSP_NOTIFY_HWND kNtNspNotifyHwnd
#define NSP_NOTIFY_EVENT kNtNspNotifyEvent
#define NSP_NOTIFY_PORT kNtNspNotifyPort
#define NSP_NOTIFY_APC kNtNspNotifyApc

#define WSACOMPLETION struct NtWsaCompletion
#define PWSACOMPLETION struct NtWsaCompletion*
#define LPWSACOMPLETION struct NtWsaCompletion*

#define WSAPOLLFD struct pollfd
#define PWSAPOLLFD struct pollfd*
#define LPWSAPOLLFD struct pollfd*

#define SD_RECEIVE SHUT_RD
#define SD_SEND SHUT_WR
#define SD_BOTH SHUT_RDWR

#define WSAMSG struct NtMsgHdr
#define PWSAMSG struct NtMsgHdr*
#define LPWSAMSG struct NtMsgHdr*

#define _MEMORYSTATUSEX NtMemoryStatusEx
#define MEMORYSTATUSEX struct NtMemoryStatusEx
#define LPMEMORYSTATUSEX struct NtMemoryStatusEx*

#define HKEY_CLASSES_ROOT kNtHkeyClassesRoot
#define HKEY_CURRENT_USER kNtHkeyCurrentUser
#define HKEY_LOCAL_MACHINE kNtHkeyLocalMachine
#define HKEY_USERS kNtHkeyUsers
#define HKEY_PERFORMANCE_DATA kNtHkeyPerformanceData
#define HKEY_PERFORMANCE_TEXT kNtHkeyPerformanceText
#define HKEY_PERFORMANCE_NLSTEXT kNtHkeyPerformanceNlstext
#define HKEY_CURRENT_CONFIG kNtHkeyCurrentConfig
#define HKEY_DYN_DATA kNtHkeyDynData
#define HKEY_CURRENT_USER_LOCAL_SETTINGS kNtHkeyCurrentUserLocalSettings
#define KEY_QUERY_VALUE kNtKeyQueryValue
#define KEY_SET_VALUE kNtKeySetValue
#define KEY_CREATE_SUB_KEY kNtKeyCreateSubKey
#define KEY_ENUMERATE_SUB_KEYS kNtKeyEnumerateSubKeys
#define KEY_NOTIFY kNtKeyNotify
#define KEY_CREATE_LINK kNtKeyCreateLink
#define KEY_WOW64_32KEY kNtWow6432Key
#define KEY_WOW64_64KEY kNtWow6464Key
#define KEY_WOW64_RES kNtWow64Res
#define KEY_READ kNtKeyRead
#define KEY_WRITE kNtKeyWrite
#define KEY_EXECUTE kNtKeyExecute
#define KEY_ALL_ACCESS kNtKeyAllAccess
#define REG_NONE kNtRegNone
#define REG_SZ kNtRegSz
#define REG_EXPAND_SZ kNtRegExpandSz
#define REG_BINARY kNtRegBinary
#define REG_DWORD kNtRegDword
#define REG_DWORD_BIG_ENDIAN kNtRegDwordBigEndian
#define REG_LINK kNtRegLink
#define REG_MULTI_SZ kNtRegMultiSz
#define REG_RESOURCE_LIST kNtRegResourceList
#define REG_FULL_RESOURCE_DESCRIPTOR kNtRegFullResourceDescriptor
#define REG_RESOURCE_REQUIREMENTS_LIST kNtRegResourceRequirementsList
#define REG_QWORD kNtRegQword
#define RRF_RT_REG_NONE kNtRrfRtRegNone
#define RRF_RT_REG_SZ kNtRrfRtRegSz
#define RRF_RT_REG_EXPAND_SZ kNtRrfRtRegExpandSz
#define RRF_RT_REG_BINARY kNtRrfRtRegBinary
#define RRF_RT_REG_DWORD kNtRrfRtRegDword
#define RRF_RT_REG_MULTI_SZ kNtRrfRtRegMultiSz
#define RRF_RT_REG_QWORD kNtRrfRtRegQword
#define RRF_RT_DWORD kNtRrfRtDword
#define RRF_RT_QWORD kNtRrfRtQword
#define RRF_RT_ANY kNtRrfRtAny
#define RRF_SUBKEY_WOW6464KEY kNtRrfSubkeyWow6464key
#define RRF_SUBKEY_WOW6432KEY kNtRrfSubkeyWow6432key
#define RRF_WOW64_MASK kNtRrfWow64Mask
#define RRF_NOEXPAND kNtRrfNoexpand
#define RRF_ZEROONFAILURE kNtRrfZeroonfailure

#define NUMA_NO_PREFERRED_NODE kNtNumaNoPreferredNode

#define TF_DISCONNECT kNtTfDisconnect
#define TF_REUSE_SOCKET kNtTfReuseSocket
#define TF_WRITE_BEHIND kNtTfWriteBehind
#define TF_USE_DEFAULT_WORKER kNtTfUseDefaultWorker
#define TF_USE_SYSTEM_THREAD kNtTfUseSystemThread
#define TF_USE_KERNEL_APC kNtTfUseKernelApc

#define SIO_UDP_CONNRESET kNtSioUdpConnreset
#define SIO_SOCKET_CLOSE_NOTIFY kNtSioSocketCloseNotify
#define SIO_UDP_NETRESET kNtSioUdpNetreset

#define THREAD_TERMINATE kNtThreadTerminate
#define THREAD_SUSPEND_RESUME kNtThreadSuspendResume
#define THREAD_GET_CONTEXT kNtThreadGetContext
#define THREAD_SET_CONTEXT kNtThreadSetContext
#define THREAD_QUERY_INFORMATION kNtThreadQueryInformation
#define THREAD_SET_INFORMATION kNtThreadSetInformation
#define THREAD_SET_THREAD_TOKEN kNtThreadSetThreadToken
#define THREAD_IMPERSONATE kNtThreadImpersonate
#define THREAD_DIRECT_IMPERSONATION kNtThreadDirectImpersonation
#define THREAD_SET_LIMITED_INFORMATION kNtThreadSetLimitedInformation
#define THREAD_QUERY_LIMITED_INFORMATION kNtThreadQueryLimitedInformation
#define THREAD_RESUME kNtThreadResume
#define THREAD_ALL_ACCESS kNtThreadAllAccess

#define _FILE_SEGMENT_ELEMENT NtFileSegmentElement
#define FILE_SEGMENT_ELEMENT union NtFileSegmentElement
#define PFILE_SEGMENT_ELEMENT union NtFileSegmentElement*

#define FileBasicInfo kNtFileBasicInfo
#define FileStandardInfo kNtFileStandardInfo
#define FileNameInfo kNtFileNameInfo
#define FileRenameInfo kNtFileRenameInfo
#define FileDispositionInfo kNtFileDispositionInfo
#define FileAllocationInfo kNtFileAllocationInfo
#define FileEndOfFileInfo kNtFileEndOfFileInfo
#define FileStreamInfo kNtFileStreamInfo
#define FileCompressionInfo kNtFileCompressionInfo
#define FileAttributeTagInfo kNtFileAttributeTagInfo
#define FileIdBothDirectoryInfo kNtFileIdBothDirectoryInfo
#define FileIdBothDirectoryRestartInfo kNtFileIdBothDirectoryRestartInfo
#define FileIoPriorityHintInfo kNtFileIoPriorityHintInfo
#define FileRemoteProtocolInfo kNtFileRemoteProtocolInfo
#define FileFullDirectoryInfo kNtFileFullDirectoryInfo
#define FileFullDirectoryRestartInfo kNtFileFullDirectoryRestartInfo
#define FileStorageInfo kNtFileStorageInfo
#define FileAlignmentInfo kNtFileAlignmentInfo
#define FileIdInfo kNtFileIdInfo
#define FileIdExtdDirectoryInfo kNtFileIdExtdDirectoryInfo
#define FileIdExtdDirectoryRestartInfo kNtFileIdExtdDirectoryRestartInfo
#define FileDispositionInfoEx kNtFileDispositionInfoEx
#define FileRenameInfoEx kNtFileRenameInfoEx

#define _FILE_FULL_DIR_INFO NtFileFullDirectoryInformation
#define FILE_FULL_DIR_INFO struct NtFileFullDirectoryInformation
#define PFILE_FULL_DIR_INFO struct NtFileFullDirectoryInformation*

#define _FILE_BASIC_INFO NtFileBasicInformation
#define FILE_BASIC_INFO struct NtFileBasicInformation
#define PFILE_BASIC_INFO struct NtFileBasicInformation*

#define _FILE_STANDARD_INFO NtFileStandardInformation
#define FILE_STANDARD_INFO struct NtFileStandardInformation
#define PFILE_STANDARD_INFO struct NtFileStandardInformation*

#define HANDLE_FLAG_INHERIT kNtHandleFlagInherit
#define HANDLE_FLAG_PROTECT_FROM_CLOSE kNtHandleFlagProtectFromClose

#define SYMBOLIC_LINK_FLAG_DIRECTORY kNtSymbolicLinkFlagDirectory

#define NT_FORMAT_MESSAGE_ALLOCATE_BUFFER kNtFormatMessageAllocateBuffer
#define NT_FORMAT_MESSAGE_IGNORE_INSERTS kNtFormatMessageIgnoreInserts
#define NT_FORMAT_MESSAGE_FROM_STRING kNtFormatMessageFromString
#define NT_FORMAT_MESSAGE_FROM_HMODULE kNtFormatMessageFromHmodule
#define NT_FORMAT_MESSAGE_FROM_SYSTEM kNtFormatMessageFromSystem
#define NT_FORMAT_MESSAGE_ARGUMENT_ARRAY kNtFormatMessageArgumentArray
#define NT_FORMAT_MESSAGE_MAX_WIDTH_MASK kNtFormatMessageMaxWidthMask

#define THREAD_BASE_PRIORITY_IDLE kNtThreadBasePriorityIdle
#define THREAD_BASE_PRIORITY_MIN kNtThreadBasePriorityMin
#define THREAD_BASE_PRIORITY_MAX kNtThreadBasePriorityMax
#define THREAD_BASE_PRIORITY_LOWRT kNtThreadBasePriorityLowrt

#define THREAD_PRIORITY_IDLE kNtThreadPriorityIdle
#define THREAD_PRIORITY_LOWEST kNtThreadPriorityLowest
#define THREAD_PRIORITY_BELOW_NORMAL kNtThreadPriorityBelowNormal
#define THREAD_PRIORITY_NORMAL kNtThreadPriorityNormal
#define THREAD_PRIORITY_ABOVE_NORMAL kNtThreadPriorityAboveNormal
#define THREAD_PRIORITY_HIGHEST kNtThreadPriorityHighest
#define THREAD_PRIORITY_TIME_CRITICAL kNtThreadPriorityTimeCritical

#define ERROR_SUCCESS kNtErrorSuccess
#define ERROR_INVALID_FUNCTION kNtErrorInvalidFunction
#define ERROR_FILE_NOT_FOUND kNtErrorFileNotFound
#define ERROR_PATH_NOT_FOUND kNtErrorPathNotFound
#define ERROR_TOO_MANY_OPEN_FILES kNtErrorTooManyOpenFiles
#define ERROR_ACCESS_DENIED kNtErrorAccessDenied
#define ERROR_INVALID_HANDLE kNtErrorInvalidHandle
#define ERROR_ARENA_TRASHED kNtErrorArenaTrashed
#define ERROR_NOT_ENOUGH_MEMORY kNtErrorNotEnoughMemory
#define ERROR_INVALID_BLOCK kNtErrorInvalidBlock
#define ERROR_BAD_ENVIRONMENT kNtErrorBadEnvironment
#define ERROR_BAD_FORMAT kNtErrorBadFormat
#define ERROR_INVALID_ACCESS kNtErrorInvalidAccess
#define ERROR_INVALID_DATA kNtErrorInvalidData
#define ERROR_OUTOFMEMORY kNtErrorOutofmemory
#define ERROR_INVALID_DRIVE kNtErrorInvalidDrive
#define ERROR_CURRENT_DIRECTORY kNtErrorCurrentDirectory
#define ERROR_NOT_SAME_DEVICE kNtErrorNotSameDevice
#define ERROR_NO_MORE_FILES kNtErrorNoMoreFiles
#define ERROR_WRITE_PROTECT kNtErrorWriteProtect
#define ERROR_BAD_UNIT kNtErrorBadUnit
#define ERROR_NOT_READY kNtErrorNotReady
#define ERROR_BAD_COMMAND kNtErrorBadCommand
#define ERROR_CRC kNtErrorCrc
#define ERROR_BAD_LENGTH kNtErrorBadLength
#define ERROR_SEEK kNtErrorSeek
#define ERROR_NOT_DOS_DISK kNtErrorNotDosDisk
#define ERROR_SECTOR_NOT_FOUND kNtErrorSectorNotFound
#define ERROR_OUT_OF_PAPER kNtErrorOutOfPaper
#define ERROR_WRITE_FAULT kNtErrorWriteFault
#define ERROR_READ_FAULT kNtErrorReadFault
#define ERROR_GEN_FAILURE kNtErrorGenFailure
#define ERROR_SHARING_VIOLATION kNtErrorSharingViolation
#define ERROR_LOCK_VIOLATION kNtErrorLockViolation
#define ERROR_WRONG_DISK kNtErrorWrongDisk
#define ERROR_SHARING_BUFFER_EXCEEDED kNtErrorSharingBufferExceeded
#define ERROR_HANDLE_EOF kNtErrorHandleEof
#define ERROR_HANDLE_DISK_FULL kNtErrorHandleDiskFull
#define ERROR_NOT_SUPPORTED kNtErrorNotSupported
#define ERROR_REM_NOT_LIST kNtErrorRemNotList
#define ERROR_DUP_NAME kNtErrorDupName
#define ERROR_BAD_NETPATH kNtErrorBadNetpath
#define ERROR_NETWORK_BUSY kNtErrorNetworkBusy
#define ERROR_DEV_NOT_EXIST kNtErrorDevNotExist
#define ERROR_TOO_MANY_CMDS kNtErrorTooManyCmds
#define ERROR_ADAP_HDW_ERR kNtErrorAdapHdwErr
#define ERROR_BAD_NET_RESP kNtErrorBadNetResp
#define ERROR_UNEXP_NET_ERR kNtErrorUnexpNetErr
#define ERROR_BAD_REM_ADAP kNtErrorBadRemAdap
#define ERROR_PRINTQ_FULL kNtErrorPrintqFull
#define ERROR_NO_SPOOL_SPACE kNtErrorNoSpoolSpace
#define ERROR_PRINT_CANCELLED kNtErrorPrintCancelled
#define ERROR_NETNAME_DELETED kNtErrorNetnameDeleted
#define ERROR_NETWORK_ACCESS_DENIED kNtErrorNetworkAccessDenied
#define ERROR_BAD_DEV_TYPE kNtErrorBadDevType
#define ERROR_BAD_NET_NAME kNtErrorBadNetName
#define ERROR_TOO_MANY_NAMES kNtErrorTooManyNames
#define ERROR_TOO_MANY_SESS kNtErrorTooManySess
#define ERROR_SHARING_PAUSED kNtErrorSharingPaused
#define ERROR_REQ_NOT_ACCEP kNtErrorReqNotAccep
#define ERROR_REDIR_PAUSED kNtErrorRedirPaused
#define ERROR_FILE_EXISTS kNtErrorFileExists
#define ERROR_CANNOT_MAKE kNtErrorCannotMake
#define ERROR_FAIL_I24 kNtErrorFailI24
#define ERROR_OUT_OF_STRUCTURES kNtErrorOutOfStructures
#define ERROR_ALREADY_ASSIGNED kNtErrorAlreadyAssigned
#define ERROR_INVALID_PASSWORD kNtErrorInvalidPassword
#define ERROR_INVALID_PARAMETER kNtErrorInvalidParameter
#define ERROR_NET_WRITE_FAULT kNtErrorNetWriteFault
#define ERROR_NO_PROC_SLOTS kNtErrorNoProcSlots
#define ERROR_TOO_MANY_SEMAPHORES kNtErrorTooManySemaphores
#define ERROR_EXCL_SEM_ALREADY_OWNED kNtErrorExclSemAlreadyOwned
#define ERROR_SEM_IS_SET kNtErrorSemIsSet
#define ERROR_TOO_MANY_SEM_REQUESTS kNtErrorTooManySemRequests
#define ERROR_INVALID_AT_INTERRUPT_TIME kNtErrorInvalidAtInterruptTime
#define ERROR_SEM_OWNER_DIED kNtErrorSemOwnerDied
#define ERROR_SEM_USER_LIMIT kNtErrorSemUserLimit
#define ERROR_DISK_CHANGE kNtErrorDiskChange
#define ERROR_DRIVE_LOCKED kNtErrorDriveLocked
#define ERROR_BROKEN_PIPE kNtErrorBrokenPipe
#define ERROR_OPEN_FAILED kNtErrorOpenFailed
#define ERROR_BUFFER_OVERFLOW kNtErrorBufferOverflow
#define ERROR_DISK_FULL kNtErrorDiskFull
#define ERROR_NO_MORE_SEARCH_HANDLES kNtErrorNoMoreSearchHandles
#define ERROR_INVALID_TARGET_HANDLE kNtErrorInvalidTargetHandle
#define ERROR_INVALID_CATEGORY kNtErrorInvalidCategory
#define ERROR_INVALID_VERIFY_SWITCH kNtErrorInvalidVerifySwitch
#define ERROR_BAD_DRIVER_LEVEL kNtErrorBadDriverLevel
#define ERROR_CALL_NOT_IMPLEMENTED kNtErrorCallNotImplemented
#define ERROR_SEM_TIMEOUT kNtErrorSemTimeout
#define ERROR_INSUFFICIENT_BUFFER kNtErrorInsufficientBuffer
#define ERROR_INVALID_NAME kNtErrorInvalidName
#define ERROR_INVALID_LEVEL kNtErrorInvalidLevel
#define ERROR_NO_VOLUME_LABEL kNtErrorNoVolumeLabel
#define ERROR_MOD_NOT_FOUND kNtErrorModNotFound
#define ERROR_PROC_NOT_FOUND kNtErrorProcNotFound
#define ERROR_WAIT_NO_CHILDREN kNtErrorWaitNoChildren
#define ERROR_CHILD_NOT_COMPLETE kNtErrorChildNotComplete
#define ERROR_DIRECT_ACCESS_HANDLE kNtErrorDirectAccessHandle
#define ERROR_NEGATIVE_SEEK kNtErrorNegativeSeek
#define ERROR_SEEK_ON_DEVICE kNtErrorSeekOnDevice
#define ERROR_IS_JOIN_TARGET kNtErrorIsJoinTarget
#define ERROR_IS_JOINED kNtErrorIsJoined
#define ERROR_IS_SUBSTED kNtErrorIsSubsted
#define ERROR_NOT_JOINED kNtErrorNotJoined
#define ERROR_NOT_SUBSTED kNtErrorNotSubsted
#define ERROR_JOIN_TO_JOIN kNtErrorJoinToJoin
#define ERROR_SUBST_TO_SUBST kNtErrorSubstToSubst
#define ERROR_JOIN_TO_SUBST kNtErrorJoinToSubst
#define ERROR_SUBST_TO_JOIN kNtErrorSubstToJoin
#define ERROR_BUSY_DRIVE kNtErrorBusyDrive
#define ERROR_SAME_DRIVE kNtErrorSameDrive
#define ERROR_DIR_NOT_ROOT kNtErrorDirNotRoot
#define ERROR_DIR_NOT_EMPTY kNtErrorDirNotEmpty
#define ERROR_IS_SUBST_PATH kNtErrorIsSubstPath
#define ERROR_IS_JOIN_PATH kNtErrorIsJoinPath
#define ERROR_PATH_BUSY kNtErrorPathBusy
#define ERROR_IS_SUBST_TARGET kNtErrorIsSubstTarget
#define ERROR_SYSTEM_TRACE kNtErrorSystemTrace
#define ERROR_INVALID_EVENT_COUNT kNtErrorInvalidEventCount
#define ERROR_TOO_MANY_MUXWAITERS kNtErrorTooManyMuxwaiters
#define ERROR_INVALID_LIST_FORMAT kNtErrorInvalidListFormat
#define ERROR_LABEL_TOO_LONG kNtErrorLabelTooLong
#define ERROR_TOO_MANY_TCBS kNtErrorTooManyTcbs
#define ERROR_SIGNAL_REFUSED kNtErrorSignalRefused
#define ERROR_DISCARDED kNtErrorDiscarded
#define ERROR_NOT_LOCKED kNtErrorNotLocked
#define ERROR_BAD_THREADID_ADDR kNtErrorBadThreadidAddr
#define ERROR_BAD_ARGUMENTS kNtErrorBadArguments
#define ERROR_BAD_PATHNAME kNtErrorBadPathname
#define ERROR_SIGNAL_PENDING kNtErrorSignalPending
#define ERROR_MAX_THRDS_REACHED kNtErrorMaxThrdsReached
#define ERROR_LOCK_FAILED kNtErrorLockFailed
#define ERROR_BUSY kNtErrorBusy
#define ERROR_DEVICE_SUPPORT_IN_PROGRESS kNtErrorDeviceSupportInProgress
#define ERROR_CANCEL_VIOLATION kNtErrorCancelViolation
#define ERROR_ATOMIC_LOCKS_NOT_SUPPORTED kNtErrorAtomicLocksNotSupported
#define ERROR_INVALID_SEGMENT_NUMBER kNtErrorInvalidSegmentNumber
#define ERROR_INVALID_ORDINAL kNtErrorInvalidOrdinal
#define ERROR_ALREADY_EXISTS kNtErrorAlreadyExists
#define ERROR_INVALID_FLAG_NUMBER kNtErrorInvalidFlagNumber
#define ERROR_SEM_NOT_FOUND kNtErrorSemNotFound
#define ERROR_INVALID_STARTING_CODESEG kNtErrorInvalidStartingCodeseg
#define ERROR_INVALID_STACKSEG kNtErrorInvalidStackseg
#define ERROR_INVALID_MODULETYPE kNtErrorInvalidModuletype
#define ERROR_INVALID_EXE_SIGNATURE kNtErrorInvalidExeSignature
#define ERROR_EXE_MARKED_INVALID kNtErrorExeMarkedInvalid
#define ERROR_BAD_EXE_FORMAT kNtErrorBadExeFormat
#define ERROR_INVALID_MINALLOCSIZE kNtErrorInvalidMinallocsize
#define ERROR_DYNLINK_FROM_INVALID_RING kNtErrorDynlinkFromInvalidRing
#define ERROR_IOPL_NOT_ENABLED kNtErrorIoplNotEnabled
#define ERROR_INVALID_SEGDPL kNtErrorInvalidSegdpl
#define ERROR_RING2SEG_MUST_BE_MOVABLE kNtErrorRing2segMustBeMovable
#define ERROR_RELOC_CHAIN_XEEDS_SEGLIM kNtErrorRelocChainXeedsSeglim
#define ERROR_INFLOOP_IN_RELOC_CHAIN kNtErrorInfloopInRelocChain
#define ERROR_ENVVAR_NOT_FOUND kNtErrorEnvvarNotFound
#define ERROR_NO_SIGNAL_SENT kNtErrorNoSignalSent
#define ERROR_FILENAME_EXCED_RANGE kNtErrorFilenameExcedRange
#define ERROR_RING2_STACK_IN_USE kNtErrorRing2StackInUse
#define ERROR_META_EXPANSION_TOO_LONG kNtErrorMetaExpansionTooLong
#define ERROR_INVALID_SIGNAL_NUMBER kNtErrorInvalidSignalNumber
#define ERROR_LOCKED kNtErrorLocked
#define ERROR_TOO_MANY_MODULES kNtErrorTooManyModules
#define ERROR_NESTING_NOT_ALLOWED kNtErrorNestingNotAllowed
#define ERROR_EXE_MACHINE_TYPE_MISMATCH kNtErrorExeMachineTypeMismatch
#define ERROR_EXE_CANNOT_MODIFY_SIGNED_BINARY kNtErrorExeCannotModifySignedBinary
#define ERROR_EXE_CANNOT_MODIFY_STRONG_SIGNED_BINARY kNtErrorExeCannotModifyStrongSignedBinary
#define ERROR_FILE_CHECKED_OUT kNtErrorFileCheckedOut
#define ERROR_CHECKOUT_REQUIRED kNtErrorCheckoutRequired
#define ERROR_BAD_FILE_TYPE kNtErrorBadFileType
#define ERROR_FILE_TOO_LARGE kNtErrorFileTooLarge
#define ERROR_FORMS_AUTH_REQUIRED kNtErrorFormsAuthRequired
#define ERROR_VIRUS_INFECTED kNtErrorVirusInfected
#define ERROR_VIRUS_DELETED kNtErrorVirusDeleted
#define ERROR_PIPE_LOCAL kNtErrorPipeLocal
#define ERROR_BAD_PIPE kNtErrorBadPipe
#define ERROR_PIPE_BUSY kNtErrorPipeBusy
#define ERROR_NO_DATA kNtErrorNoData
#define ERROR_PIPE_NOT_CONNECTED kNtErrorPipeNotConnected
#define ERROR_MORE_DATA kNtErrorMoreData
#define ERROR_NO_WORK_DONE kNtErrorNoWorkDone
#define ERROR_VC_DISCONNECTED kNtErrorVcDisconnected
#define ERROR_INVALID_EA_NAME kNtErrorInvalidEaName
#define ERROR_EA_LIST_INCONSISTENT kNtErrorEaListInconsistent
#define ERROR_NO_MORE_ITEMS kNtErrorNoMoreItems
#define ERROR_CANNOT_COPY kNtErrorCannotCopy
#define ERROR_DIRECTORY kNtErrorDirectory
#define ERROR_EAS_DIDNT_FIT kNtErrorEasDidntFit
#define ERROR_EA_FILE_CORRUPT kNtErrorEaFileCorrupt
#define ERROR_EA_TABLE_FULL kNtErrorEaTableFull
#define ERROR_INVALID_EA_HANDLE kNtErrorInvalidEaHandle
#define ERROR_EAS_NOT_SUPPORTED kNtErrorEasNotSupported
#define ERROR_NOT_OWNER kNtErrorNotOwner
#define ERROR_TOO_MANY_POSTS kNtErrorTooManyPosts
#define ERROR_PARTIAL_COPY kNtErrorPartialCopy
#define ERROR_OPLOCK_NOT_GRANTED kNtErrorOplockNotGranted
#define ERROR_INVALID_OPLOCK_PROTOCOL kNtErrorInvalidOplockProtocol
#define ERROR_DISK_TOO_FRAGMENTED kNtErrorDiskTooFragmented
#define ERROR_DELETE_PENDING kNtErrorDeletePending
#define ERROR_INCOMPATIBLE_WITH_GLOBAL_SHORT_NAME_REGISTRY_SETTING kNtErrorIncompatibleWithGlobalShortNameRegistrySetting
#define ERROR_SHORT_NAMES_NOT_ENABLED_ON_VOLUME kNtErrorShortNamesNotEnabledOnVolume
#define ERROR_SECURITY_STREAM_IS_INCONSISTENT kNtErrorSecurityStreamIsInconsistent
#define ERROR_INVALID_LOCK_RANGE kNtErrorInvalidLockRange
#define ERROR_IMAGE_SUBSYSTEM_NOT_PRESENT kNtErrorImageSubsystemNotPresent
#define ERROR_NOTIFICATION_GUID_ALREADY_DEFINED kNtErrorNotificationGuidAlreadyDefined
#define ERROR_INVALID_EXCEPTION_HANDLER kNtErrorInvalidExceptionHandler
#define ERROR_DUPLICATE_PRIVILEGES kNtErrorDuplicatePrivileges
#define ERROR_NO_RANGES_PROCESSED kNtErrorNoRangesProcessed
#define ERROR_NOT_ALLOWED_ON_SYSTEM_FILE kNtErrorNotAllowedOnSystemFile
#define ERROR_DISK_RESOURCES_EXHAUSTED kNtErrorDiskResourcesExhausted
#define ERROR_INVALID_TOKEN kNtErrorInvalidToken
#define ERROR_DEVICE_FEATURE_NOT_SUPPORTED kNtErrorDeviceFeatureNotSupported
#define ERROR_MR_MID_NOT_FOUND kNtErrorMrMidNotFound
#define ERROR_SCOPE_NOT_FOUND kNtErrorScopeNotFound
#define ERROR_UNDEFINED_SCOPE kNtErrorUndefinedScope
#define ERROR_INVALID_CAP kNtErrorInvalidCap
#define ERROR_DEVICE_UNREACHABLE kNtErrorDeviceUnreachable
#define ERROR_DEVICE_NO_RESOURCES kNtErrorDeviceNoResources
#define ERROR_DATA_CHECKSUM_ERROR kNtErrorDataChecksumError
#define ERROR_INTERMIXED_KERNEL_EA_OPERATION kNtErrorIntermixedKernelEaOperation
#define ERROR_FILE_LEVEL_TRIM_NOT_SUPPORTED kNtErrorFileLevelTrimNotSupported
#define ERROR_OFFSET_ALIGNMENT_VIOLATION kNtErrorOffsetAlignmentViolation
#define ERROR_INVALID_FIELD_IN_PARAMETER_LIST kNtErrorInvalidFieldInParameterList
#define ERROR_OPERATION_IN_PROGRESS kNtErrorOperationInProgress
#define ERROR_BAD_DEVICE_PATH kNtErrorBadDevicePath
#define ERROR_TOO_MANY_DESCRIPTORS kNtErrorTooManyDescriptors
#define ERROR_SCRUB_DATA_DISABLED kNtErrorScrubDataDisabled
#define ERROR_NOT_REDUNDANT_STORAGE kNtErrorNotRedundantStorage
#define ERROR_RESIDENT_FILE_NOT_SUPPORTED kNtErrorResidentFileNotSupported
#define ERROR_COMPRESSED_FILE_NOT_SUPPORTED kNtErrorCompressedFileNotSupported
#define ERROR_DIRECTORY_NOT_SUPPORTED kNtErrorDirectoryNotSupported
#define ERROR_NOT_READ_FROM_COPY kNtErrorNotReadFromCopy
#define ERROR_FT_WRITE_FAILURE kNtErrorFtWriteFailure
#define ERROR_FT_DI_SCAN_REQUIRED kNtErrorFtDiScanRequired
#define ERROR_INVALID_KERNEL_INFO_VERSION kNtErrorInvalidKernelInfoVersion
#define ERROR_INVALID_PEP_INFO_VERSION kNtErrorInvalidPepInfoVersion
#define ERROR_OBJECT_NOT_EXTERNALLY_BACKED kNtErrorObjectNotExternallyBacked
#define ERROR_EXTERNAL_BACKING_PROVIDER_UNKNOWN kNtErrorExternalBackingProviderUnknown
#define ERROR_COMPRESSION_NOT_BENEFICIAL kNtErrorCompressionNotBeneficial
#define ERROR_STORAGE_TOPOLOGY_ID_MISMATCH kNtErrorStorageTopologyIdMismatch
#define ERROR_BLOCKED_BY_PARENTAL_CONTROLS kNtErrorBlockedByParentalControls
#define ERROR_BLOCK_TOO_MANY_REFERENCES kNtErrorBlockTooManyReferences
#define ERROR_MARKED_TO_DISALLOW_WRITES kNtErrorMarkedToDisallowWrites
#define ERROR_ENCLAVE_FAILURE kNtErrorEnclaveFailure
#define ERROR_FAIL_NOACTION_REBOOT kNtErrorFailNoactionReboot
#define ERROR_FAIL_SHUTDOWN kNtErrorFailShutdown
#define ERROR_FAIL_RESTART kNtErrorFailRestart
#define ERROR_MAX_SESSIONS_REACHED kNtErrorMaxSessionsReached
#define ERROR_NETWORK_ACCESS_DENIED_EDP kNtErrorNetworkAccessDeniedEdp
#define ERROR_DEVICE_HINT_NAME_BUFFER_TOO_SMALL kNtErrorDeviceHintNameBufferTooSmall
#define ERROR_EDP_POLICY_DENIES_OPERATION kNtErrorEdpPolicyDeniesOperation
#define ERROR_EDP_DPL_POLICY_CANT_BE_SATISFIED kNtErrorEdpDplPolicyCantBeSatisfied
#define ERROR_CLOUD_FILE_SYNC_ROOT_METADATA_CORRUPT kNtErrorCloudFileSyncRootMetadataCorrupt
#define ERROR_DEVICE_IN_MAINTENANCE kNtErrorDeviceInMaintenance
#define ERROR_NOT_SUPPORTED_ON_DAX kNtErrorNotSupportedOnDax
#define ERROR_DAX_MAPPING_EXISTS kNtErrorDaxMappingExists
#define ERROR_CLOUD_FILE_PROVIDER_NOT_RUNNING kNtErrorCloudFileProviderNotRunning
#define ERROR_CLOUD_FILE_METADATA_CORRUPT kNtErrorCloudFileMetadataCorrupt
#define ERROR_CLOUD_FILE_METADATA_TOO_LARGE kNtErrorCloudFileMetadataTooLarge
#define ERROR_CLOUD_FILE_PROPERTY_BLOB_TOO_LARGE kNtErrorCloudFilePropertyBlobTooLarge
#define ERROR_CLOUD_FILE_PROPERTY_BLOB_CHECKSUM_MISMATCH kNtErrorCloudFilePropertyBlobChecksumMismatch
#define ERROR_CHILD_PROCESS_BLOCKED kNtErrorChildProcessBlocked
#define ERROR_STORAGE_LOST_DATA_PERSISTENCE kNtErrorStorageLostDataPersistence
#define ERROR_FILE_SYSTEM_VIRTUALIZATION_UNAVAILABLE kNtErrorFileSystemVirtualizationUnavailable
#define ERROR_FILE_SYSTEM_VIRTUALIZATION_METADATA_CORRUPT kNtErrorFileSystemVirtualizationMetadataCorrupt
#define ERROR_FILE_SYSTEM_VIRTUALIZATION_BUSY kNtErrorFileSystemVirtualizationBusy
#define ERROR_FILE_SYSTEM_VIRTUALIZATION_PROVIDER_UNKNOWN kNtErrorFileSystemVirtualizationProviderUnknown
#define ERROR_GDI_HANDLE_LEAK kNtErrorGdiHandleLeak
#define ERROR_CLOUD_FILE_TOO_MANY_PROPERTY_BLOBS kNtErrorCloudFileTooManyPropertyBlobs
#define ERROR_CLOUD_FILE_PROPERTY_VERSION_NOT_SUPPORTED kNtErrorCloudFilePropertyVersionNotSupported
#define ERROR_NOT_ACLOUD_FILE kNtErrorNotACloudFile
#define ERROR_CLOUD_FILE_NOT_IN_SYNC kNtErrorCloudFileNotInSync
#define ERROR_CLOUD_FILE_ALREADY_CONNECTED kNtErrorCloudFileAlreadyConnected
#define ERROR_CLOUD_FILE_NOT_SUPPORTED kNtErrorCloudFileNotSupported
#define ERROR_CLOUD_FILE_INVALID_REQUEST kNtErrorCloudFileInvalidRequest
#define ERROR_CLOUD_FILE_READ_ONLY_VOLUME kNtErrorCloudFileReadOnlyVolume
#define ERROR_CLOUD_FILE_CONNECTED_PROVIDER_ONLY kNtErrorCloudFileConnectedProviderOnly
#define ERROR_CLOUD_FILE_VALIDATION_FAILED kNtErrorCloudFileValidationFailed
#define ERROR_SMB1_NOT_AVAILABLE kNtErrorSmb1NotAvailable
#define ERROR_FILE_SYSTEM_VIRTUALIZATION_INVALID_OPERATION kNtErrorFileSystemVirtualizationInvalidOperation
#define ERROR_CLOUD_FILE_AUTHENTICATION_FAILED kNtErrorCloudFileAuthenticationFailed
#define ERROR_CLOUD_FILE_INSUFFICIENT_RESOURCES kNtErrorCloudFileInsufficientResources
#define ERROR_CLOUD_FILE_NETWORK_UNAVAILABLE kNtErrorCloudFileNetworkUnavailable
#define ERROR_CLOUD_FILE_UNSUCCESSFUL kNtErrorCloudFileUnsuccessful
#define ERROR_CLOUD_FILE_NOT_UNDER_SYNC_ROOT kNtErrorCloudFileNotUnderSyncRoot
#define ERROR_CLOUD_FILE_IN_USE kNtErrorCloudFileInUse
#define ERROR_CLOUD_FILE_PINNED kNtErrorCloudFilePinned
#define ERROR_CLOUD_FILE_REQUEST_ABORTED kNtErrorCloudFileRequestAborted
#define ERROR_CLOUD_FILE_PROPERTY_CORRUPT kNtErrorCloudFilePropertyCorrupt
#define ERROR_CLOUD_FILE_ACCESS_DENIED kNtErrorCloudFileAccessDenied
#define ERROR_CLOUD_FILE_INCOMPATIBLE_HARDLINKS kNtErrorCloudFileIncompatibleHardlinks
#define ERROR_CLOUD_FILE_PROPERTY_LOCK_CONFLICT kNtErrorCloudFilePropertyLockConflict
#define ERROR_CLOUD_FILE_REQUEST_CANCELED kNtErrorCloudFileRequestCanceled
#define ERROR_EXTERNAL_SYSKEY_NOT_SUPPORTED kNtErrorExternalSyskeyNotSupported
#define ERROR_THREAD_MODE_ALREADY_BACKGROUND kNtErrorThreadModeAlreadyBackground
#define ERROR_THREAD_MODE_NOT_BACKGROUND kNtErrorThreadModeNotBackground
#define ERROR_PROCESS_MODE_ALREADY_BACKGROUND kNtErrorProcessModeAlreadyBackground
#define ERROR_PROCESS_MODE_NOT_BACKGROUND kNtErrorProcessModeNotBackground
#define ERROR_CLOUD_FILE_PROVIDER_TERMINATED kNtErrorCloudFileProviderTerminated
#define ERROR_NOT_ACLOUD_SYNC_ROOT kNtErrorNotACloudSyncRoot
#define ERROR_FILE_PROTECTED_UNDER_DPL kNtErrorFileProtectedUnderDpl
#define ERROR_VOLUME_NOT_CLUSTER_ALIGNED kNtErrorVolumeNotClusterAligned
#define ERROR_NO_PHYSICALLY_ALIGNED_FREE_SPACE_FOUND kNtErrorNoPhysicallyAlignedFreeSpaceFound
#define ERROR_APPX_FILE_NOT_ENCRYPTED kNtErrorAppxFileNotEncrypted
#define ERROR_RWRAW_ENCRYPTED_FILE_NOT_ENCRYPTED kNtErrorRwrawEncryptedFileNotEncrypted
#define ERROR_RWRAW_ENCRYPTED_INVALID_EDATAINFO_FILEOFFSET kNtErrorRwrawEncryptedInvalidEdatainfoFileoffset
#define ERROR_RWRAW_ENCRYPTED_INVALID_EDATAINFO_FILERANGE kNtErrorRwrawEncryptedInvalidEdatainfoFilerange
#define ERROR_RWRAW_ENCRYPTED_INVALID_EDATAINFO_PARAMETER kNtErrorRwrawEncryptedInvalidEdatainfoParameter
#define ERROR_LINUX_SUBSYSTEM_NOT_PRESENT kNtErrorLinuxSubsystemNotPresent
#define ERROR_CAPAUTHZ_NOT_DEVUNLOCKED kNtErrorCapauthzNotDevunlocked
#define ERROR_CAPAUTHZ_CHANGE_TYPE kNtErrorCapauthzChangeType
#define ERROR_CAPAUTHZ_NOT_PROVISIONED kNtErrorCapauthzNotProvisioned
#define ERROR_CAPAUTHZ_NOT_AUTHORIZED kNtErrorCapauthzNotAuthorized
#define ERROR_CAPAUTHZ_NO_POLICY kNtErrorCapauthzNoPolicy
#define ERROR_CAPAUTHZ_DB_CORRUPTED kNtErrorCapauthzDbCorrupted
#define ERROR_CAPAUTHZ_SCCD_INVALID_CATALOG kNtErrorCapauthzSccdInvalidCatalog
#define ERROR_CAPAUTHZ_SCCD_NO_AUTH_ENTITY kNtErrorCapauthzSccdNoAuthEntity
#define ERROR_CAPAUTHZ_SCCD_PARSE_ERROR kNtErrorCapauthzSccdParseError
#define ERROR_CAPAUTHZ_SCCD_DEV_MODE_REQUIRED kNtErrorCapauthzSccdDevModeRequired
#define ERROR_CAPAUTHZ_SCCD_NO_CAPABILITY_MATCH kNtErrorCapauthzSccdNoCapabilityMatch
#define ERROR_PNP_QUERY_REMOVE_DEVICE_TIMEOUT kNtErrorPnpQueryRemoveDeviceTimeout
#define ERROR_PNP_QUERY_REMOVE_RELATED_DEVICE_TIMEOUT kNtErrorPnpQueryRemoveRelatedDeviceTimeout
#define ERROR_PNP_QUERY_REMOVE_UNRELATED_DEVICE_TIMEOUT kNtErrorPnpQueryRemoveUnrelatedDeviceTimeout
#define ERROR_DEVICE_HARDWARE_ERROR kNtErrorDeviceHardwareError
#define ERROR_INVALID_ADDRESS kNtErrorInvalidAddress
#define ERROR_VRF_CFG_ENABLED kNtErrorVrfCfgEnabled
#define ERROR_PARTITION_TERMINATING kNtErrorPartitionTerminating
#define ERROR_USER_PROFILE_LOAD kNtErrorUserProfileLoad
#define ERROR_ARITHMETIC_OVERFLOW kNtErrorArithmeticOverflow
#define ERROR_PIPE_CONNECTED kNtErrorPipeConnected
#define ERROR_PIPE_LISTENING kNtErrorPipeListening
#define ERROR_VERIFIER_STOP kNtErrorVerifierStop
#define ERROR_ABIOS_ERROR kNtErrorAbiosError
#define ERROR_WX86_WARNING kNtErrorWx86Warning
#define ERROR_WX86_ERROR kNtErrorWx86Error
#define ERROR_TIMER_NOT_CANCELED kNtErrorTimerNotCanceled
#define ERROR_UNWIND kNtErrorUnwind
#define ERROR_BAD_STACK kNtErrorBadStack
#define ERROR_INVALID_UNWIND_TARGET kNtErrorInvalidUnwindTarget
#define ERROR_INVALID_PORT_ATTRIBUTES kNtErrorInvalidPortAttributes
#define ERROR_PORT_MESSAGE_TOO_LONG kNtErrorPortMessageTooLong
#define ERROR_INVALID_QUOTA_LOWER kNtErrorInvalidQuotaLower
#define ERROR_DEVICE_ALREADY_ATTACHED kNtErrorDeviceAlreadyAttached
#define ERROR_INSTRUCTION_MISALIGNMENT kNtErrorInstructionMisalignment
#define ERROR_PROFILING_NOT_STARTED kNtErrorProfilingNotStarted
#define ERROR_PROFILING_NOT_STOPPED kNtErrorProfilingNotStopped
#define ERROR_COULD_NOT_INTERPRET kNtErrorCouldNotInterpret
#define ERROR_PROFILING_AT_LIMIT kNtErrorProfilingAtLimit
#define ERROR_CANT_WAIT kNtErrorCantWait
#define ERROR_CANT_TERMINATE_SELF kNtErrorCantTerminateSelf
#define ERROR_UNEXPECTED_MM_CREATE_ERR kNtErrorUnexpectedMmCreateErr
#define ERROR_UNEXPECTED_MM_MAP_ERROR kNtErrorUnexpectedMmMapError
#define ERROR_UNEXPECTED_MM_EXTEND_ERR kNtErrorUnexpectedMmExtendErr
#define ERROR_BAD_FUNCTION_TABLE kNtErrorBadFunctionTable
#define ERROR_NO_GUID_TRANSLATION kNtErrorNoGuidTranslation
#define ERROR_INVALID_LDT_SIZE kNtErrorInvalidLdtSize
#define ERROR_INVALID_LDT_OFFSET kNtErrorInvalidLdtOffset
#define ERROR_INVALID_LDT_DESCRIPTOR kNtErrorInvalidLdtDescriptor
#define ERROR_TOO_MANY_THREADS kNtErrorTooManyThreads
#define ERROR_THREAD_NOT_IN_PROCESS kNtErrorThreadNotInProcess
#define ERROR_PAGEFILE_QUOTA_EXCEEDED kNtErrorPagefileQuotaExceeded
#define ERROR_LOGON_SERVER_CONFLICT kNtErrorLogonServerConflict
#define ERROR_SYNCHRONIZATION_REQUIRED kNtErrorSynchronizationRequired
#define ERROR_NET_OPEN_FAILED kNtErrorNetOpenFailed
#define ERROR_IO_PRIVILEGE_FAILED kNtErrorIoPrivilegeFailed
#define ERROR_CONTROL_CEXIT kNtErrorControlCExit
#define ERROR_MISSING_SYSTEMFILE kNtErrorMissingSystemfile
#define ERROR_UNHANDLED_EXCEPTION kNtErrorUnhandledException
#define ERROR_APP_INIT_FAILURE kNtErrorAppInitFailure
#define ERROR_PAGEFILE_CREATE_FAILED kNtErrorPagefileCreateFailed
#define ERROR_INVALID_IMAGE_HASH kNtErrorInvalidImageHash
#define ERROR_NO_PAGEFILE kNtErrorNoPagefile
#define ERROR_ILLEGAL_FLOAT_CONTEXT kNtErrorIllegalFloatContext
#define ERROR_NO_EVENT_PAIR kNtErrorNoEventPair
#define ERROR_DOMAIN_CTRLR_CONFIG_ERROR kNtErrorDomainCtrlrConfigError
#define ERROR_ILLEGAL_CHARACTER kNtErrorIllegalCharacter
#define ERROR_UNDEFINED_CHARACTER kNtErrorUndefinedCharacter
#define ERROR_FLOPPY_VOLUME kNtErrorFloppyVolume
#define ERROR_BIOS_FAILED_TO_CONNECT_INTERRUPT kNtErrorBiosFailedToConnectInterrupt
#define ERROR_BACKUP_CONTROLLER kNtErrorBackupController
#define ERROR_MUTANT_LIMIT_EXCEEDED kNtErrorMutantLimitExceeded
#define ERROR_FS_DRIVER_REQUIRED kNtErrorFsDriverRequired
#define ERROR_CANNOT_LOAD_REGISTRY_FILE kNtErrorCannotLoadRegistryFile
#define ERROR_DEBUG_ATTACH_FAILED kNtErrorDebugAttachFailed
#define ERROR_SYSTEM_PROCESS_TERMINATED kNtErrorSystemProcessTerminated
#define ERROR_DATA_NOT_ACCEPTED kNtErrorDataNotAccepted
#define ERROR_VDM_HARD_ERROR kNtErrorVdmHardError
#define ERROR_DRIVER_CANCEL_TIMEOUT kNtErrorDriverCancelTimeout
#define ERROR_REPLY_MESSAGE_MISMATCH kNtErrorReplyMessageMismatch
#define ERROR_LOST_WRITEBEHIND_DATA kNtErrorLostWritebehindData
#define ERROR_CLIENT_SERVER_PARAMETERS_INVALID kNtErrorClientServerParametersInvalid
#define ERROR_NOT_TINY_STREAM kNtErrorNotTinyStream
#define ERROR_STACK_OVERFLOW_READ kNtErrorStackOverflowRead
#define ERROR_CONVERT_TO_LARGE kNtErrorConvertToLarge
#define ERROR_FOUND_OUT_OF_SCOPE kNtErrorFoundOutOfScope
#define ERROR_ALLOCATE_BUCKET kNtErrorAllocateBucket
#define ERROR_MARSHALL_OVERFLOW kNtErrorMarshallOverflow
#define ERROR_INVALID_VARIANT kNtErrorInvalidVariant
#define ERROR_BAD_COMPRESSION_BUFFER kNtErrorBadCompressionBuffer
#define ERROR_AUDIT_FAILED kNtErrorAuditFailed
#define ERROR_TIMER_RESOLUTION_NOT_SET kNtErrorTimerResolutionNotSet
#define ERROR_INSUFFICIENT_LOGON_INFO kNtErrorInsufficientLogonInfo
#define ERROR_BAD_DLL_ENTRYPOINT kNtErrorBadDllEntrypoint
#define ERROR_BAD_SERVICE_ENTRYPOINT kNtErrorBadServiceEntrypoint
#define ERROR_IP_ADDRESS_CONFLICT1 kNtErrorIpAddressConflict1
#define ERROR_IP_ADDRESS_CONFLICT2 kNtErrorIpAddressConflict2
#define ERROR_REGISTRY_QUOTA_LIMIT kNtErrorRegistryQuotaLimit
#define ERROR_NO_CALLBACK_ACTIVE kNtErrorNoCallbackActive
#define ERROR_PWD_TOO_SHORT kNtErrorPwdTooShort
#define ERROR_PWD_TOO_RECENT kNtErrorPwdTooRecent
#define ERROR_PWD_HISTORY_CONFLICT kNtErrorPwdHistoryConflict
#define ERROR_UNSUPPORTED_COMPRESSION kNtErrorUnsupportedCompression
#define ERROR_INVALID_HW_PROFILE kNtErrorInvalidHwProfile
#define ERROR_INVALID_PLUGPLAY_DEVICE_PATH kNtErrorInvalidPlugplayDevicePath
#define ERROR_QUOTA_LIST_INCONSISTENT kNtErrorQuotaListInconsistent
#define ERROR_EVALUATION_EXPIRATION kNtErrorEvaluationExpiration
#define ERROR_ILLEGAL_DLL_RELOCATION kNtErrorIllegalDllRelocation
#define ERROR_DLL_INIT_FAILED_LOGOFF kNtErrorDllInitFailedLogoff
#define ERROR_VALIDATE_CONTINUE kNtErrorValidateContinue
#define ERROR_NO_MORE_MATCHES kNtErrorNoMoreMatches
#define ERROR_RANGE_LIST_CONFLICT kNtErrorRangeListConflict
#define ERROR_SERVER_SID_MISMATCH kNtErrorServerSidMismatch
#define ERROR_CANT_ENABLE_DENY_ONLY kNtErrorCantEnableDenyOnly
#define ERROR_FLOAT_MULTIPLE_FAULTS kNtErrorFloatMultipleFaults
#define ERROR_FLOAT_MULTIPLE_TRAPS kNtErrorFloatMultipleTraps
#define ERROR_NOINTERFACE kNtErrorNointerface
#define ERROR_DRIVER_FAILED_SLEEP kNtErrorDriverFailedSleep
#define ERROR_CORRUPT_SYSTEM_FILE kNtErrorCorruptSystemFile
#define ERROR_COMMITMENT_MINIMUM kNtErrorCommitmentMinimum
#define ERROR_PNP_RESTART_ENUMERATION kNtErrorPnpRestartEnumeration
#define ERROR_SYSTEM_IMAGE_BAD_SIGNATURE kNtErrorSystemImageBadSignature
#define ERROR_PNP_REBOOT_REQUIRED kNtErrorPnpRebootRequired
#define ERROR_INSUFFICIENT_POWER kNtErrorInsufficientPower
#define ERROR_MULTIPLE_FAULT_VIOLATION kNtErrorMultipleFaultViolation
#define ERROR_SYSTEM_SHUTDOWN kNtErrorSystemShutdown
#define ERROR_PORT_NOT_SET kNtErrorPortNotSet
#define ERROR_DS_VERSION_CHECK_FAILURE kNtErrorDsVersionCheckFailure
#define ERROR_RANGE_NOT_FOUND kNtErrorRangeNotFound
#define ERROR_NOT_SAFE_MODE_DRIVER kNtErrorNotSafeModeDriver
#define ERROR_FAILED_DRIVER_ENTRY kNtErrorFailedDriverEntry
#define ERROR_DEVICE_ENUMERATION_ERROR kNtErrorDeviceEnumerationError
#define ERROR_MOUNT_POINT_NOT_RESOLVED kNtErrorMountPointNotResolved
#define ERROR_INVALID_DEVICE_OBJECT_PARAMETER kNtErrorInvalidDeviceObjectParameter
#define ERROR_MCA_OCCURED kNtErrorMcaOccured
#define ERROR_DRIVER_DATABASE_ERROR kNtErrorDriverDatabaseError
#define ERROR_SYSTEM_HIVE_TOO_LARGE kNtErrorSystemHiveTooLarge
#define ERROR_DRIVER_FAILED_PRIOR_UNLOAD kNtErrorDriverFailedPriorUnload
#define ERROR_VOLSNAP_PREPARE_HIBERNATE kNtErrorVolsnapPrepareHibernate
#define ERROR_HIBERNATION_FAILURE kNtErrorHibernationFailure
#define ERROR_PWD_TOO_LONG kNtErrorPwdTooLong
#define ERROR_FILE_SYSTEM_LIMITATION kNtErrorFileSystemLimitation
#define ERROR_ASSERTION_FAILURE kNtErrorAssertionFailure
#define ERROR_ACPI_ERROR kNtErrorAcpiError
#define ERROR_WOW_ASSERTION kNtErrorWowAssertion
#define ERROR_PNP_BAD_MPS_TABLE kNtErrorPnpBadMpsTable
#define ERROR_PNP_TRANSLATION_FAILED kNtErrorPnpTranslationFailed
#define ERROR_PNP_IRQ_TRANSLATION_FAILED kNtErrorPnpIrqTranslationFailed
#define ERROR_PNP_INVALID_ID kNtErrorPnpInvalidId
#define ERROR_WAKE_SYSTEM_DEBUGGER kNtErrorWakeSystemDebugger
#define ERROR_HANDLES_CLOSED kNtErrorHandlesClosed
#define ERROR_EXTRANEOUS_INFORMATION kNtErrorExtraneousInformation
#define ERROR_RXACT_COMMIT_NECESSARY kNtErrorRxactCommitNecessary
#define ERROR_MEDIA_CHECK kNtErrorMediaCheck
#define ERROR_GUID_SUBSTITUTION_MADE kNtErrorGuidSubstitutionMade
#define ERROR_STOPPED_ON_SYMLINK kNtErrorStoppedOnSymlink
#define ERROR_LONGJUMP kNtErrorLongjump
#define ERROR_PLUGPLAY_QUERY_VETOED kNtErrorPlugplayQueryVetoed
#define ERROR_UNWIND_CONSOLIDATE kNtErrorUnwindConsolidate
#define ERROR_REGISTRY_HIVE_RECOVERED kNtErrorRegistryHiveRecovered
#define ERROR_DLL_MIGHT_BE_INSECURE kNtErrorDllMightBeInsecure
#define ERROR_DLL_MIGHT_BE_INCOMPATIBLE kNtErrorDllMightBeIncompatible
#define ERROR_DBG_EXCEPTION_NOT_HANDLED kNtErrorDbgExceptionNotHandled
#define ERROR_DBG_REPLY_LATER kNtErrorDbgReplyLater
#define ERROR_DBG_UNABLE_TO_PROVIDE_HANDLE kNtErrorDbgUnableToProvideHandle
#define ERROR_DBG_TERMINATE_THREAD kNtErrorDbgTerminateThread
#define ERROR_DBG_TERMINATE_PROCESS kNtErrorDbgTerminateProcess
#define ERROR_DBG_CONTROL_C kNtErrorDbgControlC
#define ERROR_DBG_PRINTEXCEPTION_C kNtErrorDbgPrintexceptionC
#define ERROR_DBG_RIPEXCEPTION kNtErrorDbgRipexception
#define ERROR_DBG_CONTROL_BREAK kNtErrorDbgControlBreak
#define ERROR_DBG_COMMAND_EXCEPTION kNtErrorDbgCommandException
#define ERROR_OBJECT_NAME_EXISTS kNtErrorObjectNameExists
#define ERROR_THREAD_WAS_SUSPENDED kNtErrorThreadWasSuspended
#define ERROR_IMAGE_NOT_AT_BASE kNtErrorImageNotAtBase
#define ERROR_RXACT_STATE_CREATED kNtErrorRxactStateCreated
#define ERROR_SEGMENT_NOTIFICATION kNtErrorSegmentNotification
#define ERROR_BAD_CURRENT_DIRECTORY kNtErrorBadCurrentDirectory
#define ERROR_FT_READ_RECOVERY_FROM_BACKUP kNtErrorFtReadRecoveryFromBackup
#define ERROR_FT_WRITE_RECOVERY kNtErrorFtWriteRecovery
#define ERROR_IMAGE_MACHINE_TYPE_MISMATCH kNtErrorImageMachineTypeMismatch
#define ERROR_RECEIVE_PARTIAL kNtErrorReceivePartial
#define ERROR_RECEIVE_EXPEDITED kNtErrorReceiveExpedited
#define ERROR_RECEIVE_PARTIAL_EXPEDITED kNtErrorReceivePartialExpedited
#define ERROR_EVENT_DONE kNtErrorEventDone
#define ERROR_EVENT_PENDING kNtErrorEventPending
#define ERROR_CHECKING_FILE_SYSTEM kNtErrorCheckingFileSystem
#define ERROR_FATAL_APP_EXIT kNtErrorFatalAppExit
#define ERROR_PREDEFINED_HANDLE kNtErrorPredefinedHandle
#define ERROR_WAS_UNLOCKED kNtErrorWasUnlocked
#define ERROR_SERVICE_NOTIFICATION kNtErrorServiceNotification
#define ERROR_WAS_LOCKED kNtErrorWasLocked
#define ERROR_LOG_HARD_ERROR kNtErrorLogHardError
#define ERROR_ALREADY_WIN32 kNtErrorAlreadyWin32
#define ERROR_IMAGE_MACHINE_TYPE_MISMATCH_EXE kNtErrorImageMachineTypeMismatchExe
#define ERROR_NO_YIELD_PERFORMED kNtErrorNoYieldPerformed
#define ERROR_TIMER_RESUME_IGNORED kNtErrorTimerResumeIgnored
#define ERROR_ARBITRATION_UNHANDLED kNtErrorArbitrationUnhandled
#define ERROR_CARDBUS_NOT_SUPPORTED kNtErrorCardbusNotSupported
#define ERROR_MP_PROCESSOR_MISMATCH kNtErrorMpProcessorMismatch
#define ERROR_HIBERNATED kNtErrorHibernated
#define ERROR_RESUME_HIBERNATION kNtErrorResumeHibernation
#define ERROR_FIRMWARE_UPDATED kNtErrorFirmwareUpdated
#define ERROR_DRIVERS_LEAKING_LOCKED_PAGES kNtErrorDriversLeakingLockedPages
#define ERROR_WAKE_SYSTEM kNtErrorWakeSystem
#define ERRORWAIT_1 kNtErrorWait_1
#define ERRORWAIT_2 kNtErrorWait_2
#define ERRORWAIT_3 kNtErrorWait_3
#define ERRORWAIT_63 kNtErrorWait_63
#define ERRORABANDONEDWAIT_0 kNtErrorAbandonedWait_0
#define ERRORABANDONEDWAIT_63 kNtErrorAbandonedWait_63
#define ERROR_USER_APC kNtErrorUserApc
#define ERROR_KERNEL_APC kNtErrorKernelApc
#define ERROR_ALERTED kNtErrorAlerted
#define ERROR_ELEVATION_REQUIRED kNtErrorElevationRequired
#define ERROR_REPARSE kNtErrorReparse
#define ERROR_OPLOCK_BREAK_IN_PROGRESS kNtErrorOplockBreakInProgress
#define ERROR_VOLUME_MOUNTED kNtErrorVolumeMounted
#define ERROR_RXACT_COMMITTED kNtErrorRxactCommitted
#define ERROR_NOTIFY_CLEANUP kNtErrorNotifyCleanup
#define ERROR_PRIMARY_TRANSPORT_CONNECT_FAILED kNtErrorPrimaryTransportConnectFailed
#define ERROR_PAGE_FAULT_TRANSITION kNtErrorPageFaultTransition
#define ERROR_PAGE_FAULT_DEMAND_ZERO kNtErrorPageFaultDemandZero
#define ERROR_PAGE_FAULT_COPY_ON_WRITE kNtErrorPageFaultCopyOnWrite
#define ERROR_PAGE_FAULT_GUARD_PAGE kNtErrorPageFaultGuardPage
#define ERROR_PAGE_FAULT_PAGING_FILE kNtErrorPageFaultPagingFile
#define ERROR_CACHE_PAGE_LOCKED kNtErrorCachePageLocked
#define ERROR_CRASH_DUMP kNtErrorCrashDump
#define ERROR_BUFFER_ALL_ZEROS kNtErrorBufferAllZeros
#define ERROR_REPARSE_OBJECT kNtErrorReparseObject
#define ERROR_RESOURCE_REQUIREMENTS_CHANGED kNtErrorResourceRequirementsChanged
#define ERROR_TRANSLATION_COMPLETE kNtErrorTranslationComplete
#define ERROR_NOTHING_TO_TERMINATE kNtErrorNothingToTerminate
#define ERROR_PROCESS_NOT_IN_JOB kNtErrorProcessNotInJob
#define ERROR_PROCESS_IN_JOB kNtErrorProcessInJob
#define ERROR_VOLSNAP_HIBERNATE_READY kNtErrorVolsnapHibernateReady
#define ERROR_FSFILTER_OP_COMPLETED_SUCCESSFULLY kNtErrorFsfilterOpCompletedSuccessfully
#define ERROR_INTERRUPT_VECTOR_ALREADY_CONNECTED kNtErrorInterruptVectorAlreadyConnected
#define ERROR_INTERRUPT_STILL_CONNECTED kNtErrorInterruptStillConnected
#define ERROR_WAIT_FOR_OPLOCK kNtErrorWaitForOplock
#define ERROR_DBG_EXCEPTION_HANDLED kNtErrorDbgExceptionHandled
#define ERROR_DBG_CONTINUE kNtErrorDbgContinue
#define ERROR_CALLBACK_POP_STACK kNtErrorCallbackPopStack
#define ERROR_COMPRESSION_DISABLED kNtErrorCompressionDisabled
#define ERROR_CANTFETCHBACKWARDS kNtErrorCantfetchbackwards
#define ERROR_CANTSCROLLBACKWARDS kNtErrorCantscrollbackwards
#define ERROR_ROWSNOTRELEASED kNtErrorRowsnotreleased
#define ERROR_BAD_ACCESSOR_FLAGS kNtErrorBadAccessorFlags
#define ERROR_ERRORS_ENCOUNTERED kNtErrorErrorsEncountered
#define ERROR_NOT_CAPABLE kNtErrorNotCapable
#define ERROR_REQUEST_OUT_OF_SEQUENCE kNtErrorRequestOutOfSequence
#define ERROR_VERSION_PARSE_ERROR kNtErrorVersionParseError
#define ERROR_BADSTARTPOSITION kNtErrorBadstartposition
#define ERROR_MEMORY_HARDWARE kNtErrorMemoryHardware
#define ERROR_DISK_REPAIR_DISABLED kNtErrorDiskRepairDisabled
#define ERROR_INSUFFICIENT_RESOURCE_FOR_SPECIFIED_SHARED_SECTION_SIZE kNtErrorInsufficientResourceForSpecifiedSharedSectionSize
#define ERROR_SYSTEM_POWERSTATE_TRANSITION kNtErrorSystemPowerstateTransition
#define ERROR_SYSTEM_POWERSTATE_COMPLEX_TRANSITION kNtErrorSystemPowerstateComplexTransition
#define ERROR_MCA_EXCEPTION kNtErrorMcaException
#define ERROR_ACCESS_AUDIT_BY_POLICY kNtErrorAccessAuditByPolicy
#define ERROR_ACCESS_DISABLED_NO_SAFER_UI_BY_POLICY kNtErrorAccessDisabledNoSaferUiByPolicy
#define ERROR_ABANDON_HIBERFILE kNtErrorAbandonHiberfile
#define ERROR_LOST_WRITEBEHIND_DATA_NETWORK_DISCONNECTED kNtErrorLostWritebehindDataNetworkDisconnected
#define ERROR_LOST_WRITEBEHIND_DATA_NETWORK_SERVER_ERROR kNtErrorLostWritebehindDataNetworkServerError
#define ERROR_LOST_WRITEBEHIND_DATA_LOCAL_DISK_ERROR kNtErrorLostWritebehindDataLocalDiskError
#define ERROR_BAD_MCFG_TABLE kNtErrorBadMcfgTable
#define ERROR_DISK_REPAIR_REDIRECTED kNtErrorDiskRepairRedirected
#define ERROR_DISK_REPAIR_UNSUCCESSFUL kNtErrorDiskRepairUnsuccessful
#define ERROR_CORRUPT_LOG_OVERFULL kNtErrorCorruptLogOverfull
#define ERROR_CORRUPT_LOG_CORRUPTED kNtErrorCorruptLogCorrupted
#define ERROR_CORRUPT_LOG_UNAVAILABLE kNtErrorCorruptLogUnavailable
#define ERROR_CORRUPT_LOG_DELETED_FULL kNtErrorCorruptLogDeletedFull
#define ERROR_CORRUPT_LOG_CLEARED kNtErrorCorruptLogCleared
#define ERROR_ORPHAN_NAME_EXHAUSTED kNtErrorOrphanNameExhausted
#define ERROR_OPLOCK_SWITCHED_TO_NEW_HANDLE kNtErrorOplockSwitchedToNewHandle
#define ERROR_CANNOT_GRANT_REQUESTED_OPLOCK kNtErrorCannotGrantRequestedOplock
#define ERROR_CANNOT_BREAK_OPLOCK kNtErrorCannotBreakOplock
#define ERROR_OPLOCK_HANDLE_CLOSED kNtErrorOplockHandleClosed
#define ERROR_NO_ACE_CONDITION kNtErrorNoAceCondition
#define ERROR_INVALID_ACE_CONDITION kNtErrorInvalidAceCondition
#define ERROR_FILE_HANDLE_REVOKED kNtErrorFileHandleRevoked
#define ERROR_IMAGE_AT_DIFFERENT_BASE kNtErrorImageAtDifferentBase
#define ERROR_ENCRYPTED_IO_NOT_POSSIBLE kNtErrorEncryptedIoNotPossible
#define ERROR_FILE_METADATA_OPTIMIZATION_IN_PROGRESS kNtErrorFileMetadataOptimizationInProgress
#define ERROR_QUOTA_ACTIVITY kNtErrorQuotaActivity
#define ERROR_HANDLE_REVOKED kNtErrorHandleRevoked
#define ERROR_CALLBACK_INVOKE_INLINE kNtErrorCallbackInvokeInline
#define ERROR_CPU_SET_INVALID kNtErrorCpuSetInvalid
#define ERROR_ENCLAVE_NOT_TERMINATED kNtErrorEnclaveNotTerminated
#define ERROR_ENCLAVE_VIOLATION kNtErrorEnclaveViolation
#define ERROR_EA_ACCESS_DENIED kNtErrorEaAccessDenied
#define ERROR_OPERATION_ABORTED kNtErrorOperationAborted
#define ERROR_IO_INCOMPLETE kNtErrorIoIncomplete
#define ERROR_IO_PENDING kNtErrorIoPending
#define ERROR_NOACCESS kNtErrorNoaccess
#define ERROR_SWAPERROR kNtErrorSwaperror
#define ERROR_STACK_OVERFLOW kNtErrorStackOverflow
#define ERROR_INVALID_MESSAGE kNtErrorInvalidMessage
#define ERROR_CAN_NOT_COMPLETE kNtErrorCanNotComplete
#define ERROR_INVALID_FLAGS kNtErrorInvalidFlags
#define ERROR_UNRECOGNIZED_VOLUME kNtErrorUnrecognizedVolume
#define ERROR_FILE_INVALID kNtErrorFileInvalid
#define ERROR_FULLSCREEN_MODE kNtErrorFullscreenMode
#define ERROR_NO_TOKEN kNtErrorNoToken
#define ERROR_BADDB kNtErrorBaddb
#define ERROR_BADKEY kNtErrorBadkey
#define ERROR_CANTOPEN kNtErrorCantopen
#define ERROR_CANTREAD kNtErrorCantread
#define ERROR_CANTWRITE kNtErrorCantwrite
#define ERROR_REGISTRY_RECOVERED kNtErrorRegistryRecovered
#define ERROR_REGISTRY_CORRUPT kNtErrorRegistryCorrupt
#define ERROR_REGISTRY_IO_FAILED kNtErrorRegistryIoFailed
#define ERROR_NOT_REGISTRY_FILE kNtErrorNotRegistryFile
#define ERROR_KEY_DELETED kNtErrorKeyDeleted
#define ERROR_NO_LOG_SPACE kNtErrorNoLogSpace
#define ERROR_KEY_HAS_CHILDREN kNtErrorKeyHasChildren
#define ERROR_CHILD_MUST_BE_VOLATILE kNtErrorChildMustBeVolatile
#define ERROR_NOTIFY_ENUM_DIR kNtErrorNotifyEnumDir
#define ERROR_DEPENDENT_SERVICES_RUNNING kNtErrorDependentServicesRunning
#define ERROR_INVALID_SERVICE_CONTROL kNtErrorInvalidServiceControl
#define ERROR_SERVICE_REQUEST_TIMEOUT kNtErrorServiceRequestTimeout
#define ERROR_SERVICE_NO_THREAD kNtErrorServiceNoThread
#define ERROR_SERVICE_DATABASE_LOCKED kNtErrorServiceDatabaseLocked
#define ERROR_SERVICE_ALREADY_RUNNING kNtErrorServiceAlreadyRunning
#define ERROR_INVALID_SERVICE_ACCOUNT kNtErrorInvalidServiceAccount
#define ERROR_SERVICE_DISABLED kNtErrorServiceDisabled
#define ERROR_CIRCULAR_DEPENDENCY kNtErrorCircularDependency
#define ERROR_SERVICE_DOES_NOT_EXIST kNtErrorServiceDoesNotExist
#define ERROR_SERVICE_CANNOT_ACCEPT_CTRL kNtErrorServiceCannotAcceptCtrl
#define ERROR_SERVICE_NOT_ACTIVE kNtErrorServiceNotActive
#define ERROR_FAILED_SERVICE_CONTROLLER_CONNECT kNtErrorFailedServiceControllerConnect
#define ERROR_EXCEPTION_IN_SERVICE kNtErrorExceptionInService
#define ERROR_DATABASE_DOES_NOT_EXIST kNtErrorDatabaseDoesNotExist
#define ERROR_SERVICE_SPECIFIC_ERROR kNtErrorServiceSpecificError
#define ERROR_PROCESS_ABORTED kNtErrorProcessAborted
#define ERROR_SERVICE_DEPENDENCY_FAIL kNtErrorServiceDependencyFail
#define ERROR_SERVICE_LOGON_FAILED kNtErrorServiceLogonFailed
#define ERROR_SERVICE_START_HANG kNtErrorServiceStartHang
#define ERROR_INVALID_SERVICE_LOCK kNtErrorInvalidServiceLock
#define ERROR_SERVICE_MARKED_FOR_DELETE kNtErrorServiceMarkedForDelete
#define ERROR_SERVICE_EXISTS kNtErrorServiceExists
#define ERROR_ALREADY_RUNNING_LKG kNtErrorAlreadyRunningLkg
#define ERROR_SERVICE_DEPENDENCY_DELETED kNtErrorServiceDependencyDeleted
#define ERROR_BOOT_ALREADY_ACCEPTED kNtErrorBootAlreadyAccepted
#define ERROR_SERVICE_NEVER_STARTED kNtErrorServiceNeverStarted
#define ERROR_DUPLICATE_SERVICE_NAME kNtErrorDuplicateServiceName
#define ERROR_DIFFERENT_SERVICE_ACCOUNT kNtErrorDifferentServiceAccount
#define ERROR_CANNOT_DETECT_DRIVER_FAILURE kNtErrorCannotDetectDriverFailure
#define ERROR_CANNOT_DETECT_PROCESS_ABORT kNtErrorCannotDetectProcessAbort
#define ERROR_NO_RECOVERY_PROGRAM kNtErrorNoRecoveryProgram
#define ERROR_SERVICE_NOT_IN_EXE kNtErrorServiceNotInExe
#define ERROR_NOT_SAFEBOOT_SERVICE kNtErrorNotSafebootService
#define ERROR_END_OF_MEDIA kNtErrorEndOfMedia
#define ERROR_FILEMARK_DETECTED kNtErrorFilemarkDetected
#define ERROR_BEGINNING_OF_MEDIA kNtErrorBeginningOfMedia
#define ERROR_SETMARK_DETECTED kNtErrorSetmarkDetected
#define ERROR_NO_DATA_DETECTED kNtErrorNoDataDetected
#define ERROR_PARTITION_FAILURE kNtErrorPartitionFailure
#define ERROR_INVALID_BLOCK_LENGTH kNtErrorInvalidBlockLength
#define ERROR_DEVICE_NOT_PARTITIONED kNtErrorDeviceNotPartitioned
#define ERROR_UNABLE_TO_LOCK_MEDIA kNtErrorUnableToLockMedia
#define ERROR_UNABLE_TO_UNLOAD_MEDIA kNtErrorUnableToUnloadMedia
#define ERROR_MEDIA_CHANGED kNtErrorMediaChanged
#define ERROR_BUS_RESET kNtErrorBusReset
#define ERROR_NO_MEDIA_IN_DRIVE kNtErrorNoMediaInDrive
#define ERROR_NO_UNICODE_TRANSLATION kNtErrorNoUnicodeTranslation
#define ERROR_DLL_INIT_FAILED kNtErrorDllInitFailed
#define ERROR_SHUTDOWN_IN_PROGRESS kNtErrorShutdownInProgress
#define ERROR_NO_SHUTDOWN_IN_PROGRESS kNtErrorNoShutdownInProgress
#define ERROR_IO_DEVICE kNtErrorIoDevice
#define ERROR_SERIAL_NO_DEVICE kNtErrorSerialNoDevice
#define ERROR_IRQ_BUSY kNtErrorIrqBusy
#define ERROR_MORE_WRITES kNtErrorMoreWrites
#define ERROR_COUNTER_TIMEOUT kNtErrorCounterTimeout
#define ERROR_FLOPPY_ID_MARK_NOT_FOUND kNtErrorFloppyIdMarkNotFound
#define ERROR_FLOPPY_WRONG_CYLINDER kNtErrorFloppyWrongCylinder
#define ERROR_FLOPPY_UNKNOWN_ERROR kNtErrorFloppyUnknownError
#define ERROR_FLOPPY_BAD_REGISTERS kNtErrorFloppyBadRegisters
#define ERROR_DISK_RECALIBRATE_FAILED kNtErrorDiskRecalibrateFailed
#define ERROR_DISK_OPERATION_FAILED kNtErrorDiskOperationFailed
#define ERROR_DISK_RESET_FAILED kNtErrorDiskResetFailed
#define ERROR_EOM_OVERFLOW kNtErrorEomOverflow
#define ERROR_NOT_ENOUGH_SERVER_MEMORY kNtErrorNotEnoughServerMemory
#define ERROR_POSSIBLE_DEADLOCK kNtErrorPossibleDeadlock
#define ERROR_MAPPED_ALIGNMENT kNtErrorMappedAlignment
#define ERROR_SET_POWER_STATE_VETOED kNtErrorSetPowerStateVetoed
#define ERROR_SET_POWER_STATE_FAILED kNtErrorSetPowerStateFailed
#define ERROR_TOO_MANY_LINKS kNtErrorTooManyLinks
#define ERROR_OLD_WIN_VERSION kNtErrorOldWinVersion
#define ERROR_APP_WRONG_OS kNtErrorAppWrongOs
#define ERROR_SINGLE_INSTANCE_APP kNtErrorSingleInstanceApp
#define ERROR_RMODE_APP kNtErrorRmodeApp
#define ERROR_INVALID_DLL kNtErrorInvalidDll
#define ERROR_NO_ASSOCIATION kNtErrorNoAssociation
#define ERROR_DDE_FAIL kNtErrorDdeFail
#define ERROR_DLL_NOT_FOUND kNtErrorDllNotFound
#define ERROR_NO_MORE_USER_HANDLES kNtErrorNoMoreUserHandles
#define ERROR_MESSAGE_SYNC_ONLY kNtErrorMessageSyncOnly
#define ERROR_SOURCE_ELEMENT_EMPTY kNtErrorSourceElementEmpty
#define ERROR_DESTINATION_ELEMENT_FULL kNtErrorDestinationElementFull
#define ERROR_ILLEGAL_ELEMENT_ADDRESS kNtErrorIllegalElementAddress
#define ERROR_MAGAZINE_NOT_PRESENT kNtErrorMagazineNotPresent
#define ERROR_DEVICE_REINITIALIZATION_NEEDED kNtErrorDeviceReinitializationNeeded
#define ERROR_DEVICE_REQUIRES_CLEANING kNtErrorDeviceRequiresCleaning
#define ERROR_DEVICE_DOOR_OPEN kNtErrorDeviceDoorOpen
#define ERROR_DEVICE_NOT_CONNECTED kNtErrorDeviceNotConnected
#define ERROR_NOT_FOUND kNtErrorNotFound
#define ERROR_NO_MATCH kNtErrorNoMatch
#define ERROR_SET_NOT_FOUND kNtErrorSetNotFound
#define ERROR_POINT_NOT_FOUND kNtErrorPointNotFound
#define ERROR_NO_TRACKING_SERVICE kNtErrorNoTrackingService
#define ERROR_NO_VOLUME_ID kNtErrorNoVolumeId
#define ERROR_UNABLE_TO_REMOVE_REPLACED kNtErrorUnableToRemoveReplaced
#define ERROR_UNABLE_TO_MOVE_REPLACEMENT kNtErrorUnableToMoveReplacement
#define ERROR_JOURNAL_DELETE_IN_PROGRESS kNtErrorJournalDeleteInProgress
#define ERROR_JOURNAL_NOT_ACTIVE kNtErrorJournalNotActive
#define ERROR_POTENTIAL_FILE_FOUND kNtErrorPotentialFileFound
#define ERROR_JOURNAL_ENTRY_DELETED kNtErrorJournalEntryDeleted
#define ERROR_SHUTDOWN_IS_SCHEDULED kNtErrorShutdownIsScheduled
#define ERROR_SHUTDOWN_USERS_LOGGED_ON kNtErrorShutdownUsersLoggedOn
#define ERROR_BAD_DEVICE kNtErrorBadDevice
#define ERROR_CONNECTION_UNAVAIL kNtErrorConnectionUnavail
#define ERROR_DEVICE_ALREADY_REMEMBERED kNtErrorDeviceAlreadyRemembered
#define ERROR_NO_NET_OR_BAD_PATH kNtErrorNoNetOrBadPath
#define ERROR_BAD_PROVIDER kNtErrorBadProvider
#define ERROR_CANNOT_OPEN_PROFILE kNtErrorCannotOpenProfile
#define ERROR_BAD_PROFILE kNtErrorBadProfile
#define ERROR_NOT_CONTAINER kNtErrorNotContainer
#define ERROR_EXTENDED_ERROR kNtErrorExtendedError
#define ERROR_INVALID_GROUPNAME kNtErrorInvalidGroupname
#define ERROR_INVALID_COMPUTERNAME kNtErrorInvalidComputername
#define ERROR_INVALID_EVENTNAME kNtErrorInvalidEventname
#define ERROR_INVALID_DOMAINNAME kNtErrorInvalidDomainname
#define ERROR_INVALID_SERVICENAME kNtErrorInvalidServicename
#define ERROR_INVALID_NETNAME kNtErrorInvalidNetname
#define ERROR_INVALID_SHARENAME kNtErrorInvalidSharename
#define ERROR_INVALID_PASSWORDNAME kNtErrorInvalidPasswordname
#define ERROR_INVALID_MESSAGENAME kNtErrorInvalidMessagename
#define ERROR_INVALID_MESSAGEDEST kNtErrorInvalidMessagedest
#define ERROR_SESSION_CREDENTIAL_CONFLICT kNtErrorSessionCredentialConflict
#define ERROR_REMOTE_SESSION_LIMIT_EXCEEDED kNtErrorRemoteSessionLimitExceeded
#define ERROR_DUP_DOMAINNAME kNtErrorDupDomainname
#define ERROR_NO_NETWORK kNtErrorNoNetwork
#define ERROR_CANCELLED kNtErrorCancelled
#define ERROR_USER_MAPPED_FILE kNtErrorUserMappedFile
#define ERROR_CONNECTION_REFUSED kNtErrorConnectionRefused
#define ERROR_GRACEFUL_DISCONNECT kNtErrorGracefulDisconnect
#define ERROR_ADDRESS_ALREADY_ASSOCIATED kNtErrorAddressAlreadyAssociated
#define ERROR_ADDRESS_NOT_ASSOCIATED kNtErrorAddressNotAssociated
#define ERROR_CONNECTION_INVALID kNtErrorConnectionInvalid
#define ERROR_CONNECTION_ACTIVE kNtErrorConnectionActive
#define ERROR_NETWORK_UNREACHABLE kNtErrorNetworkUnreachable
#define ERROR_HOST_UNREACHABLE kNtErrorHostUnreachable
#define ERROR_PROTOCOL_UNREACHABLE kNtErrorProtocolUnreachable
#define ERROR_PORT_UNREACHABLE kNtErrorPortUnreachable
#define ERROR_REQUEST_ABORTED kNtErrorRequestAborted
#define ERROR_CONNECTION_ABORTED kNtErrorConnectionAborted
#define ERROR_RETRY kNtErrorRetry
#define ERROR_CONNECTION_COUNT_LIMIT kNtErrorConnectionCountLimit
#define ERROR_LOGIN_TIME_RESTRICTION kNtErrorLoginTimeRestriction
#define ERROR_LOGIN_WKSTA_RESTRICTION kNtErrorLoginWkstaRestriction
#define ERROR_INCORRECT_ADDRESS kNtErrorIncorrectAddress
#define ERROR_ALREADY_REGISTERED kNtErrorAlreadyRegistered
#define ERROR_SERVICE_NOT_FOUND kNtErrorServiceNotFound
#define ERROR_NOT_AUTHENTICATED kNtErrorNotAuthenticated
#define ERROR_NOT_LOGGED_ON kNtErrorNotLoggedOn
#define ERROR_CONTINUE kNtErrorContinue
#define ERROR_ALREADY_INITIALIZED kNtErrorAlreadyInitialized
#define ERROR_NO_MORE_DEVICES kNtErrorNoMoreDevices
#define ERROR_NO_SUCH_SITE kNtErrorNoSuchSite
#define ERROR_DOMAIN_CONTROLLER_EXISTS kNtErrorDomainControllerExists
#define ERROR_ONLY_IF_CONNECTED kNtErrorOnlyIfConnected
#define ERROR_OVERRIDE_NOCHANGES kNtErrorOverrideNochanges
#define ERROR_BAD_USER_PROFILE kNtErrorBadUserProfile
#define ERROR_NOT_SUPPORTED_ON_SBS kNtErrorNotSupportedOnSbs
#define ERROR_SERVER_SHUTDOWN_IN_PROGRESS kNtErrorServerShutdownInProgress
#define ERROR_HOST_DOWN kNtErrorHostDown
#define ERROR_NON_ACCOUNT_SID kNtErrorNonAccountSid
#define ERROR_NON_DOMAIN_SID kNtErrorNonDomainSid
#define ERROR_APPHELP_BLOCK kNtErrorApphelpBlock
#define ERROR_ACCESS_DISABLED_BY_POLICY kNtErrorAccessDisabledByPolicy
#define ERROR_REG_NAT_CONSUMPTION kNtErrorRegNatConsumption
#define ERROR_CSCSHARE_OFFLINE kNtErrorCscshareOffline
#define ERROR_PKINIT_FAILURE kNtErrorPkinitFailure
#define ERROR_SMARTCARD_SUBSYSTEM_FAILURE kNtErrorSmartcardSubsystemFailure
#define ERROR_DOWNGRADE_DETECTED kNtErrorDowngradeDetected
#define ERROR_MACHINE_LOCKED kNtErrorMachineLocked
#define ERROR_SMB_GUEST_LOGON_BLOCKED kNtErrorSmbGuestLogonBlocked
#define ERROR_CALLBACK_SUPPLIED_INVALID_DATA kNtErrorCallbackSuppliedInvalidData
#define ERROR_SYNC_FOREGROUND_REFRESH_REQUIRED kNtErrorSyncForegroundRefreshRequired
#define ERROR_DRIVER_BLOCKED kNtErrorDriverBlocked
#define ERROR_INVALID_IMPORT_OF_NON_DLL kNtErrorInvalidImportOfNonDll
#define ERROR_ACCESS_DISABLED_WEBBLADE kNtErrorAccessDisabledWebblade
#define ERROR_ACCESS_DISABLED_WEBBLADE_TAMPER kNtErrorAccessDisabledWebbladeTamper
#define ERROR_RECOVERY_FAILURE kNtErrorRecoveryFailure
#define ERROR_ALREADY_FIBER kNtErrorAlreadyFiber
#define ERROR_ALREADY_THREAD kNtErrorAlreadyThread
#define ERROR_STACK_BUFFER_OVERRUN kNtErrorStackBufferOverrun
#define ERROR_PARAMETER_QUOTA_EXCEEDED kNtErrorParameterQuotaExceeded
#define ERROR_DEBUGGER_INACTIVE kNtErrorDebuggerInactive
#define ERROR_DELAY_LOAD_FAILED kNtErrorDelayLoadFailed
#define ERROR_VDM_DISALLOWED kNtErrorVdmDisallowed
#define ERROR_UNIDENTIFIED_ERROR kNtErrorUnidentifiedError
#define ERROR_INVALID_CRUNTIME_PARAMETER kNtErrorInvalidCruntimeParameter
#define ERROR_BEYOND_VDL kNtErrorBeyondVdl
#define ERROR_INCOMPATIBLE_SERVICE_SID_TYPE kNtErrorIncompatibleServiceSidType
#define ERROR_DRIVER_PROCESS_TERMINATED kNtErrorDriverProcessTerminated
#define ERROR_IMPLEMENTATION_LIMIT kNtErrorImplementationLimit
#define ERROR_PROCESS_IS_PROTECTED kNtErrorProcessIsProtected
#define ERROR_SERVICE_NOTIFY_CLIENT_LAGGING kNtErrorServiceNotifyClientLagging
#define ERROR_DISK_QUOTA_EXCEEDED kNtErrorDiskQuotaExceeded
#define ERROR_CONTENT_BLOCKED kNtErrorContentBlocked
#define ERROR_INCOMPATIBLE_SERVICE_PRIVILEGE kNtErrorIncompatibleServicePrivilege
#define ERROR_APP_HANG kNtErrorAppHang
#define ERROR_INVALID_LABEL kNtErrorInvalidLabel
#define ERROR_NOT_ALL_ASSIGNED kNtErrorNotAllAssigned
#define ERROR_SOME_NOT_MAPPED kNtErrorSomeNotMapped
#define ERROR_NO_QUOTAS_FOR_ACCOUNT kNtErrorNoQuotasForAccount
#define ERROR_LOCAL_USER_SESSION_KEY kNtErrorLocalUserSessionKey
#define ERROR_NULL_LM_PASSWORD kNtErrorNullLmPassword
#define ERROR_UNKNOWN_REVISION kNtErrorUnknownRevision
#define ERROR_REVISION_MISMATCH kNtErrorRevisionMismatch
#define ERROR_INVALID_OWNER kNtErrorInvalidOwner
#define ERROR_INVALID_PRIMARY_GROUP kNtErrorInvalidPrimaryGroup
#define ERROR_NO_IMPERSONATION_TOKEN kNtErrorNoImpersonationToken
#define ERROR_CANT_DISABLE_MANDATORY kNtErrorCantDisableMandatory
#define ERROR_NO_LOGON_SERVERS kNtErrorNoLogonServers
#define ERROR_NO_SUCH_LOGON_SESSION kNtErrorNoSuchLogonSession
#define ERROR_NO_SUCH_PRIVILEGE kNtErrorNoSuchPrivilege
#define ERROR_PRIVILEGE_NOT_HELD kNtErrorPrivilegeNotHeld
#define ERROR_INVALID_ACCOUNT_NAME kNtErrorInvalidAccountName
#define ERROR_USER_EXISTS kNtErrorUserExists
#define ERROR_NO_SUCH_USER kNtErrorNoSuchUser
#define ERROR_GROUP_EXISTS kNtErrorGroupExists
#define ERROR_NO_SUCH_GROUP kNtErrorNoSuchGroup
#define ERROR_MEMBER_IN_GROUP kNtErrorMemberInGroup
#define ERROR_MEMBER_NOT_IN_GROUP kNtErrorMemberNotInGroup
#define ERROR_LAST_ADMIN kNtErrorLastAdmin
#define ERROR_WRONG_PASSWORD kNtErrorWrongPassword
#define ERROR_ILL_FORMED_PASSWORD kNtErrorIllFormedPassword
#define ERROR_PASSWORD_RESTRICTION kNtErrorPasswordRestriction
#define ERROR_LOGON_FAILURE kNtErrorLogonFailure
#define ERROR_ACCOUNT_RESTRICTION kNtErrorAccountRestriction
#define ERROR_INVALID_LOGON_HOURS kNtErrorInvalidLogonHours
#define ERROR_INVALID_WORKSTATION kNtErrorInvalidWorkstation
#define ERROR_PASSWORD_EXPIRED kNtErrorPasswordExpired
#define ERROR_ACCOUNT_DISABLED kNtErrorAccountDisabled
#define ERROR_NONE_MAPPED kNtErrorNoneMapped
#define ERROR_TOO_MANY_LUIDS_REQUESTED kNtErrorTooManyLuidsRequested
#define ERROR_LUIDS_EXHAUSTED kNtErrorLuidsExhausted
#define ERROR_INVALID_SUB_AUTHORITY kNtErrorInvalidSubAuthority
#define ERROR_INVALID_ACL kNtErrorInvalidAcl
#define ERROR_INVALID_SID kNtErrorInvalidSid
#define ERROR_INVALID_SECURITY_DESCR kNtErrorInvalidSecurityDescr
#define ERROR_BAD_INHERITANCE_ACL kNtErrorBadInheritanceAcl
#define ERROR_SERVER_DISABLED kNtErrorServerDisabled
#define ERROR_SERVER_NOT_DISABLED kNtErrorServerNotDisabled
#define ERROR_INVALID_ID_AUTHORITY kNtErrorInvalidIdAuthority
#define ERROR_ALLOTTED_SPACE_EXCEEDED kNtErrorAllottedSpaceExceeded
#define ERROR_INVALID_GROUP_ATTRIBUTES kNtErrorInvalidGroupAttributes
#define ERROR_BAD_IMPERSONATION_LEVEL kNtErrorBadImpersonationLevel
#define ERROR_CANT_OPEN_ANONYMOUS kNtErrorCantOpenAnonymous
#define ERROR_BAD_VALIDATION_CLASS kNtErrorBadValidationClass
#define ERROR_BAD_TOKEN_TYPE kNtErrorBadTokenType
#define ERROR_NO_SECURITY_ON_OBJECT kNtErrorNoSecurityOnObject
#define ERROR_CANT_ACCESS_DOMAIN_INFO kNtErrorCantAccessDomainInfo
#define ERROR_INVALID_SERVER_STATE kNtErrorInvalidServerState
#define ERROR_INVALID_DOMAIN_STATE kNtErrorInvalidDomainState
#define ERROR_INVALID_DOMAIN_ROLE kNtErrorInvalidDomainRole
#define ERROR_NO_SUCH_DOMAIN kNtErrorNoSuchDomain
#define ERROR_DOMAIN_EXISTS kNtErrorDomainExists
#define ERROR_DOMAIN_LIMIT_EXCEEDED kNtErrorDomainLimitExceeded
#define ERROR_INTERNAL_DB_CORRUPTION kNtErrorInternalDbCorruption
#define ERROR_INTERNAL_ERROR kNtErrorInternalError
#define ERROR_GENERIC_NOT_MAPPED kNtErrorGenericNotMapped
#define ERROR_BAD_DESCRIPTOR_FORMAT kNtErrorBadDescriptorFormat
#define ERROR_NOT_LOGON_PROCESS kNtErrorNotLogonProcess
#define ERROR_LOGON_SESSION_EXISTS kNtErrorLogonSessionExists
#define ERROR_NO_SUCH_PACKAGE kNtErrorNoSuchPackage
#define ERROR_BAD_LOGON_SESSION_STATE kNtErrorBadLogonSessionState
#define ERROR_LOGON_SESSION_COLLISION kNtErrorLogonSessionCollision
#define ERROR_INVALID_LOGON_TYPE kNtErrorInvalidLogonType
#define ERROR_CANNOT_IMPERSONATE kNtErrorCannotImpersonate
#define ERROR_RXACT_INVALID_STATE kNtErrorRxactInvalidState
#define ERROR_RXACT_COMMIT_FAILURE kNtErrorRxactCommitFailure
#define ERROR_SPECIAL_ACCOUNT kNtErrorSpecialAccount
#define ERROR_SPECIAL_GROUP kNtErrorSpecialGroup
#define ERROR_SPECIAL_USER kNtErrorSpecialUser
#define ERROR_MEMBERS_PRIMARY_GROUP kNtErrorMembersPrimaryGroup
#define ERROR_TOKEN_ALREADY_IN_USE kNtErrorTokenAlreadyInUse
#define ERROR_NO_SUCH_ALIAS kNtErrorNoSuchAlias
#define ERROR_MEMBER_NOT_IN_ALIAS kNtErrorMemberNotInAlias
#define ERROR_MEMBER_IN_ALIAS kNtErrorMemberInAlias
#define ERROR_ALIAS_EXISTS kNtErrorAliasExists
#define ERROR_LOGON_NOT_GRANTED kNtErrorLogonNotGranted
#define ERROR_TOO_MANY_SECRETS kNtErrorTooManySecrets
#define ERROR_SECRET_TOO_LONG kNtErrorSecretTooLong
#define ERROR_INTERNAL_DB_ERROR kNtErrorInternalDbError
#define ERROR_TOO_MANY_CONTEXT_IDS kNtErrorTooManyContextIds
#define ERROR_LOGON_TYPE_NOT_GRANTED kNtErrorLogonTypeNotGranted
#define ERROR_NT_CROSS_ENCRYPTION_REQUIRED kNtErrorNtCrossEncryptionRequired
#define ERROR_NO_SUCH_MEMBER kNtErrorNoSuchMember
#define ERROR_INVALID_MEMBER kNtErrorInvalidMember
#define ERROR_TOO_MANY_SIDS kNtErrorTooManySids
#define ERROR_LM_CROSS_ENCRYPTION_REQUIRED kNtErrorLmCrossEncryptionRequired
#define ERROR_NO_INHERITANCE kNtErrorNoInheritance
#define ERROR_FILE_CORRUPT kNtErrorFileCorrupt
#define ERROR_DISK_CORRUPT kNtErrorDiskCorrupt
#define ERROR_NO_USER_SESSION_KEY kNtErrorNoUserSessionKey
#define ERROR_LICENSE_QUOTA_EXCEEDED kNtErrorLicenseQuotaExceeded
#define ERROR_WRONG_TARGET_NAME kNtErrorWrongTargetName
#define ERROR_MUTUAL_AUTH_FAILED kNtErrorMutualAuthFailed
#define ERROR_TIME_SKEW kNtErrorTimeSkew
#define ERROR_CURRENT_DOMAIN_NOT_ALLOWED kNtErrorCurrentDomainNotAllowed
#define ERROR_INVALID_WINDOW_HANDLE kNtErrorInvalidWindowHandle
#define ERROR_INVALID_MENU_HANDLE kNtErrorInvalidMenuHandle
#define ERROR_INVALID_CURSOR_HANDLE kNtErrorInvalidCursorHandle
#define ERROR_INVALID_ACCEL_HANDLE kNtErrorInvalidAccelHandle
#define ERROR_INVALID_HOOK_HANDLE kNtErrorInvalidHookHandle
#define ERROR_INVALID_DWP_HANDLE kNtErrorInvalidDwpHandle
#define ERROR_TLW_WITH_WSCHILD kNtErrorTlwWithWschild
#define ERROR_CANNOT_FIND_WND_CLASS kNtErrorCannotFindWndClass
#define ERROR_WINDOW_OF_OTHER_THREAD kNtErrorWindowOfOtherThread
#define ERROR_HOTKEY_ALREADY_REGISTERED kNtErrorHotkeyAlreadyRegistered
#define ERROR_CLASS_ALREADY_EXISTS kNtErrorClassAlreadyExists
#define ERROR_CLASS_DOES_NOT_EXIST kNtErrorClassDoesNotExist
#define ERROR_CLASS_HAS_WINDOWS kNtErrorClassHasWindows
#define ERROR_INVALID_INDEX kNtErrorInvalidIndex
#define ERROR_INVALID_ICON_HANDLE kNtErrorInvalidIconHandle
#define ERROR_PRIVATE_DIALOG_INDEX kNtErrorPrivateDialogIndex
#define ERROR_LISTBOX_ID_NOT_FOUND kNtErrorListboxIdNotFound
#define ERROR_NO_WILDCARD_CHARACTERS kNtErrorNoWildcardCharacters
#define ERROR_CLIPBOARD_NOT_OPEN kNtErrorClipboardNotOpen
#define ERROR_HOTKEY_NOT_REGISTERED kNtErrorHotkeyNotRegistered
#define ERROR_WINDOW_NOT_DIALOG kNtErrorWindowNotDialog
#define ERROR_CONTROL_ID_NOT_FOUND kNtErrorControlIdNotFound
#define ERROR_INVALID_COMBOBOX_MESSAGE kNtErrorInvalidComboboxMessage
#define ERROR_WINDOW_NOT_COMBOBOX kNtErrorWindowNotCombobox
#define ERROR_INVALID_EDIT_HEIGHT kNtErrorInvalidEditHeight
#define ERROR_DC_NOT_FOUND kNtErrorDcNotFound
#define ERROR_INVALID_HOOK_FILTER kNtErrorInvalidHookFilter
#define ERROR_INVALID_FILTER_PROC kNtErrorInvalidFilterProc
#define ERROR_HOOK_NEEDS_HMOD kNtErrorHookNeedsHmod
#define ERROR_GLOBAL_ONLY_HOOK kNtErrorGlobalOnlyHook
#define ERROR_JOURNAL_HOOK_SET kNtErrorJournalHookSet
#define ERROR_HOOK_NOT_INSTALLED kNtErrorHookNotInstalled
#define ERROR_INVALID_LB_MESSAGE kNtErrorInvalidLbMessage
#define ERROR_SETCOUNT_ON_BAD_LB kNtErrorSetcountOnBadLb
#define ERROR_LB_WITHOUT_TABSTOPS kNtErrorLbWithoutTabstops
#define ERROR_DESTROY_OBJECT_OF_OTHER_THREAD kNtErrorDestroyObjectOfOtherThread
#define ERROR_CHILD_WINDOW_MENU kNtErrorChildWindowMenu
#define ERROR_NO_SYSTEM_MENU kNtErrorNoSystemMenu
#define ERROR_INVALID_MSGBOX_STYLE kNtErrorInvalidMsgboxStyle
#define ERROR_INVALID_SPI_VALUE kNtErrorInvalidSpiValue
#define ERROR_SCREEN_ALREADY_LOCKED kNtErrorScreenAlreadyLocked
#define ERROR_HWNDS_HAVE_DIFF_PARENT kNtErrorHwndsHaveDiffParent
#define ERROR_NOT_CHILD_WINDOW kNtErrorNotChildWindow
#define ERROR_INVALID_GW_COMMAND kNtErrorInvalidGwCommand
#define ERROR_INVALID_THREAD_ID kNtErrorInvalidThreadId
#define ERROR_NON_MDICHILD_WINDOW kNtErrorNonMdichildWindow
#define ERROR_POPUP_ALREADY_ACTIVE kNtErrorPopupAlreadyActive
#define ERROR_NO_SCROLLBARS kNtErrorNoScrollbars
#define ERROR_INVALID_SCROLLBAR_RANGE kNtErrorInvalidScrollbarRange
#define ERROR_INVALID_SHOWWIN_COMMAND kNtErrorInvalidShowwinCommand
#define ERROR_NO_SYSTEM_RESOURCES kNtErrorNoSystemResources
#define ERROR_NONPAGED_SYSTEM_RESOURCES kNtErrorNonpagedSystemResources
#define ERROR_PAGED_SYSTEM_RESOURCES kNtErrorPagedSystemResources
#define ERROR_WORKING_SET_QUOTA kNtErrorWorkingSetQuota
#define ERROR_PAGEFILE_QUOTA kNtErrorPagefileQuota
#define ERROR_COMMITMENT_LIMIT kNtErrorCommitmentLimit
#define ERROR_MENU_ITEM_NOT_FOUND kNtErrorMenuItemNotFound
#define ERROR_INVALID_KEYBOARD_HANDLE kNtErrorInvalidKeyboardHandle
#define ERROR_HOOK_TYPE_NOT_ALLOWED kNtErrorHookTypeNotAllowed
#define ERROR_REQUIRES_INTERACTIVE_WINDOWSTATION kNtErrorRequiresInteractiveWindowstation
#define ERROR_TIMEOUT kNtErrorTimeout
#define ERROR_INVALID_MONITOR_HANDLE kNtErrorInvalidMonitorHandle
#define ERROR_INCORRECT_SIZE kNtErrorIncorrectSize
#define ERROR_SYMLINK_CLASS_DISABLED kNtErrorSymlinkClassDisabled
#define ERROR_SYMLINK_NOT_SUPPORTED kNtErrorSymlinkNotSupported
#define ERROR_XML_PARSE_ERROR kNtErrorXmlParseError
#define ERROR_XMLDSIG_ERROR kNtErrorXmldsigError
#define ERROR_RESTART_APPLICATION kNtErrorRestartApplication
#define ERROR_WRONG_COMPARTMENT kNtErrorWrongCompartment
#define ERROR_AUTHIP_FAILURE kNtErrorAuthipFailure
#define ERROR_NO_NVRAM_RESOURCES kNtErrorNoNvramResources
#define ERROR_NOT_GUI_PROCESS kNtErrorNotGuiProcess
#define ERROR_EVENTLOG_FILE_CORRUPT kNtErrorEventlogFileCorrupt
#define ERROR_EVENTLOG_CANT_START kNtErrorEventlogCantStart
#define ERROR_LOG_FILE_FULL kNtErrorLogFileFull
#define ERROR_EVENTLOG_FILE_CHANGED kNtErrorEventlogFileChanged
#define ERROR_CONTAINER_ASSIGNED kNtErrorContainerAssigned
#define ERROR_JOB_NO_CONTAINER kNtErrorJobNoContainer
#define ERROR_INVALID_TASK_NAME kNtErrorInvalidTaskName
#define ERROR_INVALID_TASK_INDEX kNtErrorInvalidTaskIndex
#define ERROR_THREAD_ALREADY_IN_TASK kNtErrorThreadAlreadyInTask
#define ERROR_INSTALL_SERVICE_FAILURE kNtErrorInstallServiceFailure
#define ERROR_INSTALL_USEREXIT kNtErrorInstallUserexit
#define ERROR_INSTALL_FAILURE kNtErrorInstallFailure
#define ERROR_INSTALL_SUSPEND kNtErrorInstallSuspend
#define ERROR_UNKNOWN_PRODUCT kNtErrorUnknownProduct
#define ERROR_UNKNOWN_FEATURE kNtErrorUnknownFeature
#define ERROR_UNKNOWN_COMPONENT kNtErrorUnknownComponent
#define ERROR_UNKNOWN_PROPERTY kNtErrorUnknownProperty
#define ERROR_INVALID_HANDLE_STATE kNtErrorInvalidHandleState
#define ERROR_BAD_CONFIGURATION kNtErrorBadConfiguration
#define ERROR_INDEX_ABSENT kNtErrorIndexAbsent
#define ERROR_INSTALL_SOURCE_ABSENT kNtErrorInstallSourceAbsent
#define ERROR_INSTALL_PACKAGE_VERSION kNtErrorInstallPackageVersion
#define ERROR_PRODUCT_UNINSTALLED kNtErrorProductUninstalled
#define ERROR_BAD_QUERY_SYNTAX kNtErrorBadQuerySyntax
#define ERROR_INVALID_FIELD kNtErrorInvalidField
#define ERROR_DEVICE_REMOVED kNtErrorDeviceRemoved
#define ERROR_INSTALL_ALREADY_RUNNING kNtErrorInstallAlreadyRunning
#define ERROR_INSTALL_PACKAGE_OPEN_FAILED kNtErrorInstallPackageOpenFailed
#define ERROR_INSTALL_PACKAGE_INVALID kNtErrorInstallPackageInvalid
#define ERROR_INSTALL_UI_FAILURE kNtErrorInstallUiFailure
#define ERROR_INSTALL_LOG_FAILURE kNtErrorInstallLogFailure
#define ERROR_INSTALL_LANGUAGE_UNSUPPORTED kNtErrorInstallLanguageUnsupported
#define ERROR_INSTALL_TRANSFORM_FAILURE kNtErrorInstallTransformFailure
#define ERROR_INSTALL_PACKAGE_REJECTED kNtErrorInstallPackageRejected
#define ERROR_FUNCTION_NOT_CALLED kNtErrorFunctionNotCalled
#define ERROR_FUNCTION_FAILED kNtErrorFunctionFailed
#define ERROR_INVALID_TABLE kNtErrorInvalidTable
#define ERROR_DATATYPE_MISMATCH kNtErrorDatatypeMismatch
#define ERROR_UNSUPPORTED_TYPE kNtErrorUnsupportedType
#define ERROR_CREATE_FAILED kNtErrorCreateFailed
#define ERROR_INSTALL_TEMP_UNWRITABLE kNtErrorInstallTempUnwritable
#define ERROR_INSTALL_PLATFORM_UNSUPPORTED kNtErrorInstallPlatformUnsupported
#define ERROR_INSTALL_NOTUSED kNtErrorInstallNotused
#define ERROR_PATCH_PACKAGE_OPEN_FAILED kNtErrorPatchPackageOpenFailed
#define ERROR_PATCH_PACKAGE_INVALID kNtErrorPatchPackageInvalid
#define ERROR_PATCH_PACKAGE_UNSUPPORTED kNtErrorPatchPackageUnsupported
#define ERROR_PRODUCT_VERSION kNtErrorProductVersion
#define ERROR_INVALID_COMMAND_LINE kNtErrorInvalidCommandLine
#define ERROR_INSTALL_REMOTE_DISALLOWED kNtErrorInstallRemoteDisallowed
#define ERROR_SUCCESS_REBOOT_INITIATED kNtErrorSuccessRebootInitiated
#define ERROR_PATCH_TARGET_NOT_FOUND kNtErrorPatchTargetNotFound
#define ERROR_PATCH_PACKAGE_REJECTED kNtErrorPatchPackageRejected
#define ERROR_INSTALL_TRANSFORM_REJECTED kNtErrorInstallTransformRejected
#define ERROR_INSTALL_REMOTE_PROHIBITED kNtErrorInstallRemoteProhibited
#define ERROR_PATCH_REMOVAL_UNSUPPORTED kNtErrorPatchRemovalUnsupported
#define ERROR_UNKNOWN_PATCH kNtErrorUnknownPatch
#define ERROR_PATCH_NO_SEQUENCE kNtErrorPatchNoSequence
#define ERROR_PATCH_REMOVAL_DISALLOWED kNtErrorPatchRemovalDisallowed
#define ERROR_INVALID_PATCH_XML kNtErrorInvalidPatchXml
#define ERROR_PATCH_MANAGED_ADVERTISED_PRODUCT kNtErrorPatchManagedAdvertisedProduct
#define ERROR_INSTALL_SERVICE_SAFEBOOT kNtErrorInstallServiceSafeboot
#define ERROR_FAIL_FAST_EXCEPTION kNtErrorFailFastException
#define ERROR_INSTALL_REJECTED kNtErrorInstallRejected
#define ERROR_DYNAMIC_CODE_BLOCKED kNtErrorDynamicCodeBlocked
#define ERROR_NOT_SAME_OBJECT kNtErrorNotSameObject
#define ERROR_STRICT_CFG_VIOLATION kNtErrorStrictCfgViolation
#define ERROR_SET_CONTEXT_DENIED kNtErrorSetContextDenied
#define ERROR_CROSS_PARTITION_VIOLATION kNtErrorCrossPartitionViolation
#define ERROR_INVALID_USER_BUFFER kNtErrorInvalidUserBuffer
#define ERROR_UNRECOGNIZED_MEDIA kNtErrorUnrecognizedMedia
#define ERROR_NO_TRUST_LSA_SECRET kNtErrorNoTrustLsaSecret
#define ERROR_NO_TRUST_SAM_ACCOUNT kNtErrorNoTrustSamAccount
#define ERROR_TRUSTED_DOMAIN_FAILURE kNtErrorTrustedDomainFailure
#define ERROR_TRUSTED_RELATIONSHIP_FAILURE kNtErrorTrustedRelationshipFailure
#define ERROR_TRUST_FAILURE kNtErrorTrustFailure
#define ERROR_NETLOGON_NOT_STARTED kNtErrorNetlogonNotStarted
#define ERROR_ACCOUNT_EXPIRED kNtErrorAccountExpired
#define ERROR_REDIRECTOR_HAS_OPEN_HANDLES kNtErrorRedirectorHasOpenHandles
#define ERROR_PRINTER_DRIVER_ALREADY_INSTALLED kNtErrorPrinterDriverAlreadyInstalled
#define ERROR_UNKNOWN_PORT kNtErrorUnknownPort
#define ERROR_UNKNOWN_PRINTER_DRIVER kNtErrorUnknownPrinterDriver
#define ERROR_UNKNOWN_PRINTPROCESSOR kNtErrorUnknownPrintprocessor
#define ERROR_INVALID_SEPARATOR_FILE kNtErrorInvalidSeparatorFile
#define ERROR_INVALID_PRIORITY kNtErrorInvalidPriority
#define ERROR_INVALID_PRINTER_NAME kNtErrorInvalidPrinterName
#define ERROR_PRINTER_ALREADY_EXISTS kNtErrorPrinterAlreadyExists
#define ERROR_INVALID_PRINTER_COMMAND kNtErrorInvalidPrinterCommand
#define ERROR_INVALID_DATATYPE kNtErrorInvalidDatatype
#define ERROR_INVALID_ENVIRONMENT kNtErrorInvalidEnvironment
#define ERROR_NOLOGON_INTERDOMAIN_TRUST_ACCOUNT kNtErrorNologonInterdomainTrustAccount
#define ERROR_NOLOGON_WORKSTATION_TRUST_ACCOUNT kNtErrorNologonWorkstationTrustAccount
#define ERROR_NOLOGON_SERVER_TRUST_ACCOUNT kNtErrorNologonServerTrustAccount
#define ERROR_DOMAIN_TRUST_INCONSISTENT kNtErrorDomainTrustInconsistent
#define ERROR_SERVER_HAS_OPEN_HANDLES kNtErrorServerHasOpenHandles
#define ERROR_RESOURCE_DATA_NOT_FOUND kNtErrorResourceDataNotFound
#define ERROR_RESOURCE_TYPE_NOT_FOUND kNtErrorResourceTypeNotFound
#define ERROR_RESOURCE_NAME_NOT_FOUND kNtErrorResourceNameNotFound
#define ERROR_RESOURCE_LANG_NOT_FOUND kNtErrorResourceLangNotFound
#define ERROR_NOT_ENOUGH_QUOTA kNtErrorNotEnoughQuota
#define ERROR_INVALID_TIME kNtErrorInvalidTime
#define ERROR_INVALID_FORM_NAME kNtErrorInvalidFormName
#define ERROR_INVALID_FORM_SIZE kNtErrorInvalidFormSize
#define ERROR_ALREADY_WAITING kNtErrorAlreadyWaiting
#define ERROR_PRINTER_DELETED kNtErrorPrinterDeleted
#define ERROR_INVALID_PRINTER_STATE kNtErrorInvalidPrinterState
#define ERROR_PASSWORD_MUST_CHANGE kNtErrorPasswordMustChange
#define ERROR_DOMAIN_CONTROLLER_NOT_FOUND kNtErrorDomainControllerNotFound
#define ERROR_ACCOUNT_LOCKED_OUT kNtErrorAccountLockedOut
#define ERROR_NO_SITENAME kNtErrorNoSitename
#define ERROR_CANT_ACCESS_FILE kNtErrorCantAccessFile
#define ERROR_CANT_RESOLVE_FILENAME kNtErrorCantResolveFilename
#define ERROR_KM_DRIVER_BLOCKED kNtErrorKmDriverBlocked
#define ERROR_CONTEXT_EXPIRED kNtErrorContextExpired
#define ERROR_PER_USER_TRUST_QUOTA_EXCEEDED kNtErrorPerUserTrustQuotaExceeded
#define ERROR_ALL_USER_TRUST_QUOTA_EXCEEDED kNtErrorAllUserTrustQuotaExceeded
#define ERROR_USER_DELETE_TRUST_QUOTA_EXCEEDED kNtErrorUserDeleteTrustQuotaExceeded
#define ERROR_AUTHENTICATION_FIREWALL_FAILED kNtErrorAuthenticationFirewallFailed
#define ERROR_REMOTE_PRINT_CONNECTIONS_BLOCKED kNtErrorRemotePrintConnectionsBlocked
#define ERROR_NTLM_BLOCKED kNtErrorNtlmBlocked
#define ERROR_PASSWORD_CHANGE_REQUIRED kNtErrorPasswordChangeRequired
#define ERROR_LOST_MODE_LOGON_RESTRICTION kNtErrorLostModeLogonRestriction
#define ERROR_INVALID_PIXEL_FORMAT kNtErrorInvalidPixelFormat
#define ERROR_BAD_DRIVER kNtErrorBadDriver
#define ERROR_INVALID_WINDOW_STYLE kNtErrorInvalidWindowStyle
#define ERROR_METAFILE_NOT_SUPPORTED kNtErrorMetafileNotSupported
#define ERROR_TRANSFORM_NOT_SUPPORTED kNtErrorTransformNotSupported
#define ERROR_CLIPPING_NOT_SUPPORTED kNtErrorClippingNotSupported
#define ERROR_INVALID_CMM kNtErrorInvalidCmm
#define ERROR_INVALID_PROFILE kNtErrorInvalidProfile
#define ERROR_TAG_NOT_FOUND kNtErrorTagNotFound
#define ERROR_TAG_NOT_PRESENT kNtErrorTagNotPresent
#define ERROR_DUPLICATE_TAG kNtErrorDuplicateTag
#define ERROR_PROFILE_NOT_ASSOCIATED_WITH_DEVICE kNtErrorProfileNotAssociatedWithDevice
#define ERROR_PROFILE_NOT_FOUND kNtErrorProfileNotFound
#define ERROR_INVALID_COLORSPACE kNtErrorInvalidColorspace
#define ERROR_ICM_NOT_ENABLED kNtErrorIcmNotEnabled
#define ERROR_DELETING_ICM_XFORM kNtErrorDeletingIcmXform
#define ERROR_INVALID_TRANSFORM kNtErrorInvalidTransform
#define ERROR_COLORSPACE_MISMATCH kNtErrorColorspaceMismatch
#define ERROR_INVALID_COLORINDEX kNtErrorInvalidColorindex
#define ERROR_PROFILE_DOES_NOT_MATCH_DEVICE kNtErrorProfileDoesNotMatchDevice
#define ERROR_CONNECTED_OTHER_PASSWORD kNtErrorConnectedOtherPassword
#define ERROR_CONNECTED_OTHER_PASSWORD_DEFAULT kNtErrorConnectedOtherPasswordDefault
#define ERROR_BAD_USERNAME kNtErrorBadUsername
#define ERROR_NOT_CONNECTED kNtErrorNotConnected
#define ERROR_OPEN_FILES kNtErrorOpenFiles
#define ERROR_ACTIVE_CONNECTIONS kNtErrorActiveConnections
#define ERROR_DEVICE_IN_USE kNtErrorDeviceInUse
#define ERROR_UNKNOWN_PRINT_MONITOR kNtErrorUnknownPrintMonitor
#define ERROR_PRINTER_DRIVER_IN_USE kNtErrorPrinterDriverInUse
#define ERROR_SPOOL_FILE_NOT_FOUND kNtErrorSpoolFileNotFound
#define ERROR_SPL_NO_STARTDOC kNtErrorSplNoStartdoc
#define ERROR_SPL_NO_ADDJOB kNtErrorSplNoAddjob
#define ERROR_PRINT_PROCESSOR_ALREADY_INSTALLED kNtErrorPrintProcessorAlreadyInstalled
#define ERROR_PRINT_MONITOR_ALREADY_INSTALLED kNtErrorPrintMonitorAlreadyInstalled
#define ERROR_INVALID_PRINT_MONITOR kNtErrorInvalidPrintMonitor
#define ERROR_PRINT_MONITOR_IN_USE kNtErrorPrintMonitorInUse
#define ERROR_PRINTER_HAS_JOBS_QUEUED kNtErrorPrinterHasJobsQueued
#define ERROR_SUCCESS_REBOOT_REQUIRED kNtErrorSuccessRebootRequired
#define ERROR_SUCCESS_RESTART_REQUIRED kNtErrorSuccessRestartRequired
#define ERROR_PRINTER_NOT_FOUND kNtErrorPrinterNotFound
#define ERROR_PRINTER_DRIVER_WARNED kNtErrorPrinterDriverWarned
#define ERROR_PRINTER_DRIVER_BLOCKED kNtErrorPrinterDriverBlocked
#define ERROR_PRINTER_DRIVER_PACKAGE_IN_USE kNtErrorPrinterDriverPackageInUse
#define ERROR_CORE_DRIVER_PACKAGE_NOT_FOUND kNtErrorCoreDriverPackageNotFound
#define ERROR_FAIL_REBOOT_REQUIRED kNtErrorFailRebootRequired
#define ERROR_FAIL_REBOOT_INITIATED kNtErrorFailRebootInitiated
#define ERROR_PRINTER_DRIVER_DOWNLOAD_NEEDED kNtErrorPrinterDriverDownloadNeeded
#define ERROR_PRINT_JOB_RESTART_REQUIRED kNtErrorPrintJobRestartRequired
#define ERROR_INVALID_PRINTER_DRIVER_MANIFEST kNtErrorInvalidPrinterDriverManifest
#define ERROR_PRINTER_NOT_SHAREABLE kNtErrorPrinterNotShareable
#define ERROR_REQUEST_PAUSED kNtErrorRequestPaused
#define ERROR_APPEXEC_CONDITION_NOT_SATISFIED kNtErrorAppexecConditionNotSatisfied
#define ERROR_APPEXEC_HANDLE_INVALIDATED kNtErrorAppexecHandleInvalidated
#define ERROR_APPEXEC_INVALID_HOST_GENERATION kNtErrorAppexecInvalidHostGeneration
#define ERROR_APPEXEC_UNEXPECTED_PROCESS_REGISTRATION kNtErrorAppexecUnexpectedProcessRegistration
#define ERROR_APPEXEC_INVALID_HOST_STATE kNtErrorAppexecInvalidHostState
#define ERROR_APPEXEC_NO_DONOR kNtErrorAppexecNoDonor
#define ERROR_APPEXEC_HOST_ID_MISMATCH kNtErrorAppexecHostIdMismatch
#define ERROR_IO_REISSUE_AS_CACHED kNtErrorIoReissueAsCached
#define ERROR_WINS_INTERNAL kNtErrorWinsInternal
#define ERROR_CAN_NOT_DEL_LOCAL_WINS kNtErrorCanNotDelLocalWins
#define ERROR_STATIC_INIT kNtErrorStaticInit
#define ERROR_INC_BACKUP kNtErrorIncBackup
#define ERROR_FULL_BACKUP kNtErrorFullBackup
#define ERROR_REC_NON_EXISTENT kNtErrorRecNonExistent
#define ERROR_RPL_NOT_ALLOWED kNtErrorRplNotAllowed
#define ERROR_DHCP_ADDRESS_CONFLICT kNtErrorDhcpAddressConflict
#define ERROR_WMI_GUID_NOT_FOUND kNtErrorWmiGuidNotFound
#define ERROR_WMI_INSTANCE_NOT_FOUND kNtErrorWmiInstanceNotFound
#define ERROR_WMI_ITEMID_NOT_FOUND kNtErrorWmiItemidNotFound
#define ERROR_WMI_TRY_AGAIN kNtErrorWmiTryAgain
#define ERROR_WMI_DP_NOT_FOUND kNtErrorWmiDpNotFound
#define ERROR_WMI_UNRESOLVED_INSTANCE_REF kNtErrorWmiUnresolvedInstanceRef
#define ERROR_WMI_ALREADY_ENABLED kNtErrorWmiAlreadyEnabled
#define ERROR_WMI_GUID_DISCONNECTED kNtErrorWmiGuidDisconnected
#define ERROR_WMI_SERVER_UNAVAILABLE kNtErrorWmiServerUnavailable
#define ERROR_WMI_DP_FAILED kNtErrorWmiDpFailed
#define ERROR_WMI_INVALID_MOF kNtErrorWmiInvalidMof
#define ERROR_WMI_INVALID_REGINFO kNtErrorWmiInvalidReginfo
#define ERROR_WMI_ALREADY_DISABLED kNtErrorWmiAlreadyDisabled
#define ERROR_WMI_READ_ONLY kNtErrorWmiReadOnly
#define ERROR_WMI_SET_FAILURE kNtErrorWmiSetFailure
#define ERROR_NOT_APPCONTAINER kNtErrorNotAppcontainer
#define ERROR_APPCONTAINER_REQUIRED kNtErrorAppcontainerRequired
#define ERROR_NOT_SUPPORTED_IN_APPCONTAINER kNtErrorNotSupportedInAppcontainer
#define ERROR_INVALID_PACKAGE_SID_LENGTH kNtErrorInvalidPackageSidLength
#define ERROR_INVALID_MEDIA kNtErrorInvalidMedia
#define ERROR_INVALID_LIBRARY kNtErrorInvalidLibrary
#define ERROR_INVALID_MEDIA_POOL kNtErrorInvalidMediaPool
#define ERROR_DRIVE_MEDIA_MISMATCH kNtErrorDriveMediaMismatch
#define ERROR_MEDIA_OFFLINE kNtErrorMediaOffline
#define ERROR_LIBRARY_OFFLINE kNtErrorLibraryOffline
#define ERROR_EMPTY kNtErrorEmpty
#define ERROR_NOT_EMPTY kNtErrorNotEmpty
#define ERROR_MEDIA_UNAVAILABLE kNtErrorMediaUnavailable
#define ERROR_RESOURCE_DISABLED kNtErrorResourceDisabled
#define ERROR_INVALID_CLEANER kNtErrorInvalidCleaner
#define ERROR_UNABLE_TO_CLEAN kNtErrorUnableToClean
#define ERROR_OBJECT_NOT_FOUND kNtErrorObjectNotFound
#define ERROR_DATABASE_FAILURE kNtErrorDatabaseFailure
#define ERROR_DATABASE_FULL kNtErrorDatabaseFull
#define ERROR_MEDIA_INCOMPATIBLE kNtErrorMediaIncompatible
#define ERROR_RESOURCE_NOT_PRESENT kNtErrorResourceNotPresent
#define ERROR_INVALID_OPERATION kNtErrorInvalidOperation
#define ERROR_MEDIA_NOT_AVAILABLE kNtErrorMediaNotAvailable
#define ERROR_DEVICE_NOT_AVAILABLE kNtErrorDeviceNotAvailable
#define ERROR_REQUEST_REFUSED kNtErrorRequestRefused
#define ERROR_INVALID_DRIVE_OBJECT kNtErrorInvalidDriveObject
#define ERROR_LIBRARY_FULL kNtErrorLibraryFull
#define ERROR_MEDIUM_NOT_ACCESSIBLE kNtErrorMediumNotAccessible
#define ERROR_UNABLE_TO_LOAD_MEDIUM kNtErrorUnableToLoadMedium
#define ERROR_UNABLE_TO_INVENTORY_DRIVE kNtErrorUnableToInventoryDrive
#define ERROR_UNABLE_TO_INVENTORY_SLOT kNtErrorUnableToInventorySlot
#define ERROR_UNABLE_TO_INVENTORY_TRANSPORT kNtErrorUnableToInventoryTransport
#define ERROR_TRANSPORT_FULL kNtErrorTransportFull
#define ERROR_CONTROLLING_IEPORT kNtErrorControllingIeport
#define ERROR_UNABLE_TO_EJECT_MOUNTED_MEDIA kNtErrorUnableToEjectMountedMedia
#define ERROR_CLEANER_SLOT_SET kNtErrorCleanerSlotSet
#define ERROR_CLEANER_SLOT_NOT_SET kNtErrorCleanerSlotNotSet
#define ERROR_CLEANER_CARTRIDGE_SPENT kNtErrorCleanerCartridgeSpent
#define ERROR_UNEXPECTED_OMID kNtErrorUnexpectedOmid
#define ERROR_CANT_DELETE_LAST_ITEM kNtErrorCantDeleteLastItem
#define ERROR_MESSAGE_EXCEEDS_MAX_SIZE kNtErrorMessageExceedsMaxSize
#define ERROR_VOLUME_CONTAINS_SYS_FILES kNtErrorVolumeContainsSysFiles
#define ERROR_INDIGENOUS_TYPE kNtErrorIndigenousType
#define ERROR_NO_SUPPORTING_DRIVES kNtErrorNoSupportingDrives
#define ERROR_CLEANER_CARTRIDGE_INSTALLED kNtErrorCleanerCartridgeInstalled
#define ERROR_IEPORT_FULL kNtErrorIeportFull
#define ERROR_FILE_OFFLINE kNtErrorFileOffline
#define ERROR_REMOTE_STORAGE_NOT_ACTIVE kNtErrorRemoteStorageNotActive
#define ERROR_REMOTE_STORAGE_MEDIA_ERROR kNtErrorRemoteStorageMediaError
#define ERROR_NOT_AREPARSE_POINT kNtErrorNotAReparsePoint
#define ERROR_REPARSE_ATTRIBUTE_CONFLICT kNtErrorReparseAttributeConflict
#define ERROR_INVALID_REPARSE_DATA kNtErrorInvalidReparseData
#define ERROR_REPARSE_TAG_INVALID kNtErrorReparseTagInvalid
#define ERROR_REPARSE_TAG_MISMATCH kNtErrorReparseTagMismatch
#define ERROR_REPARSE_POINT_ENCOUNTERED kNtErrorReparsePointEncountered
#define ERROR_APP_DATA_NOT_FOUND kNtErrorAppDataNotFound
#define ERROR_APP_DATA_EXPIRED kNtErrorAppDataExpired
#define ERROR_APP_DATA_CORRUPT kNtErrorAppDataCorrupt
#define ERROR_APP_DATA_LIMIT_EXCEEDED kNtErrorAppDataLimitExceeded
#define ERROR_APP_DATA_REBOOT_REQUIRED kNtErrorAppDataRebootRequired
#define ERROR_SECUREBOOT_ROLLBACK_DETECTED kNtErrorSecurebootRollbackDetected
#define ERROR_SECUREBOOT_POLICY_VIOLATION kNtErrorSecurebootPolicyViolation
#define ERROR_SECUREBOOT_INVALID_POLICY kNtErrorSecurebootInvalidPolicy
#define ERROR_SECUREBOOT_POLICY_PUBLISHER_NOT_FOUND kNtErrorSecurebootPolicyPublisherNotFound
#define ERROR_SECUREBOOT_POLICY_NOT_SIGNED kNtErrorSecurebootPolicyNotSigned
#define ERROR_SECUREBOOT_NOT_ENABLED kNtErrorSecurebootNotEnabled
#define ERROR_SECUREBOOT_FILE_REPLACED kNtErrorSecurebootFileReplaced
#define ERROR_SECUREBOOT_POLICY_NOT_AUTHORIZED kNtErrorSecurebootPolicyNotAuthorized
#define ERROR_SECUREBOOT_POLICY_UNKNOWN kNtErrorSecurebootPolicyUnknown
#define ERROR_SECUREBOOT_POLICY_MISSING_ANTIROLLBACKVERSION kNtErrorSecurebootPolicyMissingAntirollbackversion
#define ERROR_SECUREBOOT_PLATFORM_ID_MISMATCH kNtErrorSecurebootPlatformIdMismatch
#define ERROR_SECUREBOOT_POLICY_ROLLBACK_DETECTED kNtErrorSecurebootPolicyRollbackDetected
#define ERROR_SECUREBOOT_POLICY_UPGRADE_MISMATCH kNtErrorSecurebootPolicyUpgradeMismatch
#define ERROR_SECUREBOOT_REQUIRED_POLICY_FILE_MISSING kNtErrorSecurebootRequiredPolicyFileMissing
#define ERROR_SECUREBOOT_NOT_BASE_POLICY kNtErrorSecurebootNotBasePolicy
#define ERROR_SECUREBOOT_NOT_SUPPLEMENTAL_POLICY kNtErrorSecurebootNotSupplementalPolicy
#define ERROR_OFFLOAD_READ_FLT_NOT_SUPPORTED kNtErrorOffloadReadFltNotSupported
#define ERROR_OFFLOAD_WRITE_FLT_NOT_SUPPORTED kNtErrorOffloadWriteFltNotSupported
#define ERROR_OFFLOAD_READ_FILE_NOT_SUPPORTED kNtErrorOffloadReadFileNotSupported
#define ERROR_OFFLOAD_WRITE_FILE_NOT_SUPPORTED kNtErrorOffloadWriteFileNotSupported
#define ERROR_ALREADY_HAS_STREAM_ID kNtErrorAlreadyHasStreamId
#define ERROR_SMR_GARBAGE_COLLECTION_REQUIRED kNtErrorSmrGarbageCollectionRequired
#define ERROR_WOF_WIM_HEADER_CORRUPT kNtErrorWofWimHeaderCorrupt
#define ERROR_WOF_WIM_RESOURCE_TABLE_CORRUPT kNtErrorWofWimResourceTableCorrupt
#define ERROR_WOF_FILE_RESOURCE_TABLE_CORRUPT kNtErrorWofFileResourceTableCorrupt
#define ERROR_VOLUME_NOT_SIS_ENABLED kNtErrorVolumeNotSisEnabled
#define ERROR_SYSTEM_INTEGRITY_ROLLBACK_DETECTED kNtErrorSystemIntegrityRollbackDetected
#define ERROR_SYSTEM_INTEGRITY_POLICY_VIOLATION kNtErrorSystemIntegrityPolicyViolation
#define ERROR_SYSTEM_INTEGRITY_INVALID_POLICY kNtErrorSystemIntegrityInvalidPolicy
#define ERROR_SYSTEM_INTEGRITY_POLICY_NOT_SIGNED kNtErrorSystemIntegrityPolicyNotSigned
#define ERROR_VSM_NOT_INITIALIZED kNtErrorVsmNotInitialized
#define ERROR_VSM_DMA_PROTECTION_NOT_IN_USE kNtErrorVsmDmaProtectionNotInUse
#define ERROR_PLATFORM_MANIFEST_NOT_AUTHORIZED kNtErrorPlatformManifestNotAuthorized
#define ERROR_PLATFORM_MANIFEST_INVALID kNtErrorPlatformManifestInvalid
#define ERROR_PLATFORM_MANIFEST_FILE_NOT_AUTHORIZED kNtErrorPlatformManifestFileNotAuthorized
#define ERROR_PLATFORM_MANIFEST_CATALOG_NOT_AUTHORIZED kNtErrorPlatformManifestCatalogNotAuthorized
#define ERROR_PLATFORM_MANIFEST_BINARY_ID_NOT_FOUND kNtErrorPlatformManifestBinaryIdNotFound
#define ERROR_PLATFORM_MANIFEST_NOT_ACTIVE kNtErrorPlatformManifestNotActive
#define ERROR_PLATFORM_MANIFEST_NOT_SIGNED kNtErrorPlatformManifestNotSigned
#define ERROR_DEPENDENT_RESOURCE_EXISTS kNtErrorDependentResourceExists
#define ERROR_DEPENDENCY_NOT_FOUND kNtErrorDependencyNotFound
#define ERROR_DEPENDENCY_ALREADY_EXISTS kNtErrorDependencyAlreadyExists
#define ERROR_RESOURCE_NOT_ONLINE kNtErrorResourceNotOnline
#define ERROR_HOST_NODE_NOT_AVAILABLE kNtErrorHostNodeNotAvailable
#define ERROR_RESOURCE_NOT_AVAILABLE kNtErrorResourceNotAvailable
#define ERROR_RESOURCE_NOT_FOUND kNtErrorResourceNotFound
#define ERROR_SHUTDOWN_CLUSTER kNtErrorShutdownCluster
#define ERROR_CANT_EVICT_ACTIVE_NODE kNtErrorCantEvictActiveNode
#define ERROR_OBJECT_ALREADY_EXISTS kNtErrorObjectAlreadyExists
#define ERROR_OBJECT_IN_LIST kNtErrorObjectInList
#define ERROR_GROUP_NOT_AVAILABLE kNtErrorGroupNotAvailable
#define ERROR_GROUP_NOT_FOUND kNtErrorGroupNotFound
#define ERROR_GROUP_NOT_ONLINE kNtErrorGroupNotOnline
#define ERROR_HOST_NODE_NOT_RESOURCE_OWNER kNtErrorHostNodeNotResourceOwner
#define ERROR_HOST_NODE_NOT_GROUP_OWNER kNtErrorHostNodeNotGroupOwner
#define ERROR_RESMON_CREATE_FAILED kNtErrorResmonCreateFailed
#define ERROR_RESMON_ONLINE_FAILED kNtErrorResmonOnlineFailed
#define ERROR_RESOURCE_ONLINE kNtErrorResourceOnline
#define ERROR_QUORUM_RESOURCE kNtErrorQuorumResource
#define ERROR_NOT_QUORUM_CAPABLE kNtErrorNotQuorumCapable
#define ERROR_CLUSTER_SHUTTING_DOWN kNtErrorClusterShuttingDown
#define ERROR_INVALID_STATE kNtErrorInvalidState
#define ERROR_RESOURCE_PROPERTIES_STORED kNtErrorResourcePropertiesStored
#define ERROR_NOT_QUORUM_CLASS kNtErrorNotQuorumClass
#define ERROR_CORE_RESOURCE kNtErrorCoreResource
#define ERROR_QUORUM_RESOURCE_ONLINE_FAILED kNtErrorQuorumResourceOnlineFailed
#define ERROR_QUORUMLOG_OPEN_FAILED kNtErrorQuorumlogOpenFailed
#define ERROR_CLUSTERLOG_CORRUPT kNtErrorClusterlogCorrupt
#define ERROR_CLUSTERLOG_RECORD_EXCEEDS_MAXSIZE kNtErrorClusterlogRecordExceedsMaxsize
#define ERROR_CLUSTERLOG_EXCEEDS_MAXSIZE kNtErrorClusterlogExceedsMaxsize
#define ERROR_CLUSTERLOG_CHKPOINT_NOT_FOUND kNtErrorClusterlogChkpointNotFound
#define ERROR_CLUSTERLOG_NOT_ENOUGH_SPACE kNtErrorClusterlogNotEnoughSpace
#define ERROR_QUORUM_OWNER_ALIVE kNtErrorQuorumOwnerAlive
#define ERROR_NETWORK_NOT_AVAILABLE kNtErrorNetworkNotAvailable
#define ERROR_NODE_NOT_AVAILABLE kNtErrorNodeNotAvailable
#define ERROR_ALL_NODES_NOT_AVAILABLE kNtErrorAllNodesNotAvailable
#define ERROR_RESOURCE_FAILED kNtErrorResourceFailed
#define ERROR_CLUSTER_INVALID_NODE kNtErrorClusterInvalidNode
#define ERROR_CLUSTER_NODE_EXISTS kNtErrorClusterNodeExists
#define ERROR_CLUSTER_JOIN_IN_PROGRESS kNtErrorClusterJoinInProgress
#define ERROR_CLUSTER_NODE_NOT_FOUND kNtErrorClusterNodeNotFound
#define ERROR_CLUSTER_LOCAL_NODE_NOT_FOUND kNtErrorClusterLocalNodeNotFound
#define ERROR_CLUSTER_NETWORK_EXISTS kNtErrorClusterNetworkExists
#define ERROR_CLUSTER_NETWORK_NOT_FOUND kNtErrorClusterNetworkNotFound
#define ERROR_CLUSTER_NETINTERFACE_EXISTS kNtErrorClusterNetinterfaceExists
#define ERROR_CLUSTER_NETINTERFACE_NOT_FOUND kNtErrorClusterNetinterfaceNotFound
#define ERROR_CLUSTER_INVALID_REQUEST kNtErrorClusterInvalidRequest
#define ERROR_CLUSTER_INVALID_NETWORK_PROVIDER kNtErrorClusterInvalidNetworkProvider
#define ERROR_CLUSTER_NODE_DOWN kNtErrorClusterNodeDown
#define ERROR_CLUSTER_NODE_UNREACHABLE kNtErrorClusterNodeUnreachable
#define ERROR_CLUSTER_NODE_NOT_MEMBER kNtErrorClusterNodeNotMember
#define ERROR_CLUSTER_JOIN_NOT_IN_PROGRESS kNtErrorClusterJoinNotInProgress
#define ERROR_CLUSTER_INVALID_NETWORK kNtErrorClusterInvalidNetwork
#define ERROR_CLUSTER_NODE_UP kNtErrorClusterNodeUp
#define ERROR_CLUSTER_IPADDR_IN_USE kNtErrorClusterIpaddrInUse
#define ERROR_CLUSTER_NODE_NOT_PAUSED kNtErrorClusterNodeNotPaused
#define ERROR_CLUSTER_NO_SECURITY_CONTEXT kNtErrorClusterNoSecurityContext
#define ERROR_CLUSTER_NETWORK_NOT_INTERNAL kNtErrorClusterNetworkNotInternal
#define ERROR_CLUSTER_NODE_ALREADY_UP kNtErrorClusterNodeAlreadyUp
#define ERROR_CLUSTER_NODE_ALREADY_DOWN kNtErrorClusterNodeAlreadyDown
#define ERROR_CLUSTER_NETWORK_ALREADY_ONLINE kNtErrorClusterNetworkAlreadyOnline
#define ERROR_CLUSTER_NETWORK_ALREADY_OFFLINE kNtErrorClusterNetworkAlreadyOffline
#define ERROR_CLUSTER_NODE_ALREADY_MEMBER kNtErrorClusterNodeAlreadyMember
#define ERROR_CLUSTER_LAST_INTERNAL_NETWORK kNtErrorClusterLastInternalNetwork
#define ERROR_CLUSTER_NETWORK_HAS_DEPENDENTS kNtErrorClusterNetworkHasDependents
#define ERROR_INVALID_OPERATION_ON_QUORUM kNtErrorInvalidOperationOnQuorum
#define ERROR_DEPENDENCY_NOT_ALLOWED kNtErrorDependencyNotAllowed
#define ERROR_CLUSTER_NODE_PAUSED kNtErrorClusterNodePaused
#define ERROR_NODE_CANT_HOST_RESOURCE kNtErrorNodeCantHostResource
#define ERROR_CLUSTER_NODE_NOT_READY kNtErrorClusterNodeNotReady
#define ERROR_CLUSTER_NODE_SHUTTING_DOWN kNtErrorClusterNodeShuttingDown
#define ERROR_CLUSTER_JOIN_ABORTED kNtErrorClusterJoinAborted
#define ERROR_CLUSTER_INCOMPATIBLE_VERSIONS kNtErrorClusterIncompatibleVersions
#define ERROR_CLUSTER_MAXNUM_OF_RESOURCES_EXCEEDED kNtErrorClusterMaxnumOfResourcesExceeded
#define ERROR_CLUSTER_SYSTEM_CONFIG_CHANGED kNtErrorClusterSystemConfigChanged
#define ERROR_CLUSTER_RESOURCE_TYPE_NOT_FOUND kNtErrorClusterResourceTypeNotFound
#define ERROR_CLUSTER_RESTYPE_NOT_SUPPORTED kNtErrorClusterRestypeNotSupported
#define ERROR_CLUSTER_RESNAME_NOT_FOUND kNtErrorClusterResnameNotFound
#define ERROR_CLUSTER_NO_RPC_PACKAGES_REGISTERED kNtErrorClusterNoRpcPackagesRegistered
#define ERROR_CLUSTER_OWNER_NOT_IN_PREFLIST kNtErrorClusterOwnerNotInPreflist
#define ERROR_CLUSTER_DATABASE_SEQMISMATCH kNtErrorClusterDatabaseSeqmismatch
#define ERROR_RESMON_INVALID_STATE kNtErrorResmonInvalidState
#define ERROR_CLUSTER_GUM_NOT_LOCKER kNtErrorClusterGumNotLocker
#define ERROR_QUORUM_DISK_NOT_FOUND kNtErrorQuorumDiskNotFound
#define ERROR_DATABASE_BACKUP_CORRUPT kNtErrorDatabaseBackupCorrupt
#define ERROR_CLUSTER_NODE_ALREADY_HAS_DFS_ROOT kNtErrorClusterNodeAlreadyHasDfsRoot
#define ERROR_RESOURCE_PROPERTY_UNCHANGEABLE kNtErrorResourcePropertyUnchangeable
#define ERROR_NO_ADMIN_ACCESS_POINT kNtErrorNoAdminAccessPoint
#define ERROR_CLUSTER_MEMBERSHIP_INVALID_STATE kNtErrorClusterMembershipInvalidState
#define ERROR_CLUSTER_QUORUMLOG_NOT_FOUND kNtErrorClusterQuorumlogNotFound
#define ERROR_CLUSTER_MEMBERSHIP_HALT kNtErrorClusterMembershipHalt
#define ERROR_CLUSTER_INSTANCE_ID_MISMATCH kNtErrorClusterInstanceIdMismatch
#define ERROR_CLUSTER_NETWORK_NOT_FOUND_FOR_IP kNtErrorClusterNetworkNotFoundForIp
#define ERROR_CLUSTER_PROPERTY_DATA_TYPE_MISMATCH kNtErrorClusterPropertyDataTypeMismatch
#define ERROR_CLUSTER_EVICT_WITHOUT_CLEANUP kNtErrorClusterEvictWithoutCleanup
#define ERROR_CLUSTER_PARAMETER_MISMATCH kNtErrorClusterParameterMismatch
#define ERROR_NODE_CANNOT_BE_CLUSTERED kNtErrorNodeCannotBeClustered
#define ERROR_CLUSTER_WRONG_OS_VERSION kNtErrorClusterWrongOsVersion
#define ERROR_CLUSTER_CANT_CREATE_DUP_CLUSTER_NAME kNtErrorClusterCantCreateDupClusterName
#define ERROR_CLUSCFG_ALREADY_COMMITTED kNtErrorCluscfgAlreadyCommitted
#define ERROR_CLUSCFG_ROLLBACK_FAILED kNtErrorCluscfgRollbackFailed
#define ERROR_CLUSCFG_SYSTEM_DISK_DRIVE_LETTER_CONFLICT kNtErrorCluscfgSystemDiskDriveLetterConflict
#define ERROR_CLUSTER_OLD_VERSION kNtErrorClusterOldVersion
#define ERROR_CLUSTER_MISMATCHED_COMPUTER_ACCT_NAME kNtErrorClusterMismatchedComputerAcctName
#define ERROR_CLUSTER_NO_NET_ADAPTERS kNtErrorClusterNoNetAdapters
#define ERROR_CLUSTER_POISONED kNtErrorClusterPoisoned
#define ERROR_CLUSTER_GROUP_MOVING kNtErrorClusterGroupMoving
#define ERROR_CLUSTER_RESOURCE_TYPE_BUSY kNtErrorClusterResourceTypeBusy
#define ERROR_RESOURCE_CALL_TIMED_OUT kNtErrorResourceCallTimedOut
#define ERROR_INVALID_CLUSTER_IPV6_ADDRESS kNtErrorInvalidClusterIpv6Address
#define ERROR_CLUSTER_INTERNAL_INVALID_FUNCTION kNtErrorClusterInternalInvalidFunction
#define ERROR_CLUSTER_PARAMETER_OUT_OF_BOUNDS kNtErrorClusterParameterOutOfBounds
#define ERROR_CLUSTER_PARTIAL_SEND kNtErrorClusterPartialSend
#define ERROR_CLUSTER_REGISTRY_INVALID_FUNCTION kNtErrorClusterRegistryInvalidFunction
#define ERROR_CLUSTER_INVALID_STRING_TERMINATION kNtErrorClusterInvalidStringTermination
#define ERROR_CLUSTER_INVALID_STRING_FORMAT kNtErrorClusterInvalidStringFormat
#define ERROR_CLUSTER_DATABASE_TRANSACTION_IN_PROGRESS kNtErrorClusterDatabaseTransactionInProgress
#define ERROR_CLUSTER_DATABASE_TRANSACTION_NOT_IN_PROGRESS kNtErrorClusterDatabaseTransactionNotInProgress
#define ERROR_CLUSTER_NULL_DATA kNtErrorClusterNullData
#define ERROR_CLUSTER_PARTIAL_READ kNtErrorClusterPartialRead
#define ERROR_CLUSTER_PARTIAL_WRITE kNtErrorClusterPartialWrite
#define ERROR_CLUSTER_CANT_DESERIALIZE_DATA kNtErrorClusterCantDeserializeData
#define ERROR_DEPENDENT_RESOURCE_PROPERTY_CONFLICT kNtErrorDependentResourcePropertyConflict
#define ERROR_CLUSTER_NO_QUORUM kNtErrorClusterNoQuorum
#define ERROR_CLUSTER_INVALID_IPV6_NETWORK kNtErrorClusterInvalidIpv6Network
#define ERROR_CLUSTER_INVALID_IPV6_TUNNEL_NETWORK kNtErrorClusterInvalidIpv6TunnelNetwork
#define ERROR_QUORUM_NOT_ALLOWED_IN_THIS_GROUP kNtErrorQuorumNotAllowedInThisGroup
#define ERROR_DEPENDENCY_TREE_TOO_COMPLEX kNtErrorDependencyTreeTooComplex
#define ERROR_EXCEPTION_IN_RESOURCE_CALL kNtErrorExceptionInResourceCall
#define ERROR_CLUSTER_RHS_FAILED_INITIALIZATION kNtErrorClusterRhsFailedInitialization
#define ERROR_CLUSTER_NOT_INSTALLED kNtErrorClusterNotInstalled
#define ERROR_CLUSTER_RESOURCES_MUST_BE_ONLINE_ON_THE_SAME_NODE kNtErrorClusterResourcesMustBeOnlineOnTheSameNode
#define ERROR_CLUSTER_MAX_NODES_IN_CLUSTER kNtErrorClusterMaxNodesInCluster
#define ERROR_CLUSTER_TOO_MANY_NODES kNtErrorClusterTooManyNodes
#define ERROR_CLUSTER_OBJECT_ALREADY_USED kNtErrorClusterObjectAlreadyUsed
#define ERROR_NONCORE_GROUPS_FOUND kNtErrorNoncoreGroupsFound
#define ERROR_FILE_SHARE_RESOURCE_CONFLICT kNtErrorFileShareResourceConflict
#define ERROR_CLUSTER_EVICT_INVALID_REQUEST kNtErrorClusterEvictInvalidRequest
#define ERROR_CLUSTER_SINGLETON_RESOURCE kNtErrorClusterSingletonResource
#define ERROR_CLUSTER_GROUP_SINGLETON_RESOURCE kNtErrorClusterGroupSingletonResource
#define ERROR_CLUSTER_RESOURCE_PROVIDER_FAILED kNtErrorClusterResourceProviderFailed
#define ERROR_CLUSTER_RESOURCE_CONFIGURATION_ERROR kNtErrorClusterResourceConfigurationError
#define ERROR_CLUSTER_GROUP_BUSY kNtErrorClusterGroupBusy
#define ERROR_CLUSTER_NOT_SHARED_VOLUME kNtErrorClusterNotSharedVolume
#define ERROR_CLUSTER_INVALID_SECURITY_DESCRIPTOR kNtErrorClusterInvalidSecurityDescriptor
#define ERROR_CLUSTER_SHARED_VOLUMES_IN_USE kNtErrorClusterSharedVolumesInUse
#define ERROR_CLUSTER_USE_SHARED_VOLUMES_API kNtErrorClusterUseSharedVolumesApi
#define ERROR_CLUSTER_BACKUP_IN_PROGRESS kNtErrorClusterBackupInProgress
#define ERROR_NON_CSV_PATH kNtErrorNonCsvPath
#define ERROR_CSV_VOLUME_NOT_LOCAL kNtErrorCsvVolumeNotLocal
#define ERROR_CLUSTER_WATCHDOG_TERMINATING kNtErrorClusterWatchdogTerminating
#define ERROR_CLUSTER_RESOURCE_VETOED_MOVE_INCOMPATIBLE_NODES kNtErrorClusterResourceVetoedMoveIncompatibleNodes
#define ERROR_CLUSTER_INVALID_NODE_WEIGHT kNtErrorClusterInvalidNodeWeight
#define ERROR_CLUSTER_RESOURCE_VETOED_CALL kNtErrorClusterResourceVetoedCall
#define ERROR_RESMON_SYSTEM_RESOURCES_LACKING kNtErrorResmonSystemResourcesLacking
#define ERROR_CLUSTER_RESOURCE_VETOED_MOVE_NOT_ENOUGH_RESOURCES_ON_SOURCE kNtErrorClusterResourceVetoedMoveNotEnoughResourcesOnSource
#define ERROR_CLUSTER_GROUP_QUEUED kNtErrorClusterGroupQueued
#define ERROR_CLUSTER_RESOURCE_LOCKED_STATUS kNtErrorClusterResourceLockedStatus
#define ERROR_CLUSTER_SHARED_VOLUME_FAILOVER_NOT_ALLOWED kNtErrorClusterSharedVolumeFailoverNotAllowed
#define ERROR_CLUSTER_NODE_DRAIN_IN_PROGRESS kNtErrorClusterNodeDrainInProgress
#define ERROR_CLUSTER_DISK_NOT_CONNECTED kNtErrorClusterDiskNotConnected
#define ERROR_DISK_NOT_CSV_CAPABLE kNtErrorDiskNotCsvCapable
#define ERROR_RESOURCE_NOT_IN_AVAILABLE_STORAGE kNtErrorResourceNotInAvailableStorage
#define ERROR_CLUSTER_SHARED_VOLUME_REDIRECTED kNtErrorClusterSharedVolumeRedirected
#define ERROR_CLUSTER_SHARED_VOLUME_NOT_REDIRECTED kNtErrorClusterSharedVolumeNotRedirected
#define ERROR_CLUSTER_CANNOT_RETURN_PROPERTIES kNtErrorClusterCannotReturnProperties
#define ERROR_CLUSTER_RESOURCE_IS_IN_MAINTENANCE_MODE kNtErrorClusterResourceIsInMaintenanceMode
#define ERROR_CLUSTER_AFFINITY_CONFLICT kNtErrorClusterAffinityConflict
#define ERROR_CLUSTER_RESOURCE_IS_REPLICA_VIRTUAL_MACHINE kNtErrorClusterResourceIsReplicaVirtualMachine
#define ERROR_CLUSTER_UPGRADE_INCOMPATIBLE_VERSIONS kNtErrorClusterUpgradeIncompatibleVersions
#define ERROR_CLUSTER_UPGRADE_FIX_QUORUM_NOT_SUPPORTED kNtErrorClusterUpgradeFixQuorumNotSupported
#define ERROR_CLUSTER_UPGRADE_RESTART_REQUIRED kNtErrorClusterUpgradeRestartRequired
#define ERROR_CLUSTER_UPGRADE_IN_PROGRESS kNtErrorClusterUpgradeInProgress
#define ERROR_CLUSTER_UPGRADE_INCOMPLETE kNtErrorClusterUpgradeIncomplete
#define ERROR_CLUSTER_NODE_IN_GRACE_PERIOD kNtErrorClusterNodeInGracePeriod
#define ERROR_CLUSTER_CSV_IO_PAUSE_TIMEOUT kNtErrorClusterCsvIoPauseTimeout
#define ERROR_NODE_NOT_ACTIVE_CLUSTER_MEMBER kNtErrorNodeNotActiveClusterMember
#define ERROR_CLUSTER_RESOURCE_NOT_MONITORED kNtErrorClusterResourceNotMonitored
#define ERROR_CLUSTER_RESOURCE_DOES_NOT_SUPPORT_UNMONITORED kNtErrorClusterResourceDoesNotSupportUnmonitored
#define ERROR_CLUSTER_RESOURCE_IS_REPLICATED kNtErrorClusterResourceIsReplicated
#define ERROR_CLUSTER_NODE_ISOLATED kNtErrorClusterNodeIsolated
#define ERROR_CLUSTER_NODE_QUARANTINED kNtErrorClusterNodeQuarantined
#define ERROR_CLUSTER_DATABASE_UPDATE_CONDITION_FAILED kNtErrorClusterDatabaseUpdateConditionFailed
#define ERROR_CLUSTER_SPACE_DEGRADED kNtErrorClusterSpaceDegraded
#define ERROR_CLUSTER_TOKEN_DELEGATION_NOT_SUPPORTED kNtErrorClusterTokenDelegationNotSupported
#define ERROR_CLUSTER_CSV_INVALID_HANDLE kNtErrorClusterCsvInvalidHandle
#define ERROR_CLUSTER_CSV_SUPPORTED_ONLY_ON_COORDINATOR kNtErrorClusterCsvSupportedOnlyOnCoordinator
#define ERROR_GROUPSET_NOT_AVAILABLE kNtErrorGroupsetNotAvailable
#define ERROR_GROUPSET_NOT_FOUND kNtErrorGroupsetNotFound
#define ERROR_GROUPSET_CANT_PROVIDE kNtErrorGroupsetCantProvide
#define ERROR_CLUSTER_FAULT_DOMAIN_PARENT_NOT_FOUND kNtErrorClusterFaultDomainParentNotFound
#define ERROR_CLUSTER_FAULT_DOMAIN_INVALID_HIERARCHY kNtErrorClusterFaultDomainInvalidHierarchy
#define ERROR_CLUSTER_FAULT_DOMAIN_FAILED_S2D_VALIDATION kNtErrorClusterFaultDomainFailedS2dValidation
#define ERROR_CLUSTER_FAULT_DOMAIN_S2D_CONNECTIVITY_LOSS kNtErrorClusterFaultDomainS2dConnectivityLoss
#define ERROR_CLUSTER_INVALID_INFRASTRUCTURE_FILESERVER_NAME kNtErrorClusterInvalidInfrastructureFileserverName
#define ERROR_CLUSTERSET_MANAGEMENT_CLUSTER_UNREACHABLE kNtErrorClustersetManagementClusterUnreachable
#define ERROR_ENCRYPTION_FAILED kNtErrorEncryptionFailed
#define ERROR_DECRYPTION_FAILED kNtErrorDecryptionFailed
#define ERROR_FILE_ENCRYPTED kNtErrorFileEncrypted
#define ERROR_NO_RECOVERY_POLICY kNtErrorNoRecoveryPolicy
#define ERROR_NO_EFS kNtErrorNoEfs
#define ERROR_WRONG_EFS kNtErrorWrongEfs
#define ERROR_NO_USER_KEYS kNtErrorNoUserKeys
#define ERROR_FILE_NOT_ENCRYPTED kNtErrorFileNotEncrypted
#define ERROR_NOT_EXPORT_FORMAT kNtErrorNotExportFormat
#define ERROR_FILE_READ_ONLY kNtErrorFileReadOnly
#define ERROR_DIR_EFS_DISALLOWED kNtErrorDirEfsDisallowed
#define ERROR_EFS_SERVER_NOT_TRUSTED kNtErrorEfsServerNotTrusted
#define ERROR_BAD_RECOVERY_POLICY kNtErrorBadRecoveryPolicy
#define ERROR_EFS_ALG_BLOB_TOO_BIG kNtErrorEfsAlgBlobTooBig
#define ERROR_VOLUME_NOT_SUPPORT_EFS kNtErrorVolumeNotSupportEfs
#define ERROR_EFS_DISABLED kNtErrorEfsDisabled
#define ERROR_EFS_VERSION_NOT_SUPPORT kNtErrorEfsVersionNotSupport
#define ERROR_CS_ENCRYPTION_INVALID_SERVER_RESPONSE kNtErrorCsEncryptionInvalidServerResponse
#define ERROR_CS_ENCRYPTION_UNSUPPORTED_SERVER kNtErrorCsEncryptionUnsupportedServer
#define ERROR_CS_ENCRYPTION_EXISTING_ENCRYPTED_FILE kNtErrorCsEncryptionExistingEncryptedFile
#define ERROR_CS_ENCRYPTION_NEW_ENCRYPTED_FILE kNtErrorCsEncryptionNewEncryptedFile
#define ERROR_CS_ENCRYPTION_FILE_NOT_CSE kNtErrorCsEncryptionFileNotCse
#define ERROR_ENCRYPTION_POLICY_DENIES_OPERATION kNtErrorEncryptionPolicyDeniesOperation
#define ERROR_NO_BROWSER_SERVERS_FOUND kNtErrorNoBrowserServersFound
#define ERROR_LOG_SECTOR_INVALID kNtErrorLogSectorInvalid
#define ERROR_LOG_SECTOR_PARITY_INVALID kNtErrorLogSectorParityInvalid
#define ERROR_LOG_SECTOR_REMAPPED kNtErrorLogSectorRemapped
#define ERROR_LOG_BLOCK_INCOMPLETE kNtErrorLogBlockIncomplete
#define ERROR_LOG_INVALID_RANGE kNtErrorLogInvalidRange
#define ERROR_LOG_BLOCKS_EXHAUSTED kNtErrorLogBlocksExhausted
#define ERROR_LOG_READ_CONTEXT_INVALID kNtErrorLogReadContextInvalid
#define ERROR_LOG_RESTART_INVALID kNtErrorLogRestartInvalid
#define ERROR_LOG_BLOCK_VERSION kNtErrorLogBlockVersion
#define ERROR_LOG_BLOCK_INVALID kNtErrorLogBlockInvalid
#define ERROR_LOG_READ_MODE_INVALID kNtErrorLogReadModeInvalid
#define ERROR_LOG_NO_RESTART kNtErrorLogNoRestart
#define ERROR_LOG_METADATA_CORRUPT kNtErrorLogMetadataCorrupt
#define ERROR_LOG_METADATA_INVALID kNtErrorLogMetadataInvalid
#define ERROR_LOG_METADATA_INCONSISTENT kNtErrorLogMetadataInconsistent
#define ERROR_LOG_RESERVATION_INVALID kNtErrorLogReservationInvalid
#define ERROR_LOG_CANT_DELETE kNtErrorLogCantDelete
#define ERROR_LOG_CONTAINER_LIMIT_EXCEEDED kNtErrorLogContainerLimitExceeded
#define ERROR_LOG_START_OF_LOG kNtErrorLogStartOfLog
#define ERROR_LOG_POLICY_ALREADY_INSTALLED kNtErrorLogPolicyAlreadyInstalled
#define ERROR_LOG_POLICY_NOT_INSTALLED kNtErrorLogPolicyNotInstalled
#define ERROR_LOG_POLICY_INVALID kNtErrorLogPolicyInvalid
#define ERROR_LOG_POLICY_CONFLICT kNtErrorLogPolicyConflict
#define ERROR_LOG_PINNED_ARCHIVE_TAIL kNtErrorLogPinnedArchiveTail
#define ERROR_LOG_RECORD_NONEXISTENT kNtErrorLogRecordNonexistent
#define ERROR_LOG_RECORDS_RESERVED_INVALID kNtErrorLogRecordsReservedInvalid
#define ERROR_LOG_SPACE_RESERVED_INVALID kNtErrorLogSpaceReservedInvalid
#define ERROR_LOG_TAIL_INVALID kNtErrorLogTailInvalid
#define ERROR_LOG_FULL kNtErrorLogFull
#define ERROR_COULD_NOT_RESIZE_LOG kNtErrorCouldNotResizeLog
#define ERROR_LOG_MULTIPLEXED kNtErrorLogMultiplexed
#define ERROR_LOG_DEDICATED kNtErrorLogDedicated
#define ERROR_LOG_ARCHIVE_NOT_IN_PROGRESS kNtErrorLogArchiveNotInProgress
#define ERROR_LOG_ARCHIVE_IN_PROGRESS kNtErrorLogArchiveInProgress
#define ERROR_LOG_EPHEMERAL kNtErrorLogEphemeral
#define ERROR_LOG_NOT_ENOUGH_CONTAINERS kNtErrorLogNotEnoughContainers
#define ERROR_LOG_CLIENT_ALREADY_REGISTERED kNtErrorLogClientAlreadyRegistered
#define ERROR_LOG_CLIENT_NOT_REGISTERED kNtErrorLogClientNotRegistered
#define ERROR_LOG_FULL_HANDLER_IN_PROGRESS kNtErrorLogFullHandlerInProgress
#define ERROR_LOG_CONTAINER_READ_FAILED kNtErrorLogContainerReadFailed
#define ERROR_LOG_CONTAINER_WRITE_FAILED kNtErrorLogContainerWriteFailed
#define ERROR_LOG_CONTAINER_OPEN_FAILED kNtErrorLogContainerOpenFailed
#define ERROR_LOG_CONTAINER_STATE_INVALID kNtErrorLogContainerStateInvalid
#define ERROR_LOG_STATE_INVALID kNtErrorLogStateInvalid
#define ERROR_LOG_PINNED kNtErrorLogPinned
#define ERROR_LOG_METADATA_FLUSH_FAILED kNtErrorLogMetadataFlushFailed
#define ERROR_LOG_INCONSISTENT_SECURITY kNtErrorLogInconsistentSecurity
#define ERROR_LOG_APPENDED_FLUSH_FAILED kNtErrorLogAppendedFlushFailed
#define ERROR_LOG_PINNED_RESERVATION kNtErrorLogPinnedReservation
#define ERROR_INVALID_TRANSACTION kNtErrorInvalidTransaction
#define ERROR_TRANSACTION_NOT_ACTIVE kNtErrorTransactionNotActive
#define ERROR_TRANSACTION_REQUEST_NOT_VALID kNtErrorTransactionRequestNotValid
#define ERROR_TRANSACTION_NOT_REQUESTED kNtErrorTransactionNotRequested
#define ERROR_TRANSACTION_ALREADY_ABORTED kNtErrorTransactionAlreadyAborted
#define ERROR_TRANSACTION_ALREADY_COMMITTED kNtErrorTransactionAlreadyCommitted
#define ERROR_TM_INITIALIZATION_FAILED kNtErrorTmInitializationFailed
#define ERROR_RESOURCEMANAGER_READ_ONLY kNtErrorResourcemanagerReadOnly
#define ERROR_TRANSACTION_NOT_JOINED kNtErrorTransactionNotJoined
#define ERROR_TRANSACTION_SUPERIOR_EXISTS kNtErrorTransactionSuperiorExists
#define ERROR_CRM_PROTOCOL_ALREADY_EXISTS kNtErrorCrmProtocolAlreadyExists
#define ERROR_TRANSACTION_PROPAGATION_FAILED kNtErrorTransactionPropagationFailed
#define ERROR_CRM_PROTOCOL_NOT_FOUND kNtErrorCrmProtocolNotFound
#define ERROR_TRANSACTION_INVALID_MARSHALL_BUFFER kNtErrorTransactionInvalidMarshallBuffer
#define ERROR_CURRENT_TRANSACTION_NOT_VALID kNtErrorCurrentTransactionNotValid
#define ERROR_TRANSACTION_NOT_FOUND kNtErrorTransactionNotFound
#define ERROR_RESOURCEMANAGER_NOT_FOUND kNtErrorResourcemanagerNotFound
#define ERROR_ENLISTMENT_NOT_FOUND kNtErrorEnlistmentNotFound
#define ERROR_TRANSACTIONMANAGER_NOT_FOUND kNtErrorTransactionmanagerNotFound
#define ERROR_TRANSACTIONMANAGER_NOT_ONLINE kNtErrorTransactionmanagerNotOnline
#define ERROR_TRANSACTIONMANAGER_RECOVERY_NAME_COLLISION kNtErrorTransactionmanagerRecoveryNameCollision
#define ERROR_TRANSACTION_NOT_ROOT kNtErrorTransactionNotRoot
#define ERROR_TRANSACTION_OBJECT_EXPIRED kNtErrorTransactionObjectExpired
#define ERROR_TRANSACTION_RESPONSE_NOT_ENLISTED kNtErrorTransactionResponseNotEnlisted
#define ERROR_TRANSACTION_RECORD_TOO_LONG kNtErrorTransactionRecordTooLong
#define ERROR_IMPLICIT_TRANSACTION_NOT_SUPPORTED kNtErrorImplicitTransactionNotSupported
#define ERROR_TRANSACTION_INTEGRITY_VIOLATED kNtErrorTransactionIntegrityViolated
#define ERROR_TRANSACTIONMANAGER_IDENTITY_MISMATCH kNtErrorTransactionmanagerIdentityMismatch
#define ERROR_RM_CANNOT_BE_FROZEN_FOR_SNAPSHOT kNtErrorRmCannotBeFrozenForSnapshot
#define ERROR_TRANSACTION_MUST_WRITETHROUGH kNtErrorTransactionMustWritethrough
#define ERROR_TRANSACTION_NO_SUPERIOR kNtErrorTransactionNoSuperior
#define ERROR_HEURISTIC_DAMAGE_POSSIBLE kNtErrorHeuristicDamagePossible
#define ERROR_TRANSACTIONAL_CONFLICT kNtErrorTransactionalConflict
#define ERROR_RM_NOT_ACTIVE kNtErrorRmNotActive
#define ERROR_RM_METADATA_CORRUPT kNtErrorRmMetadataCorrupt
#define ERROR_DIRECTORY_NOT_RM kNtErrorDirectoryNotRm
#define ERROR_TRANSACTIONS_UNSUPPORTED_REMOTE kNtErrorTransactionsUnsupportedRemote
#define ERROR_LOG_RESIZE_INVALID_SIZE kNtErrorLogResizeInvalidSize
#define ERROR_OBJECT_NO_LONGER_EXISTS kNtErrorObjectNoLongerExists
#define ERROR_STREAM_MINIVERSION_NOT_FOUND kNtErrorStreamMiniversionNotFound
#define ERROR_STREAM_MINIVERSION_NOT_VALID kNtErrorStreamMiniversionNotValid
#define ERROR_MINIVERSION_INACCESSIBLE_FROM_SPECIFIED_TRANSACTION kNtErrorMiniversionInaccessibleFromSpecifiedTransaction
#define ERROR_CANT_OPEN_MINIVERSION_WITH_MODIFY_INTENT kNtErrorCantOpenMiniversionWithModifyIntent
#define ERROR_CANT_CREATE_MORE_STREAM_MINIVERSIONS kNtErrorCantCreateMoreStreamMiniversions
#define ERROR_REMOTE_FILE_VERSION_MISMATCH kNtErrorRemoteFileVersionMismatch
#define ERROR_HANDLE_NO_LONGER_VALID kNtErrorHandleNoLongerValid
#define ERROR_NO_TXF_METADATA kNtErrorNoTxfMetadata
#define ERROR_LOG_CORRUPTION_DETECTED kNtErrorLogCorruptionDetected
#define ERROR_CANT_RECOVER_WITH_HANDLE_OPEN kNtErrorCantRecoverWithHandleOpen
#define ERROR_RM_DISCONNECTED kNtErrorRmDisconnected
#define ERROR_ENLISTMENT_NOT_SUPERIOR kNtErrorEnlistmentNotSuperior
#define ERROR_RECOVERY_NOT_NEEDED kNtErrorRecoveryNotNeeded
#define ERROR_RM_ALREADY_STARTED kNtErrorRmAlreadyStarted
#define ERROR_FILE_IDENTITY_NOT_PERSISTENT kNtErrorFileIdentityNotPersistent
#define ERROR_CANT_BREAK_TRANSACTIONAL_DEPENDENCY kNtErrorCantBreakTransactionalDependency
#define ERROR_CANT_CROSS_RM_BOUNDARY kNtErrorCantCrossRmBoundary
#define ERROR_TXF_DIR_NOT_EMPTY kNtErrorTxfDirNotEmpty
#define ERROR_INDOUBT_TRANSACTIONS_EXIST kNtErrorIndoubtTransactionsExist
#define ERROR_TM_VOLATILE kNtErrorTmVolatile
#define ERROR_ROLLBACK_TIMER_EXPIRED kNtErrorRollbackTimerExpired
#define ERROR_TXF_ATTRIBUTE_CORRUPT kNtErrorTxfAttributeCorrupt
#define ERROR_EFS_NOT_ALLOWED_IN_TRANSACTION kNtErrorEfsNotAllowedInTransaction
#define ERROR_TRANSACTIONAL_OPEN_NOT_ALLOWED kNtErrorTransactionalOpenNotAllowed
#define ERROR_LOG_GROWTH_FAILED kNtErrorLogGrowthFailed
#define ERROR_TRANSACTED_MAPPING_UNSUPPORTED_REMOTE kNtErrorTransactedMappingUnsupportedRemote
#define ERROR_TXF_METADATA_ALREADY_PRESENT kNtErrorTxfMetadataAlreadyPresent
#define ERROR_TRANSACTION_SCOPE_CALLBACKS_NOT_SET kNtErrorTransactionScopeCallbacksNotSet
#define ERROR_TRANSACTION_REQUIRED_PROMOTION kNtErrorTransactionRequiredPromotion
#define ERROR_CANNOT_EXECUTE_FILE_IN_TRANSACTION kNtErrorCannotExecuteFileInTransaction
#define ERROR_TRANSACTIONS_NOT_FROZEN kNtErrorTransactionsNotFrozen
#define ERROR_TRANSACTION_FREEZE_IN_PROGRESS kNtErrorTransactionFreezeInProgress
#define ERROR_NOT_SNAPSHOT_VOLUME kNtErrorNotSnapshotVolume
#define ERROR_NO_SAVEPOINT_WITH_OPEN_FILES kNtErrorNoSavepointWithOpenFiles
#define ERROR_DATA_LOST_REPAIR kNtErrorDataLostRepair
#define ERROR_SPARSE_NOT_ALLOWED_IN_TRANSACTION kNtErrorSparseNotAllowedInTransaction
#define ERROR_TM_IDENTITY_MISMATCH kNtErrorTmIdentityMismatch
#define ERROR_FLOATED_SECTION kNtErrorFloatedSection
#define ERROR_CANNOT_ACCEPT_TRANSACTED_WORK kNtErrorCannotAcceptTransactedWork
#define ERROR_CANNOT_ABORT_TRANSACTIONS kNtErrorCannotAbortTransactions
#define ERROR_BAD_CLUSTERS kNtErrorBadClusters
#define ERROR_COMPRESSION_NOT_ALLOWED_IN_TRANSACTION kNtErrorCompressionNotAllowedInTransaction
#define ERROR_VOLUME_DIRTY kNtErrorVolumeDirty
#define ERROR_NO_LINK_TRACKING_IN_TRANSACTION kNtErrorNoLinkTrackingInTransaction
#define ERROR_OPERATION_NOT_SUPPORTED_IN_TRANSACTION kNtErrorOperationNotSupportedInTransaction
#define ERROR_EXPIRED_HANDLE kNtErrorExpiredHandle
#define ERROR_TRANSACTION_NOT_ENLISTED kNtErrorTransactionNotEnlisted
#define ERROR_CTX_WINSTATION_NAME_INVALID kNtErrorCtxWinstationNameInvalid
#define ERROR_CTX_INVALID_PD kNtErrorCtxInvalidPd
#define ERROR_CTX_PD_NOT_FOUND kNtErrorCtxPdNotFound
#define ERROR_CTX_WD_NOT_FOUND kNtErrorCtxWdNotFound
#define ERROR_CTX_CANNOT_MAKE_EVENTLOG_ENTRY kNtErrorCtxCannotMakeEventlogEntry
#define ERROR_CTX_SERVICE_NAME_COLLISION kNtErrorCtxServiceNameCollision
#define ERROR_CTX_CLOSE_PENDING kNtErrorCtxClosePending
#define ERROR_CTX_NO_OUTBUF kNtErrorCtxNoOutbuf
#define ERROR_CTX_MODEM_INF_NOT_FOUND kNtErrorCtxModemInfNotFound
#define ERROR_CTX_INVALID_MODEMNAME kNtErrorCtxInvalidModemname
#define ERROR_CTX_MODEM_RESPONSE_ERROR kNtErrorCtxModemResponseError
#define ERROR_CTX_MODEM_RESPONSE_TIMEOUT kNtErrorCtxModemResponseTimeout
#define ERROR_CTX_MODEM_RESPONSE_NO_CARRIER kNtErrorCtxModemResponseNoCarrier
#define ERROR_CTX_MODEM_RESPONSE_NO_DIALTONE kNtErrorCtxModemResponseNoDialtone
#define ERROR_CTX_MODEM_RESPONSE_BUSY kNtErrorCtxModemResponseBusy
#define ERROR_CTX_MODEM_RESPONSE_VOICE kNtErrorCtxModemResponseVoice
#define ERROR_CTX_TD_ERROR kNtErrorCtxTdError
#define ERROR_CTX_WINSTATION_NOT_FOUND kNtErrorCtxWinstationNotFound
#define ERROR_CTX_WINSTATION_ALREADY_EXISTS kNtErrorCtxWinstationAlreadyExists
#define ERROR_CTX_WINSTATION_BUSY kNtErrorCtxWinstationBusy
#define ERROR_CTX_BAD_VIDEO_MODE kNtErrorCtxBadVideoMode
#define ERROR_CTX_GRAPHICS_INVALID kNtErrorCtxGraphicsInvalid
#define ERROR_CTX_LOGON_DISABLED kNtErrorCtxLogonDisabled
#define ERROR_CTX_NOT_CONSOLE kNtErrorCtxNotConsole
#define ERROR_CTX_CLIENT_QUERY_TIMEOUT kNtErrorCtxClientQueryTimeout
#define ERROR_CTX_CONSOLE_DISCONNECT kNtErrorCtxConsoleDisconnect
#define ERROR_CTX_CONSOLE_CONNECT kNtErrorCtxConsoleConnect
#define ERROR_CTX_SHADOW_DENIED kNtErrorCtxShadowDenied
#define ERROR_CTX_WINSTATION_ACCESS_DENIED kNtErrorCtxWinstationAccessDenied
#define ERROR_CTX_INVALID_WD kNtErrorCtxInvalidWd
#define ERROR_CTX_SHADOW_INVALID kNtErrorCtxShadowInvalid
#define ERROR_CTX_SHADOW_DISABLED kNtErrorCtxShadowDisabled
#define ERROR_CTX_CLIENT_LICENSE_IN_USE kNtErrorCtxClientLicenseInUse
#define ERROR_CTX_CLIENT_LICENSE_NOT_SET kNtErrorCtxClientLicenseNotSet
#define ERROR_CTX_LICENSE_NOT_AVAILABLE kNtErrorCtxLicenseNotAvailable
#define ERROR_CTX_LICENSE_CLIENT_INVALID kNtErrorCtxLicenseClientInvalid
#define ERROR_CTX_LICENSE_EXPIRED kNtErrorCtxLicenseExpired
#define ERROR_CTX_SHADOW_NOT_RUNNING kNtErrorCtxShadowNotRunning
#define ERROR_CTX_SHADOW_ENDED_BY_MODE_CHANGE kNtErrorCtxShadowEndedByModeChange
#define ERROR_ACTIVATION_COUNT_EXCEEDED kNtErrorActivationCountExceeded
#define ERROR_CTX_WINSTATIONS_DISABLED kNtErrorCtxWinstationsDisabled
#define ERROR_CTX_ENCRYPTION_LEVEL_REQUIRED kNtErrorCtxEncryptionLevelRequired
#define ERROR_CTX_SESSION_IN_USE kNtErrorCtxSessionInUse
#define ERROR_CTX_NO_FORCE_LOGOFF kNtErrorCtxNoForceLogoff
#define ERROR_CTX_ACCOUNT_RESTRICTION kNtErrorCtxAccountRestriction
#define ERROR_RDP_PROTOCOL_ERROR kNtErrorRdpProtocolError
#define ERROR_CTX_CDM_CONNECT kNtErrorCtxCdmConnect
#define ERROR_CTX_CDM_DISCONNECT kNtErrorCtxCdmDisconnect
#define ERROR_CTX_SECURITY_LAYER_ERROR kNtErrorCtxSecurityLayerError
#define ERROR_TS_INCOMPATIBLE_SESSIONS kNtErrorTsIncompatibleSessions
#define ERROR_TS_VIDEO_SUBSYSTEM_ERROR kNtErrorTsVideoSubsystemError
#define ERROR_DS_NOT_INSTALLED kNtErrorDsNotInstalled
#define ERROR_DS_MEMBERSHIP_EVALUATED_LOCALLY kNtErrorDsMembershipEvaluatedLocally
#define ERROR_DS_NO_ATTRIBUTE_OR_VALUE kNtErrorDsNoAttributeOrValue
#define ERROR_DS_INVALID_ATTRIBUTE_SYNTAX kNtErrorDsInvalidAttributeSyntax
#define ERROR_DS_ATTRIBUTE_TYPE_UNDEFINED kNtErrorDsAttributeTypeUndefined
#define ERROR_DS_ATTRIBUTE_OR_VALUE_EXISTS kNtErrorDsAttributeOrValueExists
#define ERROR_DS_BUSY kNtErrorDsBusy
#define ERROR_DS_UNAVAILABLE kNtErrorDsUnavailable
#define ERROR_DS_NO_RIDS_ALLOCATED kNtErrorDsNoRidsAllocated
#define ERROR_DS_NO_MORE_RIDS kNtErrorDsNoMoreRids
#define ERROR_DS_INCORRECT_ROLE_OWNER kNtErrorDsIncorrectRoleOwner
#define ERROR_DS_RIDMGR_INIT_ERROR kNtErrorDsRidmgrInitError
#define ERROR_DS_OBJ_CLASS_VIOLATION kNtErrorDsObjClassViolation
#define ERROR_DS_CANT_ON_NON_LEAF kNtErrorDsCantOnNonLeaf
#define ERROR_DS_CANT_ON_RDN kNtErrorDsCantOnRdn
#define ERROR_DS_CANT_MOD_OBJ_CLASS kNtErrorDsCantModObjClass
#define ERROR_DS_CROSS_DOM_MOVE_ERROR kNtErrorDsCrossDomMoveError
#define ERROR_DS_GC_NOT_AVAILABLE kNtErrorDsGcNotAvailable
#define ERROR_SHARED_POLICY kNtErrorSharedPolicy
#define ERROR_POLICY_OBJECT_NOT_FOUND kNtErrorPolicyObjectNotFound
#define ERROR_POLICY_ONLY_IN_DS kNtErrorPolicyOnlyInDs
#define ERROR_PROMOTION_ACTIVE kNtErrorPromotionActive
#define ERROR_NO_PROMOTION_ACTIVE kNtErrorNoPromotionActive
#define ERROR_DS_OPERATIONS_ERROR kNtErrorDsOperationsError
#define ERROR_DS_PROTOCOL_ERROR kNtErrorDsProtocolError
#define ERROR_DS_TIMELIMIT_EXCEEDED kNtErrorDsTimelimitExceeded
#define ERROR_DS_SIZELIMIT_EXCEEDED kNtErrorDsSizelimitExceeded
#define ERROR_DS_ADMIN_LIMIT_EXCEEDED kNtErrorDsAdminLimitExceeded
#define ERROR_DS_COMPARE_FALSE kNtErrorDsCompareFalse
#define ERROR_DS_COMPARE_TRUE kNtErrorDsCompareTrue
#define ERROR_DS_AUTH_METHOD_NOT_SUPPORTED kNtErrorDsAuthMethodNotSupported
#define ERROR_DS_STRONG_AUTH_REQUIRED kNtErrorDsStrongAuthRequired
#define ERROR_DS_INAPPROPRIATE_AUTH kNtErrorDsInappropriateAuth
#define ERROR_DS_AUTH_UNKNOWN kNtErrorDsAuthUnknown
#define ERROR_DS_REFERRAL kNtErrorDsReferral
#define ERROR_DS_UNAVAILABLE_CRIT_EXTENSION kNtErrorDsUnavailableCritExtension
#define ERROR_DS_CONFIDENTIALITY_REQUIRED kNtErrorDsConfidentialityRequired
#define ERROR_DS_INAPPROPRIATE_MATCHING kNtErrorDsInappropriateMatching
#define ERROR_DS_CONSTRAINT_VIOLATION kNtErrorDsConstraintViolation
#define ERROR_DS_NO_SUCH_OBJECT kNtErrorDsNoSuchObject
#define ERROR_DS_ALIAS_PROBLEM kNtErrorDsAliasProblem
#define ERROR_DS_INVALID_DN_SYNTAX kNtErrorDsInvalidDnSyntax
#define ERROR_DS_IS_LEAF kNtErrorDsIsLeaf
#define ERROR_DS_ALIAS_DEREF_PROBLEM kNtErrorDsAliasDerefProblem
#define ERROR_DS_UNWILLING_TO_PERFORM kNtErrorDsUnwillingToPerform
#define ERROR_DS_LOOP_DETECT kNtErrorDsLoopDetect
#define ERROR_DS_NAMING_VIOLATION kNtErrorDsNamingViolation
#define ERROR_DS_OBJECT_RESULTS_TOO_LARGE kNtErrorDsObjectResultsTooLarge
#define ERROR_DS_AFFECTS_MULTIPLE_DSAS kNtErrorDsAffectsMultipleDsas
#define ERROR_DS_SERVER_DOWN kNtErrorDsServerDown
#define ERROR_DS_LOCAL_ERROR kNtErrorDsLocalError
#define ERROR_DS_ENCODING_ERROR kNtErrorDsEncodingError
#define ERROR_DS_DECODING_ERROR kNtErrorDsDecodingError
#define ERROR_DS_FILTER_UNKNOWN kNtErrorDsFilterUnknown
#define ERROR_DS_PARAM_ERROR kNtErrorDsParamError
#define ERROR_DS_NOT_SUPPORTED kNtErrorDsNotSupported
#define ERROR_DS_NO_RESULTS_RETURNED kNtErrorDsNoResultsReturned
#define ERROR_DS_CONTROL_NOT_FOUND kNtErrorDsControlNotFound
#define ERROR_DS_CLIENT_LOOP kNtErrorDsClientLoop
#define ERROR_DS_REFERRAL_LIMIT_EXCEEDED kNtErrorDsReferralLimitExceeded
#define ERROR_DS_SORT_CONTROL_MISSING kNtErrorDsSortControlMissing
#define ERROR_DS_OFFSET_RANGE_ERROR kNtErrorDsOffsetRangeError
#define ERROR_DS_RIDMGR_DISABLED kNtErrorDsRidmgrDisabled
#define ERROR_DS_ROOT_MUST_BE_NC kNtErrorDsRootMustBeNc
#define ERROR_DS_ADD_REPLICA_INHIBITED kNtErrorDsAddReplicaInhibited
#define ERROR_DS_ATT_NOT_DEF_IN_SCHEMA kNtErrorDsAttNotDefInSchema
#define ERROR_DS_MAX_OBJ_SIZE_EXCEEDED kNtErrorDsMaxObjSizeExceeded
#define ERROR_DS_OBJ_STRING_NAME_EXISTS kNtErrorDsObjStringNameExists
#define ERROR_DS_NO_RDN_DEFINED_IN_SCHEMA kNtErrorDsNoRdnDefinedInSchema
#define ERROR_DS_RDN_DOESNT_MATCH_SCHEMA kNtErrorDsRdnDoesntMatchSchema
#define ERROR_DS_NO_REQUESTED_ATTS_FOUND kNtErrorDsNoRequestedAttsFound
#define ERROR_DS_USER_BUFFER_TO_SMALL kNtErrorDsUserBufferToSmall
#define ERROR_DS_ATT_IS_NOT_ON_OBJ kNtErrorDsAttIsNotOnObj
#define ERROR_DS_ILLEGAL_MOD_OPERATION kNtErrorDsIllegalModOperation
#define ERROR_DS_OBJ_TOO_LARGE kNtErrorDsObjTooLarge
#define ERROR_DS_BAD_INSTANCE_TYPE kNtErrorDsBadInstanceType
#define ERROR_DS_MASTERDSA_REQUIRED kNtErrorDsMasterdsaRequired
#define ERROR_DS_OBJECT_CLASS_REQUIRED kNtErrorDsObjectClassRequired
#define ERROR_DS_MISSING_REQUIRED_ATT kNtErrorDsMissingRequiredAtt
#define ERROR_DS_ATT_NOT_DEF_FOR_CLASS kNtErrorDsAttNotDefForClass
#define ERROR_DS_ATT_ALREADY_EXISTS kNtErrorDsAttAlreadyExists
#define ERROR_DS_CANT_ADD_ATT_VALUES kNtErrorDsCantAddAttValues
#define ERROR_DS_SINGLE_VALUE_CONSTRAINT kNtErrorDsSingleValueConstraint
#define ERROR_DS_RANGE_CONSTRAINT kNtErrorDsRangeConstraint
#define ERROR_DS_ATT_VAL_ALREADY_EXISTS kNtErrorDsAttValAlreadyExists
#define ERROR_DS_CANT_REM_MISSING_ATT kNtErrorDsCantRemMissingAtt
#define ERROR_DS_CANT_REM_MISSING_ATT_VAL kNtErrorDsCantRemMissingAttVal
#define ERROR_DS_ROOT_CANT_BE_SUBREF kNtErrorDsRootCantBeSubref
#define ERROR_DS_NO_CHAINING kNtErrorDsNoChaining
#define ERROR_DS_NO_CHAINED_EVAL kNtErrorDsNoChainedEval
#define ERROR_DS_NO_PARENT_OBJECT kNtErrorDsNoParentObject
#define ERROR_DS_PARENT_IS_AN_ALIAS kNtErrorDsParentIsAnAlias
#define ERROR_DS_CANT_MIX_MASTER_AND_REPS kNtErrorDsCantMixMasterAndReps
#define ERROR_DS_CHILDREN_EXIST kNtErrorDsChildrenExist
#define ERROR_DS_OBJ_NOT_FOUND kNtErrorDsObjNotFound
#define ERROR_DS_ALIASED_OBJ_MISSING kNtErrorDsAliasedObjMissing
#define ERROR_DS_BAD_NAME_SYNTAX kNtErrorDsBadNameSyntax
#define ERROR_DS_ALIAS_POINTS_TO_ALIAS kNtErrorDsAliasPointsToAlias
#define ERROR_DS_CANT_DEREF_ALIAS kNtErrorDsCantDerefAlias
#define ERROR_DS_OUT_OF_SCOPE kNtErrorDsOutOfScope
#define ERROR_DS_OBJECT_BEING_REMOVED kNtErrorDsObjectBeingRemoved
#define ERROR_DS_CANT_DELETE_DSA_OBJ kNtErrorDsCantDeleteDsaObj
#define ERROR_DS_GENERIC_ERROR kNtErrorDsGenericError
#define ERROR_DS_DSA_MUST_BE_INT_MASTER kNtErrorDsDsaMustBeIntMaster
#define ERROR_DS_CLASS_NOT_DSA kNtErrorDsClassNotDsa
#define ERROR_DS_INSUFF_ACCESS_RIGHTS kNtErrorDsInsuffAccessRights
#define ERROR_DS_ILLEGAL_SUPERIOR kNtErrorDsIllegalSuperior
#define ERROR_DS_ATTRIBUTE_OWNED_BY_SAM kNtErrorDsAttributeOwnedBySam
#define ERROR_DS_NAME_TOO_MANY_PARTS kNtErrorDsNameTooManyParts
#define ERROR_DS_NAME_TOO_LONG kNtErrorDsNameTooLong
#define ERROR_DS_NAME_VALUE_TOO_LONG kNtErrorDsNameValueTooLong
#define ERROR_DS_NAME_UNPARSEABLE kNtErrorDsNameUnparseable
#define ERROR_DS_NAME_TYPE_UNKNOWN kNtErrorDsNameTypeUnknown
#define ERROR_DS_NOT_AN_OBJECT kNtErrorDsNotAnObject
#define ERROR_DS_SEC_DESC_TOO_SHORT kNtErrorDsSecDescTooShort
#define ERROR_DS_SEC_DESC_INVALID kNtErrorDsSecDescInvalid
#define ERROR_DS_NO_DELETED_NAME kNtErrorDsNoDeletedName
#define ERROR_DS_SUBREF_MUST_HAVE_PARENT kNtErrorDsSubrefMustHaveParent
#define ERROR_DS_NCNAME_MUST_BE_NC kNtErrorDsNcnameMustBeNc
#define ERROR_DS_CANT_ADD_SYSTEM_ONLY kNtErrorDsCantAddSystemOnly
#define ERROR_DS_CLASS_MUST_BE_CONCRETE kNtErrorDsClassMustBeConcrete
#define ERROR_DS_INVALID_DMD kNtErrorDsInvalidDmd
#define ERROR_DS_OBJ_GUID_EXISTS kNtErrorDsObjGuidExists
#define ERROR_DS_NOT_ON_BACKLINK kNtErrorDsNotOnBacklink
#define ERROR_DS_NO_CROSSREF_FOR_NC kNtErrorDsNoCrossrefForNc
#define ERROR_DS_SHUTTING_DOWN kNtErrorDsShuttingDown
#define ERROR_DS_UNKNOWN_OPERATION kNtErrorDsUnknownOperation
#define ERROR_DS_INVALID_ROLE_OWNER kNtErrorDsInvalidRoleOwner
#define ERROR_DS_COULDNT_CONTACT_FSMO kNtErrorDsCouldntContactFsmo
#define ERROR_DS_CROSS_NC_DN_RENAME kNtErrorDsCrossNcDnRename
#define ERROR_DS_CANT_MOD_SYSTEM_ONLY kNtErrorDsCantModSystemOnly
#define ERROR_DS_REPLICATOR_ONLY kNtErrorDsReplicatorOnly
#define ERROR_DS_OBJ_CLASS_NOT_DEFINED kNtErrorDsObjClassNotDefined
#define ERROR_DS_OBJ_CLASS_NOT_SUBCLASS kNtErrorDsObjClassNotSubclass
#define ERROR_DS_NAME_REFERENCE_INVALID kNtErrorDsNameReferenceInvalid
#define ERROR_DS_CROSS_REF_EXISTS kNtErrorDsCrossRefExists
#define ERROR_DS_CANT_DEL_MASTER_CROSSREF kNtErrorDsCantDelMasterCrossref
#define ERROR_DS_SUBTREE_NOTIFY_NOT_NC_HEAD kNtErrorDsSubtreeNotifyNotNcHead
#define ERROR_DS_NOTIFY_FILTER_TOO_COMPLEX kNtErrorDsNotifyFilterTooComplex
#define ERROR_DS_DUP_RDN kNtErrorDsDupRdn
#define ERROR_DS_DUP_OID kNtErrorDsDupOid
#define ERROR_DS_DUP_MAPI_ID kNtErrorDsDupMapiId
#define ERROR_DS_DUP_SCHEMA_ID_GUID kNtErrorDsDupSchemaIdGuid
#define ERROR_DS_DUP_LDAP_DISPLAY_NAME kNtErrorDsDupLdapDisplayName
#define ERROR_DS_SEMANTIC_ATT_TEST kNtErrorDsSemanticAttTest
#define ERROR_DS_SYNTAX_MISMATCH kNtErrorDsSyntaxMismatch
#define ERROR_DS_EXISTS_IN_MUST_HAVE kNtErrorDsExistsInMustHave
#define ERROR_DS_EXISTS_IN_MAY_HAVE kNtErrorDsExistsInMayHave
#define ERROR_DS_NONEXISTENT_MAY_HAVE kNtErrorDsNonexistentMayHave
#define ERROR_DS_NONEXISTENT_MUST_HAVE kNtErrorDsNonexistentMustHave
#define ERROR_DS_AUX_CLS_TEST_FAIL kNtErrorDsAuxClsTestFail
#define ERROR_DS_NONEXISTENT_POSS_SUP kNtErrorDsNonexistentPossSup
#define ERROR_DS_SUB_CLS_TEST_FAIL kNtErrorDsSubClsTestFail
#define ERROR_DS_BAD_RDN_ATT_ID_SYNTAX kNtErrorDsBadRdnAttIdSyntax
#define ERROR_DS_EXISTS_IN_AUX_CLS kNtErrorDsExistsInAuxCls
#define ERROR_DS_EXISTS_IN_SUB_CLS kNtErrorDsExistsInSubCls
#define ERROR_DS_EXISTS_IN_POSS_SUP kNtErrorDsExistsInPossSup
#define ERROR_DS_RECALCSCHEMA_FAILED kNtErrorDsRecalcschemaFailed
#define ERROR_DS_TREE_DELETE_NOT_FINISHED kNtErrorDsTreeDeleteNotFinished
#define ERROR_DS_CANT_DELETE kNtErrorDsCantDelete
#define ERROR_DS_ATT_SCHEMA_REQ_ID kNtErrorDsAttSchemaReqId
#define ERROR_DS_BAD_ATT_SCHEMA_SYNTAX kNtErrorDsBadAttSchemaSyntax
#define ERROR_DS_CANT_CACHE_ATT kNtErrorDsCantCacheAtt
#define ERROR_DS_CANT_CACHE_CLASS kNtErrorDsCantCacheClass
#define ERROR_DS_CANT_REMOVE_ATT_CACHE kNtErrorDsCantRemoveAttCache
#define ERROR_DS_CANT_REMOVE_CLASS_CACHE kNtErrorDsCantRemoveClassCache
#define ERROR_DS_CANT_RETRIEVE_DN kNtErrorDsCantRetrieveDn
#define ERROR_DS_MISSING_SUPREF kNtErrorDsMissingSupref
#define ERROR_DS_CANT_RETRIEVE_INSTANCE kNtErrorDsCantRetrieveInstance
#define ERROR_DS_CODE_INCONSISTENCY kNtErrorDsCodeInconsistency
#define ERROR_DS_DATABASE_ERROR kNtErrorDsDatabaseError
#define ERROR_DS_GOVERNSID_MISSING kNtErrorDsGovernsidMissing
#define ERROR_DS_MISSING_EXPECTED_ATT kNtErrorDsMissingExpectedAtt
#define ERROR_DS_NCNAME_MISSING_CR_REF kNtErrorDsNcnameMissingCrRef
#define ERROR_DS_SECURITY_CHECKING_ERROR kNtErrorDsSecurityCheckingError
#define ERROR_DS_SCHEMA_NOT_LOADED kNtErrorDsSchemaNotLoaded
#define ERROR_DS_SCHEMA_ALLOC_FAILED kNtErrorDsSchemaAllocFailed
#define ERROR_DS_ATT_SCHEMA_REQ_SYNTAX kNtErrorDsAttSchemaReqSyntax
#define ERROR_DS_GCVERIFY_ERROR kNtErrorDsGcverifyError
#define ERROR_DS_DRA_SCHEMA_MISMATCH kNtErrorDsDraSchemaMismatch
#define ERROR_DS_CANT_FIND_DSA_OBJ kNtErrorDsCantFindDsaObj
#define ERROR_DS_CANT_FIND_EXPECTED_NC kNtErrorDsCantFindExpectedNc
#define ERROR_DS_CANT_FIND_NC_IN_CACHE kNtErrorDsCantFindNcInCache
#define ERROR_DS_CANT_RETRIEVE_CHILD kNtErrorDsCantRetrieveChild
#define ERROR_DS_SECURITY_ILLEGAL_MODIFY kNtErrorDsSecurityIllegalModify
#define ERROR_DS_CANT_REPLACE_HIDDEN_REC kNtErrorDsCantReplaceHiddenRec
#define ERROR_DS_BAD_HIERARCHY_FILE kNtErrorDsBadHierarchyFile
#define ERROR_DS_BUILD_HIERARCHY_TABLE_FAILED kNtErrorDsBuildHierarchyTableFailed
#define ERROR_DS_CONFIG_PARAM_MISSING kNtErrorDsConfigParamMissing
#define ERROR_DS_COUNTING_AB_INDICES_FAILED kNtErrorDsCountingAbIndicesFailed
#define ERROR_DS_HIERARCHY_TABLE_MALLOC_FAILED kNtErrorDsHierarchyTableMallocFailed
#define ERROR_DS_INTERNAL_FAILURE kNtErrorDsInternalFailure
#define ERROR_DS_UNKNOWN_ERROR kNtErrorDsUnknownError
#define ERROR_DS_ROOT_REQUIRES_CLASS_TOP kNtErrorDsRootRequiresClassTop
#define ERROR_DS_REFUSING_FSMO_ROLES kNtErrorDsRefusingFsmoRoles
#define ERROR_DS_MISSING_FSMO_SETTINGS kNtErrorDsMissingFsmoSettings
#define ERROR_DS_UNABLE_TO_SURRENDER_ROLES kNtErrorDsUnableToSurrenderRoles
#define ERROR_DS_DRA_GENERIC kNtErrorDsDraGeneric
#define ERROR_DS_DRA_INVALID_PARAMETER kNtErrorDsDraInvalidParameter
#define ERROR_DS_DRA_BUSY kNtErrorDsDraBusy
#define ERROR_DS_DRA_BAD_DN kNtErrorDsDraBadDn
#define ERROR_DS_DRA_BAD_NC kNtErrorDsDraBadNc
#define ERROR_DS_DRA_DN_EXISTS kNtErrorDsDraDnExists
#define ERROR_DS_DRA_INTERNAL_ERROR kNtErrorDsDraInternalError
#define ERROR_DS_DRA_INCONSISTENT_DIT kNtErrorDsDraInconsistentDit
#define ERROR_DS_DRA_CONNECTION_FAILED kNtErrorDsDraConnectionFailed
#define ERROR_DS_DRA_BAD_INSTANCE_TYPE kNtErrorDsDraBadInstanceType
#define ERROR_DS_DRA_OUT_OF_MEM kNtErrorDsDraOutOfMem
#define ERROR_DS_DRA_MAIL_PROBLEM kNtErrorDsDraMailProblem
#define ERROR_DS_DRA_REF_ALREADY_EXISTS kNtErrorDsDraRefAlreadyExists
#define ERROR_DS_DRA_REF_NOT_FOUND kNtErrorDsDraRefNotFound
#define ERROR_DS_DRA_OBJ_IS_REP_SOURCE kNtErrorDsDraObjIsRepSource
#define ERROR_DS_DRA_DB_ERROR kNtErrorDsDraDbError
#define ERROR_DS_DRA_NO_REPLICA kNtErrorDsDraNoReplica
#define ERROR_DS_DRA_ACCESS_DENIED kNtErrorDsDraAccessDenied
#define ERROR_DS_DRA_NOT_SUPPORTED kNtErrorDsDraNotSupported
#define ERROR_DS_DRA_RPC_CANCELLED kNtErrorDsDraRpcCancelled
#define ERROR_DS_DRA_SOURCE_DISABLED kNtErrorDsDraSourceDisabled
#define ERROR_DS_DRA_SINK_DISABLED kNtErrorDsDraSinkDisabled
#define ERROR_DS_DRA_NAME_COLLISION kNtErrorDsDraNameCollision
#define ERROR_DS_DRA_SOURCE_REINSTALLED kNtErrorDsDraSourceReinstalled
#define ERROR_DS_DRA_MISSING_PARENT kNtErrorDsDraMissingParent
#define ERROR_DS_DRA_PREEMPTED kNtErrorDsDraPreempted
#define ERROR_DS_DRA_ABANDON_SYNC kNtErrorDsDraAbandonSync
#define ERROR_DS_DRA_SHUTDOWN kNtErrorDsDraShutdown
#define ERROR_DS_DRA_INCOMPATIBLE_PARTIAL_SET kNtErrorDsDraIncompatiblePartialSet
#define ERROR_DS_DRA_SOURCE_IS_PARTIAL_REPLICA kNtErrorDsDraSourceIsPartialReplica
#define ERROR_DS_DRA_EXTN_CONNECTION_FAILED kNtErrorDsDraExtnConnectionFailed
#define ERROR_DS_INSTALL_SCHEMA_MISMATCH kNtErrorDsInstallSchemaMismatch
#define ERROR_DS_DUP_LINK_ID kNtErrorDsDupLinkId
#define ERROR_DS_NAME_ERROR_RESOLVING kNtErrorDsNameErrorResolving
#define ERROR_DS_NAME_ERROR_NOT_FOUND kNtErrorDsNameErrorNotFound
#define ERROR_DS_NAME_ERROR_NOT_UNIQUE kNtErrorDsNameErrorNotUnique
#define ERROR_DS_NAME_ERROR_NO_MAPPING kNtErrorDsNameErrorNoMapping
#define ERROR_DS_NAME_ERROR_DOMAIN_ONLY kNtErrorDsNameErrorDomainOnly
#define ERROR_DS_NAME_ERROR_NO_SYNTACTICAL_MAPPING kNtErrorDsNameErrorNoSyntacticalMapping
#define ERROR_DS_CONSTRUCTED_ATT_MOD kNtErrorDsConstructedAttMod
#define ERROR_DS_WRONG_OM_OBJ_CLASS kNtErrorDsWrongOmObjClass
#define ERROR_DS_DRA_REPL_PENDING kNtErrorDsDraReplPending
#define ERROR_DS_DS_REQUIRED kNtErrorDsDsRequired
#define ERROR_DS_INVALID_LDAP_DISPLAY_NAME kNtErrorDsInvalidLdapDisplayName
#define ERROR_DS_NON_BASE_SEARCH kNtErrorDsNonBaseSearch
#define ERROR_DS_CANT_RETRIEVE_ATTS kNtErrorDsCantRetrieveAtts
#define ERROR_DS_BACKLINK_WITHOUT_LINK kNtErrorDsBacklinkWithoutLink
#define ERROR_DS_EPOCH_MISMATCH kNtErrorDsEpochMismatch
#define ERROR_DS_SRC_NAME_MISMATCH kNtErrorDsSrcNameMismatch
#define ERROR_DS_SRC_AND_DST_NC_IDENTICAL kNtErrorDsSrcAndDstNcIdentical
#define ERROR_DS_DST_NC_MISMATCH kNtErrorDsDstNcMismatch
#define ERROR_DS_NOT_AUTHORITIVE_FOR_DST_NC kNtErrorDsNotAuthoritiveForDstNc
#define ERROR_DS_SRC_GUID_MISMATCH kNtErrorDsSrcGuidMismatch
#define ERROR_DS_CANT_MOVE_DELETED_OBJECT kNtErrorDsCantMoveDeletedObject
#define ERROR_DS_PDC_OPERATION_IN_PROGRESS kNtErrorDsPdcOperationInProgress
#define ERROR_DS_CROSS_DOMAIN_CLEANUP_REQD kNtErrorDsCrossDomainCleanupReqd
#define ERROR_DS_ILLEGAL_XDOM_MOVE_OPERATION kNtErrorDsIllegalXdomMoveOperation
#define ERROR_DS_CANT_WITH_ACCT_GROUP_MEMBERSHPS kNtErrorDsCantWithAcctGroupMembershps
#define ERROR_DS_NC_MUST_HAVE_NC_PARENT kNtErrorDsNcMustHaveNcParent
#define ERROR_DS_CR_IMPOSSIBLE_TO_VALIDATE kNtErrorDsCrImpossibleToValidate
#define ERROR_DS_DST_DOMAIN_NOT_NATIVE kNtErrorDsDstDomainNotNative
#define ERROR_DS_MISSING_INFRASTRUCTURE_CONTAINER kNtErrorDsMissingInfrastructureContainer
#define ERROR_DS_CANT_MOVE_ACCOUNT_GROUP kNtErrorDsCantMoveAccountGroup
#define ERROR_DS_CANT_MOVE_RESOURCE_GROUP kNtErrorDsCantMoveResourceGroup
#define ERROR_DS_INVALID_SEARCH_FLAG kNtErrorDsInvalidSearchFlag
#define ERROR_DS_NO_TREE_DELETE_ABOVE_NC kNtErrorDsNoTreeDeleteAboveNc
#define ERROR_DS_COULDNT_LOCK_TREE_FOR_DELETE kNtErrorDsCouldntLockTreeForDelete
#define ERROR_DS_COULDNT_IDENTIFY_OBJECTS_FOR_TREE_DELETE kNtErrorDsCouldntIdentifyObjectsForTreeDelete
#define ERROR_DS_SAM_INIT_FAILURE kNtErrorDsSamInitFailure
#define ERROR_DS_SENSITIVE_GROUP_VIOLATION kNtErrorDsSensitiveGroupViolation
#define ERROR_DS_CANT_MOD_PRIMARYGROUPID kNtErrorDsCantModPrimarygroupid
#define ERROR_DS_ILLEGAL_BASE_SCHEMA_MOD kNtErrorDsIllegalBaseSchemaMod
#define ERROR_DS_NONSAFE_SCHEMA_CHANGE kNtErrorDsNonsafeSchemaChange
#define ERROR_DS_SCHEMA_UPDATE_DISALLOWED kNtErrorDsSchemaUpdateDisallowed
#define ERROR_DS_CANT_CREATE_UNDER_SCHEMA kNtErrorDsCantCreateUnderSchema
#define ERROR_DS_INSTALL_NO_SRC_SCH_VERSION kNtErrorDsInstallNoSrcSchVersion
#define ERROR_DS_INSTALL_NO_SCH_VERSION_IN_INIFILE kNtErrorDsInstallNoSchVersionInInifile
#define ERROR_DS_INVALID_GROUP_TYPE kNtErrorDsInvalidGroupType
#define ERROR_DS_NO_NEST_GLOBALGROUP_IN_MIXEDDOMAIN kNtErrorDsNoNestGlobalgroupInMixeddomain
#define ERROR_DS_NO_NEST_LOCALGROUP_IN_MIXEDDOMAIN kNtErrorDsNoNestLocalgroupInMixeddomain
#define ERROR_DS_GLOBAL_CANT_HAVE_LOCAL_MEMBER kNtErrorDsGlobalCantHaveLocalMember
#define ERROR_DS_GLOBAL_CANT_HAVE_UNIVERSAL_MEMBER kNtErrorDsGlobalCantHaveUniversalMember
#define ERROR_DS_UNIVERSAL_CANT_HAVE_LOCAL_MEMBER kNtErrorDsUniversalCantHaveLocalMember
#define ERROR_DS_GLOBAL_CANT_HAVE_CROSSDOMAIN_MEMBER kNtErrorDsGlobalCantHaveCrossdomainMember
#define ERROR_DS_LOCAL_CANT_HAVE_CROSSDOMAIN_LOCAL_MEMBER kNtErrorDsLocalCantHaveCrossdomainLocalMember
#define ERROR_DS_HAVE_PRIMARY_MEMBERS kNtErrorDsHavePrimaryMembers
#define ERROR_DS_STRING_SD_CONVERSION_FAILED kNtErrorDsStringSdConversionFailed
#define ERROR_DS_NAMING_MASTER_GC kNtErrorDsNamingMasterGc
#define ERROR_DS_DNS_LOOKUP_FAILURE kNtErrorDsDnsLookupFailure
#define ERROR_DS_COULDNT_UPDATE_SPNS kNtErrorDsCouldntUpdateSpns
#define ERROR_DS_CANT_RETRIEVE_SD kNtErrorDsCantRetrieveSd
#define ERROR_DS_KEY_NOT_UNIQUE kNtErrorDsKeyNotUnique
#define ERROR_DS_WRONG_LINKED_ATT_SYNTAX kNtErrorDsWrongLinkedAttSyntax
#define ERROR_DS_SAM_NEED_BOOTKEY_PASSWORD kNtErrorDsSamNeedBootkeyPassword
#define ERROR_DS_SAM_NEED_BOOTKEY_FLOPPY kNtErrorDsSamNeedBootkeyFloppy
#define ERROR_DS_CANT_START kNtErrorDsCantStart
#define ERROR_DS_INIT_FAILURE kNtErrorDsInitFailure
#define ERROR_DS_NO_PKT_PRIVACY_ON_CONNECTION kNtErrorDsNoPktPrivacyOnConnection
#define ERROR_DS_SOURCE_DOMAIN_IN_FOREST kNtErrorDsSourceDomainInForest
#define ERROR_DS_DESTINATION_DOMAIN_NOT_IN_FOREST kNtErrorDsDestinationDomainNotInForest
#define ERROR_DS_DESTINATION_AUDITING_NOT_ENABLED kNtErrorDsDestinationAuditingNotEnabled
#define ERROR_DS_CANT_FIND_DC_FOR_SRC_DOMAIN kNtErrorDsCantFindDcForSrcDomain
#define ERROR_DS_SRC_OBJ_NOT_GROUP_OR_USER kNtErrorDsSrcObjNotGroupOrUser
#define ERROR_DS_SRC_SID_EXISTS_IN_FOREST kNtErrorDsSrcSidExistsInForest
#define ERROR_DS_SRC_AND_DST_OBJECT_CLASS_MISMATCH kNtErrorDsSrcAndDstObjectClassMismatch
#define ERROR_SAM_INIT_FAILURE kNtErrorSamInitFailure
#define ERROR_DS_DRA_SCHEMA_INFO_SHIP kNtErrorDsDraSchemaInfoShip
#define ERROR_DS_DRA_SCHEMA_CONFLICT kNtErrorDsDraSchemaConflict
#define ERROR_DS_DRA_EARLIER_SCHEMA_CONFLICT kNtErrorDsDraEarlierSchemaConflict
#define ERROR_DS_DRA_OBJ_NC_MISMATCH kNtErrorDsDraObjNcMismatch
#define ERROR_DS_NC_STILL_HAS_DSAS kNtErrorDsNcStillHasDsas
#define ERROR_DS_GC_REQUIRED kNtErrorDsGcRequired
#define ERROR_DS_LOCAL_MEMBER_OF_LOCAL_ONLY kNtErrorDsLocalMemberOfLocalOnly
#define ERROR_DS_NO_FPO_IN_UNIVERSAL_GROUPS kNtErrorDsNoFpoInUniversalGroups
#define ERROR_DS_CANT_ADD_TO_GC kNtErrorDsCantAddToGc
#define ERROR_DS_NO_CHECKPOINT_WITH_PDC kNtErrorDsNoCheckpointWithPdc
#define ERROR_DS_SOURCE_AUDITING_NOT_ENABLED kNtErrorDsSourceAuditingNotEnabled
#define ERROR_DS_CANT_CREATE_IN_NONDOMAIN_NC kNtErrorDsCantCreateInNondomainNc
#define ERROR_DS_INVALID_NAME_FOR_SPN kNtErrorDsInvalidNameForSpn
#define ERROR_DS_FILTER_USES_CONTRUCTED_ATTRS kNtErrorDsFilterUsesContructedAttrs
#define ERROR_DS_UNICODEPWD_NOT_IN_QUOTES kNtErrorDsUnicodepwdNotInQuotes
#define ERROR_DS_MACHINE_ACCOUNT_QUOTA_EXCEEDED kNtErrorDsMachineAccountQuotaExceeded
#define ERROR_DS_MUST_BE_RUN_ON_DST_DC kNtErrorDsMustBeRunOnDstDc
#define ERROR_DS_SRC_DC_MUST_BE_SP4_OR_GREATER kNtErrorDsSrcDcMustBeSp4OrGreater
#define ERROR_DS_CANT_TREE_DELETE_CRITICAL_OBJ kNtErrorDsCantTreeDeleteCriticalObj
#define ERROR_DS_INIT_FAILURE_CONSOLE kNtErrorDsInitFailureConsole
#define ERROR_DS_SAM_INIT_FAILURE_CONSOLE kNtErrorDsSamInitFailureConsole
#define ERROR_DS_FOREST_VERSION_TOO_HIGH kNtErrorDsForestVersionTooHigh
#define ERROR_DS_DOMAIN_VERSION_TOO_HIGH kNtErrorDsDomainVersionTooHigh
#define ERROR_DS_FOREST_VERSION_TOO_LOW kNtErrorDsForestVersionTooLow
#define ERROR_DS_DOMAIN_VERSION_TOO_LOW kNtErrorDsDomainVersionTooLow
#define ERROR_DS_INCOMPATIBLE_VERSION kNtErrorDsIncompatibleVersion
#define ERROR_DS_LOW_DSA_VERSION kNtErrorDsLowDsaVersion
#define ERROR_DS_NO_BEHAVIOR_VERSION_IN_MIXEDDOMAIN kNtErrorDsNoBehaviorVersionInMixeddomain
#define ERROR_DS_NOT_SUPPORTED_SORT_ORDER kNtErrorDsNotSupportedSortOrder
#define ERROR_DS_NAME_NOT_UNIQUE kNtErrorDsNameNotUnique
#define ERROR_DS_MACHINE_ACCOUNT_CREATED_PRENT4 kNtErrorDsMachineAccountCreatedPrent4
#define ERROR_DS_OUT_OF_VERSION_STORE kNtErrorDsOutOfVersionStore
#define ERROR_DS_INCOMPATIBLE_CONTROLS_USED kNtErrorDsIncompatibleControlsUsed
#define ERROR_DS_NO_REF_DOMAIN kNtErrorDsNoRefDomain
#define ERROR_DS_RESERVED_LINK_ID kNtErrorDsReservedLinkId
#define ERROR_DS_LINK_ID_NOT_AVAILABLE kNtErrorDsLinkIdNotAvailable
#define ERROR_DS_AG_CANT_HAVE_UNIVERSAL_MEMBER kNtErrorDsAgCantHaveUniversalMember
#define ERROR_DS_MODIFYDN_DISALLOWED_BY_INSTANCE_TYPE kNtErrorDsModifydnDisallowedByInstanceType
#define ERROR_DS_NO_OBJECT_MOVE_IN_SCHEMA_NC kNtErrorDsNoObjectMoveInSchemaNc
#define ERROR_DS_MODIFYDN_DISALLOWED_BY_FLAG kNtErrorDsModifydnDisallowedByFlag
#define ERROR_DS_MODIFYDN_WRONG_GRANDPARENT kNtErrorDsModifydnWrongGrandparent
#define ERROR_DS_NAME_ERROR_TRUST_REFERRAL kNtErrorDsNameErrorTrustReferral
#define ERROR_NOT_SUPPORTED_ON_STANDARD_SERVER kNtErrorNotSupportedOnStandardServer
#define ERROR_DS_CANT_ACCESS_REMOTE_PART_OF_AD kNtErrorDsCantAccessRemotePartOfAd
#define ERROR_DS_CR_IMPOSSIBLE_TO_VALIDATE_V2 kNtErrorDsCrImpossibleToValidateV2
#define ERROR_DS_THREAD_LIMIT_EXCEEDED kNtErrorDsThreadLimitExceeded
#define ERROR_DS_NOT_CLOSEST kNtErrorDsNotClosest
#define ERROR_DS_CANT_DERIVE_SPN_WITHOUT_SERVER_REF kNtErrorDsCantDeriveSpnWithoutServerRef
#define ERROR_DS_SINGLE_USER_MODE_FAILED kNtErrorDsSingleUserModeFailed
#define ERROR_DS_NTDSCRIPT_SYNTAX_ERROR kNtErrorDsNtdscriptSyntaxError
#define ERROR_DS_NTDSCRIPT_PROCESS_ERROR kNtErrorDsNtdscriptProcessError
#define ERROR_DS_DIFFERENT_REPL_EPOCHS kNtErrorDsDifferentReplEpochs
#define ERROR_DS_DRS_EXTENSIONS_CHANGED kNtErrorDsDrsExtensionsChanged
#define ERROR_DS_REPLICA_SET_CHANGE_NOT_ALLOWED_ON_DISABLED_CR kNtErrorDsReplicaSetChangeNotAllowedOnDisabledCr
#define ERROR_DS_NO_MSDS_INTID kNtErrorDsNoMsdsIntid
#define ERROR_DS_DUP_MSDS_INTID kNtErrorDsDupMsdsIntid
#define ERROR_DS_EXISTS_IN_RDNATTID kNtErrorDsExistsInRdnattid
#define ERROR_DS_AUTHORIZATION_FAILED kNtErrorDsAuthorizationFailed
#define ERROR_DS_INVALID_SCRIPT kNtErrorDsInvalidScript
#define ERROR_DS_REMOTE_CROSSREF_OP_FAILED kNtErrorDsRemoteCrossrefOpFailed
#define ERROR_DS_CROSS_REF_BUSY kNtErrorDsCrossRefBusy
#define ERROR_DS_CANT_DERIVE_SPN_FOR_DELETED_DOMAIN kNtErrorDsCantDeriveSpnForDeletedDomain
#define ERROR_DS_CANT_DEMOTE_WITH_WRITEABLE_NC kNtErrorDsCantDemoteWithWriteableNc
#define ERROR_DS_DUPLICATE_ID_FOUND kNtErrorDsDuplicateIdFound
#define ERROR_DS_INSUFFICIENT_ATTR_TO_CREATE_OBJECT kNtErrorDsInsufficientAttrToCreateObject
#define ERROR_DS_GROUP_CONVERSION_ERROR kNtErrorDsGroupConversionError
#define ERROR_DS_CANT_MOVE_APP_BASIC_GROUP kNtErrorDsCantMoveAppBasicGroup
#define ERROR_DS_CANT_MOVE_APP_QUERY_GROUP kNtErrorDsCantMoveAppQueryGroup
#define ERROR_DS_ROLE_NOT_VERIFIED kNtErrorDsRoleNotVerified
#define ERROR_DS_WKO_CONTAINER_CANNOT_BE_SPECIAL kNtErrorDsWkoContainerCannotBeSpecial
#define ERROR_DS_DOMAIN_RENAME_IN_PROGRESS kNtErrorDsDomainRenameInProgress
#define ERROR_DS_EXISTING_AD_CHILD_NC kNtErrorDsExistingAdChildNc
#define ERROR_DS_REPL_LIFETIME_EXCEEDED kNtErrorDsReplLifetimeExceeded
#define ERROR_DS_DISALLOWED_IN_SYSTEM_CONTAINER kNtErrorDsDisallowedInSystemContainer
#define ERROR_DS_LDAP_SEND_QUEUE_FULL kNtErrorDsLdapSendQueueFull
#define ERROR_DS_DRA_OUT_SCHEDULE_WINDOW kNtErrorDsDraOutScheduleWindow
#define ERROR_DS_POLICY_NOT_KNOWN kNtErrorDsPolicyNotKnown
#define ERROR_NO_SITE_SETTINGS_OBJECT kNtErrorNoSiteSettingsObject
#define ERROR_NO_SECRETS kNtErrorNoSecrets
#define ERROR_NO_WRITABLE_DC_FOUND kNtErrorNoWritableDcFound
#define ERROR_DS_NO_SERVER_OBJECT kNtErrorDsNoServerObject
#define ERROR_DS_NO_NTDSA_OBJECT kNtErrorDsNoNtdsaObject
#define ERROR_DS_NON_ASQ_SEARCH kNtErrorDsNonAsqSearch
#define ERROR_DS_AUDIT_FAILURE kNtErrorDsAuditFailure
#define ERROR_DS_INVALID_SEARCH_FLAG_SUBTREE kNtErrorDsInvalidSearchFlagSubtree
#define ERROR_DS_INVALID_SEARCH_FLAG_TUPLE kNtErrorDsInvalidSearchFlagTuple
#define ERROR_DS_HIERARCHY_TABLE_TOO_DEEP kNtErrorDsHierarchyTableTooDeep
#define ERROR_DS_DRA_CORRUPT_UTD_VECTOR kNtErrorDsDraCorruptUtdVector
#define ERROR_DS_DRA_SECRETS_DENIED kNtErrorDsDraSecretsDenied
#define ERROR_DS_RESERVED_MAPI_ID kNtErrorDsReservedMapiId
#define ERROR_DS_MAPI_ID_NOT_AVAILABLE kNtErrorDsMapiIdNotAvailable
#define ERROR_DS_DRA_MISSING_KRBTGT_SECRET kNtErrorDsDraMissingKrbtgtSecret
#define ERROR_DS_DOMAIN_NAME_EXISTS_IN_FOREST kNtErrorDsDomainNameExistsInForest
#define ERROR_DS_FLAT_NAME_EXISTS_IN_FOREST kNtErrorDsFlatNameExistsInForest
#define ERROR_INVALID_USER_PRINCIPAL_NAME kNtErrorInvalidUserPrincipalName
#define ERROR_DS_OID_MAPPED_GROUP_CANT_HAVE_MEMBERS kNtErrorDsOidMappedGroupCantHaveMembers
#define ERROR_DS_OID_NOT_FOUND kNtErrorDsOidNotFound
#define ERROR_DS_DRA_RECYCLED_TARGET kNtErrorDsDraRecycledTarget
#define ERROR_DS_DISALLOWED_NC_REDIRECT kNtErrorDsDisallowedNcRedirect
#define ERROR_DS_HIGH_ADLDS_FFL kNtErrorDsHighAdldsFfl
#define ERROR_DS_HIGH_DSA_VERSION kNtErrorDsHighDsaVersion
#define ERROR_DS_LOW_ADLDS_FFL kNtErrorDsLowAdldsFfl
#define ERROR_DOMAIN_SID_SAME_AS_LOCAL_WORKSTATION kNtErrorDomainSidSameAsLocalWorkstation
#define ERROR_DS_UNDELETE_SAM_VALIDATION_FAILED kNtErrorDsUndeleteSamValidationFailed
#define ERROR_INCORRECT_ACCOUNT_TYPE kNtErrorIncorrectAccountType
#define ERROR_DS_SPN_VALUE_NOT_UNIQUE_IN_FOREST kNtErrorDsSpnValueNotUniqueInForest
#define ERROR_DS_UPN_VALUE_NOT_UNIQUE_IN_FOREST kNtErrorDsUpnValueNotUniqueInForest
#define ERROR_DS_MISSING_FOREST_TRUST kNtErrorDsMissingForestTrust
#define ERROR_DS_VALUE_KEY_NOT_UNIQUE kNtErrorDsValueKeyNotUnique
#define ERROR_IPSEC_QM_POLICY_EXISTS kNtErrorIpsecQmPolicyExists
#define ERROR_IPSEC_QM_POLICY_NOT_FOUND kNtErrorIpsecQmPolicyNotFound
#define ERROR_IPSEC_QM_POLICY_IN_USE kNtErrorIpsecQmPolicyInUse
#define ERROR_IPSEC_MM_POLICY_EXISTS kNtErrorIpsecMmPolicyExists
#define ERROR_IPSEC_MM_POLICY_NOT_FOUND kNtErrorIpsecMmPolicyNotFound
#define ERROR_IPSEC_MM_POLICY_IN_USE kNtErrorIpsecMmPolicyInUse
#define ERROR_IPSEC_MM_FILTER_EXISTS kNtErrorIpsecMmFilterExists
#define ERROR_IPSEC_MM_FILTER_NOT_FOUND kNtErrorIpsecMmFilterNotFound
#define ERROR_IPSEC_TRANSPORT_FILTER_EXISTS kNtErrorIpsecTransportFilterExists
#define ERROR_IPSEC_TRANSPORT_FILTER_NOT_FOUND kNtErrorIpsecTransportFilterNotFound
#define ERROR_IPSEC_MM_AUTH_EXISTS kNtErrorIpsecMmAuthExists
#define ERROR_IPSEC_MM_AUTH_NOT_FOUND kNtErrorIpsecMmAuthNotFound
#define ERROR_IPSEC_MM_AUTH_IN_USE kNtErrorIpsecMmAuthInUse
#define ERROR_IPSEC_DEFAULT_MM_POLICY_NOT_FOUND kNtErrorIpsecDefaultMmPolicyNotFound
#define ERROR_IPSEC_DEFAULT_MM_AUTH_NOT_FOUND kNtErrorIpsecDefaultMmAuthNotFound
#define ERROR_IPSEC_DEFAULT_QM_POLICY_NOT_FOUND kNtErrorIpsecDefaultQmPolicyNotFound
#define ERROR_IPSEC_TUNNEL_FILTER_EXISTS kNtErrorIpsecTunnelFilterExists
#define ERROR_IPSEC_TUNNEL_FILTER_NOT_FOUND kNtErrorIpsecTunnelFilterNotFound
#define ERROR_IPSEC_MM_FILTER_PENDING_DELETION kNtErrorIpsecMmFilterPendingDeletion
#define ERROR_IPSEC_TRANSPORT_FILTER_PENDING_DELETION kNtErrorIpsecTransportFilterPendingDeletion
#define ERROR_IPSEC_TUNNEL_FILTER_PENDING_DELETION kNtErrorIpsecTunnelFilterPendingDeletion
#define ERROR_IPSEC_MM_POLICY_PENDING_DELETION kNtErrorIpsecMmPolicyPendingDeletion
#define ERROR_IPSEC_MM_AUTH_PENDING_DELETION kNtErrorIpsecMmAuthPendingDeletion
#define ERROR_IPSEC_QM_POLICY_PENDING_DELETION kNtErrorIpsecQmPolicyPendingDeletion
#define ERROR_IPSEC_IKE_NEG_STATUS_BEGIN kNtErrorIpsecIkeNegStatusBegin
#define ERROR_IPSEC_IKE_AUTH_FAIL kNtErrorIpsecIkeAuthFail
#define ERROR_IPSEC_IKE_ATTRIB_FAIL kNtErrorIpsecIkeAttribFail
#define ERROR_IPSEC_IKE_NEGOTIATION_PENDING kNtErrorIpsecIkeNegotiationPending
#define ERROR_IPSEC_IKE_GENERAL_PROCESSING_ERROR kNtErrorIpsecIkeGeneralProcessingError
#define ERROR_IPSEC_IKE_TIMED_OUT kNtErrorIpsecIkeTimedOut
#define ERROR_IPSEC_IKE_NO_CERT kNtErrorIpsecIkeNoCert
#define ERROR_IPSEC_IKE_SA_DELETED kNtErrorIpsecIkeSaDeleted
#define ERROR_IPSEC_IKE_SA_REAPED kNtErrorIpsecIkeSaReaped
#define ERROR_IPSEC_IKE_MM_ACQUIRE_DROP kNtErrorIpsecIkeMmAcquireDrop
#define ERROR_IPSEC_IKE_QM_ACQUIRE_DROP kNtErrorIpsecIkeQmAcquireDrop
#define ERROR_IPSEC_IKE_QUEUE_DROP_MM kNtErrorIpsecIkeQueueDropMm
#define ERROR_IPSEC_IKE_QUEUE_DROP_NO_MM kNtErrorIpsecIkeQueueDropNoMm
#define ERROR_IPSEC_IKE_DROP_NO_RESPONSE kNtErrorIpsecIkeDropNoResponse
#define ERROR_IPSEC_IKE_MM_DELAY_DROP kNtErrorIpsecIkeMmDelayDrop
#define ERROR_IPSEC_IKE_QM_DELAY_DROP kNtErrorIpsecIkeQmDelayDrop
#define ERROR_IPSEC_IKE_ERROR kNtErrorIpsecIkeError
#define ERROR_IPSEC_IKE_CRL_FAILED kNtErrorIpsecIkeCrlFailed
#define ERROR_IPSEC_IKE_INVALID_KEY_USAGE kNtErrorIpsecIkeInvalidKeyUsage
#define ERROR_IPSEC_IKE_INVALID_CERT_TYPE kNtErrorIpsecIkeInvalidCertType
#define ERROR_IPSEC_IKE_NO_PRIVATE_KEY kNtErrorIpsecIkeNoPrivateKey
#define ERROR_IPSEC_IKE_SIMULTANEOUS_REKEY kNtErrorIpsecIkeSimultaneousRekey
#define ERROR_IPSEC_IKE_DH_FAIL kNtErrorIpsecIkeDhFail
#define ERROR_IPSEC_IKE_CRITICAL_PAYLOAD_NOT_RECOGNIZED kNtErrorIpsecIkeCriticalPayloadNotRecognized
#define ERROR_IPSEC_IKE_INVALID_HEADER kNtErrorIpsecIkeInvalidHeader
#define ERROR_IPSEC_IKE_NO_POLICY kNtErrorIpsecIkeNoPolicy
#define ERROR_IPSEC_IKE_INVALID_SIGNATURE kNtErrorIpsecIkeInvalidSignature
#define ERROR_IPSEC_IKE_KERBEROS_ERROR kNtErrorIpsecIkeKerberosError
#define ERROR_IPSEC_IKE_NO_PUBLIC_KEY kNtErrorIpsecIkeNoPublicKey
#define ERROR_IPSEC_IKE_PROCESS_ERR kNtErrorIpsecIkeProcessErr
#define ERROR_IPSEC_IKE_PROCESS_ERR_SA kNtErrorIpsecIkeProcessErrSa
#define ERROR_IPSEC_IKE_PROCESS_ERR_PROP kNtErrorIpsecIkeProcessErrProp
#define ERROR_IPSEC_IKE_PROCESS_ERR_TRANS kNtErrorIpsecIkeProcessErrTrans
#define ERROR_IPSEC_IKE_PROCESS_ERR_KE kNtErrorIpsecIkeProcessErrKe
#define ERROR_IPSEC_IKE_PROCESS_ERR_ID kNtErrorIpsecIkeProcessErrId
#define ERROR_IPSEC_IKE_PROCESS_ERR_CERT kNtErrorIpsecIkeProcessErrCert
#define ERROR_IPSEC_IKE_PROCESS_ERR_CERT_REQ kNtErrorIpsecIkeProcessErrCertReq
#define ERROR_IPSEC_IKE_PROCESS_ERR_HASH kNtErrorIpsecIkeProcessErrHash
#define ERROR_IPSEC_IKE_PROCESS_ERR_SIG kNtErrorIpsecIkeProcessErrSig
#define ERROR_IPSEC_IKE_PROCESS_ERR_NONCE kNtErrorIpsecIkeProcessErrNonce
#define ERROR_IPSEC_IKE_PROCESS_ERR_NOTIFY kNtErrorIpsecIkeProcessErrNotify
#define ERROR_IPSEC_IKE_PROCESS_ERR_DELETE kNtErrorIpsecIkeProcessErrDelete
#define ERROR_IPSEC_IKE_PROCESS_ERR_VENDOR kNtErrorIpsecIkeProcessErrVendor
#define ERROR_IPSEC_IKE_INVALID_PAYLOAD kNtErrorIpsecIkeInvalidPayload
#define ERROR_IPSEC_IKE_LOAD_SOFT_SA kNtErrorIpsecIkeLoadSoftSa
#define ERROR_IPSEC_IKE_SOFT_SA_TORN_DOWN kNtErrorIpsecIkeSoftSaTornDown
#define ERROR_IPSEC_IKE_INVALID_COOKIE kNtErrorIpsecIkeInvalidCookie
#define ERROR_IPSEC_IKE_NO_PEER_CERT kNtErrorIpsecIkeNoPeerCert
#define ERROR_IPSEC_IKE_PEER_CRL_FAILED kNtErrorIpsecIkePeerCrlFailed
#define ERROR_IPSEC_IKE_POLICY_CHANGE kNtErrorIpsecIkePolicyChange
#define ERROR_IPSEC_IKE_NO_MM_POLICY kNtErrorIpsecIkeNoMmPolicy
#define ERROR_IPSEC_IKE_NOTCBPRIV kNtErrorIpsecIkeNotcbpriv
#define ERROR_IPSEC_IKE_SECLOADFAIL kNtErrorIpsecIkeSecloadfail
#define ERROR_IPSEC_IKE_FAILSSPINIT kNtErrorIpsecIkeFailsspinit
#define ERROR_IPSEC_IKE_FAILQUERYSSP kNtErrorIpsecIkeFailqueryssp
#define ERROR_IPSEC_IKE_SRVACQFAIL kNtErrorIpsecIkeSrvacqfail
#define ERROR_IPSEC_IKE_SRVQUERYCRED kNtErrorIpsecIkeSrvquerycred
#define ERROR_IPSEC_IKE_GETSPIFAIL kNtErrorIpsecIkeGetspifail
#define ERROR_IPSEC_IKE_INVALID_FILTER kNtErrorIpsecIkeInvalidFilter
#define ERROR_IPSEC_IKE_OUT_OF_MEMORY kNtErrorIpsecIkeOutOfMemory
#define ERROR_IPSEC_IKE_ADD_UPDATE_KEY_FAILED kNtErrorIpsecIkeAddUpdateKeyFailed
#define ERROR_IPSEC_IKE_INVALID_POLICY kNtErrorIpsecIkeInvalidPolicy
#define ERROR_IPSEC_IKE_UNKNOWN_DOI kNtErrorIpsecIkeUnknownDoi
#define ERROR_IPSEC_IKE_INVALID_SITUATION kNtErrorIpsecIkeInvalidSituation
#define ERROR_IPSEC_IKE_DH_FAILURE kNtErrorIpsecIkeDhFailure
#define ERROR_IPSEC_IKE_INVALID_GROUP kNtErrorIpsecIkeInvalidGroup
#define ERROR_IPSEC_IKE_ENCRYPT kNtErrorIpsecIkeEncrypt
#define ERROR_IPSEC_IKE_DECRYPT kNtErrorIpsecIkeDecrypt
#define ERROR_IPSEC_IKE_POLICY_MATCH kNtErrorIpsecIkePolicyMatch
#define ERROR_IPSEC_IKE_UNSUPPORTED_ID kNtErrorIpsecIkeUnsupportedId
#define ERROR_IPSEC_IKE_INVALID_HASH kNtErrorIpsecIkeInvalidHash
#define ERROR_IPSEC_IKE_INVALID_HASH_ALG kNtErrorIpsecIkeInvalidHashAlg
#define ERROR_IPSEC_IKE_INVALID_HASH_SIZE kNtErrorIpsecIkeInvalidHashSize
#define ERROR_IPSEC_IKE_INVALID_ENCRYPT_ALG kNtErrorIpsecIkeInvalidEncryptAlg
#define ERROR_IPSEC_IKE_INVALID_AUTH_ALG kNtErrorIpsecIkeInvalidAuthAlg
#define ERROR_IPSEC_IKE_INVALID_SIG kNtErrorIpsecIkeInvalidSig
#define ERROR_IPSEC_IKE_LOAD_FAILED kNtErrorIpsecIkeLoadFailed
#define ERROR_IPSEC_IKE_RPC_DELETE kNtErrorIpsecIkeRpcDelete
#define ERROR_IPSEC_IKE_BENIGN_REINIT kNtErrorIpsecIkeBenignReinit
#define ERROR_IPSEC_IKE_INVALID_RESPONDER_LIFETIME_NOTIFY kNtErrorIpsecIkeInvalidResponderLifetimeNotify
#define ERROR_IPSEC_IKE_INVALID_MAJOR_VERSION kNtErrorIpsecIkeInvalidMajorVersion
#define ERROR_IPSEC_IKE_INVALID_CERT_KEYLEN kNtErrorIpsecIkeInvalidCertKeylen
#define ERROR_IPSEC_IKE_MM_LIMIT kNtErrorIpsecIkeMmLimit
#define ERROR_IPSEC_IKE_NEGOTIATION_DISABLED kNtErrorIpsecIkeNegotiationDisabled
#define ERROR_IPSEC_IKE_QM_LIMIT kNtErrorIpsecIkeQmLimit
#define ERROR_IPSEC_IKE_MM_EXPIRED kNtErrorIpsecIkeMmExpired
#define ERROR_IPSEC_IKE_PEER_MM_ASSUMED_INVALID kNtErrorIpsecIkePeerMmAssumedInvalid
#define ERROR_IPSEC_IKE_CERT_CHAIN_POLICY_MISMATCH kNtErrorIpsecIkeCertChainPolicyMismatch
#define ERROR_IPSEC_IKE_UNEXPECTED_MESSAGE_ID kNtErrorIpsecIkeUnexpectedMessageId
#define ERROR_IPSEC_IKE_INVALID_AUTH_PAYLOAD kNtErrorIpsecIkeInvalidAuthPayload
#define ERROR_IPSEC_IKE_DOS_COOKIE_SENT kNtErrorIpsecIkeDosCookieSent
#define ERROR_IPSEC_IKE_SHUTTING_DOWN kNtErrorIpsecIkeShuttingDown
#define ERROR_IPSEC_IKE_CGA_AUTH_FAILED kNtErrorIpsecIkeCgaAuthFailed
#define ERROR_IPSEC_IKE_PROCESS_ERR_NATOA kNtErrorIpsecIkeProcessErrNatoa
#define ERROR_IPSEC_IKE_INVALID_MM_FOR_QM kNtErrorIpsecIkeInvalidMmForQm
#define ERROR_IPSEC_IKE_QM_EXPIRED kNtErrorIpsecIkeQmExpired
#define ERROR_IPSEC_IKE_TOO_MANY_FILTERS kNtErrorIpsecIkeTooManyFilters
#define ERROR_IPSEC_IKE_NEG_STATUS_END kNtErrorIpsecIkeNegStatusEnd
#define ERROR_IPSEC_IKE_KILL_DUMMY_NAP_TUNNEL kNtErrorIpsecIkeKillDummyNapTunnel
#define ERROR_IPSEC_IKE_INNER_IP_ASSIGNMENT_FAILURE kNtErrorIpsecIkeInnerIpAssignmentFailure
#define ERROR_IPSEC_IKE_REQUIRE_CP_PAYLOAD_MISSING kNtErrorIpsecIkeRequireCpPayloadMissing
#define ERROR_IPSEC_KEY_MODULE_IMPERSONATION_NEGOTIATION_PENDING kNtErrorIpsecKeyModuleImpersonationNegotiationPending
#define ERROR_IPSEC_IKE_COEXISTENCE_SUPPRESS kNtErrorIpsecIkeCoexistenceSuppress
#define ERROR_IPSEC_IKE_RATELIMIT_DROP kNtErrorIpsecIkeRatelimitDrop
#define ERROR_IPSEC_IKE_PEER_DOESNT_SUPPORT_MOBIKE kNtErrorIpsecIkePeerDoesntSupportMobike
#define ERROR_IPSEC_IKE_AUTHORIZATION_FAILURE kNtErrorIpsecIkeAuthorizationFailure
#define ERROR_IPSEC_IKE_STRONG_CRED_AUTHORIZATION_FAILURE kNtErrorIpsecIkeStrongCredAuthorizationFailure
#define ERROR_IPSEC_IKE_AUTHORIZATION_FAILURE_WITH_OPTIONAL_RETRY kNtErrorIpsecIkeAuthorizationFailureWithOptionalRetry
#define ERROR_IPSEC_IKE_STRONG_CRED_AUTHORIZATION_AND_CERTMAP_FAILURE kNtErrorIpsecIkeStrongCredAuthorizationAndCertmapFailure
#define ERROR_IPSEC_IKE_NEG_STATUS_EXTENDED_END kNtErrorIpsecIkeNegStatusExtendedEnd
#define ERROR_IPSEC_BAD_SPI kNtErrorIpsecBadSpi
#define ERROR_IPSEC_SA_LIFETIME_EXPIRED kNtErrorIpsecSaLifetimeExpired
#define ERROR_IPSEC_WRONG_SA kNtErrorIpsecWrongSa
#define ERROR_IPSEC_REPLAY_CHECK_FAILED kNtErrorIpsecReplayCheckFailed
#define ERROR_IPSEC_INVALID_PACKET kNtErrorIpsecInvalidPacket
#define ERROR_IPSEC_INTEGRITY_CHECK_FAILED kNtErrorIpsecIntegrityCheckFailed
#define ERROR_IPSEC_CLEAR_TEXT_DROP kNtErrorIpsecClearTextDrop
#define ERROR_IPSEC_AUTH_FIREWALL_DROP kNtErrorIpsecAuthFirewallDrop
#define ERROR_IPSEC_THROTTLE_DROP kNtErrorIpsecThrottleDrop
#define ERROR_IPSEC_DOSP_BLOCK kNtErrorIpsecDospBlock
#define ERROR_IPSEC_DOSP_RECEIVED_MULTICAST kNtErrorIpsecDospReceivedMulticast
#define ERROR_IPSEC_DOSP_INVALID_PACKET kNtErrorIpsecDospInvalidPacket
#define ERROR_IPSEC_DOSP_STATE_LOOKUP_FAILED kNtErrorIpsecDospStateLookupFailed
#define ERROR_IPSEC_DOSP_MAX_ENTRIES kNtErrorIpsecDospMaxEntries
#define ERROR_IPSEC_DOSP_KEYMOD_NOT_ALLOWED kNtErrorIpsecDospKeymodNotAllowed
#define ERROR_IPSEC_DOSP_NOT_INSTALLED kNtErrorIpsecDospNotInstalled
#define ERROR_IPSEC_DOSP_MAX_PER_IP_RATELIMIT_QUEUES kNtErrorIpsecDospMaxPerIpRatelimitQueues
#define ERROR_SXS_SECTION_NOT_FOUND kNtErrorSxsSectionNotFound
#define ERROR_SXS_CANT_GEN_ACTCTX kNtErrorSxsCantGenActctx
#define ERROR_SXS_INVALID_ACTCTXDATA_FORMAT kNtErrorSxsInvalidActctxdataFormat
#define ERROR_SXS_ASSEMBLY_NOT_FOUND kNtErrorSxsAssemblyNotFound
#define ERROR_SXS_MANIFEST_FORMAT_ERROR kNtErrorSxsManifestFormatError
#define ERROR_SXS_MANIFEST_PARSE_ERROR kNtErrorSxsManifestParseError
#define ERROR_SXS_ACTIVATION_CONTEXT_DISABLED kNtErrorSxsActivationContextDisabled
#define ERROR_SXS_KEY_NOT_FOUND kNtErrorSxsKeyNotFound
#define ERROR_SXS_VERSION_CONFLICT kNtErrorSxsVersionConflict
#define ERROR_SXS_WRONG_SECTION_TYPE kNtErrorSxsWrongSectionType
#define ERROR_SXS_THREAD_QUERIES_DISABLED kNtErrorSxsThreadQueriesDisabled
#define ERROR_SXS_PROCESS_DEFAULT_ALREADY_SET kNtErrorSxsProcessDefaultAlreadySet
#define ERROR_SXS_UNKNOWN_ENCODING_GROUP kNtErrorSxsUnknownEncodingGroup
#define ERROR_SXS_UNKNOWN_ENCODING kNtErrorSxsUnknownEncoding
#define ERROR_SXS_INVALID_XML_NAMESPACE_URI kNtErrorSxsInvalidXmlNamespaceUri
#define ERROR_SXS_ROOT_MANIFEST_DEPENDENCY_NOT_INSTALLED kNtErrorSxsRootManifestDependencyNotInstalled
#define ERROR_SXS_LEAF_MANIFEST_DEPENDENCY_NOT_INSTALLED kNtErrorSxsLeafManifestDependencyNotInstalled
#define ERROR_SXS_INVALID_ASSEMBLY_IDENTITY_ATTRIBUTE kNtErrorSxsInvalidAssemblyIdentityAttribute
#define ERROR_SXS_MANIFEST_MISSING_REQUIRED_DEFAULT_NAMESPACE kNtErrorSxsManifestMissingRequiredDefaultNamespace
#define ERROR_SXS_MANIFEST_INVALID_REQUIRED_DEFAULT_NAMESPACE kNtErrorSxsManifestInvalidRequiredDefaultNamespace
#define ERROR_SXS_PRIVATE_MANIFEST_CROSS_PATH_WITH_REPARSE_POINT kNtErrorSxsPrivateManifestCrossPathWithReparsePoint
#define ERROR_SXS_DUPLICATE_DLL_NAME kNtErrorSxsDuplicateDllName
#define ERROR_SXS_DUPLICATE_WINDOWCLASS_NAME kNtErrorSxsDuplicateWindowclassName
#define ERROR_SXS_DUPLICATE_CLSID kNtErrorSxsDuplicateClsid
#define ERROR_SXS_DUPLICATE_IID kNtErrorSxsDuplicateIid
#define ERROR_SXS_DUPLICATE_TLBID kNtErrorSxsDuplicateTlbid
#define ERROR_SXS_DUPLICATE_PROGID kNtErrorSxsDuplicateProgid
#define ERROR_SXS_DUPLICATE_ASSEMBLY_NAME kNtErrorSxsDuplicateAssemblyName
#define ERROR_SXS_FILE_HASH_MISMATCH kNtErrorSxsFileHashMismatch
#define ERROR_SXS_POLICY_PARSE_ERROR kNtErrorSxsPolicyParseError
#define ERROR_SXS_XML_EMISSINGQUOTE kNtErrorSxsXmlEMissingquote
#define ERROR_SXS_XML_ECOMMENTSYNTAX kNtErrorSxsXmlECommentsyntax
#define ERROR_SXS_XML_EBADSTARTNAMECHAR kNtErrorSxsXmlEBadstartnamechar
#define ERROR_SXS_XML_EBADNAMECHAR kNtErrorSxsXmlEBadnamechar
#define ERROR_SXS_XML_EBADCHARINSTRING kNtErrorSxsXmlEBadcharinstring
#define ERROR_SXS_XML_EXMLDECLSYNTAX kNtErrorSxsXmlEXmldeclsyntax
#define ERROR_SXS_XML_EBADCHARDATA kNtErrorSxsXmlEBadchardata
#define ERROR_SXS_XML_EMISSINGWHITESPACE kNtErrorSxsXmlEMissingwhitespace
#define ERROR_SXS_XML_EEXPECTINGTAGEND kNtErrorSxsXmlEExpectingtagend
#define ERROR_SXS_XML_EMISSINGSEMICOLON kNtErrorSxsXmlEMissingsemicolon
#define ERROR_SXS_XML_EUNBALANCEDPAREN kNtErrorSxsXmlEUnbalancedparen
#define ERROR_SXS_XML_EINTERNALERROR kNtErrorSxsXmlEInternalerror
#define ERROR_SXS_XML_EUNEXPECTED_WHITESPACE kNtErrorSxsXmlEUnexpectedWhitespace
#define ERROR_SXS_XML_EINCOMPLETE_ENCODING kNtErrorSxsXmlEIncompleteEncoding
#define ERROR_SXS_XML_EMISSING_PAREN kNtErrorSxsXmlEMissingParen
#define ERROR_SXS_XML_EEXPECTINGCLOSEQUOTE kNtErrorSxsXmlEExpectingclosequote
#define ERROR_SXS_XML_EMULTIPLE_COLONS kNtErrorSxsXmlEMultipleColons
#define ERROR_SXS_XML_EINVALID_DECIMAL kNtErrorSxsXmlEInvalidDecimal
#define ERROR_SXS_XML_EINVALID_HEXIDECIMAL kNtErrorSxsXmlEInvalidHexidecimal
#define ERROR_SXS_XML_EINVALID_UNICODE kNtErrorSxsXmlEInvalidUnicode
#define ERROR_SXS_XML_EWHITESPACEORQUESTIONMARK kNtErrorSxsXmlEWhitespaceorquestionmark
#define ERROR_SXS_XML_EUNEXPECTEDENDTAG kNtErrorSxsXmlEUnexpectedendtag
#define ERROR_SXS_XML_EUNCLOSEDTAG kNtErrorSxsXmlEUnclosedtag
#define ERROR_SXS_XML_EDUPLICATEATTRIBUTE kNtErrorSxsXmlEDuplicateattribute
#define ERROR_SXS_XML_EMULTIPLEROOTS kNtErrorSxsXmlEMultipleroots
#define ERROR_SXS_XML_EINVALIDATROOTLEVEL kNtErrorSxsXmlEInvalidatrootlevel
#define ERROR_SXS_XML_EBADXMLDECL kNtErrorSxsXmlEBadxmldecl
#define ERROR_SXS_XML_EMISSINGROOT kNtErrorSxsXmlEMissingroot
#define ERROR_SXS_XML_EUNEXPECTEDEOF kNtErrorSxsXmlEUnexpectedeof
#define ERROR_SXS_XML_EBADPEREFINSUBSET kNtErrorSxsXmlEBadperefinsubset
#define ERROR_SXS_XML_EUNCLOSEDSTARTTAG kNtErrorSxsXmlEUnclosedstarttag
#define ERROR_SXS_XML_EUNCLOSEDENDTAG kNtErrorSxsXmlEUnclosedendtag
#define ERROR_SXS_XML_EUNCLOSEDSTRING kNtErrorSxsXmlEUnclosedstring
#define ERROR_SXS_XML_EUNCLOSEDCOMMENT kNtErrorSxsXmlEUnclosedcomment
#define ERROR_SXS_XML_EUNCLOSEDDECL kNtErrorSxsXmlEUncloseddecl
#define ERROR_SXS_XML_EUNCLOSEDCDATA kNtErrorSxsXmlEUnclosedcdata
#define ERROR_SXS_XML_ERESERVEDNAMESPACE kNtErrorSxsXmlEReservednamespace
#define ERROR_SXS_XML_EINVALIDENCODING kNtErrorSxsXmlEInvalidencoding
#define ERROR_SXS_XML_EINVALIDSWITCH kNtErrorSxsXmlEInvalidswitch
#define ERROR_SXS_XML_EBADXMLCASE kNtErrorSxsXmlEBadxmlcase
#define ERROR_SXS_XML_EINVALID_STANDALONE kNtErrorSxsXmlEInvalidStandalone
#define ERROR_SXS_XML_EUNEXPECTED_STANDALONE kNtErrorSxsXmlEUnexpectedStandalone
#define ERROR_SXS_XML_EINVALID_VERSION kNtErrorSxsXmlEInvalidVersion
#define ERROR_SXS_XML_EMISSINGEQUALS kNtErrorSxsXmlEMissingequals
#define ERROR_SXS_PROTECTION_RECOVERY_FAILED kNtErrorSxsProtectionRecoveryFailed
#define ERROR_SXS_PROTECTION_PUBLIC_KEY_TOO_SHORT kNtErrorSxsProtectionPublicKeyTooShort
#define ERROR_SXS_PROTECTION_CATALOG_NOT_VALID kNtErrorSxsProtectionCatalogNotValid
#define ERROR_SXS_UNTRANSLATABLE_HRESULT kNtErrorSxsUntranslatableHresult
#define ERROR_SXS_PROTECTION_CATALOG_FILE_MISSING kNtErrorSxsProtectionCatalogFileMissing
#define ERROR_SXS_MISSING_ASSEMBLY_IDENTITY_ATTRIBUTE kNtErrorSxsMissingAssemblyIdentityAttribute
#define ERROR_SXS_INVALID_ASSEMBLY_IDENTITY_ATTRIBUTE_NAME kNtErrorSxsInvalidAssemblyIdentityAttributeName
#define ERROR_SXS_ASSEMBLY_MISSING kNtErrorSxsAssemblyMissing
#define ERROR_SXS_CORRUPT_ACTIVATION_STACK kNtErrorSxsCorruptActivationStack
#define ERROR_SXS_CORRUPTION kNtErrorSxsCorruption
#define ERROR_SXS_EARLY_DEACTIVATION kNtErrorSxsEarlyDeactivation
#define ERROR_SXS_INVALID_DEACTIVATION kNtErrorSxsInvalidDeactivation
#define ERROR_SXS_MULTIPLE_DEACTIVATION kNtErrorSxsMultipleDeactivation
#define ERROR_SXS_PROCESS_TERMINATION_REQUESTED kNtErrorSxsProcessTerminationRequested
#define ERROR_SXS_RELEASE_ACTIVATION_CONTEXT kNtErrorSxsReleaseActivationContext
#define ERROR_SXS_SYSTEM_DEFAULT_ACTIVATION_CONTEXT_EMPTY kNtErrorSxsSystemDefaultActivationContextEmpty
#define ERROR_SXS_INVALID_IDENTITY_ATTRIBUTE_VALUE kNtErrorSxsInvalidIdentityAttributeValue
#define ERROR_SXS_INVALID_IDENTITY_ATTRIBUTE_NAME kNtErrorSxsInvalidIdentityAttributeName
#define ERROR_SXS_IDENTITY_DUPLICATE_ATTRIBUTE kNtErrorSxsIdentityDuplicateAttribute
#define ERROR_SXS_IDENTITY_PARSE_ERROR kNtErrorSxsIdentityParseError
#define ERROR_MALFORMED_SUBSTITUTION_STRING kNtErrorMalformedSubstitutionString
#define ERROR_SXS_INCORRECT_PUBLIC_KEY_TOKEN kNtErrorSxsIncorrectPublicKeyToken
#define ERROR_UNMAPPED_SUBSTITUTION_STRING kNtErrorUnmappedSubstitutionString
#define ERROR_SXS_ASSEMBLY_NOT_LOCKED kNtErrorSxsAssemblyNotLocked
#define ERROR_SXS_COMPONENT_STORE_CORRUPT kNtErrorSxsComponentStoreCorrupt
#define ERROR_ADVANCED_INSTALLER_FAILED kNtErrorAdvancedInstallerFailed
#define ERROR_XML_ENCODING_MISMATCH kNtErrorXmlEncodingMismatch
#define ERROR_SXS_MANIFEST_IDENTITY_SAME_BUT_CONTENTS_DIFFERENT kNtErrorSxsManifestIdentitySameButContentsDifferent
#define ERROR_SXS_IDENTITIES_DIFFERENT kNtErrorSxsIdentitiesDifferent
#define ERROR_SXS_ASSEMBLY_IS_NOT_ADEPLOYMENT kNtErrorSxsAssemblyIsNotADeployment
#define ERROR_SXS_FILE_NOT_PART_OF_ASSEMBLY kNtErrorSxsFileNotPartOfAssembly
#define ERROR_SXS_MANIFEST_TOO_BIG kNtErrorSxsManifestTooBig
#define ERROR_SXS_SETTING_NOT_REGISTERED kNtErrorSxsSettingNotRegistered
#define ERROR_SXS_TRANSACTION_CLOSURE_INCOMPLETE kNtErrorSxsTransactionClosureIncomplete
#define ERROR_SMI_PRIMITIVE_INSTALLER_FAILED kNtErrorSmiPrimitiveInstallerFailed
#define ERROR_GENERIC_COMMAND_FAILED kNtErrorGenericCommandFailed
#define ERROR_SXS_FILE_HASH_MISSING kNtErrorSxsFileHashMissing
#define ERROR_EVT_INVALID_CHANNEL_PATH kNtErrorEvtInvalidChannelPath
#define ERROR_EVT_INVALID_QUERY kNtErrorEvtInvalidQuery
#define ERROR_EVT_PUBLISHER_METADATA_NOT_FOUND kNtErrorEvtPublisherMetadataNotFound
#define ERROR_EVT_EVENT_TEMPLATE_NOT_FOUND kNtErrorEvtEventTemplateNotFound
#define ERROR_EVT_INVALID_PUBLISHER_NAME kNtErrorEvtInvalidPublisherName
#define ERROR_EVT_INVALID_EVENT_DATA kNtErrorEvtInvalidEventData
#define ERROR_EVT_CHANNEL_NOT_FOUND kNtErrorEvtChannelNotFound
#define ERROR_EVT_MALFORMED_XML_TEXT kNtErrorEvtMalformedXmlText
#define ERROR_EVT_SUBSCRIPTION_TO_DIRECT_CHANNEL kNtErrorEvtSubscriptionToDirectChannel
#define ERROR_EVT_CONFIGURATION_ERROR kNtErrorEvtConfigurationError
#define ERROR_EVT_QUERY_RESULT_STALE kNtErrorEvtQueryResultStale
#define ERROR_EVT_QUERY_RESULT_INVALID_POSITION kNtErrorEvtQueryResultInvalidPosition
#define ERROR_EVT_NON_VALIDATING_MSXML kNtErrorEvtNonValidatingMsxml
#define ERROR_EVT_FILTER_ALREADYSCOPED kNtErrorEvtFilterAlreadyscoped
#define ERROR_EVT_FILTER_NOTELTSET kNtErrorEvtFilterNoteltset
#define ERROR_EVT_FILTER_INVARG kNtErrorEvtFilterInvarg
#define ERROR_EVT_FILTER_INVTEST kNtErrorEvtFilterInvtest
#define ERROR_EVT_FILTER_INVTYPE kNtErrorEvtFilterInvtype
#define ERROR_EVT_FILTER_PARSEERR kNtErrorEvtFilterParseerr
#define ERROR_EVT_FILTER_UNSUPPORTEDOP kNtErrorEvtFilterUnsupportedop
#define ERROR_EVT_FILTER_UNEXPECTEDTOKEN kNtErrorEvtFilterUnexpectedtoken
#define ERROR_EVT_INVALID_OPERATION_OVER_ENABLED_DIRECT_CHANNEL kNtErrorEvtInvalidOperationOverEnabledDirectChannel
#define ERROR_EVT_INVALID_CHANNEL_PROPERTY_VALUE kNtErrorEvtInvalidChannelPropertyValue
#define ERROR_EVT_INVALID_PUBLISHER_PROPERTY_VALUE kNtErrorEvtInvalidPublisherPropertyValue
#define ERROR_EVT_CHANNEL_CANNOT_ACTIVATE kNtErrorEvtChannelCannotActivate
#define ERROR_EVT_FILTER_TOO_COMPLEX kNtErrorEvtFilterTooComplex
#define ERROR_EVT_MESSAGE_NOT_FOUND kNtErrorEvtMessageNotFound
#define ERROR_EVT_MESSAGE_ID_NOT_FOUND kNtErrorEvtMessageIdNotFound
#define ERROR_EVT_UNRESOLVED_VALUE_INSERT kNtErrorEvtUnresolvedValueInsert
#define ERROR_EVT_UNRESOLVED_PARAMETER_INSERT kNtErrorEvtUnresolvedParameterInsert
#define ERROR_EVT_MAX_INSERTS_REACHED kNtErrorEvtMaxInsertsReached
#define ERROR_EVT_EVENT_DEFINITION_NOT_FOUND kNtErrorEvtEventDefinitionNotFound
#define ERROR_EVT_MESSAGE_LOCALE_NOT_FOUND kNtErrorEvtMessageLocaleNotFound
#define ERROR_EVT_VERSION_TOO_OLD kNtErrorEvtVersionTooOld
#define ERROR_EVT_VERSION_TOO_NEW kNtErrorEvtVersionTooNew
#define ERROR_EVT_CANNOT_OPEN_CHANNEL_OF_QUERY kNtErrorEvtCannotOpenChannelOfQuery
#define ERROR_EVT_PUBLISHER_DISABLED kNtErrorEvtPublisherDisabled
#define ERROR_EVT_FILTER_OUT_OF_RANGE kNtErrorEvtFilterOutOfRange
#define ERROR_EC_SUBSCRIPTION_CANNOT_ACTIVATE kNtErrorEcSubscriptionCannotActivate
#define ERROR_EC_LOG_DISABLED kNtErrorEcLogDisabled
#define ERROR_EC_CIRCULAR_FORWARDING kNtErrorEcCircularForwarding
#define ERROR_EC_CREDSTORE_FULL kNtErrorEcCredstoreFull
#define ERROR_EC_CRED_NOT_FOUND kNtErrorEcCredNotFound
#define ERROR_EC_NO_ACTIVE_CHANNEL kNtErrorEcNoActiveChannel
#define ERROR_MUI_FILE_NOT_FOUND kNtErrorMuiFileNotFound
#define ERROR_MUI_INVALID_FILE kNtErrorMuiInvalidFile
#define ERROR_MUI_INVALID_RC_CONFIG kNtErrorMuiInvalidRcConfig
#define ERROR_MUI_INVALID_LOCALE_NAME kNtErrorMuiInvalidLocaleName
#define ERROR_MUI_INVALID_ULTIMATEFALLBACK_NAME kNtErrorMuiInvalidUltimatefallbackName
#define ERROR_MUI_FILE_NOT_LOADED kNtErrorMuiFileNotLoaded
#define ERROR_RESOURCE_ENUM_USER_STOP kNtErrorResourceEnumUserStop
#define ERROR_MUI_INTLSETTINGS_UILANG_NOT_INSTALLED kNtErrorMuiIntlsettingsUilangNotInstalled
#define ERROR_MUI_INTLSETTINGS_INVALID_LOCALE_NAME kNtErrorMuiIntlsettingsInvalidLocaleName
#define ERROR_MRM_RUNTIME_NO_DEFAULT_OR_NEUTRAL_RESOURCE kNtErrorMrmRuntimeNoDefaultOrNeutralResource
#define ERROR_MRM_INVALID_PRICONFIG kNtErrorMrmInvalidPriconfig
#define ERROR_MRM_INVALID_FILE_TYPE kNtErrorMrmInvalidFileType
#define ERROR_MRM_UNKNOWN_QUALIFIER kNtErrorMrmUnknownQualifier
#define ERROR_MRM_INVALID_QUALIFIER_VALUE kNtErrorMrmInvalidQualifierValue
#define ERROR_MRM_NO_CANDIDATE kNtErrorMrmNoCandidate
#define ERROR_MRM_NO_MATCH_OR_DEFAULT_CANDIDATE kNtErrorMrmNoMatchOrDefaultCandidate
#define ERROR_MRM_RESOURCE_TYPE_MISMATCH kNtErrorMrmResourceTypeMismatch
#define ERROR_MRM_DUPLICATE_MAP_NAME kNtErrorMrmDuplicateMapName
#define ERROR_MRM_DUPLICATE_ENTRY kNtErrorMrmDuplicateEntry
#define ERROR_MRM_INVALID_RESOURCE_IDENTIFIER kNtErrorMrmInvalidResourceIdentifier
#define ERROR_MRM_FILEPATH_TOO_LONG kNtErrorMrmFilepathTooLong
#define ERROR_MRM_UNSUPPORTED_DIRECTORY_TYPE kNtErrorMrmUnsupportedDirectoryType
#define ERROR_MRM_INVALID_PRI_FILE kNtErrorMrmInvalidPriFile
#define ERROR_MRM_NAMED_RESOURCE_NOT_FOUND kNtErrorMrmNamedResourceNotFound
#define ERROR_MRM_MAP_NOT_FOUND kNtErrorMrmMapNotFound
#define ERROR_MRM_UNSUPPORTED_PROFILE_TYPE kNtErrorMrmUnsupportedProfileType
#define ERROR_MRM_INVALID_QUALIFIER_OPERATOR kNtErrorMrmInvalidQualifierOperator
#define ERROR_MRM_INDETERMINATE_QUALIFIER_VALUE kNtErrorMrmIndeterminateQualifierValue
#define ERROR_MRM_AUTOMERGE_ENABLED kNtErrorMrmAutomergeEnabled
#define ERROR_MRM_TOO_MANY_RESOURCES kNtErrorMrmTooManyResources
#define ERROR_MRM_UNSUPPORTED_FILE_TYPE_FOR_MERGE kNtErrorMrmUnsupportedFileTypeForMerge
#define ERROR_MRM_UNSUPPORTED_FILE_TYPE_FOR_LOAD_UNLOAD_PRI_FILE kNtErrorMrmUnsupportedFileTypeForLoadUnloadPriFile
#define ERROR_MRM_NO_CURRENT_VIEW_ON_THREAD kNtErrorMrmNoCurrentViewOnThread
#define ERROR_DIFFERENT_PROFILE_RESOURCE_MANAGER_EXIST kNtErrorDifferentProfileResourceManagerExist
#define ERROR_OPERATION_NOT_ALLOWED_FROM_SYSTEM_COMPONENT kNtErrorOperationNotAllowedFromSystemComponent
#define ERROR_MRM_DIRECT_REF_TO_NON_DEFAULT_RESOURCE kNtErrorMrmDirectRefToNonDefaultResource
#define ERROR_MRM_GENERATION_COUNT_MISMATCH kNtErrorMrmGenerationCountMismatch
#define ERROR_PRI_MERGE_VERSION_MISMATCH kNtErrorPriMergeVersionMismatch
#define ERROR_PRI_MERGE_MISSING_SCHEMA kNtErrorPriMergeMissingSchema
#define ERROR_PRI_MERGE_LOAD_FILE_FAILED kNtErrorPriMergeLoadFileFailed
#define ERROR_PRI_MERGE_ADD_FILE_FAILED kNtErrorPriMergeAddFileFailed
#define ERROR_PRI_MERGE_WRITE_FILE_FAILED kNtErrorPriMergeWriteFileFailed
#define ERROR_PRI_MERGE_MULTIPLE_PACKAGE_FAMILIES_NOT_ALLOWED kNtErrorPriMergeMultiplePackageFamiliesNotAllowed
#define ERROR_PRI_MERGE_MULTIPLE_MAIN_PACKAGES_NOT_ALLOWED kNtErrorPriMergeMultipleMainPackagesNotAllowed
#define ERROR_PRI_MERGE_BUNDLE_PACKAGES_NOT_ALLOWED kNtErrorPriMergeBundlePackagesNotAllowed
#define ERROR_PRI_MERGE_MAIN_PACKAGE_REQUIRED kNtErrorPriMergeMainPackageRequired
#define ERROR_PRI_MERGE_RESOURCE_PACKAGE_REQUIRED kNtErrorPriMergeResourcePackageRequired
#define ERROR_PRI_MERGE_INVALID_FILE_NAME kNtErrorPriMergeInvalidFileName
#define ERROR_MCA_INVALID_CAPABILITIES_STRING kNtErrorMcaInvalidCapabilitiesString
#define ERROR_MCA_INVALID_VCP_VERSION kNtErrorMcaInvalidVcpVersion
#define ERROR_MCA_MONITOR_VIOLATES_MCCS_SPECIFICATION kNtErrorMcaMonitorViolatesMccsSpecification
#define ERROR_MCA_MCCS_VERSION_MISMATCH kNtErrorMcaMccsVersionMismatch
#define ERROR_MCA_UNSUPPORTED_MCCS_VERSION kNtErrorMcaUnsupportedMccsVersion
#define ERROR_MCA_INTERNAL_ERROR kNtErrorMcaInternalError
#define ERROR_MCA_INVALID_TECHNOLOGY_TYPE_RETURNED kNtErrorMcaInvalidTechnologyTypeReturned
#define ERROR_MCA_UNSUPPORTED_COLOR_TEMPERATURE kNtErrorMcaUnsupportedColorTemperature
#define ERROR_AMBIGUOUS_SYSTEM_DEVICE kNtErrorAmbiguousSystemDevice
#define ERROR_SYSTEM_DEVICE_NOT_FOUND kNtErrorSystemDeviceNotFound
#define ERROR_HASH_NOT_SUPPORTED kNtErrorHashNotSupported
#define ERROR_HASH_NOT_PRESENT kNtErrorHashNotPresent
#define ERROR_SECONDARY_IC_PROVIDER_NOT_REGISTERED kNtErrorSecondaryIcProviderNotRegistered
#define ERROR_GPIO_CLIENT_INFORMATION_INVALID kNtErrorGpioClientInformationInvalid
#define ERROR_GPIO_VERSION_NOT_SUPPORTED kNtErrorGpioVersionNotSupported
#define ERROR_GPIO_INVALID_REGISTRATION_PACKET kNtErrorGpioInvalidRegistrationPacket
#define ERROR_GPIO_OPERATION_DENIED kNtErrorGpioOperationDenied
#define ERROR_GPIO_INCOMPATIBLE_CONNECT_MODE kNtErrorGpioIncompatibleConnectMode
#define ERROR_GPIO_INTERRUPT_ALREADY_UNMASKED kNtErrorGpioInterruptAlreadyUnmasked
#define ERROR_CANNOT_SWITCH_RUNLEVEL kNtErrorCannotSwitchRunlevel
#define ERROR_INVALID_RUNLEVEL_SETTING kNtErrorInvalidRunlevelSetting
#define ERROR_RUNLEVEL_SWITCH_TIMEOUT kNtErrorRunlevelSwitchTimeout
#define ERROR_RUNLEVEL_SWITCH_AGENT_TIMEOUT kNtErrorRunlevelSwitchAgentTimeout
#define ERROR_RUNLEVEL_SWITCH_IN_PROGRESS kNtErrorRunlevelSwitchInProgress
#define ERROR_SERVICES_FAILED_AUTOSTART kNtErrorServicesFailedAutostart
#define ERROR_COM_TASK_STOP_PENDING kNtErrorComTaskStopPending
#define ERROR_INSTALL_OPEN_PACKAGE_FAILED kNtErrorInstallOpenPackageFailed
#define ERROR_INSTALL_PACKAGE_NOT_FOUND kNtErrorInstallPackageNotFound
#define ERROR_INSTALL_INVALID_PACKAGE kNtErrorInstallInvalidPackage
#define ERROR_INSTALL_RESOLVE_DEPENDENCY_FAILED kNtErrorInstallResolveDependencyFailed
#define ERROR_INSTALL_OUT_OF_DISK_SPACE kNtErrorInstallOutOfDiskSpace
#define ERROR_INSTALL_NETWORK_FAILURE kNtErrorInstallNetworkFailure
#define ERROR_INSTALL_REGISTRATION_FAILURE kNtErrorInstallRegistrationFailure
#define ERROR_INSTALL_DEREGISTRATION_FAILURE kNtErrorInstallDeregistrationFailure
#define ERROR_INSTALL_CANCEL kNtErrorInstallCancel
#define ERROR_INSTALL_FAILED kNtErrorInstallFailed
#define ERROR_REMOVE_FAILED kNtErrorRemoveFailed
#define ERROR_PACKAGE_ALREADY_EXISTS kNtErrorPackageAlreadyExists
#define ERROR_NEEDS_REMEDIATION kNtErrorNeedsRemediation
#define ERROR_INSTALL_PREREQUISITE_FAILED kNtErrorInstallPrerequisiteFailed
#define ERROR_PACKAGE_REPOSITORY_CORRUPTED kNtErrorPackageRepositoryCorrupted
#define ERROR_INSTALL_POLICY_FAILURE kNtErrorInstallPolicyFailure
#define ERROR_PACKAGE_UPDATING kNtErrorPackageUpdating
#define ERROR_DEPLOYMENT_BLOCKED_BY_POLICY kNtErrorDeploymentBlockedByPolicy
#define ERROR_PACKAGES_IN_USE kNtErrorPackagesInUse
#define ERROR_RECOVERY_FILE_CORRUPT kNtErrorRecoveryFileCorrupt
#define ERROR_INVALID_STAGED_SIGNATURE kNtErrorInvalidStagedSignature
#define ERROR_DELETING_EXISTING_APPLICATIONDATA_STORE_FAILED kNtErrorDeletingExistingApplicationdataStoreFailed
#define ERROR_INSTALL_PACKAGE_DOWNGRADE kNtErrorInstallPackageDowngrade
#define ERROR_SYSTEM_NEEDS_REMEDIATION kNtErrorSystemNeedsRemediation
#define ERROR_APPX_INTEGRITY_FAILURE_CLR_NGEN kNtErrorAppxIntegrityFailureClrNgen
#define ERROR_RESILIENCY_FILE_CORRUPT kNtErrorResiliencyFileCorrupt
#define ERROR_INSTALL_FIREWALL_SERVICE_NOT_RUNNING kNtErrorInstallFirewallServiceNotRunning
#define ERROR_PACKAGE_MOVE_FAILED kNtErrorPackageMoveFailed
#define ERROR_INSTALL_VOLUME_NOT_EMPTY kNtErrorInstallVolumeNotEmpty
#define ERROR_INSTALL_VOLUME_OFFLINE kNtErrorInstallVolumeOffline
#define ERROR_INSTALL_VOLUME_CORRUPT kNtErrorInstallVolumeCorrupt
#define ERROR_NEEDS_REGISTRATION kNtErrorNeedsRegistration
#define ERROR_INSTALL_WRONG_PROCESSOR_ARCHITECTURE kNtErrorInstallWrongProcessorArchitecture
#define ERROR_DEV_SIDELOAD_LIMIT_EXCEEDED kNtErrorDevSideloadLimitExceeded
#define ERROR_INSTALL_OPTIONAL_PACKAGE_REQUIRES_MAIN_PACKAGE kNtErrorInstallOptionalPackageRequiresMainPackage
#define ERROR_PACKAGE_NOT_SUPPORTED_ON_FILESYSTEM kNtErrorPackageNotSupportedOnFilesystem
#define ERROR_PACKAGE_MOVE_BLOCKED_BY_STREAMING kNtErrorPackageMoveBlockedByStreaming
#define ERROR_INSTALL_OPTIONAL_PACKAGE_APPLICATIONID_NOT_UNIQUE kNtErrorInstallOptionalPackageApplicationidNotUnique
#define ERROR_PACKAGE_STAGING_ONHOLD kNtErrorPackageStagingOnhold
#define ERROR_INSTALL_INVALID_RELATED_SET_UPDATE kNtErrorInstallInvalidRelatedSetUpdate
#define ERROR_PACKAGES_REPUTATION_CHECK_FAILED kNtErrorPackagesReputationCheckFailed
#define ERROR_PACKAGES_REPUTATION_CHECK_TIMEDOUT kNtErrorPackagesReputationCheckTimedout
#define ERROR_STATE_LOAD_STORE_FAILED kNtErrorStateLoadStoreFailed
#define ERROR_STATE_GET_VERSION_FAILED kNtErrorStateGetVersionFailed
#define ERROR_STATE_SET_VERSION_FAILED kNtErrorStateSetVersionFailed
#define ERROR_STATE_STRUCTURED_RESET_FAILED kNtErrorStateStructuredResetFailed
#define ERROR_STATE_OPEN_CONTAINER_FAILED kNtErrorStateOpenContainerFailed
#define ERROR_STATE_CREATE_CONTAINER_FAILED kNtErrorStateCreateContainerFailed
#define ERROR_STATE_DELETE_CONTAINER_FAILED kNtErrorStateDeleteContainerFailed
#define ERROR_STATE_READ_SETTING_FAILED kNtErrorStateReadSettingFailed
#define ERROR_STATE_WRITE_SETTING_FAILED kNtErrorStateWriteSettingFailed
#define ERROR_STATE_DELETE_SETTING_FAILED kNtErrorStateDeleteSettingFailed
#define ERROR_STATE_QUERY_SETTING_FAILED kNtErrorStateQuerySettingFailed
#define ERROR_STATE_READ_COMPOSITE_SETTING_FAILED kNtErrorStateReadCompositeSettingFailed
#define ERROR_STATE_WRITE_COMPOSITE_SETTING_FAILED kNtErrorStateWriteCompositeSettingFailed
#define ERROR_STATE_ENUMERATE_CONTAINER_FAILED kNtErrorStateEnumerateContainerFailed
#define ERROR_STATE_ENUMERATE_SETTINGS_FAILED kNtErrorStateEnumerateSettingsFailed
#define ERROR_STATE_COMPOSITE_SETTING_VALUE_SIZE_LIMIT_EXCEEDED kNtErrorStateCompositeSettingValueSizeLimitExceeded
#define ERROR_STATE_SETTING_VALUE_SIZE_LIMIT_EXCEEDED kNtErrorStateSettingValueSizeLimitExceeded
#define ERROR_STATE_SETTING_NAME_SIZE_LIMIT_EXCEEDED kNtErrorStateSettingNameSizeLimitExceeded
#define ERROR_STATE_CONTAINER_NAME_SIZE_LIMIT_EXCEEDED kNtErrorStateContainerNameSizeLimitExceeded
#define ERROR_API_UNAVAILABLE kNtErrorApiUnavailable

#define NOERROR 0
#define S_OK ((HRESULT)0)
#define S_FALSE ((HRESULT)1)
#define E_UNEXPECTED ((HRESULT)0x8000FFFF)
#define E_NOTIMPL ((HRESULT)0x80004001)
#define E_OUTOFMEMORY ((HRESULT)0x8007000E)
#define E_INVALIDARG ((HRESULT)0x80070057)
#define E_NOINTERFACE ((HRESULT)0x80004002)
#define E_POINTER ((HRESULT)0x80004003)
#define E_HANDLE ((HRESULT)0x80070006)
#define E_ABORT ((HRESULT)0x80004004)
#define E_FAIL ((HRESULT)0x80004005)
#define E_ACCESSDENIED ((HRESULT)0x80070005)
#define E_NOTIMPL ((HRESULT)0x80000001)
#define E_OUTOFMEMORY ((HRESULT)0x80000002)
#define E_INVALIDARG ((HRESULT)0x80000003)
#define E_NOINTERFACE ((HRESULT)0x80000004)
#define E_POINTER ((HRESULT)0x80000005)
#define E_HANDLE ((HRESULT)0x80000006)
#define E_ABORT ((HRESULT)0x80000007)
#define E_FAIL ((HRESULT)0x80000008)
#define E_ACCESSDENIED ((HRESULT)0x80000009)
#define E_PENDING ((HRESULT)0x8000000A)
#define E_BOUNDS ((HRESULT)0x8000000B)
#define E_CHANGED_STATE ((HRESULT)0x8000000C)
#define E_ILLEGAL_STATE_CHANGE ((HRESULT)0x8000000D)
#define E_ILLEGAL_METHOD_CALL ((HRESULT)0x8000000E)

#define FACILITY_NULL 0
#define FACILITY_RPC 1
#define FACILITY_DISPATCH 2
#define FACILITY_STORAGE 3
#define FACILITY_ITF 4
#define FACILITY_WIN32 7
#define FACILITY_WINDOWS 8
#define FACILITY_SSPI 9
#define FACILITY_SECURITY 9
#define FACILITY_CONTROL 10
#define FACILITY_CERT 11
#define FACILITY_INTERNET 12
#define FACILITY_MEDIASERVER 13
#define FACILITY_MSMQ 14
#define FACILITY_SETUPAPI 15
#define FACILITY_SCARD 16
#define FACILITY_COMPLUS 17
#define FACILITY_AAF 18
#define FACILITY_URT 19
#define FACILITY_ACS 20
#define FACILITY_DPLAY 21
#define FACILITY_UMI 22
#define FACILITY_SXS 23
#define FACILITY_WINDOWS_CE 24
#define FACILITY_HTTP 25
#define FACILITY_USERMODE_COMMONLOG 26
#define FACILITY_WER 27
#define FACILITY_USERMODE_FILTER_MANAGER 31
#define FACILITY_BACKGROUNDCOPY 32
#define FACILITY_CONFIGURATION 33
#define FACILITY_WIA 33
#define FACILITY_STATE_MANAGEMENT 34
#define FACILITY_METADIRECTORY 35
#define FACILITY_WINDOWSUPDATE 36
#define FACILITY_DIRECTORYSERVICE 37
#define FACILITY_GRAPHICS 38
#define FACILITY_SHELL 39
#define FACILITY_NAP 39
#define FACILITY_TPM_SERVICES 40
#define FACILITY_TPM_SOFTWARE 41
#define FACILITY_UI 42
#define FACILITY_XAML 43
#define FACILITY_ACTION_QUEUE 44
#define FACILITY_PLA 48
#define FACILITY_WINDOWS_SETUP 48
#define FACILITY_FVE 49
#define FACILITY_FWP 50
#define FACILITY_WINRM 51
#define FACILITY_NDIS 52
#define FACILITY_USERMODE_HYPERVISOR 53
#define FACILITY_CMI 54
#define FACILITY_USERMODE_VIRTUALIZATION 55
#define FACILITY_USERMODE_VOLMGR 56
#define FACILITY_BCD 57
#define FACILITY_USERMODE_VHD 58
#define FACILITY_USERMODE_HNS 59
#define FACILITY_SDIAG 60
#define FACILITY_WEBSERVICES 61
#define FACILITY_WINPE 61
#define FACILITY_WPN 62
#define FACILITY_WINDOWS_STORE 63
#define FACILITY_INPUT 64
#define FACILITY_EAP 66
#define FACILITY_WINDOWS_DEFENDER 80
#define FACILITY_OPC 81
#define FACILITY_XPS 82
#define FACILITY_MBN 84
#define FACILITY_POWERSHELL 84
#define FACILITY_RAS 83
#define FACILITY_P2P_INT 98
#define FACILITY_P2P 99
#define FACILITY_DAF 100
#define FACILITY_BLUETOOTH_ATT 101
#define FACILITY_AUDIO 102
#define FACILITY_STATEREPOSITORY 103
#define FACILITY_VISUALCPP 109
#define FACILITY_SCRIPT 112
#define FACILITY_PARSE 113
#define FACILITY_BLB 120
#define FACILITY_BLB_CLI 121
#define FACILITY_WSBAPP 122
#define FACILITY_BLBUI 128
#define FACILITY_USN 129
#define FACILITY_USERMODE_VOLSNAP 130
#define FACILITY_TIERING 131
#define FACILITY_WSB_ONLINE 133
#define FACILITY_ONLINE_ID 134
#define FACILITY_DEVICE_UPDATE_AGENT 135
#define FACILITY_DRVSERVICING 136
#define FACILITY_DLS 153
#define FACILITY_DELIVERY_OPTIMIZATION 208
#define FACILITY_USERMODE_SPACES 231
#define FACILITY_USER_MODE_SECURITY_CORE 232
#define FACILITY_USERMODE_LICENSING 234
#define FACILITY_SOS 160
#define FACILITY_DEBUGGERS 176
#define FACILITY_SPP 256
#define FACILITY_RESTORE 256
#define FACILITY_DMSERVER 256
#define FACILITY_DEPLOYMENT_SERVICES_SERVER 257
#define FACILITY_DEPLOYMENT_SERVICES_IMAGING 258
#define FACILITY_DEPLOYMENT_SERVICES_MANAGEMENT 259
#define FACILITY_DEPLOYMENT_SERVICES_UTIL 260
#define FACILITY_DEPLOYMENT_SERVICES_BINLSVC 261
#define FACILITY_DEPLOYMENT_SERVICES_PXE 263
#define FACILITY_DEPLOYMENT_SERVICES_TFTP 264
#define FACILITY_DEPLOYMENT_SERVICES_TRANSPORT_MANAGEMENT 272
#define FACILITY_DEPLOYMENT_SERVICES_DRIVER_PROVISIONING 278
#define FACILITY_DEPLOYMENT_SERVICES_MULTICAST_SERVER 289
#define FACILITY_DEPLOYMENT_SERVICES_MULTICAST_CLIENT 290
#define FACILITY_DEPLOYMENT_SERVICES_CONTENT_PROVIDER 293
#define FACILITY_LINGUISTIC_SERVICES 305
#define FACILITY_AUDIOSTREAMING 1094
#define FACILITY_ACCELERATOR 1536
#define FACILITY_WMAAECMA 1996
#define FACILITY_DIRECTMUSIC 2168
#define FACILITY_DIRECT3D10 2169
#define FACILITY_DXGI 2170
#define FACILITY_DXGI_DDI 2171
#define FACILITY_DIRECT3D11 2172
#define FACILITY_DIRECT3D11_DEBUG 2173
#define FACILITY_DIRECT3D12 2174
#define FACILITY_DIRECT3D12_DEBUG 2175
#define FACILITY_LEAP 2184
#define FACILITY_AUDCLNT 2185
#define FACILITY_WINCODEC_DWRITE_DWM 2200
#define FACILITY_WINML 2192
#define FACILITY_DIRECT2D 2201
#define FACILITY_DEFRAG 2304
#define FACILITY_USERMODE_SDBUS 2305
#define FACILITY_JSCRIPT 2306
#define FACILITY_PIDGENX 2561
#define FACILITY_EAS 85
#define FACILITY_WEB 885
#define FACILITY_WEB_SOCKET 886
#define FACILITY_MOBILE 1793
#define FACILITY_SQLITE 1967
#define FACILITY_UTC 1989
#define FACILITY_WEP 2049
#define FACILITY_SYNCENGINE 2050
#define FACILITY_XBOX 2339
#define FACILITY_GAME 2340
#define FACILITY_PIX 2748

#define SEVERITY_SUCCESS 0
#define SEVERITY_ERROR 1

#define SUCCEEDED(hr) (((HRESULT)(hr)) >= 0)
#define FAILED(hr) (((HRESULT)(hr)) < 0)
#define IS_ERROR(Status) (((unsigned long)(Status)) >> 31 == SEVERITY_ERROR)
#define HRESULT_CODE(hr) ((hr) & 0xFFFF)
#define SCODE_CODE(sc) ((sc) & 0xFFFF)
#define HRESULT_FACILITY(hr) (((hr) >> 16) & 0x1fff)
#define SCODE_FACILITY(sc) (((sc) >> 16) & 0x1fff)
#define HRESULT_SEVERITY(hr) (((hr) >> 31) & 0x1)
#define SCODE_SEVERITY(sc) (((sc) >> 31) & 0x1)
#define MAKE_HRESULT(sev,fac,code) ((HRESULT) (((unsigned long)(sev)<<31) | ((unsigned long)(fac)<<16) | ((unsigned long)(code))) )
#define MAKE_SCODE(sev,fac,code) ((SCODE) (((unsigned long)(sev)<<31) | ((unsigned long)(fac)<<16) | ((unsigned long)(code))) )

#define CP_ACP        0
#define CP_OEMCP      1
#define CP_MACCP      2
#define CP_THREAD_ACP 3
#define CP_SYMBOL     42

#define CP_UTF7 65000
#define CP_UTF8 65001

#endif /* COSMOPOLITAN_LIBC_COMPAT_INCLUDE_WINDOWS_H_ */
