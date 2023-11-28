#ifndef COSMOPOLITAN_LIBC_NT_EFI_H_
#define COSMOPOLITAN_LIBC_NT_EFI_H_
/*
    ▐██ ░█████████▓   ▐██▌     ██▓░   ▐█▌  ▐██  ░██░    ▓█▌  ▓██▒     ▓██
    ▐██     ░██░     ▒█▓██░    ████░  ▐█▌  ▐██  ░██░    ▓█▌  ▓███░   ▓███
    ▐██     ░██░     ██░▐█▓    ██▒▓█░ ▐█▌  ▐██  ░██░    ▓█▌  ▓█▌▓█░ ▓█▒██
    ▐██     ░██░    ▐█▌  ▓█▌   ██░░▓█░▐█▌  ▐██  ░██░    ▓█▌  ▓█▌░█▓▓█▒░██
    ▐██     ░██░   ▒██▓█████░  ██░ ░▓▓▓█▌  ▐██  ░██░    ██░  ▓█▌ ░██▌ ░██
    ▐██     ░██░   ▓█▌    ▓█▓  ██░  ░███▌  ▐██   ▐██▄▄▄▓█▓   ▓█▌      ░██
╔────────────────────────────────────────────────────────────────────────────│─╗
│ αcτµαlly pδrταblε εxεcµταblε § the unified extensible firmware interface ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

#define EFI_SUCCESS           0x8000000000000000
#define EFI_LOAD_ERROR        0x8000000000000001
#define EFI_INVALID_PARAMETER 0x8000000000000002
#define EFI_UNSUPPORTED       0x8000000000000003
#define EFI_BAD_BUFFER_SIZE   0x8000000000000004
#define EFI_BUFFER_TOO_SMALL  0x8000000000000005
#define EFI_NOT_READY         0x8000000000000006
#define EFI_DEVICE_ERROR      0x8000000000000007
#define EFI_WRITE_PROTECTED   0x8000000000000008
#define EFI_OUT_OF_RESOURCES  0x8000000000000009
#define EFI_VOLUME_CORRUPTED  0x800000000000000a
#define EFI_VOLUME_FULL       0x800000000000000b
#define EFI_NO_MEDIA          0x800000000000000c
#define EFI_MEDIA_CHANGED     0x800000000000000d
#define EFI_NOT_FOUND         0x800000000000000e
#define EFI_ACCESS_DENIED     0x800000000000000f
#define EFI_NO_RESPONSE       0x8000000000000010
#define EFI_NO_MAPPING        0x8000000000000011
#define EFI_TIMEOUT           0x8000000000000012
#define EFI_NOT_STARTED       0x8000000000000013
#define EFI_ALREADY_STARTED   0x8000000000000014
#define EFI_ABORTED           0x8000000000000015
#define EFI_ICMP_ERROR        0x8000000000000016
#define EFI_TFTP_ERROR        0x8000000000000017
#define EFI_PROTOCOL_ERROR    0x8000000000000018

#define EFI_VARIABLE_NON_VOLATILE                          0x00000001
#define EFI_VARIABLE_BOOTSERVICE_ACCESS                    0x00000002
#define EFI_VARIABLE_RUNTIME_ACCESS                        0x00000004
#define EFI_VARIABLE_HARDWARE_ERROR_RECORD                 0x00000008
#define EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS            0x00000010
#define EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS 0x00000020
#define EFI_VARIABLE_APPEND_WRITE                          0x00000040

#define EFI_MEMORY_UC            0x0000000000000001U
#define EFI_MEMORY_WC            0x0000000000000002U
#define EFI_MEMORY_WT            0x0000000000000004U
#define EFI_MEMORY_WB            0x0000000000000008U
#define EFI_MEMORY_UCE           0x0000000000000010U
#define EFI_MEMORY_WP            0x0000000000001000U
#define EFI_MEMORY_RP            0x0000000000002000U
#define EFI_MEMORY_XP            0x0000000000004000U
#define EFI_MEMORY_RO            0x0000000000020000U
#define EFI_MEMORY_NV            0x0000000000008000U
#define EFI_MEMORY_MORE_RELIABLE 0x0000000000010000U
#define EFI_MEMORY_RUNTIME       0x8000000000000000U

#define EFI_OPTIONAL_PTR 0x00000001

#define EFI_SCAN_NULL      0x0000
#define EFI_SCAN_UP        0x0001
#define EFI_SCAN_DOWN      0x0002
#define EFI_SCAN_RIGHT     0x0003
#define EFI_SCAN_LEFT      0x0004
#define EFI_SCAN_HOME      0x0005
#define EFI_SCAN_END       0x0006
#define EFI_SCAN_INSERT    0x0007
#define EFI_SCAN_DELETE    0x0008
#define EFI_SCAN_PAGE_UP   0x0009
#define EFI_SCAN_PAGE_DOWN 0x000A
#define EFI_SCAN_F1        0x000B
#define EFI_SCAN_F2        0x000C
#define EFI_SCAN_F3        0x000D
#define EFI_SCAN_F4        0x000E
#define EFI_SCAN_F5        0x000F
#define EFI_SCAN_F6        0x0010
#define EFI_SCAN_F7        0x0011
#define EFI_SCAN_F8        0x0012
#define EFI_SCAN_F9        0x0013
#define EFI_SCAN_F10       0x0014
#define EFI_SCAN_ESC       0x0017

#define EFI_EVT_TIMER                         0x80000000
#define EFI_EVT_RUNTIME                       0x40000000
#define EFI_EVT_NOTIFY_WAIT                   0x00000100
#define EFI_EVT_NOTIFY_SIGNAL                 0x00000200
#define EFI_EVT_SIGNAL_EXIT_BOOT_SERVICES     0x00000201
#define EFI_EVT_SIGNAL_VIRTUAL_ADDRESS_CHANGE 0x60000202
#define EFI_EVT_RUNTIME_CONTEXT               0x20000000

#define LOADED_IMAGE_PROTOCOL                        \
  {                                                  \
    0x5B1B31A1, 0x9562, 0x11d2, {                    \
      0x8E, 0x3F, 0x00, 0xA0, 0xC9, 0x69, 0x72, 0x3B \
    }                                                \
  }
#define GRAPHICS_OUTPUT_PROTOCOL                     \
  {                                                  \
    0x9042A9DE, 0x23DC, 0x4A38, {                    \
      0x96, 0xFB, 0x7A, 0xDE, 0xD0, 0x80, 0x51, 0x6A \
    }                                                \
  }
#define ACPI_20_TABLE_GUID                           \
  {                                                  \
    0x8868E871, 0xE4F1, 0x11D3, {                    \
      0xBC, 0x22, 0x00, 0x80, 0xC7, 0x3C, 0x88, 0x81 \
    }                                                \
  }
#define ACPI_10_TABLE_GUID                           \
  {                                                  \
    0xEB9D2D30, 0x2D88, 0x11D3, {                    \
      0x9A, 0x16, 0x00, 0x90, 0x27, 0x3F, 0xC1, 0x4D \
    }                                                \
  }

COSMOPOLITAN_C_START_

#if defined(__GNUC__) && __GNUC__ >= 6 && !defined(__chibicc__) && \
    defined(__x86_64__)
#define EFIAPI __attribute__((__ms_abi__))
#else
#define EFIAPI /* TODO(jart): fix me */
#endif

