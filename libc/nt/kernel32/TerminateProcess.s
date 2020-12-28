.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_TerminateProcess,TerminateProcess,0

	.text.windows
TerminateProcess:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_TerminateProcess(%rip),%rax
	jmp	__sysv2nt
	.endfn	TerminateProcess,globl
	.previous
