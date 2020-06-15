.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_GetFileAttributesExW,GetFileAttributesExW,545

	.text.windows
GetFileAttributesEx:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetFileAttributesExW(%rip),%rax
	jmp	__sysv2nt
	.endfn	GetFileAttributesEx,globl
	.previous