#define EFI_STATUS uint64_t
#define EFI_EVENT  uintptr_t
#define EFI_HANDLE uintptr_t

typedef struct _EFI_SIMPLE_TEXT_INPUT_PROTOCOL EFI_SIMPLE_TEXT_INPUT_PROTOCOL;
typedef struct _EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL;
typedef struct _EFI_GRAPHICS_OUTPUT_PROTOCOL EFI_GRAPHICS_OUTPUT_PROTOCOL;

typedef enum {
  EfiReservedMemoryType,
  EfiLoaderCode,
  EfiLoaderData,
  EfiBootServicesCode,
  EfiBootServicesData,
  EfiRuntimeServicesCode,
  EfiRuntimeServicesData,
  EfiConventionalMemory,
  EfiUnusableMemory,
  EfiACPIReclaimMemory,
  EfiACPIMemoryNVS,
  EfiMemoryMappedIO,
  EfiMemoryMappedIOPortSpace,
  EfiPalCode,
  EfiPersistentMemory,
  EfiMaxMemoryType
} EFI_MEMORY_TYPE;

typedef enum {
  EfiResetCold,
  EfiResetWarm,
  EfiResetShutdown,
  EfiResetPlatformSpecific
} EFI_RESET_TYPE;

typedef enum {
  AllocateAnyPages,
  AllocateMaxAddress,
  AllocateAddress,
  MaxAllocateType
} EFI_ALLOCATE_TYPE;

