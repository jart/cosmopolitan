#ifndef COSMOPOLITAN_LIBC_NT_ENUM_CONSOLEMODEFLAGS_H_
#define COSMOPOLITAN_LIBC_NT_ENUM_CONSOLEMODEFLAGS_H_

/* input mode */
#define kNtEnableProcessedInput 0x0001u
#define kNtEnableLineInput 0x0002u
#define kNtEnableEchoInput 0x0004u
#define kNtEnableWindowInput 0x0008u
#define kNtEnableMouseInput 0x0010u
#define kNtEnableInsertMode 0x0020u
#define kNtEnableQuickEditMode 0x0040u
#define kNtEnableExtendedFlags 0x0080u
#define kNtEnableAutoPosition 0x0100u
#define kNtEnableVirtualTerminalInput 0x0200u

/* output mode */
#define kNtEnableProcessedOutput 0x0001u
#define kNtEnableWrapAtEolOutput 0x0002u
#define kNtEnableVirtualTerminalProcessing 0x0004u
#define kNtDisableNewlineAutoReturn 0x0008u
#define kNtEnableLvbGridWorldwide 0x0010u

#endif /* COSMOPOLITAN_LIBC_NT_ENUM_CONSOLEMODEFLAGS_H_ */
