/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ This program is free software; you can redistribute it and/or modify         │
│ it under the terms of the GNU General Public License as published by         │
│ the Free Software Foundation; version 2 of the License.                      │
│                                                                              │
│ This program is distributed in the hope that it will be useful, but          │
│ WITHOUT ANY WARRANTY; without even the implied warranty of                   │
│ MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU             │
│ General Public License for more details.                                     │
│                                                                              │
│ You should have received a copy of the GNU General Public License            │
│ along with this program; if not, write to the Free Software                  │
│ Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA                │
│ 02110-1301 USA                                                               │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/pe.h"
#include "tool/decode/lib/peidnames.h"

const struct IdName kNtImageFileMachineNames[] = {
    {kNtImageFileMachineUnknown, "kNtImageFileMachineUnknown"},
    {kNtImageFileMachineTargetHost, "kNtImageFileMachineTargetHost"},
    {kNtImageFileMachineI386, "kNtImageFileMachineI386"},
    {kNtImageFileMachineR3000, "kNtImageFileMachineR3000"},
    {kNtImageFileMachineR4000, "kNtImageFileMachineR4000"},
    {kNtImageFileMachineR10000, "kNtImageFileMachineR10000"},
    {kNtImageFileMachineWcemipsv2, "kNtImageFileMachineWcemipsv2"},
    {kNtImageFileMachineAlpha, "kNtImageFileMachineAlpha"},
    {kNtImageFileMachineSh3, "kNtImageFileMachineSh3"},
    {kNtImageFileMachineSh3dsp, "kNtImageFileMachineSh3dsp"},
    {kNtImageFileMachineSh3e, "kNtImageFileMachineSh3e"},
    {kNtImageFileMachineSh4, "kNtImageFileMachineSh4"},
    {kNtImageFileMachineSh5, "kNtImageFileMachineSh5"},
    {kNtImageFileMachineArm, "kNtImageFileMachineArm"},
    {kNtImageFileMachineThumb, "kNtImageFileMachineThumb"},
    {kNtImageFileMachineArmnt, "kNtImageFileMachineArmnt"},
    {kNtImageFileMachineAm33, "kNtImageFileMachineAm33"},
    {kNtImageFileMachinePowerpc, "kNtImageFileMachinePowerpc"},
    {kNtImageFileMachinePowerpcfp, "kNtImageFileMachinePowerpcfp"},
    {kNtImageFileMachineIa64, "kNtImageFileMachineIa64"},
    {kNtImageFileMachineMips16, "kNtImageFileMachineMips16"},
    {kNtImageFileMachineAlpha64, "kNtImageFileMachineAlpha64"},
    {kNtImageFileMachineMipsfpu, "kNtImageFileMachineMipsfpu"},
    {kNtImageFileMachineMipsfpu16, "kNtImageFileMachineMipsfpu16"},
    {kNtImageFileMachineAxp64, "kNtImageFileMachineAxp64"},
    {kNtImageFileMachineTricore, "kNtImageFileMachineTricore"},
    {kNtImageFileMachineCef, "kNtImageFileMachineCef"},
    {kNtImageFileMachineEbc, "kNtImageFileMachineEbc"},
    {kNtImageFileMachineNexgen32e, "kNtImageFileMachineNexgen32e"},
    {kNtImageFileMachineM32r, "kNtImageFileMachineM32r"},
    {kNtImageFileMachineArm64, "kNtImageFileMachineArm64"},
    {kNtImageFileMachineCee, "kNtImageFileMachineCee"},
    {0, 0},
};

const struct IdName kNtPeOptionalHeaderMagicNames[] = {
    {kNtPe32bit, "kNtPe32bit"},
    {kNtPe64bit, "kNtPe64bit"},
    {0, 0},
};