typedef enum {
  TimerCancel,
  TimerPeriodic,
  TimerRelative,
} EFI_TIMER_DELAY;

typedef struct {
  uint32_t Resolution;
  uint32_t Accuracy;
  bool32 SetsToZero;
} EFI_TIME_CAPABILITIES;

typedef struct {
  uint32_t Data1;
  uint16_t Data2;
  uint16_t Data3;
  uint8_t Data4[8];
} EFI_GUID;

typedef struct {
  uint16_t Year;
  uint8_t Month;
  uint8_t Day;
  uint8_t Hour;
  uint8_t Minute;
  uint8_t Second;
  uint8_t Pad1;
  uint32_t Nanosecond;
  int16_t TimeZone;
  uint8_t Daylight;
  uint8_t Pad2;
} EFI_TIME;

typedef struct {
  uint32_t Type;
  uint64_t PhysicalStart;
  uint64_t VirtualStart;
  uint64_t NumberOfPages;
  uint64_t Attribute;
} EFI_MEMORY_DESCRIPTOR;

typedef struct {
  EFI_GUID VendorGuid;
  void *VendorTable;
} EFI_CONFIGURATION_TABLE;

typedef struct {
  EFI_GUID CapsuleGuid;
  uint32_t HeaderSize;
  uint32_t Flags;
  uint32_t CapsuleImageSize;
} EFI_CAPSULE_HEADER;

typedef struct {
  uint16_t ScanCode;
  char16_t UnicodeChar;
} EFI_INPUT_KEY;

typedef struct {
  int32_t MaxMode;
  int32_t Mode;
  int32_t Attribute;
  int32_t CursorColumn;
  int32_t CursorRow;
  bool32 CursorVisible;
} EFI_SIMPLE_TEXT_OUTPUT_MODE;

typedef enum {
  PixelRedGreenBlueReserved8BitPerColor,
  PixelBlueGreenRedReserved8BitPerColor,
  PixelBitMask,
  PixelBltOnly,
  PixelFormatMax
} EFI_GRAPHICS_PIXEL_FORMAT;

typedef struct {
  uint32_t RedMask;
  uint32_t GreenMask;
  uint32_t BlueMask;
  uint32_t ReservedMask;
} EFI_PIXEL_BITMASK;

typedef struct {
  uint32_t Version;
  uint32_t HorizontalResolution;
  uint32_t VerticalResolution;
  EFI_GRAPHICS_PIXEL_FORMAT PixelFormat;
  EFI_PIXEL_BITMASK PixelInformation;
  uint32_t PixelsPerScanLine;
} EFI_GRAPHICS_OUTPUT_MODE_INFORMATION;

typedef struct {
  uint8_t Blue;
  uint8_t Green;
  uint8_t Red;
  uint8_t Reserved;
} EFI_GRAPHICS_OUTPUT_BLT_PIXEL;

typedef enum {
  EfiBltVideoFill,
  EfiBltVideoToBltBuffer,
  EfiBltBufferToVideo,
  EfiBltVideoToVideo,
  EfiGraphicsOutputBltOperationMax
} EFI_GRAPHICS_OUTPUT_BLT_OPERATION;

typedef struct {
  uint32_t MaxMode;
  uint32_t Mode;
  EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *Info;
  uint32_t SizeOfInfo;
  uint64_t FrameBufferBase;
  uint32_t FrameBufferSize;
} EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE;

typedef struct {
  uint64_t Signature;
  uint32_t Revision;
  uint32_t HeaderSize;
  uint32_t CRC32;
  uint32_t Reserved;
} EFI_TABLE_HEADER;

typedef struct {
  uint8_t Type;
  uint8_t SubType;
  uint8_t Length[2];
} EFI_DEVICE_PATH_PROTOCOL;

