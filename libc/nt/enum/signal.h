#ifndef COSMOPOLITAN_LIBC_NT_ENUM_SIGNAL_H_
#define COSMOPOLITAN_LIBC_NT_ENUM_SIGNAL_H_

#define kNtSignalBreakpoint          0x80000003u
#define kNtSignalIllegalInstruction  0xC000001Du
#define kNtSignalPrivInstruction     0xC0000096u
#define kNtSignalGuardPage           0x80000001u
#define kNtSignalAccessViolation     0xC0000005u
#define kNtSignalInPageError         0xC0000006u
#define kNtSignalInvalidHandle       0xC0000008u
#define kNtSignalInvalidParameter    0xC000000du
#define kNtSignalFltDenormalOperand  0xC000008Du
#define kNtSignalFltDivideByZero     0xC000008Eu
#define kNtSignalFltInexactResult    0xC000008Fu
#define kNtSignalFltInvalidOperation 0xC0000090u
#define kNtSignalFltOverflow         0xC0000091u
#define kNtSignalFltStackCheck       0xC0000092u
#define kNtSignalFltUnderflow        0xC0000093u
#define kNtSignalIntegerDivideByZero 0xC0000094u
#define kNtSignalDllNotFound         0xC0000135u
#define kNtSignalOrdinalNotFound     0xC0000138u
#define kNtSignalEntrypointNotFound  0xC0000139u
#define kNtSignalControlCExit        0xC000013Au
#define kNtSignalDllInitFailed       0xC0000142u
#define kNtSignalFloatMultipleFaults 0xC00002B4u
#define kNtSignalFloatMultipleTraps  0xC00002B5u
#define kNtSignalAssertionFailure    0xC0000420u

#endif /* COSMOPOLITAN_LIBC_NT_ENUM_SIGNAL_H_ */
