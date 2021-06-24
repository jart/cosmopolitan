.include "o/libc/nt/codegen.inc"
.imp	iphlpapi,__imp_CaptureInterfaceHardwareCrossTimestamp,CaptureInterfaceHardwareCrossTimestamp,0

	.text.windows
CaptureInterfaceHardwareCrossTimestamp:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_CaptureInterfaceHardwareCrossTimestamp(%rip),%rax
	jmp	__sysv2nt
	.endfn	CaptureInterfaceHardwareCrossTimestamp,globl
	.previous