typedef EFI_STATUS(EFIAPI *EFI_EXIT)(EFI_HANDLE ImageHandle,
                                     EFI_STATUS ExitStatus,
                                     uintptr_t ExitDataSize,
                                     char16_t *opt_ExitData);

typedef EFI_STATUS(EFIAPI *EFI_GET_VARIABLE)(char16_t *VariableName,
                                             EFI_GUID *VendorGuid,
                                             uint32_t *outopt_Attributes,
                                             uintptr_t *inout_DataSize,
                                             void *outopt_Data);
typedef EFI_STATUS(EFIAPI *EFI_SET_VARIABLE)(char16_t *VariableName,
                                             EFI_GUID *VendorGuid,
                                             uint32_t Attributes,
                                             uintptr_t DataSize, void *Data);
typedef EFI_STATUS(EFIAPI *EFI_GET_NEXT_VARIABLE_NAME)(
    uintptr_t *inout_VariableNameSize, char16_t *inout_VariableName,
    EFI_GUID *inout_VendorGuid);
typedef EFI_STATUS(EFIAPI *EFI_QUERY_VARIABLE_INFO)(
    uint32_t Attributes, uint64_t *out_MaximumVariableStorageSize,
    uint64_t *out_RemainingVariableStorageSize,
    uint64_t *out_MaximumVariableSize);

typedef EFI_STATUS(EFIAPI *EFI_ALLOCATE_PAGES)(EFI_ALLOCATE_TYPE Type,
                                               EFI_MEMORY_TYPE MemoryType,
                                               uintptr_t Pages,
                                               uint64_t *inout_Memory);
typedef EFI_STATUS(EFIAPI *EFI_FREE_PAGES)(uint64_t Memory, uintptr_t Pages);
typedef EFI_STATUS(EFIAPI *EFI_GET_MEMORY_MAP)(
    uintptr_t *inout_MemoryMapSize, EFI_MEMORY_DESCRIPTOR *inout_MemoryMap,
    uintptr_t *out_MapKey, uintptr_t *out_DescriptorSize,
    uint32_t *out_DescriptorVersion);

typedef EFI_STATUS(EFIAPI *EFI_ALLOCATE_POOL)(EFI_MEMORY_TYPE PoolType,
                                              uintptr_t Size, void *out_Buffer);
typedef EFI_STATUS(EFIAPI *EFI_FREE_POOL)(void *Buffer);
typedef void(EFIAPI *EFI_SET_MEM)(void *Buffer, uintptr_t Size, uint8_t Value);
typedef void(EFIAPI *EFI_COPY_MEM)(void *Destination, void *Source,
                                   uintptr_t Length);

typedef EFI_STATUS(EFIAPI *EFI_CHECK_EVENT)(EFI_EVENT Event);
typedef EFI_STATUS(EFIAPI *EFI_CLOSE_EVENT)(EFI_EVENT Event);
typedef EFI_STATUS(EFIAPI *EFI_SIGNAL_EVENT)(EFI_EVENT Event);
typedef EFI_STATUS(EFIAPI *EFI_WAIT_FOR_EVENT)(uintptr_t NumberOfEvents,
                                               EFI_EVENT *Events,
                                               uintptr_t *out_Index);
typedef EFI_STATUS(EFIAPI *EFI_SET_TIMER)(EFI_EVENT Event, EFI_TIMER_DELAY Type,
                                          uint64_t TriggerTime);
typedef void(EFIAPI *EFI_EVENT_NOTIFY)(EFI_EVENT Event, void *Context);
typedef EFI_STATUS(EFIAPI *EFI_CREATE_EVENT)(uint32_t Type, uintptr_t NotifyTpl,
                                             EFI_EVENT_NOTIFY NotifyFunction,
                                             void *NotifyContext,
                                             EFI_EVENT *out_Event);
typedef EFI_STATUS(EFIAPI *EFI_CREATE_EVENT_EX)(
    uint32_t Type, uintptr_t NotifyTpl, EFI_EVENT_NOTIFY opt_NotifyFunction,
    const void *opt_NotifyContext, const EFI_GUID *opt_EventGroup,
    EFI_EVENT *out_Event);

