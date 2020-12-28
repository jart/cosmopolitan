.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_OpenThread,OpenThread,0

	.text.windows
OpenThread:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_OpenThread(%rip),%rax
	jmp	__sysv2nt
	.endfn	OpenThread,globl
	.previous
