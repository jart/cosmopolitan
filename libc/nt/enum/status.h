#ifndef COSMOPOLITAN_LIBC_NT_STATUS_H_
#define COSMOPOLITAN_LIBC_NT_STATUS_H_

/* high two bits = {success,informational,warning,error} */
#define kNtStatusSuccess                  0x00000000u /* success statuses */
#define kNtStatusWait0                    0x00000000u
#define kNtStatusAbandonedWait0           0x00000080u
#define kNtStatusUserApc                  0x000000C0u
#define kNtStatusTimeout                  0x00000102u
#define kNtStatusPending                  0x00000103u
#define kNtStatusGuardPageViolation       0x80000001u /* warning statuses */
#define kNtStatusDatatypeMisalignment     0x80000002u
#define kNtStatusBreakpoint               0x80000003u
#define kNtStatusSingleStep               0x80000004u
#define kNtStatusLongjump                 0x80000026u
#define kNtStatusUnwindConsolidate        0x80000029u
#define kNtStatusAccessViolation          0xC0000005u /* error statuses */
#define kNtStatusInPageError              0xC0000006u
#define kNtStatusInvalidHandle            0xC0000008u
#define kNtStatusInvalidParameter         0xC000000Du
#define kNtStatusNoMemory                 0xC0000017u
#define kNtStatusIllegalInstruction       0xC000001Du
#define kNtStatusNoncontinuableException  0xC0000025u
#define kNtStatusInvalidDisposition       0xC0000026u
#define kNtStatusArrayBoundsExceeded      0xC000008Cu
#define kNtStatusFloatDenormalOperand     0xC000008Du
#define kNtStatusFloatDivideByZero        0xC000008Eu
#define kNtStatusFloatInexactResult       0xC000008Fu
#define kNtStatusFloatInvalidOperation    0xC0000090u
#define kNtStatusFloatOverflow            0xC0000091u
#define kNtStatusFloatStackCheck          0xC0000092u
#define kNtStatusFloatUnderflow           0xC0000093u
#define kNtStatusIntegerDivideBYZero      0xC0000094u
#define kNtStatusIntegerOverflow          0xC0000095u
#define kNtStatusPrivilegedInstruction    0xC0000096u
#define kNtStatusStackOverflow            0xC00000FDu
#define kNtStatusDllNotFound              0xC0000135u
#define kNtStatusOrdinalNotFound          0xC0000138u
#define kNtStatusEntrypointNotFound       0xC0000139u
#define kNtStatusControlCExit             0xC000013Au
#define kNtStatusDllInitFailed            0xC0000142u
#define kNtStatusFloatMultipleFaults      0xC00002B4u
#define kNtStatusFloatMultipleTraps       0xC00002B5u
#define kNtStatusRegNatConsumption        0xC00002C9u
#define kNtStatusHeapCorruption           0xC0000374u
#define kNtStatusStackBufferOverrun       0xC0000409u
#define kNtStatusInvalidCruntimeParameter 0xC0000417u
#define kNtStatusAssertionFailure         0xC0000420u
#define kNtStatusEnclaveViolation         0xC00004A2u
#define kNtStatusSegmentNotification      0x40000005u
#define kNtStatusFatalAppExit             0x40000015u
#define kNtStatusNotFound                 0xC0000225u
#define kNtStatusCancelled                0xC0000120u

#define kNtDbgExceptionHandled    0x00010001u
#define kNtDbgContinue            0x00010002u
#define kNtDbgReplyLater          0x40010001u
#define kNtDbgTerminateThread     0x40010003u
#define kNtDbgTerminateProcess    0x40010004u
#define kNtDbgControlC            0x40010005u
#define kNtDbgPrintexceptionC     0x40010006u
#define kNtDbgRipexception        0x40010007u
#define kNtDbgControlBreak        0x40010008u
#define kNtDbgCommandException    0x40010009u
#define kNtDbgPrintexceptionWideC 0x4001000Au
#define kNtDbgExceptionNotHandled 0x80010001u
#define kNtStillActive            kNtStatusPending

#if !(__ASSEMBLER__ + __LINKER__ + 0)

typedef uint32_t NtStatus;

#define NtSuccess(s)      ((NtStatus)(s) >= 0)
#define NtInformation(s)  (NtSeverity(s) == 1)
#define NtWarning(s)      (NtSeverity(s) == 2)
#define NtError(s)        (NtSeverity(s) == 3)
#define NtCode(s)         ((NtStatus)(s)&0xffff)
#define NtSeverity(s)     ((NtStatus)(s) >> 30)
#define NtFacility(s)     (((NtStatus)(s) >> 16) & 0xfff)
#define NtFacilityCode(s) ((NtStatus)(s)&0x0FFFFFFF)

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_NT_STATUS_H_ */
