.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_GetProcessIoCounters,GetProcessIoCounters,701

	.text.windows
GetProcessIoCounters:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetProcessIoCounters(%rip),%rax
	jmp	__sysv2nt
	.endfn	GetProcessIoCounters,globl
	.previous
