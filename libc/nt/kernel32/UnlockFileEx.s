.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_UnlockFileEx,UnlockFileEx,0

	.text.windows
__UnlockFileEx:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_UnlockFileEx(%rip),%rax
	jmp	__sysv2nt6
	.endfn	__UnlockFileEx,globl
	.previous
