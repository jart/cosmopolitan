.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_LoadLibraryExW,LoadLibraryExW,0

	.text.windows
LoadLibraryEx:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	__imp_LoadLibraryExW(%rip),%rax
	jmp	__sysv2nt
	.endfn	LoadLibraryEx,globl
	.previous
