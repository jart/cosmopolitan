.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_LoadLibraryA,LoadLibraryA,0

	.text.windows
LoadLibraryA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_LoadLibraryA(%rip)
	leave
	ret
	.endfn	LoadLibraryA,globl
	.previous
