.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_RegisterWaitForSingleObject,RegisterWaitForSingleObject,1191

	.text.windows
RegisterWaitForSingleObject:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_RegisterWaitForSingleObject(%rip),%rax
	jmp	__sysv2nt6
	.endfn	RegisterWaitForSingleObject,globl
	.previous
