.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_TerminateThread,TerminateThread,0

	.text.windows
TerminateThread:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_TerminateThread(%rip),%rax
	jmp	__sysv2nt
	.endfn	TerminateThread,globl
	.previous
