.include "o/libc/nt/codegen.inc"
.imp	advapi32,__imp_GetUserNameA,GetUserNameA,1380

	.text.windows
GetUserNameA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetUserNameA(%rip),%rax
	jmp	__sysv2nt
	.endfn	GetUserNameA,globl
	.previous
