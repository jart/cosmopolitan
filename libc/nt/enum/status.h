#ifndef COSMOPOLITAN_LIBC_NT_STATUS_H_
#define COSMOPOLITAN_LIBC_NT_STATUS_H_

/* high two bits = {success,informational,warning,error} */
#define kNtStatusSuccess                  0x00000000 /* success statuses */
#define kNtStatusWait0                    0x00000000
#define kNtStatusAbandonedWait0           0x00000080
#define kNtStatusUserApc                  0x000000C0
#define kNtStatusTimeout                  0x00000102
#define kNtStatusPending                  0x00000103
#define kNtStatusGuardPageViolation       0x80000001 /* warning statuses */
#define kNtStatusDatatypeMisalignment     0x80000002
#define kNtStatusBreakpoint               0x80000003
#define kNtStatusSingleStep               0x80000004
#define kNtStatusLongjump                 0x80000026
#define kNtStatusUnwindConsolidate        0x80000029
#define kNtStatusAccessViolation          0xC0000005 /* error statuses */
#define kNtStatusInPageError              0xC0000006
#define kNtStatusInvalidHandle            0xC0000008
#define kNtStatusInvalidParameter         0xC000000D
#define kNtStatusNoMemory                 0xC0000017
#define kNtStatusIllegalInstruction       0xC000001D
#define kNtStatusNoncontinuableException  0xC0000025
#define kNtStatusInvalidDisposition       0xC0000026
#define kNtStatusArrayBoundsExceeded      0xC000008C
#define kNtStatusFloatDenormalOperand     0xC000008D
#define kNtStatusFloatDivideByZero        0xC000008E
#define kNtStatusFloatInexactResult       0xC000008F
#define kNtStatusFloatInvalidOperation    0xC0000090
#define kNtStatusFloatOverflow            0xC0000091
#define kNtStatusFloatStackCheck          0xC0000092
#define kNtStatusFloatUnderflow           0xC0000093
#define kNtStatusIntegerDivideBYZero      0xC0000094
#define kNtStatusIntegerOverflow          0xC0000095
#define kNtStatusPrivilegedInstruction    0xC0000096
#define kNtStatusStackOverflow            0xC00000FD
#define kNtStatusDllNotFound              0xC0000135
#define kNtStatusOrdinalNotFound          0xC0000138
#define kNtStatusEntrypointNotFound       0xC0000139
#define kNtStatusControlCExit             0xC000013A
#define kNtStatusDllInitFailed            0xC0000142
#define kNtStatusFloatMultipleFaults      0xC00002B4
#define kNtStatusFloatMultipleTraps       0xC00002B5
#define kNtStatusRegNatConsumption        0xC00002C9
#define kNtStatusHeapCorruption           0xC0000374
#define kNtStatusStackBufferOverrun       0xC0000409
#define kNtStatusInvalidCruntimeParameter 0xC0000417
#define kNtStatusAssertionFailure         0xC0000420
#define kNtStatusEnclaveViolation         0xC00004A2
#define kNtStatusSegmentNotification      0x40000005
#define kNtStatusFatalAppExit             0x40000015
#define kNtStatusNotFound                 0xC0000225
#define kNtStatusCancelled                0xC0000120

#define kNtDbgExceptionHandled    0x00010001
#define kNtDbgContinue            0x00010002
#define kNtDbgReplyLater          0x40010001
#define kNtDbgTerminateThread     0x40010003
#define kNtDbgTerminateProcess    0x40010004
#define kNtDbgControlC            0x40010005
#define kNtDbgPrintexceptionC     0x40010006
#define kNtDbgRipexception        0x40010007
#define kNtDbgControlBreak        0x40010008
#define kNtDbgCommandException    0x40010009
#define kNtDbgPrintexceptionWideC 0x4001000A
#define kNtDbgExceptionNotHandled 0x80010001
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
