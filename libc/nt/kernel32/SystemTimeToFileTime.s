.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_SystemTimeToFileTime,SystemTimeToFileTime,0

	.text.windows
SystemTimeToFileTime:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_SystemTimeToFileTime(%rip),%rax
	jmp	__sysv2nt
	.endfn	SystemTimeToFileTime,globl
	.previous
