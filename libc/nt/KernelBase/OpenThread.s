.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_OpenThread,OpenThread,1056

	.text.windows
OpenThread:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_OpenThread(%rip),%rax
	jmp	__sysv2nt
	.endfn	OpenThread,globl
	.previous
