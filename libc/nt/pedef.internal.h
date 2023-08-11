#ifndef COSMOPOLITAN_LIBC_NT_PEDEF_H_
#define COSMOPOLITAN_LIBC_NT_PEDEF_H_
/*───────────────────────────────────────────────────────────────────────────│─╗
│ αcτµαlly pδrταblε εxεcµταblε § portable executable                       ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│─╝
  .text:  Code
  .data:  Initialized data
  .bss:   Uninitialized data
  .rdata: Const/read-only (and initialized) data
  .edata: Export descriptors
  .idata: Import descriptors
  .reloc: Relocation table (for code instructions with absolute
          addressing when the module could not be loaded at its
          preferred base address)
  .rsrc:  Resources (icon, bitmap, dialog, ...)
  .tls:   __declspec(thread) data (Fails with dynamically loaded DLLs ->
          hard to find bugs) */

#define kNtImageDosSignature   0x5A4D
#define kNtImageOs2Signature   0x454E
#define kNtImageOs2SignatureLe 0x454C
#define kNtImageVxdSignature   0x454C
#define kNtImageNtSignature    0x00004550

#define kNtPeFileRelocsStripped    0x00001
#define kNtPeFileExecutableImage   0x00002
#define kNtPeFileLineNumsStripped  0x00004
#define kNtPeFileLocalSymsStripped 0x00008
#define kNtPeFile_32bitMachine     0x00100
#define kNtPeFileDll               0x02000

#define kNtPe32bit 0x010b
#define kNtPe64bit 0x020b

#define kNtPeSectionCntCode              0x00000020
#define kNtPeSectionCntInitializedData   0x00000040
#define kNtPeSectionCntUninitializedData 0x00000080
#define kNtPeSectionGprel                0x00008000
#define kNtPeSectionMemDiscardable       0x02000000
#define kNtPeSectionMemNotCached         0x04000000
#define kNtPeSectionMemNotPaged          0x08000000
#define kNtPeSectionMemShared            0x10000000
#define kNtPeSectionMemExecute           0x20000000
#define kNtPeSectionMemRead              0x40000000
#define kNtPeSectionMemWrite             0x80000000

#define kNtPeGuardCfInstrumented         0x000000100
#define kNtPeGuardCfwInstrumented        0x000000200
#define kNtPeGuardCfFunctionTablePresent 0x000000400
#define kNtPeGuardSecurityCookieUnused   0x000000800

#define kNtPeRelBasedAbsolute      0
#define kNtPeRelBasedHigh          1
#define kNtPeRelBasedLow           2
#define kNtPeRelBasedHighlow       3
#define kNtPeRelBasedHighadj       4
#define kNtPeRelBasedMipsJmpaddr   5
#define kNtPeRelBasedSection       6
#define kNtPeRelBasedRel32         7
#define kNtPeRelBasedMipsJmpaddr16 9
#define kNtPeRelBasedIa64Imm64     9
#define kNtPeRelBasedDir64         10
#define kNtPeRelBasedHigh3adj      11

#define kNtImageFileRelocsStripped       0x0001
#define kNtImageFileExecutableImage      0x0002
#define kNtImageFileLineNumsStripped     0x0004
#define kNtImageFileLocalSymsStripped    0x0008
#define kNtImageFileAggresiveWsTrim      0x0010
#define kNtImageFileLargeAddressAware    0x0020
#define kNtImageFileBytesReversedLo      0x0080
#define kNtImageFile32bitMachine         0x0100
#define kNtImageFileDebugStripped        0x0200
#define kNtImageFileRemovableRunFromSwap 0x0400
#define kNtImageFileNetRunFromSwap       0x0800
#define kNtImageFileSystem               0x1000
#define kNtImageFileDll                  0x2000
#define kNtImageFileUpSystemOnly         0x4000
#define kNtImageFileBytesReversedHi      0x8000

