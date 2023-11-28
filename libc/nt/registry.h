#ifndef COSMOPOLITAN_LIBC_NT_REGISTRY_H_
#define COSMOPOLITAN_LIBC_NT_REGISTRY_H_
#include "libc/nt/enum/securityinformation.h"
#include "libc/nt/struct/filetime.h"
#include "libc/nt/struct/securityattributes.h"
#include "libc/nt/struct/valent.h"
/*                            ░░░░
                       ▒▒▒░░░▒▒▒▒▒▒▒▓▓▓░
                      ▒▒▒▒░░░▒▒▒▒▒▒▓▓▓▓▓▓░
                     ▒▒▒▒░░░▒▒▒▒▒▒▒▓▓▓▓▓▓  ▒▓░
                     ▒▒▒░░░░▒▒▒▒▒▒▓▓▓▓▓▓   ▓▓▓▓▓▓▒        ▒▒▒▓▓█
                    ▒▒▒▒░░░▒▒▒▒▒▒▒▓▓▓▓▓▓  ▓▓▓▓▓▓▓▓▒▒▒▒▒▒▒▒▒▒▓▓▓
                   ░▒▒▒░░░░▒▒▒▒▒▒▓▓▓▓▓▓   █▓▓▓▓▓▓▓▒▒▒▒▒▒▒▒▒▒▓▓█
                   ▒▒▒▒░░░▒▒▒▒▒▒▒▓▓▓▓▓░  ▓▓▓▓▓▓▓▓▒▒▒▒▒▒▒▒▒▒▓▓▓
                  ▒▒▒▒░░░▒▒▒▒▒▒▒▓▓▓▓▓▓  ▒▓▓▓▓▓▓▓▓▒▒▒▒▒▒▒▒▒▒▓▓▒
                  ▒▒▒▒▓▓      ▓▒▒▓▓▓▓   ▓▓▓▓▓▓▓▓▒▒▒▒▒▒▒▒▒▒▓▓█
                                   ▒▓  ▓▓▓▓▓▓▓▓▓▒▒▒▒▒▒▒▒▒▒▓▓
                  ░░░░░░░░░░░▒▒▒▒      ▓▓▓▓▓▓▓▓▒▒▒▒▒▒▒▒▒▒▓▓█
                ▒▒░░░░░░░░░░▒▒▒▒▒▓▓▓     ▓▓▓▓▓▒▒▒▒▒▒▒▒▒▒▓▓▓
               ░▒░░░░░░░░░░░▒▒▒▒▒▓▓   ▓░      ░▓███▓
               ▒▒░░░░░░░░░░▒▒▒▒▒▓▓░  ▒▓▓▓▒▒▒         ░▒▒▒▓   ████████████
              ▒▒░░░░░░░░░░░▒▒▒▒▒▓▓  ▒▓▓▓▓▒▒▒▒▒▒▒▒░░░▒▒▒▒▒░           ░███
              ▒░░░░░░░░░░░▒▒▒▒▒▓▓   ▓▓▓▓▒▒▒▒▒▒▒▒░░░░▒▒▒▒▓            ███
             ▒▒░░░░░░░░░░▒▒▒▒▒▒▓▓  ▒▓▓▓▒▒▒▒▒▒▒▒░░░░▒▒▒▒▒            ▓██
             ▒░░░░░░░░░░░▒▒▒▒▒▓▓   ▓▓▓▓▒▒▒▒▒▒▒▒░░░▒▒▒▒▒▓           ▓██
            ▒▒░░░▒▒▒░░░▒▒░▒▒▒▓▓▒  ▒▓▓▓▒▒▒▒▒▒▒▒░░░░▒▒▒▒▒           ███
                            ░▒▓  ░▓▓▓▓▒▒▒▒▒▒▒▒░░░░▒▒▒▒▓          ▓██
╔────────────────────────────────────────────────────────────────▀▀▀─────────│─╗
│ cosmopolitan § new technology » registry                                 ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

#define kNtMaxKeyNameChars   255
#define kNtMaxValueNameChars 16383
#define kNtMaxValueBytes     0x100000

#define kNtHkeyClassesRoot              0x80000000l
#define kNtHkeyCurrentUser              0x80000001l
#define kNtHkeyLocalMachine             0x80000002l
#define kNtHkeyUsers                    0x80000003l
#define kNtHkeyPerformanceData          0x80000004l
#define kNtHkeyPerformanceText          0x80000050l
#define kNtHkeyPerformanceNlstext       0x80000060l
#define kNtHkeyCurrentConfig            0x80000005l
#define kNtHkeyDynData                  0x80000006l
#define kNtHkeyCurrentUserLocalSettings 0x80000007l

COSMOPOLITAN_C_START_

int RegOpenKey(int64_t hKey, const char16_t *opt_lpSubKey,
               int64_t *out_phkResult) paramsnonnull((3));
int RegOpenKeyEx(int64_t hKey, const char16_t *opt_lpSubKey,
                 uint32_t opt_ulOptions, int samDesired, int64_t *out_phkResult)
    paramsnonnull((5));
int RegCloseKey(int64_t hKey);

int RegGetValue(int64_t hkey, const char16_t *opt_lpSubKey,
                const char16_t *opt_lpValue, unsigned dwFlags, int *opt_pdwType,
                void *opt_out_pvData, uint32_t *opt_inout_pcbDataBytes);
int RegSetValue(int64_t hKey, const char16_t *lpSubKey, int dwType,
                const char16_t *lpData, uint32_t cbData);
int RegSetValueEx(int64_t hKey, const char16_t *lpValueName, uint32_t Reserved,
                  int dwType, const unsigned char *lpData, uint32_t cbData);

int RegQueryInfoKey(int64_t hKey, char16_t *opt_out_lpClass,
                    uint32_t *opt_inout_lpClassLen, uint32_t *lpReserved,
                    uint32_t *opt_out_lpcSubKeys,
                    uint32_t *opt_out_lpcbMaxSubKeyBytes,
                    uint32_t *opt_out_lpcbMaxClassBytes,
                    uint32_t *opt_out_lpcValues,
                    uint32_t *opt_out_lpcbMaxValueNameBytes,
                    uint32_t *opt_out_lpcbMaxValueBytes,
                    uint32_t *opt_out_lpcbSecurityDescriptorBytes,
                    struct NtFileTime *opt_out_lpftLastWriteTime);
int RegEnumKey(int64_t hKey, uint32_t dwIndex, char16_t *opt_lpName,
               uint32_t NameLen);
int RegEnumKeyEx(int64_t hKey, uint32_t dwIndex, char16_t *out_lpName,
                 uint32_t *inout_lpcchName, uint32_t *lpReserved,
                 char16_t *opt_out_lpClass, uint32_t *opt_inout_lpcchClassLen,
                 struct NtFileTime *opt_out_lpftLastWriteTime);

int RegEnumValue(int64_t hKey, uint32_t dwIndex, char16_t *lpValueName,
                 uint32_t *lpValueNameLen, uint32_t *lpReserved,
                 int *opt_out_lpType, unsigned char *opt_out_lpData,
                 uint32_t *opt_inout_lpcbDataBytes);
int RegQueryValue(int64_t hKey, const char16_t *opt_lpSubKey,
                  char16_t *opt_out_lpData, int32_t *opt_inout_lpcbDataBytes);
int RegQueryValueEx(int64_t hKey, const char16_t *opt_lpValueName,
                    uint32_t *lpReserved, int *opt_out_lpType,
                    unsigned char *opt_out_lpData,
                    uint32_t *opt_inout_lpcbDataBytes);

int RegOverridePredefKey(int64_t hKey, int64_t hNewHKey);
int RegOpenUserClassesRoot(void *hToken, uint32_t dwOptions, int samDesired,
                           int64_t *phkResult);
int RegOpenCurrentUser(int samDesired, int64_t *phkResult);
int RegDisablePredefinedCache();
int RegConnectRegistry(const char16_t *lpMachineName, int64_t hKey,
                       int64_t *phkResult);
int RegConnectRegistryEx(const char16_t *lpMachineName, int64_t hKey,
                         uint32_t Flags, int64_t *phkResult);
int RegCreateKey(int64_t hKey, const char16_t *lpSubKey, int64_t *phkResult);
int RegCreateKeyEx(int64_t hKey, const char16_t *lpSubKey, uint32_t Reserved,
                   int16_t *lpClass, uint32_t dwOptions, int samDesired,
                   const struct NtSecurityAttributes *lpSecurityAttributes,
                   int64_t *phkResult, uint32_t *lpdwDisposition);
int RegDeleteKey(int64_t hKey, const char16_t *lpSubKey);
int RegDeleteKeyEx(int64_t hKey, const char16_t *lpSubKey, int samDesired,
                   uint32_t Reserved);
int RegDeleteTree(int64_t hKey, const char16_t *opt_lpSubKey);
int RegDisableReflectionKey(int64_t hBase);
int RegEnableReflectionKey(int64_t hBase);
int RegQueryReflectionKey(int64_t hBase, bool32 *bIsReflectionDisabled);
int RegDeleteValue(int64_t hKey, const char16_t *lpValueName);
int RegFlushKey(int64_t hKey);
int RegGetKeySecurity(int64_t hKey, uint32_t SecurityInformation,
                      void *pSecurityDescriptor,
                      uint32_t *lpcbSecurityDescriptorBytes);
int RegLoadKey(int64_t hKey, const char16_t *lpSubKey, const char16_t *lpFile);
int RegNotifyChangeKeyValue(int64_t hKey, bool32 bWatchSubtree,
                            uint32_t dwNotifyFilter, void *hEvent,
                            int fAsynchronous);
int RegQueryMultipleValues(int64_t hKey, struct NtValent *inout_val_list,
                           uint32_t num_vals, int16_t *out_lpValueBuf,
                           uint32_t *inout_ldwTotsize) paramsnonnull();
int RegReplaceKey(int64_t hKey, const char16_t *lpSubKey,
                  const char16_t *lpNewFile, const char16_t *lpOldFile);
int RegRestoreKey(int64_t hKey, const char16_t *lpFile, uint32_t dwFlags);
int RegSaveKey(int64_t hKey, const char16_t *lpFile,
               const struct NtSecurityAttributes *lpSecurityAttributes);
int RegSetKeySecurity(int64_t hKey, uint32_t SecurityInformation,
                      void *pSecurityDescriptor);
int RegUnLoadKey(int64_t hKey, const char16_t *lpSubKey);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_NT_REGISTRY_H_ */
