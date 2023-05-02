#include "libc/nt/codegen.h"
.imp	iphlpapi,__imp_CaptureInterfaceHardwareCrossTimestamp,CaptureInterfaceHardwareCrossTimestamp,0

#ifdef __x86_64__
	.text.windows
CaptureInterfaceHardwareCrossTimestamp:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_CaptureInterfaceHardwareCrossTimestamp(%rip),%rax
	jmp	__sysv2nt
	.endfn	CaptureInterfaceHardwareCrossTimestamp,globl
	.previous
#endif