const struct IdName kNtImageDllcharacteristicNames[] = {
    {kNtImageDllcharacteristicsHighEntropyVa,
     "kNtImageDllcharacteristicsHighEntropyVa"},
    {kNtImageDllcharacteristicsDynamicBase,
     "kNtImageDllcharacteristicsDynamicBase"},
    {kNtImageDllcharacteristicsForceIntegrity,
     "kNtImageDllcharacteristicsForceIntegrity"},
    {kNtImageDllcharacteristicsNxCompat, "kNtImageDllcharacteristicsNxCompat"},
    {kNtImageDllcharacteristicsNoIsolation,
     "kNtImageDllcharacteristicsNoIsolation"},
    {kNtImageDllcharacteristicsNoSeh, "kNtImageDllcharacteristicsNoSeh"},
    {kNtImageDllcharacteristicsNoBind, "kNtImageDllcharacteristicsNoBind"},
    {kNtImageDllcharacteristicsAppcontainer,
     "kNtImageDllcharacteristicsAppcontainer"},
    {kNtImageDllcharacteristicsWdmDriver,
     "kNtImageDllcharacteristicsWdmDriver"},
    {kNtImageDllcharacteristicsGuardCf, "kNtImageDllcharacteristicsGuardCf"},
    {kNtImageDllcharacteristicsTerminalServerAware,
     "kNtImageDllcharacteristicsTerminalServerAware"},
    {0, 0},
};

const struct IdName kNtImageSubsystemNames[] = {
    {kNtImageSubsystemUnknown, "kNtImageSubsystemUnknown"},
    {kNtImageSubsystemNative, "kNtImageSubsystemNative"},
    {kNtImageSubsystemWindowsGui, "kNtImageSubsystemWindowsGui"},
    {kNtImageSubsystemWindowsCui, "kNtImageSubsystemWindowsCui"},
    {kNtImageSubsystemOs2Cui, "kNtImageSubsystemOs2Cui"},
    {kNtImageSubsystemPosixCui, "kNtImageSubsystemPosixCui"},
    {kNtImageSubsystemNativeWindows, "kNtImageSubsystemNativeWindows"},
    {kNtImageSubsystemWindowsCeGui, "kNtImageSubsystemWindowsCeGui"},
    {kNtImageSubsystemEfiApplication, "kNtImageSubsystemEfiApplication"},
    {kNtImageSubsystemEfiBootServiceDriver,
     "kNtImageSubsystemEfiBootServiceDriver"},
    {kNtImageSubsystemEfiRuntimeDriver, "kNtImageSubsystemEfiRuntimeDriver"},
    {kNtImageSubsystemEfiRom, "kNtImageSubsystemEfiRom"},
    {kNtImageSubsystemXbox, "kNtImageSubsystemXbox"},
    {kNtImageSubsystemWindowsBootApplication,
     "kNtImageSubsystemWindowsBootApplication"},
    {kNtImageSubsystemXboxCodeCatalog, "kNtImageSubsystemXboxCodeCatalog"},
    {0, 0},
};

const struct IdName kNtImageScnNames[] = {
    {kNtImageScnTypeNoPad, "kNtImageScnTypeNoPad"},
    {kNtImageScnCntCode, "kNtImageScnCntCode"},
    {kNtImageScnCntInitializedData, "kNtImageScnCntInitializedData"},
    {kNtImageScnCntUninitializedData, "kNtImageScnCntUninitializedData"},
    {kNtImageScnLnkOther, "kNtImageScnLnkOther"},
    {kNtImageScnLnkInfo, "kNtImageScnLnkInfo"},
    {kNtImageScnLnkRemove, "kNtImageScnLnkRemove"},
    {kNtImageScnLnkComdat, "kNtImageScnLnkComdat"},
    {kNtImageScnNoDeferSpecExc, "kNtImageScnNoDeferSpecExc"},
    {kNtImageScnGprel, "kNtImageScnGprel"},
    {kNtImageScnMemFardata, "kNtImageScnMemFardata"},
    {kNtImageScnMemPurgeable, "kNtImageScnMemPurgeable"},
    {kNtImageScnMem16bit, "kNtImageScnMem16bit"},
    {kNtImageScnMemLocked, "kNtImageScnMemLocked"},
    {kNtImageScnMemPreload, "kNtImageScnMemPreload"},
    {0, 0},
};

