.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_PurgeComm,PurgeComm,0

	.text.windows
PurgeComm:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_PurgeComm(%rip),%rax
	jmp	__sysv2nt
	.endfn	PurgeComm,globl
	.previous