typedef EFI_STATUS(EFIAPI *EFI_UPDATE_CAPSULE)(
    EFI_CAPSULE_HEADER **CapsuleHeaderArray, uintptr_t CapsuleCount,
    uint64_t opt_ScatterGatherList);
typedef EFI_STATUS(EFIAPI *EFI_QUERY_CAPSULE_CAPABILITIES)(
    EFI_CAPSULE_HEADER **CapsuleHeaderArray, uintptr_t CapsuleCount,
    uint64_t *out_MaximumCapsuleSize, EFI_RESET_TYPE *out_ResetType);
typedef EFI_STATUS(EFIAPI *EFI_GET_WAKEUP_TIME)(bool32 *out_Enabled,
                                                bool32 *out_Pending,
                                                EFI_TIME *out_Time);
typedef EFI_STATUS(EFIAPI *EFI_SET_WAKEUP_TIME)(bool32 Enable,
                                                EFI_TIME *opt_Time);
typedef EFI_STATUS(EFIAPI *EFI_SET_WATCHDOG_TIMER)(uintptr_t Timeout,
                                                   uint64_t WatchdogCode,
                                                   uintptr_t DataSize,
                                                   char16_t *opt_WatchdogData);
typedef EFI_STATUS(EFIAPI *EFI_LOCATE_PROTOCOL)(EFI_GUID *Protocol,
                                                void *Registration,
                                                void *Interface);

typedef EFI_STATUS(EFIAPI *EFI_SET_TIME)(EFI_TIME *Time);
typedef EFI_STATUS(EFIAPI *EFI_GET_TIME)(
    EFI_TIME *out_Time, EFI_TIME_CAPABILITIES *outopt_Capabilities);
typedef EFI_STATUS(EFIAPI *EFI_GET_NEXT_HIGH_MONO_COUNT)(
    uint32_t *out_HighCount);
typedef EFI_STATUS(EFIAPI *EFI_STALL)(uintptr_t Microseconds);
typedef EFI_STATUS(EFIAPI *EFI_GET_NEXT_MONOTONIC_COUNT)(uint64_t *out_Count);

typedef EFI_STATUS(EFIAPI *EFI_SET_VIRTUAL_ADDRESS_MAP)(
    uintptr_t MemoryMapSize, uintptr_t DescriptorSize,
    uint32_t DescriptorVersion, EFI_MEMORY_DESCRIPTOR *VirtualMap);
typedef void(EFIAPI *EFI_RESET_SYSTEM)(EFI_RESET_TYPE ResetType,
                                       EFI_STATUS ResetStatus,
                                       uintptr_t DataSize, void *opt_ResetData);
typedef EFI_STATUS(EFIAPI *EFI_CONVERT_POINTER)(uintptr_t DebugDisposition,
                                                void **inout_Address);

typedef EFI_STATUS(EFIAPI *EFI_INPUT_RESET)(
    EFI_SIMPLE_TEXT_INPUT_PROTOCOL *This, bool32 ExtendedVerification);
typedef EFI_STATUS(EFIAPI *EFI_INPUT_READ_KEY)(
    EFI_SIMPLE_TEXT_INPUT_PROTOCOL *This, EFI_INPUT_KEY *out_Key);

typedef EFI_STATUS(EFIAPI *EFI_TEXT_RESET)(
    EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This, bool32 ExtendedVerification);
typedef EFI_STATUS(EFIAPI *EFI_TEXT_STRING)(
    EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This, char16_t *String);
typedef EFI_STATUS(EFIAPI *EFI_TEXT_TEST_STRING)(
    EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This, char16_t *String);
typedef EFI_STATUS(EFIAPI *EFI_TEXT_QUERY_MODE)(
    EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This, uint64_t ModeNumber,
    uint64_t *out_Columns, uint64_t *out_Rows);
typedef EFI_STATUS(EFIAPI *EFI_TEXT_SET_MODE)(
    EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This, uint64_t ModeNumber);
typedef EFI_STATUS(EFIAPI *EFI_TEXT_SET_ATTRIBUTE)(
    EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This, uint64_t Attribute);
typedef EFI_STATUS(EFIAPI *EFI_TEXT_CLEAR_SCREEN)(
    EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This);
