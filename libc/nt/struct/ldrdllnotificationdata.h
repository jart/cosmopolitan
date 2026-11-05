#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_LDRDLLNOTIFICATIONDATA_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_LDRDLLNOTIFICATIONDATA_H_
#include "libc/nt/struct/unicodestring.h"

#define kNtLdrDllNotificationReasonLoaded   0x00000001
#define kNtLdrDllNotificationReasonUnloaded 0x00000002

struct NtLdrDllLoadedNotificationData {
  uint32_t Flags;
  const struct NtUnicodeString* FullDllName;
  const struct NtUnicodeString* BaseDllName;
  void* DllBase;
  uint32_t SizeOfImage;
};
struct NtLdrDllUnloadedNotificationData {
  uint32_t Flags;
  const struct NtUnicodeString* FullDllName;
  const struct NtUnicodeString* BaseDllName;
  void* DllBase;
  uint32_t SizeOfImage;
};

union NtLdrDllNotificationData {
  struct NtLdrDllLoadedNotificationData Loaded;
  struct NtLdrDllUnloadedNotificationData Unloaded;
};

#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_LDRDLLNOTIFICATIONDATA_H_ */
