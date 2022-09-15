.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_LockFileEx,LockFileEx,0

	.text.windows
__LockFileEx:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_LockFileEx(%rip),%rax
	jmp	__sysv2nt6
	.endfn	__LockFileEx,globl
	.previous