typedef EFI_STATUS(EFIAPI *EFI_TEXT_SET_CURSOR_POSITION)(
    EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This, uint64_t Column, uint64_t Row);
typedef EFI_STATUS(EFIAPI *EFI_TEXT_ENABLE_CURSOR)(
    EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This, bool32 Visible);

typedef EFI_STATUS(EFIAPI *EFI_GRAPHICS_OUTPUT_PROTOCOL_QUERY_MODE)(
    EFI_GRAPHICS_OUTPUT_PROTOCOL *This, uint32_t ModeNumber,
    uint32_t *SizeOfInfo, EFI_GRAPHICS_OUTPUT_MODE_INFORMATION **Info);
typedef EFI_STATUS(EFIAPI *EFI_GRAPHICS_OUTPUT_PROTOCOL_SET_MODE)(
    EFI_GRAPHICS_OUTPUT_PROTOCOL *This, uint32_t ModeNumber);
typedef EFI_STATUS(EFIAPI *EFI_GRAPHICS_OUTPUT_PROTOCOL_BLT)(
    EFI_GRAPHICS_OUTPUT_PROTOCOL *This,
    EFI_GRAPHICS_OUTPUT_BLT_PIXEL *BltBuffer,
    EFI_GRAPHICS_OUTPUT_BLT_OPERATION BltOperation, uint32_t SourceX,
    uint32_t SourceY, uint32_t DestinationX, uint32_t DestinationY,
    uint32_t Width, uint32_t Height, uint32_t Delta);

typedef EFI_STATUS(EFIAPI *EFI_HANDLE_PROTOCOL)(EFI_HANDLE Handle,
                                                EFI_GUID *Protocol,
                                                void *out_Interface);

typedef EFI_STATUS(EFIAPI *EFI_IMAGE_LOAD)(bool32 BootPolicy,
                                           EFI_HANDLE ParentImageHandle,
                                           EFI_DEVICE_PATH_PROTOCOL *DevicePath,
                                           void *opt_SourceBuffer,
                                           uintptr_t SourceSize,
                                           EFI_HANDLE *out_ImageHandle);
typedef EFI_STATUS(EFIAPI *EFI_IMAGE_UNLOAD)(EFI_HANDLE ImageHandle);
typedef EFI_STATUS(EFIAPI *EFI_EXIT_BOOT_SERVICES)(EFI_HANDLE ImageHandle,
                                                   uintptr_t MapKey);

typedef struct {
  EFI_TABLE_HEADER Hdr;
  EFI_GET_TIME GetTime;
  EFI_SET_TIME SetTime;
  EFI_GET_WAKEUP_TIME GetWakeupTime;
  EFI_SET_WAKEUP_TIME SetWakeupTime;
  EFI_SET_VIRTUAL_ADDRESS_MAP SetVirtualAddressMap;
  EFI_CONVERT_POINTER ConvertPointer;
  EFI_GET_VARIABLE GetVariable;
  EFI_GET_NEXT_VARIABLE_NAME GetNextVariableName;
  EFI_SET_VARIABLE SetVariable;
  EFI_GET_NEXT_HIGH_MONO_COUNT GetNextHighMonotonicCount;
  EFI_RESET_SYSTEM ResetSystem;
  EFI_UPDATE_CAPSULE UpdateCapsule;
  EFI_QUERY_CAPSULE_CAPABILITIES QueryCapsuleCapabilities;
  EFI_QUERY_VARIABLE_INFO QueryVariableInfo;
} EFI_RUNTIME_SERVICES;