#define kNtImageFileMachineUnknown    0
#define kNtImageFileMachineTargetHost 0x0001
#define kNtImageFileMachineI386       0x014c
#define kNtImageFileMachineR3000      0x0162
#define kNtImageFileMachineR4000      0x0166
#define kNtImageFileMachineR10000     0x0168
#define kNtImageFileMachineWcemipsv2  0x0169
#define kNtImageFileMachineAlpha      0x0184
#define kNtImageFileMachineSh3        0x01a2
#define kNtImageFileMachineSh3dsp     0x01a3
#define kNtImageFileMachineSh3e       0x01a4
#define kNtImageFileMachineSh4        0x01a6
#define kNtImageFileMachineSh5        0x01a8
#define kNtImageFileMachineArm        0x01c0
#define kNtImageFileMachineThumb      0x01c2
#define kNtImageFileMachineArmnt      0x01c4
#define kNtImageFileMachineAm33       0x01d3
#define kNtImageFileMachinePowerpc    0x01F0
#define kNtImageFileMachinePowerpcfp  0x01f1
#define kNtImageFileMachineIa64       0x0200
#define kNtImageFileMachineMips16     0x0266
#define kNtImageFileMachineAlpha64    0x0284
#define kNtImageFileMachineMipsfpu    0x0366
#define kNtImageFileMachineMipsfpu16  0x0466
#define kNtImageFileMachineAxp64      kNtImageFileMachineAlpha64
#define kNtImageFileMachineTricore    0x0520
#define kNtImageFileMachineCef        0x0CEF
#define kNtImageFileMachineEbc        0x0EBC
#define kNtImageFileMachineNexgen32e  0x8664
#define kNtImageFileMachineM32r       0x9041
#define kNtImageFileMachineArm64      0xAA64
#define kNtImageFileMachineCee        0xC0EE

#define kNtImageSubsystemUnknown                0
#define kNtImageSubsystemNative                 1
#define kNtImageSubsystemWindowsGui             2
#define kNtImageSubsystemWindowsCui             3
#define kNtImageSubsystemOs2Cui                 5
#define kNtImageSubsystemPosixCui               7
#define kNtImageSubsystemNativeWindows          8
#define kNtImageSubsystemWindowsCeGui           9
#define kNtImageSubsystemEfiApplication         10
#define kNtImageSubsystemEfiBootServiceDriver   11
#define kNtImageSubsystemEfiRuntimeDriver       12
#define kNtImageSubsystemEfiRom                 13
#define kNtImageSubsystemXbox                   14
#define kNtImageSubsystemWindowsBootApplication 16
#define kNtImageSubsystemXboxCodeCatalog        17

#define kNtImageDllcharacteristicsHighEntropyVa       0x0020
#define kNtImageDllcharacteristicsDynamicBase         0x0040
#define kNtImageDllcharacteristicsForceIntegrity      0x0080
#define kNtImageDllcharacteristicsNxCompat            0x0100
#define kNtImageDllcharacteristicsNoIsolation         0x0200
#define kNtImageDllcharacteristicsNoSeh               0x0400
#define kNtImageDllcharacteristicsNoBind              0x0800
#define kNtImageDllcharacteristicsAppcontainer        0x1000
#define kNtImageDllcharacteristicsWdmDriver           0x2000
#define kNtImageDllcharacteristicsGuardCf             0x4000
#define kNtImageDllcharacteristicsTerminalServerAware 0x8000

#define kNtImageDirectoryEntryExport        0
#define kNtImageDirectoryEntryImport        1
#define kNtImageDirectoryEntryResource      2
#define kNtImageDirectoryEntryException     3
#define kNtImageDirectoryEntrySecurity      4
#define kNtImageDirectoryEntryBasereloc     5
#define kNtImageDirectoryEntryDebug         6
#define kNtImageDirectoryEntryArchitecture  7
#define kNtImageDirectoryEntryGlobalptr     8
#define kNtImageDirectoryEntryTls           9
#define kNtImageDirectoryEntryLoadConfig    10
#define kNtImageDirectoryEntryBoundImport   11
#define kNtImageDirectoryEntryIat           12
#define kNtImageDirectoryEntryDelayImport   13
#define kNtImageDirectoryEntryComDescriptor 14

