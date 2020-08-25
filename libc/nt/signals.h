#ifndef COSMOPOLITAN_LIBC_NT_EXCEPTIONS_H_
#define COSMOPOLITAN_LIBC_NT_EXCEPTIONS_H_
#include "libc/nt/enum/errormodeflags.h"
#include "libc/nt/struct/context.h"
#include "libc/nt/thunk/msabi.h"
#if 0
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
│ cosmopolitan § new technology » signals                                  ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/
#endif

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

#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

struct NtExceptionPointers;

typedef int (*NtTopLevelExceptionFilter)(const struct NtExceptionPointers *);
typedef int32_t (*NtVectoredExceptionHandler)(struct NtExceptionPointers *);

enum NtErrorModeFlags SetErrorMode(enum NtErrorModeFlags uMode);

int64_t AddVectoredExceptionHandler(uint32_t First,
                                    NtVectoredExceptionHandler pHandler);
int64_t AddVectoredContinueHandler(uint32_t First,
                                   NtVectoredExceptionHandler pHandler);

uint32_t RemoveVectoredExceptionHandler(int64_t hHandle);
uint32_t RemoveVectoredContinueHandler(int64_t hHandle);

NtTopLevelExceptionFilter SetUnhandledExceptionFilter(
    NtTopLevelExceptionFilter opt_lpTopLevelExceptionFilter);

#if ShouldUseMsabiAttribute()
#include "libc/nt/thunk/signals.inc"
#endif /* ShouldUseMsabiAttribute() */
COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_NT_EXCEPTIONS_H_ */