typedef struct {
  EFI_TABLE_HEADER Hdr;
  void *RaiseTPL;
  void *RestoreTPL;
  EFI_ALLOCATE_PAGES AllocatePages;
  EFI_FREE_PAGES FreePages;
  EFI_GET_MEMORY_MAP GetMemoryMap;
  EFI_ALLOCATE_POOL AllocatePool;
  EFI_FREE_POOL FreePool;
  EFI_CREATE_EVENT CreateEvent;
  EFI_SET_TIMER SetTimer;
  EFI_WAIT_FOR_EVENT WaitForEvent;
  EFI_SIGNAL_EVENT SignalEvent;
  EFI_CLOSE_EVENT CloseEvent;
  EFI_CHECK_EVENT CheckEvent;
  void *InstallProtocolInterface;
  void *ReinstallProtocolInterface;
  void *UninstallProtocolInterface;
  EFI_HANDLE_PROTOCOL HandleProtocol;
  void *Reserved;
  void *RegisterProtocolNotify;
  void *LocateHandle;
  void *LocateDevicePath;
  void *InstallConfigurationTable;
  EFI_IMAGE_LOAD LoadImage;
  void *StartImage;
  EFI_EXIT Exit;
  EFI_IMAGE_UNLOAD UnloadImage;
  EFI_EXIT_BOOT_SERVICES ExitBootServices;
  EFI_GET_NEXT_MONOTONIC_COUNT GetNextMonotonicCount;
  EFI_STALL Stall;
  EFI_SET_WATCHDOG_TIMER SetWatchdogTimer;
  void *ConnectController;
  void *DisconnectController;
  void *OpenProtocol;
  void *CloseProtocol;
  void *OpenProtocolInformation;
  void *ProtocolsPerHandle;
  void *LocateHandleBuffer;
  EFI_LOCATE_PROTOCOL LocateProtocol;
  void *InstallMultipleProtocolInterfaces;
  void *UninstallMultipleProtocolInterfaces;
  void *CalculateCrc32;
  EFI_COPY_MEM CopyMem;
  EFI_SET_MEM SetMem;
  EFI_CREATE_EVENT_EX CreateEventEx;
} EFI_BOOT_SERVICES;

typedef struct {
  EFI_TABLE_HEADER Hdr;
  char16_t *FirmwareVendor;
  uint32_t FirmwareRevision;
  EFI_HANDLE ConsoleInHandle;
  EFI_SIMPLE_TEXT_INPUT_PROTOCOL *ConIn;
  EFI_HANDLE ConsoleOutHandle;
  EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *ConOut;
  EFI_HANDLE StandardErrorHandle;
  EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *StdErr;
  EFI_RUNTIME_SERVICES *RuntimeServices;
  EFI_BOOT_SERVICES *BootServices;
  uintptr_t NumberOfTableEntries;
  EFI_CONFIGURATION_TABLE *ConfigurationTable;
} EFI_SYSTEM_TABLE;

struct _EFI_SIMPLE_TEXT_INPUT_PROTOCOL {
  EFI_INPUT_RESET Reset;
  EFI_INPUT_READ_KEY ReadKeyStroke;
  EFI_EVENT WaitForKey;
};

struct _EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL {
  EFI_TEXT_RESET Reset;
  EFI_TEXT_STRING OutputString;
  EFI_TEXT_TEST_STRING TestString;
  EFI_TEXT_QUERY_MODE QueryMode;
  EFI_TEXT_SET_MODE SetMode;
  EFI_TEXT_SET_ATTRIBUTE SetAttribute;
  EFI_TEXT_CLEAR_SCREEN ClearScreen;
  EFI_TEXT_SET_CURSOR_POSITION SetCursorPosition;
  EFI_TEXT_ENABLE_CURSOR EnableCursor;
  EFI_SIMPLE_TEXT_OUTPUT_MODE *Mode;
};

struct _EFI_GRAPHICS_OUTPUT_PROTOCOL {
  EFI_GRAPHICS_OUTPUT_PROTOCOL_QUERY_MODE QueryMode;
  EFI_GRAPHICS_OUTPUT_PROTOCOL_SET_MODE SetMode;
  EFI_GRAPHICS_OUTPUT_PROTOCOL_BLT Blt;
  EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE *Mode;
};

typedef struct {
  uint32_t Revision;
  EFI_HANDLE ParentHandle;
  EFI_SYSTEM_TABLE *SystemTable;
  EFI_HANDLE DeviceHandle;
  EFI_DEVICE_PATH_PROTOCOL *FilePath;
  void *Reserved;
  uint32_t LoadOptionsSize;
  void *LoadOptions;
  void *ImageBase;
  uint64_t ImageSize;
  EFI_MEMORY_TYPE ImageCodeType;
  EFI_MEMORY_TYPE ImageDataType;
  EFI_IMAGE_UNLOAD Unload;
} EFI_LOADED_IMAGE;

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_NT_EFI_H_ */