#define kNtImageScnTypeNoPad            0x00000008
#define kNtImageScnCntCode              0x00000020
#define kNtImageScnCntInitializedData   0x00000040
#define kNtImageScnCntUninitializedData 0x00000080
#define kNtImageScnLnkOther             0x00000100
#define kNtImageScnLnkInfo              0x00000200
#define kNtImageScnLnkRemove            0x00000800
#define kNtImageScnLnkComdat            0x00001000
#define kNtImageScnNoDeferSpecExc       0x00004000
#define kNtImageScnGprel                0x00008000
#define kNtImageScnMemFardata           0x00008000
#define kNtImageScnMemPurgeable         0x00020000
#define kNtImageScnMem16bit             0x00020000
#define kNtImageScnMemLocked            0x00040000
#define kNtImageScnMemPreload           0x00080000

#define kNtImageScnAlign1bytes    0x00100000
#define kNtImageScnAlign2bytes    0x00200000
#define kNtImageScnAlign4bytes    0x00300000
#define kNtImageScnAlign8bytes    0x00400000
#define kNtImageScnAlign16bytes   0x00500000
#define kNtImageScnAlign32bytes   0x00600000
#define kNtImageScnAlign64bytes   0x00700000
#define kNtImageScnAlign128bytes  0x00800000
#define kNtImageScnAlign256bytes  0x00900000
#define kNtImageScnAlign512bytes  0x00A00000
#define kNtImageScnAlign1024bytes 0x00B00000
#define kNtImageScnAlign2048bytes 0x00C00000
#define kNtImageScnAlign4096bytes 0x00D00000
#define kNtImageScnAlign8192bytes 0x00E00000
#define kNtImageScnAlignMask      0x00F00000

#define kNtImageScnLnkNrelocOvfl  0x01000000
#define kNtImageScnMemDiscardable 0x02000000
#define kNtImageScnMemNotCached   0x04000000
#define kNtImageScnMemNotPaged    0x08000000
#define kNtImageScnMemShared      0x10000000
#define kNtImageScnMemExecute     0x20000000
#define kNtImageScnMemRead        0x40000000
#define kNtImageScnMemWrite       0x80000000
#define kNtImageScnScaleIndex     0x00000001

#define kNtImageSymUndefined            ((uint16_t)0)
#define kNtImageSymAbsolute             ((uint16_t)-1)
#define kNtImageSymDebug                ((uint16_t)-2)
#define kNtImageSymSectionMax           0xFEFF
#define kNtImageSymSectionMaxEx         __LONG_MAX__
#define kNtImageSymTypeNull             0x0000
#define kNtImageSymTypeVoid             0x0001
#define kNtImageSymTypeChar             0x0002
#define kNtImageSymTypeShort            0x0003
#define kNtImageSymTypeInt              0x0004
#define kNtImageSymTypeLong             0x0005
#define kNtImageSymTypeFloat            0x0006
#define kNtImageSymTypeDouble           0x0007
#define kNtImageSymTypeStruct           0x0008
#define kNtImageSymTypeUnion            0x0009
#define kNtImageSymTypeEnum             0x000A
#define kNtImageSymTypeMoe              0x000B
#define kNtImageSymTypeByte             0x000C
#define kNtImageSymTypeWord             0x000D
#define kNtImageSymTypeUint             0x000E
#define kNtImageSymTypeDword            0x000F
#define kNtImageSymTypePcode            0x8000
#define kNtImageSymDtypeNull            0
#define kNtImageSymDtypePointer         1
#define kNtImageSymDtypeFunction        2
#define kNtImageSymDtypeArray           3
#define kNtImageSymClassEndOfFunction   ((unsigned char)-1)
#define kNtImageSymClassNull            0x0000
#define kNtImageSymClassAutomatic       0x0001
#define kNtImageSymClassExternal        0x0002
#define kNtImageSymClassStatic          0x0003
#define kNtImageSymClassRegister        0x0004
#define kNtImageSymClassExternalDef     0x0005
#define kNtImageSymClassLabel           0x0006
#define kNtImageSymClassUndefinedLabel  0x0007
#define kNtImageSymClassMemberOfStruct  0x0008
#define kNtImageSymClassArgument        0x0009
#define kNtImageSymClassStructTag       0x000A
#define kNtImageSymClassMemberOfUnion   0x000B
#define kNtImageSymClassUnionTag        0x000C
#define kNtImageSymClassTypeDefinition  0x000D
#define kNtImageSymClassUndefinedStatic 0x000E
#define kNtImageSymClassEnumTag         0x000F
#define kNtImageSymClassMemberOfEnum    0x0010
#define kNtImageSymClassRegisterParam   0x0011
#define kNtImageSymClassBitField        0x0012
#define kNtImageSymClassFarExternal     0x0044
#define kNtImageSymClassBlock           0x0064
#define kNtImageSymClassFunction        0x0065
#define kNtImageSymClassEndOfStruct     0x0066
#define kNtImageSymClassFile            0x0067
#define kNtImageSymClassSection         0x0068
#define kNtImageSymClassWeakExternal    0x0069
#define kNtImageSymClassClrToken        0x006B

