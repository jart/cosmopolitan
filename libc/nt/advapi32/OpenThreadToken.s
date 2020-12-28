.include "o/libc/nt/codegen.inc"
.imp	advapi32,__imp_OpenThreadToken,OpenThreadToken,0

	.text.windows
OpenThreadToken:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_OpenThreadToken(%rip),%rax
	jmp	__sysv2nt
	.endfn	OpenThreadToken,globl
	.previous
