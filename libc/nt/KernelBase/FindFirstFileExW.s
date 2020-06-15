.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_FindFirstFileExW,FindFirstFileExW,365

	.text.windows
FindFirstFileEx:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_FindFirstFileExW(%rip),%rax
	jmp	__sysv2nt6
	.endfn	FindFirstFileEx,globl
	.previous
