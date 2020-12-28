.include "o/libc/nt/codegen.inc"
.imp	advapi32,__imp_DuplicateToken,DuplicateToken,0

	.text.windows
DuplicateToken:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_DuplicateToken(%rip),%rax
	jmp	__sysv2nt
	.endfn	DuplicateToken,globl
	.previous
