.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_LockFileEx,LockFileEx,994

	.text.windows
LockFileEx:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_LockFileEx(%rip),%rax
	jmp	__sysv2nt6
	.endfn	LockFileEx,globl
	.previous
