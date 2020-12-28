.include "o/libc/nt/codegen.inc"
.imp	advapi32,__imp_GetFileSecurityW,GetFileSecurityW,0

	.text.windows
GetFileSecurity:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetFileSecurityW(%rip),%rax
	jmp	__sysv2nt6
	.endfn	GetFileSecurity,globl
	.previous
