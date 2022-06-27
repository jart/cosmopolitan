.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_CreatePipe,CreatePipe,0

	.text.windows
__CreatePipe:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_CreatePipe(%rip),%rax
	jmp	__sysv2nt
	.endfn	__CreatePipe,globl
	.previous
