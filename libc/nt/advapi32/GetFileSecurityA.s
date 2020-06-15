.include "o/libc/nt/codegen.inc"
.imp	advapi32,__imp_GetFileSecurityA,GetFileSecurityA,1326

	.text.windows
GetFileSecurityA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetFileSecurityA(%rip),%rax
	jmp	__sysv2nt6
	.endfn	GetFileSecurityA,globl
	.previous
