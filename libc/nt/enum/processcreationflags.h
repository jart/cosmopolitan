#ifndef COSMOPOLITAN_LIBC_NT_ENUM_PROCESSCREATIONFLAGS_H_
#define COSMOPOLITAN_LIBC_NT_ENUM_PROCESSCREATIONFLAGS_H_

#define kNtIdlePriorityClass        0x00000040u
#define kNtBelowNormalPriorityClass 0x00004000u
#define kNtNormalPriorityClass      0x00000020u
#define kNtAboveNormalPriorityClass 0x00008000u
#define kNtHighPriorityClass        0x00000080u
#define kNtRealtimePriorityClass    0x00000100u

#define kNtDebugProcess                 0x00000001u
#define kNtDebugOnlyThisProcess         0x00000002u
#define kNtCreateSuspended              0x00000004u
#define kNtDetachedProcess              0x00000008u
#define kNtCreateNewConsole             0x00000010u
#define kNtCreateNewProcessGroup        0x00000200u
#define kNtCreateUnicodeEnvironment     0x00000400u
#define kNtCreateSeparateWowVdm         0x00000800u
#define kNtCreateSharedWowVdm           0x00001000u
#define kNtCreateForcedos               0x00002000u
#define kNtInheritParentAffinity        0x00010000u
#define kNtCreateProtectedProcess       0x00040000u
#define kNtExtendedStartupinfoPresent   0x00080000u
#define kNtProcessModeBackgroundBegin   0x00100000u
#define kNtProcessModeBackgroundEnd     0x00200000u
#define kNtCreateSecureProcess          0x00400000u
#define kNtCreateBreakawayFromJob       0x01000000u
#define kNtCreatePreserveCodeAuthzLevel 0x02000000u
#define kNtCreateDefaultErrorMode       0x04000000u
#define kNtCreateNoWindow               0x08000000u
#define kNtProfileUser                  0x10000000u
#define kNtProfileKernel                0x20000000u
#define kNtProfileServer                0x40000000u
#define kNtCreateIgnoreSystemDefault    0x80000000u

#define kNtStackSizeParamIsAReservation 0x00010000

#endif /* COSMOPOLITAN_LIBC_NT_ENUM_PROCESSCREATIONFLAGS_H_ */
