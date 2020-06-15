.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_GetSystemDirectoryA,GetSystemDirectoryA,737

	.text.windows
GetSystemDirectoryA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_GetSystemDirectoryA(%rip),%rax
	jmp	__sysv2nt
	.endfn	GetSystemDirectoryA,globl
	.previous
