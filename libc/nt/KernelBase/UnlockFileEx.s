.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_UnlockFileEx,UnlockFileEx,1694

	.text.windows
UnlockFileEx:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_UnlockFileEx(%rip),%rax
	jmp	__sysv2nt6
	.endfn	UnlockFileEx,globl
	.previous
