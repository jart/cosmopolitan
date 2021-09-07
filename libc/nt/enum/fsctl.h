#ifndef COSMOPOLITAN_LIBC_NT_ENUM_FSCTL_H_
#define COSMOPOLITAN_LIBC_NT_ENUM_FSCTL_H_

#define kNtFsctlDisableLocalBuffering   0x000902B8u
#define kNtFsctlFilePrefetch            0x00090120u
#define kNtFsctlFilesystemGetStatistics 0x00090060u
#define kNtFsctlGetCompression          0x0009003Cu
#define kNtFsctlGetNtfsFileRecord       0x00090068u
#define kNtFsctlGetNtfsVolumeData       0x00090064u
#define kNtFsctlQueryAllocatedRanges    0x000940CFu
#define kNtFsctlScrubData               0x000902B0u
#define kNtFsctlSetCompression          0x0009C040u
#define kNtFsctlSetSparse               0x000900C4u
#define kNtFsctlSetZeroData             0x000980C8u
#define kNtFsctlGetReparsePoint         0x000900a8u
#define kNtFsctlSetReparsePoint         0x000900a4u

#endif /* COSMOPOLITAN_LIBC_NT_ENUM_FSCTL_H_ */
