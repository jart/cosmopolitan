#ifndef COSMOPOLITAN_LIBC_NT_ENUM_STARTF_H_
#define COSMOPOLITAN_LIBC_NT_ENUM_STARTF_H_

#define kNtStartfUseshowwindow 0x00000001 /* otherwise wShowWindow ignored */
#define kNtStartfUsesize 0x00000002       /* otherwise dwX / dwY ignored */
#define kNtStartfUseposition 0x00000004   /* otherwise dwX/YSize ignored */
#define kNtStartfUsecountchars 0x00000008 /* otherwise dwX/YCountChars ign. */
#define kNtStartfUsefillattribute 0x00000010 /* etc. */
#define kNtStartfRunfullscreen 0x00000020
#define kNtStartfForceonfeedback 0x00000040
#define kNtStartfForceofffeedback 0x00000080
#define kNtStartfUsestdhandles 0x00000100 /* otherwise hStd... ignored */
#define kNtStartfUsehotkey 0x00000200
#define kNtStartfTitleislinkname 0x00000800
#define kNtStartfTitleisappid 0x00001000
#define kNtStartfPreventpinning 0x00002000
#define kNtStartfUntrustedsource 0x00008000

#endif /* COSMOPOLITAN_LIBC_NT_ENUM_STARTF_H_ */
