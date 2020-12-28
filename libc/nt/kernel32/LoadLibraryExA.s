.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_LoadLibraryExA,LoadLibraryExA,0

	.text.windows
LoadLibraryExA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_LoadLibraryExA(%rip),%rax
	jmp	__sysv2nt
	.endfn	LoadLibraryExA,globl
	.previous