#define kNtImageComdatSelectNoduplicates 1
#define kNtImageComdatSelectAny          2
#define kNtImageComdatSelectSameSize     3
#define kNtImageComdatSelectExactMatch   4
#define kNtImageComdatSelectAssociative  5
#define kNtImageComdatSelectLargest      6
#define kNtImageComdatSelectNewest       7

#define kNtImageWeakExternSearchNolibrary 1
#define kNtImageWeakExternSearchLibrary   2
#define kNtImageWeakExternSearchAlias     3
#define kNtImageWeakExternAntiDependency  4

#define kNtImageRelNexgen32eAbsolute 0x0000
#define kNtImageRelNexgen32eAddr64   0x0001
#define kNtImageRelNexgen32eAddr32   0x0002
#define kNtImageRelNexgen32eAddr32nb 0x0003
#define kNtImageRelNexgen32eRel32    0x0004
#define kNtImageRelNexgen32eRel32_1  0x0005
#define kNtImageRelNexgen32eRel32_2  0x0006
#define kNtImageRelNexgen32eRel32_3  0x0007
#define kNtImageRelNexgen32eRel32_4  0x0008
#define kNtImageRelNexgen32eRel32_5  0x0009
#define kNtImageRelNexgen32eSection  0x000A
#define kNtImageRelNexgen32eSecrel   0x000B
#define kNtImageRelNexgen32eSecrel7  0x000C
#define kNtImageRelNexgen32eToken    0x000D
#define kNtImageRelNexgen32eSrel32   0x000E
#define kNtImageRelNexgen32ePair     0x000F
#define kNtImageRelNexgen32eSspan32  0x0010

#define kNtImageRelBasedAbsolute          0
#define kNtImageRelBasedHigh              1
#define kNtImageRelBasedLow               2
#define kNtImageRelBasedHighlow           3
#define kNtImageRelBasedHighadj           4
#define kNtImageRelBasedMachineSpecific_5 5
#define kNtImageRelBasedReserved          6
#define kNtImageRelBasedMachineSpecific_7 7
#define kNtImageRelBasedMachineSpecific_8 8
#define kNtImageRelBasedMachineSpecific_9 9
#define kNtImageRelBasedDir64             10

#define kNtImageArchiveStartSize       8
#define kNtImageArchiveStart           "!<arch>\n"
#define kNtImageArchiveEnd             "`\n"
#define kNtImageArchivePad             "\n"
#define kNtImageArchiveLinkerMember    "/               "
#define kNtImageArchiveLongnamesMember "//              "
#define kNtImageArchiveHybridmapMember "/<HYBRIDMAP>/   "

#define kNtImageOrdinalFlag           0x8000000000000000
#define NtImageOrdinal(Ordinal)       (Ordinal & 0xffff)
#define NtImageSnapByOrdinal(Ordinal) ((Ordinal & IMAGE_ORDINAL_FLAG64) != 0)

#define kNtImageResourceNameIsString    0x80000000
#define kNtImageResourceDataIsDirectory 0x80000000

