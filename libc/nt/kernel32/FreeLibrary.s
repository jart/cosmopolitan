.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_FreeLibrary,FreeLibrary,0

	.text.windows
FreeLibrary:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_FreeLibrary(%rip)
	leave
	ret
	.endfn	FreeLibrary,globl
	.previous
