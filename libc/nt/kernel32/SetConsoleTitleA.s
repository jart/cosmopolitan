.include "o/libc/nt/codegen.inc"
.imp	kernel32,__imp_SetConsoleTitleA,SetConsoleTitleA,0

	.text.windows
SetConsoleTitleA:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	mov	%rdi,%rcx
	sub	$32,%rsp
	call	*__imp_SetConsoleTitleA(%rip)
	leave
	ret
	.endfn	SetConsoleTitleA,globl
	.previous
