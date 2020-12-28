.include "o/libc/nt/codegen.inc"
.imp	advapi32,__imp_DuplicateTokenEx,DuplicateTokenEx,0

	.text.windows
DuplicateTokenEx:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_DuplicateTokenEx(%rip),%rax
	jmp	__sysv2nt6
	.endfn	DuplicateTokenEx,globl
	.previous