const struct IdName kNtImageDirectoryEntryNames[] = {
    {kNtImageDirectoryEntryExport, "kNtImageDirectoryEntryExport"},
    {kNtImageDirectoryEntryImport, "kNtImageDirectoryEntryImport"},
    {kNtImageDirectoryEntryResource, "kNtImageDirectoryEntryResource"},
    {kNtImageDirectoryEntryException, "kNtImageDirectoryEntryException"},
    {kNtImageDirectoryEntrySecurity, "kNtImageDirectoryEntrySecurity"},
    {kNtImageDirectoryEntryBasereloc, "kNtImageDirectoryEntryBasereloc"},
    {kNtImageDirectoryEntryDebug, "kNtImageDirectoryEntryDebug"},
    {kNtImageDirectoryEntryArchitecture, "kNtImageDirectoryEntryArchitecture"},
    {kNtImageDirectoryEntryGlobalptr, "kNtImageDirectoryEntryGlobalptr"},
    {kNtImageDirectoryEntryTls, "kNtImageDirectoryEntryTls"},
    {kNtImageDirectoryEntryLoadConfig, "kNtImageDirectoryEntryLoadConfig"},
    {kNtImageDirectoryEntryBoundImport, "kNtImageDirectoryEntryBoundImport"},
    {kNtImageDirectoryEntryIat, "kNtImageDirectoryEntryIat"},
    {kNtImageDirectoryEntryDelayImport, "kNtImageDirectoryEntryDelayImport"},
    {kNtImageDirectoryEntryComDescriptor,
     "kNtImageDirectoryEntryComDescriptor"},
    {0, 0},
};

const struct IdName kNtImageCharacteristicNames[] = {
    {kNtImageFileRelocsStripped, "kNtImageFileRelocsStripped"},
    {kNtImageFileExecutableImage, "kNtImageFileExecutableImage"},
    {kNtImageFileLineNumsStripped, "kNtImageFileLineNumsStripped"},
    {kNtImageFileLocalSymsStripped, "kNtImageFileLocalSymsStripped"},
    {kNtImageFileAggresiveWsTrim, "kNtImageFileAggresiveWsTrim"},
    {kNtImageFileLargeAddressAware, "kNtImageFileLargeAddressAware"},
    {kNtImageFileBytesReversedLo, "kNtImageFileBytesReversedLo"},
    {kNtImageFile32bitMachine, "kNtImageFile32bitMachine"},
    {kNtImageFileDebugStripped, "kNtImageFileDebugStripped"},
    {kNtImageFileRemovableRunFromSwap, "kNtImageFileRemovableRunFromSwap"},
    {kNtImageFileNetRunFromSwap, "kNtImageFileNetRunFromSwap"},
    {kNtImageFileSystem, "kNtImageFileSystem"},
    {kNtImageFileDll, "kNtImageFileDll"},
    {kNtImageFileUpSystemOnly, "kNtImageFileUpSystemOnly"},
    {kNtImageFileBytesReversedHi, "kNtImageFileBytesReversedHi"},
    {0, 0},
};

const struct IdName kNtPeSectionNames[] = {
    {kNtPeSectionCntCode, "kNtPeSectionCntCode"},
    {kNtPeSectionCntInitializedData, "kNtPeSectionCntInitializedData"},
    {kNtPeSectionCntUninitializedData, "kNtPeSectionCntUninitializedData"},
    {kNtPeSectionGprel, "kNtPeSectionGprel"},
    {kNtPeSectionMemDiscardable, "kNtPeSectionMemDiscardable"},
    {kNtPeSectionMemNotCached, "kNtPeSectionMemNotCached"},
    {kNtPeSectionMemNotPaged, "kNtPeSectionMemNotPaged"},
    {kNtPeSectionMemShared, "kNtPeSectionMemShared"},
    {kNtPeSectionMemExecute, "kNtPeSectionMemExecute"},
    {kNtPeSectionMemRead, "kNtPeSectionMemRead"},
    {kNtPeSectionMemWrite, "kNtPeSectionMemWrite"},
    {0, 0},
};
