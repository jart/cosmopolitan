#include "libc/nt/codegen.h"
.imp	iphlpapi,__imp_CaptureInterfaceHardwareCrossTimestamp,CaptureInterfaceHardwareCrossTimestamp

	.text.windows
	.ftrace1
CaptureInterfaceHardwareCrossTimestamp:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	__imp_CaptureInterfaceHardwareCrossTimestamp(%rip),%rax
	jmp	__sysv2nt
#elif defined(__aarch64__)
	mov	x0,#0
	ret
#endif
	.endfn	CaptureInterfaceHardwareCrossTimestamp,globl
	.previous
