#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_FILEALLINFORMATION_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_FILEALLINFORMATION_H_
#include "libc/nt/struct/fileaccessinformation.h"
#include "libc/nt/struct/filealignmentinformation.h"
#include "libc/nt/struct/filebasicinformation.h"
#include "libc/nt/struct/fileeainformation.h"
#include "libc/nt/struct/fileinternalinformation.h"
#include "libc/nt/struct/filemodeinformation.h"
#include "libc/nt/struct/filenameinformation.h"
#include "libc/nt/struct/filepositioninformation.h"
#include "libc/nt/struct/filestandardinformation.h"

struct NtFileAllInformation {
  struct NtFileBasicInformation BasicInformation;
  struct NtFileStandardInformation StandardInformation;
  struct NtFileInternalInformation InternalInformation;
  struct NtFileEaInformation EaInformation;
  struct NtFileAccessInformation AccessInformation;
  struct NtFilePositionInformation PositionInformation;
  struct NtFileModeInformation ModeInformation;
  struct NtFileAlignmentInformation AlignmentInformation;
  struct NtFileNameInformation NameInformation;
};

#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_FILEALLINFORMATION_H_ */