#define kNtImageDynamicRelocationGuardRfPrologue 0x00000001
#define kNtImageDynamicRelocationGuardRfEpilogue 0x00000002

#define kNtImageHotPatchBaseObligatory              0x00000001
#define kNtImageHotPatchChunkInverse                0x80000000
#define kNtImageHotPatchChunkObligatory             0x40000000
#define kNtImageHotPatchChunkReserved               0x3FF03000
#define kNtImageHotPatchChunkType                   0x000FC000
#define kNtImageHotPatchChunkSourceRva              0x00008000
#define kNtImageHotPatchChunkTargetRva              0x00004000
#define kNtImageHotPatchChunkSize                   0x00000FFF
#define kNtImageHotPatchNone                        0x00000000
#define kNtImageHotPatchFunction                    0x0001C000
#define kNtImageHotPatchAbsolute                    0x0002C000
#define kNtImageHotPatchRel32                       0x0003C000
#define kNtImageHotPatchCallTarget                  0x00044000
#define kNtImageHotPatchIndirect                    0x0005C000
#define kNtImageHotPatchNoCallTarget                0x00064000
#define kNtImageHotPatchDynamicValue                0x00078000
#define kNtImageGuardCfInstrumented                 0x00000100
#define kNtImageGuardCfwInstrumented                0x00000200
#define kNtImageGuardCfFunctionTablePresent         0x00000400
#define kNtImageGuardSecurityCookieUnused           0x00000800
#define kNtImageGuardProtectDelayloadIat            0x00001000
#define kNtImageGuardDelayloadIatInItsOwnSection    0x00002000
#define kNtImageGuardCfExportSuppressionInfoPresent 0x00004000
#define kNtImageGuardCfEnableExportSuppression      0x00008000
#define kNtImageGuardCfLongjumpTablePresent         0x00010000
#define kNtImageGuardRfInstrumented                 0x00020000
#define kNtImageGuardRfEnable                       0x00040000
#define kNtImageGuardRfStrict                       0x00080000
#define kNtImageGuardCfFunctionTableSizeMask        0xF0000000
#define kNtImageGuardCfFunctionTableSizeShift       28
#define kNtImageGuardFlagFidSuppressed              0x01
#define kNtImageGuardFlagExportSuppressed           0x02

#define kNtImageEnclaveImportMatchNone     0x00000000
#define kNtImageEnclaveImportMatchUniqueId 0x00000001
#define kNtImageEnclaveImportMatchAuthorId 0x00000002
#define kNtImageEnclaveImportMatchFamilyId 0x00000003
#define kNtImageEnclaveImportMatchImageId  0x00000004

#define kNtImageDebugTypeUnknown     0
#define kNtImageDebugTypeCoff        1
#define kNtImageDebugTypeCodeview    2
#define kNtImageDebugTypeFpo         3
#define kNtImageDebugTypeMisc        4
#define kNtImageDebugTypeException   5
#define kNtImageDebugTypeFixup       6
#define kNtImageDebugTypeOmapToSrc   7
#define kNtImageDebugTypeOmapFromSrc 8
#define kNtImageDebugTypeBorland     9
#define kNtImageDebugTypeReserved10  10
#define kNtImageDebugTypeClsid       11
#define kNtImageDebugTypeVcFeature   12
#define kNtImageDebugTypePogo        13
#define kNtImageDebugTypeIltcg       14
#define kNtImageDebugTypeMpx         15
#define kNtImageDebugTypeRepro       16

#define kNtFrameFpo    0
#define kNtFrameTrap   1
#define kNtFrameTss    2
#define kNtFrameNonfpo 3

#define kNtImageSizeofShortName          8
#define kNtImageSizeofSectionHeader      40
#define kNtImageSizeofSymbol             18
#define kNtImageEnclaveLongIdLength      32
#define kNtImageEnclaveShortIdLength     16
#define kNtImageNumberofDirectoryEntries 16

#endif /* COSMOPOLITAN_LIBC_NT_PEDEF_H_ */
