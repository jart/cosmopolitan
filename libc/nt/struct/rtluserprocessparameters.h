#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_RTLUSERPROCESSPARAMETERS_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_RTLUSERPROCESSPARAMETERS_H_
#include "libc/nt/struct/unicodestring.h"

struct NtRtlUserProcessParameters {
  uint32_t MaximumLength;
  uint32_t Length;
  uint32_t Flags;
  uint32_t DebugFlags;
  int64_t ConsoleHandle;
  uint32_t ConsoleFlags;
  int64_t StdInputHandle;
  int64_t StdOutputHandle;
  int64_t StdErrorHandle;
  struct NtUnicodeString *CurrentDirectoryPath;
  int64_t CurrentDirectoryHandle;
  struct NtUnicodeString *DllPath;
  struct NtUnicodeString *ImagePathName;
  struct NtUnicodeString *CommandLine;
  void *Environment;
  uint32_t StartingPositionLeft;
  uint32_t StartingPositionTop;
  uint32_t Width;
  uint32_t Height;
  uint32_t CharWidth;
  uint32_t CharHeight;
  uint32_t ConsoleTextAttributes;
  uint32_t WindowFlags;
  uint32_t ShowWindowFlags;
  struct NtUnicodeString *WindowTitle;
  struct NtUnicodeString *DesktopName;
  struct NtUnicodeString *ShellInfo;
  struct NtUnicodeString *RuntimeData;
};

#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_RTLUSERPROCESSPARAMETERS_H_ */
