.include "o/libc/nt/codegen.inc"
.imp	KernelBase,__imp_LoadLibraryExA,LoadLibraryExA,976

	.text.windows
LoadLibraryExA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_LoadLibraryExA(%rip),%rax
	jmp	__sysv2nt
	.endfn	LoadLibraryExA,globl
	.previous
