.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_WaitForSingleObjectEx,WaitForSingleObjectEx,0

	.text.windows
WaitForSingleObjectEx:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_WaitForSingleObjectEx(%rip),%rax
	jmp	__sysv2nt
	.endfn	WaitForSingleObjectEx,globl
	.previous
